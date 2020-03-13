#ifndef CNNPLANNER_LOOP_SCHEDULER_H_
#define CNNPLANNER_LOOP_SCHEDULER_H_

#include <thread>
#include <mutex>
#include <vector>
#include <utility>
#include <memory>

#include "loop/cnn_loop.h"
#include "arch/architecture.h"
#include "general/tqdm.h"

using std::thread;
using std::mutex;
using std::vector;
using std::pair;
using std::unique_ptr;

using loop::CnnLoop;
using arch::Architecture;

namespace loop {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Scheduling CNN loops
//! @details    Manage loop sequence and tiling strategy.
//! @author     Minsu Kim
//! @date       2019-07-03
////////////////////////////////////////////////////////////////////////////////
enum Stationary { INPUT=0, WEIGHT, OUTPUT };

class Scheduler
{
  public:
    //! @brief                  Constructor of LoopScheduler
    //! @details                Set private variables.
    Scheduler(void);
    //! @brief                  Destructor
    ~Scheduler(void);
    //! @brief                  Search best tiling case and loop structure.
    //! @details                Core engine of this project.
    //!                         Implement double return method by pointer.
    //! @param loop             Overall loop informantion of CNN.
    //! @param arch             Hardware configurations.
    //! @return                 New instance of CnnLoop.
    CnnLoop* SearchBestLoopCase(const CnnLoop& loop, const Architecture& arch);
    //! @brief                  Set parallelization loop variables
    //! @param on_vars          Intra loop variables
    //! @param arch             Hardware configurations
    //! @return                 Unroll loop variables based on architecture
    loop::Variables MakeParlLoopVariables(const loop::Variables& on_vars, 
                                          const Architecture& arch);
    //! @brief                  Set intra loop structure by fixed sequence.
    //! @detils                 Need to add architecture based algorithm to decide intra loop structure.
    //! @return                 New on-chip loop structure.
    //TODO (MinsuKim): Make algorithm to decide on-chip loop structure.
    Structure* FixOnLoopStructure(void);

  private:
    unsigned int num_threads_;
    thread* search_threads_;
    mutex mtx_lock_;
    tqdm progress_bar_;
    int progress_;

    size_t kw_itr_cnt_;
    size_t kh_itr_cnt_;
    size_t ow_itr_cnt_;
    size_t oh_itr_cnt_;
    size_t ic_itr_cnt_;
    size_t oc_itr_cnt_;
    size_t total_itr_;

    void StartProgress(void);
    void IncreaseProgress(int interval=1);
    void FinishProgress(void);

    //TODO (MinsuKim): If all variables are fully tiled, then, there is no need to search space pruning anymore.
    bool EndPruning(const VariableSet& varset) const;
    VariableSet* LoopInitializing(const VariableSet& varset, 
                                  const Architecture& arch);
    CnnLoop* LoopElimination(const CnnLoop& loop, const Architecture& arch);
    pair<CnnLoop*, Stationary> LoopInterchange(const CnnLoop& loop);
    void SearchBestLoopCaseThread(CnnLoop* loop, Stationary s, 
                                  const Architecture& arch,
                                  double* edp, int start_itr, int end_itr);

    // EDP: Energy-Delay Product
    double GetEdp(const CnnLoop& loop, 
                  const Architecture& arch, Stationary s) const;

    long int GetDramAccesses(const VariableSet& varset, Stationary s) const;
    int GetInputDataReload(const VariableSet& varset, Stationary s) const;
    int GetWeightDataReload(const VariableSet& varset, Stationary s) const;
    int GetOutputDataReload(const VariableSet& varset, Stationary s) const;
    int GetPsumReload(const VariableSet& varset, Stationary s) const;
    int GetPsumStore(const VariableSet& varset, Stationary s) const;

    double GetPeUtil(const VariableSet& varset, const Architecture& arch) const;
    double GetParamUtil(int tile_param, const int unroll_param) const;
    
    bool IsMemorySizeOverflow(const VariableSet& varset,
                              const Architecture& arch) const;

    int GreatestCommonDivisor(int a, int b) const;
    vector<int> PrimeFactorization(int a);
};
} // namespace loop
#endif
