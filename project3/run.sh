#!/bin/bash

# sudo fuser -v tests

# Define experiment parameters
RWMIXREADS="0 70 100"
ACCESS_SIZES="4096 32768 131072"
QUEUE_DEPTHS="16"
TEST_DURATION="20s"
SSD_DEVICE="/dev/nvme0n1"
PARTITION_START="2048"
TEST_PARTITION="/dev/nvme0n1p2"
TEST_DIRECTORY="./tests"
RESULTS_DIRECTORY="./results"
MLC_EXECUTABLE="./mlc"
MLC_OUTPUT_FILE="mlc_output.txt"

# Create an empty partition for FIO testing
sudo fdisk $SSD_DEVICE <<EOF
n
p
1
$PARTITION_START

w
EOF

sudo mkfs.ext4 $TEST_PARTITION
sudo mkdir $TEST_DIRECTORY
sudo mount $TEST_PARTITION $TEST_DIRECTORY
sudo chmod 777 $TEST_DIRECTORY

# Run FIO tests
for rwmixread in $RWMIXREADS; do
    for size in $ACCESS_SIZES; do
        for depth in $QUEUE_DEPTHS; do
            fio --name=randwrite --ioengine=libaio --rw=randwrite --bs=$size --numjobs=1 --size=1g --runtime=$TEST_DURATION --time_based --iodepth=$depth --rwmixread=$rwmixread --directory=$TEST_DIRECTORY --output=$RESULTS_DIRECTORY/fio_results_$rwmixread-$size-$depth.txt
        done
    done
done

MLC_ACCESS_SIZES="64 256"
READ_WRITE_RATIO="9 7 8 6"
# Run MLC tests
for option in $READ_WRITE_RATIO; do
    for access_size in $MLC_ACCESS_SIZES; do
        sudo $MLC_EXECUTABLE --loaded_latency -W$option -l$access_size > $RESULTS_DIRECTORY/mlc_test_$access_size-W-$option.txt
    done
done
# Analyze results and write report
# You can use Python or R scripts to analyze the results and generate graphs and tables for your report.

# Cleanup
sudo umount $TEST_PARTITION
sudo rm -r $TEST_DIRECTORY
sudo fdisk $SSD_DEVICE <<EOF
d
w
EOF
