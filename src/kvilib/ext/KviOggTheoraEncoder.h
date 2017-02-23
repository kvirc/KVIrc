#ifndef _KVIOGGTHEORAENCODER_H_
#define _KVIOGGTHEORAENCODER_H_
//=============================================================================
//
//   File : KviOggTheoraEncoder.h
//   Creation date : Wed Dec 29 2010 15:51:21 CEST by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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
* \file KviOggTheoraEncoder.h
* \author Elvio Basello
* \brief Contains the KviOggTheoraEncoder class
*
* This file was originally part of KviOggTheoraDecoder.h
*/

#ifndef COMPILE_DISABLE_OGG_THEORA

#include "kvi_settings.h"
#include "KviOggTheoraGeometry.h"

#include "theora/theoraenc.h"

#include <QRgb>

class KviDataBuffer;

/**
* \def YUV444_BPP Bytes per pixel in a YUV444 image
*/
#define YUV444_BPP 3

/**
* \class KviOggTheoraEncoder
* \brief An ogg/theora+irct encoder class; accepts argb32 images and binary text, outputs an ogg stream
*/
class KVILIB_API KviOggTheoraEncoder
{
public:
	/**
	* \brief Constructs the KviOggTheoraEncoder object
	* \param stream the output stream object
	* \param iWidth width of the stream
	* \param iHeight height of the stream
	* \param iFpsN frames per second: numerator
	* \param iFpsD frames per second: denominator
	* \param iParN aspect ratio: numerator
	* \param iParD aspect ratio: denominator
	* \return KviOggTheoraEncoder
	*/
	KviOggTheoraEncoder(KviDataBuffer * stream, int iWidth = 320, int iHeight = 240, int iFpsN = 5, int iFpsD = 1, int iParN = 4, int iParD = 3);

	/**
	* \brief Destroys the KviOggTheoraEncoder object
	*/
	virtual ~KviOggTheoraEncoder();

private:
	KviOggTheoraGeometry geometry; /**< Stream geometry definition */
	KviDataBuffer * m_pStream;     /**< Stream pointer */
	quint8 * videoYuv;             /**< Yuv encoded surface pointer */

	ogg_int64_t text_sofar; /**< Number of transmitted text frames */

	int frame_state;             /**< Internal flag used in video frame processing */
	unsigned char * yuvframe[3]; /**< Internal struct used in video frame processing */

	size_t y4m_dst_buf_sz;      /**< The size of each converted frame buffer */
	size_t y4m_dst_buf_read_sz; /**< The amount to read directly into the converted frame buffer */
	size_t y4m_aux_buf_sz;      /**< The size of the auxiliary buffer */
	size_t y4m_aux_buf_read_sz; /**< The amount to read into the auxiliary buffer */

	ogg_stream_state zo; /**< Take physical pages, weld into a logical stream of irct packets */
	ogg_stream_state to; /**< Take physical pages, weld into a logical stream of theora packets */
	ogg_page og;         /**< One Ogg bitstream page. Vorbis packets are inside */
	ogg_packet op;       /**< One raw packet of encoded data */

	th_enc_ctx * td; /**< Theora stream encoding struct */
	th_info ti;      /**< Theora stream info struct */
	th_comment tc;   /**< Theora stream comments struct */

	int textflag;  /**< Internal flag used in text frame processing */
	int videoflag; /**< Internal flag used in video frame processing */
public:
	/**
	* \brief Appends a video frame to the stream
	* \param rgb32 video frame as a matrix of rgb32 pixels
	* \param videoSize size of the video frame
	* \return void
	*/
	void addVideoFrame(QRgb * rgb32, int videoSize);

	/**
	* \brief Appends a text frame to the stream
	* \param textPkt buffer containing the text
	* \param textSize size of the text buffer
	* \return void
	*/
	void addTextFrame(unsigned char * textPkt, int textSize);

private:
	/**
	* \brief Internal function that processes a video frame to create an ogg page
	* \return int
	*/
	int fetch_and_process_video(quint8 * videoYuv, ogg_page * videopage, ogg_stream_state * to, th_enc_ctx * td, int videoflag);

	/**
	* \brief Internal function that processes a video frame to create an ogg packet
	* \return int
	*/
	int fetch_and_process_video_packet(quint8 * videoYuv, th_enc_ctx * td, ogg_packet * op);

	/**
	* \brief Internal function used to calculate our granule shift
	* \return int
	*/
	static int ilog(unsigned _v);
};

#endif // COMPILE_DISABLE_OGG_THEORA
#endif // _KVIOGGTHEORAENCODER_H_
