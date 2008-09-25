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
	popupeditor proxydb
	raweditor regchan reguser
	serverdb setup sharedfile sharedfileswindow
	snd socketspy spaste str system
	texticons term
#	theme  <- to port
	tip tmphighlight toolbar toolbareditor
	trayicon
	upnp
	url
	window

	# broken modules
	# torrent

	# modules to deprecate ?
	# perl, perlcore

	# forgotten ?
	# offer
)

# this is currently broken on MINGW because of some virtual functions
# messing up linker's brain

IF(NOT MINGW)
	ADD_SUBDIRECTORY(rijndael)
ENDIF()
