###############################################################################
# KVirc IRC client Makefile - 18.09.99 Szymon Stefanek <pragma at kvirc dot net>
###############################################################################

# The name of this module
THISMODULE = about

# Don't touch this
tmpdir = $(localedir)

# The *.mo files to generate
tmp_DATA  = about_ca.mo \
	about_cs.mo \
	about_de.mo \
	about_es.mo \
	about_it.mo \
	about_pt.mo \
	about_ru.mo \
	about_pt_BR.mo \
	about_fr.mo \
	about_hu.mo \
	about_fi.mo \
	about_hr.mo \
	about_uk.mo

# The *.po files to generate
POFILES  = about_ca.po \
	about_cs.po \
	about_de.po \
	about_es.po \
	about_it.po \
	about_pt.po \
	about_ru.po \
	about_pt_BR.po \
	about_fr.po \
	about_hu.po \
	about_fi.po \
	about_hr.po \
	about_uk.po


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
