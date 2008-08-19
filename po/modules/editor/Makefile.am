################################################################################
# KVirc IRC client Makefile - 18.09.99 Szymon Stefanek <pragma at kvirc dot net>
################################################################################

# The name of this module
THISMODULE = editor

# Don't touch this
tmpdir = $(localedir)

# The *.mo files to generate
tmp_DATA  = editor_ca.mo \
	editor_cs.mo \
	editor_it.mo \
	editor_de.mo \
	editor_es.mo \
	editor_pt.mo \
	editor_ru.mo \
	editor_pt_BR.mo \
	editor_fr.mo \
	editor_hu.mo \
	editor_fi.mo \
	editor_uk.mo

# The *.po files to generate
POFILES  = editor_ca.po \
	editor_cs.po \
	editor_it.po \
	editor_de.po \
	editor_es.po \
	editor_pt.po \
	editor_ru.po \
	editor_pt_BR.po \
	editor_fr.po \
	editor_hu.po \
	editor_fi.po \
	editor_uk.po

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
