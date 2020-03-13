#ifndef CNNPLANNER_PARAMETER_PROFILER_PARAMETER_H_
#define CNNPLANNER_PARAMETER_PROFILER_PARAMETER_H_

#include <vector>

#include "parameter/parameter.h"
#include "general/data_type.h"

#define STR_LEN 256

using std::vector;

namespace parameter {
class ProfilerParameter : public parameter::Parameter
{
  public:
    /**************************************************************************/
    //                               SETTER                                   //
    /**************************************************************************/
    //! @brief               Set multiply-accumulator unit energy.
    //! @param mac_energy    The unit energy of 16-bit or 32-bit multiply-accumulator.
    //!                      It depends on DataType size in general/data_type.h.
    //!                      The unit is nJ.
    void SetMacEnergy(const double mac_energy) { mac_energy_ = mac_energy; }
    //! @brief                      Set on-chip unit energy.
    //! @param on_chip_32_energy    The unit energy which is required when 32 bits transfer for accessing to on-chip memory.
    //!                             The unit is nJ.
    void SetOnChip32Energy(const double on_chip_32_energy)
      { on_chip_32_energy_ = on_chip_32_energy; }
    //! @brief                      Set off-chip unit energy.
    //! @param off_chip_32_energy   The unit energy which is required when 32 bits transfer for accessing to off-chip memory.
    //!                             The unit is nJ.
    void SetOffChip32Energy(const double off_chip_32_energy)
      { off_chip_32_energy_ = off_chip_32_energy; }
    //! @brief                      Set path of report CSV file.
    //! @param file_pah             CSV report file path.
    void SetReportFile(const char* file_path)
      { strncpy(report_file_, file_path, STR_LEN); }

    //! @brief              Set verbose mode.
    //! @param verbosity    Whether verbose mode or not.
    void SetVerbosity(const bool verbosity) { verbosity_ = verbosity; }

    /**************************************************************************/
    //                               GETTER                                   //
    /**************************************************************************/
    //! @brief                Return multiply-accumulator unit energy.
    //! @return               The unit energy of 16-bit or 32-bit multiply-accumulator.
    //!                       It depneds on DataType size of general/data_type.h
    //!                       The unit is nJ.
    double GetMacEnergy(void) const { return mac_energy_; }
    //! @brief                Return on-chip unit energy.
    //! @return               The unit energy which is required when 32 bits transfer for accessing to on-chip memory.
    //!                       The unit is nJ.
    double GetOnChip32Energy(void) const { return on_chip_32_energy_; }
    //! @brief                Return off-chip unit energy.
    //! @return               The unit energy which is required when 32 bits transfer for accessing to off-chip memory.
    //!                       The unit is nJ.
    double GetOffChip32Energy(void) const { return off_chip_32_energy_; }
    //! @brief                Return the path of report CSV file.
    //! @return               CSV report file path.
    const char* GetReportFile(void) const { return report_file_; }

    //! @brief      Return verbose flag.
    //! @return     Verbosity flag.
    bool IsVerbose(void) const { return verbosity_; }

  private:
    double mac_energy_ = NON_VALID;
    double on_chip_32_energy_ = NON_VALID;
    double off_chip_32_energy_ = NON_VALID;

    char report_file_[STR_LEN] = "";

    bool verbosity_ = false;
};
} // namespace parameter

#endif