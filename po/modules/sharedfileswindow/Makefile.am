###############################################################################
# KVirc IRC client Makefile - 18.09.99 Szymon Stefanek <pragma at kvirc dot net>
###############################################################################

# The name of this module
THISMODULE = sharedfileswindow

# Don't touch this
tmpdir = $(localedir)

# The *.mo files to generate
tmp_DATA  = sharedfileswindow_ca.mo \
	sharedfileswindow_cs.mo \
	sharedfileswindow_it.mo \
	sharedfileswindow_de.mo \
	sharedfileswindow_es.mo \
	sharedfileswindow_pt.mo \
	sharedfileswindow_ru.mo \
	sharedfileswindow_pt_BR.mo \
	sharedfileswindow_fr.mo \
	sharedfileswindow_fi.mo \
	sharedfileswindow_hr.mo \
	sharedfileswindow_hu.mo \
	sharedfileswindow_ua.mo

# The *.po files to generate
POFILES  = sharedfileswindow_ca.po \
	sharedfileswindow_cs.po \
	sharedfileswindow_it.po \
	sharedfileswindow_de.po \
	sharedfileswindow_es.po \
	sharedfileswindow_pt.po \
	sharedfileswindow_ru.po \
	sharedfileswindow_pt_BR.po \
	sharedfileswindow_fr.po \
	sharedfileswindow_fi.po \
	sharedfileswindow_hr.po \
	sharedfileswindow_hu.po \
	sharedfileswindow_ua.po


###############################################################################
# STOP EDITING HERE
###############################################################################

# Killed the fuzzy entries: don't use them , they may SEGV
#	- touch $@ && $(SS_MSGFMT) -fvo $@ $<

CLEANFILES = $(tmp_DATA)
EXTRA_DIST = $(POFILES)

%.mo: %.po
	-touch $@ && $(SS_MSGFMT) -vo $@ $<

messages-extract:
	@find $(topdir)/src/modules/$(THISMODULE)/ -maxdepth 10 -name *.cpp > files; \
	find $(topdir)/src/modules/$(THISMODULE)/ -maxdepth 10 -name *.h >> files; \
	xgettext -o $(topdir)/po/modules/$(THISMODULE)/$(THISMODULE).pot -k__tr_ctx -k__tr_no_lookup_ctx -k__tr2qs_ctx -k__tr2wc_ctx -k__tr2ws_ctx -f files; \
	rm files; \
	echo "Messages extracted to $(THISMODULE).pot";

messages-update:
	@old=_old; \
	if test -f "$(topdir)/po/modules/$(THISMODULE)/$(THISMODULE).pot"; then \
		for lang  in $(POFILES); do \
			echo "Updating file $$lang"; \
			mv -f $$lang $$lang$$old; \
			msgmerge -o $$lang $$lang$$old $(topdir)/po/modules/$(THISMODULE)/$(THISMODULE).pot && rm -f $$lang$$old; \
		done; \
	else \
		echo "----------------------------------------------------"; \
		echo "- WARNING: ./$(THISMODULE).pot does not exist"; \
		echo "- WARNING: You must run make messages-extract first"; \
		echo "----------------------------------------------------"; \
	fi

messages: messages-extract messages-update
