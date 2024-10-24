#!/bin/bash

#count spectrums for docker replicas
FOLDER="spectrums"
NUM_FILES=$(find "$FOLDER" -type f | wc -l)

#clean previous runs
docker compose -f docker-compose-replicas.yaml down 

#start replicas indicating its file index
for ((i = 1; i <= $NUM_FILES; i++)); do
  echo -e "NUM_FILES=$NUM_FILES\nSERVICE_NAME=spectrum-$i" > specs.env
  docker compose -f docker-compose-replicas.yaml up -d --scale replica=$i --no-recreate --quiet-pull
done

cp output/confusion_matrix.txt charts
cp output/legend.txt charts