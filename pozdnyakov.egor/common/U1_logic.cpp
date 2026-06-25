#include "logic.hpp"

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

}
