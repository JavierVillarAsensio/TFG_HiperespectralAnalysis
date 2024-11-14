#!/bin/bash

result=output/result.jpg
stack_name=tfg-hiperespectral-analysis
image_replica=hiperespectral-replica-swarm:latest
image_master=hiperespectral-master:latest
image_replica_dockerfile=Docker_images/Dockerfile_replicas_swarm
image_master_dockerfile=Docker_images/Dockerfile_master
specs_directory=spectrums

if docker image inspect ${image_replica} > /dev/null 2>&1; then
    echo "Image already built."
else
    echo "Building image: ${image_replica}..."
    docker build -t ${image_replica} -f $image_replica_dockerfile .
fi

if docker image inspect ${image_master} > /dev/null 2>&1; then
    echo "Image already built."
else
    echo "Building image: ${image_master}..."
    docker build -t ${image_master} -f $image_master_dockerfile .
fi

FILES=$(ls -1q "$specs_directory" | wc -l)
export NUM_FILES=$FILES

docker swarm init > /dev/null 2>&1

docker stack deploy -c docker-swarm.yaml $stack_name

until [ -f $result ]; do
    sleep 1
done