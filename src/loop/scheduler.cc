#include "loop/scheduler.h"

#include <glog/logging.h>
#include <float.h>
#include <assert.h>
#include <algorithm>
#include <memory>

#include "loop/variable_set.h"
#include "general/data_type.h"
#include "general/utils.h"

using loop::Scheduler;

using std::min;
using std::max;
using std::unique_ptr;

using loop::VariableSet;
using loop::Stationary;
using arch::DataDimension;

Scheduler::Scheduler(void)
{
  num_threads_ = thread::hardware_concurrency();
  search_threads_ = new thread[num_threads_];
  /* #region Logging */
  LOG(INFO) << "LoopScheduler is constructed.";
  LOG(INFO) << "  The number of threads: " << num_threads_;
  /* #endregion */
}

Scheduler::~Scheduler(void)
{
  delete search_threads_;
}

void Scheduler::StartProgress(void)
{
  progress_ = 0;
  progress_bar_.progress(progress_, total_itr_);
}

void Scheduler::IncreaseProgress(int interval)
{
  mtx_lock_.lock();
  progress_ += interval;
  progress_bar_.progress(progress_, total_itr_);
  mtx_lock_.unlock();
}

void Scheduler::FinishProgress(void)
{
  progress_bar_.finish();
}

VariableSet* Scheduler::LoopInitializing( const VariableSet& varset, 
                                          const Architecture& arch)
{
  VariableSet* new_varset = new VariableSet(varset);
  //REVIEW (MinsuKim): I don't know the reason yet but if the kernel is full, the result is also good.
  new_varset->SetTkw(varset.GetKw());
  new_varset->SetTkh(varset.GetKh());
  new_varset->SetTiw(varset.GetIw());
  new_varset->SetTih(varset.GetTkh());
  new_varset->SetTic(1);
  new_varset->SetTow(varset.GetOw());
  new_varset->SetToh(1);
  new_varset->SetToc(1);
  // PE Utilization Optimizing
  const int row = 0, col = 1;
  for (int row_col = row ; row_col <= col ; row_col++) {
    vector<DataDimension> pe_strt = arch.GetPeStructure()[row_col];
    int len = arch.GetPeDim()[0][row_col];
    for (DataDimension row_col_dim : pe_strt) {
      switch (row_col_dim) {
        case DataDimension::IC:
          new_varset->SetTic(min(len, varset.GetIc()));
          len /= new_varset->GetTic();
          break;
        case DataDimension::OC:
          new_varset->SetToc(min(len, varset.GetOc()));
          len /= new_varset->GetToc();
          break;
        case DataDimension::OW:
          new_varset->SetTow(min(len, varset.GetOw()));
          new_varset->SetTiw(CalcInputLength(new_varset->GetTow()));
          len /= new_varset->GetTow();
          break;
        case DataDimension::OH:
          new_varset->SetToh(min(len, varset.GetOh()));
          new_varset->SetTih(CalcInputLength(new_varset->GetToh()));
          len /= new_varset->GetToh();
          break;
        case DataDimension::IW:
          new_varset->SetTiw(min(len, varset.GetIw()));
          new_varset->SetTow(CalcOutputLength(new_varset->GetTiw()));
          len /= new_varset->GetTiw();
          break;
        case DataDimension::IH:
          new_varset->SetTih(min(len, varset.GetIh()));
          new_varset->SetToh(CalcOutputLength(new_varset->GetToh()));
          len /= new_varset->GetTih();
          break;
        case DataDimension::KW:
        case DataDimension::KH:
        default: continue;
      }
    }
  }
  // At least, input width and height is longer than kernel.
  new_varset->SetTiw(max(new_varset->GetTkw(), new_varset->GetTiw()));
  new_varset->SetTih(max(new_varset->GetTkh(), new_varset->GetTih()));

  return new_varset;
}

