#!/bin/bash

echo "read-safe"
time ./read-until 1 
echo "read-unsafe"
time ./read-until 2 
echo "coroutine"
time ./read-until 3 
echo "cobalt 1"
time ./read-until-cobalt 1
echo "cobalt 2"
time ./read-until-cobalt 2
