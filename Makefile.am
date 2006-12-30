###############################################################################
# KVirc IRC client Makefile - 16.12.98 Szymon Stefanek 
###############################################################################

EXTRA_DIST    = autogen.sh README FAQ

SUBDIRS          = admin data doc po scripts src

AUTOMAKE_OPTIONS = foreign 

###############################################################################
# Delete moc files all around
###############################################################################

delmoc:
	rm -f $(topdir)/src/*/*/*.moc
	rm -f $(topdir)/src/modules/*/*.moc

###############################################################################
# Main make command
###############################################################################

kvirc-intro:
	@echo "################################################################################"
	@echo "### Ok..."
	@echo "### The compilation process will take a while..."
	@echo "### Have a cold beer in the meantime..."
	@echo "################################################################################"
	
kvirc-outro:
	@echo "################################################################################"
	@echo "### Finished..."
	@echo "### Now type 'make install'"
	@echo "################################################################################"

kvirc: kvirc-intro all kvirc-outro

install: install-recursive docs banner

install-devel: install-recursive banner

messages:
	cd $(topdir)/po/kvirc && make messages
	cd $(topdir)/po/modules/about && make messages
	cd $(topdir)/po/modules/dcc && make messages
	cd $(topdir)/po/modules/editor && make messages
	cd $(topdir)/po/modules/logview && make messages
	cd $(topdir)/po/modules/mediaplayer && make messages
	cd $(topdir)/po/modules/notifier && make messages
	cd $(topdir)/po/modules/options && make messages
	cd $(topdir)/po/modules/perl && make messages
	cd $(topdir)/po/modules/perlcore && make messages
	cd $(topdir)/po/modules/sharedfileswindow && make messages
	cd $(topdir)/po/modules/theme && make messages

###############################################################################
# Generate documentation
###############################################################################

docs:
	$(mkinstalldirs) $(DESTDIR)$(helpdir)
	perl admin/gendoc.pl -v $(VERSION) $(DESTDIR)$(helpdir) \
          data/doctemplates/*.template \
          `find ./ -name "*.cpp" -print`

###############################################################################
# Finished :)
###############################################################################

banner:
	@echo "################################################################################"
	@echo "### INSTALLATION COMPLETE"
	@echo "################################################################################"
	@echo "###"
	@echo "### KVIrc is now installed and ready to run."
	@echo "###"
	@echo "### Please send bug reports and suggestions"
	@echo "### to pragma at kvirc dot net"
	@echo "### You can use the mail report form on"
	@echo "###"
	@echo "### http://www.kvirc.net"
	@echo "###"
	@echo "### We hope that you will enjoy KVIrc."
	@echo "###"
	@echo "### Szymon Stefanek and the KVIrc Development Team"
	@echo "###"
	@echo "################################################################################"