CnnLoop* Scheduler::LoopElimination(const CnnLoop& loop, 
                                    const Architecture& arch)
{
  /* #region Fully Tiled Dimension 4 */
  CnnLoop* full_tiling = new CnnLoop(loop); // deep copy using copy constructor.
  VariableSet* full_varset = new VariableSet(full_tiling->GetVariableSet());

  full_varset->SetOnLoopVariables(full_varset->GetOffLoopVariables());
  if (!IsMemorySizeOverflow(*full_varset, arch)) {
    Structure* default_strt = new Structure();
    default_strt->SetOutputStationary(); //REVIEW (MinsuKim): Meaningless, just default.

    full_tiling->SetVariableSet(full_varset);
    full_tiling->SetOffStructure(default_strt);
    return full_tiling;
  }
  /* #endregion */
  /* #region Fully Tiled Dimension 3 */
  vector<vector<DataDimension>> subgroup3 = {
    { DataDimension::KW, DataDimension::OW, DataDimension::IC },
    { DataDimension::KW, DataDimension::OW, DataDimension::OC },
    { DataDimension::KW, DataDimension::IC, DataDimension::OC },
    { DataDimension::OW, DataDimension::IC, DataDimension::OC }
  };
  vector<vector<DataDimension>> subgroup2 = {
    { DataDimension::KW, DataDimension::OW },
    { DataDimension::KW, DataDimension::IC },
    { DataDimension::KW, DataDimension::OC },
    { DataDimension::OW, DataDimension::IC },
    { DataDimension::OW, DataDimension::OC },
    { DataDimension::IC, DataDimension::OC }
  };

  CnnLoop* best_loop = nullptr;

  for (size_t i = 0 ; i < subgroup3.size() ; i++) {
    vector<DataDimension> subgroup = subgroup3[i];
    best_loop = new CnnLoop(loop); // deep copy.
    VariableSet* best_varset = new VariableSet(best_loop->GetVariableSet());
    /* #region Make 3 of dimension fully tiled. */
    for (DataDimension is_fully : subgroup) {
      switch (is_fully) {
        case DataDimension::KW: //NOTE (MinsuKim): KW stands for kernel map in this case.
          best_varset->SetTkw(best_varset->GetKw());
          best_varset->SetTkh(best_varset->GetKh());
          break;
        case DataDimension::OW: //NOTE (MinsuKim): OW stands for output map in this case.
          best_varset->SetTow(best_varset->GetOw());
          best_varset->SetToh(best_varset->GetOh());
          best_varset->SetTiw(best_varset->GetIw());
          best_varset->SetTih(best_varset->GetIh());
          break;
        case DataDimension::IC:
          best_varset->SetTic(best_varset->GetIc());
          break;
        case DataDimension::OC:
          best_varset->SetToc(best_varset->GetOc());
          break;
        default:
          LOG(ERROR) << "Invalid data dimension: " << is_fully;
      }
    }
    /* #endregion */
    // Immediately allow if the tmp loop is not memory overflow.
    if (!IsMemorySizeOverflow(*best_varset, arch)) {
      Structure* default_strt = new Structure(best_loop->GetOffStructure());
      default_strt->SetOutputStationary(); //REVIEW (MinsuKim): Meaningless, just default.

      best_loop->SetVariableSet(best_varset);
      best_loop->SetOffStructure(default_strt);
      return best_loop;
    } else { // This best loop is not fit in on-chip memory
      delete best_loop;
    }
  }
  /* #endregion */
  /* #region Fully Tiled Dimension 2 */
  int best_elimination_cnt = -1; // how many iterations are eliminated?
  best_loop = new CnnLoop(loop);
  for (size_t i = 0 ; i < subgroup2.size() ; i++) {
    vector<DataDimension> subgroup = subgroup2[i];
    CnnLoop* itr_loop = new CnnLoop(loop);
    VariableSet* itr_varset = new VariableSet(itr_loop->GetVariableSet());
    int itr_elimination_cnt = 1;
    /* #region Make 2 of dimension fully tiled. */
    for (DataDimension is_fully : subgroup) {
      switch (is_fully) {
        case DataDimension::KW: //NOTE (MinsuKim): KW stands for kernel map in this case.
          itr_varset->SetTkw(itr_varset->GetKw());
          itr_varset->SetTkh(itr_varset->GetKh());
          itr_elimination_cnt *= itr_varset->GetKw() * itr_varset->GetKh();
          break;
        case DataDimension::OW: //NOTE (MinsuKim): OW stands for output map in this case.
          itr_varset->SetTow(itr_varset->GetOw());
          itr_varset->SetToh(itr_varset->GetOh());
          itr_varset->SetTiw(itr_varset->GetIw());
          itr_varset->SetTih(itr_varset->GetIh());
          itr_elimination_cnt *= itr_varset->GetOw() * itr_varset->GetOh();
          break;
        case DataDimension::IC:
          itr_varset->SetTic(itr_varset->GetIc());
          itr_elimination_cnt *= itr_varset->GetIc();
          break;
        case DataDimension::OC:
          itr_varset->SetToc(itr_varset->GetOc());
          itr_elimination_cnt *= itr_varset->GetOc();
          break;
        default:
          LOG(ERROR) << "Invalid data dimension: " << is_fully;
      }
    }
    /* #endregion */
    // Immediately allow if the tmp loop is not memory overflow.
    if (!IsMemorySizeOverflow(*itr_varset, arch)) {
      if (best_elimination_cnt < itr_elimination_cnt) {
        best_elimination_cnt = itr_elimination_cnt;
        delete best_loop; // release previous best loop.
        itr_loop->SetVariableSet(itr_varset);
        best_loop = itr_loop;
      }
    }
  }
  /* #region Logging */
  if (best_elimination_cnt == -1) {
    LOG(WARNING) << "Cannot find best tiling case in loop elimination.";
  }

  return best_loop;
  /* #endregion */
  /* #endregion */
}

