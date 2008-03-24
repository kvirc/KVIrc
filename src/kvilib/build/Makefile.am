########################################################################################################################
#
# KVILIB Makefile : KVIrc utility library
# Szymon Stefanek 21.09.2001
#
########################################################################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ $(SS_INCDIRS) $(SS_CPPFLAGS)

lib_LTLIBRARIES      = libkvilib.la
libkvilib_la_LDFLAGS = $(SS_LDFLAGS) -version-info 4:0:0 $(SS_LIBDIRS)
libkvilib_la_LIBADD  = $(SS_LIBLINK)

#
# Command useful to find out all the sources
# SRC=`find ../ -name *.cpp | sort`
# echo $SRC
#

nodist_libkvilib_la_SOURCES = \
	../ext/moc_kvi_crypt.cpp \
	../ext/moc_kvi_garbage.cpp \
	../ext/moc_kvi_regusersdb.cpp \
	../ext/moc_kvi_sharedfiles.cpp \
	../irc/moc_kvi_ircuserdb.cpp \
	../net/moc_kvi_dns.cpp \
	../net/moc_kvi_http.cpp \
	../system/moc_kvi_locale.cpp \
	../system/moc_kvi_thread.cpp \
	../tal/moc_kvi_tal_wizard.cpp

libkvilib_la_SOURCES = \
	../core/kvi_error.cpp \
	../core/kvi_heapobject.cpp \
	../core/kvi_malloc.cpp \
	../core/kvi_memmove.cpp \
	../core/kvi_qstring.cpp \
	../core/kvi_string.cpp \
	../core/kvi_stringarray.cpp \
	../ext/kvi_cmdformatter.cpp \
	../ext/kvi_config.cpp \
	../ext/kvi_crypt.cpp \
	../ext/kvi_databuffer.cpp \
	../ext/kvi_dcophelper.cpp \
	../ext/kvi_doublebuffer.cpp \
	../ext/kvi_garbage.cpp \
	../ext/kvi_osinfo.cpp \
	../ext/kvi_imagelib.cpp \
	../ext/kvi_md5.cpp \
	../ext/kvi_mediatype.cpp \
	../ext/kvi_miscutils.cpp \
	../ext/kvi_msgtype.cpp \
	../ext/kvi_parameterlist.cpp \
	../ext/kvi_pixmap.cpp \
	../ext/kvi_proxydb.cpp \
	../ext/kvi_regchan.cpp \
	../ext/kvi_regusersdb.cpp \
	../ext/kvi_sharedfiles.cpp \
	../ext/kvi_stringconversion.cpp \
	../file/kvi_file.cpp \
	../file/kvi_fileutils.cpp \
	../file/kvi_packagefile.cpp \
	../irc/kvi_avatar.cpp \
	../irc/kvi_avatarcache.cpp \
	../irc/kvi_ircmask.cpp \
	../irc/kvi_ircserver.cpp \
	../irc/kvi_ircserverdb.cpp \
	../irc/kvi_ircuserdb.cpp \
	../irc/kvi_mirccntrl.cpp \
	../irc/kvi_nickserv.cpp \
	../irc/kvi_useridentity.cpp \
	../net/kvi_dns.cpp \
	../net/kvi_http.cpp \
	../net/kvi_netutils.cpp \
	../net/kvi_socket.cpp \
	../net/kvi_ssl.cpp \
	../net/kvi_url.cpp \
	../system/kvi_env.cpp \
	../system/kvi_locale.cpp \
	../system/kvi_thread.cpp \
	../system/kvi_time.cpp \
	../tal/kvi_tal_application.cpp \
	../tal/kvi_tal_filedialog.cpp \
	../tal/kvi_tal_grid.cpp \
	../tal/kvi_tal_groupbox.cpp \
	../tal/kvi_tal_hbox.cpp \
	../tal/kvi_tal_iconview.cpp \
	../tal/kvi_tal_listbox.cpp \
	../tal/kvi_tal_listview.cpp \
	../tal/kvi_tal_mainwindow.cpp \
	../tal/kvi_tal_menubar.cpp \
	../tal/kvi_tal_popupmenu.cpp \
	../tal/kvi_tal_scrollview.cpp \
	../tal/kvi_tal_tabdialog.cpp \
	../tal/kvi_tal_textedit.cpp \
	../tal/kvi_tal_toolbar.cpp \
	../tal/kvi_tal_tooltip.cpp \
	../tal/kvi_tal_vbox.cpp \
	../tal/kvi_tal_widgetstack.cpp \
	../tal/kvi_tal_wizard.cpp

