#include "codegen/ir_gaia/load.h"

using std::endl;

using codegen::gaia::Load;

ostream& codegen::gaia::operator<<(ostream& out, const Load& load)
{
  out << "LOAD    "   << load.GetMemory().GetName() << "  "
                      << load.GetOperand().GetName() << "  "
                      << load.GetStartAddress() << "  "
                      << load.GetEndAddress() << endl;
  return out;
}