#include <iostream>

#include "array/array.hh"
#include "array/typed.hh"

using namespace array;

//------------------------------------------------------------------------------

template<typename T>
std::ostream&
operator<<(
  std::ostream& os,
  typed::ContigArray<T> const& arr)
{
  os << '[';
  for (auto i : arr)
    os << i << ", ";
  os << ']';
  return os;
}


/* Uses SSE for 64-byte inner loop.  */

template<typename T>
void
fill_typed(
  typed::ContigArray<T>& arr,
  T val)
{
  for (auto& i : arr)
    i = val;
}

template void fill_typed<int>(typed::ContigArray<int>& arr, int);


#if 0

/* Produces a long, explicit unrolling.  */

template<typename T>
void
fill_contig(
  ContigArray<sizeof(T)>& arr,
  T val)
{
  size_t constexpr size = sizeof(T);
  for (byte_t* p = arr.begin(); p != arr.end(); p += size)
    *reinterpret_cast<T*>(p) = val;
}


// Explicit instantiation.
template void fill_contig<int>(ContigArray<sizeof(int)>&, int);

#endif


//------------------------------------------------------------------------------

int
main()
{
  typed::ContigArray<int>* buf = new OwnedArray<int>(8);
  fill_typed(*buf, 42);
  std::cout << *buf << "\n";
  delete buf;

  return 0;
}