pair<CnnLoop*, Stationary> Scheduler::LoopInterchange(const CnnLoop& loop)
{
  CnnLoop* new_loop = new CnnLoop(loop);
  Structure* new_strt = new Structure(new_loop->GetOffStructure());
  Stationary s = Stationary::OUTPUT; // Just initialize.

  const VariableSet& varset = new_loop->GetVariableSet();
  vector<pair<DataDimension, int>> sorted_by_itr_size = {
    { 
      DataDimension::KW,
      ceil((double)varset.GetKw() / varset.GetTkw()) *
      ceil((double)varset.GetKh() / varset.GetTkh())
    }, {
      DataDimension::IC,
      ceil((double)varset.GetIc() / varset.GetTic())
    }, {
      DataDimension::OW,
      ceil((double)varset.GetOw() / varset.GetTow()) *
      ceil((double)varset.GetOh() / varset.GetToh())
    }, {
      DataDimension::OC,
      ceil((double)varset.GetOc() / varset.GetToc())
    }
  };
  sort(sorted_by_itr_size.begin(), sorted_by_itr_size.end(),
    [](pair<DataDimension, int>& left, pair<DataDimension, int>& right)->bool {
      return left.second > right.second; // Sort by descending.
    }
  );
  DataDimension biggest = sorted_by_itr_size[0].first;
  // Put the biggest iteration innermost.
  switch (biggest) {
    case DataDimension::KW: //NOTE (MinsuKim): KW stands for kernel map in this case.
    case DataDimension::OC:
      new_strt->SetInputStationary(); s = Stationary::INPUT;
      break;
    case DataDimension::IC:
      new_strt->SetOutputStationary(); s = Stationary::OUTPUT;
      break;
    case DataDimension::OW: //NOTE (MinsuKim): OW stands for output map in this case.
      new_strt->SetWeightStationary(); s = Stationary::WEIGHT;
      break;
    default:
      LOG(ERROR) << "Invalid data dimension: " << biggest;
  }

  new_loop->SetOffStructure(new_strt);
  return { new_loop, s };
}

