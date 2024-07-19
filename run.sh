#!/bin/bash

REPLICA_IMAGE_NAME=tfg-replica
MASTER_IMAGE_NAME=tfg-master
IMAGE_TAG=latest

#creating images if necessary
if docker images | grep -q "$REPLICA_IMAGE_NAME\s*$IMAGE_TAG"; then
  echo "La imagen $REPLICA_IMAGE_NAME:$IMAGE_TAG ya existe. No se necesita construir."
else
  echo "La imagen $REPLICA_IMAGE_NAME:$IMAGE_TAG no existe. Construyendo la imagen..."
  docker build -f Dockerfile_replicas . --no-cache=true -t tfg-replica:latest 
fi

if docker images | grep -q "$MASTER_IMAGE_NAME\s*$IMAGE_TAG"; then
  echo "La imagen $MASTER_IMAGE_NAME:$IMAGE_TAG ya existe. No se necesita construir."
else
  echo "La imagen $MASTER_IMAGE_NAME:$IMAGE_TAG no existe. Construyendo la imagen..."
  docker build -f Dockerfile_master . --no-cache=true -t tfg-master:latest
fi

#count spectrums for docker replicas
FOLDER="spectrums"
NUM_FILES=$(find "$FOLDER" -type f | wc -l)

#start replicas indicating its file index
for ((i = 1; i <= $NUM_FILES; i++)); do
  echo -e "NUM_FILES=$NUM_FILES\nSERVICE_NAME=spectrum-$i" > specs.env
  docker-compose -f docker-compose-replicas.yaml up -d --scale replica=$i --no-recreate
done