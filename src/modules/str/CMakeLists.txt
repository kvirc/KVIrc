# CMakeLists for src/modules/str

# From qmake
#win32{
#	LIBS += -leay32
#	-lssleay32
#}
#
#mac {
#	LIBS += -lssl -lcrypto
#}

SET(kvistr_SRCS
	libkvistr.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvistr_MOC_SRCS ${kvistr_MOC_HDRS})

# Create library
ADD_LIBRARY(kvistr SHARED ${kvistr_SRCS} ${kvistr_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvistr)

# Install target
SET_TARGET_PROPERTIES(kvistr PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvistr LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)