# #!/bin/bash

# # header for the table
# printf "%-30s %-10s %-15s %-20s %-20s %-20s %-20s\n" "Test" "Name" "IOPS" "BW (MiB/s)" "Avg Latency (usec)" "Min Latency (usec)" "Max Latency (usec)" > results.csv

# # set the directory where the text files are located
# dir="/home/xie/ECSE_6320_Advanced_Computer_System/project3/results"

# # loop through all the txt files in the specified directory
# for file in $dir/*.txt; do
#   # extract the relevant values using grep and awk
#   iops=$(grep -oP "(?<=IOPS=)[0-9]+" $file)
#   bw=$(grep -oP "(?<=BW=)[0-9]+(\.[0-9]+)?(?=MiB/s)" $file)
#   avg_latency=$(grep -oP "(?<=avg=)[0-9]+\.[0-9]+" $file)
#   min_latency=$(grep -oP "(?<=min=)[0-9]+" $file)
#   max_latency=$(grep -oP "(?<=max=)[0-9]+" $file)

#   # extract the test name from the file name
#   test_name=$(echo $file | cut -d '/' -f 4 | cut -d '_' -f 1)

#   # output the values as a row in the table
#   printf "%-30s %-10s %-15s %-20s %-20s %-20s %-20s\n" "$test_name" "$((iops/1000))k" "$bw" "$avg_latency" "$min_latency" "$max_latency" >> results.csv
# done

#!/bin/bash

# create a new workbook and worksheet
workbook="results.xlsx"
worksheet="Results"
dir="/home/xie/ECSE_6320_Advanced_Computer_System/project3/results"

if [ -f "$workbook" ]; then
    rm "$workbook"
fi

python - <<EOF
import xlsxwriter
import subprocess

# check if xlsxwriter is installed, and install it if necessary
try:
    import xlsxwriter
except ImportError:
    print("xlsxwriter is not installed. Installing...")
    subprocess.check_call(["pip", "install", "xlsxwriter"])
    import xlsxwriter

# create a new workbook and worksheet
workbook = xlsxwriter.Workbook("$workbook")
worksheet = workbook.add_worksheet("$worksheet")

# define cell formats
header_format = workbook.add_format({"bold": True, "font_color": "white", "bg_color": "black"})
data_format = workbook.add_format({"font_color": "black"})

# write the headers to the worksheet
headers = ["Test", "Name", "IOPS", "BW (MiB/s)", "Avg Latency (usec)", "Min Latency (usec)", "Max Latency (usec)"]
for i, header in enumerate(headers):
    worksheet.write(0, i, header, header_format)

# loop through all the txt files in the current directory
row = 1
for file in $dir/*.txt; do
  # extract the relevant values using grep and awk
  iops=$(grep -oP "(?<=IOPS=)[0-9]+" $file)
  bw=$(grep -oP "(?<=BW=)[0-9]+(\.[0-9]+)?(?=MiB/s)" $file)
  avg_latency=$(grep -oP "(?<=avg=)[0-9]+\.[0-9]+" $file)
  min_latency=$(grep -oP "(?<=min=)[0-9]+" $file)
  max_latency=$(grep -oP "(?<=max=)[0-9]+" $file)

  # extract the test name from the file name
  test_name=$(echo $file | cut -d '_' -f 1)

  # output the values as a row in the table
  row_data = [test_name, "$((iops/1000))k", bw, avg_latency, min_latency, max_latency]
  for i, data in enumerate(row_data):
      worksheet.write(row, i, data, data_format)
  row=$((row+1))
done

workbook.close()
EOF
