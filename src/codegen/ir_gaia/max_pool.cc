#include "codegen/ir_gaia/max_pool.h"

using std::endl;

using codegen::gaia::MaxPool;

ostream& codegen::gaia::operator<<(ostream& out, const MaxPool& maxpool)
{
  out << "MAXPOOL  "  << maxpool.GetOutputOperand().GetName() << "  "
                      << maxpool.GetInputOperand().GetName() << "  "
                      << maxpool.GetKernelSize() << "  "
                      << maxpool.GetStride() << "  "
                      << "("  << maxpool.GetLeftPadding() << ","
                              << maxpool.GetRightPadding() << ","
                              << maxpool.GetUpPadding() << ","
                              << maxpool.GetDownPadding()
                      << ")" << endl;
  return out;
}