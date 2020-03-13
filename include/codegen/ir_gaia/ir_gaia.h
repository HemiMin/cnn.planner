#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <string>

#include "loop/cnn_loop.h"
#include "arch/architecture.h"
#include "codegen/ir_gaia/variable.h"
#include "codegen/ir_gaia/op.h"
#include "codegen/layer_type.h"

using std::ostream;
using std::vector;
using std::pair;
using std::string;

using loop::CnnLoop;
using arch::Architecture;
using codegen::gaia::Variable;
using codegen::gaia::Operator;
using codegen::LayerList;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-30
////////////////////////////////////////////////////////////////////////////////
typedef string SymbolTag;
typedef string OperatorTag;
class GaiaIr
{
  public:
    GaiaIr(LayerList layers, const CnnLoop& loop, const Architecture& arch);
    ~GaiaIr();

    vector<pair<SymbolTag, vector<Variable*>>> GetSymbolTable(void) const
    { return symtab_; }
    vector<pair<OperatorTag, Operator*>> GetText(void) const { return text_; }

  private:
    vector<pair<SymbolTag, vector<Variable*>>> symtab_;
    vector<pair<OperatorTag, Operator*>> text_;
    int num_oc_tile_=0;
    int num_ic_tile_=0;
    int num_oh_tile_=0;
    int num_ow_tile_=0;

    void AddSymbols(const CnnLoop& loop, const Architecture& arch,
                    const int layer_num);
    void AddTexts(const CnnLoop& loop);
};
ostream& operator<<(ostream& out, const codegen::gaia::GaiaIr& gaia_ir);
} // namespace gaia
} // namespace codegen
