#ifndef KVI_OGGIRCT_H_
#define KVI_OGGIRCT_H_
//=============================================================================
//
//   File : KviOggIrcText.h
//   Creation date : Sat Apr 8 2010 22:10:10 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at libero dot it)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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
//=============================================================================

/**
* \file KviOggIrcText.h
* \author Fabio Bas
* \brief Contains the KviOggIrcText namespace;
*/

/**
* \namespace KviOggIrcText
* \brief A namespace implementing our tricky codec to send and receive text multiplexed inside an ogg stream
*/
namespace KviOggIrcText
{
	/**
	* \brief Reads len bytes from the bitpacking buffer opb to buf
	* \param opb The bitpacking buffer containing data
	* \param buf Address of a preallocated buffer where to copy data
	* \param len Length in octects to copy
	* \return void
	*/
	static void _tp_readbuffer(oggpack_buffer * opb, char * buf, const long len)
	{
		long i;

		for(i = 0; i < len; i++)
			*buf++ = oggpack_read(opb, 8);
	}

	/**
	* \brief Writes len bytes from buf to the bitpacking buffer opb
	* \param opb The bitpacking buffer where to copy data
	* \param buf Address of a buffer containing source data
	* \param len Length in octects to copy
	* \return void
	*/
	static void _tp_writebuffer(oggpack_buffer * opb, const char * buf, const long len)
	{
		long i;

		for(i = 0; i < len; i++)
			oggpack_write(opb, *buf++, 8);
	}

	/**
	* \brief Irct codec constructor function; this is where the codec state gets created (unused by now)
	* \return int
	*/
	static int irct_encode_init()
	{
		return 0;
	};

	/**
	* \brief Irct codec destructor function; this is where the codec state gets cleared (unused by now)
	* \return int
	*/
	static int irct_encode_clear()
	{
		return 0;
	};

	/**
	* \brief Creates an irct header (used when encoding)
	* \param op the ogg packet that will contain the header
	* \return int
	*/
	static int irct_encode_headerout(ogg_packet * op)
	{
		oggpack_buffer ob;
		oggpack_writeinit(&ob);
		oggpack_write(&ob, 0, 8); //header init
		_tp_writebuffer(&ob, "irct", 32);
		oggpack_write(&ob, 0, 8); //version 0
		oggpack_write(&ob, 1, 8); //subversion 1
		int bytes = oggpack_bytes(&ob);
		op->packet = (unsigned char *)KviMemory::allocate(bytes);
		memcpy(op->packet, oggpack_get_buffer(&ob), bytes);
		op->bytes = bytes;
		oggpack_writeclear(&ob);
		op->b_o_s = 1; //begins a logical bitstream
		op->e_o_s = 0;
		op->packetno = 0;
		op->granulepos = 0;

		return 0;
	}

	/**
	* \brief Creates an irct packet from some binary text (used when encoding)
	* \param textPkt the buffer containing the text
	* \param textSize size of the text buffer
	* \param last_p packet number in the irct stream
	* \param op the ogg packet that will contain the text
	* \return int
	*/
	static int irct_encode_packetout(const char * textPkt, int textSize, int last_p, ogg_packet * op)
	{
		if(!textSize)
			return (0);

		oggpack_buffer ob;
		oggpack_writeinit(&ob);

		_tp_writebuffer(&ob, textPkt, textSize); //pre-encoded text
		int bytes = oggpack_bytes(&ob);
		op->packet = (unsigned char *)KviMemory::allocate(bytes);
		memcpy(op->packet, oggpack_get_buffer(&ob), bytes);
		op->bytes = bytes;
		oggpack_writeclear(&ob);
		op->b_o_s = 0;
		op->e_o_s = last_p;

		op->packetno = last_p;
		op->granulepos = 0;

		return 0;
	}

	/**
	* \brief Decoded an irct header (used when decoding)
	* \param op the ogg packet that contains the header
	* \return int
	*/
	static int irct_decode_headerin(ogg_packet * op)
	{
		oggpack_buffer ob;
		oggpack_readinit(&ob, op->packet, op->bytes);
		quint8 ret, version, subversion;
		ret = oggpack_read(&ob, 8);
		if(ret != 0)
			return 1;
		char * buf = (char *)KviMemory::allocate(4);
		_tp_readbuffer(&ob, buf, 4);
		if(strncmp(buf, "irct", 4) != 0)
			return 1;
		version = oggpack_read(&ob, 8);
		subversion = oggpack_read(&ob, 8);
		return 0;
	}

	/**
	* \brief Extracts some bunary text from an irct packet (used when decoding)
	* \param textPkt the buffer That will contains the text
	* \param textSize will be set at the size of the text buffer
	* \param op the ogg packet that contains the text
	* \return int
	*/
	static int irct_decode_packetin(char ** textPkt, int * textSize, ogg_packet * op)
	{
		oggpack_buffer ob;
		oggpack_readinit(&ob, op->packet, op->bytes);
		*textSize = op->bytes;
		*textPkt = (char *)KviMemory::allocate(*textSize);
		_tp_readbuffer(&ob, *textPkt, *textSize);
		return 0;
	}
};

#endif //KVI_OGGIRCT_H_
