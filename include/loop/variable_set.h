#ifndef CNNPLANNER_LOOP_VARIABLE_SET_H_
#define CNNPLANNER_LOOP_VARIABLE_SET_H_

#include "parameter/parameter.h"
#include "loop/variables.h"

using parameter::Parameter;
using loop::Variables;

namespace loop{
////////////////////////////////////////////////////////////////////////////////
//! @brief      Set of CNN parameters that includes three of loop variables.
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
class VariableSet
{
  public:
    VariableSet(void) {}
    //! @brief        Constructor using parameter data.
    //! @param param  Parameter object parsed by parser.
    VariableSet(const Parameter& param);

    //! @brief          Set stride value.
    //! @param stride   Stride value
    void SetStride(const int stride);

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

    //! @brief                    Set tile input width value.
    //! @param tile_input_width   Tile input width value.
    void SetTiw(const int tile_input_width);
    //! @brief                    Set tile input height value.
    //! @param tile_input_height  Tile input height value.
    void SetTih(const int tile_input_height);
    //! @brief                    Set tile input channel value.
    //! @param tile_input_channel Tile input channel value.
    void SetTic(const int tile_input_channel);

    //! @brief                      Set tile kernel width value.
    //! @param tile_kernel_width    Tile kernel width value.
    void SetTkw(const int tile_kernel_width);
    //! @brief                      Set tile kernel height value.
    //! @param tile_kernel_height   Tile kernel height value.
    void SetTkh(const int tile_kernel_height);

    //! @brief                      Set tile output width value.
    //! @param tile_output_width    Tile output width value.
    void SetTow(const int tile_output_width);
    //! @brief                      Set tile output height value.
    //! @param tile_output_height   Tile output height value.
    void SetToh(const int tile_output_height);
    //! @brief                      Set tile output channel value.
    //! @param tile_output_channel  Tile output channel value.
    void SetToc(const int tile_output_channel);

    //! @brief                          Set unroll input width value.
    //! @param parallel_input_width     Unroll input width value.
    void SetPiw(const int parallel_input_width);
    //! @brief                          Set unroll input height value.
    //! @param parallel_input_height    Unroll input height value.
    void SetPih(const int parallel_input_height);
    //! @brief                          Set unroll input channel value.
    //! @param parallel_input_channel   Unroll input channel value.
    void SetPic(const int parallel_input_channel);

    //! @brief                          Set unroll kernel width value.
    //! @param parallel_kernel_width    Unroll kernel width value.
    void SetPkw(const int parallel_kernel_width);
    //! @brief                          Set unroll kernel height value.
    //! @param parallel_kernel_height   Unroll kernel height value.
    void SetPkh(const int parallel_kernel_height);
        
    //! @brief                          Set unroll output width value.
    //! @param parallel_output_width    Unroll output width value.
    void SetPow(const int parallel_output_width);
    //! @brief                          Set unroll output height value.
    //! @param parallel_output_height   Unroll output height value.
    void SetPoh(const int parallel_output_height);
    //! @brief                          Set unroll output channel value.
    //! @param parallel_output_channel  Unroll output channel value.
    void SetPoc(const int parallel_output_channel);

    //! @brief                  Set off-chip loop variables data which provides interface between off-chip and on-chip.
    //! @param off_loop_vars    Off-chip loop parameters.
    void SetOffLoopVariables(loop::Variables off_loop_vars);
    //! @brief                  Set on-chip loop variables data which provides interface between on-chip and register file.
    //! @param on_loop_vars     On-chip loop parameters.
    void SetOnLoopVariables(loop::Variables on_loop_vars);
    //! @brief                  Set parallelizaion loop variables data.
    //! @param parl_loop_vars   Parallelizaion loop parameters.
    void SetParlLoopVariables(loop::Variables parl_loop_vars);

    //! @brief  Return stride value.
    //! @return Stride value.
    int GetStride(void) const;

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

    //! @brief  Return tile input width value.
    //! @return Tile input width value.
    int GetTiw(void) const;
    //! @brief  Return tile input height value.
    //! @return Tile input height value.
    int GetTih(void) const;
    //! @brief  Return tile input channel value.
    //! @return Tile input channel value.
    int GetTic(void) const;

    //! @brief  Return tile kernel width value.
    //! @return Tile kernel width value.
    int GetTkw(void) const;
    //! @brief  Return tile kernel height value.
    //! @return Tile kernel height value.
    int GetTkh(void) const;

    //! @brief  Return tile output width value.
    //! @return Tile output width value.
    int GetTow(void) const;
    //! @brief  Return tile output height value.
    //! @return Tile output height value.
    int GetToh(void) const;
    //! @brief  Return tile output channel value.
    //! @return Tile output channel value.
    int GetToc(void) const;

    //! @brief  Return unroll input width value.
    //! @return Untoll input width value.
    int GetPiw(void) const;
    //! @brief  Return unroll input height value.
    //! @return Untoll input height value.
    int GetPih(void) const;
    //! @brief  Return unroll input channel value.
    //! @return Untoll input channel value.
    int GetPic(void) const;

    //! @brief  Return unroll kernel width value.
    //! @return Untoll kernel width value.
    int GetPkw(void) const;
    //! @brief  Return unroll kernel height value.
    //! @return Untoll kernel height value.
    int GetPkh(void) const;

    //! @brief  Return unroll output width value.
    //! @return Untoll output width value.
    int GetPow(void) const;
    //! @brief  Return unroll output height value.
    //! @return Untoll output height value.
    int GetPoh(void) const;
    //! @brief  Return unroll output channel value.
    //! @return Untoll output channel value.
    int GetPoc(void) const;

    //! @brief  Return off-chip loop variables which provides interface between off-chip and on-chip.
    //! @return Off-chip loop variables
    const loop::Variables& GetOffLoopVariables(void) const;
    //! @brief  Return On-chip loop variables which provides interface between on-chip and register file.
    //! @return On-chip loop variables
    const loop::Variables& GetOnLoopVariables(void) const;
    //! @brief  Return parallelization loop variables.
    //! @return Parallelization loop variables
    const loop::Variables& GetParlLoopVariables(void) const;

    /*
    //! @brief            Dump tiling factor.
    //! @param dump_file  Dump file path.
    void DumpTilingFactor(const char* dump_file);
    */

    //! @brief  Check loop structure validation.
    void CheckValid(void) const;

  private:
    loop::Variables off_loop_vars_;
    loop::Variables on_loop_vars_;
    loop::Variables parl_loop_vars_;

    void CheckVariablesRange( const loop::Variables& upper, 
                              const loop::Variables& lower) const;
};
ostream& operator<<(ostream& out, const loop::VariableSet& varset);
} // namespace loop
#endif
