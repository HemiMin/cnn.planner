#include "analysis/data_reuse_analyzer.h"

#include <glog/logging.h>
#include <math.h>

using analysis::DataReuseAnalyzer;

using std::ceil;

int DataReuseAnalyzer::AnalyzeSpatialInputReuse(const VariableSet& varset) const
{
  int ppx; // parallel input pixel
  int num_p_ops; // parallel operations

  ppx = varset.GetPic() * varset.GetPih() * varset.GetPiw();
  num_p_ops = varset.GetPic() * varset.GetPkh() * varset.GetPkw() * 
              varset.GetPoc() * varset.GetPoh() * varset.GetPow();

  /* #region Logging */
  LOG(INFO) << "  Parallel input pixel: " << ppx;
  LOG(INFO) << "  Number of parallel operations: " << num_p_ops;
  /* #endregion */

  return ceil((double)num_p_ops / ppx);
}

int DataReuseAnalyzer::AnalyzeSpatialWeightReuse(const VariableSet& varset)const
{
  int pwt;  // parallel weight
  int num_p_ops; // parallel operations

  pwt = varset.GetPoc() * varset.GetPic() * varset.GetPkh() * varset.GetPkw();
  num_p_ops = varset.GetPic() * varset.GetPkh() * varset.GetPkw() * 
              varset.GetPoc() * varset.GetPoh() * varset.GetPow();

  /* #region Logging */
  LOG(INFO) << "  Parallel weight: " << pwt;
  LOG(INFO) << "  Number of parallel operations: " << num_p_ops;
  /* #endregion */

  return ceil((double)num_p_ops / pwt);
}

int DataReuseAnalyzer::AnalyzeTemporalInputReuse( const Location intra_oc, 
                                                  const VariableSet& varset)
                                                  const
{
  if (intra_oc == Location::INNER_MOST) {
    /* #region Loggin */
    LOG(INFO) << "Intra output channel is inner most.";
    /* #endregion */
    return varset.GetToc() / varset.GetPoc();
  } else {
    /* #region Loggin */
    LOG(INFO) << "Intra output channel is not inner most.";
    /* #endregion */
    return 1;
  }
}

int DataReuseAnalyzer::AnalyzeTemporalWeightReuse(const Location on_loop_out_map
                                                , const VariableSet& varset) 
                                                  const
{
  if (on_loop_out_map == Location::INNER_MOST) {
    /* #region Loggin */
    LOG(INFO) << "Intra output map is inner most.";
    /* #endregion */
    return  (varset.GetTow() * varset.GetToh()) / 
            (varset.GetPow() * varset.GetPoh());
  } else {
    /* #region Loggin */
    LOG(INFO) << "Intra output map is not inner most.";
    /* #endregion */
    return 1;
  }
}
