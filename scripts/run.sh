#!/usr/bin/bash

script_dir=$(dirname "$0")

echo -e "\nCompiling..."

gcc -w -c $script_dir/../src/deadlock.c -o $script_dir/../obj/deadlock.o -pthread
gcc -w -c $script_dir/../src/main.c -o $script_dir/../obj/main.o -pthread
gcc -w -c $script_dir/../src/resource.c -o $script_dir/../obj/resource.o -pthread
gcc -w -c $script_dir/../src/worker.c -o $script_dir/../obj/worker.o -pthread
gcc -w -c $script_dir/../src/log.c -o $script_dir/../obj/log.o -pthread

echo -e "Linking...\n"

gcc -o $script_dir/../bin/run $script_dir/../obj/deadlock.o $script_dir/../obj/main.o $script_dir/../obj/resource.o $script_dir/../obj/worker.o $script_dir/../obj/log.o -pthread

$run $script_dir/../bin/run A 1 B 2 C 3 2 1000000
