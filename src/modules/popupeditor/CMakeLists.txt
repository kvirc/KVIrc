# CMakeLists for src/modules/popupeditor

SET(kvipopupeditor_MOC_HDRS
	popupeditor.h
)

SET(kvipopupeditor_SRCS
	libkvipopupeditor.cpp
	popupeditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvipopupeditor_MOC_SRCS ${kvipopupeditor_MOC_HDRS})

# Create library
ADD_LIBRARY(kvipopupeditor SHARED ${kvipopupeditor_SRCS} ${kvipopupeditor_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvipopupeditor)

# Install target
SET_TARGET_PROPERTIES(kvipopupeditor PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvipopupeditor LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)