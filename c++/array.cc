#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>

using std::ptrdiff_t;
using std::size_t;

//------------------------------------------------------------------------------

using byte = uint8_t;

template<class T>
inline T*
advance(
  T* const ptr,
  ptrdiff_t const stride)
{
  return (T*) ((byte*) ptr + stride);
}

template<class T>
inline T const*
advance(
  T const* const ptr,
  ptrdiff_t const stride)
{
  return (T const*) ((byte const*) ptr + stride);
}


//------------------------------------------------------------------------------

// FIXME: What should we call this?  Vector?  Seqence?  Span?

template<class T>
class Array
{
public:

  using value_type = T;
  using pointer = T*;
  using const_pointer = T const*;

  Array() noexcept                          = default;
  Array(Array const&) noexcept              = default;
  Array(Array&&) noexcept                   = default;
  Array& operator=(Array const&) noexcept   = default;
  Array& operator=(Array&&) noexcept        = default;
  ~Array() noexcept                         = default;

  Array(
    pointer const ptr, 
    size_t const length, 
    ptrdiff_t const stride=sizeof(T))
    noexcept
  : ptr_(ptr),
    length_(length),
    stride_(stride)
  {
  }

  pointer ptr() noexcept { assert(ptr_ != nullptr); return ptr_; }
  const_pointer ptr() const noexcept { assert(ptr_ != nullptr); return ptr_; }
  size_t length() const noexcept { return length_; }
  ptrdiff_t stride() const noexcept { return stride_; }

  // FIXME: Iterator.

private:

  pointer   ptr_    = nullptr;
  size_t    length_ = 0;
  ptrdiff_t stride_ = 0;

};


// FIXME: ContiguousArray, with stride=0.


//------------------------------------------------------------------------------

template<class T>
inline Array<T>
alloc(
  size_t const length)
{
  // FIXME: Leak.
  return Array<T>(new T[length], length);
}


template<class T>
inline void
fill(
  Array<T>& arr,
  T const val)
{
  // FIXME: Use std::fill().
  // FIXME: Use memset() or memset_pattern*() if appropriate?
  if (arr.length() == 0)
    return;
  auto ptr = arr.ptr();
  for (size_t i = 0; i < arr.length(); ++i, ptr = advance(ptr, arr.stride()))
    *ptr = val;
}


template<class T>
inline T
sum(
  Array<T> const& arr,
  T const init={})
{
  if (arr.length() == 0)
    return init;
  auto sum = init;
  auto ptr = arr.ptr();
  for (size_t i = 0; i < arr.length(); ++i, ptr = advance(ptr, arr.stride()))
    sum += *ptr;
  return sum;
}


template<class T>
inline T
dot(
  Array<T> const& arr0,
  Array<T> const& arr1,
  T const init={})
{
  auto length = arr0.length();
  assert(arr1.length() == length);  // ??
  if (length == 0)
    return init;

  auto dot = init;
  auto ptr0 = arr0.ptr();
  auto ptr1 = arr1.ptr();
  for (size_t i = 0;
       i < length;
       i++, 
         ptr0 = advance(ptr0, arr0.stride()), 
         ptr1 = advance(ptr1, arr1.stride()))
    dot += *ptr0 * *ptr1;
  return dot;
}


template<class T>
inline std::ostream&
operator<<(
  std::ostream& os,
  Array<T> const& arr)
{
  os << '[';
  auto ptr = arr.ptr();
  auto length = arr.length();
  for (size_t i = 0; i < length; ++i, ptr = advance(ptr, arr.stride())) {
    if (i > 0)
      os << ',' << ' ';
    os << *ptr;
  }
  os << ']';
  return os;
}  


//------------------------------------------------------------------------------




//------------------------------------------------------------------------------

int
main(
  int const argc,
  char const* const* argv)
{
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " N\n";
    exit(1);
  }

  size_t const N = atol(argv[1]);
  Array<double> arr0 = alloc<double>(N);
  Array<double> arr1 = alloc<double>(N);
  std::cout << "arr0 = " << arr0 << "\n";
  fill(arr0, 10.0);
  std::cout << "arr0 = " << arr0 << "\n";
  fill(arr1, 42.0);
  std::cout << "arr1 = " << arr1 << "\n";
  std::cout << "sum(arr0) == " << N * 10.0 << " -> " << sum(arr0) << "\n";
  std::cout << "sum(arr1) == " << N * 42.0 << " -> " << sum(arr1) << "\n";
  std::cout << "dot(arr0, arrr1) == " << N * 10.0 * 42.0
            << " -> " << dot(arr0, arr1) << "\n";
  return 0;
}


// Explicit instantiations.
template void fill<double>(Array<double>&, double);
template double dot<double>(Array<double> const&, Array<double> const&, double);
