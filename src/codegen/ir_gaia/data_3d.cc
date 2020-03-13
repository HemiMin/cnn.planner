#include "codegen/ir_gaia/data_3d.h"

using codegen::gaia::Data3d;

using std::endl;

ostream& codegen::gaia::operator<<(ostream& out, Data3d& data3d)
{
  string ind = "  ";
  out << data3d.GetName()
      << "("  << codegen::gaia::ToString(data3d.GetLayout()) << ","
              << data3d.GetStartIndex() << ","
              << data3d.GetChannel() << ","
              << data3d.GetHeight() << ","
              << data3d.GetWidth()
      << ")"  << endl;
  return out;
}
