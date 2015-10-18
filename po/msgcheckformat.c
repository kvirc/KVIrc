/*
    Checks the po file for C format correctness.
    To compile it you need the gettext package installed
    with its development files.
    Compilation commandline:

      gcc -lgettextpo -o msgcheckformat msgcheckformat.c

    Usage:

      msgcheckformat <pofile> [ask|fix]

    ----------------------------------------------------------------------------

    Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net),
                  2010 Fabio Bas (ctrlaltca at gmail dot com)

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
int total_remove = 0;
int total_ignore = 0;
int total_fix = 0;
int solution_mode = 0;

static void po_xerror( int severity, po_message_t message,
                       const char *filename, size_t lineno, size_t column,
                       int multiline_p, const char *message_text )
{
    fprintf( stderr, "%s:%u:%u: %s\n",
             filename, (unsigned int)lineno, (unsigned int)column, message_text );
    if (severity) exit(1);
}

static void po_xerror2( int severity, po_message_t message1,
                        const char *filename1, size_t lineno1, size_t column1,
                        int multiline_p1, const char *message_text1,
                        po_message_t message2,
                        const char *filename2, size_t lineno2, size_t column2,
                        int multiline_p2, const char *message_text2 )
{
    fprintf( stderr, "%s:%u:%u: %s\n",
             filename1, (unsigned int)lineno1, (unsigned int)column1, message_text1 );
    fprintf( stderr, "%s:%u:%u: %s\n",
             filename2, (unsigned int)lineno2, (unsigned int)column2, message_text2 );
    if (severity) exit(1);
}

static const struct po_xerror_handler po_xerror_handler = { po_xerror, po_xerror2 };

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


static int print_error(po_message_t m,const char * msgid,const char * msgstr,const char * error, int solution)
{
	char bufferid[16000];
	char bufferstr[16000];
	int s = 0;
	int is_fuzzy = po_message_is_fuzzy(m) ? 1 : 0;
	replace_crlf(msgid,bufferid);
	replace_crlf(msgstr,bufferstr);
	fprintf(stderr,"%s in %s:\n msgid: %s\n msgstr: %s\n reason: %s\n\n",
		is_fuzzy ? "Warning" : "Error",
		is_fuzzy ? "fuzzy entry" : "entry",
		bufferid,bufferstr,error);
	if(is_fuzzy)total_warnings++;
	else total_errors++;
	switch(solution_mode)
	{
		case 1:
			// ask the user
			switch(solution)
			{
				case 1:
					printf("Pick a solution: (i)gnore the problem, or (r)emove the offending translation:");
					while(s != 'i' && s != 'r') s = getchar();
					if(s == 'r')
					{
						// delete the wrong translation
						/* Change the msgstr (translation) of a message.
							Use an empty string to denote an untranslated message.  */
						po_message_set_msgstr(m, "");
						total_remove++;
					} else {
						total_ignore++;
					}
					break;
				case 2:
					printf("Pick a solution: (i)gnore the problem, (f)ix it or (r)emove the offending translation:");
					while(s != 'i' && s != 'f' && s != 'r') s = getchar();
					if(s == 'r')
					{
						// delete the wrong translation
						/* Change the msgstr (translation) of a message.
							Use an empty string to denote an untranslated message.  */
						po_message_set_msgstr(m, "");
						total_remove++;
					} else if(s == 'f') {
						total_fix++;
						return 1;
					} else {
						total_ignore++;
					}
					break;
			}
			break;
		case 2:
			// fix automatically
			switch(solution)
			{
				case 1:
					// delete the wrong translation
					/* Change the msgstr (translation) of a message.
						Use an empty string to denote an untranslated message.  */
					po_message_set_msgstr(m, "");
					total_remove++;
					break;
				case 2:
					total_fix++;
					return 1;
					break;
			}
			break;
		default:
			// do nothing
			break;
	}
	return 0;
}

inline int is_valid_fmtstr(const char * p)
{
	if(!*p) return 0;
	if(*p != '%') return 0;
	p++;
	if(!*p) return 0;
	if((*p >= '1' && *p <= '9') ||
		(*p >= 'a' && *p <= 'z') ||
		(*p >= 'A' && *p <= 'Z'))
	{
		p--;
		return 1;
	}
	p--;
	return 0;
}

static void process_message(po_message_t m)
{
	const char * msgstr = po_message_msgstr(m);
	const char * msgid = po_message_msgid(m);
	const char *p;
	char *q;
	char *fixedstr;

	if(msgid[0] == '\0')return;
	if(strlen(msgid) < 1)return;
	if(strlen(msgstr) < 1)return;
	if(msgstr[0] == '\0')return;

	fixedstr = strdup(msgstr);

	p = msgid;
	q = fixedstr;

	while(*p && *q)
	{
		while(*p && !is_valid_fmtstr(p))p++;
		while(*q && !is_valid_fmtstr(q))q++;
		if(*q && *p)
		{
			q++;
			p++;
			if(*p != *q)
			{
				char buff[200];
				sprintf(buff,"Mismatched format character (%c != %c)",*p,*q);
				if(print_error(m,msgid,fixedstr,buff, 2))
				{
					//fix the format string
					*q=*p;
					po_message_set_msgstr(m, fixedstr);
				}
			}
		}
	}

	if(*p || *q)
	{
		print_error(m,msgid,msgstr,"Mismatched number of format characters", 1);
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

	if(!argv[1])
	{
		fprintf(stderr,"usage: %s <pofile> [ask|fix]\n",argv[0]);
		return 0;
	}

	if(argv[2])
	{
		if(!strncmp(argv[2], "ask", 3))
		{
			solution_mode=1;
		} else if(!strncmp(argv[2], "fix", 3)) {
			solution_mode=2;
		} else {
			fprintf(stderr,"usage: %s <pofile> [ask|fix]\n",argv[0]);
			return 0;
		}
	}

	po = po_file_read(argv[1], &po_xerror_handler);

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

	if(total_errors == 0 && total_warnings == 0)
	{
		fprintf(stderr,"No errors found\n");
	} else if(solution_mode==0) {
		fprintf(stderr,"%d warnings, %d errors\n",total_warnings,total_errors);
	} else {
		fprintf(stderr,"%d warnings, %d errors, %d fixes, %d removed, %d ignored\n",total_warnings,total_errors, total_fix, total_remove, total_ignore);
		if(total_fix || total_remove)
		{
			int s = 0;
			printf("Commit changes to file? (y)es, (n)o:");
			while(s != 'y' && s != 'n') s = getchar();
			if(s == 'y')
			{
				if(po_file_write(po, argv[1], &po_xerror_handler)) printf("Changes committed.");
				else printf("Error writing file.");
			} else {
				printf("Changes discarded.");
			}
		}
	}

	po_file_free(po);

	return 0;
}
