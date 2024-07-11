#!/bin/bash

#count spectrums for docker replicas
FOLDER="spectrums"
NUM_FILES=$(find "$FOLDER" -type f | wc -l)
echo NUM_FILES=$NUM_FILES > .env

docker-compose -f docker-compose-replicas.yaml up --scale spectrum=$NUM_FILES