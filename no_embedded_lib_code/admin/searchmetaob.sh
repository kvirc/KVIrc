#!/bin/bash

DIRS=`find ../src/modules/ -type d -maxdepth 1`

HERE=`pwd`

echo "Looking for unregistered meta objects"

for adir in $DIRS; do

	cd $HERE
	cd $adir

	THEFILES=`ls | grep "\.h"`

	if test -n "$THEFILES"; then

		echo "Checking dir $adir"

		OBJECTS=`grep -h -B 4 "Q_OBJECT" *.h | grep "class" | grep ":" | sed -e 's/class[ ]*//g' | sed -e 's/[ ]*:[A-Za-z0-9 ,]*//g'`

		for aob in $OBJECTS; do
			META=`grep "unregisterMetaObject(\"$aob\")" *.cpp`
			if test -z "$META"; then
				echo "Class $aob is NOT UNREGISTERED in $adir"
			fi
		done

	fi

done
