#pragma once

#include <vector>
#include <fstream>

#include "statistic/csv_object.h"

using std::vector;
using std::ofstream;

using statistic::CsvObject;

namespace statistic {
class CsvWriter
////////////////////////////////////////////////////////////////////////////////
//! @brief    Class to generate CSV file.
//! @details  This class generates whole CSV file.
//! @author   Minsu Kim
//! @date     2019-07-03
////////////////////////////////////////////////////////////////////////////////
{
  public:
    //! @brief            Generate CSV file.
    //! @details          Generate CSV file from collection of CsvObject.
    //! @param csv_file   CSV file path.
    //! @param csv_objs   Collection of CsvObject.
    //! @return           EXIT_SUCCESS or EXIT_FAILURE
    int WriteCsv(const char* csv_file, const vector<CsvObject> csv_objs);
    //! @brief            Generate CSV file.
    //! @details          Generate CSV file from one CsvObject.
    //! @param csv_file   CSV file path.
    //! @param csv_objs   CsvObject.
    //! @return           EXIT_SUCCESS or EXIT_FAILURE
    int WriteCsv(const char* csv_file, const CsvObject& csv_obj);
  private:
    void WriteCsvFrame(ofstream* csv_file_stream);
};
} // namespace statistic
