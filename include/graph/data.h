#pragma once

#include <glog/logging.h>

namespace graph {
namespace data {
////////////////////////////////////////////////////////////////////////////////
//! @brief    A description of intermediate data through DNN.
//! @author   Minsu Kim
//! @date     2020-03-03
////////////////////////////////////////////////////////////////////////////////
enum DataType { D_FLOAT=0,S_FLOAT,H_FLOAT,FIXED64,FIXED32,FIXED16,FIXED8 };
inline int size_of_type(DataType type)
{
  switch (type) {
    case D_FLOAT:
    case FIXED64:
      return 8;
    case S_FLOAT:
    case FIXED32:
      return 4;
    case H_FLOAT:
    case FIXED16:
      return 2;
    case FIXED8:
      return 1;
    default:
      LOG(ERROR) << "Invalid date type: " << type;
  }
  return -1; // unreachable.
}
class Data
{
  public:
    Data(DataType type) 
      : type_(type), d_size_(size_of_type(type)) {}
    virtual ~Data() {}

    DataType GetType(void) const { return type_; }
    virtual int GetSize(void) const { return d_size_; }

  protected:
    const DataType type_;
    const int d_size_;
};
} // namespace data
} // namespace graph