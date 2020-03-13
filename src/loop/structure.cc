#include "loop/structure.h"

#include <glog/logging.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using loop::Structure;

using std::cout;
using std::endl;
using std::ofstream;
using std::vector;

using loop::Location;
using loop::Type;

void Structure::Bind(const loop::Type type, const loop::Location loc)
{
  CheckLocationValid(loc);
  CheckTypeValid(type);

  switch (type) {
    case KERNEL_MAP:
      kernel_map_ = loc;
      break;
    case INPUT_CHANNEL:
      input_channel_ = loc;
      break;
    case OUTPUT_MAP:
      output_map_ = loc;
      break;
    case OUTPUT_CHANNEL:
      output_channel_ = loc;
      break;
  }

  switch (loc) {
    case INNER_MOST:
      inner_most_ = type;
      break;
    case SECOND_INNER_MOST:
      second_inner_most_ = type;
      break;
    case THIRD_INNER_MOST:
      third_inner_most_ = type;
      break;
    case OUTER_MOST:
      outer_most_ = type;
      break;
    default:
      LOG(ERROR) << "Invalid location: " << loc;
  }
}

void Structure::SetKmFullyTiled(void)
{
  /* #region Logging */
  LOG(INFO) << "Kernel map is set as fully tiled.";
  /* #endregion */
  km_fully_tiled_ = true;
}

void Structure::SetIcFullyTiled(void)
{
  /* #region Logging */
  LOG(INFO) << "Input channel is set as fully tiled.";
  /* #endregion */
  ic_fully_tiled_ = true;
}

void Structure::SetOmFullyTiled(void)
{
  /* #region Logging */
  LOG(INFO) << "Output map is set as fully tiled.";
  /* #endregion */
  om_fully_tiled_ = true;
}

void Structure::SetOcFullyTiled(void)
{
  /* #region Logging */
  LOG(INFO) << "Output channel is set as fully tiled.";
  /* #endregion */
  oc_fully_tiled_ = true;
}

void Structure::SetWeightStationary(void)
{
  /* #region Logging */
  LOG(INFO) << "Set weight stationary.";
  /* #endregion */
  Bind(OUTPUT_MAP, INNER_MOST);
  Bind(KERNEL_MAP, SECOND_INNER_MOST);
  Bind(INPUT_CHANNEL, THIRD_INNER_MOST);
  Bind(OUTPUT_CHANNEL, OUTER_MOST);
  is_weight_stationary_ = true;
}

void Structure::SetOutputStationary(void)
{
  /* #region Logging */
  LOG(INFO) << "Set output stationary.";
  /* #endregion */
  Bind(KERNEL_MAP, INNER_MOST);
  Bind(INPUT_CHANNEL, SECOND_INNER_MOST);
  Bind(OUTPUT_MAP, THIRD_INNER_MOST);
  Bind(OUTPUT_CHANNEL, OUTER_MOST);
  is_output_stationary_ = true;
}

void Structure::SetInputStationary(void)
{
  /* #region Logging */
  LOG(INFO) << "Set input stationary.";
  /* #endregion */
  Bind(OUTPUT_CHANNEL, INNER_MOST);
  Bind(KERNEL_MAP, SECOND_INNER_MOST);
  Bind(INPUT_CHANNEL, THIRD_INNER_MOST);
  Bind(OUTPUT_MAP, OUTER_MOST);
  is_input_stationary_ = true;
}

loop::Location Structure::GetKernelMap(void) const
{
  return kernel_map_;
}

loop::Location Structure::GetInputChannel(void) const
{
  return input_channel_;
}

loop::Location Structure::GetOutputMap(void) const
{
  return output_map_;
}

loop::Location Structure::GetOutputChannel(void) const
{
  return output_channel_;
}

loop::Type Structure::GetInnerMost(void) const
{
  return inner_most_;
}

loop::Type Structure::GetSecondInnerMost(void) const
{
  return second_inner_most_;
}

loop::Type Structure::GetThirdInnerMost(void) const
{
  return third_inner_most_;
}

loop::Type Structure::GetOuterMost(void) const
{
  return outer_most_;
}

bool Structure::IsFullyTiled(loop::Location loc) const
{
  return IsFullyTiled(Get(loc));
}

