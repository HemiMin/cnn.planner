#include "analysis/analysis_report.h"

#include <glog/logging.h>
#include <iostream>

#include "general/data_type.h"

using std::endl;

using analysis::AnalysisReport;

AnalysisReport::AnalysisReport(void)
{
  comput_anlyzr_      =  unique_ptr<ComputeAnalyzer>(
                                new ComputeAnalyzer());
  data_reuse_anlyzr_  =  unique_ptr<DataReuseAnalyzer>(
                                new DataReuseAnalyzer());
  psum_anlyzr_        =  unique_ptr<PartialSumAnalyzer>(
                                new PartialSumAnalyzer());
  on_chip_acs_anlyzr_ =  unique_ptr<OnChipAccessAnalyzer>(
                                new OnChipAccessAnalyzer());
  off_chip_acs_anlyzr_=  unique_ptr<OffChipAccessAnalyzer>(
                                new OffChipAccessAnalyzer());
  roofline_anlyzr_    =  unique_ptr<RooflineAnalyzer>(
                                new RooflineAnalyzer());
  energy_anlyzr_      =  unique_ptr<EnergyAnalyzer>(
                                new EnergyAnalyzer());
}

void AnalysisReport::PreAnalyze(const VariableSet& varset,
                                const Structure& off_strt, 
                                const Structure& on_strt,
                                const Architecture& arch)
{
  /* #region Logging */
  LOG(INFO) << "Run pre-analyze.";
  /* #endregion */
  reg_size_ = comput_anlyzr_->AnalyzeRegisterSize(varset);
  num_ops_  = comput_anlyzr_->AnalyzeNumOps(varset);
  num_pe_   = arch.GetPeDim()[0][0] * arch.GetPeDim()[0][1];
  off_loop_itrs_ = comput_anlyzr_->AnalyzeOffLoopIterations(varset);
  on_loop_itrs_  = comput_anlyzr_->AnalyzeOnLoopIterations(varset);
  opt_exe_cycles_= comput_anlyzr_->AnalyzeOptExeCycles(num_ops_, num_pe_);
  estimated_exe_cycles_ =comput_anlyzr_->AnalyzeExeCycles(off_loop_itrs_, 
                                                          on_loop_itrs_);
  active_macs_ =comput_anlyzr_->AnalyzeActiveMacs(varset.GetParlLoopVariables(), 
                                                  arch.GetMacCycles());
  pe_util_ = (double)num_ops_ / estimated_exe_cycles_ / num_pe_ * 100;
  /* #region Logging */
  LOG(INFO) << "  Register size: " << reg_size_ << " Bytes";
  LOG(INFO) << "  Number of operations: " << num_ops_;
  LOG(INFO) << "  Number of PEs: " << num_pe_;
  LOG(INFO) << "  Number of inter loop iterations: " << off_loop_itrs_;
  LOG(INFO) << "  Number of intra loop iterations: " << on_loop_itrs_;
  LOG(INFO) << "  Optimal execution cycles: " << opt_exe_cycles_;
  LOG(INFO) << "  Estimated execution cycles: " << estimated_exe_cycles_;
  LOG(INFO) << "  Active MACs: " << active_macs_;
  LOG(INFO) << "  PE utilization: " << pe_util_;
  /* #endregion */
  s_input_reuse_ = data_reuse_anlyzr_->AnalyzeSpatialInputReuse(varset);
  s_weight_reuse_= data_reuse_anlyzr_->AnalyzeSpatialWeightReuse(varset);
  t_input_reuse_ = data_reuse_anlyzr_->AnalyzeTemporalInputReuse(
                                          on_strt.GetOutputChannel(), varset);
  t_weight_reuse_= data_reuse_anlyzr_->AnalyzeTemporalWeightReuse(
                                          on_strt.GetOutputMap(), varset);
  /* #region Logging */
  LOG(INFO) << "  Spatial input reuse: " << s_input_reuse_;
  CHECK(s_input_reuse_ > 0) << "Spatial input reuse error";
  LOG(INFO) << "  Spatial weight reuse: " << s_weight_reuse_;
  CHECK(s_weight_reuse_ > 0) << "Spatial weight reuse error";
  LOG(INFO) << "  Temporal input reuse: " << t_input_reuse_;
  CHECK(t_input_reuse_ > 0) << "Temporal input reuse error";
  LOG(INFO) << "  Temporal weight reuse: " << t_weight_reuse_;
  CHECK(t_weight_reuse_ > 0) << "Temporal weight reuse error";
  /* #endregion */
  i_buf_size_ = DecideInputBufferSize(varset) * sizeof(DataType);
  w_buf_size_ = DecideWeightBufferSize(varset) * sizeof(DataType);
  o_buf_size_ = psum_anlyzr_->GetPartialSumVariables( varset, 
                                                      off_strt).GetOutputSize() 
                                                      * sizeof(DataType);
  buf_size_ = i_buf_size_ + w_buf_size_ + o_buf_size_;
  /* #region Logging */
  LOG(INFO) << "  Input buffer size: " << i_buf_size_ << " Bytes";
  LOG(INFO) << "  Weight buffer size: " << w_buf_size_ << " Bytes";
  LOG(INFO) << "  Output buffer size: " << o_buf_size_ << " Bytes";
  LOG(INFO) << "  Total buffer size: " << buf_size_ << " Bytes";
  /* #endregion */
  on_chip_input_load_size_=on_chip_acs_anlyzr_->AnalyzeInputLoad( num_ops_
                                                                , s_input_reuse_
                                                                , t_input_reuse_
                                                                );
  on_chip_weight_load_size_=on_chip_acs_anlyzr_->AnalyzeWeightLoad(
                                                                num_ops_
                                                              , s_weight_reuse_
                                                              , t_weight_reuse_
                                                              );
  on_chip_psum_load_store_size_ = on_chip_acs_anlyzr_->AnalyzePartialSumAccess(
                                                        estimated_exe_cycles_, 
                                                        varset)
                                                        ;
  on_chip_output_store_size_ =  on_chip_acs_anlyzr_->AnalyzeOutputStore(varset);
  on_chip_total_access_size_ =  on_chip_input_load_size_ + 
                                on_chip_weight_load_size_ + 
                                on_chip_psum_load_store_size_ + 
                                on_chip_output_store_size_;
  /* #region Logging */
  LOG(INFO) << "  On-chip input load size: " << on_chip_input_load_size_ 
            << " Bytes";
  LOG(INFO) << "  On-chip weight load size: " << on_chip_weight_load_size_
            << " Bytes";
  LOG(INFO) << "  On-chip psum load size: " << on_chip_psum_load_store_size_ 
            << " Bytes";
  LOG(INFO) << "  On-chip output store size: " << on_chip_output_store_size_
            << "Bytes";
  LOG(INFO) << "  Total on-chip access size: " << on_chip_total_access_size_
            << " Bytes";
  /* #endregion */
  off_chip_input_load_size_=off_chip_acs_anlyzr_->AnalyzeInputLoad( varset, 
                                                                    off_strt);
  off_chip_weight_load_size_=off_chip_acs_anlyzr_->AnalyzeWeightLoad( varset, 
                                                                      off_strt);
  off_chip_output_store_size_=off_chip_acs_anlyzr_->AnalyzeOutputStore(varset,
                                                                        off_strt
                                                                      );
  off_chip_total_access_size_ = off_chip_input_load_size_ + 
                                off_chip_weight_load_size_ + 
                                off_chip_output_store_size_;
  /* #region Logging */
  LOG(INFO) << "  Off-chip input load size: " << off_chip_input_load_size_
            << " Bytes";
  LOG(INFO) << "  Off-chip weight load size: " << off_chip_weight_load_size_
            << " Bytes";
  LOG(INFO) << "  Off-chip output store size: " << off_chip_output_store_size_
            << " Bytes";
  LOG(INFO) << "  Total off-chip access size: "<< off_chip_total_access_size_ 
            << " Bytes"; 
  /* #endregion */
  encoded_psum_variables_ = psum_anlyzr_->GetEncodedPartialSumVariables(varset, 
                                                                        off_strt
                                                                        );
  /* #region Logging */
  CHECK(encoded_psum_variables_ != 0) 
    << "Encoded partial sum variables are wrong";
  LOG(INFO) << "Encoded partial sum variables: " << encoded_psum_variables_;
  /* #endregion */
}

