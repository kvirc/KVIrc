# CMakeLists for src/modules/texticons

SET(kvitexticons_SRCS
	libkvitexticons.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitexticons_MOC_SRCS ${kvitexticons_MOC_HDRS})

# Create library
ADD_LIBRARY(kvitexticons SHARED ${kvitexticons_SRCS} ${kvitexticons_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvitexticons)

# Install target
SET_TARGET_PROPERTIES(kvitexticons PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvitexticons LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
