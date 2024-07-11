#!/bin/bash
echo $(hostname)
HOSTNAME=$(hostname)
if [[ $HOSTNAME =~ _([0-9]+)$ ]]; then
  HOSTNAME_INDEX=${BASH_REMATCH[1]}
else
  echo "No se pudo extraer el índice del hostname."
  exit 1
fi
export HOSTNAME_INDEX