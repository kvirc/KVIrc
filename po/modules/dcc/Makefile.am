###############################################################################
# KVirc IRC client Makefile - 18.09.99 Szymon Stefanek <pragma at kvirc dot net>
###############################################################################

# The name of this module
THISMODULE = dcc

# Don't touch this
tmpdir = $(localedir)

# The *.mo files to generate
tmp_DATA  = dcc_ca.mo \
	dcc_it.mo \
	dcc_de.mo \
	dcc_es.mo \
	dcc_cs.mo \
	dcc_pt.mo \
	dcc_ru.mo \
	dcc_pt_BR.mo \
	dcc_hu.mo \
	dcc_fr.mo \
	dcc_fi.mo \
	dcc_uk.mo

# The *.po files to generate
POFILES  = dcc_ca.po \
	dcc_it.po \
	dcc_de.po \
	dcc_es.po \
	dcc_cs.po \
	dcc_pt.po \
	dcc_ru.po \
	dcc_pt_BR.po \
	dcc_hu.po \
	dcc_fr.po \
	dcc_fi.po \
	dcc_uk.po

# The *.po  -> *.mo rules
dcc_it.mo: dcc_it.po
dcc_de.mo: dcc_de.po
dcc_es.mo: dcc_es.po
dcc_cs.mo: dcc_cs.po
dcc_pt_BR.mo: dcc_pt_BR.po
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
