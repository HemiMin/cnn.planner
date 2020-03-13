#include "codegen/ir_gaia/ir_gaia.h"

#include <glog/logging.h>
#include <math.h>
#include <algorithm>

#include "loop/variable_set.h"
#include "loop/structure.h"
#include "codegen/ir_gaia/memory.h"
#include "codegen/ir_gaia/data.h"
#include "codegen/ir_gaia/data_4d.h"
#include "codegen/ir_gaia/load.h"
#include "codegen/ir_gaia/store.h"
#include "codegen/ir_gaia/conv.h"

using codegen::gaia::GaiaIr;

using std::ceil;
using std::max;
using std::min;
using std::endl;

using loop::VariableSet;
using codegen::gaia::Memory;
using codegen::gaia::Data;
using codegen::gaia::Data4d;
using codegen::gaia::SymbolTag;
using codegen::gaia::DataLayout;
using codegen::gaia::Load;
using codegen::gaia::Store;
using codegen::gaia::Convolution;

GaiaIr::GaiaIr(LayerList layers, const CnnLoop& loop, const Architecture& arch)
{
  int layer_num = 0;
  for (auto layer : layers) {
    switch (layer) {
      case codegen::LayerType::CONV:
        LOG(INFO) << "Add Convolutional Layer";
        AddSymbols(loop, arch, layer_num++);
        AddTexts(loop);
        break;
      default:  LOG(ERROR) << "Invalid layer type: " << layer;
    }
  }
  for (auto op : text_) {
    if (op.first == "CONV")
      CHECK(op.second->Validate())  << "Invalid operation: " 
                                    << *dynamic_cast<Convolution*>(op.second);
    else if (op.first == "LOAD")
      CHECK(op.second->Validate())  << "Invalid operation: "
                                    << *dynamic_cast<Load*>(op.second);
    else if (op.first == "STORE")   
      CHECK(op.second->Validate())  << "Invalid operation: "
                                    << *dynamic_cast<Store*>(op.second);
    else  LOG(ERROR) << "Invalid operation type: " << op.first;
  }
}

GaiaIr::~GaiaIr()
{
  for (auto symbol : symtab_) {
    for (Variable* var : symbol.second) {
      delete var;
    }
  }
  for (auto op : text_) {
    delete op.second;
  }
}

