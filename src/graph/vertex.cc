#include "graph/vertex.h"

using graph::Vertex;

void Vertex::SetLayer(Layer* layer)
{
  if (layer_ != nullptr) {
    delete layer_;
  }
  layer_ = layer;
}