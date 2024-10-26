#!/bin/bash
file=compose_times.txt
file_path=performance/
rm -f $file

new_specs_folder=(1 2 3 4)

echo "Performance with 4 spectrums (proposed result) with docker compose:" > $file
pushd ..
{ time ./run.sh; } 2>> $file_path$file
popd
make clean -C .. > /dev/null

for sf in "${new_specs_folder[@]}"; do
    n_specs=$((sf * 4 + 4))
    echo -e "\nPerformance with $n_specs spectrums with docker compose:" >> $file
    cp ../perf_specs/$sf/* ../spectrums
    pushd ..
    { time ./run.sh; } 2>> $file_path$file
    popd
    make clean -C .. > /dev/null
done

sed -i '/Container/d' $file
sed -i '/Network/d' $file
sed -i '/sys/d' $file
sed -i '/user/d' $file
rm ../spectrums/meteorites*

grep "real" $file > ../charts/compose_times.txt