bool Structure::IsFullyTiled(loop::Type type) const
{
  switch (type) { 
    case KERNEL_MAP:
      return km_fully_tiled_;
    case INPUT_CHANNEL:
      return ic_fully_tiled_;
    case OUTPUT_MAP:
      return om_fully_tiled_;
    case OUTPUT_CHANNEL:
      return oc_fully_tiled_;
    default:
      /* #region Logging */
      LOG(FATAL) << "Invalid type.";
      /* #endregion */
  }
}

bool Structure::IsWeightStationary(void) const
{
  return is_weight_stationary_;
}

bool Structure::IsOutputStationary(void) const
{
  return is_output_stationary_;
}

bool Structure::IsInputStationary(void) const
{
  return is_input_stationary_;
}

loop::Location Structure::Get(loop::Type type) const
{
  switch (type) {
    case KERNEL_MAP:
      return kernel_map_;
    case INPUT_CHANNEL:
      return input_channel_;
    case OUTPUT_MAP:
      return output_map_;
    case OUTPUT_CHANNEL:
      return output_channel_;
    default:
      /* #region Logging */
      LOG(FATAL) << "Invalid type.";
      /* #endregion */
  }
}

loop::Type Structure::Get(loop::Location loc) const
{
  switch (loc) {
    case INNER_MOST:
      return inner_most_;
    case SECOND_INNER_MOST:
      return second_inner_most_;
    case THIRD_INNER_MOST:
      return third_inner_most_;
    case OUTER_MOST:
      return outer_most_;
    default:
      /* #region Logging */
      LOG(FATAL) << "Invalid location.";
      /* #endregion */
  }
}

void Structure::MoveToInnerMost(loop::Type type)
{
  switch (Get(type)) {
    case OUTER_MOST:
      Swap(OUTER_MOST, THIRD_INNER_MOST);
    case THIRD_INNER_MOST:
      Swap(THIRD_INNER_MOST, SECOND_INNER_MOST);
    case SECOND_INNER_MOST:
      Swap(SECOND_INNER_MOST, INNER_MOST);
    case INNER_MOST:
      break;
    default:
      /* #region Logging */
      LOG(FATAL) << "Invalid location.";
      /* #endregion */
  }
}

void Structure::TagStationary(const loop::Variables& upper,
                              const loop::Variables& lower)
{
  if (upper.GetKw() == lower.GetKw() && upper.GetKh() == lower.GetKh())
    km_fully_tiled_ = true;
  if (upper.GetIc() == lower.GetIc())
    ic_fully_tiled_ = true;
  if (upper.GetOw() == lower.GetOw() && upper.GetOh() == lower.GetOh())
    om_fully_tiled_ = true;
  if (upper.GetOc() == lower.GetOc())
    oc_fully_tiled_ = true;

  if (km_fully_tiled_ && oc_fully_tiled_) is_input_stationary_ = true;
  if (om_fully_tiled_)  is_weight_stationary_ = true;
  if (km_fully_tiled_ && ic_fully_tiled_) is_output_stationary_ = true;

  loop::Type inner_most = inner_most_;
  if (IsFullyTiled(inner_most)) inner_most = second_inner_most_;
  if (IsFullyTiled(inner_most)) inner_most = third_inner_most_;
  if (IsFullyTiled(inner_most)) inner_most = outer_most_;

  if (inner_most == OUTPUT_CHANNEL)   is_input_stationary_ = true;
  else if (inner_most == OUTPUT_MAP)  is_weight_stationary_ = true;
  else {
    if (inner_most == KERNEL_MAP) {
      if (ic_fully_tiled_ || GetKernelMap() + 1 == GetInputChannel()) {
        is_output_stationary_ = true;
      }
    } else { // inner_most == INPUT_CHANNEL
      if (km_fully_tiled_ || GetInputChannel() + 1 == GetKernelMap()) {
        is_output_stationary_ = true;
      }
    }
  }
}

void Structure::Swap(loop::Location a, loop::Location b)
{
  loop::Type a_type = Get(a);
  loop::Type b_type = Get(b);
  loop::Location a_loc = a;
  loop::Location b_loc = b;

  Bind(a_type, b_loc);
  Bind(b_type, a_loc);

  CheckLocationValid();
  CheckTypeValid();
}

