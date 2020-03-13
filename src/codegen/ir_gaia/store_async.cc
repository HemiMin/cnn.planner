#include "codegen/ir_gaia/store_async.h"

using std::endl;

using codegen::gaia::AsyncStore;

ostream& codegen::gaia::operator<<(ostream& out, const AsyncStore& async_store)
{
  out << "STOREASYNC  " << async_store.GetOperand().GetName() << "  "
                        << async_store.GetMemory().GetName() << "  "
                        << async_store.GetStartAddress() << "  "
                        << async_store.GetEndAddress() << endl;
  return out;
}