SPEC_IMAGE_NAME=tfg-replica
MASTER_IMAGE_NAME=tfg-master
IMAGE_TAG=latest

#creating images if necessary
if docker images | grep -q "$SPEC_IMAGE_NAME\s*$IMAGE_TAG"; then
  echo "La imagen $SPEC_IMAGE_NAME:$IMAGE_TAG ya existe. No se necesita construir."
else
  echo "La imagen $SPEC_IMAGE_NAME:$IMAGE_TAG no existe. Construyendo la imagen..."
  docker build -f Dockerfile_spectrum . -t tfg-spectrum:latest
fi

if docker images | grep -q "$MASTER_IMAGE_NAME\s*$IMAGE_TAG"; then
  echo "La imagen $MASTER_IMAGE_NAME:$IMAGE_TAG ya existe. No se necesita construir."
else
  echo "La imagen $MASTER_IMAGE_NAME:$IMAGE_TAG no existe. Construyendo la imagen..."
  docker build -f Dockerfile_master .. -t tfg-master:latest
fi