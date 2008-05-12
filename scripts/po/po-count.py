#!/usr/bin/env python
# Modified for KVIrc by Elvio Basello <hellvis69 at netsons dot org> 2008
# Copyright (C) 1998, 1999, 2000 Progiciels Bourbeau-Pinard inc.
# Francois Pinard <pinard@iro.umontreal.ca>, 1998.

"""\
Count entries of various types in a PO file.

Usage: po-count [FILE]...
"""

import os, sys
sys.path.insert(0, os.path.expanduser('./'))

import libpo

def main(*arguments):
	write = sys.stdout.write
	if len(arguments) == 0:
		write('%s\n' % numbers(libpo.stats(libpo.read('-'))))
	else:
		for file in arguments:
			write('File: %s\n%s\n' % (file,numbers(libpo.stats(libpo.read(file)))))

def numbers(stats):
	return 'Translated: %d\nFuzzy: %d\nObsolete: %d\nPercentage: %d' % (stats['translated'],stats['fuzzy'] + stats['untranslated'],stats['obsolete'],libpo.percentage(stats))

if __name__ == '__main__':
	apply(main, tuple(sys.argv[1:]))
