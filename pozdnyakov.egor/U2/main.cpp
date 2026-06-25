#include <fstream>
#include <iostream>
#include "interactive.hpp"

int main(const int argc, char *argv[])
{
  if (argc > 3) {
    std::cerr << "Too many arguments\n";
    return 0;
  }

  const pozdnyakov::AppArgs2 args = pozdnyakov::parseArguments2(argc, argv);
  if (args.isError) {
    std::cerr << "Invalid arguments\n";
    return 1;
  }

  pozdnyakov::DynamicArray< pozdnyakov::Person > persons;
  pozdnyakov::initArray(persons);

  if (args.hasIn) {
    std::ifstream inFile(args.inFilename);
    if (!inFile.is_open()) {
      pozdnyakov::destroyArray(persons);
      std::cerr << "Cannot open file: " << args.inFilename << "\n";
      return 2;
    }
    pozdnyakov::readData(inFile, persons);
    inFile.close();
  }

  pozdnyakov::DynamicArray< pozdnyakov::Meet > meets;
  pozdnyakov::initArray(meets);

  {
    std::ifstream dataFile(args.dataFilename);
    if (!dataFile.is_open()) {
      pozdnyakov::destroyArray(persons);
      pozdnyakov::destroyArray(meets);
      std::cerr << "Cannot open file: " << args.dataFilename << "\n";
      return 2;
    }

    const bool meetsOk = pozdnyakov::readMeets(dataFile, meets);
    dataFile.close();

    if (!meetsOk) {
      pozdnyakov::destroyArray(persons);
      pozdnyakov::destroyArray(meets);
      std::cerr << "Invalid meets data\n";
      return 3;
    }
  }

  for (size_t i = 0; i < meets.size; ++i) {
    pozdnyakov::ensurePersonExists(persons, meets.data[i].id1);
    pozdnyakov::ensurePersonExists(persons, meets.data[i].id2);
  }

  pozdnyakov::runInteractive(std::cin, std::cout, persons, meets);

  pozdnyakov::destroyArray(persons);
  pozdnyakov::destroyArray(meets);

  return 0;
}
