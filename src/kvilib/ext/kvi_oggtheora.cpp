//=============================================================================
//
//   File : kvi_oggtheora.cpp
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
//   Based on the example encoder/player from Xiph.Org Foundation:
//=============================================================================
//
//  function: example encoder application; makes an Ogg Theora/Vorbis
//            file from YUV4MPEG2 and WAV input
//  last mod: $Id: encoder_example.c 16517 2009-08-25 19:48:57Z giles $
//
//=============================================================================
//
//  function: example SDL player application; plays Ogg Theora files (with
//            optional Vorbis audio second stream)
//  last mod: $Id: player_example.c 16628 2009-10-10 05:50:52Z gmaxwell $
//
//=============================================================================

#ifndef COMPILE_DISABLE_OGG_THEORA

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "kvi_oggtheora.h"
#include "kvi_oggirct.h"

#include "kvi_databuffer.h"

using namespace KviOggIrcText;

static void rgb32toyuv444(QRgb * rgbPt, unsigned char *yuvPt, int w, int h)
{
	for(int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			yuvPt[i * w + j] = (unsigned char) (( 66 * qRed(*rgbPt) + 129 * qGreen(*rgbPt) +  25 * qBlue(*rgbPt) + 128) >> 8) + 16;   // y
			yuvPt[(i + h) * w + j] = (unsigned char) ((-38 * qRed(*rgbPt) -  74 * qGreen(*rgbPt) + 112 * qBlue(*rgbPt) + 128) >> 8) + 128;  // u
			yuvPt[(i + 2 * h)  * w + j] = (unsigned char) ((112 * qRed(*rgbPt) -  94 * qGreen(*rgbPt) -  18 * qBlue(*rgbPt) + 128) >> 8) + 128;  // v
			//this moves the pointer forward 4 bytes (ARGB)
			rgbPt++;
		}
	}
	return;
}

