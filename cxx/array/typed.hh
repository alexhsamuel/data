#pragma once

#include "array.hh"
#include "contig.hh"

//------------------------------------------------------------------------------

namespace array {

template<class BASE, typename T>
class TypedMixin
  : public BASE
{
public:

  using BASE::BASE;

  T*
  begin_ptr() 
    const 
  { 
    return reinterpret_cast<T*>(this->BASE::begin_ptr()); 
  }

  T* 
  end_ptr() 
    const 
  { 
    return reinterpret_cast<T*>(this->BASE::end_ptr());
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

  class ConstIterator 
    : public BASE::ConstIterator
  {
  public:

    ConstIterator(
      T const* ptr)
    : BASE::ConstIterator(reinterpret_cast<byte_t const*>(ptr))
    {
    }

    T const& 
    operator*() 
      const 
    { 
      return *reinterpret_cast<T const*>(this->ptr_); 
    }

    T const* 
    operator->() 
      const 
    { 
      return reinterpret_cast<T const*>(this->ptr_); 
    }

  };

  class Iterator 
    : public BASE::Iterator
  {
  public:

    Iterator(
      T* ptr)
    : BASE::Iterator(reinterpret_cast<byte_t*>(ptr))
    {
    }

    T& 
    operator*() 
      const 
    { 
      return *reinterpret_cast<T*>(this->ptr_); 
    }

    T* 
    operator->() 
      const 
    { 
      return reinterpret_cast<T*>(this->ptr_); 
    }

  };

};


template<class BASE, typename T>
typename TypedMixin<BASE, T>::Iterator 
begin(
  TypedMixin<BASE, T>& typed) 
{ 
  return typename TypedMixin<BASE, T>::Iterator(typed.begin_ptr()); 
}

template<class BASE, typename T>
typename TypedMixin<BASE, T>::ConstIterator 
begin(
  TypedMixin<BASE, T> const& typed) 
{ 
  return typename TypedMixin<BASE, T>::ConstIterator(typed.begin_ptr()); 
}

template<class BASE, typename T>
typename TypedMixin<BASE, T>::Iterator 
end(
  TypedMixin<BASE, T>& typed) 
{ 
  return typename TypedMixin<BASE, T>::Iterator(typed.end_ptr());
}

template<class BASE, typename T>
typename TypedMixin<BASE, T>::ConstIterator 
end(
  TypedMixin<BASE, T> const& typed) 
{ 
  return typename TypedMixin<BASE, T>::ConstIterator(typed.end_ptr()); 
}


//------------------------------------------------------------------------------

template<typename T>
using 
TypedContigArray 
  = TypedMixin<ContigArray<sizeof(T)>, T>;


//------------------------------------------------------------------------------

// FIXME: Rename!

template<typename T>
class OwnedArray
  : public TypedContigArray<T>
{
public:

  OwnedArray(
    size_t length)
  : TypedContigArray<T>(
      new byte_t[sizeof(T) * length],
      length)
  {
  }

  virtual ~OwnedArray() { delete[] this->buffer_; }

};


//------------------------------------------------------------------------------

}  // namespace array


