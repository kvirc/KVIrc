###############################################################################
# KVirc IRC client Makefile - 18.09.99 Szymon Stefanek <pragma at kvirc dot net>
###############################################################################

# The name of this module
THISMODULE = perl

# Don't touch this
tmpdir = $(localedir)

# The *.mo files to generate
tmp_DATA  = perl_ca.mo \
	perl_cs.mo \
	perl_it.mo \
	perl_de.mo \
	perl_pt.mo \
	perl_es.mo \
	perl_ru.mo \
	perl_pt_BR.mo \
	perl_fr.mo \
	perl_hu.mo \
	perl_fi.mo \
	perl_ua.mo

# The *.po files to generate
POFILES  =  perl_ca.po \
	perl_cs.po \
	perl_it.po \
	perl_de.po \
	perl_pt.po \
	perl_es.po \
	perl_ru.po \
	perl_pt_BR.po \
	perl_fr.po \
	perl_hu.po \
	perl_fi.po \
	perl_ua.po


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
