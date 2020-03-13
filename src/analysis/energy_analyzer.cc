#include "analysis/energy_analyzer.h"

#define UNIT_ACCESS 4 // bytes

using analysis::EnergyAnalyzer;

double EnergyAnalyzer::AnalyzeOffChipAccessEnergy(
  const long int off_chip_total_access, const double off_chip_32_energy) const
{
  return (double)off_chip_total_access * off_chip_32_energy / UNIT_ACCESS;
}

double EnergyAnalyzer::AnalyzeOnChipAccessEnergy(
    const long int on_chip_total_access, const double on_chip_32_energy) const
{
  return (double)on_chip_total_access * on_chip_32_energy / UNIT_ACCESS;
}

double EnergyAnalyzer::AnalyzeExecutionEnergy(
  const long int num_ops, const double mac_energy) const
{
  return num_ops * mac_energy;
}

double EnergyAnalyzer::AnalyzePower(
  const long int latency, const double total_energy) const
{
  return total_energy / (double)latency;
}