CnnLoop* Scheduler::SearchBestLoopCase( const CnnLoop& loop, 
                                        const Architecture& arch)
{
  unique_ptr<CnnLoop> new_loop(new CnnLoop(loop));

  new_loop->SetVariableSet(LoopInitializing(new_loop->GetVariableSet(), arch));
  /* #region Logging */
  LOG(INFO) << "PE utilization optimizing finishes...";
  LOG(INFO) << "  TIW: " << new_loop->GetVariableSet().GetTiw();
  LOG(INFO) << "  TIH: " << new_loop->GetVariableSet().GetTih();
  LOG(INFO) << "  TIC: " << new_loop->GetVariableSet().GetTic();
  LOG(INFO) << "  TKW: " << new_loop->GetVariableSet().GetTkw();
  LOG(INFO) << "  TKH: " << new_loop->GetVariableSet().GetTkh();
  LOG(INFO) << "  TOW: " << new_loop->GetVariableSet().GetTow();
  LOG(INFO) << "  TOH: " << new_loop->GetVariableSet().GetToh();
  LOG(INFO) << "  TOC: " << new_loop->GetVariableSet().GetToc();
  /* #endregion */
  new_loop.reset(LoopElimination(*new_loop, arch));
  /* #region Logging */
  LOG(INFO) << "Loop elimination optimizing finishes...";
  LOG(INFO) << "  TIW: " << new_loop->GetVariableSet().GetTiw();
  LOG(INFO) << "  TIH: " << new_loop->GetVariableSet().GetTih();
  LOG(INFO) << "  TIC: " << new_loop->GetVariableSet().GetTic();
  LOG(INFO) << "  TKW: " << new_loop->GetVariableSet().GetTkw();
  LOG(INFO) << "  TKH: " << new_loop->GetVariableSet().GetTkh();
  LOG(INFO) << "  TOW: " << new_loop->GetVariableSet().GetTow();
  LOG(INFO) << "  TOH: " << new_loop->GetVariableSet().GetToh();
  LOG(INFO) << "  TOC: " << new_loop->GetVariableSet().GetToc();
  /* #endregion */
  pair<CnnLoop*, Stationary> interchange_result = LoopInterchange(*new_loop);
  new_loop.reset(interchange_result.first);
  Stationary s = interchange_result.second;
  /* #region Logging */
  switch (s) {
    case Stationary::INPUT:
      LOG(INFO) << "Off-chip loop is set as input stationary.";   break;
    case Stationary::WEIGHT:
      LOG(INFO) << "Off-chip loop is set as weight stationary.";  break;
    case Stationary::OUTPUT:
      LOG(INFO) << "Off-chip loop is set as output stationary.";  break;
    default:
      LOG(WARNING) << "Off-chip loop is not set as any stationary.";
  }
  /* #endregion */
  kw_itr_cnt_ = new_loop->GetVariableSet().GetKw() - 
                new_loop->GetVariableSet().GetTkw() + 1;
  kh_itr_cnt_ = new_loop->GetVariableSet().GetKh() - 
                new_loop->GetVariableSet().GetTkh() + 1;
  ow_itr_cnt_ = new_loop->GetVariableSet().GetOw() - 
                new_loop->GetVariableSet().GetTow() + 1;
  oh_itr_cnt_ = new_loop->GetVariableSet().GetOh() - 
                new_loop->GetVariableSet().GetToh() + 1;
  ic_itr_cnt_ = new_loop->GetVariableSet().GetIc() - 
                new_loop->GetVariableSet().GetTic() + 1;
  oc_itr_cnt_ = new_loop->GetVariableSet().GetOc() - 
                new_loop->GetVariableSet().GetToc() + 1;
  total_itr_  = kw_itr_cnt_ * kh_itr_cnt_ * ow_itr_cnt_ * oh_itr_cnt_ *
                ic_itr_cnt_ * oc_itr_cnt_; // encoding the tiling iterations.
  /* #region Logging */
  LOG(INFO) << "All iteration count is " << total_itr_;
  LOG(INFO) << " kw_itr_cnt: "  << kw_itr_cnt_
            << "  inter: "      << new_loop->GetVariableSet().GetKw()
            << "  tile: "       << new_loop->GetVariableSet().GetTkw();
  LOG(INFO) << " kh_itr_cnt: "  << kh_itr_cnt_
            << "  inter: "      << new_loop->GetVariableSet().GetKh()
            << "  tile: "       << new_loop->GetVariableSet().GetTkh();
  LOG(INFO) << " ow_itr_cnt: "  << ow_itr_cnt_
            << "  inter: "      << new_loop->GetVariableSet().GetOw()
            << "  tile: "       << new_loop->GetVariableSet().GetTow();
  LOG(INFO) << " oh_itr_cnt: "  << oh_itr_cnt_
            << "  inter: "      << new_loop->GetVariableSet().GetOh()
            << "  tile: "       << new_loop->GetVariableSet().GetToh();
  LOG(INFO) << " ic_itr_cnt: "  << ic_itr_cnt_
            << "  inter: "      << new_loop->GetVariableSet().GetIc()
            << "  tile: "       << new_loop->GetVariableSet().GetTic();
  LOG(INFO) << " oc_itr_cnt: "  << oc_itr_cnt_
            << "  inter: "      << new_loop->GetVariableSet().GetOc()
            << "  tile: "       << new_loop->GetVariableSet().GetToc();
  /* #endregion */
  CnnLoop* best_loop[num_threads_];
  double best_edp[num_threads_];
  for (size_t thr = 0 ; thr < num_threads_ ; thr++) { // Initialize thread values.
    best_loop[thr] = new CnnLoop(*new_loop);
    best_edp[thr] = DBL_MAX;
  }
  // Distribute jobs for each threads.
  StartProgress();
  for (size_t thr = 0 ; thr < min((size_t)num_threads_, total_itr_) ; thr++) {
    int start, end;
    if (num_threads_ < total_itr_) {
      start = thr*(total_itr_ / num_threads_) + 1;
      end   = (thr < num_threads_-1) ?  (thr+1)*(total_itr_ / num_threads_) :
                                          total_itr_;
    } else {
      start = thr + 1;
      end = start;
    }
    search_threads_[thr] = thread(
      &Scheduler::SearchBestLoopCaseThread, this,
      best_loop[thr], s, arch, &best_edp[thr],
      start, end
    );
  }
  // Join threads
  for (size_t thr = 0 ; thr < min((size_t)num_threads_, total_itr_) ; thr++)
    search_threads_[thr].join();
  FinishProgress();
  // Find final best CNN loop.
  CnnLoop* final_loop = nullptr;
  double final_edp = DBL_MAX;
  for (size_t thr = num_threads_ ; thr > 0 ; thr--) {
    if (final_edp > best_edp[thr-1]) {
      // Update best CNN loop case.
      if (final_loop != nullptr)  delete final_loop;
      final_loop = best_loop[thr-1];
      final_edp = best_edp[thr-1];
    } else {
      delete best_loop[thr-1];
    }
  }
  CHECK(final_loop != nullptr) << "Cannot find best loop";
  /* #region Logging */
  LOG(INFO) << "Searching best loop case is finished...";
  LOG(INFO) << "  TIW: " << final_loop->GetVariableSet().GetTiw();
  LOG(INFO) << "  TIH: " << final_loop->GetVariableSet().GetTih();
  LOG(INFO) << "  TIC: " << final_loop->GetVariableSet().GetTic();
  LOG(INFO) << "  TKW: " << final_loop->GetVariableSet().GetTkw();
  LOG(INFO) << "  TKH: " << final_loop->GetVariableSet().GetTkh();
  LOG(INFO) << "  TOW: " << final_loop->GetVariableSet().GetTow();
  LOG(INFO) << "  TOH: " << final_loop->GetVariableSet().GetToh();
  LOG(INFO) << "  TOC: " << final_loop->GetVariableSet().GetToc();
  LOG(INFO) << "  PIW: " << final_loop->GetVariableSet().GetPiw();
  LOG(INFO) << "  PIH: " << final_loop->GetVariableSet().GetPih();
  LOG(INFO) << "  PIC: " << final_loop->GetVariableSet().GetPic();
  LOG(INFO) << "  PKW: " << final_loop->GetVariableSet().GetPkw();
  LOG(INFO) << "  PKH: " << final_loop->GetVariableSet().GetPkh();
  LOG(INFO) << "  POW: " << final_loop->GetVariableSet().GetPow();
  LOG(INFO) << "  POH: " << final_loop->GetVariableSet().GetPoh();
  LOG(INFO) << "  POC: " << final_loop->GetVariableSet().GetPoc();
  /* #endregion */
  if (final_loop->GetVariableSet().GetTow() < 
      final_loop->GetVariableSet().GetToh()) {
    VariableSet* wh_swap = new VariableSet(final_loop->GetVariableSet());

    wh_swap->SetTow(final_loop->GetVariableSet().GetToh());
    wh_swap->SetToh(final_loop->GetVariableSet().GetTow());

    wh_swap->SetTiw(final_loop->GetVariableSet().GetTih());
    wh_swap->SetTih(final_loop->GetVariableSet().GetTiw());

    wh_swap->SetPow(final_loop->GetVariableSet().GetPoh());
    wh_swap->SetPoh(final_loop->GetVariableSet().GetPow());

    wh_swap->SetPiw(final_loop->GetVariableSet().GetPih());
    wh_swap->SetPih(final_loop->GetVariableSet().GetPiw());

    final_loop->SetVariableSet(wh_swap);
  }
  return final_loop;
}

