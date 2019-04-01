//=============================================================================
//
//   File : MpMp3.cpp
//   Creation date : Fri Mar 25 20:01:25 2005 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This file is based on the mp3tech.c. It is released under the original
//   license and the original copyright notice follows.
//
//   mp3tech.c
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

#include "MpMp3.h"
#include "KviOptions.h"

#include <QFileInfo>
#include <QTextCodec>

#define MAXGENRE 147
#define GENREROWS 50

/*
   The Information is stored in the last 128 bytes of an MP3. The Tag
   has got the following fields, and the offsets given here, are from
   0-127.

     Field      Length    Offsets
     Tag        3           0-2
     Songname   30          3-32
     Artist     30         33-62
     Album      30         63-92
     Year       4          93-96
     Comment    30         97-126
     Genre      1           127


   The string-fields contain ASCII-data, coded in ISO-Latin 1 codepage.
   Strings which are smaller than the field length are padded with zero-
   bytes.

     Tag: The tag is valid if this field contains the string "TAG". This
        has to be uppercase!

     Songname: This field contains the title of the MP3 (string as
        above).

     Artist: This field contains the artist of the MP3 (string as above).

     Album: this field contains the album where the MP3 comes from
        (string as above).

     Year: this field contains the year when this song has originally
        been released (string as above).

     Comment: this field contains a comment for the MP3 (string as
        above). Revision to this field has been made in ID3v1.1. See
        A.4.

     Genre: this byte contains the offset of a genre in a predefined
        list the byte is treated as an unsigned byte. The offset is
        starting from 0. See A.3.
*/

const char * typegenre[MAXGENRE + 2] = {
	"Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge",
	"Hip-Hop", "Jazz", "Metal", "New Age", "Oldies", "Other", "Pop", "R&B",
	"Rap", "Reggae", "Rock", "Techno", "Industrial", "Alternative", "Ska",
	"Death Metal", "Pranks", "Soundtrack", "Euro-Techno", "Ambient",
	"Trip-Hop", "Vocal", "Jazz+Funk", "Fusion", "Trance", "Classical",
	"Instrumental", "Acid", "House", "Game", "Sound Clip", "Gospel", "Noise",
	"Alt. Rock", "Bass", "Soul", "Punk", "Space", "Meditative",
	"Instrumental Pop", "Instrumental Rock", "Ethnic", "Gothic",
	"Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance",
	"Dream", "Southern Rock", "Comedy", "Cult", "Gangsta Rap", "Top 40",
	"Christian Rap", "Pop/Funk", "Jungle", "Native American", "Cabaret",
	"New Wave", "Psychedelic", "Rave", "Showtunes", "Trailer", "Lo-Fi",
	"Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical",
	"Rock & Roll", "Hard Rock", "Folk", "Folk/Rock", "National Folk",
	"Swing", "Fast-Fusion", "Bebob", "Latin", "Revival", "Celtic",
	"Bluegrass", "Avantgarde", "Gothic Rock", "Progressive Rock",
	"Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band",
	"Chorus", "Easy Listening", "Acoustic", "Humour", "Speech", "Chanson",
	"Opera", "Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus",
	"Porn Groove", "Satire", "Slow Jam", "Club", "Tango", "Samba",
	"Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle",
	"Duet", "Punk Rock", "Drum Solo", "A Cappella", "Euro-House",
	"Dance Hall", "Goa", "Drum & Bass", "Club-House", "Hardcore", "Terror",
	"Indie", "BritPop", "Negerpunk", "Polsk Punk", "Beat",
	"Christian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover",
	"Contemporary Christian", "Christian Rock", "Merengue", "Salsa",
	"Thrash Metal", "Anime", "JPop", "Synthpop", ""
};

const char * get_typegenre(int idx)
{
	if(idx > MAXGENRE)
		return typegenre[12];
	return typegenre[idx];
}

int galphagenreindex[MAXGENRE + 2] = {
	148, 123, 74, 73, 34, 99, 40, 20, 26, 145, 90,
	116, 41, 135, 85, 96, 138, 89, 0, 107, 132, 65, 88,
	104, 102, 97, 136, 61, 141, 1, 32, 128, 112, 57, 140,
	2, 139, 58, 125, 3, 50, 22, 4, 55, 127, 122, 120,
	98, 52, 48, 124, 25, 54, 84, 81, 115, 80, 119, 5,
	30, 36, 59, 126, 38, 91, 49, 6, 79, 129, 137, 7,
	35, 100, 131, 19, 46, 47, 33, 146, 29, 8, 63, 86,
	71, 45, 142, 9, 77, 82, 64, 133, 10, 66, 39, 11,
	103, 12, 75, 134, 53, 62, 13, 109, 117, 23, 108, 92,
	93, 67, 121, 43, 14, 15, 68, 16, 76, 87, 118, 78,
	17, 143, 114, 110, 69, 21, 111, 95, 105, 42, 37, 24,
	56, 44, 101, 83, 94, 106, 147, 113, 51, 18, 130, 144,
	60, 70, 31, 72, 27, 28
};

