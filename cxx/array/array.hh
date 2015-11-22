// FIXME: Should writeable be part of the array?  Or use const?

#pragma once

#include <cassert>

namespace array {

using byte_t = unsigned char;
using index_t = long;

//------------------------------------------------------------------------------

namespace {

template<typename T>
T* 
shift(
  T* ptr,
  size_t offset)
{
  return reinterpret_cast<T*>(reinterpret_cast<byte_t*>(ptr) + offset);
}


template<typename T>
T const* 
shift(
  T const* ptr,
  size_t offset)
{
  return reinterpret_cast<T const*>(
    reinterpret_cast<byte_t const*>(ptr) + offset);
}


}  // anonymous namespace

//------------------------------------------------------------------------------

/**
 * An array has,
 * - one dimension
 * - fixed length
 * - fixed-sized elements
 * - fixed stride between elements
 */
class Array
{
public:

  virtual ~Array() {}

  /**
   * The number of elements in the array.
   */
  virtual index_t length() const = 0;

  /**
   * Size in bytes of each item.
   */
  virtual size_t item_size() const = 0;

  virtual bool is_writeable() const = 0; 

  // virtual void get(index_t idx, byte_t* dst) = 0;

  // virtual void set(byte_t* src, index_t idx) = 0;

  index_t 
  check(
    index_t idx)
    const
  {
    index_t const length = this->length();
    // FIXME: Don't assert; throw instead.
    if (idx < 0) {
      assert(-length <= idx);
      return length - idx;
    }
    else {
      assert(idx < length);
      return idx;
    }
  }

};


//------------------------------------------------------------------------------

template<size_t ITEM_SIZE>
class ContiguousArray
  : public Array
{
public:

  ContiguousArray(
    byte_t* buffer,
    index_t length,
    bool writeable)
  : buffer_(buffer),
    length_(length),
    writeable_(writeable)
  {
    assert(buffer != nullptr);
    assert(length >= 0);
  }

  virtual ~ContiguousArray() {}

  // FIXME: Separate virtual and nonvirtual accessors?

  virtual size_t item_size() const override { return ITEM_SIZE; }
  virtual index_t length() const override { return length_; }
  virtual bool is_writeable() const override { return writeable_; }

  size_t stride() const { return ITEM_SIZE; }
  byte_t* buffer() const { return buffer_; }

  // FIXME: ???
  size_t size() const { return ITEM_SIZE; }
  byte_t* begin() const { return buffer_; }
  byte_t* end() const { return buffer_ + ITEM_SIZE * length_; }

protected:

  inline byte_t const* 
  addr(
    index_t idx)
    const
  {
    return buffer_ + check(idx) * ITEM_SIZE;
  }

  inline byte_t*
  addr(
    index_t idx)
  {
    // FIXME: assert(writeable_) ?
    return buffer_ + check(idx) * ITEM_SIZE;
  }

  // FIXME: Should these be protected or private?  

  byte_t* const     buffer_;
  index_t const     length_;
  bool const        writeable_;

};


//------------------------------------------------------------------------------

class ArrayBuffer 
  : public Array
{
public:

  ArrayBuffer(
    byte_t* buffer, 
    size_t item_size,
    index_t length, 
    size_t stride,
    bool writeable)
  : buffer_(buffer),
    item_size_(item_size),
    length_(length),
    stride_(stride),
    writeable_(writeable)
  {
    assert(buffer != nullptr);
    assert(length >= 0);
    assert(stride >= 0);
  }

  /**
   * For a contiguous array, i.e. stride = item_size.
   */
  ArrayBuffer(
    byte_t* buffer,
    size_t item_size,
    index_t length,
    bool writeable)
  : ArrayBuffer(buffer, item_size, length, item_size, writeable)
  {
  }

  virtual ~ArrayBuffer() {}

  virtual index_t length() const override { return length_; }

  virtual size_t item_size() const override { return item_size_; }

  virtual bool is_writeable() const override { return writeable_; }

  void 
  get(
    index_t idx, 
    byte_t* dst)
  {
    memcpy(dst, addr(idx), item_size_);
  }

  void 
  set(
    byte_t* src, 
    index_t idx)
  {
    memcpy(addr(idx), src, item_size_);
  }

  /**
   * Offset in bytes between successive elements.
   */
  size_t stride() { return stride_; }

  // virtual byte_t const* buffer(index_t idx=0) = 0;
  // virtual byte_t* buffer_write(index_t idx=0) = 0;

protected:

  inline byte_t const* 
  addr(
    index_t idx)
    const
  {
    return buffer_ + check(idx) * stride_;
  }

  inline byte_t*
  addr(
    index_t idx)
  {
    // FIXME: assert(writeable_) ?
    return buffer_ + check(idx) * stride_;
  }

  // FIXME: Should these be protected or private?

  byte_t* const     buffer_;
  size_t const      item_size_;
  index_t const     length_;
  size_t const      stride_;
  bool const        writeable_;

};


//------------------------------------------------------------------------------

template<class BASE, typename T>
class TypedMixin
  : public BASE
{
public:

  using BASE::BASE;

  T*
  ptr()
  {
    // FIXME: Contiguous only?
    // FIXME: Check writeable.
    return reinterpret_cast<T*>(this->buffer());
  }

  T const* 
  ptr() 
    const
  {
    // FIXME: Contiguous only?
    return reinterpret_cast<T const*>(this->buffer());
  }

  inline T const&
  operator[](
    index_t idx)
    const
  {
    return *reinterpret_cast<T const*>(this->addr(idx));
  }

  inline T&
  operator[](
    index_t idx)
  {
    return *reinterpret_cast<T*>(this->addr(idx));
  }

  // Most logic should be in ContiguousArray::Iterator
  class Iterator 
  {
  public:

    Iterator(T* ptr, T* end) : ptr_(ptr), end_(end) {}

    bool operator==(Iterator const& other) { return other.ptr_ == ptr_; }
    bool operator!=(Iterator const& other) { return other.ptr_ != ptr_; }

    void operator++() { ptr_ = shift(ptr_, sizeof(T)); }

    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }

  private:

    T* ptr_;
    T* const end_;

  };

