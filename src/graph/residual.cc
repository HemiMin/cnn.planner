#include "graph/residual.h"

#include <algorithm>
#include <glog/logging.h>

using graph::layer::Residual;

using std::max;

Data4d* Residual::residual(vector<Data*> inputs) const
{
  Data4d* input = dynamic_cast<Data4d*>(inputs[0]);
  Data4d* forward = dynamic_cast<Data4d*>(inputs[1]);

  CHECK(input->GetType() == forward->GetType()) 
    << "Type mismatch between input and forward: "
    << "input("<<input->GetType()<<"), forward("<<forward->GetType()<<")";
  CHECK(input->GetBatch() == forward->GetBatch())
    << "Batch size mismatch between input and forward: "
    << "input("<<input->GetBatch()<<"), forward("<<forward->GetBatch()<<")";
  return new Data4d(input->GetBatch(),
                    max(input->GetChannel(), forward->GetChannel()),
                    max(input->GetHeight(),  forward->GetHeight() ),
                    max(input->GetWidth(),   forward->GetWidth()  ),
                    input->GetType()
  );
}