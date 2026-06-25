#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <cstddef>

namespace matveev
{
  template< typename T >
  struct Array
  {
    T *data;
    size_t size;
    size_t capacity;

    Array();
    Array(const Array< T > &other);
    ~Array();
    Array< T > &operator=(const Array< T > &other);
  };

  template< typename T >
  void swap(Array< T > &lhs, Array< T > &rhs)
  {
    T *tmpData = lhs.data;
    lhs.data = rhs.data;
    rhs.data = tmpData;
    size_t tmpSize = lhs.size;
    lhs.size = rhs.size;
    rhs.size = tmpSize;
    size_t tmpCapacity = lhs.capacity;
    lhs.capacity = rhs.capacity;
    rhs.capacity = tmpCapacity;
  }

  template< typename T >
  Array< T >::Array():
    data(nullptr),
    size(0),
    capacity(0)
  {}

  template< typename T >
  Array< T >::Array(const Array< T > &other):
    data(other.capacity == 0 ? nullptr : new T[other.capacity]),
    size(other.size),
    capacity(other.capacity)
  {
    try
    {
      for (size_t i = 0; i < other.size; ++i)
      {
        data[i] = other.data[i];
      }
    }
    catch (...)
    {
      delete[] data;
      throw;
    }
  }

  template< typename T >
  Array< T >::~Array()
  {
    delete[] data;
  }

  template< typename T >
  Array< T > &Array< T >::operator=(const Array< T > &other)
  {
    Array< T > copy(other);
    swap(*this, copy);
    return *this;
  }

  template< typename T >
  void pushBack(Array< T > &array, const T &value)
  {
    if (array.size == array.capacity)
    {
      size_t newCapacity = array.capacity == 0 ? 1 : array.capacity * 2;
      T *newData = new T[newCapacity];
      try
      {
        for (size_t i = 0; i < array.size; ++i)
        {
          newData[i] = array.data[i];
        }
      }
      catch (...)
      {
        delete[] newData;
        throw;
      }
      delete[] array.data;
      array.data = newData;
      array.capacity = newCapacity;
    }
    array.data[array.size] = value;
    ++array.size;
  }
}

#endif
