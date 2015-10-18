# CMakeLists.txt for src/modules/zzz_afterlastmodule
if(APPLE)
	install(CODE "
	    file(GLOB_RECURSE KVIRC_INSTALLED_MODULES
	      \"${KVIRC_MOD_PATH}/*.so\")
	    include(BundleUtilities)
	    fixup_bundle(\"${CMAKE_INSTALL_PREFIX}\" \"\${KVIRC_INSTALLED_MODULES}\" \"${QT_LIBRARY_DIR}\")
	    " COMPONENT Runtime)
endif()
