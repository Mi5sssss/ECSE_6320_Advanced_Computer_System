#!/bin/bash

# Compile the C++ code
g++ -march=native -o main main.cc -pthread -mavx2

# Set the number of threads for the experiments
thread_counts="1 2 4 8"

# search_prefix_pairs="pmacocj pma wyfidlnqc wy gbjgsn gb"
search_prefix_pairs="pmacocj pma"

data_path="/home/xie/ECSE_6320_dataset/project4/Column.txt"

# Run the experiment for each thread count and search prefix pair
for thread_count in $thread_counts; do
  set -- $search_prefix_pairs
  while [ "$#" -gt 0 ]; do
    value="$1"
    prefix="$2"
    echo "Running experiment with $thread_count threads and search prefix pair: $value $prefix"
    export NUM_THREADS=$thread_count
    ./main $value $prefix $data_path
    echo -e "\n"
    shift 2
  done
done
