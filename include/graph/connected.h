#pragma once

#include "graph/layer.h"
#include "graph/data2d.h"

using graph::layer::Layer;
using graph::data::Data2d;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Fully connected layer.
//! @author   Minsu Kim
//! @date     2020-03-04
////////////////////////////////////////////////////////////////////////////////
class Connected : public Layer
{
  public:
    Connected(int channel) : channel_(channel) {}

    virtual Data* GetOutput(vector<Data*> inputs) const final
    { return connected(inputs); }

  private:
    const int channel_;

    Data2d* connected(vector<Data*> inputs) const
    { return new Data2d(inputs[0]->GetBatch(), 
                        channel_, 
                        inputs[0]->GetType()); }
};
} // namespace layer
} // namespace graph