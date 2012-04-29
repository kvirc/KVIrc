# CMakeLists.txt for src/modules/
IF(APPLE)
	SET(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")
ENDIF()

INCLUDE_DIRECTORIES(
	../kvilib/tal/
	../kvilib/config/
	../kvilib/core/
	../kvilib/ext/
	../kvilib/file/
	../kvilib/irc/
	../kvilib/locale/
	../kvilib/net/
	../kvilib/system/
	../kvirc/kernel/
	../kvirc/kvs/
	../kvirc/kvs/event/
	../kvirc/kvs/object/
	../kvirc/kvs/parser/
	../kvirc/kvs/tree/
	../kvirc/module/
	../kvirc/sparser/
	../kvirc/ui/
)

SUBDIRS(
	about action actioneditor addon aliaseditor avatar
	chan channelsjoin classeditor codetester config context
	dcc dialog
	editor eventeditor
	file filetransferwindow fish
	help http
	ident iograph
	lamerizer language links list log logview
	mask math mediaplayer mircimport my
	notifier
	objects options
	perlcore popup popupeditor proxydb pythoncore
	raweditor regchan reguser rijndael rot13
	serverdb setup sharedfile sharedfileswindow snd socketspy spaste str system
	texticons term theme tip tmphighlight toolbar toolbareditor torrent trayicon
	upnp url userlist
	window
)

IF(COMPILE_PERL_SUPPORT)
	SUBDIRS(perl)
ENDIF()

IF(COMPILE_PYTHON_SUPPORT)
	SUBDIRS(python)
ENDIF()

IF(APPLE)
	# This is a dirty trick ensure we get some code executed after the last module gets installed
	SUBDIRS(zzz_afterlastmodule)
ENDIF()
