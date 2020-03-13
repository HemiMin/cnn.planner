#include "codegen/ir_gaia/sync.h"

using std::endl;

using codegen::gaia::Sync;

ostream& codegen::gaia::operator<<(ostream& out, const Sync& sync)
{
  out << "SYNC" << endl;
  return out;
}