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

#include "theora/theoradec.h"
#include "theora/theoraenc.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"

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

class KVILIB_API KviTheoraEncoder {
public:
	KviTheoraEncoder(KviDataBuffer * stream, int iWidth=320, int iHeight=240, int iFpsN=5, int iFpsD=1, int iParN=4, int iParD=3);
	virtual ~KviTheoraEncoder();
	void addVideoFrame(QRgb * rgb32, int videoSize);
	void addAudioFrame(unsigned char* audioPkt, int audioSize);
private:
	int fetch_and_process_video(quint8 * videoYuv,ogg_page *videopage,ogg_stream_state *to,th_enc_ctx *td,int videoflag);
	int fetch_and_process_video_packet(quint8 * videoYuv,th_enc_ctx *td,ogg_packet *op);
	int fetch_and_process_audio(quint8 * audio, int size, ogg_page *audiopage, ogg_stream_state *vo, vorbis_dsp_state *vd, vorbis_block *vb, int audioflag);
private:
	KviTheoraGeometry geometry;
	KviDataBuffer * m_pStream;
	quint8 * videoYuv;

	bool audio;
	int audio_ch;
	int audio_hz;
	float audio_q;
	ogg_int64_t samples_sofar;

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

	ogg_stream_state to; // take physical pages, weld into a logical stream of packets
	ogg_stream_state vo; // take physical pages, weld into a logical stream of packets
	ogg_page         og; // one Ogg bitstream page.  Vorbis packets are inside
	ogg_packet       op; // one raw packet of data for decode

	th_enc_ctx      *td;
	th_info          ti;
	th_comment       tc;

	vorbis_info      vi; // struct that stores all the static vorbis bitstream settings
	vorbis_comment   vc; // struct that stores all the user comments
	vorbis_dsp_state vd; // central working state for the packet->PCM decoder
	vorbis_block     vb; // local working space for packet->PCM decode

	int audioflag;
	int videoflag;
private:
	static int ilog(unsigned _v);
};


class KVILIB_API KviTheoraDecoder {
public:
	KviTheoraDecoder(KviDataBuffer * signal);
	virtual ~KviTheoraDecoder();
	void addData(KviDataBuffer * stream);
private:
	int queue_page(ogg_page *page);
	double get_time();
	void video_write(void);
private:
	KviDataBuffer * m_pSignal;
	KviTheoraGeometry geometry;
	unsigned char * RGBbuffer;

	// Ogg and codec state for demux/decode
	ogg_sync_state   oy;
	ogg_page         og;
	ogg_stream_state vo;
	ogg_stream_state to;
	th_info          ti;
	th_comment       tc;
	th_dec_ctx       *td;
	th_setup_info    *ts;
	vorbis_info      vi;
	vorbis_dsp_state vd;
	vorbis_block     vb;
	vorbis_comment   vc;
	th_pixel_fmt     px_fmt;

	int              theora_p;
	int              vorbis_p;
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

	// single audio fragment audio buffering
	int          audiobuf_fill;
	int          audiobuf_ready;
	ogg_int16_t *audiobuf;
	ogg_int64_t  audiobuf_granulepos; // time position of last sample

	int pp_level_max;
	int pp_level;
	int pp_inc;
	int i,j;
	ogg_packet op;

	int frames;
	int dropped;

	long         audiofd_totalsize;
	int          audiofd_fragsize;      /* read and write only complete fragments
					so that SNDCTL_DSP_GETOSPACE is
					accurate immediately after a bank
					switch */
	int          audiofd;
	ogg_int64_t  audiofd_timer_calibrate;
	bool thda;
	bool thtic;

};

#endif // COMPILE_DISABLE_OGG_THEORA
#endif /* KVI_OGGTHEORA_H_ */
