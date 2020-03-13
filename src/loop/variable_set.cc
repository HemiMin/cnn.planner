#include "loop/variable_set.h"

#include <glog/logging.h>
#include <iostream>

#include "general/utils.h"

using std::endl;

using loop::VariableSet;

VariableSet::VariableSet(const Parameter& param)
{
  SetStride(param.GetStride());
    
  SetIw(param.GetIw());
  SetIh(param.GetIh());
  SetIc(param.GetIc());

  SetPw(param.GetPw());
  SetPh(param.GetPh());

  SetKw(param.GetKw());
  SetKh(param.GetKh());

  SetOw(CalcOutputLength(param.GetIw()));
  SetOh(CalcOutputLength(param.GetIh()));
  SetOc(param.GetOc());
}

void VariableSet::SetStride(const int stride)
{
  off_loop_vars_.SetStride(stride);
  on_loop_vars_.SetStride(stride);
  parl_loop_vars_.SetStride(stride);
}

void VariableSet::SetIw(const int input_width)
{
  off_loop_vars_.SetIw(input_width);
}

void VariableSet::SetIh(const int input_height)
{
  off_loop_vars_.SetIh(input_height);
}

void VariableSet::SetIc(const int input_channel)
{
  off_loop_vars_.SetIc(input_channel);
}

void VariableSet::SetPw(const int pad_width)
{
  off_loop_vars_.SetPw(pad_width);
  on_loop_vars_.SetPw(pad_width);
  parl_loop_vars_.SetPw(pad_width);
}

void VariableSet::SetPh(const int pad_height)
{
  off_loop_vars_.SetPh(pad_height);
  on_loop_vars_.SetPh(pad_height);
  parl_loop_vars_.SetPh(pad_height);
}

void VariableSet::SetKw(const int kernel_width)
{
  off_loop_vars_.SetKw(kernel_width);
}

void VariableSet::SetKh(const int kernel_height)
{
  off_loop_vars_.SetKh(kernel_height);
}

void VariableSet::SetOw(const int output_width)
{
  off_loop_vars_.SetOw(output_width);
}

void VariableSet::SetOh(const int output_height)
{
  off_loop_vars_.SetOh(output_height);
}

void VariableSet::SetOc(const int output_channel)
{
  off_loop_vars_.SetOc(output_channel);
}

void VariableSet::SetTiw(const int tile_input_width)
{
  on_loop_vars_.SetIw(tile_input_width);
}

void VariableSet::SetTih(const int tile_input_height)
{
  on_loop_vars_.SetIh(tile_input_height);
}

void VariableSet::SetTic(const int tile_input_channel)
{
  on_loop_vars_.SetIc(tile_input_channel);
}

void VariableSet::SetTkw(const int tile_kernel_width)
{
  on_loop_vars_.SetKw(tile_kernel_width);
}

void VariableSet::SetTkh(const int tile_kernel_height)
{
  on_loop_vars_.SetKh(tile_kernel_height);
}

void VariableSet::SetTow(const int tile_output_width)
{
  on_loop_vars_.SetOw(tile_output_width);
}

void VariableSet::SetToh(const int tile_output_height)
{
  on_loop_vars_.SetOh(tile_output_height);
}

void VariableSet::SetToc(const int tile_output_channel)
{
  on_loop_vars_.SetOc(tile_output_channel);
}

void VariableSet::SetPiw(const int parallel_input_width)
{
  parl_loop_vars_.SetIw(parallel_input_width);
}

void VariableSet::SetPih(const int parallel_input_height)
{
  parl_loop_vars_.SetIh(parallel_input_height);
}

void VariableSet::SetPic(const int parallel_input_channel)
{
  parl_loop_vars_.SetIc(parallel_input_channel);
}

void VariableSet::SetPkw(const int parallel_kernel_width)
{
  parl_loop_vars_.SetKw(parallel_kernel_width);
}

void VariableSet::SetPkh(const int parallel_kernel_height)
{
  parl_loop_vars_.SetKh(parallel_kernel_height);
}

void VariableSet::SetPow(const int parallel_output_width)
{
  parl_loop_vars_.SetOw(parallel_output_width);
}

void VariableSet::SetPoh(const int parallel_output_height)
{
  parl_loop_vars_.SetOh(parallel_output_height);
}

void VariableSet::SetPoc(const int parallel_output_channel)
{
  parl_loop_vars_.SetOc(parallel_output_channel);
}

void VariableSet::SetOffLoopVariables(loop::Variables off_loop_vars)
{
  off_loop_vars_ = off_loop_vars;
}

void VariableSet::SetOnLoopVariables(loop::Variables on_loop_vars)
{
  on_loop_vars_ = on_loop_vars;
}

void VariableSet::SetParlLoopVariables(loop::Variables parl_loop_vars)
{
  parl_loop_vars_ = parl_loop_vars;
}

int VariableSet::GetStride(void) const
{
  CHECK(off_loop_vars_.GetStride() == on_loop_vars_.GetStride())
    << "Stride of off-chip loop and on-chip loop is different: "
    << "off-chip loop(" << off_loop_vars_.GetStride() << ") "
    << "on-chip loop(" << on_loop_vars_.GetStride() << ")";
  CHECK(on_loop_vars_.GetStride() == parl_loop_vars_.GetStride())
    << "Stride of on-chip loop and parallelization loop is different: "
    << "on-chip loop(" << on_loop_vars_.GetStride() << ") "
    << "parallelization loop(" << parl_loop_vars_.GetStride() << ")";
  return off_loop_vars_.GetStride();
}