KviTheoraEncoder::KviTheoraEncoder(KviDataBuffer * stream, int iWidth, int iHeight, int iFpsN, int iFpsD, int iParN, int iParD)
{
	//used to check functions results
	int ret;
	videoflag=0;
	videoYuv=0;
	frame_state=-1;

	//text
	text_sofar=0;

	//user settings
	m_pStream=stream;
	geometry.pic_w=iWidth;
	geometry.pic_h=iHeight;
	//frame per second
	int video_fps_n=iFpsN;
	int video_fps_d=iFpsD;
	//aspect ratio
	int video_par_n=iParN;
	int video_par_d=iParD;


	// preset settings
	int video_q=48;
	ogg_uint32_t keyframe_frequency=64;

        y4m_dst_buf_read_sz = geometry.pic_w * geometry.pic_h * 3;
        y4m_aux_buf_sz = y4m_aux_buf_read_sz=0;

	// buffer allocations
	y4m_dst_buf_read_sz = geometry.pic_w * geometry.pic_h * 3;
	y4m_aux_buf_sz = y4m_aux_buf_read_sz=0;
	// The size of the final frame buffers is always computed from the destination chroma decimation type.
	y4m_dst_buf_sz = geometry.pic_w * geometry.pic_h * 3;

	// Set up Ogg output stream
	srand(time(NULL));
	ogg_stream_init(&to,rand());

	ogg_stream_init(&zo,rand());
	ret = irct_encode_init();
	if(ret)
	{
		qDebug("error initializing irctext");
		return;
	}


	// Set up Theora encoder
	// Theora has a divisible-by-sixteen restriction for the encoded frame size
	// scale the picture size up to the nearest /16 and calculate offsets
	geometry.frame_w = (geometry.pic_w + 15) &~ 0xF;
	geometry.frame_h = (geometry.pic_h + 15) &~ 0xF;
	// Force the offsets to be even so that chroma samples line up like we expect.
	geometry.pic_x = (geometry.frame_w - geometry.pic_w) >> 1 &~ 1;
	geometry.pic_y = (geometry.frame_h - geometry.pic_h) >> 1 &~ 1;

	// Fill in a th_info structure with details on the format of the video you wish to encode.
	th_info_init(&ti);
	ti.frame_width=geometry.frame_w;
	ti.frame_height=geometry.frame_h;
	ti.pic_width=geometry.pic_w;
	ti.pic_height=geometry.pic_h;
	ti.pic_x=geometry.pic_x;
	ti.pic_y=geometry.pic_y;
	ti.fps_numerator=video_fps_n;
	ti.fps_denominator=video_fps_d;
	ti.aspect_numerator=video_par_n;
	ti.aspect_denominator=video_par_d;
	ti.colorspace=TH_CS_UNSPECIFIED;
	//use vbr, not cbr
	ti.target_bitrate=0;
	ti.quality=video_q;
	ti.keyframe_granule_shift=ilog(keyframe_frequency-1);
	ti.pixel_fmt=TH_PF_444;
	// Allocate a th_enc_ctx handle with th_encode_alloc().
	td=th_encode_alloc(&ti);
	if(td==0){
		qDebug("Could not th_encode_alloc().");
		return;
	}

	th_info_clear(&ti);
	// Perform any additional encoder configuration required with th_encode_ctl().
	// setting just the granule shift only allows power-of-two keyframe spacing. Set the actual requested spacing.
	ret=th_encode_ctl(td,TH_ENCCTL_SET_KEYFRAME_FREQUENCY_FORCE,&keyframe_frequency,sizeof(keyframe_frequency-1));
	if(ret<0){
		qDebug("Could not set keyframe interval to %d.",(int)keyframe_frequency);
	}
	// write the bitstream header packets with proper page interleave
	th_comment_init(&tc);
	
	// Repeatedly call th_encode_flushheader() to retrieve all the header packets.
	// first packet will get its own page automatically
	if(th_encode_flushheader(td,&tc,&op)<=0){
		qDebug("Internal Theora library error.");
		return;
	}

	ogg_stream_packetin(&to,&op);
	if(ogg_stream_pageout(&to,&og)!=1){
		qDebug("Internal Ogg library error.");
		return;
	}

	m_pStream->append(og.header,og.header_len);
	m_pStream->append(og.body,og.body_len);

	// create the remaining theora headers
	for(;;){
		ret=th_encode_flushheader(td,&tc,&op);
		if(ret<0){
			qDebug("Internal Theora library error.");
			return;
		}
		else if(!ret)break;
		ogg_stream_packetin(&to,&op);
	}

	//irct
	ogg_packet header;
	irct_encode_headerout(&header);
	ogg_stream_packetin(&zo,&header); // automatically placed in its own page
	if(ogg_stream_pageout(&zo,&og)!=1){
		qDebug("Internal Ogg library error (irct).");
		return;
	}
	m_pStream->append(og.header,og.header_len);
	m_pStream->append(og.body,og.body_len);

	// Flush the rest of our headers. This ensures the actual data in each stream
	// will start on a new page, as per spec.
	for(;;)
	{
		int result = ogg_stream_flush(&to,&og);
		if(result<0){
			// can't get here
			qDebug("Internal Ogg library error.");
			return;
		}
		if(result==0)break;
		m_pStream->append(og.header,og.header_len);
		m_pStream->append(og.body,og.body_len);
	}
}

KviTheoraEncoder::~KviTheoraEncoder()
{
	// Call th_encode_free() to release all encoder memory.
	th_encode_free(td);

	// clear out state

	ogg_stream_clear(&to);
	th_comment_clear(&tc);

	irct_encode_clear();
}

void KviTheoraEncoder::addVideoFrame(QRgb * rgb32, int)
{
	/*
	 * For each uncompressed frame:
	 *	o Submit the uncompressed frame via th_encode_ycbcr_in()
	 *	o Repeatedly call th_encode_packetout() to retrieve any video data packets that are ready.
	 */

	if(!videoYuv)
		videoYuv = new quint8[geometry.pic_w * geometry.pic_h * YUV444_BPP];

	rgb32toyuv444(rgb32, videoYuv, geometry.pic_w, geometry.pic_h);
//	qDebug("addFrame p%p size%d yuv%p",rgb32,videoSize,videoYuv);
	
	ogg_page videopage;

	// is there a video page flushed?  If not, fetch one if possible
	videoflag=fetch_and_process_video(videoYuv,&videopage,&to,td,videoflag);

	// no pages?
	if(!videoflag) return;

	th_granule_time(td,ogg_page_granulepos(&videopage));

	// flush a video page
	m_pStream->append(videopage.header,videopage.header_len);
	m_pStream->append(videopage.body,videopage.body_len);
	videoflag=0;
}

