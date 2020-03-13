#include "loop/cnn_loop.h"

using loop::CnnLoop;

CnnLoop::CnnLoop(void)
{
  varset_   = unique_ptr<VariableSet>(new VariableSet());
  off_strt_ = unique_ptr<Structure>(new Structure());
  on_strt_  = unique_ptr<Structure>(new Structure());
}

CnnLoop::CnnLoop(const Parameter& param)
{
  varset_   = unique_ptr<VariableSet>(new VariableSet(param));
  off_strt_ = unique_ptr<Structure>(new Structure());
  on_strt_  = unique_ptr<Structure>(new Structure());
}

CnnLoop::CnnLoop(const CnnLoop& loop)
{
  // Deep copy.
  varset_   =unique_ptr<VariableSet>(new VariableSet(loop.GetVariableSet()));
  off_strt_ =unique_ptr<Structure>(new Structure(loop.GetOffStructure()));
  on_strt_  =unique_ptr<Structure>(new Structure(loop.GetOnStructure()));
}

void CnnLoop::SetVariableSet(VariableSet* varset)
{
  varset_.reset(varset);
}

void CnnLoop::SetOffStructure(Structure* off_strt)
{
  off_strt_.reset(off_strt);
}

void CnnLoop::SetOnStructure(Structure* on_strt)
{
  on_strt_.reset(on_strt);
}

const VariableSet& CnnLoop::GetVariableSet(void) const
{
  return *varset_;
}

const Structure& CnnLoop::GetOffStructure(void) const
{
  return *off_strt_;
}

const Structure& CnnLoop::GetOnStructure(void) const
{
  return *on_strt_;
}

void CnnLoop::MoveFullyTiledToInnerMost(void)
{
  if (varset_->GetOc() == varset_->GetToc()) {
    off_strt_->MoveToInnerMost(OUTPUT_CHANNEL);
    off_strt_->SetOcFullyTiled();
  }
  if (varset_->GetIc() == varset_->GetTic()) {
    off_strt_->MoveToInnerMost(INPUT_CHANNEL);
    off_strt_->SetIcFullyTiled();
  }
  //NOTE (MinsuKim): OW stands for output map in this case.
  if (varset_->GetOw() == varset_->GetTow() &&
      varset_->GetOh() == varset_->GetToh()) {
    off_strt_->MoveToInnerMost(OUTPUT_MAP);
    off_strt_->SetOmFullyTiled();
  }
  //NOTE (MinsuKim): KW stands for kernel map in this
  if (varset_->GetKw() == varset_->GetTkw() &&
      varset_->GetKh() == varset_->GetTkh()) {
    off_strt_->MoveToInnerMost(KERNEL_MAP);
    off_strt_->SetKmFullyTiled();
  }
}

void CnnLoop::CheckValid(void) const
{
  varset_->CheckValid();
  off_strt_->CheckTypeValid();
  off_strt_->CheckLocationValid();
  on_strt_->CheckTypeValid();
  on_strt_->CheckLocationValid();
}
