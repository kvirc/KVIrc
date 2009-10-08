/*
    Checks the po file for C format correctness.
    To compile it you need the gettext package installed
    with its development files.
    Compilation commandline:

      gcc -lgettextpo -o msgcheckformat msgcheckformat.c

    Usage:

      msgcheckformat <pofile>

    ----------------------------------------------------------------------------

    Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <gettext-po.h>

int total_errors = 0;
int total_warnings = 0;

static void replace_crlf(const char * msg,char * buf)
{
	while(*msg)
	{
		if(*msg == '\r')
		{
			*buf++ = '\\';
			*buf++ = 'r';
			msg++;
		} else if(*msg == '\n')
		{
			*buf++ = '\\';
			*buf++ = 'n';
			msg++;
		} else *buf++ = *msg++;
	}
}


static void print_error(po_message_t m,const char * msgid,const char * msgstr,const char * error)
{
	char bufferid[16000];
	char bufferstr[16000];
	int is_fuzzy = po_message_is_fuzzy(m) ? 1 : 0;
	replace_crlf(msgid,bufferid);
	replace_crlf(msgstr,bufferstr);
	fprintf(stderr,"%s in %s:\n msgid: %s\n msgstr: %s\n reason: %s\n\n",
		is_fuzzy ? "Warning" : "Error",
		is_fuzzy ? "fuzzy entry" : "entry",
		bufferid,bufferstr,error);
	if(is_fuzzy)total_warnings++;
	else total_errors++;
}

static void process_message(po_message_t m)
{
	const char * msgstr = po_message_msgstr(m);
	const char * msgid = po_message_msgid(m);
	const char *p;
	const char *q;

	if(msgid[0] == '\0')return;
	if(strlen(msgid) < 1)return;
	if(strlen(msgstr) < 1)return;
	if(msgstr[0] == '\0')return;

	p = msgid;
	q = msgstr;

	while(*p && *q)
	{
		while(*p && (*p != '%'))p++;
		while(*q && (*q != '%'))q++;
		if(*q && *p)
		{
			q++;
			p++;
			if(*p != *q)
			{
				char buff[200];
				sprintf(buff,"Mismatched format character (%c != %c)",*p,*q);
				print_error(m,msgid,msgstr,buff);
			}
		}
	}

	if(*p || *q)
	{
		print_error(m,msgid,msgstr,"Mismatched number of format characters");
	}
}

static void process_messages(po_message_iterator_t it)
{
	po_message_t m = po_next_message(it);
	while(m)
	{
		process_message(m);
		m = po_next_message(it);
	}
}

int main(int argc, char **argv)
{
	po_file_t po;
	po_message_iterator_t it;
	const char * const * domains;

	if(!*argv[1])
	{
		fprintf(stderr,"usage: %s <pofile>\n",argv[0]);
		return 0;
	}

	po = po_file_read(argv[1]);

	if(!po)
	{
		fprintf(stderr,"Couldn't read the input po file\n");
		return 0;
	}

	domains = po_file_domains(po);

	if(!domains)
	{
		fprintf(stderr,"Couldn't find the message domains in the po file\n");
		return 0;
	}

	while(*domains)
	{
		it = po_message_iterator(po,*domains);
		process_messages(it);
		po_message_iterator_free(it);
		domains++;
	}

	po_file_free(po);

	if(total_errors == 0 && total_warnings == 0)fprintf(stderr,"No errors found\n");
	else fprintf(stderr,"%d warnings, %d errors\n",total_warnings,total_errors);

	return 0;
}

