# CMakeLists for po/modules/

# sed will cut paths at this directory
set(TRANSLATION_BASEDIR "src")

# Find subdirs
subdirs(
	about
	addon
	dcc
	editor
	filetransferwindow
	log
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