void GaiaIr::AddSymbols(const CnnLoop& loop, const Architecture& arch,
                        const int layer_num)
{
  /* #region Logging */
  LOG(INFO) << "Add memory symbols";
  LOG(INFO) << "Input memory size: " << arch.GetInputMemSize();
  LOG(INFO) << "Weight memory size: " << arch.GetWeightMemSize();
  LOG(INFO) << "Output memory size: " << arch.GetOutputMemSize();
  /* #endregion */
  vector<Variable*> input_mem;
  vector<Variable*> weight_mem;
  vector<Variable*> output_mem;

  input_mem.push_back(
    new Memory("IN_MEM_0", arch.GetInputMemSize()/sizeof(DataType)));
  weight_mem.push_back(
    new Memory("WT_MEM_0", arch.GetWeightMemSize()/sizeof(DataType)));
  output_mem.push_back(
    new Memory("OT_MEM_0", arch.GetOutputMemSize()/sizeof(DataType)));

  symtab_.push_back({"input_memory", input_mem});
  symtab_.push_back({"weight_memory", weight_mem});
  symtab_.push_back({"output_memory", output_mem});

  const VariableSet varset = loop.GetVariableSet();
  /* #region Logging */
  LOG(INFO) << "Add data variable before tiled.";
  LOG(INFO) << "Input(1," << varset.GetIc() << "," 
                          << varset.GetIh() << "," 
                          << varset.GetIw() << ")";
  LOG(INFO) << "Weight("  << varset.GetOc() << ","
                          << varset.GetIc() << ","
                          << varset.GetKh() << ","
                          << varset.GetKw() << ")";
  LOG(INFO) << "Output(1,"<< varset.GetOc() << ","
                          << varset.GetOh() << ","
                          << varset.GetOw() << ")";
  /* #endregion */
  vector<Variable*> untiled_data;
  Data4d* input_data = new Data4d("INPUT_"+std::to_string(layer_num),
                                  DataLayout::NCHW,
                                  0,
                                  1,
                                  varset.GetIc(),
                                  varset.GetIh(),
                                  varset.GetIw());
  Data4d* weight_data =new Data4d("WEIGHT_"+std::to_string(layer_num),
                                  DataLayout::NCHW,
                                  0,
                                  varset.GetOc(), varset.GetIc(),
                                  varset.GetKh(), varset.GetKw());
  Data4d* output_data =new Data4d("OUTPUT_"+std::to_string(layer_num),
                                  DataLayout::NCHW,
                                  0,
                                  1,
                                  varset.GetOc(),
                                  varset.GetOh(),
                                  varset.GetOw());

  untiled_data.push_back(input_data);
  untiled_data.push_back(weight_data);
  untiled_data.push_back(output_data);
  symtab_.push_back({"untiled_data", untiled_data});

  vector<Variable*> input_tiles;
  vector<Variable*> weight_tiles;
  vector<Variable*> output_tiles;

  // Input tiles
  int tiling_cnt=0;
  int batch=1;
  for (int t_ic=0 ; t_ic < varset.GetIc() ; t_ic+=varset.GetTic()) {
    num_ic_tile_++;
    int ic_idx = t_ic;
    int channel = min(varset.GetIc()-t_ic, varset.GetTic());
    const int tih = (varset.GetToh()-1)*varset.GetStride()+
                    (varset.GetIh()-varset.GetKh())%varset.GetStride()+
                    varset.GetKh();
    int tih_start=-varset.GetPh(), tih_end=0;
    while (tih_end < varset.GetIh()+varset.GetPh()-1) {
      tih_end = min(tih_start+tih-1, varset.GetIh()+varset.GetPh()-1);
      int ih_idx = max(0, tih_start);
      int height = min(tih_end, varset.GetIh()-1) - ih_idx + 1;
      
      const int tiw = (varset.GetTow()-1)*varset.GetStride()+
                      (varset.GetIw()-varset.GetKw())%varset.GetStride()+
                      varset.GetKw();
      int tiw_start=-varset.GetPw(), tiw_end=0;
      while (tiw_end < varset.GetIh()+varset.GetPh()-1) {
        tiw_end = min(tiw_start+tiw-1, varset.GetIw()+varset.GetPw()-1);
        int iw_idx = max(0, tiw_start);
        int width = min(tiw_end, varset.GetIw()-1) - iw_idx + 1;

        string name = "INPUT_" +  std::to_string(layer_num) + "_" + 
                                  std::to_string(tiling_cnt++);
        int start_idx = iw_idx+varset.GetIw()*(ih_idx+varset.GetIh()*ic_idx);
        input_tiles.push_back(new Data4d( name,DataLayout::NCHW,start_idx,
                                          batch,channel,height,width));
        LOG(INFO) << "Add input tile ("
                  << start_idx << ", "
                  << batch << ", "
                  << varset.GetTic() << ", "
                  << height << ", "
                  << width << ")";

        tiw_start = tiw_end - (varset.GetKw()-1) + 1;
        tiw_start = ((tiw_start-varset.GetPw())/varset.GetStride())*
                    varset.GetStride() + varset.GetPw();
      }

      tih_start = tih_end - (varset.GetKh()-1) + 1;
      tih_start = ((tih_start-varset.GetPh())/varset.GetStride())*
                  varset.GetStride() + varset.GetPw();
    }
  }

  // Weight tiles
  tiling_cnt = 0;
  for (int t_oc=0 ; t_oc<varset.GetOc() ; t_oc+=varset.GetToc()) {
    for (int t_ic=0 ; t_ic<varset.GetIc() ; t_ic+=varset.GetTic()) {
      //NOTE Assume kernel width and height are fully tiled.
      int oc_idx = t_oc;
      int ic_idx = t_ic;
      int kh_idx = 0;
      int kw_idx = 0;
      int start_idx = kw_idx+varset.GetKw()*
                      (kh_idx+varset.GetKh()*(ic_idx+varset.GetIc()*oc_idx));

      string name = "WEIGHT_" + std::to_string(layer_num) + "_" +
                                std::to_string(tiling_cnt++);
      batch = min(varset.GetOc()-oc_idx, varset.GetToc());
      int channel = min(varset.GetIc()-ic_idx, varset.GetTic());
      int height = varset.GetKh();
      int width = varset.GetKw();
      weight_tiles.push_back(new Data4d(name, DataLayout::NCHW, start_idx,
                                        batch, channel, height, width));
      LOG(INFO) << "Add weight tile ("
                << start_idx << ", "
                << batch << ", "
                << channel << ", "
                << height << ", "
                << width << ")";
    }
  }

  // Output tiles
  tiling_cnt = 0;
  batch = 1;
  for (int t_oc=0 ; t_oc<varset.GetOc() ; t_oc+=varset.GetToc()) {
    num_oc_tile_++;
    for (int t_oh=0 ; t_oh<varset.GetOh() ; t_oh+=varset.GetToh()) {
      num_oh_tile_++;
      for (int t_ow=0 ; t_ow<varset.GetOw() ; t_ow+=varset.GetTow()) {
        num_ow_tile_++;
        int oc_idx = t_oc;
        int oh_idx = t_oh;
        int ow_idx = t_ow;
        int start_idx = t_ow + varset.GetOw()*(t_oh + varset.GetOh()*t_oc);

        string name = "OUTPUT_" + std::to_string(layer_num) + "_" +
                                  std::to_string(tiling_cnt++);
        int channel = min(varset.GetOc()-oc_idx, varset.GetToc());
        int height = min(varset.GetOh()-oh_idx, varset.GetToh());
        int width = min(varset.GetOw()-ow_idx, varset.GetTow());
        output_tiles.push_back(new Data4d(name, DataLayout::NCHW, start_idx,
                                          batch, channel, height, width));
        LOG(INFO) << "Add output tile ("
                  << start_idx << ", "
                  << batch << ", "
                  << channel << ", "
                  << height << ", "
                  << width << ")";
      }
    }
  }
  num_oh_tile_ /= num_oc_tile_;
  num_ow_tile_ /= num_oc_tile_*num_oh_tile_;

  symtab_.push_back({"input_tiles", input_tiles});
  symtab_.push_back({"weight_tiles", weight_tiles});
  symtab_.push_back({"output_tiles", output_tiles});
}

