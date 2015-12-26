#!/bin/sh

#============================================================================
#
#   File : sedfiles.sh
#   Creation date : Sun 17 Dec 2006 20:36:07 by Szymon Stefanek
#
#   This file is part of the KVIrc IRC Client distribution
#   Copyright (C) 2000-2009 Szymon Stefanek <pragma at kvirc dot net>
#
#   This program is FREE software. You can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your option) any later version.
#
#   This program is distributed in the HOPE that it will be USEFUL,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, write to the Free Software Foundation,
#   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
#============================================================================

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
			echo "Oops! Maybe it didn't work..."
		else
			echo "OK, it worked."
		fi
	fi

done