long int AnalysisReport::DecideInputBufferSize(const VariableSet& varset) const
{
  if (varset.GetOffLoopVariables().GetInputSize() == 
      varset.GetOnLoopVariables().GetInputSize()) {
        return varset.GetOffLoopVariables().GetInputSize();
  } else {
        return varset.GetOnLoopVariables().GetInputSize() * 2;
  }
}

long int AnalysisReport::DecideWeightBufferSize(const VariableSet& varset) const
{
  if (varset.GetOffLoopVariables().GetWeightSize() ==
      varset.GetOnLoopVariables().GetWeightSize()) {
        return varset.GetOffLoopVariables().GetWeightSize();
  } else {
        return varset.GetOnLoopVariables().GetWeightSize() * 2;
  }
}

void AnalysisReport::Analyze( const VariableSet& varset, 
                              long int latency, 
                              const Architecture& arch)
{
  /* #region Logging */
  LOG(INFO) << "Analysis start.";
  /* #endregion */
  comput_roof_ = (double)num_pe_ * arch.GetFrequency();
  opt_arith_intns_=roofline_anlyzr_->AnalyzeOptArithIntensity(num_ops_,varset);
  arith_intns_=roofline_anlyzr_->AnalyzeArithIntensity(num_ops_, 
                                                  off_chip_total_access_size_);
  opt_performance_=roofline_anlyzr_->AnalyzeOptPerformance( comput_roof_,
                                                            opt_arith_intns_, 
                                                            arch.GetBandwidth()
                                                            );
  double estimd_comput_roof = arch.GetFrequency() * num_pe_ * pe_util_ / 100;
  estimated_performance_=roofline_anlyzr_->AnalyzePerformance(estimd_comput_roof
                                                            ,arith_intns_
                                                            ,arch.GetBandwidth()
                                                            );
  attainable_performance_=roofline_anlyzr_->AnalyzePerformance( comput_roof_, 
                                                                arith_intns_, 
                                                            arch.GetBandwidth()
                                                            );
  performance_ = (double)num_ops_ / latency;
  /* #region Logging */
  LOG(INFO) << "  Computation roof: " << comput_roof_ << " GMACS";
  LOG(INFO) << "  Optimal arithmetic intensity: " << opt_arith_intns_;
  LOG(INFO) << "  Arithmetic intensity: " << arith_intns_;
  LOG(INFO) << "  Optimal performance: " << opt_performance_ << " GMACS";
  LOG(INFO) << "  Attainable performance: "<<attainable_performance_<<" GMACS";
  LOG(INFO) << "  Performance: " << performance_ << " GMACS";
  /* #endregion */
  off_chip_acs_energy_ = energy_anlyzr_->AnalyzeOffChipAccessEnergy(
                                          off_chip_total_access_size_, 
                                          arch.GetOffChipEnergy());
  on_chip_acs_energy_  = energy_anlyzr_->AnalyzeOnChipAccessEnergy(
                                          on_chip_total_access_size_, 
                                          arch.GetOnChipEnergy());
  execution_energy_ = energy_anlyzr_->AnalyzeExecutionEnergy(num_ops_, 
                                                          arch.GetMacEnergy());
  total_energy_ = off_chip_acs_energy_+on_chip_acs_energy_+execution_energy_;
  power_ = energy_anlyzr_->AnalyzePower(latency, total_energy_);
  /* #region Logging */
  LOG(INFO) << "  Off-chip access energy: " << off_chip_acs_energy_ << " nJ";
  LOG(INFO) << "  On-chip access energy: " << on_chip_acs_energy_ << " nJ";
  LOG(INFO) << "  Execution energy: " << execution_energy_ << " nJ";
  LOG(INFO) << "  Total energy: " << total_energy_ << " nJ";
  LOG(INFO) << "  Power: " << power_ << " Watt";
  /* #endregion */
  CheckValidAnalysisValues();
}

