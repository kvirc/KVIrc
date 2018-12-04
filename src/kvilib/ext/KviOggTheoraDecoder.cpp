//=============================================================================
//
//   File : KviOggTheoraDecoder.cpp
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
//   Based on the example encoder/player from Xiph.Org Foundation:
//=============================================================================

//
//  function: example encoder application; makes an Ogg Theora/Vorbis
//            file from YUV4MPEG2 and WAV input
//  last mod: $Id: encoder_example.c 16517 2009-08-25 19:48:57Z giles $
//

//
//  function: example SDL player application; plays Ogg Theora files (with
//            optional Vorbis audio second stream)
//  last mod: $Id: player_example.c 16628 2009-10-10 05:50:52Z gmaxwell $
//

#ifndef COMPILE_DISABLE_OGG_THEORA

#include <string.h>
#include <sys/time.h>

#include "KviMemory.h"
#include "KviOggTheoraDecoder.h"
#include "KviOggIrcText.h"
#include "KviDataBuffer.h"

using namespace KviOggIrcText;

#define OC_CLAMP255(_x) ((unsigned char)((((_x) < 0) - 1) & ((_x) | -((_x) > 255))))

KviOggTheoraDecoder::KviOggTheoraDecoder(KviDataBuffer * videoSignal, KviDataBuffer * textSignal)
{
	m_pVideoSignal = videoSignal;
	m_pTextSignal = textSignal;
	theora_p = 0;
	stateflag = 0;
	ts = nullptr;
	thda = false;
	thtic = false;

	irct_p = 0;

	/* single frame video buffering */
	videobuf_ready = 0;
	videobuf_granulepos = -1;
	videobuf_time = 0;

	/* start up Ogg stream synchronization layer */
	ogg_sync_init(&oy);

	/* init supporting Theora structures needed in header parsing */
	th_comment_init(&tc);
	th_info_init(&ti);

	for(int i = 0; i < 256; i++)
	{
		lu_Y[i] = 2441889 * (i - 16) + 1048576;
		lu_R[i] = 3347111 * (i - 128);
		lu_GV[i] = -1704917 * (i - 128);
		lu_GU[i] = -821585 * (i - 128);
		lu_B[i] = 4230442 * (i - 128);
	}
}

KviOggTheoraDecoder::~KviOggTheoraDecoder()
{
	if(theora_p)
	{
		ogg_stream_clear(&to);
		th_decode_free(td);
		th_comment_clear(&tc);
		th_info_clear(&ti);
	}

	if(irct_p)
	{
		ogg_stream_clear(&zo);
		irct_encode_clear();
	}

	ogg_sync_clear(&oy);
}

