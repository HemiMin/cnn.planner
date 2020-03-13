#include "loop/variables.h"

#include <glog/logging.h>
#include <iostream>
#include <fstream>
#include <string>

#include "general/utils.h"

using loop::Variables;

using std::cout;
using std::endl;
using std::ofstream;

void Variables::SetIw(const int input_width)
{
  input_width_ = input_width;
}

void Variables::SetIh(const int input_height)
{
  input_height_ = input_height;
}

void Variables::SetIc(const int input_channel)
{
  input_channel_ = input_channel;
}

void Variables::SetPw(const int pad_width)
{
  pad_width_ = pad_width;
}

void Variables::SetPh(const int pad_height)
{
  pad_height_ = pad_height;
}

void Variables::SetKw(const int kernel_width)
{
  kernel_width_ = kernel_width;
}

void Variables::SetKh(const int kernel_height)
{
  kernel_height_ = kernel_height;
}

void Variables::SetOw(const int output_width)
{
  output_width_ = output_width;
}

void Variables::SetOh(const int output_height)
{
  output_height_ = output_height;
}

void Variables::SetOc(const int output_channel)
{
  output_channel_ = output_channel;
}

void Variables::SetStride(const int stride)
{
  stride_ = stride;
}

int Variables::GetIw(void) const
{
  return input_width_;
}

int Variables::GetIh(void) const
{
  return input_height_;
}

int Variables::GetIc(void) const
{
  return input_channel_;
}

int Variables::GetPw(void) const
{
  return pad_width_;
}

int Variables::GetPh(void) const
{
  return pad_height_;
}

int Variables::GetKw(void) const
{
  return kernel_width_;
}

int Variables::GetKh(void) const
{
  return kernel_height_;
}

int Variables::GetOw(void) const
{
  return output_width_;
}

int Variables::GetOh(void) const
{
  return output_height_;
}

int Variables::GetOc(void) const
{
  return output_channel_;
}

int Variables::GetStride(void) const
{
  return stride_;
}

long int Variables::GetInputSize(void) const
{
  return input_width_ * input_height_ * input_channel_;
}

long int Variables::GetWeightSize(void) const
{
  return kernel_width_ * kernel_height_ * input_channel_ * output_channel_;
}

long int Variables::GetOutputSize(void) const
{
  return output_width_ * output_height_ * output_channel_;
}

void Variables::CheckValid(void) const
{
  CHECK(input_width_ > NON_VALID) << "input width is NON_VALID";
  CHECK(input_height_ > NON_VALID) << "input height is NON_VALID";
  CHECK(input_channel_ > NON_VALID) << "input channel is NON_VALID";

  CHECK(kernel_width_ > NON_VALID) << "kernel width is NON_VALID";
  CHECK(kernel_height_ > NON_VALID) << "kernel height is NON_VALID";

  CHECK(output_width_ > NON_VALID) << "output width is NON_VALID";
  CHECK(output_height_ > NON_VALID) << "output height is NON_VALID";
  CHECK(output_channel_ > NON_VALID) << "output channel is NON_VALID";

  CHECK(stride_ > NON_VALID) << "Stride is NON_VALID";
}

ostream& loop::operator<<(ostream& out, const loop::Variables& var)
{
  out << "Stride: " << var.GetStride() << endl
      << "Input_width: "<< var.GetIw() << endl
      << "Input_heigt: " << var.GetIh() << endl
      << "Input_channel: " << var.GetIc() << endl
      << "Padding_width: " << var.GetPw() << endl
      << "Padding_height: " << var.GetPh() << endl
      << "Kernel_width: " << var.GetKw() << endl
      << "Kernel_height: " << var.GetKh() << endl
      << "Output_width: " << var.GetOw() << endl
      << "Output_height: " << var.GetOh() << endl
      << "Output_channel: " << var.GetOc() << endl;
  return out;
}

istream& loop::operator>>(istream& in, loop::Variables& var)
{
  std::string dummy; // get rid of dummy string in front of value.
  int stride=NON_VALID, 
      input_width=NON_VALID, 
      input_height=NON_VALID, 
      input_channel=NON_VALID,
      pad_width=NON_VALID,
      pad_height=NON_VALID,
      kernel_width=NON_VALID, 
      kernel_height=NON_VALID,
      output_width=NON_VALID, 
      output_height=NON_VALID, 
      output_channel=NON_VALID;
  
  in  >> dummy >> stride
      >> dummy >> input_width
      >> dummy >> input_height
      >> dummy >> input_channel
      >> dummy >> pad_width
      >> dummy >> pad_height
      >> dummy >> kernel_width
      >> dummy >> kernel_height
      >> dummy >> output_width
      >> dummy >> output_height
      >> dummy >> output_channel;
  /* #region Error Check */
  CHECK(stride>NON_VALID)<<"Non-valid stride: "<<stride;
  CHECK(input_width>NON_VALID)<<"Non-valid input width: "<<input_width;
  CHECK(input_height>NON_VALID)<<"Non-valid input height: "<<input_height;
  CHECK(input_channel>NON_VALID)<<"Non-valid input channel: "<<input_channel;
  CHECK(pad_width>NON_VALID)<<"Non-valid padding width: "<<pad_width;
  CHECK(pad_height>NON_VALID)<<"Non-valid padding height: "<<pad_height;
  CHECK(kernel_width>NON_VALID)<<"Non-valid kernel width: "<<kernel_width;
  CHECK(kernel_height>NON_VALID)<<"Non-valid kernel height: "<<kernel_height;
  CHECK(output_width>NON_VALID)<<"Non-valid output width: "<<output_width;
  CHECK(output_height>NON_VALID)<<"Non-valid output height: "<<output_height;
  CHECK(output_channel>NON_VALID)<<"Non-valid output channel: "<<output_channel;
  /* #endregion */
  var.SetStride(stride);
  var.SetIw(input_width);
  var.SetIh(input_height);
  var.SetIc(input_channel);
  var.SetPw(pad_width);
  var.SetPh(pad_height);
  var.SetKw(kernel_width);
  var.SetKh(kernel_height);
  var.SetOw(output_width);
  var.SetOh(output_height);
  var.SetOc(output_channel);

  kStride = stride;
  kFilter_len = kernel_width;
  kPadding = pad_width;

  return in;
}