int * alphagenreindex = &(galphagenreindex[1]);

int layer_tab[4] = { 0, 3, 2, 1 };

int frequencies[3][4] = {
	{ 22050, 24000, 16000, 50000 }, /* MPEG 2.0 */
	{ 44100, 48000, 32000, 50000 }, /* MPEG 1.0 */
	{ 11025, 12000, 8000, 50000 }   /* MPEG 2.5 */
};

int bitrate[2][3][14] = {
	{
	    /* MPEG 2.0 */
	    { 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256 }, /* layer 1 */
	    { 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 },      /* layer 2 */
	    { 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 }       /* layer 3 */
	},
	{
	    /* MPEG 1.0 */
	    { 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448 }, /* layer 1 */
	    { 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384 },    /* layer 2 */
	    { 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 }      /* layer 3 */
	}
};

int frame_size_index[] = { 24000, 72000, 72000 };

const char * mode_text[] = {
	"Stereo", "Joint Stereo", "Dual Channel", "Mono"
};

const char * emphasis_text[] = {
	"None", "50/15 Microseconds", "Reserved", "CCITT J 17"
};

void resetmp3infoStruct(mp3info * i)
{
	i->file = nullptr;
	i->datasize = 0;
	i->header_isvalid = 0;
	memset(&i->header, 0, sizeof(i->header));
	i->id3_isvalid = 0;
	memset(&i->id3, 0, sizeof(i->id3));
	i->vbr = 0;
	i->vbr_average = 0;
	i->seconds = 0;
	i->frames = 0;
	i->badframes = 0;
}

int get_mp3_info(mp3info * mp3)
{
	//	mp3header header;

	QFile fi(mp3->filename);
	mp3->datasize = fi.size(); //filestat.st_size;

	get_id3(mp3);

	if(get_first_header(mp3, 0L))
	{
		int bitrate, counter = 0;
		int data_start = ftell(mp3->file);
		int lastrate = 15 - mp3->header.bitrate;
		while((counter < NUM_SAMPLES) && lastrate)
		{
			int sample_pos = (counter * (mp3->datasize / NUM_SAMPLES + 1)) + data_start;
			if(get_first_header(mp3, sample_pos))
			{
				bitrate = 15 - mp3->header.bitrate;
			}
			else
			{
				bitrate = -1;
			}

			if(bitrate != lastrate)
			{
				mp3->vbr = 1;
			}
			lastrate = bitrate;
			counter++;
		}
		mp3->frames = (mp3->datasize - data_start) / (frame_length(&mp3->header));
		mp3->seconds = (int)((float)(frame_length(&mp3->header) * mp3->frames) / (float)(header_bitrate(&mp3->header) * 125) + 0.5);
		mp3->vbr_average = (float)header_bitrate(&mp3->header);
	}

	return 0;
}

