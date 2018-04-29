#!/bin/bash
file=(
    "DSJC125.1.col.txt"
    "DSJC125.5.col.txt"
    "DSJC125.9.col.txt"
    "DSJC250.1.col.txt"
    "DSJC250.5.col.txt"
    "DSJC250.9.col.txt"
    "DSJC500.1.col.txt"
    "DSJC500.5.col.txt"
)
res=(
    "5"
    "17"
    "44"
    "8"
    "28"
    "72"
    "12"
    "49"
)
rm data.txt;
for i in {0..7}
do
    for t in {0..10}
    do
        ./coloring ${file[i]} ${res[i]} >> data.txt;
    done
done