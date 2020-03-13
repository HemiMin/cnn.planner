#pragma once

#include "graph/layer.h"
#include "graph/data4d.h"

using graph::layer::Layer;
using graph::data::Data4d;

namespace graph {
namespace layer {
namespace activ {
  enum Type { ReLU=1, LeakyReLU };
} // namespace activ
////////////////////////////////////////////////////////////////////////////////
//! @brief    Activation layer.
//! @author   Minsu Kim
//! @date     2020-03-03
////////////////////////////////////////////////////////////////////////////////
class Activation : public Layer
{
  public:
    Activation(active::Type type) : type_(type) {}
    activ::Type GetType(void) const { return type_; }
    virtual Data* GetOutput(vector<Data*> inputs) const final 
    { return act(inputs); }

  private:
    const activ::Type type_;
    //NOTE: The graph namespace describes only the parameter change through the layer.
    //NOTE: Activation layer doesn't change the parameter.
    Data4d* act(vector<Data*> inputs) const
    { return new Data4d(*dynamic_cast<Data4d*>(inputs[0])); }
};
} // namespace layer
} // namespace graph