#!/usr/bin/env bash

## USAGE
# ./run.sh 2022 7
nodemon --exec "gcc -Wall ./$1/$2/main.c -o ./$1/$2/main.out && ./$1/$2/main.out" --watch ./$1/$2/main.c
