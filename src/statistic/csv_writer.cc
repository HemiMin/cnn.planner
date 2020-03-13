#include "statistic/csv_writer.h"

#include <stdlib.h>
#include <glog/logging.h>
#include <iostream>
#include <fstream>

using statistic::CsvWriter;

using std::endl;
using std::ofstream;

//TODO (MinsuKim): Compact the logging sections.
int CsvWriter::WriteCsv(const char* csv_file, const vector<CsvObject> csv_objs)
{
  /* #region Logging */
  LOG(INFO) << "Write CSV object list." << endl;
  /* #endregion */
  ofstream csv_file_stream(csv_file, std::ios::in);
  if (!csv_file_stream.is_open()) { // csv file is not exist
    csv_file_stream.open(csv_file, std::ios::out);
    WriteCsvFrame(&csv_file_stream);
    /* #region Logging */
    LOG(INFO) << "File " << csv_file << " is generated." << endl;
    /* #endregion */
  } else {
    csv_file_stream.close();
    csv_file_stream.open(csv_file, std::ios::app);
    /* #region Logging */
    LOG(INFO) << "Open exist file " << csv_file << " is opened." << endl;
    /* #endregion */
  }

  if (!csv_file_stream.is_open()) {
    /* #region Logging */
    LOG(ERROR) << "File " << csv_file << " open error." << endl;
    /* #endregion */
    return EXIT_FAILURE;
  }

  for (const CsvObject& csv_obj : csv_objs) {
    csv_file_stream << csv_obj;
  }

  csv_file_stream.close();

  return EXIT_SUCCESS;
}

int CsvWriter::WriteCsv(const char* csv_file, const CsvObject& csv_obj)
{
  /* #region Logging */
  LOG(INFO) << "Write CSV object." << endl;
  /* #endregion */
  ofstream csv_file_stream(csv_file, std::ios::in);
  if (!csv_file_stream.is_open()) {
    csv_file_stream.open(csv_file, std::ios::out);
    WriteCsvFrame(&csv_file_stream);
    /* #region Logging */
    LOG(INFO) << "File " << csv_file << " is generated." << endl;
    /* #endregion */
  } else {
    csv_file_stream.close();
    csv_file_stream.open(csv_file, std::ios::app);
    /* #region Logging */
    LOG(INFO) << "Open exist file " << csv_file << " is opened." << endl;
    /* #endregion */
  }

  if (!csv_file_stream.is_open()) {
    return EXIT_FAILURE;
  }

  csv_file_stream << csv_obj;

  csv_file_stream.close();

  return EXIT_SUCCESS;
}

void CsvWriter::WriteCsvFrame(ofstream* csv_file_stream)
{
  *csv_file_stream << "layer_name,"
    << "TIW,TIH,TIC,TKW,TKH,TOW,TOH,TOC,"
    << "spatial_input_reuse,"
    << "spatial_weight_reuse,"
    << "temporal_input_reuse,"
    << "temporal_weight_reuse,"
    << "input_buffer_size,"
    << "weight_buffer_size,"
    << "output_buffer_size,"
    << "total_buffer_size,"
    << "on_chip_input_load_size,"
    << "on_chip_weight_load_size,"
    << "on_chip_psum_load_store_size,"
    << "on_chip_output_store_size,"
    << "on_chip_total_access_size,"
    << "off_chip_input_load_size,"
    << "off_chip_weight_load_size,"
    << "off_chip_output_store_size,"
    << "off_chip_total_access_size,"
    << "computation_roof,"
    << "opt_arithmetic_intensity,"
    << "arithmetic_intensity,"
    << "opt_performance,"
    << "attainable_performance,"
    << "estimated_performance,"
    << "performance,"
    << "active_macs,"
    << "PE_util,"
    << "off_chip_access_energy,"
    << "on_chip_access_energy,"
    << "exe_energy,"
    << "total_energy,"
    << "power,"
    << "IS,"
    << "WS,"
    << "OS"
    << endl;
}