void Scheduler::SearchBestLoopCaseThread( CnnLoop* loop, Stationary s,
                                          const Architecture& arch,
                                          double* edp,
                                          int start_itr, int end_itr)
{
  const VariableSet& varset = loop->GetVariableSet();

  double best_edp = DBL_MAX;
  CnnLoop* best_loop = nullptr;

  for (int it = end_itr ; it >= start_itr ; it--) {
    CnnLoop* itr_loop = new CnnLoop(*loop);
    VariableSet* itr_varset = new VariableSet(loop->GetVariableSet());
    double itr_edp;
    int encoded_it = it-1;
    // Decoding iterations
    itr_varset->SetTkw(varset.GetTkw() + encoded_it%kw_itr_cnt_);
    encoded_it /= kw_itr_cnt_;
    itr_varset->SetTkh(varset.GetTkh() + encoded_it%kh_itr_cnt_);
    encoded_it /= kh_itr_cnt_;
    itr_varset->SetTow(varset.GetTow() + encoded_it%ow_itr_cnt_);
    itr_varset->SetTiw(
      min(itr_varset->GetIw(),
          (itr_varset->GetTow()-1)*varset.GetStride()+varset.GetTkw()
      ));
    //itr_varset->SetTiw(CalcInputLength(itr_varset->GetTow()));
    encoded_it /= ow_itr_cnt_;
    itr_varset->SetToh(varset.GetToh() + encoded_it%oh_itr_cnt_);
    itr_varset->SetTih(
      min(itr_varset->GetIh(), 
          (itr_varset->GetToh()-1)*varset.GetStride()+varset.GetTkh()
      ));
    //itr_varset->SetTih(CalcInputLength(itr_varset->GetToh()));
    encoded_it /= oh_itr_cnt_;
    itr_varset->SetTic(varset.GetTic() + encoded_it%ic_itr_cnt_);
    encoded_it /= ic_itr_cnt_;
    itr_varset->SetToc(varset.GetToc() + encoded_it%oc_itr_cnt_);
    encoded_it /= oc_itr_cnt_;
    // Compare EDP only when it is not memory overflow.
    if (!IsMemorySizeOverflow(*itr_varset, arch)) {
      //REVIEW (MinsuKim): This is a main bottle neck of the very long compile time.
      itr_varset->SetParlLoopVariables(
        MakeParlLoopVariables(itr_varset->GetOnLoopVariables(), arch)
      );
      itr_loop->SetVariableSet(itr_varset);
      itr_edp = GetEdp(*itr_loop, arch, s);
      if (best_edp > itr_edp) {
        if (best_loop != nullptr) delete best_loop;
        best_loop = itr_loop; // allocate new loop.
        best_edp = itr_edp;
      }
    } else {
      delete itr_loop;
      delete itr_varset;
    }
    IncreaseProgress();
  }
  if (best_loop != nullptr) {
    delete loop;
    loop = new CnnLoop(*best_loop);
    *edp = best_edp;
  }
}

