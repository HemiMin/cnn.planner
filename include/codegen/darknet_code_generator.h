#pragma once

#include <string.h>
#include <glog/logging.h>

#include "codegen/code_generator.h"
#include "loop/cnn_loop.h"
#include "arch/architecture.h"

#define DIR_LEN 64

using loop::CnnLoop;
using arch::Architecture;

namespace codegen {
namespace darknet {
////////////////////////////////////////////////////////////////////////////////
//! @brief      DarkNet source code generator.
//! @details    Generate DarkNet C code to run specific layer.
//! @author     Minsu Kim
//! @date       2020-01-06
////////////////////////////////////////////////////////////////////////////////
class DarknetCodeGenerator : public codegen::CodeGenerator
{
  public:
    DarknetCodeGenerator(const char* darknet_dir)
    {
      strncpy(darknet_dir_, darknet_dir, DIR_LEN);
    }
    ~DarknetCodeGenerator() {}

    void GenCode(const CnnLoop& loop, const Architecture& arch);

  private:
    char darknet_dir_[DIR_LEN];
};
} // namespace darknet
} // namespace codegen