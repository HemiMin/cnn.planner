#pragma once

#include "loop/cnn_loop.h"
#include "arch/architecture.h"

using loop::CnnLoop;
using arch::Architecture;

namespace codegen {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Source code generator.
//! @details  Generate target source code generator.
//! @author   Minsu Kim
//! @date     2020-01-06
////////////////////////////////////////////////////////////////////////////////
class CodeGenerator
{
  public:
    virtual ~CodeGenerator() {}
    //! @brief        Generate source code
    //! @param loop   CNN loop information.
    //! @param arch   Architecture information.
    virtual void GenCode(const CnnLoop&, const Architecture& arch) = 0;
};
} // namespace codegen