int get_first_header(mp3info * mp3, long startpos)
{
	int k, l = 0, c;
	mp3header h, h2;
	long valid_start = 0;

	fseek(mp3->file, startpos, SEEK_SET);
	while(true)
	{
		while((c = fgetc(mp3->file)) != 255 && (c != EOF))
		{
		}
		if(c == 255)
		{
			ungetc(c, mp3->file);
			valid_start = ftell(mp3->file);
			if((l = get_header(mp3->file, &h)))
			{
				fseek(mp3->file, l - FRAME_HEADER_SIZE, SEEK_CUR);
				for(k = 1; (k < MIN_CONSEC_GOOD_FRAMES) && (mp3->datasize - ftell(mp3->file) >= FRAME_HEADER_SIZE); k++)
				{
					if(!(l = get_header(mp3->file, &h2)))
						break;
					if(!sameConstant(&h, &h2))
						break;
					fseek(mp3->file, l - FRAME_HEADER_SIZE, SEEK_CUR);
				}
				if(k == MIN_CONSEC_GOOD_FRAMES)
				{
					fseek(mp3->file, valid_start, SEEK_SET);
					memcpy(&(mp3->header), &h2, sizeof(mp3header));
					mp3->header_isvalid = 1;
					return 1;
				}
			}
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

// Get next MP3 frame header.
//   Return codes:
//   positive value = Frame Length of this header
//   0 = No, we did not retrieve a valid frame header

int get_header(FILE * file, mp3header * header)
{
	unsigned char buffer[FRAME_HEADER_SIZE];
	int fl;

	if(fread(&buffer, FRAME_HEADER_SIZE, 1, file) < 1)
	{
		header->sync = 0;
		return 0;
	}
	header->sync = (((int)buffer[0] << 4) | ((int)(buffer[1] & 0xE0) >> 4));
	if(buffer[1] & 0x10)
		header->version = (buffer[1] >> 3) & 1;
	else
		header->version = 2;
	header->layer = (buffer[1] >> 1) & 3;
	if((header->sync != 0xFFE) || (header->layer != 1))
	{
		header->sync = 0;
		return 0;
	}
	header->crc = buffer[1] & 1;
	header->bitrate = (buffer[2] >> 4) & 0x0F;
	header->freq = (buffer[2] >> 2) & 0x3;
	header->padding = (buffer[2] >> 1) & 0x1;
	header->extension = (buffer[2]) & 0x1;
	header->mode = (buffer[3] >> 6) & 0x3;
	header->mode_extension = (buffer[3] >> 4) & 0x3;
	header->copyright = (buffer[3] >> 3) & 0x1;
	header->original = (buffer[3] >> 2) & 0x1;
	header->emphasis = (buffer[3]) & 0x3;

	return ((fl = frame_length(header)) >= MIN_FRAME_SIZE ? fl : 0);
}

int frame_length(mp3header * header)
{
	return header->sync == 0xFFE ? (frame_size_index[3 - header->layer] * ((header->version & 1) + 1) * header_bitrate(header) / header_frequency(header)) + header->padding : 1;
}

int header_layer(mp3header * h)
{
	return layer_tab[h->layer];
}

int header_bitrate(mp3header * h)
{
	if(h->bitrate > 0)
		return bitrate[h->version & 1][3 - h->layer][h->bitrate - 1];
	else
		return -1; // unknown
}

int header_frequency(mp3header * h)
{
	return frequencies[h->version][h->freq];
}

const char * header_emphasis(mp3header * h)
{
	return emphasis_text[h->emphasis];
}

const char * header_mode(mp3header * h)
{
	return mode_text[h->mode];
}

int header_channels(mp3header * h)
{
	return h->mode == 3 ? 1 : 2;
}

int header_crc(mp3header * h)
{
	return (!h->crc);
}

int sameConstant(mp3header * h1, mp3header * h2)
{
	if((*(uint *)h1) == (*(uint *)h2))
		return 1;

	if((h1->version == h2->version) && (h1->layer == h2->layer) && (h1->crc == h2->crc) && (h1->freq == h2->freq) && (h1->mode == h2->mode) && (h1->copyright == h2->copyright) && (h1->original == h2->original) && (h1->emphasis == h2->emphasis))
		return 1;
	else
		return 0;
}

int get_id3(mp3info * mp3)
{
	// this will read ID3v1 tags
	int retcode = 0;
	char fbuf[4];

	if(mp3->datasize >= 128)
	{
		if(fseek(mp3->file, -128, SEEK_END))
		{
			retcode |= 4;
		}
		else
		{
			(void)fread(fbuf, 1, 3, mp3->file);
			fbuf[3] = '\0';
			mp3->id3.genre[0] = 255;

			if(!strcmp((const char *)"TAG", (const char *)fbuf))
			{
				mp3->id3_isvalid = 1;
				mp3->datasize -= 128;
				fseek(mp3->file, -125, SEEK_END);
				(void)fread(mp3->id3.title, 1, 30, mp3->file);
				mp3->id3.title[30] = '\0';
				(void)fread(mp3->id3.artist, 1, 30, mp3->file);
				mp3->id3.artist[30] = '\0';
				(void)fread(mp3->id3.album, 1, 30, mp3->file);
				mp3->id3.album[30] = '\0';
				(void)fread(mp3->id3.year, 1, 4, mp3->file);
				mp3->id3.year[4] = '\0';
				(void)fread(mp3->id3.comment, 1, 30, mp3->file);
				mp3->id3.comment[30] = '\0';
				if(mp3->id3.comment[28] == '\0')
				{
					mp3->id3.track[0] = mp3->id3.comment[29];
				}
				(void)fread(mp3->id3.genre, 1, 1, mp3->file);
				unpad(mp3->id3.title);
				unpad(mp3->id3.artist);
				unpad(mp3->id3.album);
				unpad(mp3->id3.year);
				unpad(mp3->id3.comment);
			}
		}
	}
	return retcode;
}

char * pad(char * string, int length)
{
	int l;

	l = strlen(string);
	while(l < length)
	{
		string[l] = ' ';
		l++;
	}

	string[l] = '\0';
	return string;
}

// Remove trailing whitespace from the end of a string
char * unpad(char * string)
{
	char * pos = string + strlen(string) - 1;
	while(isspace(pos[0]))
		(pos--)[0] = 0;
	return string;
}

bool scan_mp3_file(QString &, mp3info * i)
{
	//memset(i,0,sizeof(mp3info));
	resetmp3infoStruct(i);

	i->filename = "text";
	i->file = fopen(QTextCodec::codecForLocale()->fromUnicode(i->filename).data(), "rb");
	if(!i->file)
		return false;

	get_mp3_info(i);

	fclose(i->file);

	return (i->id3_isvalid);
}
