IF(WITH_KDE4)
# for KDE 4.2 we require at least version 0.23
set(SHARED_MIME_INFO_MINIMUM_VERSION "0.23")
find_package(SharedMimeInfo REQUIRED)

########### install files ###############
install(FILES kvirc.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
ENDIF()