double Scheduler::GetEdp( const CnnLoop& loop, 
                          const Architecture& arch, Stationary s) const
{
  const double correction_constant = 1000.0;
  // PE utilization number is much smaller than DRAM accesses size.
  // So, I make a correction constant to scale up the PE utilization number.

  const VariableSet& varset = loop.GetVariableSet();
  long int num_ops =  varset.GetKw() * varset.GetKh() * varset.GetIc() *
                      varset.GetOw() * varset.GetOh() * varset.GetOc();
  double pe_util = GetPeUtil(varset, arch);
  long int dram_accesses = GetDramAccesses(varset, s);
  int num_pe = arch.GetPeDim()[0][0] * arch.GetPeDim()[0][1];
  double performance = min(
    arch.GetFrequency() * num_pe * pe_util,
    num_ops * arch.GetBandwidth() / dram_accesses
  );
  return dram_accesses / performance / correction_constant;
}

long int Scheduler::GetDramAccesses(const VariableSet& varset,Stationary s)const
{
  long int input_accesses  =  GetInputDataReload(varset, s) *
                              varset.GetOffLoopVariables().GetInputSize() *
                              sizeof(DataType);
  long int weight_accesses =  GetWeightDataReload(varset, s) *
                              varset.GetOffLoopVariables().GetWeightSize() *
                              sizeof(DataType);
  long int output_accesses =  GetOutputDataReload(varset, s) *
                              varset.GetOffLoopVariables().GetOutputSize() *
                              sizeof(DataType);
  long int total_accesses  =  input_accesses+weight_accesses+output_accesses;

  return total_accesses;
}

int Scheduler::GetInputDataReload(const VariableSet& varset, Stationary s) const
{
  return (s == Stationary::INPUT) ? 
          1 : ceil((double)varset.GetKw() / varset.GetTkw()) *
              ceil((double)varset.GetKh() / varset.GetTkh()) *
              ceil((double)varset.GetOc() / varset.GetToc());
}

int Scheduler::GetWeightDataReload(const VariableSet& varset,Stationary s) const
{
  return (s == Stationary::WEIGHT) ?
          1 : ceil((double)varset.GetOw() / varset.GetTow()) *
              ceil((double)varset.GetOh() / varset.GetToh());
}

int Scheduler::GetOutputDataReload(const VariableSet& varset,Stationary s) const
{
  return (s == Stationary::OUTPUT) ?
          1 : GetPsumReload(varset, s) + GetPsumStore(varset, s);
}

int Scheduler::GetPsumReload(const VariableSet& varset, Stationary s) const
{
  return (s == Stationary::OUTPUT) ?
          0 : ceil((double)varset.GetKw() / varset.GetTkw()) *
              ceil((double)varset.GetKh() / varset.GetTkh()) *
              ceil((double)varset.GetIc() / varset.GetTic()) - 1;
}

