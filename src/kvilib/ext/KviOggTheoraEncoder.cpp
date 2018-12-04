//=============================================================================
//
//   File : KviOggTheoraEncoder.cpp
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

#ifndef COMPILE_DISABLE_OGG_THEORA

#include "KviMemory.h"
#include "KviOggTheoraEncoder.h"
#include "KviOggIrcText.h"
#include "KviDataBuffer.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <QColor>

using namespace KviOggIrcText;

static void rgb32toyuv444(QRgb * rgbPt, unsigned char * yuvPt, int w, int h)
{
	for(int i = 0; i < h; i++)
	{
		for(int j = 0; j < w; j++)
		{
			yuvPt[i * w + j] = (unsigned char)((66 * qRed(*rgbPt) + 129 * qGreen(*rgbPt) + 25 * qBlue(*rgbPt) + 128) >> 8) + 16;            // y
			yuvPt[(i + h) * w + j] = (unsigned char)((-38 * qRed(*rgbPt) - 74 * qGreen(*rgbPt) + 112 * qBlue(*rgbPt) + 128) >> 8) + 128;    // u
			yuvPt[(i + 2 * h) * w + j] = (unsigned char)((112 * qRed(*rgbPt) - 94 * qGreen(*rgbPt) - 18 * qBlue(*rgbPt) + 128) >> 8) + 128; // v
			//this moves the pointer forward 4 bytes (ARGB)
			rgbPt++;
		}
	}
	return;
}

KviOggTheoraEncoder::KviOggTheoraEncoder(KviDataBuffer * stream, int iWidth, int iHeight, int iFpsN, int iFpsD, int iParN, int iParD)
{
	//used to check functions results
	int ret;
	videoflag = 0;
	videoYuv = 0;
	frame_state = -1;

	//text
	text_sofar = 0;

	//user settings
	m_pStream = stream;
	geometry.pic_w = iWidth;
	geometry.pic_h = iHeight;
	//frame per second
	int video_fps_n = iFpsN;
	int video_fps_d = iFpsD;
	//aspect ratio
	int video_par_n = iParN;
	int video_par_d = iParD;

	// preset settings
	int video_q = 48;
	ogg_uint32_t keyframe_frequency = 64;

	y4m_dst_buf_read_sz = geometry.pic_w * geometry.pic_h * 3;
	y4m_aux_buf_sz = y4m_aux_buf_read_sz = 0;

	// buffer allocations
	y4m_dst_buf_read_sz = geometry.pic_w * geometry.pic_h * 3;
	y4m_aux_buf_sz = y4m_aux_buf_read_sz = 0;
	// The size of the final frame buffers is always computed from the destination chroma decimation type.
	y4m_dst_buf_sz = geometry.pic_w * geometry.pic_h * 3;

	// Set up Ogg output stream
	srand(time(nullptr));
	ogg_stream_init(&to, rand());

	ogg_stream_init(&zo, rand());
	ret = irct_encode_init();
	if(ret)
	{
		qDebug("error initializing irctext");
		return;
	}

	// Set up Theora encoder
	// Theora has a divisible-by-sixteen restriction for the encoded frame size
	// scale the picture size up to the nearest /16 and calculate offsets
	geometry.frame_w = (geometry.pic_w + 15) & ~0xF;
	geometry.frame_h = (geometry.pic_h + 15) & ~0xF;
	// Force the offsets to be even so that chroma samples line up like we expect.
	geometry.pic_x = (geometry.frame_w - geometry.pic_w) >> 1 & ~1;
	geometry.pic_y = (geometry.frame_h - geometry.pic_h) >> 1 & ~1;

	// Fill in a th_info structure with details on the format of the video you wish to encode.
	th_info_init(&ti);
	ti.frame_width = geometry.frame_w;
	ti.frame_height = geometry.frame_h;
	ti.pic_width = geometry.pic_w;
	ti.pic_height = geometry.pic_h;
	ti.pic_x = geometry.pic_x;
	ti.pic_y = geometry.pic_y;
	ti.fps_numerator = video_fps_n;
	ti.fps_denominator = video_fps_d;
	ti.aspect_numerator = video_par_n;
	ti.aspect_denominator = video_par_d;
	ti.colorspace = TH_CS_UNSPECIFIED;
	//use vbr, not cbr
	ti.target_bitrate = 0;
	ti.quality = video_q;
	ti.keyframe_granule_shift = ilog(keyframe_frequency - 1);
	ti.pixel_fmt = TH_PF_444;
	// Allocate a th_enc_ctx handle with th_encode_alloc().
	td = th_encode_alloc(&ti);
	if(td == 0)
	{
		qDebug("Could not th_encode_alloc().");
		return;
	}

	th_info_clear(&ti);
	// Perform any additional encoder configuration required with th_encode_ctl().
	// setting just the granule shift only allows power-of-two keyframe spacing. Set the actual requested spacing.
	ret = th_encode_ctl(td, TH_ENCCTL_SET_KEYFRAME_FREQUENCY_FORCE, &keyframe_frequency, sizeof(keyframe_frequency - 1));
	if(ret < 0)
	{
		qDebug("Could not set keyframe interval to %d.", (int)keyframe_frequency);
	}
	// write the bitstream header packets with proper page interleave
	th_comment_init(&tc);

	// Repeatedly call th_encode_flushheader() to retrieve all the header packets.
	// first packet will get its own page automatically
	if(th_encode_flushheader(td, &tc, &op) <= 0)
	{
		qDebug("Internal Theora library error.");
		return;
	}

	ogg_stream_packetin(&to, &op);
	if(ogg_stream_pageout(&to, &og) != 1)
	{
		qDebug("Internal Ogg library error.");
		return;
	}

	m_pStream->append(og.header, og.header_len);
	m_pStream->append(og.body, og.body_len);

	// create the remaining theora headers
	for(;;)
	{
		ret = th_encode_flushheader(td, &tc, &op);
		if(ret < 0)
		{
			qDebug("Internal Theora library error.");
			return;
		}
		else if(!ret)
			break;
		ogg_stream_packetin(&to, &op);
	}

	//irct
	ogg_packet header;
	irct_encode_headerout(&header);
	ogg_stream_packetin(&zo, &header); // automatically placed in its own page
	if(ogg_stream_pageout(&zo, &og) != 1)
	{
		qDebug("Internal Ogg library error (irct).");
		return;
	}
	m_pStream->append(og.header, og.header_len);
	m_pStream->append(og.body, og.body_len);

	// Flush the rest of our headers. This ensures the actual data in each stream
	// will start on a new page, as per spec.
	for(;;)
	{
		int result = ogg_stream_flush(&to, &og);
		if(result < 0)
		{
			// can't get here
			qDebug("Internal Ogg library error.");
			return;
		}
		if(result == 0)
			break;
		m_pStream->append(og.header, og.header_len);
		m_pStream->append(og.body, og.body_len);
	}
}

