#!/bin/bash

conf=confusion_matrix.txt
leg=legend.txt
folder=output/
static=static_times.txt
compose=compose_times.txt

rm -f $conf
rm -f $leg
rm -f $static
rm -f $compose
rm -f *.png

echo "Wait while charts are being created"

pushd .. > /dev/null
{ time ./run.sh; } > /dev/null 2>&1
until [ -f $folder$conf ]; do
    sleep 1
done
cp $folder$conf charts
cp $folder$leg charts
popd > /dev/null

pushd ../performance > /dev/null
./static_perf.sh > /dev/null 2>&1
./compose_perf.sh > /dev/null 2>&1
popd > /dev/null

python3 create_charts.py

rm -f $conf
rm -f $leg
rm -f $static
rm -f $compose