#!/bin/bash

for dim in 128 256 512 1024 2048 4096 8192 16384; do
  for isFloat in 0 1; do
    ./gen $dim $isFloat
  done
done
