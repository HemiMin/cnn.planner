#include "codegen/ir_gaia/store.h"

#include <glog/logging.h>

using std::endl;

using codegen::gaia::Store;

ostream& codegen::gaia::operator<<(ostream& out, const Store& store)
{
  out << "STORE   " << store.GetOperand().GetName() << "  "
                    << store.GetMemory().GetName() << "  " 
                    << store.GetStartAddress() << "  "
                    << store.GetEndAddress() << endl;
  return out;
}