void KviOggTheoraDecoder::addData(KviDataBuffer * stream)
{
	//qDebug("adddata signal %p stream size %d",m_pVideoSignal, stream->size() );
	if(stream->size() == 0)
		return;
	/* Ogg file open; parse the headers */

	if(!stateflag)
	{

		char * buffer = ogg_sync_buffer(&oy, stream->size());
		memcpy(buffer, stream->data(), stream->size());
		ogg_sync_wrote(&oy, stream->size());
		stream->clear();

		while(ogg_sync_pageout(&oy, &og) > 0)
		{
			ogg_stream_state test;

			/* is this a mandated initial header? If not, stop parsing */
			if(!ogg_page_bos(&og))
			{
				/* don't leak the page; get it into the appropriate stream */
				//qDebug("queue_page && return");
				queue_page(&og);
				stateflag = 1;
				return;
			}

			ogg_stream_init(&test, ogg_page_serialno(&og));
			ogg_stream_pagein(&test, &og);
			ogg_stream_packetout(&test, &op);

			/* identify the codec: try theora */
			if(!theora_p && th_decode_headerin(&ti, &tc, &ts, &op) >= 0)
			{
				qDebug("is theora, ts=%p &ts=%p", ts, &ts);
				/* it is theora */
				memcpy(&to, &test, sizeof(test));
				theora_p = 1;
			}
			else if(!irct_p && irct_decode_headerin(&op) >= 0)
			{
				qDebug("is irct");
				/* it is irct */
				memcpy(&zo, &test, sizeof(test));
				irct_p = 1;
			}
			else
			{
				//qDebug("is other");
				/* whatever it is, we don't care about it */
				ogg_stream_clear(&test);
			}
		}
		/* fall through to non-bos page parsing */
	}

	//qDebug("checkpoint 2, theora_p=%d ",theora_p);
	/* we're expecting more header packets. */
	if((theora_p && theora_p < 3))
	{
		int ret;

		/* look for further theora headers */
		while(theora_p && (theora_p < 3) && (ret = ogg_stream_packetout(&to, &op)))
		{
			if(ret < 0)
			{
				qDebug("Error parsing Theora stream headers; corrupt stream?");
				return;
			}
			if(!th_decode_headerin(&ti, &tc, &ts, &op))
			{
				qDebug("Error parsing Theora stream headers; corrupt stream?");
				return;
			}
			theora_p++;
		}

		/* The header pages/packets will arrive before anything else we
		care about, or the stream is not obeying spec */
		if(ogg_sync_pageout(&oy, &og) > 0)
		{
			qDebug("queue_page");
			queue_page(&og); /* demux into the appropriate stream */
		}
		else
		{
			qDebug("TheoraDecoder: need more data!");
			//need more data
			return;
		}
	}

	//qDebug("checkpoint 3");
	/* and now we have it all.  initialize decoders */
	if(theora_p && !thda)
	{
		thda = true;
		td = th_decode_alloc(&ti, ts);
		qDebug("Ogg logical stream %lx is Theora %dx%d %.02f fps",
		    to.serialno, ti.pic_width, ti.pic_height,
		    (double)ti.fps_numerator / ti.fps_denominator);

		geometry.pic_w = ti.pic_width;
		geometry.pic_h = ti.pic_height;
		geometry.frame_w = ti.frame_width;
		geometry.frame_h = ti.frame_height;
		geometry.pic_x = ti.pic_x;
		geometry.pic_y = ti.pic_y;

		px_fmt = ti.pixel_fmt;
		switch(ti.pixel_fmt)
		{
			case TH_PF_444:
				qDebug(" 4:4:4 video");
				break;
			default:
				qDebug(" video  (UNSUPPORTED Chroma sampling!)");
				return;
				break;
		}
		th_decode_ctl(td, TH_DECCTL_GET_PPLEVEL_MAX, &pp_level_max, sizeof(pp_level_max));
		pp_level = pp_level_max;
		th_decode_ctl(td, TH_DECCTL_SET_PPLEVEL, &pp_level, sizeof(pp_level));
		pp_inc = 0;

		int w = ((ti.pic_x + ti.pic_width + 1) & ~1) - (ti.pic_x & ~1);
		int h = ((ti.pic_y + ti.pic_height + 1) & ~1) - (ti.pic_y & ~1);
		RGBbuffer = (unsigned char *)calloc(sizeof(char), w * h * 4);
	}
	else if(!thtic)
	{
		thtic = true;
		/* tear down the partial theora setup */
		th_info_clear(&ti);
		th_comment_clear(&tc);

		th_setup_free(ts);
	}
	//	 stateflag=0; /* playback has not begun */

	if(irct_p)
	{
		if(ogg_stream_packetout(&zo, &op) > 0)
		{
			char * textPkt = nullptr;
			int textSize = 0;
			if(irct_decode_packetin(&textPkt, &textSize, &op) == 0)
			{
				m_pTextSignal->append((unsigned char *)textPkt, textSize);
			}
		}
	}

	if(theora_p && !videobuf_ready)
	{
		if(stream->size() > 0)
		{
			char * buffer = ogg_sync_buffer(&oy, stream->size());
			memcpy(buffer, stream->data(), stream->size());
			ogg_sync_wrote(&oy, stream->size());
			stream->clear();

			while(ogg_sync_pageout(&oy, &og) > 0)
				queue_page(&og);
		}
		//qDebug("loop4 theora_p=%d videobuf_ready=%d",theora_p, videobuf_ready);
		// theora is one in, one out...

		if(ogg_stream_packetout(&to, &op) > 0)
		{
			if(th_decode_packetin(td, &op, &videobuf_granulepos) == 0)
				videobuf_ready = 1;
		}
		else
		{
			//qDebug("ogg_stream_packetout <=0");
			//need more data :)
			return;
		}
	}

	if(!videobuf_ready)
		return;

	if(stateflag && videobuf_ready)
	{
		video_write();
		videobuf_ready = 0;
	}

	// if our buffers either don't exist or are ready to go,we can begin playback
	if(!theora_p || videobuf_ready)
		stateflag = 1;
}

/* helper: push a page into the appropriate steam */
/* this can be done blindly; a stream won't accept a page
   that doesn't belong to it */
int KviOggTheoraDecoder::queue_page(ogg_page * page)
{
	if(theora_p)
		ogg_stream_pagein(&to, page);
	if(irct_p)
		ogg_stream_pagein(&zo, page);
	return 0;
}

void KviOggTheoraDecoder::video_write(void)
{
	th_ycbcr_buffer yuv;
	int y_offset;
	th_decode_ycbcr_out(td, yuv);

	y_offset = (geometry.pic_x & ~1) + yuv[0].stride * (geometry.pic_y & ~1);

	for(int i = 0; i < geometry.pic_h; i++)
	{
		unsigned char * in_y = (unsigned char *)yuv[0].data + y_offset + yuv[0].stride * i;
		unsigned char * in_u = (unsigned char *)yuv[1].data + y_offset + yuv[1].stride * i;
		unsigned char * in_v = (unsigned char *)yuv[2].data + y_offset + yuv[2].stride * i;
		unsigned char * out = RGBbuffer + (geometry.pic_w * i * ARGB32_BPP);
		for(int j = 0; j < geometry.pic_w; j++)
		{
			int r, g, b;
			int y = lu_Y[in_y[j]];
			b = (y + lu_B[in_u[j]]) >> 21;
			out[j * 4] = OC_CLAMP255(b);
			g = (y + lu_GV[in_v[j]] + lu_GU[in_u[j]]) >> 21;
			out[j * 4 + 1] = OC_CLAMP255(g);
			r = (y + lu_R[in_v[j]]) >> 21;
			out[j * 4 + 2] = OC_CLAMP255(r);
			out[j * 4 + 3] = 255; //alpha
		}
	}

	int size = sizeof(char) * geometry.pic_w * geometry.pic_h * ARGB32_BPP;

	//qDebug("VIDEO WRITE size=%d",size);
	m_pVideoSignal->clear();
	m_pVideoSignal->append(RGBbuffer, size);
}

#endif // COMPILE_DISABLE_OGG_THEORA
