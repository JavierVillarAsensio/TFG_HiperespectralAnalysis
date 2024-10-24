#!/bin/bash
file=static_times.txt
rm -f $file

echo "Performance with 4 spectrums (proposed result)" > $file
{ time make performance -C ..; } 2>> $file

echo -e "\n\nPerformance with 8 spectrums" >> $file
cp ../perf_specs/1/* ../spectrums
{ time make performance -C ..; } 2>> $file

echo -e "\n\nPerformance with 12 spectrums" >> $file
cp ../perf_specs/2/* ../spectrums
{ time make performance -C ..; } 2>> $file

echo -e "\n\nPerformance with 16 spectrums" >> $file
cp ../perf_specs/3/* ../spectrums
{ time make performance -C ..; } 2>> $file

echo -e "\n\nPerformance with 20 spectrums" >> $file
cp ../perf_specs/4/* ../spectrums
{ time make performance -C ..; } 2>> $file

sed -i '/sys/d' $file
sed -i '/user/d' $file
rm ../spectrums/meteorites*