int AnalysisReport::GetEncodedPsumVariables(void) const
{
  return encoded_psum_variables_;
}

int AnalysisReport::GetRegisterSize(void) const
{
  return reg_size_;
}

long int AnalysisReport::GetNumOps(void) const
{
  return num_ops_;
}

int AnalysisReport::GetNumPe(void) const
{
  return num_pe_;
}

long int AnalysisReport::GetOffLoopIterations(void) const
{
  return off_loop_itrs_;
}

long int AnalysisReport::GetOnLoopIterations(void) const
{
  return on_loop_itrs_;
}

long int AnalysisReport::GetOptExeCycles(void) const
{
  return opt_exe_cycles_;
}

long int AnalysisReport::GetEstimatedExeCycles(void) const
{
  return estimated_exe_cycles_;
}

int AnalysisReport::GetActiveMacs(void) const
{
  return active_macs_;
}

double AnalysisReport::GetPeUtilization(void) const
{
  return pe_util_;
}

int AnalysisReport::GetSpatialInputReuse(void) const
{
  return s_input_reuse_;
}

int AnalysisReport::GetSpatialWeightReuse(void) const
{
  return s_weight_reuse_;
}

int AnalysisReport::GetTemporalInputReuse(void) const
{
  return t_input_reuse_;
}

