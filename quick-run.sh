#!/usr/bin/env bash
# quick run script for GCC that handles build, execution, and cleanup.
# use the first argument as the binary to run

gcc $1.c -o $1
./$1
rm $1