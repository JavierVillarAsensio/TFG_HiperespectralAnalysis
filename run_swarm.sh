#!/bin/bash

result=output/result.jpg
stack_name=tfg-hiperespectral-analysis
image_replica=hiperespectral-replica-swarm:latest
image_replica_dockerfile=Docker_images/Dockerfile_replicas_swarm

if docker image inspect ${image_replica} > /dev/null 2>&1; then
    echo "Image already built."
else
    echo "Building image: ${image_replica}..."
    docker build -t ${image_replica} -f $image_dockerfile .
fi

docker swarm init > /dev/null 2>&1

docker stack deploy -c docker-swarm.yaml $stack_name

until [ -f $result ]; do
    sleep 1
done