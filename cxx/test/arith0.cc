#include <iostream>

#include "array/array.hh"

using namespace array;

template<typename T>
std::ostream&
operator<<(
  std::ostream& os,
  TypedArray<T> const& arr)
{
  os << '[';
  for (index_t i = 0; i < arr.length(); ++i)
    os << arr[i] << ", ";
  os << ']';
  return os;
}


template<typename T>
void
fill_checked(
  TypedArray<T>& arr,
  T val)
{
  for (index_t i = 0; i < arr.length(); ++i)
    arr[i] = val;
}


template<typename T>
void
fill_raw(
  TypedArray<T>& arr,
  T val)
{
  T* ptr = arr.ptr();
  T* end = ptr + arr.length();
  for (; ptr < end; ++ptr)
    *ptr = val;
}


/* Uses SSE for 64-byte inner loop.  */

template<typename T>
void
fill_typed(
  TypedArray<T>& arr,
  T val)
{
  for (auto& i : arr)
    i = val;
}

template void fill_typed<int>(TypedArray<int>& arr, int);


/* Produces a long, explicit unrolling.  */

template<typename T>
void
fill_contig(
  ContiguousArray<sizeof(T)>& arr,
  T val)
{
  size_t constexpr size = sizeof(T);
  for (byte_t* p = arr.begin(); p != arr.end(); p += size)
    *reinterpret_cast<T*>(p) = val;
}


// Explicit instantiation.
template void fill_contig<int>(ContiguousArray<sizeof(int)>&, int);


int
main()
{
  TypedArray<int>* buf = new OwnedArrayBuffer<int>(8);
  fill_typed(*buf, 42);

  std::cout << *buf << "\n";

  delete buf;

  return 0;
}