const char* Structure::ToString(const loop::Location loc) const
{
  switch (loc) {
    case INNER_MOST:
      return S_INNER_MOST;
    case SECOND_INNER_MOST:
      return S_SECOND_INNER_MOST;
    case THIRD_INNER_MOST:
      return S_THIRD_INNER_MOST;
    case OUTER_MOST:
      return S_OUTER_MOST;
    default:
      /* #region Logging */
      LOG(FATAL) << "Invalid location.";
      /* #endregion */
  }
}

const char* Structure::ToString(const loop::Type type) const
{
  switch (type) {
    case KERNEL_MAP:
      return S_KERNEL_MAP;
    case INPUT_CHANNEL:
      return S_INPUT_CHANNEL;
    case OUTPUT_MAP:
      return S_OUTPUT_MAP;
    case OUTPUT_CHANNEL:
      return S_OUTPUT_CHANNEL;
    default:
      /* #region Logging */
      LOG(FATAL) << "Invalid type.";
      /* #endregion */
  }
}

void Structure::CheckLocationValid(void) const
{
  CheckLocationValid(kernel_map_);
  CheckLocationValid(input_channel_);
  CheckLocationValid(output_map_);
  CheckLocationValid(output_channel_);

  CHECK(kernel_map_ != input_channel_) 
    << "Same location error: kernel map and input channel.";
  CHECK(kernel_map_ != output_map_) 
    << "Same location error: kernel map and output map.";
  CHECK(kernel_map_ != output_channel_) 
    << "Same location error: kernel map and output channel";
  CHECK(input_channel_ != output_map_) 
    << "Same location error: input channel and output map.";
  CHECK(input_channel_ != output_channel_)
    << "Same location error: input channel and output channel.";;
  CHECK(output_map_ != output_channel_)
    << "Same location error: output map and output channel.";
}

void Structure::CheckLocationValid(loop::Location loc) const
{
  CHECK(loc>=INNER_MOST && loc<=OUTER_MOST) 
    << "Location enum is invalid: " << loc;
}

void Structure::CheckTypeValid(void) const
{
  CheckTypeValid(inner_most_);
  CheckTypeValid(second_inner_most_);
  CheckTypeValid(third_inner_most_);
  CheckTypeValid(outer_most_);

  CHECK(inner_most_ != second_inner_most_)
    << "Same type error: innter most and second inner most.";
  CHECK(inner_most_ != third_inner_most_)
    << "Same type error: inner most and third inner most.";
  CHECK(inner_most_ != outer_most_)
    << "Same type error: inner most and outer most.";
  CHECK(second_inner_most_ != third_inner_most_)
    << "Same type error: second inner most and third inner most.";
  CHECK(second_inner_most_ != outer_most_)
    << "Same type error: second inner most and outer most.";
  CHECK(third_inner_most_ != outer_most_)
    << "Same type error: third inner most and outer most";
}

void Structure::CheckTypeValid(loop::Type type) const
{
  CHECK(type >= KERNEL_MAP && type <= OUTPUT_CHANNEL)
    << "Type enum is invalid: " << type;
}

/*
//TODO (MinsuKim): This method can be replaced with operator <<.
void Structure::PrintStationary(void) const
{
  if (is_weight_stationary_) {
    cout << "Weight Stationary" << endl;
  }
  if (is_output_stationary_) {
    cout << "Output Stationary" << endl;
  }
  if (is_input_stationary_) {
    cout << "Input Stationary" << endl;
  }
}

//TODO (MinsuSim): This method can be replaced with operator <<.
void Structure::Dump(const char* dump_file)
{
  ofstream dump_file_stream(dump_file, std::ios::app);
  dump_file_stream << kernel_map_ + 1 << input_channel_ + 1
    << output_map_ + 1 << output_channel_ + 1 << endl;
  dump_file_stream.close();
}

//TODO (MinsuKim): This method can be replaced with operator >>.
void Structure::ParseLoopSequence(const int loop_sequence)
{
  Bind(KERNEL_MAP, (LoopLocation)(loop_sequence / 1000 - 1));
  Bind(INPUT_CHANNEL, (LoopLocation)((loop_sequence / 100) % 10 - 1));
  Bind(OUTPUT_MAP, (LoopLocation)((loop_sequence / 10) % 10 - 1));
  Bind(OUTPUT_CHANNEL, (LoopLocation)(loop_sequence % 10 - 1));
  LOG(INFO) << "Loop sequence is " << loop_sequence << endl;
  LOG(INFO) << "Bind kernel map to: " << loop_sequence / 1000 - 1 << endl;
  LOG(INFO) << "Bind input channel to: " << (loop_sequence / 100) % 10 - 1
    << endl;
  LOG(INFO) << "Bind output map to: " << (loop_sequence / 10) % 10 - 1 << endl;
  LOG(INFO) << "Bind output channel to: " << loop_sequence % 10 - 1 << endl;

  CheckTypeValid();
  CheckLocationValid();
}
*/

