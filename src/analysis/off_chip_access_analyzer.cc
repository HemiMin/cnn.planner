#include "analysis/off_chip_access_analyzer.h"

#include <glog/logging.h>
#include <cmath>

#include "general/data_type.h"

using analysis::OffChipAccessAnalyzer;

using std::ceil;

long int OffChipAccessAnalyzer::AnalyzeInputLoad( const VariableSet& varset, 
                                                  const Structure& off_strt) 
                                                  const
{
  long int input_size = varset.GetOffLoopVariables().GetInputSize() *
                        sizeof(DataType);
  /* #region Logging */
  LOG(INFO) << "  Input size: " << input_size << " Bytes";
  /* #endregion */
  return input_size * AnalyzeInputReload(varset, off_strt);
}

int OffChipAccessAnalyzer::AnalyzeInputReload(const VariableSet& varset, 
                                              const Structure& off_strt) const
{
  if (off_strt.IsInputStationary()) {
    /* #region Logging */
    LOG(INFO) << "Input reload case 1 (Input Stationary).";
    /* #endregion */
    return 1;
  } else {
    /* #region Logging */
    LOG(INFO) << "Input reload case 2 (Not Input Stationary).";
    /* #endregion */
    return  ceil((double)varset.GetOc() / varset.GetToc()) *
            ceil((double)varset.GetKw() / varset.GetTkw()) *
            ceil((double)varset.GetKh() / varset.GetTkh());
  }
}

long int OffChipAccessAnalyzer::AnalyzeOutputStore( const VariableSet& varset,
                                                    const Structure& off_strt) 
  const
{
  return  varset.GetOffLoopVariables().GetOutputSize() * sizeof(DataType) *
          (2*AnalyzePartialSumReload(varset, off_strt) + 1);
}

long int OffChipAccessAnalyzer::AnalyzeWeightLoad(const VariableSet& varset, 
                                                  const Structure& off_strt) 
                                                  const
{
  long int weight_size =  varset.GetOffLoopVariables().GetWeightSize() *
                          sizeof(DataType);
  /* #region Logging */
  LOG(INFO) << "  Weight size: " << weight_size << " Bytes";
  /* #endregion */
  return weight_size * AnalyzeWeightReload(varset, off_strt);
}

int OffChipAccessAnalyzer::AnalyzeWeightReload( const VariableSet& varset,
                                                const Structure& off_strt) const
{
  if (off_strt.IsWeightStationary()) {
    /* #region Logging */
    LOG(INFO) << "Weight reload case 1 (Weight Stationary).";
    /* #endregion */
    return 1;
  } else {
    /* #region Logging */
    LOG(INFO) << "Weight reload case 2 (Not Weight Stationary).";
    /* #endregion */
    return  ceil((double)varset.GetOw() / varset.GetTow()) *
            ceil((double)varset.GetOh() / varset.GetToh());
  }
}

int OffChipAccessAnalyzer::AnalyzePartialSumReload( const VariableSet& varset,
                                                    const Structure& off_strt)
                                                    const
{
  if (off_strt.IsOutputStationary()) {
    /* #region Logging */
    LOG(INFO) << "Partial Sum reload case 1 (Output Stationary).";
    /* #endregion */
    return 0;
  } else {
    /* #region Logging */
    LOG(INFO) << "Partial Sum reload case 2 (Not Output Stationary).";
    /* #endregion */
    return  ceil((double)varset.GetKw() / varset.GetTkw()) *
            ceil((double)varset.GetKh() / varset.GetTkh()) *
            ceil((double)varset.GetIc() / varset.GetTic()) - 1;
  }
}
