#include <fstream>
#include <iostream>
#include "logic.hpp"

int main(const int argc, char *argv[])
{
  if (argc > 3) {
    std::cerr << "Too many arguments\n";
    return 0;
  }

  const pozdnyakov::AppArgs args = pozdnyakov::parseArguments(argc, argv);
  if (args.isError) {
    std::cerr << "Invalid arguments\n";
    return 1;
  }

  pozdnyakov::DynamicArray< pozdnyakov::Person > persons;
  pozdnyakov::initArray(persons);

  pozdnyakov::ProcessStats stats;
  stats.successCount = 0;
  stats.ignoredCount = 0;

  if (args.hasIn) {
    std::ifstream inFile(args.inFilename);
    if (!inFile.is_open()) {
      pozdnyakov::destroyArray(persons);
      return 2;
    }
    stats = pozdnyakov::readData(inFile, persons);
    inFile.close();
  } else {
    stats = pozdnyakov::readData(std::cin, persons);
  }

  if (args.hasOut) {
    std::ofstream outFile(args.outFilename);
    if (!outFile.is_open()) {
      pozdnyakov::destroyArray(persons);
      return 2;
    }
    pozdnyakov::writeData(outFile, persons);
    outFile.close();
  } else {
    pozdnyakov::writeData(std::cout, persons);
  }

  std::cerr << stats.successCount << " " << stats.ignoredCount << "\n";

  pozdnyakov::destroyArray(persons);

  return 0;
}
