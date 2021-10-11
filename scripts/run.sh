#!/usr/bin/bash

script_dir=$(dirname "$0")

echo -e "\nCompiling..."

gcc -c $script_dir/../src/deadlock.c -o $script_dir/../obj/deadlock.o -pthread
gcc -c $script_dir/../src/main.c -o $script_dir/../obj/main.o -pthread
gcc -c $script_dir/../src/resource.c -o $script_dir/../obj/resource.o -pthread
gcc -c $script_dir/../src/worker.c -o $script_dir/../obj/worker.o -pthread

echo -e "\nLinking Object Files\n"

gcc -o $script_dir/../bin/run $script_dir/../obj/deadlock.o $script_dir/../obj/main.o $script_dir/../obj/resource.o $script_dir/../obj/worker.o -pthread

$run $script_dir/../bin/run A 1 B 2 C 3 10 5