###############################################################################
# KVirc IRC client Makefile - 18.09.99 Szymon Stefanek <pragma at kvirc dot net>
###############################################################################

# The name of this module
THISMODULE = filetransferwindow

# Don't touch this
tmpdir = $(localedir)

# The *.mo files to generate
tmp_DATA  = filetransferwindow_ca.mo \
	filetransferwindow_cs.mo \
	filetransferwindow_it.mo \
	filetransferwindow_de.mo \
	filetransferwindow_pt.mo \
	filetransferwindow_es.mo \
	filetransferwindow_ru.mo \
	filetransferwindow_pt_BR.mo \
	filetransferwindow_fr.mo \
	filetransferwindow_hu.mo \
	filetransferwindow_fi.mo \
	filetransferwindow_hr.mo \
	filetransferwindow_uk.mo

# The *.po files to generate
POFILES  = filetransferwindow_ca.po \
	filetransferwindow_cs.po \
	filetransferwindow_it.po \
	filetransferwindow_de.po \
	filetransferwindow_pt.po \
	filetransferwindow_es.po \
	filetransferwindow_ru.po \
	filetransferwindow_pt_BR.po \
	filetransferwindow_fr.po \
	filetransferwindow_hu.po \
	filetransferwindow_fi.po \
	filetransferwindow_hr.po \
	filetransferwindow_uk.po

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
