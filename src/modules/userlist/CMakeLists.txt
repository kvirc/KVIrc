# CMakeLists for src/modules/userlist

SET(kviuserlist_SRCS
	libkviuserlist.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviuserlist_MOC_SRCS ${kviuserlist_MOC_HDRS})

SET(kvi_module_name kviuserlist)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
