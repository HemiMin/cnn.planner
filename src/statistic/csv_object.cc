#include "statistic/csv_object.h"

#include <string.h>
#include <glog/logging.h>

using statistic::CsvObject;

using std::endl;

CsvObject::CsvObject( const char* layer_name, 
                      const VariableSet& varset, 
                      const Structure& off_strt, 
                      const AnalysisReport& report)
{
  strncpy(layer_name_, layer_name, STR_LEN);

  tiw_ = varset.GetTiw();
  tih_ = varset.GetTih();
  tic_ = varset.GetTic();
  tkw_ = varset.GetTkw();
  tkh_ = varset.GetTkh();
  tow_ = varset.GetTow();
  toh_ = varset.GetToh();
  toc_ = varset.GetToc();

  s_input_reuse_ = report.GetSpatialInputReuse();
  s_weight_reuse_= report.GetSpatialWeightReuse();
  t_input_reuse_ = report.GetTemporalInputReuse();
  t_weight_reuse_= report.GetTemporalWeightReuse();

  i_buf_size_ = report.GetInputBufferSize();
  w_buf_size_ = report.GetWeightBufferSize();
  o_buf_size_ = report.GetOutputBufferSize();
  buf_size_   = report.GetTotalBufferSize();

  on_chip_input_load_size_      = report.GetOnChipInputLoadSize();
  on_chip_weight_load_size_     = report.GetOnChipWeightLoadSize();
  on_chip_psum_load_store_size_ = report.GetOnChipPartialSumLoadStoreSize();
  on_chip_output_store_size_    = report.GetOnChipOutputStoreSize();
  on_chip_total_access_size_    = report.GetOnChipTotalAccessSize();

  off_chip_input_load_size_   = report.GetOffChipInputLoadSize();
  off_chip_weight_load_size_  = report.GetOffChipWeightLoadSize();
  off_chip_output_store_size_ = report.GetOffChipOutputStoreSize();
  off_chip_total_access_size_ = report.GetOffChipTotalAccessSize();

  comput_roof_            = report.GetComputationRoof();
  opt_arith_intns_        = report.GetOptimalArithmeticIntensity();
  arith_intns_            = report.GetArithmeticIntensity();
  opt_performance_        = report.GetOptimalPerformance();
  attainable_performance_ = report.GetAttainablePerformance();
  estimated_performance_  = report.GetEstimatedPerformance();
  performance_            = report.GetPerformance();
  active_macs_            = report.GetActiveMacs();
  pe_util_                = report.GetPeUtilization();

  off_chip_acs_energy_= report.GetOffChipAccessEnergy();
  on_chip_acs_energy_ = report.GetOnChipAccessEnergy();
  exe_energy_         = report.GetExecutionEnergy();
  total_energy_       = report.GetTotalEnergy();
  power_              = report.GetPower();

  is_input_stationary_  = off_strt.IsInputStationary();
  is_weight_stationary_ = off_strt.IsWeightStationary();
  is_output_stationary_ = off_strt.IsOutputStationary();

  /* #region Logging */
  LOG(INFO) << "Generate CSV object.";

  LOG(INFO) << "  Test case: " << layer_name_;

  LOG(INFO) << "  tile input width:"    << tiw_;
  LOG(INFO) << "  tile input height: "  << tih_;
  LOG(INFO) << "  tile input channel: " << tic_;
  LOG(INFO) << "  tile kernel width: "  << tkw_;
  LOG(INFO) << "  tile kernel height: " << tkh_;
  LOG(INFO) << "  tile output width: "  << tow_;
  LOG(INFO) << "  tile output height: " << toh_;
  LOG(INFO) << "  tile output channel: "<< toc_;

  LOG(INFO) << "  spatial input reuse: "   << s_input_reuse_;
  LOG(INFO) << "  spatial weight reuse: "  << s_weight_reuse_;
  LOG(INFO) << "  temporal input reuse: "  << t_input_reuse_;
  LOG(INFO) << "  temporal weight reuse: " << t_weight_reuse_;

  LOG(INFO) << "  input buffer size: "  << i_buf_size_ << " Bytes";
  LOG(INFO) << "  weight buffer size: " << w_buf_size_ << " Bytes";
  LOG(INFO) << "  output buffer size: " << o_buf_size_ << " Bytes";
  LOG(INFO) << "  total buffer size: "  << buf_size_   << " Bytes";

  LOG(INFO) << "  on-chip input load size: " 
            << on_chip_input_load_size_ << " Bytes";
  LOG(INFO) << "  on-chip weight load size: " 
            << on_chip_weight_load_size_<< " Bytes";
  LOG(INFO) << "  on-chip psum load store size: " 
            << on_chip_psum_load_store_size_ << " Bytes";
  LOG(INFO) << "  on-chip output store size: " 
            << on_chip_output_store_size_ << " Bytes";
  LOG(INFO) << "  on-chip total access size: " 
            << on_chip_total_access_size_ << " Bytes";

  LOG(INFO) << "  off-chip input load size:" 
            << off_chip_input_load_size_ << " Bytes";
  LOG(INFO) << "  off-chip weight load size: " 
            << off_chip_weight_load_size_ << " Bytes";
  LOG(INFO) << "  off-chip output store size: " 
            << off_chip_output_store_size_ << " Bytes";
  LOG(INFO) << "  off-chip total access size: " 
            << off_chip_total_access_size_ << " Bytes";

  LOG(INFO) << "  computation roof: " << comput_roof_ << " GMACS";
  LOG(INFO) << "  optimal arithmetic intensity: " << opt_arith_intns_;
  LOG(INFO) << "  arithmetic intensity: " << arith_intns_;
  LOG(INFO) << "  optimal performance: " << opt_performance_ << " GMACS";
  LOG(INFO) << "  attainable performance: "<<attainable_performance_<< " GMACS";
  LOG(INFO) << "  performance: " << performance_ << " GMACS";
  LOG(INFO) << "  active macs: " << active_macs_;
  LOG(INFO) << "  PE utilization: " << pe_util_;

  LOG(INFO) << "  off-chip access energy: " << off_chip_acs_energy_ << " nJ";
  LOG(INFO) << "  on-chip access energy: " << on_chip_acs_energy_ << " nJ";
  LOG(INFO) << "  execution energy: " << exe_energy_ << " nJ";
  LOG(INFO) << "  total energy: " << total_energy_ << " nJ";
  LOG(INFO) << "  power: " << power_ << " Watt";
  /* #endregion */
}

