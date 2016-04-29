#ifndef _KVI_OGGTHEORADECODER_H_
#define _KVI_OGGTHEORADECODER_H_
//=============================================================================
//
//   File : KviOggTheoraDecoder.h
//   Creation date : Sat Nov 21 2009 22:53:21 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Fabio Bas (ctrlaltca at libero dot it)
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
* \file KviOggTheoraDecoder.h
* \author Fabio Bas
* \brief Contains the KviOggTheoraDecoder class
*/

#ifndef COMPILE_DISABLE_OGG_THEORA

#include "kvi_settings.h"
#include "KviOggTheoraGeometry.h"

#include "theora/theoradec.h"

/**
* \def ARGB32_BPP Bytes per pixel in an ARGB32 image
*/
#define ARGB32_BPP 4

class KviDataBuffer;

/**
* \class KviOggTheoraDecoder
* \brief An ogg/theora+irct decoder class; accepts a proper ogg stream, outputs argb32 images and binary text
*/
class KVILIB_API KviOggTheoraDecoder
{
public:
	/**
	* \brief Constructs the KviOggTheoraDecoder object
	* \param videoSignal the output video stream object
	* \param textSignal the output text stream object
	* \return KviOggTheoraDecoder
	*/
	KviOggTheoraDecoder(KviDataBuffer * videoSignal, KviDataBuffer * textSignal);

	/**
	* \brief Destroys the KviTheoraEncoder object
	*/
	virtual ~KviOggTheoraDecoder();

	/**
	* \brief Feeds the decoded with some received data to decode
	* \param stream the databuffer containing the received data
	* \return void
	*/
	void addData(KviDataBuffer * stream);

private:
	KviDataBuffer * m_pVideoSignal; /**< Output video stream pointer */
	KviDataBuffer * m_pTextSignal;  /**< Output text stream pointer */
	KviOggTheoraGeometry geometry;  /**< Stream geometry definition */
	unsigned char * RGBbuffer;      /**< RGB decoded surface pointer */

	// Ogg and codec state for demux/decode
	ogg_sync_state oy;
	ogg_packet op;       /**< One raw packet of encoded data */
	ogg_page og;         /**< One Ogg bitstream page. Vorbis packets are inside */
	ogg_stream_state to; /**< Take physical pages, weld into a logical stream of theora packets */
	ogg_stream_state zo; /**< Take physical pages, weld into a logical stream of irct packets */

	th_info ti;          /**< Theora stream info struct */
	th_comment tc;       /**< Theora stream comments struct */
	th_dec_ctx * td;     /**< Theora stream decoding struct */
	th_setup_info * ts;  /**< Theora stream setup info struct */
	th_pixel_fmt px_fmt; /**< Theora stream pixel format definition */

	int theora_p;  /**< True if the ogg stream contains a theora stream */
	int irct_p;    /**< True if the ogg stream contains an irct stream */
	int stateflag; /**< Internal flag used in stream processing */

	int lu_Y[256];  /**< Surface used in yuv->rgb processing (Y) */
	int lu_R[256];  /**< Surface used in yuv->rgb processing (R) */
	int lu_GU[256]; /**< Surface used in yuv->rgb processing (GU) */
	int lu_GV[256]; /**< Surface used in yuv->rgb processing (GV) */
	int lu_B[256];  /**< Surface used in yuv->rgb processing (B) */

	// Single frame video buffering
	int videobuf_ready;              /**< Single frame video buffering : ready state */
	ogg_int64_t videobuf_granulepos; /**< Single frame video buffering : granule position */
	double videobuf_time;            /**< Single frame video buffering : duration */

	int pp_level_max; /**< Theora postprocessing: max level */
	int pp_level;     /**< Theora postprocessing: current level */
	int pp_inc;       /**< Theora postprocessing: increment */

	bool thda;  /**< Theora decode alloc state */
	bool thtic; /**< Theora setup clear state */
private:
	/**
	* \brief Internal function that queues an ogg page to the codec decoders
	* \return int
	*/
	int queue_page(ogg_page * page);
	/**
	* \brief Internal function that takes a theora yuv buffer, transforms it and appends to the video stream
	* \return int
	*/
	void video_write(void);
};

#endif // COMPILE_DISABLE_OGG_THEORA
#endif // KVI_OGGTHEORADECODER_H_
