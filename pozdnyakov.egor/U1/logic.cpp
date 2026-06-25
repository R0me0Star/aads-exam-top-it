#include "logic.hpp"
#include <cctype>

namespace pozdnyakov
{

  AppArgs parseArguments(const int argc, char *argv[])
  {
    AppArgs args;
    args.hasIn = false;
    args.hasOut = false;
    args.isError = false;

    for (int i = 1; i < argc; ++i) {
      const std::string argStr = argv[i];
      if (argStr.find("in:") == 0) {
        if (args.hasIn) {
          args.isError = true;
          return args;
        }
        args.inFilename = argStr.substr(3);
        args.hasIn = true;
      } else if (argStr.find("out:") == 0) {
        if (args.hasOut) {
          args.isError = true;
          return args;
        }
        args.outFilename = argStr.substr(4);
        args.hasOut = true;
      } else {
        args.isError = true;
        return args;
      }
    }

    return args;
  }

  bool containsId(const DynamicArray< Person > &arr, const size_t id)
  {
    for (size_t i = 0; i < arr.size; ++i) {
      if (arr.data[i].id == id) {
        return true;
      }
    }
    return false;
  }

  ParseResult parseLine(const std::string &line)
  {
    ParseResult result;
    result.success = false;

    size_t pos = 0;
    const size_t len = line.length();

    while (pos < len && std::isspace(static_cast< unsigned char >(line[pos]))) {
      ++pos;
    }

    if (pos == len) {
      return result;
    }

    size_t id = 0;
    bool hasDigits = false;
    while (pos < len && std::isdigit(static_cast< unsigned char >(line[pos]))) {
      id = id * 10 + static_cast< size_t >(line[pos] - '0');
      ++pos;
      hasDigits = true;
    }

    if (!hasDigits) {
      return result;
    }

    while (pos < len && std::isspace(static_cast< unsigned char >(line[pos]))) {
      ++pos;
    }

    if (pos == len) {
      return result;
    }

    result.person.id = id;
    result.person.info = line.substr(pos);
    result.success = true;

    return result;
  }

  ProcessStats readData(std::istream &in, DynamicArray< Person > &arr)
  {
    ProcessStats stats;
    stats.successCount = 0;
    stats.ignoredCount = 0;

    std::string line;
    while (std::getline(in, line)) {
      if (!line.empty() && line[line.length() - 1] == '\r') {
        line.erase(line.length() - 1);
      }

      bool isBlank = true;
      for (size_t i = 0; i < line.length(); ++i) {
        if (!std::isspace(static_cast< unsigned char >(line[i]))) {
          isBlank = false;
          break;
        }
      }

      if (isBlank) {
        continue;
      }

      const ParseResult parsed = parseLine(line);
      if (parsed.success) {
        if (parsed.person.info.empty()) {
          ++stats.ignoredCount;
        } else if (containsId(arr, parsed.person.id)) {
          ++stats.ignoredCount;
        } else {
          pushBack(arr, parsed.person);
          ++stats.successCount;
        }
      } else {
        ++stats.ignoredCount;
      }
    }

    return stats;
  }

  void writeData(std::ostream &out, const DynamicArray< Person > &arr)
  {
    if (arr.size == 0) {
      out << "\n";
      return;
    }
    for (size_t i = 0; i < arr.size; ++i) {
      out << arr.data[i].id << " " << arr.data[i].info << "\n";
    }
  }

}
