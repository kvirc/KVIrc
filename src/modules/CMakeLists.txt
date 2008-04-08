# CMakeLists.txt for src/modules/
IF(APPLE)
	SET(CMAKE_MODULE_LINKER_FLAGS "-single_module -undefined dynamic_lookup -multiply_defined suppress")
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
	chan channelsjoin clock codetester config context
	dcc dialog
	editor eventeditor
	file filetransferwindow
	help http
	ident iograph
	lamerizer links list log logview
	mask math mediaplayer mircimport my
	notifier
	objects options
	popupeditor
	raweditor regchan reguser rijndael
	setup sharedfile sharedfileswindow snd
	socketspy spaste str system
	texticons term theme tip tmphighlight toolbar toolbareditor trayicon
	url
	window

	# broken modules
	# torrent

	# modules to deprecate ?
	# perl, perlcore

	# forgotten ?
	# language offer
)
