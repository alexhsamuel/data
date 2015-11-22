#pragma once

namespace array {

using byte_t = unsigned char;
using index_t = long;

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
  virtual index_t length() = 0;

  /**
   * Size in bytes of each item.
   */
  virtual size_t item_size() = 0;

  virtual bool is_writable() = 0;

  virtual void get(index_t idx, byte* dst) = 0;
  virtual void set(byte* src, index_t idx) = 0;

  index_t check(index_t idx) 
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


class ArrayBuffer final
  : public Array
{
public:

  /**
   * Offset in bytes between successive elements.
   */
  size_t stride() { return stride_; }

  virtual byte_t const* buffer(index_t idx=0) = 0;
  virtual byte_t* buffer_write(index_t idx=0) = 0;

private:

  index_t length_;
  bool writeable_;
  byte_t* buffer_;
  size_t stride_;

};


}  // namespace array