void GaiaIr::AddTexts(const CnnLoop& loop)
{
  vector<Variable*> input_mems;
  vector<Variable*> weight_mems;
  vector<Variable*> output_mems;
  vector<Variable*> input_tiles;
  vector<Variable*> weight_tiles;
  vector<Variable*> output_tiles;

  for (auto syms : symtab_) {
    if (syms.first == "input_memory") input_mems = syms.second;
    else if (syms.first == "weight_memory") weight_mems = syms.second;
    else if (syms.first == "output_memory") output_mems = syms.second;
    else if (syms.first == "input_tiles") input_tiles = syms.second;
    else if (syms.first == "weight_tiles") weight_tiles = syms.second;
    else if (syms.first == "output_tiles") output_tiles = syms.second;
    else if (syms.first == "untiled_data") continue;
    else LOG(ERROR) << "Unknown symbol tag: " << syms.first;
  }

  vector<loop::Type> loop_seq;
  // Sort by loop sequence
  //NOTE Assume kernel map is fully tiled. So, skip here.
  if (loop.GetOffStructure().GetInnerMost() != loop::Type::KERNEL_MAP)
    loop_seq.push_back(loop.GetOffStructure().GetInnerMost());
  if (loop.GetOffStructure().GetSecondInnerMost() != loop::Type::KERNEL_MAP)
    loop_seq.push_back(loop.GetOffStructure().GetSecondInnerMost());
  if (loop.GetOffStructure().GetThirdInnerMost() != loop::Type::KERNEL_MAP)
    loop_seq.push_back(loop.GetOffStructure().GetThirdInnerMost());
  if (loop.GetOffStructure().GetOuterMost() != loop::Type::KERNEL_MAP)
    loop_seq.push_back(loop.GetOffStructure().GetOuterMost());

  const VariableSet varset = loop.GetVariableSet();
  const int num_oc_tile = num_oc_tile_;
  const int num_ic_tile = num_ic_tile_;
  const int num_oh_tile = num_oh_tile_;
  const int num_ow_tile = num_ow_tile_;
  /* #region Logging */
  LOG(INFO) << "The number of output channel tile: " << num_oc_tile;
  LOG(INFO) << "The number of input channel tile: " << num_ic_tile;
  LOG(INFO) << "The number of output height tile: " << num_oh_tile;
  LOG(INFO) << "The number of output width tile: " << num_ow_tile;
  /* #endregion */

  Memory* input_mem = dynamic_cast<Memory*>(input_mems[0]);
  Memory* weight_mem = dynamic_cast<Memory*>(weight_mems[0]);
  Memory* output_mem = dynamic_cast<Memory*>(output_mems[0]);

  int index;

  Data4d* in_tile = nullptr;
  Data4d* wt_tile = nullptr;
  Data4d* ot_tile = nullptr;

  if (loop_seq[0] == loop::Type::OUTPUT_MAP) {
    if (loop_seq[1] == loop::Type::INPUT_CHANNEL) {
      CHECK(loop_seq[2] == loop::Type::OUTPUT_CHANNEL) 
        << "Invalid outer most loop: " << loop_seq[2];
      /* #region OM-IC-OC Weight Stationary */
      LOG(INFO) << "Layer sequence: OM-IC-OC from inner to outer.";
      for (int oc = 0 ; oc < num_oc_tile ; oc++) {
        for (int ic = 0 ; ic < num_ic_tile ; ic++) {
          index = ic+oc*num_ic_tile;
          wt_tile = dynamic_cast<Data4d*>(weight_tiles[index]);
          text_.push_back(
            {
              "LOAD", new Load(*weight_mem, *wt_tile, 0, wt_tile->GetSize()-1)
            }
          );
          LOG(INFO) << "Add Weight LOAD operation. "  << num_oh_tile << " " 
                                                      << num_ow_tile;
          for (int oh = 0 ; oh < num_oh_tile ; oh++) {
            for (int ow = 0 ; ow < num_ow_tile ; ow++) {
              LOG(INFO) << "oc: " << oc << "/" << num_oc_tile 
                        << " ic: " << ic << "/" << num_ic_tile
                        << " oh: " << oh << "/" << num_oh_tile
                        << " ow: " << ow << "/" << num_ow_tile;
              if (num_oh_tile*num_ow_tile*num_ic_tile > 1 || oc == 0) {
                index = ow+num_ow_tile*(oh+ic*num_oh_tile);
                in_tile = dynamic_cast<Data4d*>(input_tiles[index]);
                LOG(INFO) << "size of input tiles: " << input_tiles.size()
                          << "index: " << index;
                text_.push_back(
                  { 
                    "LOAD", 
                    new Load(*input_mem, *in_tile, 0, in_tile->GetSize()-1) 
                  }
                );
                LOG(INFO) << "Add Input LOAD operation.";
              }
              LOG(INFO) << "LOAD1";
              if (num_oh_tile*num_ow_tile > 1 || ic == 0) {
                index = ow+num_ow_tile*(oh+oc*num_oh_tile);
                ot_tile = dynamic_cast<Data4d*>(output_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
                  }
                );
                LOG(INFO) << "Add Output LOAD operation.";
              }
              LOG(INFO) << "LOAD2";
              CHECK(in_tile != nullptr) << "in_tile is null pointer.";
              CHECK(wt_tile != nullptr) << "wt_tile is null pointer.";
              CHECK(ot_tile != nullptr) << "ot_tile is null pointer.";
              text_.push_back(
                {
                  "CONV",
                  new Convolution(*in_tile, *wt_tile, *ot_tile,
                                  varset.GetStride(),
                                  ow == 0 ? varset.GetPw() : 0,
                                  ow == num_ow_tile-1 ? varset.GetPw() : 0,
                                  oh == 0 ? varset.GetPh() : 0,
                                  oh == num_oh_tile-1 ? varset.GetPh() : 0)
                }
              );
              LOG(INFO) << "CONV";
              if (num_oh_tile*num_ow_tile>1 || ic == num_ic_tile-1) {
                text_.push_back(
                  {
                    "STORE",
                    new Store(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
                  }
                );
              }
              LOG(INFO) << "STORE";
            }
          }
        }
      }
      LOG(INFO) << "Success to generate text section in Gaia IR";
      /* #endregion */
    } else 
    if (loop_seq[1] == loop::Type::OUTPUT_CHANNEL) {
      CHECK(loop_seq[2] == loop::Type::INPUT_CHANNEL)
        << "Invalid outer most loop: " << loop_seq[2];
      /* #region OM-OC-IC Weight Stationary */
      LOG(INFO) << "Layer sequence: OM-OC-IC from inner to outer.";
      for (int ic = 0 ; ic < num_ic_tile ; ic++) {
        for (int oc = 0 ; oc < num_oc_tile ; oc++) {
          index = ic+oc*num_ic_tile;
          wt_tile = dynamic_cast<Data4d*>(weight_tiles[index]);
          text_.push_back(
            {
              "LOAD",
              new Load(*weight_mem, *wt_tile, 0, wt_tile->GetSize()-1)
            }
          );
          for (int oh = 0 ; oh < num_oh_tile ; oh++) {
            for (int ow = 0 ; ow < num_ow_tile ; ow++) {
              if (num_oh_tile*num_ow_tile > 1 || oc == 0) {
                index = ow+num_ow_tile*(oh+ic*num_oh_tile);
                in_tile = dynamic_cast<Data4d*>(input_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*input_mem, *in_tile, 0, in_tile->GetSize()-1)
                  }
                );
              }
              if (num_oc_tile*num_oh_tile*num_ow_tile > 1 || ic == 0) {
                index = ow+num_ow_tile*(oh+oc*num_oh_tile);
                ot_tile = dynamic_cast<Data4d*>(output_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
                  }
                );
              }
              CHECK(in_tile != nullptr) << "in_tile is null pointer.";
              CHECK(wt_tile != nullptr) << "wt_tile is null pointer.";
              CHECK(ot_tile != nullptr) << "ot_tile is null pointer.";
              text_.push_back(
                {
                  "CONV",
                  new Convolution(*in_tile, *wt_tile, *ot_tile,
                                  varset.GetStride(),
                                  ow == 0 ? varset.GetPw() : 0,
                                    ow == num_ow_tile-1 ? varset.GetPw() : 0,
                                  oh == 0 ? varset.GetPh() : 0,
                                  oh == num_oh_tile-1 ? varset.GetPh() : 0)
                }
              );
              if (num_oc_tile*num_oh_tile*num_ow_tile>1 || ic==num_ic_tile-1) {
                text_.push_back(
                  {
                    "STORE",
                    new Store(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
                  }
                );
              }
            }
          }
        }
      }
      LOG(INFO) << "Success to generate text section in Gaia IR";
      /* #endregion */
    } else {
      LOG(ERROR) << "Invalid second inner most loop: " << loop_seq[1];
    }
  } else 
  if (loop_seq[0] == loop::Type::INPUT_CHANNEL) {
    if (loop_seq[1] == loop::Type::OUTPUT_MAP) {
      CHECK(loop_seq[2] == loop::Type::OUTPUT_CHANNEL)
        << "Invalid outer most loop: " << loop_seq[2];
      /* #region IC-OM-OC Output Stationary */
      LOG(INFO) << "Layer sequence: IC-OM-OC from inner to outer.";
      for (int oc = 0 ; oc < num_oc_tile ; oc++) {
        for (int oh = 0 ; oh < num_oh_tile ; oh++) {
          for (int ow = 0 ; ow < num_ow_tile ; ow++) {
            index = ow+num_ow_tile*(oh+oc*num_oh_tile);
            Data4d* ot_tile = dynamic_cast<Data4d*>(output_tiles[index]);
            text_.push_back(
              {
                "LOAD",
                new Load(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
              }
            );
            for (int ic = 0 ; ic < num_ic_tile ; ic++) {
              if (num_ic_tile*num_ow_tile*num_oh_tile > 1 || oc == 0) {
                index = ow+num_ow_tile*(oh+ic*num_oh_tile);
                in_tile = dynamic_cast<Data4d*>(input_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*input_mem, *in_tile, 0, in_tile->GetSize()-1)
                  }
                );
              }
              if (num_ic_tile > 1 || ow+oh == 0) {
                index = ic+oc*num_ic_tile;
                wt_tile = dynamic_cast<Data4d*>(weight_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*weight_mem, *wt_tile, 0, wt_tile->GetSize()-1)
                  }
                );
              }
              CHECK(in_tile != nullptr) << "in_tile is null pointer.";
              CHECK(wt_tile != nullptr) << "wt_tile is null pointer.";
              CHECK(ot_tile != nullptr) << "ot_tile is null pointer.";
              text_.push_back(
                {
                  "CONV",
                  new Convolution(*in_tile, *wt_tile, *ot_tile,
                                  varset.GetStride(),
                                  ow == 0 ? varset.GetPw() : 0,
                                  ow == num_ow_tile-1 ? varset.GetPw() : 0,
                                  oh == 0 ? varset.GetPh() : 0,
                                  oh == num_oh_tile-1 ? varset.GetPh() : 0)
                }
              );
            }
            text_.push_back(
              {
                "STORE",
                new Store(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
              }
            );
          }
        }
      }
      LOG(INFO) << "Success to generate text section in Gaia IR";
      /* #endregion */
    } else 
    if (loop_seq[1] == loop::Type::OUTPUT_CHANNEL) {
      CHECK(loop_seq[2] == loop::Type::OUTPUT_MAP)
        << "Invalid outer most loop: " << loop_seq[2];
      /* #region IC-OC-OM Output Stationary */
      LOG(INFO) << "Layer sequence: IC-OC-OM from inner to outer.";
      for (int oh = 0 ; oh < num_oh_tile ; oh++) {
        for (int ow = 0 ; ow < num_ow_tile ; ow++) {
          for (int oc = 0 ; oc < num_oc_tile ; oc++) {
            index = ow+num_ow_tile*(oh+oc*num_oh_tile);
            Data4d* ot_tile = dynamic_cast<Data4d*>(output_tiles[index]);
            text_.push_back(
              {
                "LOAD",
                new Load(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
              }
            );
            for (int ic = 0 ; ic < num_ic_tile ; ic++) {
              if (num_ic_tile > 1 || oc == 0) {
                index = ow+num_ow_tile*(oh+ic*num_oh_tile);
                in_tile = dynamic_cast<Data4d*>(input_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*input_mem, *in_tile, 0, in_tile->GetSize()-1)
                  }
                );
              }
              if (num_ic_tile*num_oc_tile > 1 || ow+oh == 0) {
                index = ic+oc*num_ic_tile;
                wt_tile = dynamic_cast<Data4d*>(weight_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*weight_mem, *wt_tile, 0, wt_tile->GetSize()-1)
                  }
                );
              }
              CHECK(in_tile != nullptr) << "in_tile is null pointer.";
              CHECK(wt_tile != nullptr) << "wt_tile is null pointer.";
              CHECK(ot_tile != nullptr) << "ot_tile is null pointer.";
              text_.push_back(
                {
                  "CONV",
                  new Convolution(*in_tile, *wt_tile, *ot_tile,
                                  varset.GetStride(),
                                  ow == 0 ? varset.GetPw() : 0,
                                  ow == num_ow_tile-1 ? varset.GetPw() : 0,
                                  oh == 0 ? varset.GetPh() : 0,
                                  oh == num_oh_tile-1 ? varset.GetPh() : 0)
                }
              );
            }
            text_.push_back(
              {
                "STORE",
                new Store(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
              }
            );
          }
        }
      }
      LOG(INFO) << "Success to generate text section in Gaia IR";
      /* #endregion */
    } else {
      LOG(ERROR) << "Invalid second inner most loop: " << loop_seq[1];
    }
  } else 
  if (loop_seq[0] == loop::Type::OUTPUT_CHANNEL) {
    if (loop_seq[1] == loop::Type::OUTPUT_MAP) {
      CHECK(loop_seq[2] == loop::Type::INPUT_CHANNEL)
        << "Invalid outer most loop: " << loop_seq[2];
      /* #region OC-OM-IC Input Stationary */
      LOG(INFO) << "Layer sequence: OC-OM-IC from inner to outer.";
      for (int ic = 0 ; ic < num_ic_tile ; ic++) {
        for (int oh = 0 ; oh < num_oh_tile ; oh++) {
          for (int ow = 0 ; ow < num_ow_tile ; ow++) {
            index = ow+num_ow_tile*(oh+ic*num_oh_tile);
            in_tile = dynamic_cast<Data4d*>(input_tiles[index]);
            text_.push_back(
              {
                "LOAD",
                new Load(*input_mem, *in_tile, 0, in_tile->GetSize()-1)
              }
            );
            for (int oc = 0 ; oc < num_oc_tile ; oc++) {
              if (num_oc_tile > 1 || ow+oh == 0) {
                index = ic+oc*num_ic_tile;
                wt_tile = dynamic_cast<Data4d*>(weight_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*weight_mem, *wt_tile, 0, wt_tile->GetSize()-1)
                  }
                );
              }
              if (num_oc_tile*num_ow_tile*num_oh_tile > 1 || ic == 0) {
                index = ow+num_ow_tile*(oh+oc*num_oh_tile);
                ot_tile = dynamic_cast<Data4d*>(output_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
                  }
                );
              }
              CHECK(in_tile != nullptr) << "in_tile is null pointer.";
              CHECK(wt_tile != nullptr) << "wt_tile is null pointer.";
              CHECK(ot_tile != nullptr) << "ot_tile is null pointer.";
              text_.push_back(
                {
                  "CONV",
                  new Convolution(*in_tile, *wt_tile, *ot_tile,
                                  varset.GetStride(),
                                  ow == 0 ? varset.GetPw() : 0,
                                  ow == num_ow_tile-1 ? varset.GetPw() : 0,
                                  oh == 0 ? varset.GetPh() : 0,
                                  oh == num_oh_tile-1 ? varset.GetPh() : 0)
                }
              );
              if (num_oc_tile*num_ow_tile*num_oh_tile>1 || ic==num_ic_tile-1) {
                text_.push_back(
                  {
                    "STORE",
                    new Store(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
                  }
                );
              }
            }
          }
        }
      }
      LOG(INFO) << "Success to generate text section in Gaia IR";
      /* #endregion */
    } else 
    if (loop_seq[1] == loop::Type::INPUT_CHANNEL) {
      CHECK(loop_seq[2] == loop::Type::OUTPUT_MAP)
        << "Invalid outer most loop: " << loop_seq[2];
      /* #region OC-IC-OM Input Stationary */
      LOG(INFO) << "Layer sequence: OC-IC-OM from inner to outer.";
      for (int oh = 0 ; oh < num_oh_tile ; oh++) {
        for (int ow = 0 ; ow < num_ow_tile ; ow++) {
          for (int ic = 0 ; ic < num_ic_tile ; ic++) {
            index = ow+num_ow_tile*(oh+ic*num_oh_tile);
            in_tile = dynamic_cast<Data4d*>(input_tiles[index]);
            text_.push_back(
              {
                "LOAD",
                new Load(*input_mem, *in_tile, 0, in_tile->GetSize()-1)
              }
            );
            for (int oc = 0 ; oc < num_oc_tile ; oc++) {
              if (num_oc_tile*num_ic_tile > 1 || ow+oh == 0) {
                index = ic+oc*num_ic_tile;
                wt_tile = dynamic_cast<Data4d*>(weight_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*weight_mem, *wt_tile, 0, wt_tile->GetSize()-1)
                  }
                );
              }
              if (num_oc_tile > 1 || ic == 0) {
                index = ow+num_ow_tile*(oh+oc*num_oh_tile);
                ot_tile = dynamic_cast<Data4d*>(output_tiles[index]);
                text_.push_back(
                  {
                    "LOAD",
                    new Load(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
                  }
                );
              }
              CHECK(in_tile != nullptr) << "in_tile is null pointer.";
              CHECK(wt_tile != nullptr) << "wt_tile is null pointer.";
              CHECK(ot_tile != nullptr) << "ot_tile is null pointer.";
              text_.push_back(
                {
                  "CONV",
                  new Convolution(*in_tile, *wt_tile, *ot_tile,
                                  varset.GetStride(),
                                  ow == 0 ? varset.GetPw() : 0,
                                  ow == num_ow_tile-1 ? varset.GetPw() : 0,
                                  oh == 0 ? varset.GetPh() : 0,
                                  oh == num_oh_tile-1 ? varset.GetPh() : 0)
                }
              );
              if (num_oc_tile > 1 || ic == num_ic_tile-1) {
                text_.push_back(
                  {
                    "STORE",
                    new Store(*output_mem, *ot_tile, 0, ot_tile->GetSize()-1)
                  }
                );
              }
            }
          }
        }
      }
      LOG(INFO) << "Success to generate text section in Gaia IR";
      /* #endregion */
    } else {
      LOG(ERROR) << "Invalid second inner most loop: " << loop_seq[1];
    }
  } else {
    LOG(ERROR) << "Invalid inner most loop: " << loop_seq[0];
  }
}

