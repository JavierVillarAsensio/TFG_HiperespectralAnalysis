#!/bin/bash
file=swarm_times.txt
file_path=performance/
result=output/result.jpg
stack_name=tfg-hiperespectral-analysis
rm -f $file

swarm_yaml=docker-swarm.yaml

new_specs_folder=(1 2 3 4)

echo "Performance with 4 spectrums (proposed result) with docker swarm:" > $file
pushd ..
{ time ./run_swarm.sh > /dev/null 2>&1; } 2>> $file_path$file
docker stack rm $stack_name
popd
make clean -C .. > /dev/null

for sf in "${new_specs_folder[@]}"; do
    sleep 5     #let the previous stack be fully removed

    n_specs=$((sf * 4 + 4))
    echo -e "\nPerformance with $n_specs spectrums with docker swarm:" >> $file
    cp ../perf_specs/$sf/* ../spectrums

    pushd ..
    { time ./run_swarm.sh > /dev/null 2>&1; } 2>> $file_path$file
    docker stack rm $stack_name
    popd

    make clean -C .. > /dev/null
done

sed -i '/Container/d' $file
sed -i '/Network/d' $file
sed -i '/sys/d' $file
sed -i '/user/d' $file
rm ../spectrums/meteorites*

grep "real" $file > ../charts/swarm_times.txt