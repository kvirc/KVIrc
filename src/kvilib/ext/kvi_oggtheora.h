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

#ifndef COMPILE_DISABLE_OGG_THEORA

#include "kvi_settings.h"
#include "kvi_malloc.h"

#include "theora/theoradec.h"
#include "theora/theoraenc.h"

#include <QColor>

#define OC_MINI(_a,_b)      ((_a)>(_b)?(_b):(_a))
#define OC_MAXI(_a,_b)      ((_a)<(_b)?(_b):(_a))
#define OC_CLAMPI(_a,_b,_c) (OC_MAXI(_a,OC_MINI(_b,_c)))

#define YUV444_BPP 3
#define ARGB32_BPP 4

class KviDataBuffer;

typedef struct _KviTheoraGeometry {
	//original geometry, set by user
	int pic_w;
	int pic_h;
	//calculated geometry: frame
	int frame_w;
	int frame_h;
	//calculated geometry: offset
	int pic_x;
	int pic_y;
} KviTheoraGeometry;

class KVILIB_API KviOggIrcText
{
private:
	static void _tp_readbuffer(oggpack_buffer *opb, char *buf, const long len)
	{
		long i;

		for (i = 0; i < len; i++)
			*buf++=oggpack_read(opb,8);
	}

	static void _tp_writebuffer(oggpack_buffer *opb, const char *buf, const long len)
	{
		long i;

		for (i = 0; i < len; i++)
			oggpack_write(opb, *buf++, 8);
	}

public:
	static int irct_encode_init()
	{
		return 0;
	};

	static int irct_encode_clear()
	{
		return 0;
	};

	static int irct_encode_headerout(ogg_packet *op) {
		oggpack_buffer ob;
		oggpack_writeinit(&ob);
		oggpack_write(&ob, 0, 8);			//header init
		_tp_writebuffer(&ob, "irct", 32);
		oggpack_write(&ob, 0, 8);			//version 0
		oggpack_write(&ob, 1, 8);			//subversion 1
		int bytes=oggpack_bytes(&ob);
		op->packet= (unsigned char*) kvi_malloc(bytes);
		memcpy(op->packet, oggpack_get_buffer(&ob), bytes);
		op->bytes=bytes;
		oggpack_writeclear(&ob);
		op->b_o_s=1;  //begins a logical bitstream
		op->e_o_s=0;
		op->packetno=0;
		op->granulepos=0;

		return 0;
	}
	static int irct_encode_packetout(const char* textPkt, int textSize, int last_p, ogg_packet *op) {
		if(!textSize)return(0);

		oggpack_buffer ob;
		oggpack_writeinit(&ob);

		_tp_writebuffer(&ob, textPkt, textSize); //pre-encoded text
		int bytes=oggpack_bytes(&ob);
		op->packet= (unsigned char *)kvi_malloc(bytes);
		memcpy(op->packet, oggpack_get_buffer(&ob), bytes);
		qDebug("irct_encode_packetout bytes=%d |%s", bytes, textPkt);
		op->bytes=bytes;
		oggpack_writeclear(&ob);
		op->b_o_s=0;
		op->e_o_s=last_p;

		op->packetno=last_p;
		op->granulepos=0;

		return 0;
	}
	static int irct_decode_headerin(ogg_packet *op) {
		oggpack_buffer ob;
		oggpack_readinit(&ob, op->packet, op->bytes);
		quint8 ret, version, subversion;
		ret = oggpack_read(&ob,8);
		if(ret!=0) return 1;
		char * buf = (char*) kvi_malloc(4);
		_tp_readbuffer(&ob, buf, 4);
		if(strncmp(buf, "irct", 4)!=0) return 1;
		version = oggpack_read(&ob,8);
		subversion = oggpack_read(&ob,8);
		return 0;
	}
	static int irct_decode_packetin(char* textPkt, int textSize, ogg_packet *op)
	{
		oggpack_buffer ob;
		oggpack_readinit(&ob, op->packet, op->bytes);
		textSize = op->bytes;
		textPkt=(char*)kvi_malloc(textSize);
		_tp_readbuffer(&ob, textPkt, textSize);
		return 0;
	}
};

class KVILIB_API KviTheoraEncoder
{
public:
	KviTheoraEncoder(KviDataBuffer * stream, int iWidth=320, int iHeight=240, int iFpsN=5, int iFpsD=1, int iParN=4, int iParD=3);
	virtual ~KviTheoraEncoder();
	void addVideoFrame(QRgb * rgb32, int videoSize);
	void addTextFrame(unsigned char* textPkt, int textSize);
private:
	int fetch_and_process_video(quint8 * videoYuv,ogg_page *videopage,ogg_stream_state *to,th_enc_ctx *td,int videoflag);
	int fetch_and_process_video_packet(quint8 * videoYuv,th_enc_ctx *td,ogg_packet *op);
private:
	KviTheoraGeometry geometry;
	KviDataBuffer * m_pStream;
	quint8 * videoYuv;

	ogg_int64_t text_sofar;
	
	int                 frame_state;
	ogg_int64_t         frames;
	unsigned char      *yuvframe[3];

	// The size of each converted frame buffer.
	size_t y4m_dst_buf_sz;
	// The amount to read directly into the converted frame buffer.
	size_t y4m_dst_buf_read_sz;
	// The size of the auxilliary buffer.
	size_t y4m_aux_buf_sz;
	// The amount to read into the auxilliary buffer.
	size_t y4m_aux_buf_read_sz;

	ogg_stream_state zo; // take physical pages, weld into a logical stream of packets
	ogg_stream_state to; // take physical pages, weld into a logical stream of packets
	ogg_page         og; // one Ogg bitstream page.  Vorbis packets are inside
	ogg_packet       op; // one raw packet of data for decode

	th_enc_ctx      *td;
	th_info          ti;
	th_comment       tc;

	int textflag;
	int videoflag;
private:
	static int ilog(unsigned _v);
};


class KVILIB_API KviTheoraDecoder
{
public:
	KviTheoraDecoder(KviDataBuffer * videoSignal,KviDataBuffer * textSignal);
	virtual ~KviTheoraDecoder();
	void addData(KviDataBuffer * stream);
private:
	int queue_page(ogg_page *page);
	double get_time();
	void video_write(void);
private:
	KviDataBuffer * m_pVideoSignal;
	KviDataBuffer * m_pTextSignal;
	KviTheoraGeometry geometry;
	unsigned char * RGBbuffer;

	// Ogg and codec state for demux/decode
	ogg_sync_state   oy;
	ogg_page         og;
	ogg_stream_state to;
	ogg_stream_state zo;
	th_info          ti;
	th_comment       tc;
	th_dec_ctx       *td;
	th_setup_info    *ts;
	th_pixel_fmt     px_fmt;

	int              theora_p;
	int              irct_p;
	int              stateflag;

	int lu_Y[256];
	int lu_R[256];
	int lu_GU[256];
	int lu_GV[256];
	int lu_B[256];

	// single frame video buffering
	int          videobuf_ready;
	ogg_int64_t  videobuf_granulepos;
	double       videobuf_time;

	int pp_level_max;
	int pp_level;
	int pp_inc;
	int i,j;
	ogg_packet op;

	int frames;
	int dropped;

	bool thda;
	bool thtic;

};

#endif // COMPILE_DISABLE_OGG_THEORA
#endif /* KVI_OGGTHEORA_H_ */