#
# Command useful to find out all the headers
# rm -f ../include/*.h
# SRC=`find ../ -name *.h | sort`
# echo $SRC
#


headers_HEADERS = \
	../config/kvi_confignames.h \
	../config/kvi_debug.h \
	../config/kvi_defaults.h \
	../config/kvi_fileextensions.h \
	../config/kvi_settings.h \
	../config/kvi_sourcesdate.h \
	../config/kvi_wincfg.h \
	../core/kvi_bswap.h \
	../core/kvi_error.h \
	../core/kvi_heapobject.h \
	../core/kvi_inttypes.h \
	../core/kvi_malloc.h \
	../core/kvi_memmove.h \
	../core/kvi_pointerlist.h \
	../core/kvi_pointerhashtable.h \
	../core/kvi_qcstring.h \
	../core/kvi_qstring.h \
	../core/kvi_strasm.h \
	../core/kvi_string.h \
	../core/kvi_stringarray.h \
	../core/kvi_valuelist.h \
	../ext/kvi_accel.h \
	../ext/kvi_cmdformatter.h \
	../ext/kvi_config.h \
	../ext/kvi_crypt.h \
	../ext/kvi_databuffer.h \
	../ext/kvi_doublebuffer.h \
	../ext/kvi_draganddrop.h \
	../ext/kvi_osinfo.h \
	../ext/kvi_garbage.h \
	../ext/kvi_imagelib.h \
	../ext/kvi_md5.h \
	../ext/kvi_mediatype.h \
	../ext/kvi_miscutils.h \
	../ext/kvi_msgtype.h \
	../ext/kvi_parameterlist.h \
	../ext/kvi_pixmap.h \
	../ext/kvi_proxydb.h \
	../ext/kvi_regchan.h \
	../ext/kvi_regusersdb.h \
	../ext/kvi_sharedfiles.h \
	../ext/kvi_stringconversion.h \
	../ext/kvi_xlib.h \
	../file/kvi_file.h \
	../file/kvi_fileutils.h \
	../file/kvi_packagefile.h \
	../irc/kvi_avatar.h \
	../irc/kvi_avatarcache.h \
	../irc/kvi_ircmask.h \
	../irc/kvi_ircserverdb.h \
	../irc/kvi_ircserver.h \
	../irc/kvi_ircuserdb.h \
	../irc/kvi_mirccntrl.h \
	../irc/kvi_nickserv.h \
	../irc/kvi_useridentity.h \
	../net/kvi_dns.h \
	../net/kvi_http.h \
	../net/kvi_netutils.h \
	../net/kvi_socket.h \
	../net/kvi_sockettype.h \
	../net/kvi_ssl.h \
	../net/kvi_url.h \
	../system/kvi_env.h \
	../system/kvi_library.h \
	../system/kvi_locale.h \
	../system/kvi_process.h \
	../system/kvi_stdarg.h \
	../system/kvi_thread.h \
	../system/kvi_time.h \
	../tal/kvi_tal_application.h \
	../tal/kvi_tal_application_kde.h \
	../tal/kvi_tal_application_qt.h \
	../tal/kvi_tal_filedialog.h \
	../tal/kvi_tal_filedialog_kde.h \
	../tal/kvi_tal_filedialog_qt.h \
	../tal/kvi_tal_filedialog_qt4.h \
	../tal/kvi_tal_grid.h \
	../tal/kvi_tal_grid_qt3.h \
	../tal/kvi_tal_grid_qt4.h \
	../tal/kvi_tal_groupbox.h \
	../tal/kvi_tal_groupbox_qt3.h \
	../tal/kvi_tal_groupbox_qt4.h \
	../tal/kvi_tal_hbox.h \
	../tal/kvi_tal_hbox_qt3.h \
	../tal/kvi_tal_hbox_qt4.h \
	../tal/kvi_tal_iconview.h \
	../tal/kvi_tal_iconview_qt3.h \
	../tal/kvi_tal_iconview_qt4.h \
	../tal/kvi_tal_listbox.h \
	../tal/kvi_tal_listbox_qt3.h \
	../tal/kvi_tal_listbox_qt4.h \
	../tal/kvi_tal_listview.h \
	../tal/kvi_tal_listview_qt3.h \
	../tal/kvi_tal_listview_qt4.h \
	../tal/kvi_tal_mainwindow.h \
	../tal/kvi_tal_mainwindow_kde.h \
	../tal/kvi_tal_mainwindow_qt3.h \
	../tal/kvi_tal_mainwindow_qt4.h \
	../tal/kvi_tal_menubar.h \
	../tal/kvi_tal_menubar_kde.h \
	../tal/kvi_tal_menubar_qt.h \
	../tal/kvi_tal_popupmenu.h \
	../tal/kvi_tal_popupmenu_qt3.h \
	../tal/kvi_tal_popupmenu_qt4.h \
	../tal/kvi_tal_scrollview.h \
	../tal/kvi_tal_scrollview_qt3.h \
	../tal/kvi_tal_scrollview_qt4.h \
	../tal/kvi_tal_tabdialog.h \
	../tal/kvi_tal_tabdialog_qt3.h \
	../tal/kvi_tal_tabdialog_qt4.h \
	../tal/kvi_tal_textedit.h \
	../tal/kvi_tal_textedit_qt3.h \
	../tal/kvi_tal_textedit_qt4.h \
	../tal/kvi_tal_toolbar.h \
	../tal/kvi_tal_toolbar_kde.h \
	../tal/kvi_tal_toolbar_qt3.h \
	../tal/kvi_tal_toolbar_qt4.h \
	../tal/kvi_tal_toolbardocktype.h \
	../tal/kvi_tal_tooltip.h \
	../tal/kvi_tal_vbox.h \
	../tal/kvi_tal_vbox_qt3.h \
	../tal/kvi_tal_vbox_qt4.h \
	../tal/kvi_tal_widgetstack.h \
	../tal/kvi_tal_widgetstack_qt3.h \
	../tal/kvi_tal_widgetstack_qt4.h \
	../tal/kvi_tal_wizard.h \
	../tal/kvi_tal_windowstate.h