int AnalysisReport::GetTemporalWeightReuse(void) const
{
  return t_weight_reuse_;
}

long int AnalysisReport::GetInputBufferSize(void) const
{
  return i_buf_size_;
}

long int AnalysisReport::GetWeightBufferSize(void) const
{
  return w_buf_size_;
}

long int AnalysisReport::GetOutputBufferSize(void) const
{
  return o_buf_size_;
}

long int AnalysisReport::GetTotalBufferSize(void) const
{
  return buf_size_;
}

long int AnalysisReport::GetOnChipInputLoadSize(void) const
{
  return on_chip_input_load_size_;
}

long int AnalysisReport::GetOnChipWeightLoadSize(void) const
{
  return on_chip_weight_load_size_;
}

long int AnalysisReport::GetOnChipPartialSumLoadStoreSize(void) const
{
  return on_chip_psum_load_store_size_;
}

long int AnalysisReport::GetOnChipOutputStoreSize(void) const
{
  return on_chip_output_store_size_;
}

long int AnalysisReport::GetOnChipTotalAccessSize(void) const
{
  return on_chip_total_access_size_;
}

long int AnalysisReport::GetOffChipInputLoadSize(void) const
{
  return off_chip_input_load_size_;
}

long int AnalysisReport::GetOffChipWeightLoadSize(void) const
{
  return off_chip_weight_load_size_;
}

long int AnalysisReport::GetOffChipOutputStoreSize(void) const
{
  return off_chip_output_store_size_;
}

long int AnalysisReport::GetOffChipTotalAccessSize(void) const
{
  return off_chip_total_access_size_;
}

double AnalysisReport::GetComputationRoof(void) const
{
  return comput_roof_;
}

double AnalysisReport::GetOptimalArithmeticIntensity(void) const
{
  return opt_arith_intns_;
}

double AnalysisReport::GetArithmeticIntensity(void) const
{
  return arith_intns_;
}

double AnalysisReport::GetOptimalPerformance(void) const
{
  return opt_performance_;
}

double AnalysisReport::GetAttainablePerformance(void) const
{
  return attainable_performance_;
}

double AnalysisReport::GetEstimatedPerformance(void) const
{
  return estimated_performance_;
}

double AnalysisReport::GetPerformance(void) const
{
  return performance_;
}

double AnalysisReport::GetOffChipAccessEnergy(void) const
{
  return off_chip_acs_energy_;
}

double AnalysisReport::GetOnChipAccessEnergy(void) const
{
  return on_chip_acs_energy_;
}

double AnalysisReport::GetExecutionEnergy(void) const
{
  return execution_energy_;
}

double AnalysisReport::GetTotalEnergy(void) const
{
  return total_energy_;
}

double AnalysisReport::GetPower(void) const
{
  return power_;
}

ostream& AnalysisReport::PrintComputeAnalysisReport(ostream& out) const
{
  CheckValidComputeAnalysisValues();

  out << "# of PEs: "        << num_pe_               << endl
      << "register size: "   << reg_size_ << " Bytes" << endl
      << "# of Operations: " << num_ops_              << endl
      << "active MACs: "     << active_macs_          << endl
      << "PE utilization: "  << pe_util_              << endl;
  return out;
}

ostream& AnalysisReport::PrintDataReuseAnalysisReport(ostream& out) const
{
  CheckValidDataReuseAnalysisValues();

  out << "spatial input reuse: "  << s_input_reuse_  << endl
      << "spatial weight reuse: " << s_weight_reuse_ << endl
      << "temporal input reuse: " << t_input_reuse_  << endl
      << "temporal weight reuse: "<< t_weight_reuse_ << endl;
  return out;
}

