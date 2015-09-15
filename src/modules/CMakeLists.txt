# CMakeLists.txt for src/modules/
if(APPLE)
	set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")
endif()

include_directories(
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

subdirs(
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
	package perlcore popup popupeditor proxydb pythoncore
	raweditor regchan reguser rijndael rot13
	serverdb setup sharedfile sharedfileswindow snd socketspy spaste str system
	texticons term theme tip tmphighlight toolbar toolbareditor torrent trayicon
	upnp url userlist
	window
)

if(COMPILE_PERL_SUPPORT)
	subdirs(perl)
endif()

if(COMPILE_PYTHON_SUPPORT)
	subdirs(python)
endif()

if(COMPILE_ENCHANT_SUPPORT)
	subdirs(spellchecker)
endif()

if(APPLE)
	# This is a dirty trick ensure we get some code executed after the last module gets installed
	subdirs(zzz_afterlastmodule)
endif()
