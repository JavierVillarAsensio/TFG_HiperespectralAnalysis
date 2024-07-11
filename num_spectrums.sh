#!/bin/bash
FOLDER="/spectrums"
NUM_FILES=$(find "$FOLDER" -type f | wc -l)
export NUM_FILES