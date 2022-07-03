#!/bin/bash

# args: łatka

points=0

# Sprawdzenie obecności definicji stałej BUCKET_Q
if grep -q "BUCKET_Q" $1; then
  printf '4.1 OK\n'
  ((points++))
else
  printf '4.1 FAIL\n'
fi

if grep -q "NR_BUCKETS" $1; then
  printf '4.2 OK\n'
  ((points++))
else
  printf '4.2 FAIL\n'
fi

exit $points
