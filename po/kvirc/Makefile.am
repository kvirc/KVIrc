###############################################################################
# KVirc IRC client Makefile - 18.09.99 Szymon Stefanek <stefanek@tin.it>
###############################################################################

tmpdir = $(localedir)

tmp_DATA  = kvirc_bg.mo \
	kvirc_ca.mo \
	kvirc_cs.mo \
	kvirc_de.mo \
	kvirc_es.mo \
	kvirc_fr.mo \
	kvirc_hu.mo \
	kvirc_it.mo \
	kvirc_nl.mo \
	kvirc_pl.mo \
	kvirc_pt.mo \
	kvirc_pt_BR.mo \
	kvirc_ru.mo \
	kvirc_sr.mo \
	kvirc_hu.mo \
	kvirc_hr.mo

CLEANFILES = $(tmp_DATA)

POFILES = kvirc_bg.po \
	kvirc_ca.po \
	kvirc_cs.po \
	kvirc_de.po \
	kvirc_es.po \
	kvirc_fr.po \
	kvirc_hu.po \
	kvirc_it.po \
	kvirc_nl.po \
	kvirc_pl.po \
	kvirc_pt.po \
	kvirc_pt_BR.po \
	kvirc_ru.po \
	kvirc_sr.po \
	kvirc_hu.po \
	kvirc_hr.po

EXTRA_DIST=$(POFILES)

# Killed the fuzzy entries: don't use them , they may SEGV
#	- touch $@ && $(SS_MSGFMT) -fvo $@ $<

%.mo: %.po
	-touch $@ && $(SS_MSGFMT) -vo $@ $<

###############################################################################
# Messages
###############################################################################

messages-extract:
	@find $(topdir)/src/ -maxdepth 10 -name *.cpp > files
	find $(topdir)/src/ -maxdepth 10 -name *.h >> files
	find $(topdir)/data/defscript/ -maxdepth 10 -name *.kvs >> files
	xgettext -o $(topdir)/po/kvirc/kvirc.pot -k__tr -k__tr_no_lookup -k__tr2qs -k__tr2wc -k__tr2ws -ktr -f files
	rm files; \
	echo "Messages extracted to kvirc.pot";


messages-update:
	@old=_old; \
	if test -f "$(topdir)/po/kvirc/kvirc.pot"; then \
		for lang  in $(POFILES); do \
			echo "Updating file $$lang"; \
			mv -f $$lang $$lang$$old; \
			msgmerge -o $$lang $$lang$$old $(topdir)/po/kvirc/kvirc.pot && rm -f $$lang$$old; \
		done; \
	else \
		echo "----------------------------------------------------"; \
		echo "- WARNING: ./kvirc.pot does not exist"; \
		echo "- WARNING: You must run make messages-extract first"; \
		echo "----------------------------------------------------"; \
	fi

messages: messages-extract messages-update