KviOggTheoraEncoder::~KviOggTheoraEncoder()
{
	// Call th_encode_free() to release all encoder memory.
	th_encode_free(td);

	// clear out state
	ogg_stream_clear(&to);
	th_comment_clear(&tc);

	irct_encode_clear();
}

void KviOggTheoraEncoder::addVideoFrame(QRgb * rgb32, int)
{
	/*
	 * For each uncompressed frame:
	 *	o Submit the uncompressed frame via th_encode_ycbcr_in()
	 *	o Repeatedly call th_encode_packetout() to retrieve any video data packets that are ready.
	 */

	if(!videoYuv)
		videoYuv = new quint8[geometry.pic_w * geometry.pic_h * YUV444_BPP];

	rgb32toyuv444(rgb32, videoYuv, geometry.pic_w, geometry.pic_h);
	//qDebug("addFrame p%p size%d yuv%p",rgb32,videoSize,videoYuv);

	ogg_page videopage;

	// is there a video page flushed?  If not, fetch one if possible
	videoflag = fetch_and_process_video(videoYuv, &videopage, &to, td, videoflag);

	// no pages?
	if(!videoflag)
		return;

	th_granule_time(td, ogg_page_granulepos(&videopage));

	// flush a video page
	m_pStream->append(videopage.header, videopage.header_len);
	m_pStream->append(videopage.body, videopage.body_len);
	videoflag = 0;
}

