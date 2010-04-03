# CMakeLists for po/modules/

# sed will cut paths at this directory
SET(TRANSLATION_BASEDIR "src")

# Find subdirs
SUBDIRS(
	about
	addon
	dcc
	editor
	filetransferwindow
	logview
	mediaplayer
	notifier
	objects
	options
	perl
	python
	register
	serverdb
	sharedfileswindow
	theme
	torrent
)
