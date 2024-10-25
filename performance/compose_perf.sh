#!/bin/bash
file=compose_times.txt
file_path=performance/
rm -f $file

echo "Performance with 4 spectrums (proposed result) with docker compose:" > $file
pushd ..
{ time ./run.sh; } 2>> $file_path$file
popd
make clean -C ..


echo -e "\n\nPerformance with 8 spectrums with docker compose:" >> $file
cp ../perf_specs/1/* ../spectrums
pushd ..
{ time ./run.sh; } 2>> $file_path$file
popd
make clean -C ..

echo -e "\n\nPerformance with 12 spectrums with docker compose:" >> $file
cp ../perf_specs/2/* ../spectrums
pushd ..
{ time ./run.sh; } 2>> $file_path$file
popd
make clean -C ..

echo -e "\n\nPerformance with 16 spectrums with docker compose:" >> $file
cp ../perf_specs/3/* ../spectrums
pushd ..
{ time ./run.sh; } 2>> $file_path$file
popd
make clean -C ..

echo -e "\n\nPerformance with 20 spectrums with docker compose:" >> $file
cp ../perf_specs/4/* ../spectrums
pushd ..
{ time ./run.sh; } 2>> $file_path$file
popd
make clean -C ..

sed -i '/Container/d' $file
sed -i '/Network/d' $file
sed -i '/sys/d' $file
sed -i '/user/d' $file
rm ../spectrums/meteorites*

grep "real" $file > ../charts/compose_times.txt