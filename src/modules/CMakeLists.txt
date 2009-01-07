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
	../kvilib/net/
	../kvilib/system/
	../kvirc/kernel/
	../kvirc/kvs/
	../kvirc/module/
	../kvirc/sparser/
	../kvirc/ui/
)

SUBDIRS(
	about action actioneditor addon aliaseditor avatar
	chan channelsjoin codetester config context
	dcc dialog
	editor eventeditor
	file filetransferwindow
	help http
	ident iograph
	lamerizer language links list log logview
	mask math mediaplayer mircimport my
	notifier
	objects options
	perl perlcore popupeditor proxydb
	raweditor regchan reguser rijndael
	serverdb setup sharedfile sharedfileswindow snd socketspy spaste str system
	texticons term theme tip tmphighlight toolbar toolbareditor torrent trayicon
	upnp
	url
	window

	# forgotten ?
	# offer
)
