#!/usr/bin/bash

script_dir=$(dirname "$0")

echo -e "\nCompiling...\n"
gcc $script_dir/../unit-tests/deadlock.c -w -o $script_dir/../unit-tests/bin/deadlock
gcc $script_dir/../unit-tests/resource.c -w -o $script_dir/../unit-tests/bin/resource

echo -e "Running Unit Tests...\n"

$script_dir/../unit-tests/bin/deadlock
echo -e "\n"
$script_dir/../unit-tests/bin/resource
