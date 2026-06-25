#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <cstddef>
#include <iostream>
#include <string>

namespace pozdnyakov
{

  struct Person
  {
    size_t id;
    std::string info;
  };

  template< typename T >
  struct DynamicArray
  {
    T *data;
    size_t size;
    size_t capacity;
  };

  template< typename T >
  void initArray(DynamicArray< T > &arr)
  {
    arr.data = nullptr;
    arr.size = 0;
    arr.capacity = 0;
  }

  template< typename T >
  void destroyArray(DynamicArray< T > &arr)
  {
    delete[] arr.data;
    arr.data = nullptr;
    arr.size = 0;
    arr.capacity = 0;
  }

  template< typename T >
  void pushBack(DynamicArray< T > &arr, const T &item)
  {
    if (arr.size == arr.capacity) {
      size_t newCapacity = 0;
      if (arr.capacity == 0) {
        newCapacity = 1;
      } else {
        newCapacity = arr.capacity * 2;
      }

      T *newData = new T[newCapacity];
      for (size_t i = 0; i < arr.size; ++i) {
        newData[i] = arr.data[i];
      }

      delete[] arr.data;
      arr.data = newData;
      arr.capacity = newCapacity;
    }
    arr.data[arr.size] = item;
    ++arr.size;
  }

  struct AppArgs
  {
    std::string inFilename;
    std::string outFilename;
    bool hasIn;
    bool hasOut;
    bool isError;
  };

  AppArgs parseArguments(const int argc, char *argv[]);

  bool containsId(const DynamicArray< Person > &arr, const size_t id);

  struct ParseResult
  {
    Person person;
    bool success;
  };

  ParseResult parseLine(const std::string &line);

  struct ProcessStats
  {
    size_t successCount;
    size_t ignoredCount;
  };

  ProcessStats readData(std::istream &in, DynamicArray< Person > &arr);

  void writeData(std::ostream &out, const DynamicArray< Person > &arr);

}

#endif
