# CMakeLists for src/modules/notifier

SET(kvinotifier_SRCS
	libkvinotifier.cpp
	NotifierWindow.cpp
	NotifierMessage.cpp
	NotifierWindowTab.cpp
	NotifierWindowBorder.cpp
)

SET(kvi_module_name kvinotifier)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

ADD_SUBDIRECTORY(pics)
