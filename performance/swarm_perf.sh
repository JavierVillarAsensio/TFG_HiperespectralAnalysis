#!/bin/bash
file=swarm_times.txt
file_path=performance/
rm -f $file

save=save.yaml
swarm_yaml=docker-swarm.yaml

cp ../$swarm_yaml ../$save

new_specs_folder=(1 2 3 4)
old_n_specs=4

echo "Performance with 4 spectrums (proposed result) with docker swarm:" > $file
pushd ..
{ time ./run_swarm.sh; } 2>> $file_path$file
popd
make clean -C .. > /dev/null

for sf in "${new_specs_folder[@]}"; do
    n_specs=$((sf * 4 + 4))
    echo -e "\nPerformance with $n_specs spectrums with docker swarm:" >> $file
    cp ../perf_specs/$sf/* ../spectrums
    pushd ..
    sed -i "s/$old_n_specs/$n_specs/g" $swarm_yaml
    { time ./run_swarm.sh; } 2>> $file_path$file
    popd
    old_n_specs=$n_specs
    make clean -C .. > /dev/null
done

sed -i '/Container/d' $file
sed -i '/Network/d' $file
sed -i '/sys/d' $file
sed -i '/user/d' $file
rm ../spectrums/meteorites*
rm ../$swarm_yaml
mv ../$save ../$swarm_yaml

grep "real" $file > ../charts/swarm_times.txt