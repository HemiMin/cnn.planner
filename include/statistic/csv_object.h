#pragma once

#include <iostream>

#include "loop/variable_set.h"
#include "loop/structure.h"
#include "analysis/analysis_report.h"

#define STR_LEN 256

using std::ostream;
using loop::VariableSet;
using loop::Structure;
using analysis::AnalysisReport;

namespace statistic {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Class which contains report result to generate CSV file.
//! @details  This class is one record of table of CSV file.
//! @author   Minsu Kim
//! @date     2019-07-03
////////////////////////////////////////////////////////////////////////////////
class CsvObject
{
  public:
    //! @brief                        Constructor to generate CsvObject.
    //! @param layer_name             Record key name.
    //! @param varset                 Set of CNN parameters.
    //! @param off_strt               Inter loop structure which provides interface between off-chip and on-chip.
    //!                               CsvObject needs it in order to record stationary.
    //! @param report                 Reference to access AnalysisReport object.
    CsvObject(const char*           layer_name, 
              const VariableSet&    varset, 
              const Structure&      off_strt, 
              const AnalysisReport& report);

    //! @brief    Operator overloading
    //! @details  Operator overloading to return output stream.
    friend ostream& operator<<(ostream& out, const CsvObject& csv_obj);

  private:
    char layer_name_[STR_LEN];

    int tiw_, tih_, tic_, tkw_, tkh_, tow_, toh_, toc_;

    int s_input_reuse_;
    int s_weight_reuse_;
    int t_input_reuse_;
    int t_weight_reuse_;

    long int i_buf_size_;
    long int w_buf_size_;
    long int o_buf_size_;
    long int buf_size_;

    long int on_chip_input_load_size_;
    long int on_chip_weight_load_size_;
    long int on_chip_psum_load_store_size_;
    long int on_chip_output_store_size_;
    long int on_chip_total_access_size_;

    long int off_chip_input_load_size_;
    long int off_chip_weight_load_size_;
    long int off_chip_output_store_size_;
    long int off_chip_total_access_size_;

    double comput_roof_;
    double opt_arith_intns_;
    double arith_intns_;
    double opt_performance_;
    double attainable_performance_;
    double estimated_performance_;
    double performance_;
    int active_macs_;
    double pe_util_;

    double off_chip_acs_energy_;
    double on_chip_acs_energy_;
    double exe_energy_;
    double total_energy_;
    double power_;

    bool is_input_stationary_;
    bool is_weight_stationary_;
    bool is_output_stationary_;
};
ostream& operator<<(ostream& out, const CsvObject& csv_obj);
} // namespace statistic
