#include "graph/conv.h"

using graph::layer::Convolution;

Data4d* Convolution::conv(vector<Data*> inputs) const
{
  Data4d* input = dynamic_cast<Data4d*>(inputs[0]);

  int out_w = (input->GetWidth() + 2*padding_ - ksize_) / stride_ + 1;
  int out_h = (input->GetHeight()+ 2*padding_ - ksize_) / stride_ + 1;
  return new Data4d(input->GetBatch(),channel_,out_h,out_w,input->GetType());
}