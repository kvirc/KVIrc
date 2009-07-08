#!/bin/bash
for aa in $*; do
file $aa
dos2unix $aa 
done
