#!/bin/bash

#find file start with letter "b" in /bin
find /bin -name "b*" > tmp.txt

#exclude '/bin'
cat tmp.txt | awk -F '/' '{if(NF>2) {print $0;}}' > tmp2.txt

#ls -l these file
cat tmp2.txt | xargs ls -l > tmp.txt

#create output.txt
cat /dev/null > output.txt

#get name, owner and permission of these file
cat tmp.txt | awk '{ split($9,str,"/"); print str[3], $3,$1; }' > output.txt

#sort by filename
sort output.txt -o output.txt

rm tmp.txt tmp2.txt

#change premission of output.txt
chmod 644 output.txt
