#!/bin/bash
read file
cat $file | awk '{ if(NR == 10) { print $0 } }'
