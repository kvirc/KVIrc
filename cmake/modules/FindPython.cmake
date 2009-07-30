# Copyright (c) 2003-2007 FlashCode <flashcode@flashtux.org>
# Copyright (c) 2009 Fabio Bas <ctrlaltca@libero.it>
# Copyright (c) 2009 Kai Wasserbäch <debian@carbon-project>
# Copyright (c) 2009 Elvio Basello <hell@hellvis69.netsons.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# - Find Python
# This module finds if Python is installed and determines where the include files
# and libraries are. It also determines what the name of the library is. This
# code sets the following variables:
#
#  PYTHON_INCLUDE_PATH = path to where python.h can be found
#  PYTHON_LIBRARY = path to where libpython.so* can be found
#  PYTHON_LFLAGS = python compiler options for linking

IF(PYTHON_FOUND)
	# Already in cache, be silent
	SET(PYTHON_FIND_QUIETLY TRUE)
ENDIF()

FIND_PACKAGE(PythonInterp)
IF(PYTHONINTERP_FOUND)
	FIND_PACKAGE(PythonLibs)

	EXECUTE_PROCESS(
		COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import *; print get_config_var('LINKFORSHARED')"
		OUTPUT_VARIABLE PYTHON_LFLAGS
	)
  
	# remove the new lines from the output by replacing them with empty strings
	STRING(REPLACE "\n" "" PYTHON_LFLAGS "${PYTHON_LFLAGS}")

	# Gets the version of the interpreter
	EXECUTE_PROCESS(
		COMMAND ${PYTHON_EXECUTABLE} -c "import platform; print platform.python_version()"
		OUTPUT_VARIABLE PYTHON_VERSION
	)

	# remove the new lines from the output by replacing them with empty strings
	STRING(REPLACE "\n" "" PYTHON_VERSION "${PYTHON_VERSION}")

	ADD_DEFINITIONS(-DPYTHON_VERSION=${PYTHON_VERSION})
	MESSAGE(STATUS "Found Python ${PYTHON_VERSION}")

	IF(PYTHONLIBS_FOUND)  
		SET(PYTHON_LIBRARY ${PYTHON_LIBRARIES})
	ENDIF()

	IF(PYTHON_LIBRARY AND PYTHON_INCLUDE_PATH)
		SET(PYTHON_FOUND TRUE)
	ENDIF()
  
	MARK_AS_ADVANCED(
		PYTHON_INCLUDE_PATH
		PYTHON_LIBRARY
		PYTHON_LFLAGS
	)
ENDIF() 
