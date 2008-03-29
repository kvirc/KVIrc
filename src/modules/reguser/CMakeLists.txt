# CMakeLists for src/modules/reguser

SET(kvieditor_MOC_HDRS
	edituser.h
	wizard.h
	dialog.h
)

SET(kvireguser_SRCS
	libkvireguser.cpp
	edituser.cpp
	wizard.cpp
	dialog.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvireguser_MOC_SRCS ${kvireguser_MOC_HDRS})

# Create library
ADD_LIBRARY(kvireguser SHARED ${kvireguser_SRCS} ${kvireguser_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvireguser)

# Install target
SET_TARGET_PROPERTIES(kvireguser PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvireguser LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)