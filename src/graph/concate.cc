#include "graph/concate.h"

#include <glog/logging.h>

using graph::layer::Concate;

Data4d* Concate::concat(vector<Data*> inputs) const
{
  Data4d* data_1 = dynamic_cast<Data4d*>(inputs[0]);
  Data4d* data_2 = dynamic_cast<Data4d*>(inputs[1]);

  CHECK(data_1->GetType() == data_2->GetType())
    << "Data type mismatch between data_1 and data_2: "
    << "data_1("<<data_1->GetType()<<"), data_2("<<data_2->GetType()<<")";
  CHECK(data_1->GetBatch() == data_2->GetBatch())
    << "Batch size mismatch between data_1 and data_2: "
    << "data_1("<<data_1->GetBatch()<<"), data_2("<<data_2->GetBatch()<<")";
  CHECK(data_1->GetHeight() == data_2->GetHeight())
    << "Height size mismatch between data_1 and data_2: "
    << "data_1("<<data_1->GetHeight()<<"), data_2("<<data_2->GetHeight()<<")";
  CHECK(data_1->GetWidth() == data_2->GetWidth())
    << "Width size mismatch between data_1 and data_2: "
    << "data_1("<<data_1->GetWidth()<<"), data_2("<<data_2->GetWidth()<<")";
  return new Data4d(data_1->GetBatch(),
                    data_1->GetChannel() + data_2->GetChannel(),
                    data_1->GetHeight(),
                    data_1->GetWidth(),
                    data_1->GetType());
}