  T const* 
  end_ptr() 
    const 
  { 
    return shift(this->ptr(), this->stride() * this->length()); 
  }
  
  T* 
  end_ptr() 
  { 
    return shift(this->ptr(), this->stride() * this->length()); 
  }

  // FIXME: Move these to functions for ADL.
  Iterator begin() { return Iterator(ptr(), end_ptr()); }
  Iterator end() { return Iterator(end_ptr(), end_ptr()); }

};


template<typename T>
using TypedArray = TypedMixin<ContiguousArray<sizeof(T)>, T>;


//------------------------------------------------------------------------------

template<typename T>
class OwnedArrayBuffer
  : public TypedArray<T>
{
public:

  OwnedArrayBuffer(
    size_t length)
  : TypedArray<T>(
      new byte_t[sizeof(T) * length],
      length,
      true)
  {
  }

  virtual ~OwnedArrayBuffer() { delete[] this->buffer_; }

};


//------------------------------------------------------------------------------

/*

template<typename T>
class UniquePtrArrayBuffer
  : public TypedBuffer<T>
{
public:

  UniquePtrArrayBuffer(
    std::unique_ptr<T>&& ptr,
    size_t length,
    bool writeable)
  : TypedBuffer<T>(
      reinterpret_cast<byte_t*>(ptr.get()), 
      length,
      writeable),
    ptr_(std::move(ptr))
  {
  }

  virtual ~UniquePtrArrayBuffer() {}

private:

  std::unique_ptr<T> ptr_;

};

*/

//------------------------------------------------------------------------------

}  // namespace array

