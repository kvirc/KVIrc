# Handling of PO files contents.
# Modified for KVIrc by Elvio Basello <hellvis69 at netsons dot org> 2008
# Copyright (C) 2001, 2002 Free Translation Project.
# Copyright (C) 1998, 1999, 2000, 2001 Progiciels Bourbeau-Pinard inc.
# Francois Pinard <pinard@iro.umontreal.ca>, 1998.

import re, string, sys, types
cre = re

def _(text):
	return text

_default_header = { 'TITLE': _('SOME DESCRIPTIVE TITLE.'),
					'COPYRIGHT'
					: 'Copyright (C) YEAR Free Software Foundation, Inc.',
					'FIRST-AUTHOR'
					: _('FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.'),
					'COMMENTS': '',
					'FLAGS': '',
					'project-id-version': _('PACKAGE VERSION'),
					'pot-creation-date': _('YEAR-MO-DA HO:MI +ZONE'),
					'po-revision-date': _('YEAR-MO-DA HO:MI +ZONE'),
					'last-translator': _('FULL NAME <EMAIL@ADDRESS>'),
					'language-team': _('LANGUAGE <LL@li.org>'),
					'mime-version': '1.0',
					'content-type': _('text/plain; charset=CHARSET'),
					'content-transfer-encoding': _('ENCODING'),
					'FIELDS': '' }

_default_order = [ 'Project-Id-Version',
				   'POT-Creation-Date',
				   'PO-Revision-Date',
				   'Last-Translator',
				   'Language-Team',
				   'MIME-Version',
				   'Content-Type',
				   'Content-Transfer-Encoding' ]

# XXX: This doesn't produce the true canonical form: if there are many
# #: source references, it writes them all in a single line, whereas
# GNU gettext breaks them at appropriate places. Use msgcat instead.
def copy_canonical(source, to):
	entries = read(source)
	data = header(entries)
	set_header(entries, data)
	write(to, entries)
	return data

# Input and output.

def read(name):
	entries = []				# retained entries
	entry = {}				# entry being constructed
	keys = {}				# msgid to full entry dictionary
	status = None				# None, 'msgid' or `msgstr'
	msgid = ''				# msgid being accumulated
	msgid_plural = ''
	msgstr = ''				# msgstr being accumulated

	if name == '-':
		file = sys.stdin
	else:
		file = open(name)
	line_count = 0

	for line in file.readlines():
		line = cre.sub(r'\r$', '', line, 1)
		line_count = line_count + 1

		obsolete = line[:2] == '#~'
		# FIXME: Better study the obsoleteness of an obsolete entry.

		line = cre.sub(r'[ \t]+$', '', line, 1)

		if obsolete:
			line = cre.sub('#~ *', '', line, 1)
		if cre.match('$', line):
			continue

		if line[0] == '#':
			if status == 'msgstr':
				entry['msgid'] = unquote(msgid)
				if msgid_plural:
					entry['msgid_plural'] = unquote(msgid_plural)
					entry['msgstr'] = map(unquote, msgstr)
				else:
					entry['msgstr'] = unquote(msgstr)
				_append(entries, entry, keys)
				entry = {}
				status = None
			if obsolete:
				entry['obsolete'] = 1
			if cre.match(r'#[ \t\n]', line):
				if entry.has_key('comment'):
					entry['comment'] = entry['comment'] + line
				elif not cre.match(r'#[ \t\n]*$', line):
					entry['comment'] = line
			elif cre.match(r'#\.', line):
				if entry.has_key('quote'):
					entry['quote'] = entry['quote'] + line
				else:
					entry['quote'] = line
			elif cre.match('#:', line):
				if entry.has_key('refs'):
					entry['refs'] = entry['refs'] + line
				else:
					entry['refs'] = line
			elif cre.match('#,', line):
				if entry.has_key('flags'):
					entry['flags'] = entry['flags'] + line
				else:
					entry['flags'] = line
			else:
				sys.stderr.write(_('%s:%d: Unrecognised line\n%s') % (name, line_count, line))
		elif cre.match('msgid_plural', line):
			msgid_plural = line
			status = 'msgid_plural'
			msgstr = []
		elif cre.match('msgid', line):
			if status == 'msgstr':
				entry['msgid'] = unquote(msgid)
				if msgid_plural:
					entry['msgid_plural'] = unquote(msgid_plural)
					entry['msgstr'] = map(unquote, msgstr)
				else:
					entry['msgstr'] = unquote(msgstr)
				_append(entries, entry, keys)
				entry = {}
			msgid_plural = ''
			if obsolete:
				entry['obsolete'] = 1
			status = 'msgid'
			msgid = line
		elif cre.match('msgstr', line):
			if obsolete:
				entry['obsolete'] = 1
			status = 'msgstr'
			if msgid_plural:
				msgstr.append(line)
			else:
				msgstr = line
		elif cre.match(r'[ \t]*"', line):
			if obsolete:
				entry['obsolete'] = 1
			if status == 'msgid':
				msgid = msgid + line
			elif status == 'msgid_plural':
				msgid_plural = msgid_plural + line
			elif status == 'msgstr':
				if msgid_plural:
					msgstr[-1] = msgstr[-1] + line
				else:
					msgstr = msgstr + line
			else:
				sys.stderr.write(_('%s:%d: Unrecognised line\n%s') % (name, line_count, line))
		else:
			sys.stderr.write(_('%s:%d: Unrecognised line\n%s') % (name, line_count, line))

	if name != '-':
		file.close()

	if status == 'msgstr':
		entry['msgid'] = unquote(msgid)
		if msgid_plural:
			entry['msgid_plural'] = unquote(msgid_plural)
			entry['msgstr'] = map(unquote, msgstr)
		else:
			entry['msgstr'] = unquote(msgstr)
		_append(entries, entry, keys)
	elif status is None and entry.has_key('comment'):
		# Allow terminating comment
		entries.append(entry)

	return entries

