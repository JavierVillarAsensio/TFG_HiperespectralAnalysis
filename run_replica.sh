#!/bin/bash
HOSTNAME=$SERVICE_NAME

HOSTNAME_INDEX=${HOSTNAME##*-}

echo $HOSTNAME_INDEX > index.txt