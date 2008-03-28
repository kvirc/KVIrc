###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

EXTRA_DIST = mkcreateinstanceproc.sh

pluglib_LTLIBRARIES      = libkvioptions.la

libkvioptions_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkvioptions_la_SOURCES = libkvioptions.cpp \
				container.cpp \
				dialog.cpp \
				instances.cpp \
				optw_antispam.cpp \
				optw_avatar.cpp \
				optw_connection.cpp \
				optw_ctcp.cpp \
				optw_ctcpfloodprotection.cpp \
				optw_dcc.cpp \
				optw_generalopt.cpp \
				optw_highlighting.cpp \
				optw_ident.cpp \
				optw_identity.cpp \
				optw_ignore.cpp \
				optw_input.cpp \
				optw_interfacefeatures.cpp \
				optw_interfacelookglobal.cpp \
				optw_irc.cpp \
				optw_ircoutput.cpp \
				optw_ircview.cpp \
				optw_lag.cpp \
				optw_logging.cpp \
				optw_mediatypes.cpp \
				optw_messages.cpp \
				optw_nickserv.cpp \
				optw_notify.cpp \
				optw_protection.cpp \
				optw_proxy.cpp \
				optw_query.cpp \
				optw_servers.cpp \
				optw_sound.cpp \
				optw_ssl.cpp \
				optw_taskbar.cpp \
				optw_textencoding.cpp \
				optw_texticons.cpp \
				optw_theme.cpp \
				optw_tools.cpp \
				optw_transport.cpp \
				optw_topiclabel.cpp \
				optw_away.cpp \
				optw_uparser.cpp \
				optw_urlhandlers.cpp \
				optw_userlist.cpp \
				optw_tray.cpp

libkvioptions_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

noinst_HEADERS= container.h \
				dialog.h \
				instances.h \
				optw_antispam.h \
				optw_avatar.h \
				optw_connection.h \
				optw_ctcp.h \
				optw_ctcpfloodprotection.h \
				optw_dcc.h \
				optw_generalopt.h \
				optw_highlighting.h \
				optw_ident.h \
				optw_identity.h \
				optw_ignore.h \
				optw_input.h \
				optw_interfacefeatures.h \
				optw_interfacelookglobal.h \
				optw_irc.h \
				optw_ircoutput.h \
				optw_ircview.h \
				optw_lag.h \
				optw_logging.h \
				optw_mediatypes.h \
				optw_messages.h \
				optw_nickserv.h \
				optw_notify.h \
				optw_protection.h \
				optw_proxy.h \
				optw_query.h \
				optw_servers.h \
				optw_sound.h \
				optw_ssl.h \
				optw_taskbar.h \
				optw_textencoding.h \
				optw_texticons.h \
				optw_theme.h \
				optw_tools.h \
				optw_transport.h \
				optw_topiclabel.h \
				optw_away.h \
				optw_uparser.h \
				optw_urlhandlers.h \
				optw_userlist.h \
				optw_tray.h

instances.h: optw_*.h mkcreateinstanceproc.sh
	./mkcreateinstanceproc.sh

m_%.moc: %.h
	$(SS_QT_MOC) $< -o $@

container.cpp: m_container.moc
dialog.cpp: m_dialog.moc
instances.cpp: m_instances.moc
optw_antispam.cpp: m_optw_antispam.moc
optw_avatar.cpp: m_optw_avatar.moc
optw_away.cpp: m_optw_away.moc
optw_connection.cpp: m_optw_connection.moc
optw_ctcp.cpp: m_optw_ctcp.moc
optw_ctcpfloodprotection.cpp: m_optw_ctcpfloodprotection.moc
optw_dcc.cpp: m_optw_dcc.moc
optw_generalopt.cpp: m_optw_generalopt.moc
optw_highlighting.cpp: m_optw_highlighting.moc
optw_ident.cpp: m_optw_ident.moc
optw_identity.cpp: m_optw_identity.moc
optw_ignore.cpp: m_optw_ignore.moc
optw_input.cpp: m_optw_input.moc
optw_interfacefeatures.cpp: m_optw_interfacefeatures.moc
optw_interfacelookglobal.cpp: m_optw_interfacelookglobal.moc
optw_irc.cpp: m_optw_irc.moc
optw_ircoutput.cpp: m_optw_ircoutput.moc
optw_ircview.cpp: m_optw_ircview.moc
optw_lag.cpp: m_optw_lag.moc
optw_logging.cpp: m_optw_logging.moc
optw_look.cpp: m_optw_look.moc
optw_mediatypes.cpp: m_optw_mediatypes.moc
optw_messages.cpp: m_optw_messages.moc
optw_nickserv.cpp: m_optw_nickserv.moc
optw_notify.cpp: m_optw_notify.moc
optw_protection.cpp: m_optw_protection.moc
optw_proxy.cpp: m_optw_proxy.moc
optw_query.cpp: m_optw_query.moc
optw_servers.cpp: m_optw_servers.moc
optw_sound.cpp: m_optw_sound.moc
optw_ssl.cpp: m_optw_ssl.moc
optw_taskbar.cpp: m_optw_taskbar.moc
optw_textencoding.cpp: m_optw_textencoding.moc
optw_texticons.cpp: m_optw_texticons.moc
optw_transport.cpp: m_optw_transport.moc
optw_theme.cpp: m_optw_theme.moc
optw_topiclabel.cpp: m_optw_topiclabel.moc
optw_tools.cpp: m_optw_tools.moc
optw_uparser.cpp: m_optw_uparser.moc
optw_urlhandlers.cpp: m_optw_urlhandlers.moc
optw_userlist.cpp: m_optw_userlist.moc
optw_tray.cpp : m_optw_tray.moc
