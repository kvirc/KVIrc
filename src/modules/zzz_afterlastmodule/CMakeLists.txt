# CMakeLists.txt for src/modules/zzz_afterlastmodule
IF(APPLE)
	INSTALL(CODE "
	    file(GLOB_RECURSE KVIRC_INSTALLED_MODULES
	      \"${KVIRC_MOD_PATH}/*.so\")
	    include(BundleUtilities)
	    fixup_bundle(\"${CMAKE_INSTALL_PREFIX}\" \"\${KVIRC_INSTALLED_MODULES}\" \"\")
	    " COMPONENT Runtime)
ENDIF()
