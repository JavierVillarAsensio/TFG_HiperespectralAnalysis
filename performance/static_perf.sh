#!/bin/bash
file=static_times.txt
rm -f $file

new_specs_folder=(1 2 3 4)

echo "Performance with 4 spectrums (proposed result) in static version:" > $file
{ time make performance -C ..; } 2>> $file

for sf in "${new_specs_folder[@]}"; do
    n_specs=$((sf * 4 + 4))
    echo -e "\nPerformance with $n_specs spectrums in static version:" >> $file
    cp ../perf_specs/$sf/* ../spectrums
    { time make performance -C ..; } 2>> $file
    make clean -C ..
done

sed -i '/sys/d' $file
sed -i '/user/d' $file
rm ../spectrums/meteorites*

grep "real" $file > ../charts/static_times.txt