#!/bin/bash

if test -z "$*"; then
	echo "No files to parse"
	exit
fi


SEARCH="contextStackSize"
REPLACE="actualStackSize"

echo "Searching for $SEARCH"
echo "Replacing with $REPLACE"

for afile in $*; do
	DATA=`grep "$SEARCH" $afile`

	if test -n "$DATA"; then

		echo "Processing $afile ..."

		rm -f $afile.backup
		rm -f $afile.backup2

		cp $afile $afile.backup

		COMMAND="s/$SEARCH/$REPLACE/g"
		echo "`cat $afile.backup | sed -e $COMMAND`" > $afile

		if grep "$SEARCH" $afile; then
			echo "Ops.. maybe it didn't work..."
		else
			echo "Ok, it worked."
		fi
	fi

done
