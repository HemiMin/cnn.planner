#ifndef CNNPLANNER_PARAMETER_COMPILER_PARSER_H_
#define CNNPLANNER_PARAMETER_COMPILER_PARSER_H_

#include <getopt.h>

#include "parameter/compiler_parameter.h"

using parameter::CompilerParameter;

namespace parameter {
const struct option c_options[] { // compiler options
  {"stride",          1, 0, 0},
  {"iw",              1, 0, 0},
  {"ih",              1, 0, 0},
  {"ic",              1, 0, 0},
  {"pw",              1, 0, 0},
  {"ph",              1, 0, 0},
  {"kw",              1, 0, 0},
  {"kh",              1, 0, 0},
  {"oc",              1, 0, 0},
  {"mac-cycles",      1, 0, 0},
  {"frequency",       1, 0, 0},
  {"bandwidth",       1, 0, 0},
  {"input-mem-size",  1, 0, 0},
  {"weight-mem-size", 1, 0, 0},
  {"output-mem-size", 1, 0, 0},
  {"pe-dim",          1, 0, 0},
  {"pe-structure",    1, 0, 0},
  {"code-path",       1, 0, 0},
  {"gaia-path",       1, 0, 0},
  {"latency-path",    1, 0, 0},
  {"timestamp-path",  1, 0, 0},
  {"tiling-dump",     1, 0, 0},
  {"loop-seq-dump",   1, 0, 0},
  {"layer",           1, 0, 0},
  {"help",            0, 0, 0},
  {0, 0, 0, 0} // terminate
};
////////////////////////////////////////////////////////////////////////////////
//! @brief    Command line arguments parser for compiler.
//! @author   Minsu Kim
//! @date     2019-11-26
////////////////////////////////////////////////////////////////////////////////
class CompilerParser
{
  public:
    //! @brief    Parse and check parameter from argv.
    //! @return   CompilerParameter object which includes arguments.
    CompilerParameter* BuildParameter(int argc, char** argv);

  private:
    CompilerParameter* Parsing(int argc, char** argv);
    void ParseLongOptions(int opt_index,char* exe_cmd,CompilerParameter* param);
    void CheckParameterValid(const CompilerParameter& param) const;

    void PrintHelp(char* exe_cmd) const;
};
} // namespace parameter

#endif