../ext/moc_%.cpp: ../ext/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

../file/moc_%.cpp: ../file/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

../irc/moc_%.cpp: ../irc/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

../net/moc_%.cpp: ../net/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

../system/moc_%.cpp: ../system/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

../tal/moc_%.cpp: ../tal/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@


%.moc: %.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

#%.h.gch: %.h
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -c $< -o $@

#PRECOMPILED_HDRS = \
#	../config/kvi_confignames.h.gch \
#   ...

#
# Command useful to find the moc dependancies
# grep "\.moc" ../*/*.cpp | sort | sed -e 's/[[:space:]]*#include//' | sed -e 's/\"//g'
# needs to be modified by hand then to add the moc file path... :)
#

../tal/kvi_tal_application.cpp: ../tal/kvi_tal_application_kde.moc
../tal/kvi_tal_application.cpp: ../tal/kvi_tal_application_qt.moc
../tal/kvi_tal_mainwindow.cpp: ../tal/kvi_tal_mainwindow_kde.moc
../tal/kvi_tal_mainwindow.cpp: ../tal/kvi_tal_mainwindow_qt3.moc
../tal/kvi_tal_mainwindow.cpp: ../tal/kvi_tal_mainwindow_qt4.moc
../tal/kvi_tal_menubar.cpp: ../tal/kvi_tal_menubar_kde.moc
../tal/kvi_tal_menubar.cpp: ../tal/kvi_tal_menubar_qt.moc
../tal/kvi_tal_filedialog.cpp: ../tal/kvi_tal_filedialog_kde.moc
../tal/kvi_tal_filedialog.cpp: ../tal/kvi_tal_filedialog_qt.moc
../tal/kvi_tal_filedialog.cpp: ../tal/kvi_tal_filedialog_qt4.moc
../tal/kvi_tal_grid.cpp: ../tal/kvi_tal_grid_qt3.moc
../tal/kvi_tal_grid.cpp: ../tal/kvi_tal_grid_qt4.moc
../tal/kvi_tal_groupbox.cpp: ../tal/kvi_tal_groupbox_qt3.moc
../tal/kvi_tal_groupbox.cpp: ../tal/kvi_tal_groupbox_qt4.moc
../tal/kvi_tal_hbox.cpp: ../tal/kvi_tal_hbox_qt3.moc
../tal/kvi_tal_hbox.cpp: ../tal/kvi_tal_hbox_qt4.moc
../tal/kvi_tal_vbox.cpp: ../tal/kvi_tal_vbox_qt3.moc
../tal/kvi_tal_vbox.cpp: ../tal/kvi_tal_vbox_qt4.moc
../tal/kvi_tal_popupmenu.cpp: ../tal/kvi_tal_popupmenu_qt3.moc
../tal/kvi_tal_popupmenu.cpp: ../tal/kvi_tal_popupmenu_qt4.moc
../tal/kvi_tal_listview.cpp: ../tal/kvi_tal_listview_qt3.moc
../tal/kvi_tal_listview.cpp: ../tal/kvi_tal_listview_qt4.moc
../tal/kvi_tal_listbox.cpp: ../tal/kvi_tal_listbox_qt3.moc
../tal/kvi_tal_listbox.cpp: ../tal/kvi_tal_listbox_qt4.moc
../tal/kvi_tal_iconview.cpp: ../tal/kvi_tal_iconview_qt3.moc
../tal/kvi_tal_iconview.cpp: ../tal/kvi_tal_iconview_qt4.moc
../tal/kvi_tal_scrollview.cpp: ../tal/kvi_tal_scrollview_qt3.moc
../tal/kvi_tal_scrollview.cpp: ../tal/kvi_tal_scrollview_qt4.moc
../tal/kvi_tal_tabdialog.cpp: ../tal/kvi_tal_tabdialog_qt3.moc
../tal/kvi_tal_tabdialog.cpp: ../tal/kvi_tal_tabdialog_qt4.moc
../tal/kvi_tal_textedit.cpp: ../tal/kvi_tal_textedit_qt3.moc
../tal/kvi_tal_textedit.cpp: ../tal/kvi_tal_textedit_qt4.moc
../tal/kvi_tal_toolbar.cpp: ../tal/kvi_tal_toolbar_qt3.moc
../tal/kvi_tal_toolbar.cpp: ../tal/kvi_tal_toolbar_qt4.moc
../tal/kvi_tal_toolbar.cpp: ../tal/kvi_tal_toolbar_kde.moc
../tal/kvi_tal_tooltip.cpp: ../tal/kvi_tal_tooltip.moc
../tal/kvi_tal_widgetstack.cpp: ../tal/kvi_tal_widgetstack_qt3.moc
../tal/kvi_tal_widgetstack.cpp: ../tal/kvi_tal_widgetstack_qt4.moc

