SUBDIRS = libtool

EXTRA_DIST = acinclude.m4.in libtool.m4.in gendoc.pl gensrc.pl \
		depcomp config.guess config.sub install-sh ltmain.sh \
		missing objprelink.c sedfiles.sh \
		searchmetaob.sh mkcvstag.sh src_cvstag src_date

objprelink: objprelink.c
	$(CC) -o $@ -O2 $< /usr/lib/libbfd.a /usr/lib/libiberty.a

