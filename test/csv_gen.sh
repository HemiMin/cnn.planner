#/bin/bash

csv_gen=$(pwd)/../build/csv_gen
cd $1;
gaias=($(ls *.gaia))
echo ${gaias[@]}

for gaia in "${gaias[@]}"; do
  $csv_gen $gaia
done

