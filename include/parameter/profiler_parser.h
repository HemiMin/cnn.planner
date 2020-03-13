#ifndef CNNPLANNER_PARAMETER_PROFILER_PARSER_H_
#define CNNPLANNER_PARAMETER_PROFILER_PARSER_H_

#include <getopt.h>

#include "parameter/profiler_parameter.h"

using parameter::ProfilerParameter;

namespace parameter {
const struct option p_options[] { // profiler options
  {"stride",            1, 0, 0},
  {"iw",                1, 0, 0},
  {"ih",                1, 0, 0},
  {"ic",                1, 0, 0},
  {"pw",                1, 0, 0},
  {"ph",                1, 0, 0},
  {"ow",                1, 0, 0},
  {"oh",                1, 0, 0},
  {"kw",                1, 0, 0},
  {"kh",                1, 0, 0},
  {"oc",                1, 0, 0},
  {"mac-cycles",        1, 0, 0},
  {"frequency",         1, 0, 0},
  {"bandwidth",         1, 0, 0},
  {"mac-energy",        1, 0, 0},
  {"on-chip-32-energy", 1, 0, 0},
  {"off-chip-32-energy",1, 0, 0},
  {"input-mem-size",    1, 0, 0},
  {"weight-mem-size",   1, 0, 0},
  {"output-mem-size",   1, 0, 0},
  {"pe-dim",            1, 0, 0},
  {"pe-structure",      1, 0, 0},
  {"latency-path",      1, 0, 0},
  {"tiling-dump",       1, 0, 0},
  {"loop-seq-dump",     1, 0, 0},
  {"report-path",       1, 0, 0},
  {"layer",             1, 0, 0},
  {"help",              0, 0, 0},
  {0, 0, 0, 0} // terminate
};
////////////////////////////////////////////////////////////////////////////////
//! @brief    Command line arguments parser for profiler.
//! @author   Minsu Kim
//! @date     2019-11-26
////////////////////////////////////////////////////////////////////////////////
class ProfilerParser
{
  public:
    //! @brief    Parse and check parameter from argv.
    //! @return   ProfilerParser object which includes arguments.
    ProfilerParameter* BuildParameter(int argc, char** argv);

  private:
    ProfilerParameter* Parsing(int argc, char** argv);
    void ParseLongOptions(int opt_index,char* exe_cmd,ProfilerParameter* param);
    void CheckParameterValid(const ProfilerParameter& param) const;

    void PrintHelp(char* exe_cmd) const;
};
} // namespace parameter

#endif