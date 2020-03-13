#pragma once

#include "graph/layer.h"
#include "graph/data4d.h"

using graph::layer::Layer;
using graph::data::Data4d;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Add bias layer.
//! @author   Minsu Kim
//! @date     2020-03-03
////////////////////////////////////////////////////////////////////////////////
class Bias : public Layer
{
  public:
    virtual Data* GetOutput(vector<Data*> inputs) const final
    { return addBias(inputs); }

  private:
    //NOTE: The graph namespace describes only the parameter change through the layer.
    //NOTE: Bias layer doesn't change the parameter.
    Data4d* addBias(vector<Data*> inputs) const
    { return new Data4d(*dynamic_cast<Data4d*>(inputs[0])); }
};
} // namespace layer
} // namespace graph