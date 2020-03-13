#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <string>

#include "codegen/ir_gaia/variable.h"

using std::ostream;
using std::istream;
using std::pair;
using std::vector;
using std::string;

using codegen::gaia::Variable;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Data class for Gaia intermediate representation.
//! @details    Gaia is name of goddess of Greek mythology.
//! @author     Minsu Kim
//! @date       2020-01-27
////////////////////////////////////////////////////////////////////////////////
enum DataLayout { NCHW=0,  NHWC };
string ToString(const DataLayout layout);
//typedef pair<int, int> Range; // Address range.
class Data : public Variable
{
  public:
    Data( string name, DataLayout layout, int start)
      : name_(name), layout_(layout), start_index_(start) {}

    //! @brief    name_ means string representation of Data.
    string GetName(void) const { return name_; }
    //! @brief    layout_ means data layout. e.g. NCHW, NHWC, etc.
    DataLayout GetLayout(void) const { return layout_; }
    //! @brief    start_index_ means an index of left-top point in the cuboid tile.
    int GetStartIndex(void) const { return start_index_; }
    //! @brief    size_ means data size in bytes.
    size_t GetSize(void) const { return size_; }

    virtual bool operator==(const Data& data) const 
    { return data.GetLayout() == layout_ && data.GetSize() == size_; }

  protected:
    string      name_;
    DataLayout  layout_;
    int         start_index_;
    size_t      size_=0; // Bytes.
};
} // namespace gaia
} // namespace codegen