def _append(entries, entry, keys):
	msgid = entry['msgid']
	if entry.has_key('obsolete'):
		entries.append(entry)
	else:
		if keys.has_key(msgid):
			old_entry = keys[msgid]
			if old_entry['msgstr'] == entry['msgstr']:
				for field in 'comment', 'quote', 'refs':
					if entry.has_key(field):
						if old_entry.has_key(field):
							old_entry[field] = old_entry[field] + entry[field]
						else:
							old_entry[field] = entry[field]
				if entry.has_key('flags'):
					if old_entry.has_key('flags'):
						if old_entry['flags'] != entry['flags']:
							old_entry['flags'] = (
								old_entry['flags'] + entry['flags'])
					else:
						old_entry['flags'] = entry['flags']
			else:
				sys.stderr.write(requote(_('Duplicate'), msgid))
		else:
			entries.append(entry)
			keys[msgid] = entry

def unquote(text):
	text = cre.sub(r'^[^"]+"([^\0]*)"[^"]*\n$', r'\1', text)
	text = cre.sub(r'"[ \t]*\\?\n(\#~)?[ \t]*"', '', text)
	text = cre.sub(r'\\[ \t]*\n', '', text)

	result = ''
	while 1:
		match = cre.search(r'\\([abfnrt\"\\]|[0-7]+)', text)
		if not match:
			break
		result = result + text[:match.start()]

		group = match.group(1)
		if group == 'a':
			result = result + '\a'
		elif group == 'b':
			result = result + '\b'
		elif group == 'f':
			result = result + '\f'
		elif group == 'n':
			result = result + '\n'
		elif group == 'r':
			result = result + '\r'
		elif group == 't':
			result = result + '\t'
		elif group == '"':
			result = result + '"'
		elif group == '\\':
			result = result + '\\'
		else:
			result = result + chr(string.atoi(group, 8))

		text = text[match.end():]

	return result + text

