#ifndef CNNPLANNER_LOOP_STRUCTURE_H_
#define CNNPLANNER_LOOP_STRUCTURE_H_

#include <iostream>

#include "loop/variables.h"

using std::ostream;
using std::istream;

namespace loop {
enum Location {
  INNER_MOST=0,
  SECOND_INNER_MOST,
  THIRD_INNER_MOST,
  OUTER_MOST
};

enum Type {
  KERNEL_MAP=0,
  INPUT_CHANNEL,
  OUTPUT_MAP,
  OUTPUT_CHANNEL
};

#define S_INNER_MOST          "Inner_Most"
#define S_SECOND_INNER_MOST   "Second_Inner_Most"
#define S_THIRD_INNER_MOST    "Third_Inner_Most"
#define S_OUTER_MOST          "Outer_Most"

#define S_KERNEL_MAP      "Kernel_Map"
#define S_INPUT_CHANNEL   "Input_Channel"
#define S_OUTPUT_MAP      "Output_Map"
#define S_OUTPUT_CHANNEL  "Output_Channel"

////////////////////////////////////////////////////////////////////////////////
//! @brief    Structure of CNN loop.
//! @details  This class contains loop sequence and fully tiled information
//! @author   Minsu Kim
//! @date     2019-07-03
////////////////////////////////////////////////////////////////////////////////
class Structure
{
  public:
    //! @brief        Bind loop type and loop location.
    //! @param type   Loop type (e.g. KERNEL_MAP, INPUT_CHANNEL, OUTPUT_MAP, OUTPUT_CHANNEL)
    //! @param loc    Loop location (e.g. INNER_MOST, SECOND_INNER_MOST, THIRD_INNER_MOST, OUTER_MOST)
    void Bind(const loop::Type type, const loop::Location loc);

    //! @brief    Tag fully tiled to KERNEL_MAP
    void SetKmFullyTiled(void);
    //! @brief    Tag fully tiled to INPUT_CHANNEL
    void SetIcFullyTiled(void);
    //! @brief    Tag fully tiled to OUTPUT_MAP
    void SetOmFullyTiled(void);
    //! @brief    Tag fully tiled to OUTPUT_CHANNEL
    void SetOcFullyTiled(void);

    //! @brief    Tag weight stationary.
    void SetWeightStationary(void);
    //! @brief    Tag output stationary.
    void SetOutputStationary(void);
    //! @brief    Tag input stationary.
    void SetInputStationary(void);

    //! @brief    Return loop location of KERNEL_MAP
    //! @return   Loop location of KERNEL_MAP
    loop::Location GetKernelMap(void) const;
    //! @brief    Return loop location of INPUT_CHANNEL
    //! @return   Loop location of INPUT_CHANNEL
    loop::Location GetInputChannel(void) const;
    //! @brief    Return loop location of OUTPUT_MAP
    //! @return   Loop location of OUTPUT_MAP
    loop::Location GetOutputMap(void) const;
    //! @brief    Return loop location of OUTPUT_CHANNEL
    //! @return   Loop location of OUTPUT_CHANNEL
    loop::Location GetOutputChannel(void) const;

    //! @brief    Return loop type of INNER_MOST
    //! @return   Loop type of INNER_MOST
    loop::Type GetInnerMost(void) const;
    //! @brief    Return loop type of SECOND_INNER_MOST
    //! @return   Loop type of SECOND_INNER_MOST
    loop::Type GetSecondInnerMost(void) const;
    //! @brief    Return loop type of THIRD_INNER_MOST
    //! @return   Loop type of THIRD_INNER_MOST
    loop::Type GetThirdInnerMost(void) const;
    //! @brief    Return loop type of OUTER_MOST
    //! @return   Loop type of OUTER_MOST
    loop::Type GetOuterMost(void) const;

    //! @brief      Return fully tiled tag of specific location.
    //! @param loc  Loop location which you want to know whether fully tiled or not.
    //! @return     Fully tiled tag of specific location
    bool IsFullyTiled(loop::Location loc) const;
    //! @brief        Return fully tiled tag of specific type.
    //! @param type   Loop type which you want to know whether fully tiled or not.
    //! @return       Fully tiled tag of specific type.
    bool IsFullyTiled(loop::Type type) const;

    //! @brief  Return weight stationary tag.
    //! @return Weight stationary tag
    bool IsWeightStationary(void) const;
    //! @brief  Return output stationary tag.
    //! @return Output stationary tag
    bool IsOutputStationary(void) const;
    //! @brief  Return input stationary tag.
    //! @return Input stationary tag
    bool IsInputStationary(void) const;

    //! @brief        Return loop location of specific loop type.
    //! @param type   Loop type which you want to know its location.
    //! @return       Loop location of specific loop type
    loop::Location Get(loop::Type type) const;
    //! @brief        Return loop type of specific loop location.
    //! @param loc    Loop location which you want to know its type.
    //! @return       Loop type of specific loop location
    loop::Type Get(loop::Location loc) const;

    //! @brief        Move specific type of loop inner most.
    //! @details      This function is used when you want to move fully tiled loop to inner most.
    //! @param type   Loop type which you want to move it to inner most.
    void MoveToInnerMost(loop::Type type);

    //! @brief        Tag stationary flag depending on fully tilied.
    //! @param upper  Upper side loop.
    //! @param lower  Lower side loop.
    void TagStationary( const loop::Variables& upper, 
                        const loop::Variables& lower);

    //! @brief      Return loop location representation to string type.
    //! @param loc  Loop location you want to represent to string type.
    //! @return     String type of specific location.
    const char* ToString(const loop::Location loc) const;
    //! @brief      Return loop type representation to string type.
    //! @param loc  Loop type you want to represent to string type.
    //! @return     String type of specific type.
    const char* ToString(const loop::Type type) const;

    //! @brief  Check whether the location tags are valid.
    void CheckLocationValid(void) const;
    //! @brief  Check whether the type tags are valid.
    void CheckTypeValid(void) const;

  private:
    loop::Location kernel_map_;
    loop::Location input_channel_;
    loop::Location output_map_;
    loop::Location output_channel_;

    loop::Type inner_most_;
    loop::Type second_inner_most_;
    loop::Type third_inner_most_;
    loop::Type outer_most_;

    bool km_fully_tiled_ = false;
    bool ic_fully_tiled_ = false;
    bool om_fully_tiled_ = false;
    bool oc_fully_tiled_ = false;

    bool is_weight_stationary_ = false;
    bool is_output_stationary_ = false;
    bool is_input_stationary_ = false;

    void Swap(loop::Location a, loop::Location b);

    void CheckLocationValid(loop::Location loc) const;
    void CheckTypeValid(loop::Type type) const;

    //void ParseLoopSequence(const int loop_sequence);
};
loop::Location& operator++(loop::Location& loc);

ostream& operator<<(ostream& out, const loop::Location& loc);
ostream& operator<<(ostream& out, const loop::Type& type);
ostream& operator<<(ostream& out, const loop::Structure& strt);
istream& operator>>(istream& in, loop::Structure& strt);
} // namespace loop
#endif
