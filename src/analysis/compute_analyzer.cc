#include "analysis/compute_analyzer.h"

#include <cmath>

#include "general/data_type.h"

using analysis::ComputeAnalyzer;

using std::ceil;

int ComputeAnalyzer::AnalyzeRegisterSize(const VariableSet& varset) const
{
  return (
          varset.GetPiw()*varset.GetPih()*varset.GetPic() +
          varset.GetPkw()*varset.GetPkh()*varset.GetPic()*varset.GetPoc() +
          varset.GetPow()*varset.GetPoh()*varset.GetPoc()
         ) * sizeof(DataType);
}

long int ComputeAnalyzer::AnalyzeNumOps(const VariableSet& varset) const
{
  return  (long int)varset.GetKw()*varset.GetKh()*varset.GetIc()* 
                    varset.GetOw()*varset.GetOh()*varset.GetOc();
}

long int ComputeAnalyzer::AnalyzeNumTileOps(const VariableSet& varset) const
{
  return  (long int)varset.GetTkw()*varset.GetTkh()*varset.GetTic()* 
                    varset.GetTow()*varset.GetToh()*varset.GetToc();
}

long int ComputeAnalyzer::AnalyzeOptExeCycles(const long int num_ops, 
                                              const int num_pe) const
{
  return (long int)ceil((double)num_ops / num_pe);
}

long int ComputeAnalyzer::AnalyzeOffLoopIterations(const VariableSet& varset) 
  const
{
  return (long int)(
      ceil((double)varset.GetOc() / varset.GetToc()) *
      ceil((double)varset.GetOh() / varset.GetToh()) * 
      ceil((double)varset.GetOw() / varset.GetTow()) *
      ceil((double)varset.GetIc() / varset.GetTic()) *
      ceil((double)varset.GetKh() / varset.GetTkh()) * 
      ceil((double)varset.GetKw() / varset.GetTkw())
      );
}

long int ComputeAnalyzer::AnalyzeOnLoopIterations(const VariableSet& varset) 
  const
{
  return (long int)(
      ceil((double)varset.GetToc() / varset.GetPoc()) *
      ceil((double)varset.GetToh() / varset.GetPoh()) *
      ceil((double)varset.GetTow() / varset.GetPow()) *
      ceil((double)varset.GetTic() / varset.GetPic()) *
      ceil((double)varset.GetTkh() / varset.GetPkh()) *
      ceil((double)varset.GetTkw() / varset.GetPkw())
      );
}

long int ComputeAnalyzer::AnalyzeExeCycles( const long int off_loop_itr, 
                                            const long int on_loop_itr) const
{
  return off_loop_itr * on_loop_itr;
}

int ComputeAnalyzer::AnalyzeActiveMacs( const Variables& parl_vars,
                                        int mac_cycles) const
{
  return  parl_vars.GetKw() * parl_vars.GetKh() * parl_vars.GetIc() *
          parl_vars.GetOw() * parl_vars.GetOh() * parl_vars.GetOc() /
          mac_cycles;
}