int Scheduler::GetPsumStore(const VariableSet& varset, Stationary s) const
{
  return (s == Stationary::OUTPUT) ?
          0 : ceil((double)varset.GetKw() / varset.GetTkw()) *
              ceil((double)varset.GetKh() / varset.GetTkh()) *
              ceil((double)varset.GetIc() / varset.GetTic()) - 1;
}

double Scheduler::GetPeUtil(const VariableSet& varset, const Architecture& arch)
  const
{
  double util = 1.0;
  const int row = 0, col = 1;
  for (int r_c = row ; r_c <= col ; r_c++) {
    vector<DataDimension> dim_vec = arch.GetPeStructure()[r_c];
    for (DataDimension dim : dim_vec) {
      switch (dim) {
        case DataDimension::KW:
          util *= GetParamUtil(varset.GetTkw(), varset.GetPkw()); break;
        case DataDimension::KH:
          util *= GetParamUtil(varset.GetTkh(), varset.GetPkh()); break;
        case DataDimension::IC:
          util *= GetParamUtil(varset.GetTic(), varset.GetPic()); break;
        case DataDimension::OW:
          util *= GetParamUtil(varset.GetTow(), varset.GetPow()); break;
        case DataDimension::OH:
          util *= GetParamUtil(varset.GetToh(), varset.GetPoh()); break;
        case DataDimension::OC:
          util *= GetParamUtil(varset.GetToc(), varset.GetPoc()); break;
        case DataDimension::IW:
          util *= GetParamUtil(varset.GetTiw(), varset.GetPiw()); break;
        case DataDimension::IH:
          util *= GetParamUtil(varset.GetTih(), varset.GetPih()); break;
        default:
          LOG(ERROR) << "Invalid DataDimension: " << dim;
      }
    }
  }
  return util;
}

double Scheduler::GetParamUtil(const int tile, const int unroll) const
{
  return (double)tile / (ceil((double)tile/unroll) * unroll);
}

Variables Scheduler::MakeParlLoopVariables( const Variables& on_vars, 
                                            const Architecture& arch)
{
  // Initialize unroll loop variables
  Variables parl_vars;
  parl_vars.SetStride(on_vars.GetStride());
  parl_vars.SetKw(1);
  parl_vars.SetKh(1);
  parl_vars.SetIc(1);
  parl_vars.SetOw(1);
  parl_vars.SetOh(1);
  parl_vars.SetOc(1);
  parl_vars.SetIw(1);
  parl_vars.SetIh(1);
  parl_vars.SetPw(on_vars.GetPw());
  parl_vars.SetPh(on_vars.GetPh());
  // Iterate from PE row to PE column
  for (int r_c = 0 ; r_c < 2 ; r_c++) { // r_c = 0: row, r_c = 1: column
    int pe_len = arch.GetPeDim()[0][r_c];
    // Allocate based on Greatest Common Divisor first.
    vector<vector<DataDimension>> pe_strt = arch.GetPeStructure();
    for (DataDimension d : pe_strt[r_c]) {
      int gcd = 1; // greatest common divisor
      switch (d) {
        case DataDimension::KW:
          gcd = GreatestCommonDivisor(on_vars.GetKw(), pe_len);
          parl_vars.SetKw(gcd);
          break;
        case DataDimension::KH:
          gcd = GreatestCommonDivisor(on_vars.GetKh(), pe_len);
          parl_vars.SetKh(gcd);
          break;
        case DataDimension::OC:
          gcd = GreatestCommonDivisor(on_vars.GetOc(), pe_len);
          parl_vars.SetOc(gcd);
          break;
        case DataDimension::IC:
          gcd = GreatestCommonDivisor(on_vars.GetIc(), pe_len);
          parl_vars.SetIc(gcd);
          break;
        case DataDimension::OW:
          gcd = GreatestCommonDivisor(on_vars.GetOw(), pe_len);
          parl_vars.SetOw(gcd);
          break;
        case DataDimension::OH:
          gcd = GreatestCommonDivisor(on_vars.GetOh(), pe_len);
          parl_vars.SetOh(gcd);
          break;
        case DataDimension::IW:
          gcd = GreatestCommonDivisor(on_vars.GetIw(), pe_len);
          parl_vars.SetIw(gcd);
          break;
        case DataDimension::IH:
          gcd = GreatestCommonDivisor(on_vars.GetIh(), pe_len);
          parl_vars.SetIh(gcd);
          break;
        default:
          LOG(ERROR) << "Invalid data dimension: " << d;
      }
      pe_len /= gcd;
    }
    // Distribute remain factors.
    for (DataDimension d : pe_strt[r_c]) {
      vector<int> factors = PrimeFactorization(pe_len);
      for (int factor : factors) {
        switch (d) {
          case DataDimension::KW:
            if (on_vars.GetKw()/parl_vars.GetKw() >= factor) {
              parl_vars.SetKw(parl_vars.GetKw() * factor);
              pe_len /= factor;
            }
            break;
          case DataDimension::KH:
            if (on_vars.GetKh()/parl_vars.GetKh() >= factor) {
              parl_vars.SetKh(parl_vars.GetKh() * factor);
              pe_len /= factor;
            }
            break;
          case DataDimension::OC:
            if (on_vars.GetOc()/parl_vars.GetOc() >= factor) {
              parl_vars.SetOc(parl_vars.GetOc() * factor);
              pe_len /= factor;
            }
            break;
          case DataDimension::IC:
            if (on_vars.GetIc()/parl_vars.GetIc() >= factor) {
              parl_vars.SetIc(parl_vars.GetIc() * factor);
              pe_len /= factor;
            }
            break;
          case DataDimension::OW:
            if (on_vars.GetOw()/parl_vars.GetOw() >= factor) {
              parl_vars.SetOw(parl_vars.GetOw() * factor);
              pe_len /= factor;
            }
            break;
          case DataDimension::OH:
            if (on_vars.GetOh()/parl_vars.GetOh() >= factor) {
              parl_vars.SetOh(parl_vars.GetOh() * factor);
              pe_len /= factor;
            }
            break;
          case DataDimension::IW:
            if (on_vars.GetIw()/parl_vars.GetIw() >= factor) {
              parl_vars.SetIw(parl_vars.GetIw() * factor);
              pe_len /= factor;
            }
            break;
          case DataDimension::IH:
            if (on_vars.GetIh()/parl_vars.GetIh() >= factor) {
              parl_vars.SetIh(parl_vars.GetIh() * factor);
              pe_len /= factor;
            }
            break;
          default:
            LOG(ERROR) << "Invalid data dimension: " << d;
        }
      }
    }
  }
  // Following input and output width/height each other.
  parl_vars.SetIw(max(parl_vars.GetKw(), parl_vars.GetIw()));
  parl_vars.SetIh(max(parl_vars.GetKh(), parl_vars.GetIh()));
  if (parl_vars.GetOw() == 1)
    parl_vars.SetOw(CalcOutputLength(parl_vars.GetIw()));
  if (parl_vars.GetOh() == 1)
    parl_vars.SetOh(CalcOutputLength(parl_vars.GetIh()));

  return parl_vars;
}

