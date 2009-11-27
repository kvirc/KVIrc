#!/bin/sh

#============================================================================
#
#   File : svn_fixsvnignore.sh
#   Creation date : Sat 30 Dec 2006 16:17:52 by Szymon Stefanek
#
#   This file is part of the KVIrc IRC Client distribution
#   Copyright (C) 2000-2009 Szymon Stefanek <pragma at kvirc dot net>
#
#   This program is FREE software. You can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your opinion) any later version.
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

export BASEDIR=$(pwd)

for a in $(find ./ -name ".svnignore"); do
	DADIR=$(echo $a | sed -e 's/[a-z\.-]*[^/]$//g')
	echo $BASEDIR/$DADIR
	cd $BASEDIR/$DADIR
	svn propset svn:ignore -F .svnignore ./
done


