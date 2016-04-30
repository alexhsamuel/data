#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>

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


template<class T>
inline T*
index(
  T* const ptr,
  ptrdiff_t const stride,
  size_t const index)
{
  return (T*) ((byte*) ptr + stride * index);
}


//------------------------------------------------------------------------------

// FIXME: What should we call this?  Vector?  Seqence?  Span?

template<class T>
class Array
{
public:

  using const_pointer = T const*;
  using const_reference = T const&;
  using difference_type = ptrdiff_t;
  using pointer = T*;
  using reference = T&;
  using size_type = size_t;
  using value_type = T;

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

  class iterator;

  iterator begin() noexcept { return iterator(*this, 0); }
  iterator end() noexcept { return iterator(*this, length_); }

private:

  pointer           ptr_    = nullptr;
  size_type         length_ = 0;
  difference_type   stride_ = 0;

};


template<class T>
class Array<T>::iterator 
{
public:

  using difference_type = Array::difference_type;
  using pointer         = Array::pointer;
  using reference       = Array::reference;
  using value_type      = Array::value_type;

  using iterator_category = std::random_access_iterator_tag;

  iterator() = delete;  // FIXME: ??
  iterator(iterator const&) noexcept = default;
  iterator(iterator&&) noexcept = default;
  iterator& operator=(iterator const&) noexcept = default;
  iterator& operator=(iterator&&) noexcept = default;
  ~iterator() = default;

  iterator(Array& array, size_type const pos=0)
    : array_(array), pos_(pos) { assert(pos <= array.length()); }

  bool operator==(iterator const i) const noexcept
    { return same_array(i) && i.pos_ == pos_; }

  iterator&         operator++() noexcept 
    { ++pos_; return *this; }
  iterator          operator++(int) noexcept 
    { return iterator(array_, pos_++); }
  iterator&         operator--() noexcept 
    { --pos_; return *this; }
  iterator          operator--(int) noexcept 
    { return iterator(array_, pos_--); }
  iterator&         operator+=(size_type const o) noexcept 
    { pos_ += o; return *this; }
  iterator          operator+(size_type const o) const noexcept 
    { return iterator(array_, pos_ + o); }
  iterator&         operator-=(size_type const o) noexcept 
    { pos_ -= o; return *this; }
  iterator          operator-(size_type const o) const noexcept 
    { return iterator(array_, pos_ - o); }
  difference_type   operator-(iterator const i) const 
    { assert(same_array(i)); return pos_ - i.pos_; }

  reference operator*() const noexcept { return *index(array_.ptr(), array_.stride(), pos_); }
  pointer operator->() const noexcept { return index(array_.ptr(), array_.stride(), pos_); }
  reference operator[](size_type const pos) const noexcept { return *index(array_.ptr(), array_.stride(), pos + pos_); }  // FIXME: Check length?

private:

  bool same_array(iterator const i) const noexcept { return &i.array_ == &array_; }

  Array&    array_;
  size_type pos_;

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
  // FIXME: Use memset() or memset_pattern*() if appropriate?

  // if (arr.length() == 0)
  //   return;
  // auto ptr = arr.ptr();
  // for (size_t i = 0; i < arr.length(); ++i, ptr = advance(ptr, arr.stride()))
  //   *ptr = val;

  std::fill(arr.begin(), arr.end(), val);
}


template<>
__attribute((noinline)) 
inline void
fill<double>(
  Array<double>& arr,
  double const val)
{
  if (arr.stride() == sizeof(double))
    memset_pattern8(arr.ptr(), &val, arr.length() * sizeof(double));
  else
    std::fill(arr.begin(), arr.end(), val);
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
  if (N <= 16)
    std::cout << "arr0 = " << arr0 << "\n";
  fill(arr0, 10.0);
  if (N <= 16)
    std::cout << "arr0 = " << arr0 << "\n";
  fill(arr1, 42.0);
  if (N <= 16)
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
