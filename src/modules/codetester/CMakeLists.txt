# CMakeLists for src/modules/codetester

SET(kvicodetester_MOC_HDRS
	codetester.h
)

SET(kvicodetester_SRCS
	libkvicodetester.cpp
	codetester.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvicodetester_MOC_SRCS ${kvicodetester_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvicodetester MODULE ${kvicodetester_SRCS} ${kvicodetester_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvicodetester SHARED ${kvicodetester_SRCS} ${kvicodetester_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvicodetester ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvicodetester PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kvicodetester LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
