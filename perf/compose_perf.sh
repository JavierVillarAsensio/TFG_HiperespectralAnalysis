#!/bin/bash
rm -f compose_times.txt

echo "Performance with 4 spectrums (proposed result) with docker compose:" > compose_times.txt
pushd ..
{ time ./run.sh; } 2>> perf/compose_times.txt
popd
'
make clean -C ..


echo -e "\n\nPerformance with 8 spectrums with docker compose:" >> compose_times.txt
cp ../perf_specs/1/* ../spectrums
pushd ..
{ time ./run.sh; } 2>> perf/compose_times.txt
popd
make clean -C ..

echo -e "\n\nPerformance with 12 spectrums with docker compose:" >> compose_times.txt
cp ../perf_specs/2/* ../spectrums
pushd ..
{ time ./run.sh; } 2>> perf/compose_times.txt
popd
make clean -C ..

echo -e "\n\nPerformance with 16 spectrums with docker compose:" >> compose_times.txt
cp ../perf_specs/3/* ../spectrums
pushd ..
{ time ./run.sh; } 2>> perf/compose_times.txt
popd
make clean -C ..

echo -e "\n\nPerformance with 20 spectrums with docker compose:" >> compose_times.txt
cp ../perf_specs/4/* ../spectrums
pushd ..
{ time ./run.sh; } 2>> perf/compose_times.txt
popd
make clean -C ..
'
sed -i '/Container/d' compose_times.txt
sed -i '/Network/d' compose_times.txt
sed -i '/sys/d' compose_times.txt
sed -i '/user/d' compose_times.txt
rm ../spectrums/meteorites*