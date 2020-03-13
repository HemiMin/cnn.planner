#pragma once

#include "graph/layer.h"
#include "graph/data4d.h"

using graph::layer::Layer;
using graph::data::Data4d;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Concatenate layer.
//! @author   Minsu Kim
//! @date     2020-03-04
////////////////////////////////////////////////////////////////////////////////
class Concate : public Layer
{
  public:
    virtual Data* GetOutput(vector<Data*> inputs) const final
    { return concat(inputs); }

  private:
    Data4d* concat(vector<Data*> inputs) const;
};
} // namespace layer
} // namespace graph