ostream& AnalysisReport::PrintOnChipAccessAnalysisReport(ostream& out) const
{
  CheckValidOnChipAccessAnalysisValues();

  out << "input buffer size: "  << i_buf_size_ << " Bytes" << endl
      << "weight buffer size: " << w_buf_size_ << " Bytes" << endl
      << "output buffer size: " << o_buf_size_ << " Bytes" << endl
      << "total buffer size: "  << buf_size_   << " Bytes" << endl
      << "(input, weight, and output buffer is double buffered."
      << " And partial sum buffer is single buffered)"     << endl
      << endl
      << "input load size: " << on_chip_input_load_size_ << " Bytes"   << endl
      << "weight load size: " << on_chip_weight_load_size_ << " Bytes" << endl
      << "parital sum load and store size: " << on_chip_psum_load_store_size_ 
        << " Bytes" << endl
      << "output store size: " << on_chip_output_store_size_ << " Bytes" << endl
      << "total on-chip access size: " << on_chip_total_access_size_ << " Bytes" 
        << endl;
  return out;
}

ostream& AnalysisReport::PrintOffChipAccessAnalysisReport(ostream& out) const
{
  CheckValidOffChipAccessAnalysisValues();

  out << "input load size: " << off_chip_input_load_size_   <<" Bytes" << endl
      << "weight load size: "<< off_chip_weight_load_size_  <<" Bytes" << endl
      << "output store size: "<< off_chip_output_store_size_<<" Bytes" << endl
      << "total off-chip access size: "<<off_chip_total_access_size_<<" Bytes" 
        << endl;
  return out;
}

ostream& AnalysisReport::PrintRooflineAnalysisReport(ostream& out) const
{
  CheckValidRooflineAnalysisValues();

  out << "computation roof: "      << comput_roof_ << " GMACS"           <<endl
      << "optimal arithmetic intensity: " << opt_arith_intns_            <<endl
      << "arithmetic intensity: "  << arith_intns_                       <<endl
      << "optimal performance: "   << opt_performance_ << " GMACS"       <<endl
      << "attainable performance: "<< attainable_performance_ << " GMACS"<<endl
      << "estimated performance: " << estimated_performance_ << " GMACS" <<endl
      << "performance: "           << performance_ << " GMACS"           <<endl;
  return out;
}

ostream& AnalysisReport::PrintEnergyAnalysisReport(ostream& out) const
{
  CheckValidEnergyAnalysisValues();

  out << "off-chip access energy: " << off_chip_acs_energy_ << " nJ" << endl
      << "on-chip access energy: "  << on_chip_acs_energy_  << " nJ" << endl
      << "computation energy: "     << execution_energy_    << " nJ" << endl
      << "total energy: "           << total_energy_        << " nJ" << endl
      << "power: "                  << power_ << " Watt"             << endl;
  return out;
}

ostream& AnalysisReport::PrintAnalysisReport(ostream& out) const
{
  CheckValidAnalysisValues();

  PrintComputeAnalysisReport(out);      out << endl;
  PrintDataReuseAnalysisReport(out);    out << endl;
  PrintOnChipAccessAnalysisReport(out); out << endl;
  PrintOffChipAccessAnalysisReport(out);out << endl;
  PrintRooflineAnalysisReport(out);     out << endl;
  PrintEnergyAnalysisReport(out);       out << endl;

  return out;
}

void AnalysisReport::CheckValidComputeAnalysisValues(void) const
{
  CHECK(reg_size_ > NON_VALID) << "reg_size_ is non valid: " << reg_size_;
  CHECK(num_ops_ > NON_VALID) << "num_ops_ is non valid: " << num_ops_;
  CHECK(num_pe_ > NON_VALID) << "num_pe_ is non valid: " << num_pe_;
  CHECK(off_loop_itrs_ > NON_VALID) << "off_loop_itrs_ is non valid: "
                                    << off_loop_itrs_;
  CHECK(on_loop_itrs_ > NON_VALID)  << "on_loop_itrs_ is non valid: "
                                    << on_loop_itrs_;
  CHECK(opt_exe_cycles_ > NON_VALID)<< "opt_exe_cycles_ is non valid: "
                                    << opt_exe_cycles_;
  CHECK(estimated_exe_cycles_>NON_VALID)<<"estimated_exe_cycles_ is non valid: "
                                        << estimated_exe_cycles_;
}

void AnalysisReport::CheckValidDataReuseAnalysisValues(void) const
{
  CHECK(s_input_reuse_ > NON_VALID) << "s_input_reuse_ is non valid: " 
                                    << s_input_reuse_;
  CHECK(s_weight_reuse_ > NON_VALID)<< "s_weight_reuse_ is non valid: "
                                    << s_weight_reuse_;
  CHECK(t_input_reuse_ > NON_VALID) << "t_input_reuse_ is non valid: "
                                    << t_input_reuse_;
  CHECK(t_weight_reuse_ > NON_VALID)<< "t_weight_reuse_ is non valid: "
                                    << t_weight_reuse_;
}