ostream& statistic::operator<<(ostream& out, const CsvObject& csv_obj)
{
  /* #region Logging */
  LOG(INFO) << "Dump CSV object." << endl;
  /* #endregion */
  out << csv_obj.layer_name_ << ","
      << csv_obj.tiw_ << ","
      << csv_obj.tih_ << ","
      << csv_obj.tic_ << ","
      << csv_obj.tkw_ << ","
      << csv_obj.tkh_ << ","
      << csv_obj.tow_ << ","
      << csv_obj.toh_ << ","
      << csv_obj.toc_ << ","
      << csv_obj.s_input_reuse_ << ","
      << csv_obj.s_weight_reuse_<< ","
      << csv_obj.t_input_reuse_ << ","
      << csv_obj.t_weight_reuse_<< ","
      << csv_obj.i_buf_size_    << ","
      << csv_obj.w_buf_size_    << ","
      << csv_obj.o_buf_size_    << ","
      << csv_obj.buf_size_      << ","
      << csv_obj.on_chip_input_load_size_     << ","
      << csv_obj.on_chip_weight_load_size_    << ","
      << csv_obj.on_chip_psum_load_store_size_<< ","
      << csv_obj.on_chip_output_store_size_   << ","
      << csv_obj.on_chip_total_access_size_   << ","
      << csv_obj.off_chip_input_load_size_    << ","
      << csv_obj.off_chip_weight_load_size_   << ","
      << csv_obj.off_chip_output_store_size_  << ","
      << csv_obj.off_chip_total_access_size_  << ","
      << csv_obj.comput_roof_     << ","
      << csv_obj.opt_arith_intns_ << ","
      << csv_obj.arith_intns_     << ","
      << csv_obj.opt_performance_ << ","
      << csv_obj.attainable_performance_<< ","
      << csv_obj.estimated_performance_ << ","
      << csv_obj.performance_ << ","
      << csv_obj.active_macs_ << ","
      << csv_obj.pe_util_     << ","
      << csv_obj.off_chip_acs_energy_<< ","
      << csv_obj.on_chip_acs_energy_ << ","
      << csv_obj.exe_energy_  << ","
      << csv_obj.total_energy_<< ","
      << csv_obj.power_ << ",";
  if (csv_obj.is_input_stationary_)  out << "TRUE" << ",";
  else                               out << "FALSE"<< ",";
  if (csv_obj.is_weight_stationary_) out << "TRUE" << ",";
  else                               out << "FALSE"<< ",";
  if (csv_obj.is_output_stationary_) out << "TRUE" << ",";
  else                               out << "FALSE"<< ",";
  out << endl;

  return out;
}