loop::Location& loop::operator++(loop::Location& loc)
{
  switch (loc) {
    case loop::Location::INNER_MOST:        loc = SECOND_INNER_MOST; break;
    case loop::Location::SECOND_INNER_MOST: loc = THIRD_INNER_MOST;  break;
    case loop::Location::THIRD_INNER_MOST:  loc = OUTER_MOST;        break;
    default: LOG(ERROR) << "Cannot increase loop location: " << loc;
  }
  return loc;
}

ostream& loop::operator<<(ostream& out, const loop::Location& loc)
{
  switch (loc) {
    case loop::Location::INNER_MOST:          out << S_INNER_MOST;        break;
    case loop::Location::SECOND_INNER_MOST:   out << S_SECOND_INNER_MOST; break;
    case loop::Location::THIRD_INNER_MOST:    out << S_THIRD_INNER_MOST;  break;
    case loop::Location::OUTER_MOST:          out << S_OUTER_MOST;        break;
    default:  LOG(ERROR) << "Invalid loop location: " << loc;
  }
  return out;
}

ostream& loop::operator<<(ostream& out, const loop::Type& type)
{
  switch (type) {
    case loop::Type::KERNEL_MAP:      out << S_KERNEL_MAP;      break;
    case loop::Type::INPUT_CHANNEL:   out << S_INPUT_CHANNEL;   break;
    case loop::Type::OUTPUT_MAP:      out << S_OUTPUT_MAP;      break;
    case loop::Type::OUTPUT_CHANNEL:  out << S_OUTPUT_CHANNEL;  break;
    default:  LOG(ERROR) << "Invalid loop type: " << type;
  }
  return out;
}

ostream& loop::operator<<(ostream& out, const loop::Structure& strt)
{
  out << "Inner_most: " << strt.GetInnerMost() << endl
      << "second_inner_most: " << strt.GetSecondInnerMost() << endl
      << "third_inner_most: " << strt.GetThirdInnerMost() << endl
      << "outer_most: " << strt.GetOuterMost() << endl;

  //if (strt.IsWeightStationary())  out << "Weight_Stationary" << endl;
  //if (strt.IsOutputStationary())  out << "Output_Statinoary" << endl;
  //if (strt.IsInputStationary())   out << "Input_Stationary"  << endl;

  return out;
}

istream& loop::operator>>(istream& in, loop::Structure& strt)
{
  std::string dummy; // get rid of dummy string.
  std::string type_str;
  vector<loop::Type> type_seq;

  for (loop::Location loc = INNER_MOST ; loc <= OUTER_MOST ; ++loc) {
    in >> dummy >> type_str; // Scan type string.
    if      (type_str == S_KERNEL_MAP)      strt.Bind(KERNEL_MAP,     loc);
    else if (type_str == S_INPUT_CHANNEL)   strt.Bind(INPUT_CHANNEL,  loc);
    else if (type_str == S_OUTPUT_MAP)      strt.Bind(OUTPUT_MAP,     loc);
    else if (type_str == S_OUTPUT_CHANNEL)  strt.Bind(OUTPUT_CHANNEL, loc);
    else  LOG(FATAL) << "Invalid loop type string: " << type_str;

    if (loc == OUTER_MOST) break;
  }
  /* #region Logging */
  LOG(INFO) << "Bind kernel map to: "     << strt.GetKernelMap();
  LOG(INFO) << "Bind input channel to: "  << strt.GetInputChannel();
  LOG(INFO) << "Bind output map to: "     << strt.GetOutputMap();
  LOG(INFO) << "Bind output channel to: " << strt.GetOutputChannel();
  /* #endregion */
  strt.CheckTypeValid();
  strt.CheckLocationValid();

  return in;
}