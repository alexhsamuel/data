#include <iostream>

#include "array/array.hh"

using namespace array;

template<typename T>
std::ostream&
operator<<(
  std::ostream& os,
  TypedBuffer<T> const& arr)
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
  TypedBuffer<T>& arr,
  T val)
{
  for (index_t i = 0; i < arr.length(); ++i)
    arr[i] = val;
}


template<typename T>
void
fill_raw(
  TypedBuffer<T>& arr,
  T val)
{
  T* ptr = arr.ptr();
  T* end = ptr + arr.length();
  for (; ptr < end; ++ptr)
    *ptr = val;
}


template<typename T>
void
fill(
  TypedBuffer<T>& arr,
  T val)
{
  for (auto& i : arr)
    i = val;
}


// Explicit instantiation.
template void fill<int>(TypedBuffer<int>&, int);


int
main()
{
  TypedBuffer<int>* buf = new OwnedArrayBuffer<int>(8);
  fill(*buf, 42);

  std::cout << *buf << "\n";

  delete buf;

  return 0;
}