void KviTheoraEncoder::addTextFrame(unsigned char* textPkt, int textSize)
{
	ogg_page textpage;
	ogg_packet op;

	text_sofar++;

	// is there an audio page flushed?  If not, fetch one if possible
	textflag=irct_encode_packetout((char *) textPkt, textSize, text_sofar,&op);
	ogg_stream_packetin(&zo, &op);
	ogg_stream_pageout(&zo, &textpage);

	// no pages?  Must be end of stream.
// 	if(!textflag) return;

	m_pStream->append(textpage.header,textpage.header_len);
	m_pStream->append(textpage.body,textpage.body_len);
	textflag=0;
}

int KviTheoraEncoder::fetch_and_process_video_packet(quint8 * videoYuv,th_enc_ctx *td,ogg_packet *op)
{
	th_ycbcr_buffer ycbcr;

	if(frame_state==-1)
	{
		/* initialize the double frame buffer */
		yuvframe[0]=(unsigned char *)malloc(y4m_dst_buf_sz);
		yuvframe[1]=(unsigned char *)malloc(y4m_dst_buf_sz);
		yuvframe[2]=(unsigned char *)malloc(y4m_aux_buf_sz);
		frame_state=0;
	}
	/* read and process more video */

	/* have two frame buffers full (if possible) before
	proceeding.  after first pass and until eos, one will
	always be full when we get here */
	for(;frame_state<2;)
	{
		/*Read the frame data that needs no conversion.*/
		memcpy(yuvframe[frame_state],videoYuv,y4m_dst_buf_read_sz);
		/*Read the frame data that does need conversion.*/
		memcpy(yuvframe[2],videoYuv,y4m_aux_buf_read_sz);
		frame_state++;
	}
	/* check to see if there are dupes to flush */
	if(th_encode_packetout(td,frame_state<1,op)>0)return 1;
	if(frame_state<1)
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
	ycbcr[0].width=geometry.frame_w;
	ycbcr[0].height=geometry.frame_h;
	ycbcr[0].stride=geometry.pic_w;
	ycbcr[0].data=yuvframe[0] - geometry.pic_x  -geometry.pic_y * geometry.pic_w;

	ycbcr[1].width=geometry.frame_w;
	ycbcr[1].height=geometry.frame_h;
	ycbcr[1].stride=geometry.pic_w;
	ycbcr[1].data=yuvframe[0] + (geometry.pic_w * geometry.pic_h) - geometry.pic_x - geometry.pic_y * geometry.frame_w;

	ycbcr[2].width=geometry.frame_w;
	ycbcr[2].height=geometry.frame_h;
	ycbcr[2].stride=geometry.pic_w;
	ycbcr[2].data=ycbcr[1].data+(geometry.frame_w * geometry.frame_h);

	th_encode_ycbcr_in(td,ycbcr);
	{
		unsigned char *temp=yuvframe[0];
		yuvframe[0]=yuvframe[1];
		yuvframe[1]=temp;
		frame_state--;
	}
	/* if there was only one frame, it's the last in the stream */
	return th_encode_packetout(td,frame_state<1,op);
}

int KviTheoraEncoder::fetch_and_process_video(quint8 * videoYuv,ogg_page *videopage,
ogg_stream_state *to,th_enc_ctx *td,int videoflag)
{
	ogg_packet op;
	int ret;
	/* is there a video page flushed?  If not, work until there is. */
	while(!videoflag)
	{
		//qDebug("fetch_and_process_video loop");
		if(ogg_stream_pageout(to,videopage)>0) return 1;
		if(ogg_stream_eos(to)) return 0;
		ret=fetch_and_process_video_packet(videoYuv,td,&op);
		if(ret<=0)return 0;
		ogg_stream_packetin(to,&op);
	}
	return videoflag;
}

int KviTheoraEncoder::ilog(unsigned _v)
{
	int ret;
	for(ret=0;_v;ret++)_v>>=1;
	return ret;
}


//------------------------------------------------------------------------------
#define OC_CLAMP255(_x)     ((unsigned char)((((_x)<0)-1)&((_x)|-((_x)>255))))

