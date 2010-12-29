//=============================================================================
//
//   File : sonix_compress.cpp
//   Creation date : Tue Nov 10 18:08:09 2009 GMT by Fabio Bas
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2009 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//   This file has been taken from the "Kopete, The KDE Instant Messenger"
//   sourcecode. Original file had no copyright header.
//=============================================================================

#include "sonix_compress.h"

#define CLAMP(x)	((x)<0?0:((x)>255)?255:(x))

typedef struct {
	int is_abs;
	int len;
	int val;
	int unk;
} code_table_t;


/* local storage */
static code_table_t table[256];
static int init_done = 0;

/* global variable */
int sonix_unknown = 0;

/*
	sonix_decompress_init
	=====================
		pre-calculates a locally stored table for efficient huffman-decoding.

	Each entry at index x in the table represents the codeword
	present at the MSB of byte x.

*/
void sonix_decompress_init(void)
{
	int i;
	int is_abs, val, len, unk;

	for (i = 0; i < 256; i++) {
		is_abs = 0;
		val = 0;
		len = 0;
		unk = 0;
		if ((i & 0x80) == 0) {
			/* code 0 */
			val = 0;
			len = 1;
		}
		else if ((i & 0xE0) == 0x80) {
			/* code 100 */
			val = +4;
			len = 3;
		}
		else if ((i & 0xE0) == 0xA0) {
			/* code 101 */
			val = -4;
			len = 3;
		}
		else if ((i & 0xF0) == 0xD0) {
			/* code 1101 */
			val = +11;
			len = 4;
		}
		else if ((i & 0xF0) == 0xF0) {
			/* code 1111 */
			val = -11;
			len = 4;
		}
		else if ((i & 0xF8) == 0xC8) {
			/* code 11001 */
			val = +20;
			len = 5;
		}
		else if ((i & 0xFC) == 0xC0) {
			/* code 110000 */
			val = -20;
			len = 6;
		}
		else if ((i & 0xFC) == 0xC4) {
			/* code 110001xx: unknown */
			val = 0;
			len = 8;
			unk = 1;
		}
		else if ((i & 0xF0) == 0xE0) {
			/* code 1110xxxx */
			is_abs = 1;
			val = (i & 0x0F) << 4;
			len = 8;
		}
		table[i].is_abs = is_abs;
		table[i].val = val;
		table[i].len = len;
		table[i].unk = unk;
	}

	sonix_unknown = 0;
	init_done = 1;
}


/*
	sonix_decompress
	================
		decompresses an image encoded by a SN9C101 camera controller chip.

	IN	width
		height
		inp		pointer to compressed frame (with header already stripped)
	OUT	outp	pointer to decompressed frame

	Returns 0 if the operation was successful.
	Returns <0 if operation failed.

*/
int sonix_decompress(int width, int height, unsigned char *inp, unsigned char *outp)
{
	int row, col;
	int val;
	int bitpos;
	unsigned char code;
	unsigned char *addr;

	if (!init_done) {
		/* do sonix_decompress_init first! */
		return -1;
	}

	bitpos = 0;
	for (row = 0; row < height; row++) {

		col = 0;

		/* first two pixels in first two rows are stored as raw 8-bit */
		if (row < 2) {
			addr = inp + (bitpos >> 3);
			code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
			bitpos += 8;
			*outp++ = code;

			addr = inp + (bitpos >> 3);
			code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
			bitpos += 8;
			*outp++ = code;

			col += 2;
		}

		while (col < width) {
			/* get bitcode from bitstream */
			addr = inp + (bitpos >> 3);
			code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));

			/* update bit position */
			bitpos += table[code].len;

			/* update code statistics */
			sonix_unknown += table[code].unk;

			/* calculate pixel value */
			val = table[code].val;
			if (!table[code].is_abs) {
				/* value is relative to top and left pixel */
				if (col < 2) {
					/* left column: relative to top pixel */
					val += outp[-2*width];
				}
				else if (row < 2) {
					/* top row: relative to left pixel */
					val += outp[-2];
				}
				else {
					/* main area: average of left pixel and top pixel */
					val += (outp[-2] + outp[-2*width]) / 2;
				}
			}

			/* store pixel */
			*outp++ = CLAMP(val);
			col++;
		}
	}

	return 0;
}
