# CMakeLists.txt for src/modules/zzz_afterlastmodule
if(APPLE)
	set(QT_LIBRARY_DIR "${Qt5Widgets_DIR}/../../")
	get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)
	get_filename_component(QT_LIBRARY_DIR "${QT_LIBRARY_DIR}/.." ABSOLUTE)

	install(CODE "
	    file(GLOB_RECURSE KVIRC_INSTALLED_MODULES
	      \"${KVIRC_MOD_PATH}/*.so\")
	    include(BundleUtilities)
	    fixup_bundle(\"${CMAKE_INSTALL_PREFIX}\" \"\${KVIRC_INSTALLED_MODULES}\" \"${QT_LIBRARY_DIR}\")
	    " COMPONENT Runtime)
endif()