void AnalysisReport::CheckValidOnChipAccessAnalysisValues(void) const
{
  CHECK(i_buf_size_ > NON_VALID) << "i_buf_size_ is non valid: " << i_buf_size_;
  CHECK(w_buf_size_ > NON_VALID) << "w_buf_size_ is non valid: " << w_buf_size_;
  CHECK(o_buf_size_ > NON_VALID) << "o_buf_size_ is non valid: " << o_buf_size_;
  CHECK(buf_size_ > NON_VALID) << "buf_size is non valid: " << buf_size_;
  CHECK(on_chip_input_load_size_ > NON_VALID)
    << "on_chip_input_load_size_ is non valid: " << on_chip_input_load_size_;
  CHECK(on_chip_weight_load_size_ > NON_VALID)
    << "on_chip_weight_load_size_ is non valid: " << on_chip_weight_load_size_;
  CHECK(on_chip_psum_load_store_size_ > NON_VALID)
    << "on_chip_psum_load_store_size_ is non valid: "
    << on_chip_psum_load_store_size_;
  CHECK(on_chip_output_store_size_ > NON_VALID)
    << "on_chip_output_store_size_ is non valid: " <<on_chip_output_store_size_;
  CHECK(on_chip_total_access_size_ > NON_VALID)
    << "on_chip_total_access_size_ is non valid: " <<on_chip_total_access_size_;
}

void AnalysisReport::CheckValidOffChipAccessAnalysisValues(void) const
{
  CHECK(off_chip_input_load_size_ > NON_VALID)
    << "off_chip_input_load_size_ is non valid: " << off_chip_input_load_size_;
  CHECK(off_chip_weight_load_size_ > NON_VALID)
    << "off_chip_weight_load_size_ is non valid: "<< off_chip_weight_load_size_;
  CHECK(off_chip_output_store_size_ > NON_VALID)
    << "off_chip_output_store_size_ is non valid: "
    << off_chip_output_store_size_;
  CHECK(off_chip_total_access_size_ > NON_VALID)
    << "off_chip_total_access_size_ is non valid: "
    << off_chip_total_access_size_;
}

void AnalysisReport::CheckValidRooflineAnalysisValues(void) const
{
  CHECK(comput_roof_ > NON_VALID) << "comput_roof_ is non valid: "
                                  << comput_roof_;
  CHECK(opt_arith_intns_ > NON_VALID) << "opt_arith_intns_ is non valid.: "
                                      << opt_arith_intns_;
  CHECK(arith_intns_ > NON_VALID) << "arith_intns_ is non valid: "
                                  << arith_intns_;
  CHECK(opt_performance_ > NON_VALID) << "opt_performance_ is non valid: "
                                      << opt_performance_;
  CHECK(attainable_performance_ > NON_VALID)
    << "attainable_performance_ is non valid: " << attainable_performance_;
  CHECK(estimated_performance_ > NON_VALID)
    << "estimated_performance_ is non valid: " << estimated_performance_;
  CHECK(performance_ > NON_VALID) << "performance_ is non valid: " 
                                  << performance_;
}

void AnalysisReport::CheckValidEnergyAnalysisValues(void) const
{
  CHECK(off_chip_acs_energy_ > NON_VALID) 
    << "off_chip_acs_energy_ is non valid: " << off_chip_acs_energy_;
  CHECK(on_chip_acs_energy_ > NON_VALID)
    << "on_chip_acs_energy_ is non valid: " << on_chip_acs_energy_;
  CHECK(execution_energy_ > NON_VALID)  << "execution_energy_ is non valid: "
                                        << execution_energy_;
  CHECK(total_energy_ > NON_VALID)  << "total_energy_ is non valid: "
                                    << total_energy_;
  CHECK(power_ > NON_VALID) << "power_ is non valid: " << power_;
}

void AnalysisReport::CheckValidAnalysisValues(void) const
{
  CheckValidComputeAnalysisValues();
  CheckValidDataReuseAnalysisValues();
  CheckValidOnChipAccessAnalysisValues();
  CheckValidOffChipAccessAnalysisValues();
  CheckValidEnergyAnalysisValues(); 
}
