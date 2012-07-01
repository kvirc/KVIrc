#!/bin/sh

find . -name *.po -print0 | while read -d $'\0' file; do
  echo Checking "$file"
  echo y | ./msgcheckformat "$file" fix
done