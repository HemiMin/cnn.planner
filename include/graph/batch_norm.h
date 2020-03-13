#pragma once

#include "graph/layer.h"
#include "graph/data4d.h"

using graph::layer::Layer;
using graph::data::Data4d;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Batch normalization layer.
//! @author   Minsu Kim
//! @date     2020-03-04
////////////////////////////////////////////////////////////////////////////////
class BatchNorm : public Layer
{
  public:
    virtual Data* GetOutput(vector<Data*> inputs) const final
    { return batchNorm(inputs); }

  private:
    //NOTE: The graph namespace describes only the parameter change through the layer.
    //NOTE: Batch normalization layer doesn't change the parameter.
    Data4d* batchNorm(vector<Data*> inputs) const
    { return new Data4d(*dynamic_cast<Data4d*>(inputs[0])); }
};
} // namespace layer
} // namespace graph