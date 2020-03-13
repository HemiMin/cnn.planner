#ifndef CNNPLANNER_GENERAL_DATATYPE_H_
#define CNNPLANNER_GENERAL_DATATYPE_H_

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

typedef float DataType;

#define MAX_RAND_DATA   ( 1 << (sizeof(DataType) * 2) )

#define NON_VALID -1

class Matrix : public std::vector<std::vector<int>>
{
  public:
    Matrix(char* vec_str, size_t len)
    {
      char* ptr = vec_str;
      //--- Invalid format check
      assert(*ptr == '[' && *(ptr+1) == '[');
      assert(*(ptr+len-1) == ']' && *(ptr+len-2) == ']');
      //---
      char* s_ptr = nullptr; // start pointer of 1D vector
      char* e_ptr = nullptr; // end pointer of 1D vector
      char* vec_s = nullptr; // 1D vector string
      while (++ptr < vec_str+len-1) {
        if (*ptr == '[')  s_ptr = ptr;
        if (*ptr == ']') {
          e_ptr = ptr;
          assert(s_ptr != nullptr && s_ptr < e_ptr);
          size_t vec_size = (size_t)(e_ptr-s_ptr);
          vec_s = new char[vec_size];
          memcpy(vec_s, s_ptr, vec_size+1);
          this->push_back(TransStrToVec(vec_s, vec_size));
          delete vec_s;
        }
      }
    }

  private:
    std::vector<int> TransStrToVec(char* vec_str, size_t len)
    {
      char* ptr = vec_str;
      std::vector<int> vec;

      //--- Invalid format check
      assert(*ptr == '[');
      assert(*(ptr+len) == ']');
      //---
      while (++ptr < vec_str+len) {
        std::string num = "";
        while (*ptr != ',' && ptr < vec_str+len) {
          if (*ptr >= '0' && *ptr <= '9') num += *ptr;
          ptr++;
        }
        vec.push_back(std::stoi(num));
      }
      return vec;
    }
};

#endif
