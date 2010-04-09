#ifndef KVI_OGGTHEORA_H_
#define KVI_OGGTHEORA_H_
//=============================================================================
//
//   File : kvi_oggtheora.h
//   Creation date : Sat Nov 21 2009 22:53:21 CEST by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2009 Fabio Bas (ctrlaltca at libero dot it)
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
//=============================================================================

/**
* \file kvi_oggtheora.h
* \author Fabio Bas
* \brief Contains the KviTheoraEncoder and KviTheoraDecoder classes;
*/

#ifndef COMPILE_DISABLE_OGG_THEORA

#include "kvi_settings.h"
#include "kvi_malloc.h"

#include "theora/theoradec.h"
#include "theora/theoraenc.h"

#include <QColor>

/**
* \def OC_MINI simple minimum function
*/
#define OC_MINI(_a,_b)      ((_a)>(_b)?(_b):(_a))
/**
* \def OC_MINI simple maximum function
*/
#define OC_MAXI(_a,_b)      ((_a)<(_b)?(_b):(_a))
/**
* \def OC_MINI simple clamp function
*/
#define OC_CLAMPI(_a,_b,_c) (OC_MAXI(_a,OC_MINI(_b,_c)))

/**
* \def YUV444_BPP bytes per pixel in a YUV444 image
*/
#define YUV444_BPP 3
/**
* \def ARGB32_BPP bytes per pixel in an ARGB32 image
*/
#define ARGB32_BPP 4

class KviDataBuffer;

/**
* \struct KviTheoraGeometry
* \brief Contains all the needed geometry informations of a theora video stream
*/

typedef struct _KviTheoraGeometry
{
	int pic_w;	/**< width of original picture geometry, chosen by the user */
	int pic_h;	/**< height of original picture geometry, chosen by the user */

	int frame_w;	/**< width of the picture frame (calculated geometry) */
	int frame_h;	/**< height of the picture frame (calculated geometry) */

	int pic_x;	/**< x offset of the picture inside the frame (calculated geometry) */
	int pic_y;	/**< y offset of the picture inside the frame (calculated geometry) */
} KviTheoraGeometry;



/**
* \class KviTheoraEncoder
* \brief An ogg/theora+irct encoder class; accepts argb32 images and binary text, outputs an ogg stream
*/
class KVILIB_API KviTheoraEncoder
{
public:
	/**
	* \brief Constructs the KviTheoraEncoder object
	* \param stream the output stream object
	* \param iWidth width of the stream
	* \param iHeight height of the stream
	* \param iFpsN frames per second: numerator
	* \param iFpsD frames per second: denominator
	* \param iParN aspect ratio: numerator
	* \param iParD aspect ratio: denominator
	* \return KviTheoraEncoder
	*/
	KviTheoraEncoder(KviDataBuffer * stream, int iWidth=320, int iHeight=240, int iFpsN=5, int iFpsD=1, int iParN=4, int iParD=3);

	/**
	* \brief Destroys the KviTheoraEncoder object
	*/
	virtual ~KviTheoraEncoder();
private:
	KviTheoraGeometry   geometry;		/**< Stream geometry definition */
	KviDataBuffer     * m_pStream;		/**< Stream pointer */
	quint8            * videoYuv;		/**< Yuv encoded surface pointer */

	ogg_int64_t         text_sofar;		/**< Number of transmitted text frames */

	int                 frame_state;	/**< Internal flag used in video frame processing */
	unsigned char     * yuvframe[3];	/**< Internal struct used in video frame processing */

	size_t              y4m_dst_buf_sz;	/**< The size of each converted frame buffer */
	size_t              y4m_dst_buf_read_sz;/**< The amount to read directly into the converted frame buffer */
	size_t              y4m_aux_buf_sz;	/**< The size of the auxilliary buffer */
	size_t              y4m_aux_buf_read_sz;/**< The amount to read into the auxilliary buffer */

	ogg_stream_state    zo; /**< Take physical pages, weld into a logical stream of irct packets */
	ogg_stream_state    to; /**< Take physical pages, weld into a logical stream of theora packets */
	ogg_page            og; /**< One Ogg bitstream page. Vorbis packets are inside */
	ogg_packet          op; /**< One raw packet of encoded data */

	th_enc_ctx        * td; /**< Theora stream encoding struct */
	th_info             ti; /**< Theora stream info struct */
	th_comment          tc; /**< Theora stream comments struct */