void KviOggTheoraEncoder::addTextFrame(unsigned char * textPkt, int textSize)
{
	ogg_page textpage;
	ogg_packet op;

	text_sofar++;

	// is there an audio page flushed?  If not, fetch one if possible
	textflag = irct_encode_packetout((char *)textPkt, textSize, text_sofar, &op);
	ogg_stream_packetin(&zo, &op);
	ogg_stream_pageout(&zo, &textpage);

	// no pages?  Must be end of stream.

	m_pStream->append(textpage.header, textpage.header_len);
	m_pStream->append(textpage.body, textpage.body_len);
	textflag = 0;
}

int KviOggTheoraEncoder::fetch_and_process_video_packet(quint8 * videoYuv, th_enc_ctx * td, ogg_packet * op)
{
	th_ycbcr_buffer ycbcr;

	if(frame_state == -1)
	{
		/* initialize the double frame buffer */
		yuvframe[0] = (unsigned char *)malloc(y4m_dst_buf_sz);
		yuvframe[1] = (unsigned char *)malloc(y4m_dst_buf_sz);
		yuvframe[2] = (unsigned char *)malloc(y4m_aux_buf_sz);
		frame_state = 0;
	}
	/* read and process more video */

	/* have two frame buffers full (if possible) before
	proceeding.  after first pass and until eos, one will
	always be full when we get here */
	for(; frame_state < 2;)
	{
		/*Read the frame data that needs no conversion.*/
		memcpy(yuvframe[frame_state], videoYuv, y4m_dst_buf_read_sz);
		/*Read the frame data that does need conversion.*/
		memcpy(yuvframe[2], videoYuv, y4m_aux_buf_read_sz);
		frame_state++;
	}
	/* check to see if there are dupes to flush */
	if(th_encode_packetout(td, frame_state < 1, op) > 0)
		return 1;
	if(frame_state < 1)
	{
		/* can't get here unless YUV4MPEG stream has no video */
		qDebug("Video input contains no frames.");
		return 0;
	}
	/* Theora is a one-frame-in,one-frame-out system; submit a frame
	for compression and pull out the packet */

	/*We submit the buffer to the library as if it were padded, but we do not
	actually allocate space for the padding.
	This is okay, because with the 1.0 API the library will never read data from the padded
	region.*/
	ycbcr[0].width = geometry.frame_w;
	ycbcr[0].height = geometry.frame_h;
	ycbcr[0].stride = geometry.pic_w;
	ycbcr[0].data = yuvframe[0] - geometry.pic_x - geometry.pic_y * geometry.pic_w;

	ycbcr[1].width = geometry.frame_w;
	ycbcr[1].height = geometry.frame_h;
	ycbcr[1].stride = geometry.pic_w;
	ycbcr[1].data = yuvframe[0] + (geometry.pic_w * geometry.pic_h) - geometry.pic_x - geometry.pic_y * geometry.frame_w;

	ycbcr[2].width = geometry.frame_w;
	ycbcr[2].height = geometry.frame_h;
	ycbcr[2].stride = geometry.pic_w;
	ycbcr[2].data = ycbcr[1].data + (geometry.frame_w * geometry.frame_h);

	th_encode_ycbcr_in(td, ycbcr);
	{
		unsigned char * temp = yuvframe[0];
		yuvframe[0] = yuvframe[1];
		yuvframe[1] = temp;
		frame_state--;
	}
	/* if there was only one frame, it's the last in the stream */
	return th_encode_packetout(td, frame_state < 1, op);
}

int KviOggTheoraEncoder::fetch_and_process_video(quint8 * videoYuv, ogg_page * videopage,
    ogg_stream_state * to, th_enc_ctx * td, int videoflag)
{
	ogg_packet op;
	int ret;
	/* is there a video page flushed?  If not, work until there is. */
	while(!videoflag)
	{
		//qDebug("fetch_and_process_video loop");
		if(ogg_stream_pageout(to, videopage) > 0)
			return 1;
		if(ogg_stream_eos(to))
			return 0;
		ret = fetch_and_process_video_packet(videoYuv, td, &op);
		if(ret <= 0)
			return 0;
		ogg_stream_packetin(to, &op);
	}
	return videoflag;
}

int KviOggTheoraEncoder::ilog(unsigned _v)
{
	int ret;
	for(ret = 0; _v; ret++)
		_v >>= 1;
	return ret;
}

#endif // COMPILE_DISABLE_OGG_THEORA
