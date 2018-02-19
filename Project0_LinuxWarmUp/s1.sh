#!/bin/bash
read file
mkdir $file
touch name.txt stno.txt
echo 'Wang Yijun' > name.txt
echo '10152130137' > stno.txt
cp name.txt $file"/name.txt"
cp stno.txt $file"/stno.txt"

