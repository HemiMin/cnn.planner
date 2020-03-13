#include "analysis/roofline_analyzer.h"

#include <glog/logging.h>
#include <algorithm>

#include "general/data_type.h"

using analysis::RooflineAnalyzer;

using std::min;

double RooflineAnalyzer::AnalyzeArithIntensity( const long int num_ops, 
                                                const long int data_transfer) 
                                                const
{
  return (double)num_ops / (double)data_transfer;
}

double RooflineAnalyzer::AnalyzeOptArithIntensity(const long int num_ops, 
                                                  const VariableSet& varset) 
                                                  const
{
  long int input_size  =  varset.GetOffLoopVariables().GetInputSize() *
                          sizeof(DataType);
  long int weight_size =  varset.GetOffLoopVariables().GetWeightSize() *
                          sizeof(DataType);
  long int output_size =  varset.GetOffLoopVariables().GetOutputSize() *
                          sizeof(DataType);
  int data_transfer = input_size + weight_size + output_size;
  /* #region Logging */
  LOG(INFO) << "  Input size: " << input_size << " Bytes";
  LOG(INFO) << "  Weight size: " << weight_size << " Bytes";
  LOG(INFO) << "  Output size: " << output_size<< " Bytes";
  LOG(INFO) << "  Total data transfer: " << data_transfer << " Bytes";
  /* #endregion */
  return (double)num_ops / data_transfer;
}

double RooflineAnalyzer::AnalyzeOptPerformance( const double comput_roof, 
                                                const double opt_arith_intns, 
                                                const double bandwidth) const
{
  return min(comput_roof, opt_arith_intns*bandwidth);
}

double RooflineAnalyzer::AnalyzePerformance(const double comput_roof, 
                                            const double arith_intns, 
                                            const double bandwidth) const
{
  return min(comput_roof, arith_intns*bandwidth);
}
