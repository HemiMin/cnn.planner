#ifndef CNNPLANNER_ARCH_ARCHITECTURE_H_
#define CNNPLANNER_ARCH_ARCHITECTURE_H_

#include <vector>

#include "general/data_type.h"
#include "parameter/compiler_parameter.h"
#include "parameter/profiler_parameter.h"

using std::vector;

using parameter::CompilerParameter;
using parameter::ProfilerParameter;

namespace arch {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Object implemented from hardware config file
//! @details  Get the hardware config from parameter and make object including
//!           hardware configurations
//! @author   Minsu Kim
//! @date     2019-10-04
////////////////////////////////////////////////////////////////////////////////
enum DataDimension { None=0, KW, KH, IC, OW, OH, OC, IW, IH };
class Architecture
{
  public:
    //Architecture(void) = default;
    //! @brief        Construct architecture instance for compiler.
    //! @param param  Compiler parameter.
    Architecture(const CompilerParameter& param);
    //! @brief        Construct architecture instance for profiler.
    //! @param param  Profiler parameter.
    Architecture(const ProfilerParameter& param);
    //! @brief              Set MAC cycles.
    //! @param mac_cycles   MAC cycles.
    void SetMacCycles(int mac_cycles);
    //! @brief              Set bandwidth.
    //! @param mac_cycles   off-chip memory bandwidth.
    void SetBandwidth(double bandwidth);
    //! @brief              Set frequency.
    //! @param frequency    Hardware frequency.
    void SetFrequency(double frequency);
    //! @brief              Set MAC energy.
    //! @param mac_energy   MAC energy.
    void SetMacEnergy(double mac_energy);
    //! @brief              Set on-chip 32-bit data transfer energy.
    //! @param mac_cycles   On-chip 32-bit data transfer energy.
    void SetOnChipEnergy(double on_chip_energy_32);
    //! @brief              Set off-chip 32-bit data transfer energy.
    //! @param mac_cycles   Off-chip 32-bit data transfer energy.
    void SetOffChipEnergy(double off_chip_energy_32);
    //! @brief              Set input on-chip memory size.
    //! @param mac_cycles   Input on-chip memory size. 
    void SetInputMemSize(long int input_mem_size);
    //! @brief              Set weight on-chip memory size.
    //! @param mac_cycles   Weight on-chip memory size.
    void SetWeightMemSize(long int weight_mem_size);
    //! @brief              Set output on-chip memory size.
    //! @param mac_cycles   Output on-chip memory size.
    void SetOutputMemSize(long int output_mem_size);
    //! @brief              Set 2D PE physical dimension.
    //! @param mac_cycles   PE physical dimension.
    void SetPeDim(vector<vector<int>> pe_dim);
    //! @brief              Set calculation dimension on 2D PE.
    //! @param mac_cycles   Calculation dimension expressed by integer.
    void SetPeStructure(vector<vector<int>> pe_structure);
    //! @brief              Set calculation dimension on 2D PE.
    //! @param mac_cycles   Calculation dimension expressed by DataDimension.
    void SetPeStructure(vector<vector<DataDimension>> pe_structure);
    //! @brief              Get MAC cycles.
    //! @return             MAC cycles.
    int GetMacCycles(void) const;
    //! @brief              Get bandwidth.
    //! @return             off-chip memory bandwidth.
    double GetBandwidth(void) const;
    //! @brief              Get frequency.
    //! @return             Hardware frequency.
    double GetFrequency(void) const;
    //! @brief              Get MAC energy.
    //! @return             MAC energy.
    double GetMacEnergy(void) const;
    //! @brief              Get on-chip energy.
    //! @return             On-chip 32-bit data transfer energy.
    double GetOnChipEnergy(void) const;
    //! @brief              Get off-chip energy.
    //! @return             Off-chip 32-bit data transfer energy.
    double GetOffChipEnergy(void) const;
    //! @brief              Get input on-chip memory size.
    //! @return             Input on-chip memory size.
    long int GetInputMemSize(void) const;
    //! @brief              Get weight on-chip memory size.
    //! @return             Weight on-chip memory size.
    long int GetWeightMemSize(void) const;
    //! @brief              Get output on-chip memory size.
    //! @return             Output on-chip memory size.
    long int GetOutputMemSize(void) const;
    //! @brief              Get PE physical dimension.
    //! @return             PE physical dimension.
    vector<vector<int>> GetPeDim(void) const;
    //! @brief              Get calculation dimension of PE.
    //! @return             Calculation dimension of PE.
    vector<vector<DataDimension>> GetPeStructure(void) const; 
  private:
    int mac_cycles_ = NON_VALID;
    double bandwidth_ = NON_VALID;
    double frequency_ = NON_VALID;
    double mac_energy_ = NON_VALID;
    double on_chip_energy_32_ = NON_VALID;
    double off_chip_energy_32_ = NON_VALID;
    long int input_mem_size_ = NON_VALID;
    long int weight_mem_size_ = NON_VALID;
    long int output_mem_size_ = NON_VALID;
    vector<vector<int>> pe_dim_;
    vector<vector<DataDimension>> pe_structure_;
};
} // namespcae arch
#endif