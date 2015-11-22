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


index_t 
check_index(
  index_t idx,
  index_t length)
{
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


}  // anonymous namespace

//------------------------------------------------------------------------------

/**
 * An array has,
 * - one dimension
 * - fixed length
 * - fixed-sized elements
 */
class Array
{
public:

  virtual ~Array() {}

  /**
   * The number of elements in the array.
   */
  virtual index_t get_length() const = 0;

  /**
   * Size in bytes of each item.
   */
  virtual size_t get_item_size() const = 0;

};


#if 0

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

#endif

}  // namespace array

