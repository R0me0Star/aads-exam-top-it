#ifndef INTERACTIVE_HPP
#define INTERACTIVE_HPP

#include "../common/common.hpp"
#include <iostream>
#include <string>

namespace pozdnyakov
{

  struct Meet
  {
    size_t id1;
    size_t id2;
    size_t duration;
  };

  struct AppArgs2
  {
    std::string inFilename;
    std::string dataFilename;
    bool hasIn;
    bool hasData;
    bool isError;
  };

  AppArgs2 parseArguments2(const int argc, char *argv[]);

  size_t findPersonIdx(const DynamicArray< Person > &arr, const size_t id);

  bool personExists(const DynamicArray< Person > &arr, const size_t id);

  bool hasDescription(const DynamicArray< Person > &arr, const size_t id);

  void ensurePersonExists(DynamicArray< Person > &arr, const size_t id);

  bool readMeets(std::istream &in, DynamicArray< Meet > &meets);

  void sortMeets(DynamicArray< Meet > &meets);

  void cmdAnons(std::ostream &out, const DynamicArray< Person > &persons);

  bool cmdDeanon(
    DynamicArray< Person > &persons,
    DynamicArray< Meet > &meets,
    const size_t anonId,
    const size_t id
  );

  bool cmdRedesc(
    DynamicArray< Person > &persons,
    const size_t id,
    const std::string &desc
  );

  bool cmdDesc(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const size_t id
  );

  bool cmdMeets(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const DynamicArray< Meet > &meets,
    const size_t id
  );

  bool cmdCommons(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const DynamicArray< Meet > &meets,
    const size_t id1,
    const size_t id2
  );

  bool cmdLess(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const DynamicArray< Meet > &meets,
    const size_t time,
    const size_t id
  );

  bool cmdGreater(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const DynamicArray< Meet > &meets,
    const size_t time,
    const size_t id
  );

  bool cmdOutPersons(
    const DynamicArray< Person > &persons,
    const std::string &filename
  );

  void runInteractive(
    std::istream &in,
    std::ostream &out,
    DynamicArray< Person > &persons,
    DynamicArray< Meet > &meets
  );

}

#endif
