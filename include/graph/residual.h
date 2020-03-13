#pragma once

#include "graph/layer.h"
#include "graph/data4d.h"

using graph::layer::Layer;
using graph::data::Data4d;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Residual layer.
//! @author   Minsu Kim
//! @date     2020-03-04
////////////////////////////////////////////////////////////////////////////////
class Residual : public Layer
{
  public:
    Data* GetOutput(vector<Data*> inputs) const final
    { return residual(inputs); }

  private:
    Data4d* residual(vector<Data*> inputs) const;
};
} // namespace layer
} // namespace graph