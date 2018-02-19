#!/bin/bash
read file
cat $file | awk 'BEGIN {row=0; col=0;} {for(i=1;i<=NF;i++)str[i,row]=$i;row+=1;col=NF;} END {for(i=1;i<=col;++i) { for(j=0;j<row;++j) {printf("%s ", str[i,j]);}printf("\n");}}'

