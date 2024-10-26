#!/bin/bash

result=output/result.jpg
stack_name=tfg-hiperespectral-analysis
image=hiperespectral-replica-swarm:latest
image_dockerfile=Docker_images/Dockerfile_replicas_swarm

#count spectrums for docker replicas
FOLDER="spectrums"
NUM_FILES=$(find "$FOLDER" -type f | wc -l)
echo -e "NUM_FILES=$NUM_FILES" > specs.env

if docker image inspect ${image} > /dev/null 2>&1; then
    echo "Image already built."
else
    echo "Building image: ${image}..."
    docker build -t ${image} -f $image_dockerfile .
fi

docker swarm init > /dev/null 2>&1

docker stack deploy -c docker-swarm.yaml $stack_name

until [ -f $result ]; do
    sleep 1
done

docker stack rm $stack_name