KviTheoraDecoder::KviTheoraDecoder(KviDataBuffer * videoSignal,KviDataBuffer * textSignal)
{
	m_pVideoSignal=videoSignal;
	m_pTextSignal=textSignal;
	theora_p=0;
	stateflag=0;
	ts=NULL;
	thda=false;
	thtic=false;

	irct_p=0;

	/* single frame video buffering */
	videobuf_ready=0;
	videobuf_granulepos=-1;
	videobuf_time=0;

	/* start up Ogg stream synchronization layer */
	ogg_sync_init(&oy);

	/* init supporting Theora structures needed in header parsing */
	th_comment_init(&tc);
	th_info_init(&ti);

	for(int i=0;i<256;i++)
	{
		lu_Y[i]  =  2441889*(i- 16) + 1048576;
		lu_R[i]  =  3347111*(i-128);
		lu_GV[i] = -1704917*(i-128);
		lu_GU[i] =  -821585*(i-128);
		lu_B[i]  =  4230442*(i-128);
	}
}

KviTheoraDecoder::~KviTheoraDecoder()
{
	if(theora_p){
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

void KviTheoraDecoder::addData(KviDataBuffer * stream)
{
// 	qDebug("adddata signal %p stream size %d",m_pVideoSignal, stream->size() );
	if(stream->size()==0)return;
	/* Ogg file open; parse the headers */
		
	if(!stateflag)
	{

		char *buffer=ogg_sync_buffer(&oy,stream->size());
		memcpy(buffer,stream->data(),stream->size());
		ogg_sync_wrote(&oy,stream->size());
		stream->clear();

		while(ogg_sync_pageout(&oy,&og)>0)
		{
			ogg_stream_state test;

			/* is this a mandated initial header? If not, stop parsing */
			if(!ogg_page_bos(&og))
			{
				/* don't leak the page; get it into the appropriate stream */
// 				qDebug("queue_page && return");
				queue_page(&og);
				stateflag=1;
				return;
			}

			ogg_stream_init(&test,ogg_page_serialno(&og));
			ogg_stream_pagein(&test,&og);
			ogg_stream_packetout(&test,&op);


			/* identify the codec: try theora */
			if(!theora_p && th_decode_headerin(&ti,&tc,&ts,&op)>=0){
				qDebug("is theora, ts=%p &ts=%p",ts, &ts);
				/* it is theora */
				memcpy(&to,&test,sizeof(test));
				theora_p=1;
			}else if(!irct_p && irct_decode_headerin(&op)>=0){
				qDebug("is irct");
				/* it is irct */
				memcpy(&zo,&test,sizeof(test));
				irct_p=1;
			}else{
// 				qDebug("is other");
				/* whatever it is, we don't care about it */
				ogg_stream_clear(&test);
			}
		}
		/* fall through to non-bos page parsing */
	}

// 	qDebug("checkpoint 2, theora_p=%d ",theora_p);
	/* we're expecting more header packets. */
	if((theora_p && theora_p<3))
	{
		int ret;

		/* look for further theora headers */
		while(theora_p && (theora_p<3) && (ret=ogg_stream_packetout(&to,&op)))
		{
			if(ret<0){
				qDebug("Error parsing Theora stream headers; corrupt stream?");
				return;
			}
			if(!th_decode_headerin(&ti,&tc,&ts,&op)){
				qDebug("Error parsing Theora stream headers; corrupt stream?");
				return;
			}
			theora_p++;
		}

		/* The header pages/packets will arrive before anything else we
		care about, or the stream is not obeying spec */
		if(ogg_sync_pageout(&oy,&og)>0)
		{
			qDebug("queue_page");
			queue_page(&og); /* demux into the appropriate stream */
		} else {
			qDebug("TheoraDecoder: need more data!");
			//need more data
			return;
		}
	}

// 	qDebug("checkpoint 3");
	/* and now we have it all.  initialize decoders */
	if(theora_p && !thda)
	{
		thda=true;
		td=th_decode_alloc(&ti,ts);
		qDebug("Ogg logical stream %lx is Theora %dx%d %.02f fps",
			to.serialno,ti.pic_width,ti.pic_height,
			(double)ti.fps_numerator/ti.fps_denominator);

		geometry.pic_w = ti.pic_width;
		geometry.pic_h = ti.pic_height;
		geometry.frame_w = ti.frame_width;
		geometry.frame_h = ti.frame_height;
		geometry.pic_x = ti.pic_x;
		geometry.pic_y = ti.pic_y;
		
		px_fmt=ti.pixel_fmt;
		switch(ti.pixel_fmt)
		{
			case TH_PF_444: qDebug(" 4:4:4 video"); break;
			default:
			qDebug(" video  (UNSUPPORTED Chroma sampling!)");
			return;
			break;
		}
		th_decode_ctl(td,TH_DECCTL_GET_PPLEVEL_MAX,&pp_level_max,sizeof(pp_level_max));
		pp_level=pp_level_max;
		th_decode_ctl(td,TH_DECCTL_SET_PPLEVEL,&pp_level,sizeof(pp_level));
		pp_inc=0;

		int w = ((ti.pic_x + ti.pic_width + 1) &~ 1) - (ti.pic_x &~ 1);
		int h = ((ti.pic_y + ti.pic_height + 1) &~ 1) - (ti.pic_y &~ 1);
		RGBbuffer = (unsigned char *) calloc(sizeof(char), w * h * 4);
	} else if(!thtic)
	{
		thtic=true;
		/* tear down the partial theora setup */
		th_info_clear(&ti);
		th_comment_clear(&tc);

		th_setup_free(ts);
	}
//	 stateflag=0; /* playback has not begun */

	if(irct_p)
	{
		if(ogg_stream_packetout(&zo,&op)>0)
		{
			char * textPkt=0;
			int textSize=0;
			if(irct_decode_packetin(&textPkt, &textSize, &op)==0)
			{
				m_pTextSignal->append((unsigned char *)textPkt, textSize);
			}
		}
	}

	if(theora_p && !videobuf_ready)
	{
		if(stream->size()>0)
		{
			char *buffer=ogg_sync_buffer(&oy,stream->size());
			memcpy(buffer,stream->data(),stream->size());
			ogg_sync_wrote(&oy,stream->size());
			stream->clear();

			while(ogg_sync_pageout(&oy,&og)>0)
				queue_page(&og);
		}
// 		qDebug("loop4 theora_p=%d videobuf_ready=%d",theora_p, videobuf_ready);
	// theora is one in, one out...

		if(ogg_stream_packetout(&to,&op)>0)
		{
			if(th_decode_packetin(td,&op,&videobuf_granulepos)==0)
				videobuf_ready=1;
		} else {
// 			qDebug("ogg_stream_packetout <=0");
			//need more data :)
			return;
		}
	}

	if(!videobuf_ready)return;


	if(stateflag && videobuf_ready)
	{
		video_write();
		videobuf_ready=0;
	}

	// if our buffers either don't exist or are ready to go,we can begin playback
	if(!theora_p || videobuf_ready)stateflag=1;
}

/* helper: push a page into the appropriate steam */
/* this can be done blindly; a stream won't accept a page
                that doesn't belong to it */
int KviTheoraDecoder::queue_page(ogg_page *page)
{
	if(theora_p)ogg_stream_pagein(&to,page);
	if(irct_p)ogg_stream_pagein(&zo,page);
	return 0;
}

void KviTheoraDecoder::video_write(void)
{
	th_ycbcr_buffer yuv;
	int y_offset;
	th_decode_ycbcr_out(td,yuv);

	y_offset=(geometry.pic_x&~1)+yuv[0].stride*(geometry.pic_y&~1);

	for(int i=0;i<geometry.pic_h;i++)
	{
		unsigned char *in_y  = (unsigned char *)yuv[0].data+y_offset+yuv[0].stride*i;
		unsigned char *in_u  = (unsigned char *)yuv[1].data+y_offset+yuv[1].stride*i;
		unsigned char *in_v  = (unsigned char *)yuv[2].data+y_offset+yuv[2].stride*i;
		unsigned char *out = RGBbuffer+(geometry.pic_w * i * ARGB32_BPP);
		for (int j=0;j<geometry.pic_w;j++)
		{
			int r, g, b;
			int y = lu_Y[in_y[j]];
			b=(y + lu_B[in_u[j]])>>21;
			out[j*4]     = OC_CLAMP255(b);
			g=(y + lu_GV[in_v[j]] + lu_GU[in_u[j]])>>21;
			out[j*4+1]   = OC_CLAMP255(g);
			r=(y + lu_R[in_v[j]])>>21;
			out[j*4+2]   = OC_CLAMP255(r);
			out[j*4+3]   = 255; //alpha
		}
		
	}

	int size = sizeof(char) * geometry.pic_w * geometry.pic_h * ARGB32_BPP;

// 	qDebug("VIDEO WRITE size=%d",size);
	m_pVideoSignal->clear();
	m_pVideoSignal->append(RGBbuffer, size);
}

#endif // COMPILE_DISABLE_OGG_THEORA