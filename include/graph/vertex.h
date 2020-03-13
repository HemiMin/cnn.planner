#pragma once

#include <vector>

#include "graph/edge.h"
#include "graph/layer.h"

using std::vector;

using graph::Edge;
using graph::layer::Layer;

namespace graph {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Vertex (Node) of Graph data structure.
//! @author     Minsu Kim
//! @date       2020-03-11
////////////////////////////////////////////////////////////////////////////////
class Vertex
{
  public:
    Vertex(void) {}
    Vertex(Layer* layer) : layer_(layer) {}
    ~Vertex() { delete layer_; }

    void AddIndegree(Edge* indegree) { indegrees_.push_back(indegree); }
    void AddOutdegree(Edge* outdegree) { outdegrees_.push_back(outdegree); }
    
    void AddInAdjacent(Vertex* in_adjacent)
      { in_adjacents_.push_back(in_adjacent);}
    void AddOutAdjacent(Vertex* out_adjacent)
      { out_adjacents_.push_back(out_adjacent); }
    void SetLayer(Layer* layer);

    vector<Edge*> GetIndegrees(void) const { return indegrees_; }
    vector<Edge*> GetOutdegrees(void) const{ return outdegrees_;}

    vector<Vertex*> GetInAdjacents(void) const { return in_adjacents_; }
    vector<Vertex*> GetOutAdjacents(void) const{ return out_adjacents_;}

    Layer* GetLayer(void) const { return layer_; }

  protected:
    vector<Edge*> indegrees_;
    vector<Edge*> outdegrees_;

    vector<Vertex*> in_adjacents_;
    vector<Vertex*> out_adjacents_;

    Layer* layer_ = nullptr;
};
} // namespace graph