#pragma once

#include "graph/layer.h"
#include "graph/data2d.h"

using graph::layer::Layer;
using graph::data::Data2d;
using graph::data::DataType;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Detection layer in object detection application.
//! @author     Minsu Kim
//! @date       2020-03-11
////////////////////////////////////////////////////////////////////////////////
class Detect : public Layer
{
  public:
    Detect(int classes, bool background, int coord, float thresh)
      : classes_(classes), 
        background_(background), 
        coord_(coord), 
        threshold_(thresh) {}

    Data* GetOutput(vector<Data*> inputs) const final
    {  return detect(inputs); }

  private:
    const int classes_;
    const bool background_;
    const int coord_;
    const float threshold_;

    Data2d* detect(vector<Data*> inputs) const
    { return new Data2d(inputs.GetBatch(), classes_, DataType::S_FLOAT); }
};
} // namespace layer
} // namespace graph