#!/bin/bash
rm -f static_times.txt

echo "Performance with 4 spectrums (proposed result)" > static_times.txt
{ time make performance -C ..; } 2>> static_times.txt

echo -e "\n\nPerformance with 8 spectrums" >> static_times.txt
cp ../perf_specs/1/* ../spectrums
{ time make performance -C ..; } 2>> static_times.txt

echo -e "\n\nPerformance with 12 spectrums" >> static_times.txt
cp ../perf_specs/2/* ../spectrums
{ time make performance -C ..; } 2>> static_times.txt

echo -e "\n\nPerformance with 16 spectrums" >> static_times.txt
cp ../perf_specs/3/* ../spectrums
{ time make performance -C ..; } 2>> static_times.txt

echo -e "\n\nPerformance with 20 spectrums" >> static_times.txt
cp ../perf_specs/4/* ../spectrums
{ time make performance -C ..; } 2>> static_times.txt

sed -i '/sys/d' static_times.txt
sed -i '/user/d' static_times.txt
rm ../spectrums/meteorites*