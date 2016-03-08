#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>

//------------------------------------------------------------------------------

// FIXME: What are semantics for size == 0?

class Buffer
{
public:

  virtual ~Buffer() {}

  virtual void* get_start() const = 0;
  virtual size_t get_size() const = 0;

};


//------------------------------------------------------------------------------

class MallocBuffer
: public Buffer
{
public:

  MallocBuffer(size_t size=0);
  MallocBuffer(MallocBuffer&&);
  MallocBuffer(MallocBuffer const&) = delete;

  virtual ~MallocBuffer();

  MallocBuffer& operator=(MallocBuffer&&);
  MallocBuffer& operator=(MallocBuffer const&) = delete;

  virtual void* get_start() const { return start_; }
  virtual size_t get_size() const { return size_; }

private:

  size_t size_;
  void* start_;

};


inline
MallocBuffer::MallocBuffer(
  size_t const size)
: size_(size),
  start_(size == 0 ? nullptr : malloc(size))
{
  assert(size_ == 0 || start_ != nullptr);
}


inline
MallocBuffer::MallocBuffer(
  MallocBuffer&& buffer)
: size_(buffer.size_),
  start_(buffer.start_)
{
  buffer.size_ = 0;
  buffer.start_ = nullptr;
}

inline
MallocBuffer::~MallocBuffer()
{
  if (start_ != nullptr)
    free(start_);
}


inline MallocBuffer&
MallocBuffer::operator=(
  MallocBuffer&& buffer)
{
  if (start_ != nullptr)
    free(start_);
  start_ = buffer.start_;
  size_ = buffer.size_;
  buffer.start_ = nullptr;
  buffer.size_ = 0;
  return *this;
}