def write(name, entries):
	first_entry = 1

	if name == '-':
		file = sys.stdout
	else:
		file = open(name, 'w')

	for entry in entries:
		if first_entry:
			first_entry = 0
		else:
			file.write('\n')
		if entry.has_key('obsolete') and entry['obsolete']:
			text = ''
			if entry.has_key('comment'):
				text = text + entry['comment']
			if entry.has_key('quote'):
				text = text + entry['quote']
			if entry.has_key('refs'):
				text = text + entry['refs']
			if entry.has_key('flags'):
				text = text + entry['flags']
			text = text + requote('msgid', entry['msgid'])
			msgstr = entry['msgstr']
			if entry.has_key('msgid_plural'):
				text = text + requote('msgid_plural', entry['msgid_plural'])
				for i in range(len(msgstr)):
					text = text + requote('msgstr[%d]' % i, msgstr[i])
			else:
				text = text + requote('msgstr', msgstr)
			lines = string.split(text, '\n')
			del lines[-1]
			for text in lines:
				if text:
					file.write('#~ %s\n' % text)
				else:
					file.write('#~\n')
		else:
			if entry.has_key('comment'):
				file.write(entry['comment'])
			if not entry.has_key('msgid'):
				continue
			if entry.has_key('quote'):
				file.write(entry['quote'])
			if entry.has_key('refs'):
				file.write(entry['refs'])
			if entry.has_key('flags'):
				file.write(entry['flags'])
			file.write(requote('msgid', entry['msgid']))
			msgstr = entry['msgstr']
			if entry.has_key('msgid_plural'):
				file.write(requote('msgid_plural', entry['msgid_plural']))
				for i in range(len(msgstr)):
					file.write(requote('msgstr[%d]' % i, msgstr[i]))
			else:
				file.write(requote('msgstr', msgstr))

	if name != '-':
		file.close()

def requote(status, text):
	multi = cre.search('[^\n]\n+[^\n]', text)

	text = cre.sub('\\\\', r'\\\\', text)
	text = cre.sub('"', r'\\"', text)
	text = cre.sub('\a', r'\\a', text)
	text = cre.sub('\b', r'\\b', text)
	text = cre.sub('\f', r'\\f', text)
	text = cre.sub('\t', r'\\t', text)
	text = cre.sub('\r', r'\\r', text)

	if multi:
		text = cre.sub(r'\n', r'\\n"\n"', text)
		text = cre.sub(r'"\n"$', '', text)
		return '%s ""\n"%s"\n' % (status, text)

	text = cre.sub(r'\n', r'\\n', text)
	text = cre.sub(r'\t', r'\\t', text)
	return '%s "%s"\n' % (status, text)

def empty(msgstr):
	if isinstance(msgstr, types.StringType):
		return msgstr == ""
	for m in msgstr:
		if m == "":
			return 1
	return 0

def stats(entries):
	translated = fuzzy = untranslated = obsolete = 0
	total_length = translated_length = 0
	for entry in entries:
		if entry.get('msgid'):
			if entry.has_key('obsolete'):
				obsolete = obsolete + 1
			elif (entry.has_key('flags')
				  and cre.search('fuzzy', entry['flags'])):
				total_length = total_length + len(entry['msgid'])
				fuzzy = fuzzy + 1
			elif not empty(entry["msgstr"]):
				total_length = total_length + len(entry['msgid'])
				translated_length = translated_length + len(entry['msgid'])
				translated = translated + 1
			else:
				total_length = total_length + len(entry['msgid'])
				untranslated = untranslated + 1
	return {'translated': translated,
			'fuzzy': fuzzy,
			'untranslated': untranslated,
			'obsolete': obsolete,
			'translated_length': translated_length,
			'total_length': total_length}

def percentage(stats):
	if stats['total_length'] == 0:
		return 0
	return 100 * stats['translated_length'] / stats['total_length']

# Header processing.

def add_copyright(header, lines):
	if not lines:
		return
	if not header['COPYRIGHT']:
		header['COPYRIGHT'] = lines
		return
	if isinstance(header['COPYRIGHT'], types.StringType):
		header['COPYRIGHT'] = [header['COPYRIGHT']]
	for l in lines:
		header['COPYRIGHT'].append(l[2:])

