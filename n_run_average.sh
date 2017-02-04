#!/usr/bin/env zsh
#Runs the program $1 times with the $2 file, measuring the average runtime
runs=$1
file=$2
total_time=0
for ((i = 1; i <= $runs; i++)); do
        runtime=$(./tra-lopputyo $file | grep "Total" | grep -Eo '[0-9]\.[0-9]{1,6}')
        echo "[Run $i] Time: $runtime seconds"
        total_time=$((total_time + runtime))
        sleep 1
done
echo "Average runtime: $(($total_time*1.0 / $runs)) seconds"
