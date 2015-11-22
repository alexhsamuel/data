#pragma once

#include "array.hh"

//------------------------------------------------------------------------------

namespace array {

/**
 * Array backed by a byte buffer containing contiguous items.
 */
template<size_t ITEM_SIZE>
class ContigArray
  : public Array
{
public:

  ContigArray(
    byte_t* buffer,
    index_t length)
  : buffer_(buffer),
    length_(length)
  {
    assert(buffer != nullptr);
    assert(length >= 0);
  }

  virtual ~ContigArray() {}

  virtual size_t 
  get_item_size() 
    const override 
  { 
    return ITEM_SIZE; 
  }

  virtual index_t 
  get_length() 
    const override 
  { 
    return length_; 
  }

  byte_t*       buffer()          { return buffer_; }
  byte_t const* buffer()    const { return buffer_; }
  size_t        item_size() const { return ITEM_SIZE; }
  index_t       length()    const { return length_; }
  byte_t*       begin_ptr() const { return buffer_; }
  byte_t*       end_ptr()   const { return buffer_ + ITEM_SIZE * length_; }

  class ConstIterator 
  {
  public:

    ConstIterator(
      byte_t const* ptr) 
    : ptr_(ptr) 
    {
    }

    bool operator==(ConstIterator const& other) { return other.ptr_ == ptr_; }
    bool operator!=(ConstIterator const& other) { return other.ptr_ != ptr_; }

    void 
    operator++() 
    { 
      ptr_ += ITEM_SIZE;
    }

    byte_t const& 
    operator*() 
      const 
    { 
      return *ptr_; 
    }

    byte_t const* 
    operator->() 
      const 
    { 
      return ptr_; 
    }

  protected:

    byte_t const* ptr_;

  };

  class Iterator 
  {
  public:

    Iterator(
      byte_t* ptr) 
    : ptr_(ptr) 
    {
    }

    bool operator==(Iterator const& other) { return other.ptr_ == ptr_; }
    bool operator!=(Iterator const& other) { return other.ptr_ != ptr_; }

    void 
    operator++() 
    { 
      ptr_ += ITEM_SIZE;
    }

    byte_t& 
    operator*() 
      const 
    { 
      return *ptr_; 
    }

    byte_t* 
    operator->() 
      const 
    { 
      return ptr_; 
    }

  protected:

    byte_t* ptr_;

  };

protected:

  inline byte_t const* 
  addr(
    index_t idx)
    const
  {
    return buffer_ + check_index(idx, length_) * ITEM_SIZE;
  }

  inline byte_t*
  addr(
    index_t idx)
  {
    return buffer_ + check_index(idx, length_) * ITEM_SIZE;
  }

  // FIXME: Should these be protected or private?  

  byte_t* const     buffer_;
  index_t const     length_;

};

//------------------------------------------------------------------------------

}  // namespace array


