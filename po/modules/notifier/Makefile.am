################################################################################
# KVirc IRC client Makefile - 18.09.99 Szymon Stefanek <pragma at kvirc dot net>
################################################################################

# The name of this module
THISMODULE = notifier

# Don't touch this
tmpdir = $(localedir)

# The *.mo files to generate
tmp_DATA  = notifier_ca.mo \
	notifier_cs.mo \
	notifier_it.mo \
	notifier_de.mo \
	notifier_pt.mo \
	notifier_es.mo \
	notifier_ru.mo \
	notifier_pt_BR.mo \
	notifier_fr.mo \
	notifier_hu.mo \
	notifier_fi.mo

# The *.po files to generate
POFILES  =  notifier_ca.po \
	notifier_cs.po \
	notifier_it.po \
	notifier_de.po \
	notifier_pt.po \
	notifier_es.po \
	notifier_ru.po \
	notifier_pt_BR.po \
	notifier_fr.po \
	notifier_hu.po \
	notifier_fi.po


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
