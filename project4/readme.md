# Implementation of Dictionary Codec

## Introduction
Dictionary encoding is being widely used in real-world data analytics systems to compress data with relatively low cardinality and speed up search/scan operations. In essence, a dictionary encoder scans the to-be-compressed data to build the dictionary that consists of all the unique data items and replaces each data item with its dictionary ID. To accelerate dictionary look-up, one may use certain indexing data structure such as hash-table or B-tree to better manage the dictionary. In addition to reducing the data footprint, dictionary encoding makes it possible to apply SIMD instructions to significantly speed up the search/scan operations.

The implementation support the following operations:
1. Encoding: given a file consisting of raw column data, carry out dictionary encoding and generate an encoded column file consisting of both dictionary and encoded data column. Your code must support multi-threaded implementation of dictionary encoding.

2. Query: enable users to query an existing encoded column file. Your implementation should allow users to (i) check whether one data item exists in the column, if it exists, return the indices of all the matching entries in the column; (ii) given a prefix, search and return all the unique matching data and their indices. Your implementation must support the use of SIMD instructions to speed up the search/scan.

## Prerequisites
* C++ compiler with C++11 support
* AVX2 SIMD support

## Code Structure
The code is structured into the following components:

### Dictionary encoder
* `encode_thread`: This function serves as the core operation executed by each thread during the encoding process. Accepting a reference to the input data, the start and end indices for the specific data chunk to process, and a reference to the dictionary, it iterates through the data chunk. Consequently, the dictionary is updated with unique data items and their corresponding dictionary IDs.

* `encode`: The primary encoding function accepts a reference to the input data and returns a vector of integers representing the encoded data. Internally, the function divides the data into portions, spawns threads for parallel processing of these chunks, and synchronizes the completion of all threads before generating the encoded data column.

### Query functions
* `search`: This function conducts a single data item search on the unencoded data column. By taking a reference to the data and a search value, it yields a vector of indices where the specified value is located.

* `search_simd`: Enabled by AVX2 support, this SIMD-based single data item search function operates on the unencoded data column. It accepts a reference to the data and a search value, returning a vector of indices where the value is found. SIMD instructions facilitate accelerated search performance.

* `prefix_search`: Targeting the unencoded data column, this function performs a prefix search. It processes a reference to the data and a prefix, subsequently returning a vector of pairs containing the unique matching data items and their corresponding indices.

## Running Experiments
### Usage

#### 1. Make script executable:
```
chmod +x run.sh
```

#### 2. Modify the `data_path` in script

#### 3. Customization

To test multiple search values and prefixes, modify the `search_prefix_pairs` in `run.sh`.

To test different number of threads, modify the `thread_counts` in `run.sh`.

## Results and Analysis

CPU: `Intel(R) Xeon(R) CPU E5-2630 v4 @ 2.20GHz`

Memory: `128 GB`

| Num Threads | Search Prefix Pair | Encoding Time \(s\) | Search Time \(s\) | Search SIMD Time \(s\) | Prefix Search Time \(s\)  |
|:-----------:|:------------------:|:-------------------:|:-----------------:|:----------------------:|:-------------------------:|
| 1           | pmacocj pma        | 124\.918            | 1\.93217          | 1\.36706               | 4\.70761                  |
| 2           | pmacocj pma        | 252\.329            | 2\.39163          | 1\.35307               | 4\.77929                  |
| 4           | pmacocj pma        | 239\.359            | 2\.26282          | 1\.35704               | 4\.69017                  |
| 8           | pmacocj pma        | 242\.672            | 2\.02202          | 1\.34968               | 4\.78296                  |

### 1 Encoding time
The single-threaded implementation (1 thread) had the best performance with the lowest encoding time of 124.918 seconds. The encoding time increases as we move from 1 to 2 threads (252.329 seconds), and it remains relatively stable as we increase the number of threads further (239.359 seconds for 4 threads and 242.672 seconds for 8 threads).

This counterintuitive behavior could be attributed to the locking mechanism used in the encode_thread function. The use of a global mutex may cause contention among the threads, resulting in reduced performance for higher thread counts.

### 2 Search time

The SIMD-based search consistently outperforms the regular search. SIMD search times range from 1.34968 to 1.36706 seconds, while regular search times range from 1.93217 to 2.39163 seconds. This indicates that SIMD instructions effectively speed up the search operation, as intended.

### 3 Prefix search time

The prefix search time remains relatively stable, ranging from 4.69017 to 4.78296 seconds.

### 4 To make a summary

The single-threaded implementation has the fastest encoding time, while the performance for higher thread counts is suboptimal due to contention introduced by the global mutex. An improved approach could involve using concurrent data structures or local dictionaries that are later merged to optimize multi-threaded encoding.

The SIMD-based search consistently outperforms the regular search, demonstrating the effectiveness of SIMD instructions in speeding up search operations.

The prefix search time is generally higher than the single data item search times, as expected.