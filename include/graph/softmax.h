#pragma once

#include "graph/layer.h"
#include "graph/data2d.h"

using graph::layer::Layer;
using graph::data::Data2d;
using graph::data::DataType;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Softmax layer.
//! @author   Minsu Kim
//! @date     2020-03-04
////////////////////////////////////////////////////////////////////////////////
class Softmax : public Layer
{
  public:
    Softmax(float temp) : temp_(temp) {}

    Data* GetOutput(vector<Data*> inputs) const final
    { return softmax(inputs); }

  private:
    const float temp_;  // temperature

    //NOTE: The graph namespace describes only the parameter change through the layer.
    //NOTE: Softmax layer doesn't change the parameter.
    Data2d* softmax(vector<Data*> inputs) const
    {
      Data2d* input = dynamic_cast<Data2d*>(inputs[0]);
      return new Data2d(input->GetBatch(), 
                        input->GetChannel(), 
                        DataType::S_FLOAT);
    }
};
} // namespace layer
} // namespace graph