# CMakeLists for src/modules/perlcore

# From qmake
#win32{
#	INCLUDEPATH += $(PERLDIR)/lib/CORE/
#	LIBS += -lperl58
#}
#
#mac {
#	INCLUDEPATH += $(PERLDIR)/CORE/
#	LIBS += -lperl -L$(PERLDIR)/CORE/
#}

SET(kviperlcore_MOC_HDRS
	perlcoreinterface.h
)

SET(kviperlcore_SRCS
	libkviperlcore.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviperlcore_MOC_SRCS ${kviperlcore_MOC_HDRS})

# Create library
ADD_LIBRARY(kviperlcore SHARED ${kviperlcore_SRCS} ${kviperlcore_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviperlcore)

# Install target
SET_TARGET_PROPERTIES(kviperlcore PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviperlcore LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)