#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "../common/common.hpp"
#include <string>

namespace pozdnyakov
{

  struct AppArgs
  {
    std::string inFilename;
    std::string outFilename;
    bool hasIn;
    bool hasOut;
    bool isError;
  };

  AppArgs parseArguments(const int argc, char *argv[]);

}

#endif