#
# All the cpp files depend on the precompiled headers being up-to-date
#

#../*/*.cpp: symlinks $(PRECOMPILED_HDRS) gchsymlinks

../*/*.cpp: symlinks

#
# Symlinks depend on all the header files: symlink is rebuilt whenewer an *.h file changes
#

symlinks: ../config/*.h ../core/*.h ../ext/*.h ../file/*.h ../irc/*.h ../net/*.h ../system/*.h ../tal/*.h
	cd ../include; for i in kvi_*.h; do if [ ! -f $$i ]; then rm -f $$i; fi; done; for i in $(SS_TOPSRCDIR)/src/kvilib/*/kvi_*.h; do $(LN_S) $$i . >/dev/null 2>&1; echo "Created link to $$i"; done;
	touch symlinks

# gchsymlinks: ../config/*.h ../core/*.h ../ext/*.h ../file/*.h ../irc/*.h ../net/*.h ../system/*.h ../tal/*.h
#	cd ../include; for i in kvi_*.gch; do if [ ! -f $$i ]; then rm -f $$i; fi; done; for i in $(SS_TOPSRCDIR)/src/kvilib/*/kvi_*.h.gch; do $(LN_S) $$i . >/dev/null 2>&1; echo "Created link to $$i"; done;
#	touch gchsymlinks

#
# When cleaning...remove also the symlinks, gchsymlinks and gch headers
#

#CLEANFILES = ../include/*.h ../include/*.gch symlinks gchsymlinks ../*/*.moc ../*/moc_kvi_*.cpp ../*/*.gch
CLEANFILES = ../include/*.h symlinks ../*/*.moc ../*/moc_kvi_*.cpp