ostream& codegen::gaia::operator<<(ostream& out, const GaiaIr& gaia_ir)
{
  out << "[var]" << endl;
  for (auto syms : gaia_ir.GetSymbolTable()) {
    // memory
    if (syms.first == "input_memory" || 
        syms.first == "weight_memory" || 
        syms.first == "output_memory") {
      for (auto var : syms.second) {
        out << *dynamic_cast<Memory*>(var);
      }
    } else 
    // data
    if (syms.first == "untiled_data") {
      for (auto var : syms.second) {
        out << *dynamic_cast<Data4d*>(var);
      }
    } else
    // tiled data
    if (syms.first == "input_tiles" ||
        syms.first == "weight_tiles" ||
        syms.first == "output_tiles") {
      for (auto var : syms.second) {
        out << *dynamic_cast<Data4d*>(var);
      }
    }
  }
  out << endl << "[text]" << endl;
  for (auto op : gaia_ir.GetText()) {
    if (op.first == "CONV") {
      out << *dynamic_cast<Convolution*>(op.second);
    } else
    if (op.first == "LOAD") {
      out << *dynamic_cast<Load*>(op.second);
    } else 
    if (op.first == "STORE") {
      out << *dynamic_cast<Store*>(op.second);
    } else {
      LOG(ERROR) << "Invalid operator tag: " << op.first;
    }
  }
  return out;
}