int VariableSet::GetIw(void) const
{
  return off_loop_vars_.GetIw();
}

int VariableSet::GetIh(void) const
{
  return off_loop_vars_.GetIh();
}

int VariableSet::GetIc(void) const
{
  return off_loop_vars_.GetIc();
}

int VariableSet::GetPw(void) const
{
  return off_loop_vars_.GetPw();
}

int VariableSet::GetPh(void) const
{
  return off_loop_vars_.GetPh();
}

int VariableSet::GetKw(void) const
{
  return off_loop_vars_.GetKw();
}

int VariableSet::GetKh(void) const
{
  return off_loop_vars_.GetKh();
}

int VariableSet::GetOw(void) const
{
  return off_loop_vars_.GetOw();
}

int VariableSet::GetOh(void) const
{
  return off_loop_vars_.GetOh();
}

int VariableSet::GetOc(void) const
{
  return off_loop_vars_.GetOc();
}

int VariableSet::GetTiw(void) const
{
  return on_loop_vars_.GetIw();
}

int VariableSet::GetTih(void) const
{
  return on_loop_vars_.GetIh();
}

int VariableSet::GetTic(void) const
{
  return on_loop_vars_.GetIc();
}

int VariableSet::GetTkw(void) const
{
  return on_loop_vars_.GetKw();
}

int VariableSet::GetTkh(void) const
{
  return on_loop_vars_.GetKh();
}

int VariableSet::GetTow(void) const
{
  return on_loop_vars_.GetOw();
}

int VariableSet::GetToh(void) const
{
  return on_loop_vars_.GetOh();
}

int VariableSet::GetToc(void) const
{
  return on_loop_vars_.GetOc();
}

int VariableSet::GetPiw(void) const
{
  return parl_loop_vars_.GetIw();
}

int VariableSet::GetPih(void) const
{
  return parl_loop_vars_.GetIh();
}

int VariableSet::GetPic(void) const
{
  return parl_loop_vars_.GetIc();
}

int VariableSet::GetPkw(void) const
{
  return parl_loop_vars_.GetKw();
}
int VariableSet::GetPkh(void) const
{
  return parl_loop_vars_.GetKh();
}

int VariableSet::GetPow(void) const
{
  return parl_loop_vars_.GetOw();
}

int VariableSet::GetPoh(void) const
{
  return parl_loop_vars_.GetOh();
}

int VariableSet::GetPoc(void) const
{
  return parl_loop_vars_.GetOc();
}

const loop::Variables& VariableSet::GetOffLoopVariables(void) const
{
  return off_loop_vars_;
}

const loop::Variables& VariableSet::GetOnLoopVariables(void) const
{
  return on_loop_vars_;
}

const loop::Variables& VariableSet::GetParlLoopVariables(void) 
  const
{
  return parl_loop_vars_;
}

void VariableSet::CheckValid(void) const
{
  off_loop_vars_.CheckValid();
  on_loop_vars_.CheckValid();
  parl_loop_vars_.CheckValid();
  
  CheckVariablesRange(off_loop_vars_, on_loop_vars_);
  CheckVariablesRange(on_loop_vars_, parl_loop_vars_);
}

void VariableSet::CheckVariablesRange(const loop::Variables& upper, 
                                            const loop::Variables& lower) const
{
  CHECK(upper.GetIw() >= lower.GetIw())
    << "Upper input width is smaller than lower input width: "
    << "upper(" << upper.GetIw() << ") "
    << "lower(" << lower.GetIw() << ")";
  CHECK(upper.GetIh() >= lower.GetIh())
    << "Upper input height is smaller than lower input height: "
    << "upper(" << upper.GetIh() << ") "
    << "lower(" << lower.GetIh() << ")";
  CHECK(upper.GetIc() >= lower.GetIc())
    << "Upper input channel is smaller than lower input channel: "
    << "upper(" << upper.GetIc() << ") "
    << "lower(" << lower.GetIc() << ")";
  CHECK(upper.GetKw() >= lower.GetKw())
    << "Upper kernel width is smaller than lower kernel width: "
    << "upper(" << upper.GetKw() << ") "
    << "lower(" << lower.GetKw() << ")";
  CHECK(upper.GetKh() >= lower.GetKh())
    << "Upper kernel height is smaller than lower kernel height: "
    << "upper(" << upper.GetKh() << ") "
    << "lower(" << lower.GetKh() << ")";
  CHECK(upper.GetOw() >= lower.GetOw())
    << "Upper output width is smaller than lower output width: "
    << "upper(" << upper.GetOw() << ") "
    << "lower(" << lower.GetOw() << ")";
  CHECK(upper.GetOh() >= lower.GetOh())
    << "Upper output height is smaller than lower output height: "
    << "upper(" << upper.GetOh() << ") "
    << "lower(" << lower.GetOh() << ")";
  CHECK(upper.GetOc() >= lower.GetOc())
    << "Upper output channel is smaller than lower output channel: "
    << "upper(" << upper.GetOc() << ") "
    << "lower(" << lower.GetOc() << ")";
}

ostream& loop::operator<<(ostream& out, const loop::VariableSet& varset)
{
  out << "Off-chip Loop Variables."       << endl 
      << varset.GetOffLoopVariables()     << endl
      << "On-chip Loop Variables."        << endl
      << varset.GetOnLoopVariables()      << endl
      << "Parallelization Loop Variables."<< endl 
      << varset.GetParlLoopVariables()    << endl;
  return out;
}
