#
#	Makefile to create a KVIrc 3 Debian package. This is only used by the
#	package maintainer normally, not as part of a dpkg-buildpackage routine.
#
NAME=kvirc
CVSROOT=:pserver:plugin@cvs.kvirc.net:/cvs
CVS_MODULE=kvirccvs
SRC_DIR=kvirc
PREDEPENDS=autoconf automake1.9 autotools-dev gettext libtool

DEBIAN_RELEASE=1

include Makefile.generic

#
# Reset this to make signing the packages possible
#
DPKG_OPTS=#-si
export DPKG_OPTS

source-post-hook::
	#
	#	Remove CVS directories lying around
	#
	if [ "$(DEBIAN_RELEASE)" = "1" ]; then \
		find "$(DIR)" -type d -name CVS -print0 | xargs -r0 rm -rf; \
		find "$(DIR)" -name .cvsignore  -print0 | xargs -r0 rm -f;  \
	fi

	#
	#	This should not be distributed
	#
	rm -rf "$(DIR)/doc/no-dist"
	if [ ! -z "$(NON_DISTR)" ]; then \
		for dir in $(NON_DISTR); do rm -rf "$(DIR)/src/plugins/$$dir"; done; \
	fi

	#
	#	Put the right prefix in the man page
	#
	perl -pi -e 's,\\fB\$$PREFIX\\fP,\/usr,g' "$(DIR)/data/man/kvirc.1"

	#
	#	Copy ChangeLog to changelog
	#
	[ ! -f "$(DIR)/changelog" ] && ln "$(DIR)/ChangeLog" "$(DIR)/changelog"

	#
	#	Enable maintainer mode if DEBIAN_RELEASE == 0
	#
	if test "$(DEBIAN_RELEASE)" = "0"; then \
		perl -pi \
			-e "s,(DEB_CONFIGURE_EXTRA_FLAGS :=),\1 --enable-maintainer-mode," \
			"$(TMP_DEBIAN_DIR)/rules"; \
	fi

	#
	#	Automake work should be done here, outside the Debian package
	#
	cd "$(DIR)" && patch -p0 < "$(CWD)/$(TMP_DEBIAN_DIR)/patches/01_rpath.patch.noauto_apply"
	cd "$(DIR)" && patch -p0 < "$(CWD)/$(TMP_DEBIAN_DIR)/patches/04_buildlog.patch.noauto_apply"
	cd "$(DIR)" && chmod +x autogen.sh && ./autogen.sh

permissions:
	find "$(SRC_DIR)" -type f -print0 \
		-name '*.cpp' -o \
		-name '*.h' -o \
		-name 'Makefile.am' -o \
		-name '*.kvihelp' -o \
		-name '*.vcproj' -o \
		-name 'configure.in' -o \
		-name 'acinclude.*' \
		| xargs -r0 chmod 644

.PHONY:: permissions
