#!/bin/bash

for dimension in 128 256 512 1024 2048 4096 8192 16384
do
    for isFloat in 0 1
    do
        filenameA="matrixA_${dimension}_${isFloat}.txt"
        filenameB="matrixB_${dimension}_${isFloat}.txt"
        

        # Generate the matrices with the current dimension and isFloat
        ./gen $dimension $isFloat

        # Multiply the matrices and measure the time
        echo "Start the Experiment of Dimension: ${dimension}, isFloat: ${isFloat}"
        ./main ${filenameA} ${filenameB}

        rm ${filenameA}
        rm ${filenameB}
    done
done
