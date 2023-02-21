#!/bin/bash

# Check that a directory name is provided as an argument
if [ -z "$1" ]; then
  echo "Usage: $0 <directory>"
  exit 1
fi

# Set the output file name
output_file="combined.txt"

# Remove the output file if it already exists
if [ -f "$output_file" ]; then
  rm "$output_file"
fi

# Loop through all text files in the directory and append their contents to the output file
for file in "$1"/*.txt; do
  if [ -f "$file" ]; then
    cat "$file" >> "$output_file"
  fi
done

echo "Combined all text files in $1 into $output_file"
