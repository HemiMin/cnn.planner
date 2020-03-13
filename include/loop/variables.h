#ifndef CNNPLANNER_LOOP_VARIABLES_H_
#define CNNPLANNER_LOOP_VARIABLES_H_

#include <iostream>

#include "general/data_type.h"

using std::ostream;
using std::istream;

namespace loop{
////////////////////////////////////////////////////////////////////////////////
//! @brief      Set of CNN parameters that includes specific loop variables.
//! @details    <b>Abbreviations</b><br>
//!             IW: Input Width<br>
//!             IH: Input Height<br>
//!             IC: Input Channel<br>
//!             KW: Kernel Width<br>
//!             KH: Kernel Height<br>
//!             OW: Output Width<br>
//!             OH: Output Height<br>
//!             OC: Output Channel<br>
//!             T: Tiled<br>
//!             P: Parallel
//! @author     Minsu Kim
//! @date       2019-07-03
////////////////////////////////////////////////////////////////////////////////
class Variables
{
  public:
    //! @brief                Set input width value.
    //! @param input_width    Input width value
    void SetIw(const int input_width);
    //! @brief                Set input height value.
    //! @param input_height   Input height value
    void SetIh(const int input_height);
    //! @brief                Set input channel value.
    //! @param input_channel  Input channel value
    void SetIc(const int input_channel);

    //! @brief                Set padding width value.
    //! @param pad_width      Padding width value.
    void SetPw(const int pad_width);
    //! @brief                Set padding height value.
    //! @param pad_height     Padding height value.
    void SetPh(const int pad_height);

    //! @brief                Set kernel width value.
    //! @param kernel_width   Kernel width value
    void SetKw(const int kernel_width);
    //! @brief                Set kernel height value.
    //! @param kernel_height  Kernel height value
    void SetKh(const int kernel_height);

    //! @brief                Set output width value.
    //! @param output_width   Output width value
    void SetOw(const int output_width);
    //! @brief                Set output height value.
    //! @param output_height  Output height value
    void SetOh(const int output_height);
    //! @brief                Set output channel value.
    //! @param output_channel Output channel value
    void SetOc(const int output_channel);

    //! @brief          Set stride value.
    //! @param stride   Stride value
    void SetStride(const int stride);

    //! @brief  Return input width value.
    //! @return Input width value.
    int GetIw(void) const;
    //! @brief  Return input height value.
    //! @return Input height value.
    int GetIh(void) const;
    //! @brief  Return input channel value.
    //! @return Input channel value.
    int GetIc(void) const;

    //! @brief  Return padding width value.
    //! @return Padding width value.
    int GetPw(void) const;
    //! @brief  Return padding height value.
    //! @return Padding height value.
    int GetPh(void) const;

    //! @brief  Return kernel width value.
    //! @return Kernel width value.
    int GetKw(void) const;
    //! @brief  Return kernel height value.
    //! @return Kernel height value.
    int GetKh(void) const;

    //! @brief  Return output width value.
    //! @return Output width value.
    int GetOw(void) const;
    //! @brief  Return output height value.
    //! @return Output height value.
    int GetOh(void) const;
    //! @brief  Return output channel value.
    //! @return Output channel value.
    int GetOc(void) const;

    //! @brief  Return stride value.
    //! @return Stride value.
    int GetStride(void) const;

    //! @breif    Return input data size
    //! @return   The number of input data
    long int GetInputSize(void) const;
    //! @brief    Return weight data size
    //! @return   The number of weight data
    long int GetWeightSize(void) const;
    //! @brief    Return output data size
    //! @return   The number of output data
    long int GetOutputSize(void) const;

    //! @brief  Check loop variables validation.
    void CheckValid(void) const;

    /*
    //! @brief  Print loop variables.
    void Print(void) const;

    //! @brief                  Dump loop variables.
    //! @param dump_file_path   Dump file path.
    void Dump(const char* dump_file_path);
    */
  private:
    int input_width_ = NON_VALID;
    int input_height_ = NON_VALID;
    int input_channel_ = NON_VALID;

    int pad_width_ = NON_VALID;
    int pad_height_ = NON_VALID;
        
    int kernel_width_ = NON_VALID;
    int kernel_height_ = NON_VALID;

    int output_width_ = NON_VALID;
    int output_height_ = NON_VALID;
    int output_channel_ = NON_VALID;

    int stride_ = NON_VALID;
};
//! @brief        Overload ostream operator <<.
//! @param out    Output stream
//! @param var    Target loop::Variables instance.
//! @return       Updated output stream.
ostream& operator<<(ostream& out, const loop::Variables& var);
//! @brief        Overload istream operator >>.
//! @detils       Only use when read inter loop variables from file.
//! @param in     Input stream
//! @param var    Target loop::Variables instance.
//! @return       Updated input stream.
istream& operator>>(istream& in, loop::Variables& var);
} // namespace loop
#endif
