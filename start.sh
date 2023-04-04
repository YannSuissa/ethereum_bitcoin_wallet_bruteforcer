#!/bin/bash

killall compute

nb=$1
if [[ -z $1 ]]; then
  nb=1
fi

for (( c=1; c<=$nb; c++ ))
do
   ./compute -c 9 -E &
done
