# CMakeLists for src/modules/notifier

set(kvinotifier_SRCS
	libkvinotifier.cpp
	NotifierWindow.cpp
	NotifierMessage.cpp
	NotifierWindowTab.cpp
	NotifierWindowBorder.cpp
)

set(kvi_module_name kvinotifier)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

add_subdirectory(pics)
