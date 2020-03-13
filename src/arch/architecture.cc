#include "arch/architecture.h"

#include <glog/logging.h>

arch::Architecture::Architecture(const CompilerParameter& param)
{
  SetMacCycles(param.GetMacCycles());
  SetFrequency(param.GetFrequency());
  SetBandwidth(param.GetBandwidth());
  SetInputMemSize(param.GetInputMemSize());
  SetWeightMemSize(param.GetWeightMemSize());
  SetOutputMemSize(param.GetOutputMemSize());
  SetPeDim(param.GetPeDim());
  SetPeStructure(param.GetPeStructure());
  /* #region Logging */
  LOG(INFO) << "Initialize Architecture instance for compiler.";
  LOG(INFO) << "  MAC cycles: " << mac_cycles_ << " cycles";
  LOG(INFO) << "  Frequency: "  << frequency_ << " GHz";
  LOG(INFO) << "  Bandwidth: "  << bandwidth_ << " GB/s";
  LOG(INFO) << "  Input on-chip memory size: " << input_mem_size_ << " Bytes";
  LOG(INFO) << "  Weight on-chip memory size: " << weight_mem_size_ << " Bytes";
  LOG(INFO) << "  Output on-chip memory size: " << output_mem_size_ << " Bytes";
  LOG(INFO) << "  PE physical dimension";
  for (auto pe_dim_row : pe_dim_) {
    LOG(INFO) << "    (" << pe_dim_row[0] << ", " << pe_dim_row[1] << ")";
  }
  LOG(INFO) << "  PE calculation mapping structure";
  LOG(INFO) << "    <<< Row >>>";
  for (auto pe_strt_row : pe_structure_[0]) {
    LOG(INFO) << "    " << pe_strt_row;
  }
  LOG(INFO) << "    <<< Column >>>";
  for (auto pe_strt_col : pe_structure_[1]) {
    LOG(INFO) << "    " << pe_strt_col;
  }
  /* #endregion */
}

arch::Architecture::Architecture(const ProfilerParameter& param)
{
  SetMacCycles(param.GetMacCycles());
  SetFrequency(param.GetFrequency());
  SetBandwidth(param.GetBandwidth());
  SetMacEnergy(param.GetMacEnergy());
  SetOnChipEnergy(param.GetOnChip32Energy());
  SetOffChipEnergy(param.GetOffChip32Energy());
  SetInputMemSize(param.GetInputMemSize());
  SetWeightMemSize(param.GetWeightMemSize());
  SetOutputMemSize(param.GetOutputMemSize());
  SetPeDim(param.GetPeDim());
  SetPeStructure(param.GetPeStructure());
  /* #region Logging */
  LOG(INFO) << "Initialize Architecture instance for compiler.";
  LOG(INFO) << "  MAC cycles: " << mac_cycles_ << " cycles";
  LOG(INFO) << "  Frequency: "  << frequency_ << " GHz";
  LOG(INFO) << "  Bandwidth: "  << bandwidth_ << " GB/s";
  LOG(INFO) << "  MAC energy:"  << mac_energy_ << " nJ";
  LOG(INFO) << "  On-chip access energy: " << on_chip_energy_32_ << " nJ";
  LOG(INFO) << "  Off-chip access energy: " << off_chip_energy_32_ << " nJ";
  LOG(INFO) << "  Input on-chip memory size: " << input_mem_size_ << " Bytes";
  LOG(INFO) << "  Weight on-chip memory size: " << weight_mem_size_ << " Bytes";
  LOG(INFO) << "  Output on-chip memory size: " << output_mem_size_ << " Bytes";
  LOG(INFO) << "  PE physical dimension";
  for (auto pe_dim_row : pe_dim_) {
    LOG(INFO) << "    (" << pe_dim_row[0] << ", " << pe_dim_row[1] << ")";
  }
  LOG(INFO) << "  PE calculation mapping structure";
  LOG(INFO) << "    <<< Row >>>";
  for (auto pe_strt_row : pe_structure_[0]) {
    LOG(INFO) << "    " << pe_strt_row;
  }
  LOG(INFO) << "    <<< Column >>>";
  for (auto pe_strt_col : pe_structure_[1]) {
    LOG(INFO) << "    " << pe_strt_col;
  }
  /* #endregion */
}

void arch::Architecture::SetMacCycles(int mac_cycles)
{
  mac_cycles_ = mac_cycles;
}

void arch::Architecture::SetBandwidth(double bandwidth)
{
  bandwidth_ = bandwidth;
}

void arch::Architecture::SetFrequency(double frequency)
{
  frequency_ = frequency;
}

void arch::Architecture::SetMacEnergy(double mac_energy)
{
  mac_energy_ = mac_energy;
}

void arch::Architecture::SetOnChipEnergy(double on_chip_energy_32)
{
  on_chip_energy_32_ = on_chip_energy_32;
}

void arch::Architecture::SetOffChipEnergy(double off_chip_energy_32)
{
  off_chip_energy_32_ = off_chip_energy_32;
}

void arch::Architecture::SetInputMemSize(long int input_mem_size)
{
  input_mem_size_ = input_mem_size;
}

void arch::Architecture::SetWeightMemSize(long int weight_mem_size)
{
  weight_mem_size_ = weight_mem_size;
}

void arch::Architecture::SetOutputMemSize(long int output_mem_size)
{
  output_mem_size_ = output_mem_size;
}

void arch::Architecture::SetPeDim(vector<vector<int>> pe_dim)
{
  pe_dim_ = pe_dim;
}

void arch::Architecture::SetPeStructure(vector<vector<int>> pe_structure)
{
  for (auto row : pe_structure) {
    vector<DataDimension> enum_row;
    for (auto col : row) {
      enum_row.push_back((DataDimension)col);
    }
    pe_structure_.push_back(enum_row);
  }
}

void arch::Architecture::SetPeStructure(
  vector<vector<DataDimension>> pe_structure)
{
  pe_structure_ = pe_structure;
}

int arch::Architecture::GetMacCycles(void) const
{
  return mac_cycles_;
}

double arch::Architecture::GetBandwidth(void) const
{
  return bandwidth_;
}

double arch::Architecture::GetFrequency(void) const
{
  return frequency_;
}

double arch::Architecture::GetMacEnergy(void) const
{
  return mac_energy_;
}

double arch::Architecture::GetOnChipEnergy(void) const
{
  return on_chip_energy_32_;
}

double arch::Architecture::GetOffChipEnergy(void) const
{
  return off_chip_energy_32_;
}

long int arch::Architecture::GetInputMemSize(void) const
{
  return input_mem_size_;
}

long int arch::Architecture::GetWeightMemSize(void) const
{
  return weight_mem_size_;
}

long int arch::Architecture::GetOutputMemSize(void) const
{
  return output_mem_size_;
}

vector<vector<int>> arch::Architecture::GetPeDim(void) const
{
  return pe_dim_;
}

vector<vector<arch::DataDimension>> arch::Architecture::GetPeStructure(void)
const
{
  return pe_structure_;
}