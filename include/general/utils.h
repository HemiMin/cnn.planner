#pragma once

extern int kStride;
extern int kFilter_len;
extern int kPadding;

//! @brief            Get output length correspond to such input, kernel length and stride.
//! @param input_len  Input length.
inline int CalcOutputLength(int input_len)
{
  return (input_len < kFilter_len) ? 
            1 : (input_len+2*kPadding-kFilter_len)/kStride + 1;
}

//! @brief            Get input length correspond to such output, kernel length and stride.
//! @param output_len Output length.
inline int CalcInputLength(int output_len)
{
  return (output_len < 1) ? 
            kFilter_len : (output_len-1)*kStride + kFilter_len - 2*kPadding;
}