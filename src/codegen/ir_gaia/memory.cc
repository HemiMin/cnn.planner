#include "codegen/ir_gaia/memory.h"

using codegen::gaia::Memory;

using std::endl;

ostream& codegen::gaia::operator<<(ostream& out, const Memory& mem)
{
  string ind = "  ";
  out << mem.GetName() << "(" << mem.GetSize() << ")" <<  endl;
  return out;
}
