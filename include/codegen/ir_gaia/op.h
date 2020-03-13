#pragma once

namespace codegen {
namespace gaia {
class Operator 
{
  public:
    Operator(void) {}
    virtual ~Operator(void) {}

    virtual bool Validate(void) const = 0;
};
} // namespace gaia
} // namespace codegen
