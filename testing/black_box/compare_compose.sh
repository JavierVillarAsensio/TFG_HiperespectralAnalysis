#!/bin/bash
#!!!this script is intended to be executed here!!!
result_file=output/result.jpg

rm compared_results.png

pushd ../../
make clean
make
mv $result_file testing/black_box/result_static.jpg
make clean

./run.sh
while true; do
    if [[ -f "$result_file" ]]; then
        break  
    else
        sleep 1  
    fi
done
mv $result_file testing/black_box/result_compose.jpg
popd

compare result_static.jpg result_compose.jpg compared_results.png
rm *.jpg

xdg-open compared_results.png #if the pixels in the image have a WHITE filter, it means the comparison mathces
                              #if the pixels in the image have a RED filter, it means the comparison does not match