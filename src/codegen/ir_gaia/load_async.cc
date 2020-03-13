#include "codegen/ir_gaia/load_async.h"

using std::endl;

using codegen::gaia::AsyncLoad;

ostream& codegen::gaia::operator<<(ostream& out, const AsyncLoad& async_load)
{
  out << "LOADASYNC  "  << async_load.GetMemory().GetName() << "  "
                        << async_load.GetOperand().GetName() << "  "
                        << async_load.GetStartAddress() << "  "
                        << async_load.GetEndAddress() << endl;
  return out;
}