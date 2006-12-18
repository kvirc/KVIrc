################################################################################
# KVirc IRC client Makefile - 18.09.99 Szymon Stefanek <pragma at kvirc dot net>
################################################################################

# The name of this module
THISMODULE = options

# Don't touch this
tmpdir = $(localedir)


# The *.mo files to generate
tmp_DATA  = options_bg.mo \
	options_ca.mo \
	options_cs.mo \
	options_de.mo \
	options_es.mo \
	options_fr.mo \
	options_it.mo \
	options_nl.mo \
	options_pl.mo \
	options_pt.mo \
	options_pt_BR.mo \
	options_ru.mo \
	options_sr.mo

# The *.po files to generate
POFILES = options_bg.po \
	options_ca.po \
	options_cs.po \
	options_de.po \
	options_es.po \
	options_fr.po \
	options_it.po \
	options_nl.po \
	options_pl.po \
	options_pt.po \
	options_pt_BR.po \
	options_ru.po \
	options_sr.po

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
	xgettext -o $(topdir)/po/modules/$(THISMODULE)/$(THISMODULE).pot -k__tr2qs_no_lookup -k__tr2qs_ctx -k__tr_no_lookup -f files; \
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
