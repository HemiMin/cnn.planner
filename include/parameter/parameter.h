#ifndef CNNPLANNER_PARAMETER_PARAMETER_H_
#define CNNPLANNER_PARAMETER_PARAMETER_H_

#include <string.h>
#include <vector>

#include "general/data_type.h"

#define STR_LEN 256

using std::vector;

namespace parameter {
class Parameter
{
  public:
    /**************************************************************************/
    //                               SETTER                                   //
    /**************************************************************************/
    //! @brief          Set stride value in Parameter class.
    //! @param stride   Stride of Convolutional layer.
    void SetStride(const int stride) { stride_ = stride; }
        
    //! @brief              Set input width value in Parameter class.
    //! @param input_width  Input width of Convolutional layer.
    void SetIw(const int input_width) { input_width_ = input_width; }
    //! @brief                  Set input height value in Parameter class.
    //! @param input_height     Input height of Convolutional layer.
    void SetIh(const int input_height) { input_height_ = input_height; }
    //! @brief                  Set input channel value in Parameter class.
    //! @param input_channel    Input channel of Convolutional layer.
    void SetIc(const int input_channel) { input_channel_ = input_channel; }

    //! @brief                  Set padding width value in Parameter class.
    //! @param pad_width        Padding width
    void SetPw(const int pad_width) { padding_width_ = pad_width; };
    //! @brief                  Set padding height value in Parameter class.
    //! @param pad_height       Padding height
    void SetPh(const int pad_height) { padding_height_ = pad_height; }

    //! @brief                  Set kernel width value in Parameter class.
    //! @param kernel_width     Kernel width of Convolutional layer.
    void SetKw(const int kernel_width) { kernel_width_ = kernel_width; }
    //! @brief                  Set kernel height value in Parameter class.
    //! @param kernel_height    Kernel height of Convolutional layer.
    void SetKh(const int kernel_height) { kernel_height_ = kernel_height; }
    
    //! @brief                  Set output channel value in Parameter class.
    //! @param output_channel   Output channel of Convolutional layer.
    void SetOc(const int output_channel) { output_channel_ = output_channel; }

    //! @brief              Set MAC cycles.
    //! @param mac_cycles   MAC cycles.
    void SetMacCycles(const int mac_cycles) { mac_cycles_ = mac_cycles; }
    //! @brief              Set hardware frequency.
    //! @param frequency    Processing element frequency. The unit is GHz.
    void SetFrequency(const double frequency) { frequency_ = frequency; }
    //! @brief              Set off-chip memory bandwidth.
    //! @param bandwidth    Off-chip memory bandwidth. The unit is GB/sec.
    void SetBandwidth(const double bandwidth) { bandwidth_ = bandwidth; }
    //! @brief                      Set input memory size.
    //! @param input_mem_size       Input memory size.
    void SetInputMemSize(const double input_mem_size) 
      { input_mem_size_ = input_mem_size; }
    //! @brief                      Set weight memory size.
    //! @param weight_mem_size      Weight memory size.
    void SetWeightMemSize(const double weight_mem_size)
      { weight_mem_size_ = weight_mem_size; }
    //! @brief                      Set output memory size.
    //! @param output_mem_size      Output memory size.
    void SetOutputMemSize(const double output_mem_size)
      { output_mem_size_ = output_mem_size; }
    //! @brief                      Set PE dimension
    //! @param pe_dim               PE dimension
    void SetPeDim(const vector<vector<int>> pe_dim) { pe_dim_ = pe_dim; }
    //! @brief                      Set PE calculatkon structure
    //! @param pe_strt              PE calculation structure
    void SetPeStructure(const vector<vector<int>> pe_strt) {pe_strt_ = pe_strt;}

    //! @brief              Set path of latency recording file. 
    //! @details            This file provides interface 
    //!                     between code generation mode execution and simulation mode execution.
    //! @param file_path    Latency recording file path.
    void SetLatencyFile(const char* file_path)
      { strncpy(latency_file_, file_path, STR_LEN); }
    //! @brief                          Set tiling factor dump file path.
    //! @param file_path                Tiling factor dump file path.
    void SetTilingDumpFile(const char* file_path)
      { strncpy(tiling_dump_file_, file_path, STR_LEN); }
    //! @brief                                  Set intra loop sequence dump file path.
    //! @param file_path                        Intra loop structure dump file path.
    void SetLoopSequenceDumpFile(const char* file_path)
      { strncpy(loop_seq_dump_file_, file_path, STR_LEN); }