bool Scheduler::IsMemorySizeOverflow( const VariableSet& varset, 
                                      const Architecture& arch) const
{
  const Variables& on_vars  = varset.GetOnLoopVariables();
  const Variables& off_vars = varset.GetOffLoopVariables();

  int input_size  = on_vars.GetInputSize()  * sizeof(DataType); // Bytes
  int weight_size = on_vars.GetWeightSize() * sizeof(DataType); // Bytes
  int output_size = on_vars.GetOutputSize() * sizeof(DataType); // Bytes

  // If data is not fully tiled, it is double buffered. Or it is single buffered.
  int input_mem_size  = (on_vars.GetInputSize() < off_vars.GetInputSize())   ?
                        arch.GetInputMemSize() / 2 : arch.GetInputMemSize();
  int weight_mem_size = (on_vars.GetWeightSize() < off_vars.GetWeightSize()) ?
                        arch.GetWeightMemSize() / 2 : arch.GetWeightMemSize();
  int output_mem_size = (on_vars.GetOutputSize() < off_vars.GetOutputSize()) ?
                        arch.GetOutputMemSize() / 2 : arch.GetOutputMemSize();
  
  // Overflow check
  return  input_size  > input_mem_size  ||
          weight_size > weight_mem_size ||
          output_size > output_mem_size;
}

int Scheduler::GreatestCommonDivisor(int a, int b) const
{
  if (a < b) {
    int tmp = a;
    a = b;
    b = tmp;
  }
  while (b != 0) {
    int n = a % b;
    a = b;
    b = n;
  }
  return a;
}

vector<int> Scheduler::PrimeFactorization(int a)
{
  vector<int> factors;
  int prime_num = 2;

  while (a != 1) {
    if (a % prime_num == 0) {
      factors.push_back(prime_num);
      a /= prime_num;
    } else {
      prime_num++;
    }
  }
  return factors;
}

//TODO (MinsuKim): Make algorithm to decide on-chip loop structure.
Structure* Scheduler::FixOnLoopStructure(void)
{
  Structure* fix_strt = new Structure();
  fix_strt->SetOutputStationary();
  return fix_strt;
}
