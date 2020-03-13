#ifndef CNNPLANNER_PARAMETER_COMPILER_PARAMETER_H_
#define CNNPLANNER_PARAMETER_COMPILER_PARAMETER_H_

#include <vector>

#include "parameter/parameter.h"
#include "general/data_type.h"

#define STR_LEN 256

using std::vector;

namespace parameter {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Parameters for compiler.
//! @details  <b>Abbreviations</b><br>
//!           IW: Input Width<br>
//!           IH: Input Height<br>
//!           IC: Input Channel<br>
//!           KW: Kernel Width<br>
//!           KH: Kernel Height<br>
//!           OW: Output Width<br>
//!           OH: Output Height<br>
//!           OC: Output Channel<br>
//! @author   Minsu Kim
//! @date     2019-11-26
////////////////////////////////////////////////////////////////////////////////
class CompilerParameter : public parameter::Parameter
{
  public:
    /**************************************************************************/
    //                               SETTER                                   //
    /**************************************************************************/
    //! @brief              Set path of generated code file.
    //! @param file_path    Generated code file path.
    void SetCodeFile(const char* file_path) 
      { strncpy(code_file_, file_path, STR_LEN); }
    //! @brief              Set path of Gaia IR code file.
    //! @param file_path    Gaia IR code file path.
    void SetGaiaFile(const char* file_path)
      { strncpy(gaia_file_, file_path, STR_LEN); }
    //! @brief              Set path of timestamp JSON file.
    //! @param file_path    Timestamp JSON file path.
    void SetTimestampFile(const char* file_path)
      { strncpy(timestamp_file_, file_path, STR_LEN); }
    //! @brief              Whether schedule or read from dump file.
    //! @param pre_sched    If pre_sched flag is true, 
    //!                     tiling and loop structure are read from dump file.
    void SetPreScheduled(const bool pre_sched)
      { pre_sched_ = pre_sched; }

    /**************************************************************************/
    //                               GETTER                                   //
    /**************************************************************************/
    //! @brief              Return generated source code file path.
    //! @return             Generated source code file path.
    const char* GetCodeFile(void) const { return code_file_; }
    //! @brief              Return Gaia IR source code file path.
    //! @return             Gaia IR source code file path.
    const char* GetGaiaFile(void) const { return gaia_file_; }
    //! @brief              Return timestamp JSON file path.
    //! @return             Timestamp JSON file path.
    const char* GetTimestampFile(void) const { return timestamp_file_; }
    //! @brief              Return pre_sched_ flag.
    //! @return             pre_sched_ flag.
    const bool GetPreScheduled(void) const { return pre_sched_; }

  private:
    char code_file_[STR_LEN] = "";
    char gaia_file_[STR_LEN] = "";
    char timestamp_file_[STR_LEN] = "";

    bool pre_sched_ = false;
};
} // namespace parameter
#endif