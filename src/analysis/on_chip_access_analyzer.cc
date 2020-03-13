#include "analysis/on_chip_access_analyzer.h"

using analysis::OnChipAccessAnalyzer;

long int OnChipAccessAnalyzer::AnalyzeWeightLoad( const long int num_ops, 
                                                  const int s_weight_reuse, 
                                                  const int t_weight_reuse)const
{
  return num_ops / s_weight_reuse / t_weight_reuse;
}

long int OnChipAccessAnalyzer::AnalyzeInputLoad(const long int num_ops, 
                                                const int s_input_reuse, 
                                                const int t_input_reuse) const
{
  return num_ops / s_input_reuse / t_input_reuse;
}

long int OnChipAccessAnalyzer::AnalyzePartialSumAccess(const long int exe_cycles
                                                      ,const VariableSet& varset
                                                      ) const
{
  return 2 * exe_cycles * varset.GetPoc() * varset.GetPoh() * varset.GetPow();
}

long int OnChipAccessAnalyzer::AnalyzeOutputStore(const VariableSet& varset) 
  const
{
  return varset.GetOc() * varset.GetOh() * varset.GetOw();
}

long int OnChipAccessAnalyzer::AnalyzeTotalAccess(const long int input_load, 
                                                  const long int weight_load,
                                                  const long int output_store, 
                                                  const long int p_sum_access) 
                                                  const
{
  return input_load + weight_load + output_store + p_sum_access;
}
