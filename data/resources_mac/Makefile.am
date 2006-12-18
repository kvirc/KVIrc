EXTRA_DIST = Info.plist.in kvirc.icns

CLEANFILES = Info.plist

all: Info.plist

Info.plist: Makefile
	sed -e 's,[@]kvircversion[@],$(VERSION),g' < Info.plist.in > Info.plist
