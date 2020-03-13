#include "analysis/partial_sum_analyzer.h"

#include <glog/logging.h>

#include "general/data_type.h"

using analysis::PartialSumAnalyzer;

using loop::Type;
using loop::Location;

Variables PartialSumAnalyzer::GetPartialSumVariables( const VariableSet& varset, 
                                                      const Structure& off_strt)
{
  Variables psum_var;
  if (off_strt.IsFullyTiled(Type::KERNEL_MAP) &&
      off_strt.IsFullyTiled(Type::INPUT_CHANNEL)) {
    psum_var.SetOw(varset.GetTow());
    psum_var.SetOh(varset.GetToh());
    psum_var.SetOc(varset.GetToc());
  } else if (off_strt.IsFullyTiled(Type::KERNEL_MAP)) {
    if (off_strt.GetInputChannel() > 
        off_strt.GetOutputMap()) {
      psum_var.SetOw(varset.GetOw());
      psum_var.SetOh(varset.GetOh());
    } else {
      psum_var.SetOw(varset.GetTow());
      psum_var.SetOh(varset.GetToh());
    }
    if (off_strt.GetInputChannel() >
        off_strt.GetOutputChannel()) {
      psum_var.SetOc(varset.GetOc());
    } else {
      psum_var.SetOc(varset.GetToc());
    }
  } else if (off_strt.IsFullyTiled(Type::INPUT_CHANNEL)) {
    if (off_strt.GetKernelMap() >
        off_strt.GetOutputMap()) {
      psum_var.SetOw(varset.GetOw());
      psum_var.SetOh(varset.GetOh());
    } else {
      psum_var.SetOw(varset.GetTow());
      psum_var.SetOh(varset.GetToh());
    }
    if (off_strt.GetKernelMap() >
        off_strt.GetOutputChannel()) {
      psum_var.SetOc(varset.GetOc());
    } else {
      psum_var.SetOc(varset.GetToc());
    }
  } else {
    if (off_strt.GetKernelMap() > 
        off_strt.GetOutputMap() &&
        off_strt.GetInputChannel() >
        off_strt.GetOutputMap()) {
      psum_var.SetOw(varset.GetOw());
      psum_var.SetOh(varset.GetOh());
    } else {
      psum_var.SetOw(varset.GetTow());
      psum_var.SetOh(varset.GetToh());
    }
    if (off_strt.GetKernelMap() >
        off_strt.GetOutputChannel() &&
        off_strt.GetKernelMap() >
        off_strt.GetOutputChannel()) {
      psum_var.SetOc(varset.GetOc());
    } else {
      psum_var.SetOc(varset.GetToc());
    }
  }

  /* #region Logging */
  if (psum_var.GetOw() == varset.GetOw() &&
      psum_var.GetOh() == varset.GetOh()) {
    LOG(INFO) << "Partial sum output map is fully tiled.";
    LOG(INFO) << "  Partial sum output width: " << psum_var.GetOw();
    LOG(INFO) << "  Partial sum output height: " << psum_var.GetOh();
  } else {
    LOG(INFO) << "Partial sum output map is not fully tiled.";
    LOG(INFO) << "  Partial sum output width: " << psum_var.GetOw();
    LOG(INFO) << "  Partial sum output height: " << psum_var.GetOh();
  }

  if (psum_var.GetOc() == varset.GetOc()) {
    LOG(INFO) << "Partial sum output channel is fully tiled.";
    LOG(INFO) << "Partial sum output channel: " << psum_var.GetOc();
  } else {
    LOG(INFO) << "Partial sum output channel is not fully tiled.";
    LOG(INFO) << "Partial sum output channel: " << psum_var.GetOc();
  }
  /* #endregion */

  return psum_var;
}

int PartialSumAnalyzer::GetEncodedPartialSumVariables(const VariableSet& varset, 
                                                      const Structure& off_strt) 
{
  return EncodePartialSumVariables(GetPartialSumVariables(varset, off_strt));
}

int PartialSumAnalyzer::EncodePartialSumVariables(const Variables& psum_var) 
{
  return  psum_var.GetOw() << (HEIGHT_BIT + CHANNEL_BIT) |
          psum_var.GetOh() << CHANNEL_BIT |
          psum_var.GetOc();
}

Variables PartialSumAnalyzer::DecodePartialSumVariables(int en_psum_var)
{
  Variables psum_var;

  psum_var.SetOc(en_psum_var & CHANNEL_BIT_MASK);
  psum_var.SetOh((en_psum_var >> CHANNEL_BIT) & HEIGHT_BIT_MASK);
  psum_var.SetOw((en_psum_var >> (HEIGHT_BIT + CHANNEL_BIT)) & WIDTH_BIT_MASK);
  
  return psum_var;
}