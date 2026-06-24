#!/usr/bin/env bash
set -euo pipefail

for i in 10 20 30 40 50 100 200 300 400 500 1000 2500 5000; do 
  for j in {0..99}; do 
    ./pipeline.sh 1 "$i"
  done
  echo "$i"
done