def header(entries):
	entry = entries[0]

	header = { 'TITLE': '',
			   'COPYRIGHT': '',
			   'FIRST-AUTHOR': '',
			   'COMMENTS': '',
			   'FLAGS': '',
			   'project-id-version': '',
			   'pot-creation-date': '',
			   'po-revision-date': '',
			   'last-translator': '',
			   'language-team': '',
			   'mime-version': '',
			   'content-type': '',
			   'content-transfer-encoding': '',
			   'FIELDS': '' }

	if entry['msgid']:
		return header

	if entry.has_key('comment'):
		lines = string.split(entry['comment'], '\n')
		while lines[-1] == '' or cre.match('#[ \t]*$', lines[-1]):
			del lines[-1]

		if len(lines) > 0:
			if not (cre.match('# .*opyright', lines[0])
					or cre.match('# (.*) <(.*)>, .*(199[4-9]|200[0-3])',
								 lines[0])):
				match = cre.match('# +(.*)', lines[0])
				if match:
					header['TITLE'] = match.group(1)
					del lines[0]

		if len(lines) > 0:
			match = cre.match('# +(.*opyright.*)', lines[0])
			if match:
				header['COPYRIGHT'] = match.group(1)
				del lines[0]
				while lines:
					match = cre.match('# +(.*opyright.*)', lines[0])
					if match:
						add_copyright(header, lines[0])
						del lines[0]
					else:
						break

		# Add any lines between the copyright and the first author to
		# the copyright
		for lineno in range(len(lines)):
			match = cre.match('# (.+ <.+>, .*(199[4-9]|200[0-3]|YEAR).*)',
							  lines[lineno])
			if match:
				add_copyright(header, lines[:lineno])
				header['FIRST-AUTHOR'] = match.group(1)
				del lines[:lineno+1]
				break

		if len(lines) > 0:
			header['COMMENTS'] = string.joinfields(lines, '\n') + '\n'

	if entry.has_key('flags'):
		header['FLAGS'] = entry['flags']
		if header['FLAGS'][-1] == '\n':
			header['FLAGS'] = header['FLAGS'][:-1]
		header['FLAGS'] = cre.sub('^#, *', '', header['FLAGS'], 1)

	for text in string.split(entry['msgstr'], '\n'):
		match = cre.match('([^:]+):[ \t]+(.*)', text)
		if match:
			field = string.lower(match.group(1))
			if header.has_key(field):
				header[field] = match.group(2)
				header[field] = cre.sub(r'[ \t]+$', '', header[field], 1)
			else:
				header['FIELDS'] = header['FIELDS'] + text + '\n'

	return header

def set_header(entries, header):
	creation = 'pot-creation-date'
	revision = 'po-revision-date'
	fuzzy = 0
	for field in _default_header.keys():
		if not (header.has_key(field) and header[field]):
			# A while ago, POT files did not have POT-Creation-Date.
			# Do not fuzzy the header merely to add such a field.  Instead,
			# and short of better, use PO-Revision-Date value if it exists.
			if (field == creation and header.has_key(revision)
				and header[revision] != _default_header[revision]):
				header[creation] = header[revision]
			else:
				header[field] = _default_header[field]
				if _default_header[field]:
					fuzzy = 1
	if fuzzy:
		if header['FLAGS']:
			if not cre.search('fuzzy', header['FLAGS']):
				header['FLAGS'] = header['FLAGS'] + ', fuzzy'
		else:
			header['FLAGS'] = 'fuzzy'

	entry = { 'msgid': '' }

	if isinstance(header["COPYRIGHT"], types.ListType):
		copyright = ""
		for h in header["COPYRIGHT"]:
			copyright = copyright + ('# %s\n' % h)
	else:
		copyright = '# %s\n' % header["COPYRIGHT"]

	entry['comment'] = ('# %s\n' % header['TITLE']
			+ copyright
			+ '# %s\n' % header['FIRST-AUTHOR']
			+ header['COMMENTS']
			+ '#\n')

	if header['FLAGS']:
		entry['flags'] = '#, %s\n' % header['FLAGS']

	text = ''
	for field in _default_order:
		text = text + '%s: %s\n' % (field, header[string.lower(field)])
	text = text + header['FIELDS']
	entry['msgstr'] = text

	if entries[0]['msgid']:
		entries.insert(0, entry)
	else:
		entries[0] = entry

def decfunc(header):
	retval = lambda x:(x,len(x))
	match = re.match('text/plain; charset=(.*)', header['content-type'])
	if not match:
		retval
	charset = match.group(1)
	import codecs
	try:
		retval = codecs.lookup(charset)[1]
	except (LookupError,ValueError):
		pass
	return retval

def last_translator(header):
	d = decfunc(header)
	text = header['last-translator']
	match = re.match('(.*?)( +)<(.+)>$', text)
	if match:
		last_translator, last_address = match.group(1, 3)
		last_translator = d(last_translator)[0] # ignore len
		return last_translator, last_address
	return header['last-translator'], None
