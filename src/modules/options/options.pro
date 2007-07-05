TARGET = kvioptions

PRECOMPILED_HEADER  = pch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
     DEFINES += USING_PCH
     }
     
precompile_header:isEmpty(PRECOMPILED_HEADER) {
     DEFINES += CREATING_PCH
     }
 
HEADERS += container.h \
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
 						
SOURCES += libkvioptions.cpp \
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
 						
include(../module.pri)