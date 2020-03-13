#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <assert.h>

using namespace std;

struct data4d {
  int n;
  int c;
  int h;
  int w;
};

int main(int argc, char* argv[])
{
  struct data4d input, weight, output;
  FILE* gaia_f = fopen(argv[1], "r");
  ofstream csv_f("tiling_report.csv", ios::app);

  assert(gaia_f != NULL);
  assert(csv_f.is_open());

  cerr << "Files are opened" << endl;

  char buf[256];

  fscanf(gaia_f, "%s\n", buf); // consume [var]
  fscanf(gaia_f, "%s\n", buf); // consume IN_MEM_0
  fscanf(gaia_f, "%s\n", buf); // consume WT_MEM_0
  fscanf(gaia_f, "%s\n", buf); // consume OT_MEM_0

  cerr << "Consume memory variables" << endl;

  int n,c,h,w;
  fscanf(gaia_f, "INPUT_0(NCHW,0,%d,%d,%d,%d)\n", &(input.n), &(input.c), &(input.h), &(input.w));
  fscanf(gaia_f, "WEIGHT_0(NCHW,0,%d,%d,%d,%d)\n", &(weight.n), &(weight.c), &(weight.h), &(weight.w));
  fscanf(gaia_f, "OUTPUT_0(NCHW,0,%d,%d,%d,%d)\n", &(output.n), &(output.c), &(output.h), &(output.w));

  cerr << "Parse original data" << endl;

  vector<struct data4d> in_t_vec;
  vector<struct data4d> wt_t_vec;
  vector<struct data4d> ot_t_vec;

  fscanf(gaia_f, "%s\n", buf);
  while (buf[0] == 'I') {
    int t_num;
    int addr;
    struct data4d input_t;
    sscanf(buf, "INPUT_0_%d(NCHW,%d,%d,%d,%d,%d)\n", &t_num, &addr, &(input_t.n), &(input_t.c), &(input_t.h), &(input_t.w));
    in_t_vec.push_back(input_t);
    fscanf(gaia_f, "%s\n", buf);
  }
  
  cerr << "Parse input tiles" << endl;

  while (buf[0] == 'W') {
    int t_num;
    int addr;
    struct data4d weight_t;
    sscanf(buf, "WEIGHT_0_%d(NCHW,%d,%d,%d,%d,%d)\n", &t_num, &addr, &(weight_t.n), &(weight_t.c), &(weight_t.h), &(weight_t.w));
    wt_t_vec.push_back(weight_t);
    fscanf(gaia_f, "%s\n", buf);
  }

  cerr << "Parse weight tiles" << endl;

  while (buf[0] == 'O') {
    int t_num;
    int addr;
    struct data4d output_t;
    sscanf(buf, "OUTPUT_0_%d(NCHW,%d,%d,%d,%d,%d)\n", &t_num, &addr, &(output_t.n), &(output_t.c), &(output_t.h), &(output_t.w));
    ot_t_vec.push_back(output_t);
    fscanf(gaia_f, "%s\n", buf);
  }
  
  cerr << "Parse output tiles" << endl;

  struct data4d input_t, weight_t, output_t;

  if (input.h == in_t_vec[0].h) {
    input_t = in_t_vec[0];
  } else
  if (input.c == in_t_vec[0].c) {
    if (in_t_vec.size() == 2) input_t = in_t_vec[0];
    else input_t = in_t_vec[1];
  } else {
    int cnt=1;
    while (in_t_vec[0].h == in_t_vec[cnt].h) {
      cnt++;
    }
    if (cnt == 1 || cnt == 2) input_t = in_t_vec[0];
    else input_t = in_t_vec[1];
  }
  
  weight_t = wt_t_vec[0];
  output_t = ot_t_vec[0];

  cerr << "Pick representative tile" << endl;

  char sep = ',';

  csv_f << argv[1] << sep
        << input_t.w << sep << "/" << sep << input.w << sep
        << input_t.h << sep << "/" << sep << input.h << sep
        << input_t.c << sep << "/" << sep << input.c << sep
        << weight_t.w << sep << "/" << sep << weight.w << sep
        << weight_t.h << sep << "/" << sep << weight.h << sep
        << output_t.w << sep << "/" << sep << output.w << sep
        << output_t.h << sep << "/" << sep << output.h << sep
        << output_t.c << sep << "/" << sep << output.c << endl;

  cerr << "Dump CSV file" << endl;

  fclose(gaia_f);
  csv_f.close();

  cerr << "Parse " << argv[1] << " completed." << endl;

  return 0;
}
