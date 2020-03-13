#pragma once

#include <iostream>

using std::ostream;

namespace analysis {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Analyzer for Energy analysis.
//! @details    Store off-chip, on-chip, execution energy and power consumption.
//! @author     Minsu Kim
//! @date       2019-07-03
////////////////////////////////////////////////////////////////////////////////
class EnergyAnalyzer
{
  public:
    //! @brief                          Return off-chip access energy.
    //! @details                        Energy consumption for off-chip access during whole layer processing.
    //!                                 The unit is nJ
    //! @param off_chip_total_access    Total number of off-chip accesses.
    //! @param off_chip_32_energy       Energy consumption when access 32-bit off-chip data. 
    //! @return                         Off-chip access energy  
    double AnalyzeOffChipAccessEnergy(const long int  off_chip_total_access, 
                                      const double    off_chip_32_energy) const;
    //! @brief                          Return on-chip access energy.
    //! @details                        Energy consumption for on-chip access during whole layer processing.
    //!                                 The unit is nJ
    //! @param on_chip_total_access     Total number of on-chip accesses.
    //! @param on_chip_32_energyu       Energy consumption when access 32-bit on-chip data.
    //! @return                         On-chip access energy
    double AnalyzeOnChipAccessEnergy( const long int  on_chip_total_access, 
                                      const double    on_chip_32_energy) const;
    //! @brief              Return execution energy.
    //! @details            Energy consumption for processing element running.
    //!                     The unit is nJ
    //! @param num_ops      The number of operations.
    //! @param mac_energy   Energy consumption when processing element is running.
    //! @return             Execution energy
    double AnalyzeExecutionEnergy(const long int  num_ops, 
                                  const double    mac_energy) const;
    //! @brief                  Power consumption during whole layer processing.
    //! @details                The unit is nJ
    //! @param latency          Latency regardless of real simulated.
    //! @param total_energy     Total energy consumption.
    //! @return                 Power consumption
    double AnalyzePower(const long int latency, const double total_energy)const;
};
} // namespace analysis
