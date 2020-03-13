#pragma once

#include "graph/layer.h"
#include "graph/data4d.h"

using graph::layer::Layer;
using graph::data::Data4d;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Crop layer.
//! @author   Minsu Kim
//! @date     2020-03-03
////////////////////////////////////////////////////////////////////////////////
class Crop : public Layer
{
  public:
    Crop(int width, int height) : width_(width), height_(height) {}

    Data* GetOutput(vector<Data*> inputs) const final
    { return crop(inputs); }

  private:
    const int width_;
    const int height_;

    Data4d* crop(vector<Data*> inputs) const
    { 
      Data4d* input = inputs[0];
      return new Data4d(input.GetBatch(), 
                        input.GetChannel(), 
                        height_, 
                        width_,
                        input.GetType()); 
    }
};
} // namespace layer
} // namespace graph