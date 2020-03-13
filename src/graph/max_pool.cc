#include "graph/max_pool.h"

using graph::layer::MaxPool;

Data4d* MaxPool::maxpool(vector<Data*> inputs) const
{
  Data4d* input = dynamic_cast<Data4d*>(inputs[0]);
  int out_w = (input->GetWidth() + padding_ - ksize_) / stride_ + 1;
  int out_h = (input->GetHeight()+ padding_ - ksize_) / stride_ + 1;
  return new Data4d(input->GetBatch(),
                    input->GetChannel(),
                    out_h,
                    out_w,
                    input->GetType());
}