    //! @brief                  Set test case name.
    //! @details                Actually layer name.
    //! @param layer_name       CNN layer name.
    void SetLayerName(const char* layer_name) 
      { strncpy(layer_name_, layer_name, STR_LEN); }

    /**************************************************************************/
    //                               GETTER                                   //
    /**************************************************************************/
    //! @brief      Return stride.
    //! @return     Stride value.    
    int GetStride(void) const { return stride_; }
    
    //! @brief      Return input width.
    //! @return     Input width.
    int GetIw(void) const { return input_width_; }
    //! @brief      Return input height.
    //! @return     Input height.
    int GetIh(void) const { return input_height_; }
    //! @brief      Return input channel.
    //! @return     Input channel.
    int GetIc(void) const { return input_channel_; }

    //! @brief      Return padding width.
    //! @return     Padding width.
    int GetPw(void) const { return padding_width_; }
    //! @brief      Return padding height.
    //! @return     Padding height.
    int GetPh(void) const { return padding_height_; }

    //! @brief      Return kernel width.
    //! @return     Kernel width.
    int GetKw(void) const { return kernel_width_; }
    //! @brief      Return kernel height.
    //! @return     Kernel height.
    int GetKh(void) const { return kernel_height_; }

    //! @brief      Return output channel.
    //! @return     Output channel.
    int GetOc(void) const { return output_channel_; }

    //! @brief              Return MAC cycles.
    //! @return             MAC cycles.
    int GetMacCycles(void) const { return mac_cycles_; }
    //! @brief      Return frequency.
    //! @return     Processing element frequency. The unit is GHz.
    double GetFrequency(void) const { return frequency_; }
    //! @brief      Return off-chip bandwidth.
    //! @return     Off-chip memory bandwidth. The unit is GB/sec.
    double GetBandwidth(void) const { return bandwidth_; }
    //! @brief      Return input on-chip memory size
    //! @return     Input on-chip memory size.
    double GetInputMemSize(void) const { return input_mem_size_; }
    //! @brief      Return weight on-chip memory size
    //! @return     Weight on-chip memory size.
    double GetWeightMemSize(void) const { return weight_mem_size_; }
    //! @brief      Return output on-chip memory size
    //! @return     Output on-chip memory size.
    double GetOutputMemSize(void) const { return output_mem_size_; }
    //! @brief      Return PE dimension (2D)
    //! @return     PE dimension
    vector<vector<int>> GetPeDim(void) const { return pe_dim_; }
    //! @brief      Return PE calculation structure (2D)
    //! @return     PE calculation structure
    vector<vector<int>> GetPeStructure(void) const { return pe_strt_; }

    //! @brief      Return latency file path.
    //! @return     Latency recording file path.
    const char* GetLatencyFile(void) const { return latency_file_; }
    //! @brief      Return tiling factor dump file path.
    //! @return     Tiling factor dump file path.
    const char* GetTilingDumpFile(void) const { return tiling_dump_file_; }
    //! @brief      Return Intra loop sequence dump file path.
    //! @return     Intra loop sequence dump file path.
    const char* GetLoopSequenceDumpFile(void) const 
      { return loop_seq_dump_file_; }

    //! @brief      Return test case name.
    //! @detailes   Actually layer name.
    //! @return     Test case name.
    const char* GetLayerName(void) const { return layer_name_; }

  protected:
    int stride_ = NON_VALID;

    int input_width_ = NON_VALID;
    int input_height_ = NON_VALID;
    int input_channel_ = NON_VALID;

    int padding_width_ = NON_VALID;
    int padding_height_ = NON_VALID;

    int kernel_width_ = NON_VALID;
    int kernel_height_ = NON_VALID;

    int output_channel_ = NON_VALID;

    int mac_cycles_ = NON_VALID;
    double frequency_ = NON_VALID;
    double bandwidth_ = NON_VALID;
    double input_mem_size_ = NON_VALID;   // KB
    double weight_mem_size_ = NON_VALID;  // KB
    double output_mem_size_ = NON_VALID;  // KB
    vector<vector<int>> pe_dim_;
    vector<vector<int>> pe_strt_;

    char latency_file_[STR_LEN] = "";
    char tiling_dump_file_[STR_LEN] = "";
    char loop_seq_dump_file_[STR_LEN] = "";
    
    char layer_name_[STR_LEN] = "";
};
} // namespace parameter
#endif