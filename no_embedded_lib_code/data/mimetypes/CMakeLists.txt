IF(KDE4_FOUND)
    include(MacroEnsureVersion)

    macro_ensure_version("4.2.0" ${KDE_VERSION} KDE_4_2_FOUND)

    # for KDE 4.2 we require at least version 0.23
    IF(KDE_4_2_FOUND)
        set(SHARED_MIME_INFO_MINIMUM_VERSION "0.23")
    ENDIF(KDE_4_2_FOUND)

    # now to shared mime info
    find_package(SharedMimeInfo)

    IF(SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})
        install(FILES kvirc.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
        update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
    ENDIF(SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})
ENDIF(KDE4_FOUND)
