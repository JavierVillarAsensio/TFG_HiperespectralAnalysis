#!/bin/bash

files=($(ls spectrums))

for file in "${files[@]}"; do
    echo "$file"
done