//=============================================================================
//
//   File : MpMp3.h
//   Creation date : Fri Mar 25 20:01:25 2005 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This file is based on the mp3tech.h. It is released under the original
//   license and the original copyright notice follows.
//
//   mp3tech.hCO
//
//   Copyright (C) 2000  Cedric Tefft <cedric@earthling.net>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software Foundation
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//   This file is based in part on:
//
//   * MP3Info 0.5 by Ricardo Cerqueira <rmc@rccn.net>
//   * MP3Stat 0.9 by Ed Sweetman <safemode@voicenet.com> and
//                    Johannes Overmann <overmann@iname.com>
//
//   There has been also a remarkable work by Cristopher Tieckle (Crissi)
//
//=============================================================================

// MIN_CONSEC_GOOD_FRAMES defines how many consecutive valid MP3 frames
//   we need to see before we decide we are looking at a real MP3 file
#define MIN_CONSEC_GOOD_FRAMES 4
#define FRAME_HEADER_SIZE 4
#define MIN_FRAME_SIZE 21
#define NUM_SAMPLES 4
#define TEXT_FIELD_LEN 30
#define INT_FIELD_LEN 4

#include "kvi_settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <QString>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#endif

enum VBR_REPORT
{
	VBR_VARIABLE,
	VBR_AVERAGE,
	VBR_MEDIAN
};

struct mp3header
{
	unsigned int sync;
	unsigned int version;
	unsigned int layer;
	unsigned int crc;
	unsigned int bitrate;
	unsigned int freq;
	unsigned int padding;
	unsigned int extension;
	unsigned int mode;
	unsigned int mode_extension;
	unsigned int copyright;
	unsigned int original;
	unsigned int emphasis;
};

struct id3tag
{
	char title[31];
	char artist[31];
	char album[31];
	char year[5];
	char comment[31];
	unsigned char track[1];
	unsigned char genre[1];
};

struct mp3info
{
	QString filename;
	FILE * file;
	unsigned int datasize;
	int header_isvalid;
	mp3header header;
	int id3_isvalid;
	id3tag id3;
	int vbr;
	float vbr_average;
	int seconds;
	int frames;
	int badframes;
};

// mode field:
// 00 - Stereo
// 01 - Joint stereo (Stereo)
// 10 - Dual channel (2 mono channels)
// 11 - Single channel (Mono)

bool scan_mp3_file(QString & szFileName, mp3info * i);

void resetmp3infoStruct(mp3info * i);

int header_channels(mp3header * h);
int header_frequency(mp3header * h);
const char * header_emphasis(mp3header * h);
const char * header_mode(mp3header * h);
int header_layer(mp3header * h);
int header_bitrate(mp3header * h);
double header_version(mp3header * h);
int header_crc(mp3header * h);
int get_header(FILE * file, mp3header * header);
int frame_length(mp3header * header);
int sameConstant(mp3header * h1, mp3header * h2);
int get_mp3_info(mp3info * mp3);
int get_id3(mp3info * mp3);
char * pad(char * string, int length);
char * unpad(char * string);
int get_first_header(mp3info * mp3, long startpos);
//void tagedit_curs(char *filename, int filenum, int fileoutof, id3tag *tag);
const char * get_typegenre(int idx);
