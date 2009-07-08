#!/bin/bash

HERE=$(pwd)

for size in 16 32 48 64 128; do
	echo "Creating icons with size" $size "x" $size
	for file in *.svgz; do
		PNGFNAME=$(echo $file | sed -e 's/svgz/png/g')
		echo "Creating icon" $PNGFNAME
		set -x
		# Ksvgtopng exports broken pngs..
		#ksvgtopng $size $size $HERE/$file $HERE/../$size"x"$size/$PNGFNAME
		inkscape \
			-z \
			--export-png=$HERE/../$size"x"$size/$PNGFNAME \
			--export-area-drawing \
			--export-width=$size \
			--export-height=$size \
			$HERE/$file
		set +x
	done
done

echo "Done :)"