	int                 textflag;	/**< Internal flag used in text frame processing */
	int                 videoflag;	/**< Internal flag used in video frame processing */
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
	void addTextFrame(unsigned char* textPkt, int textSize);
private:
	/**
	* \brief Internal function that processes a video frame to create an ogg page
	* \return int
	*/
	int fetch_and_process_video(quint8 * videoYuv,ogg_page *videopage,ogg_stream_state *to,th_enc_ctx *td,int videoflag);

	/**
	* \brief Internal function that processes a video frame to create an ogg packet
	* \return int
	*/
	int fetch_and_process_video_packet(quint8 * videoYuv,th_enc_ctx *td,ogg_packet *op);

	/**
	* \brief Internal function used to calculate our granule shift
	* \return int
	*/
	static int ilog(unsigned _v);
};

/**
* \class KviTheoraDecoder
* \brief An ogg/theora+irct decoder class; accepts a proper ogg stream, outputs argb32 images and binary text
*/

class KVILIB_API KviTheoraDecoder
{
public:
	/**
	* \brief Constructs the KviTheoraDecoder object
	* \param videoSignal the output video stream object
	* \param textSignal the output text stream object
	* \return KviTheoraDecoder
	*/
	KviTheoraDecoder(KviDataBuffer * videoSignal,KviDataBuffer * textSignal);

	/**
	* \brief Destroys the KviTheoraEncoder object
	*/
	virtual ~KviTheoraDecoder();

	/**
	* \brief Feeds the decoded with some received data to decode
	* \param stream the databuffer containing the received data
	* \return void
	*/
	void addData(KviDataBuffer * stream);
private:
	KviDataBuffer     * m_pVideoSignal;	/**< Output video stream pointer */
	KviDataBuffer     * m_pTextSignal;	/**< Output text stream pointer */
	KviTheoraGeometry   geometry;		/**< Stream geometry definition */
	unsigned char     * RGBbuffer;		/**< RGB decoded surface pointer */

	// Ogg and codec state for demux/decode
	ogg_sync_state      oy;
	ogg_packet          op;		/**< One raw packet of encoded data */
	ogg_page            og;		/**< One Ogg bitstream page. Vorbis packets are inside */
	ogg_stream_state    to;		/**< Take physical pages, weld into a logical stream of theora packets */
	ogg_stream_state    zo;		/**< Take physical pages, weld into a logical stream of irct packets */

	th_info             ti;		/**< Theora stream info struct */
	th_comment          tc;		/**< Theora stream comments struct */
	th_dec_ctx        * td;		/**< Theora stream decoding struct */
	th_setup_info     * ts;		/**< Theora stream setup info struct */
	th_pixel_fmt        px_fmt;	/**< Theora stream pixel format definition */

	int                 theora_p;	/**< True if the ogg stream contains a theora stream */
	int                 irct_p;	/**< True if the ogg stream contains an irct stream */
	int                 stateflag;	/**< Internal flag used in stream processing */

	int                 lu_Y[256];	/**< Surface used in yuv->rgb processing (Y) */
	int                 lu_R[256];	/**< Surface used in yuv->rgb processing (R) */
	int                 lu_GU[256];	/**< Surface used in yuv->rgb processing (GU) */
	int                 lu_GV[256];	/**< Surface used in yuv->rgb processing (GV) */
	int                 lu_B[256];	/**< Surface used in yuv->rgb processing (B) */

	// 
	int                 videobuf_ready;		/**< Single frame video buffering : ready state */
	ogg_int64_t         videobuf_granulepos;	/**< Single frame video buffering : granule position */
	double              videobuf_time;		/**< Single frame video buffering : duration */

	int                 pp_level_max;	/**< Theora postprocessing: max level */
	int                 pp_level;		/**< Theora postprocessing: current level */
	int                 pp_inc;		/**< Theora postprocessing: increment */

	bool                thda;		/**< Theora decode alloc state */
	bool                thtic;		/**< Theora setup clear state */
private:
	/**
	* \brief Internal function that queues an ogg page to the codec decoders
	* \return int
	*/
	int queue_page(ogg_page *page);
	/**
	* \brief Internal function that takes a theora yuv buffer, transforms it and appends to the video stream
	* \return int
	*/
	void video_write(void);
};

#endif // COMPILE_DISABLE_OGG_THEORA
#endif /* KVI_OGGTHEORA_H_ */
