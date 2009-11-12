//=============================================================================
//
//   File : videodevice.h
//   Creation date : Tue Nov 10 18:08:09 2009 GMT by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
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
//   sourcecode. Original copyright of the file follows:
//=============================================================================

/*
    videodevice.h  -  Kopete Video Device Low-level Support

    Copyright (c) 2005-2006 by Cláudio da Silveira Pinheiro   <taupter@gmail.com>

    Kopete    (c) 2002-2003      by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#define ENABLE_AV

#ifndef KOPETE_AVVIDEODEVICELISTITEM_H
#define KOPETE_AVVIDEODEVICELISTITEM_H

//#include <config-kopete.h>

#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#if defined(__linux__) && defined(ENABLE_AV)

#include <asm/types.h>
#undef __STRICT_ANSI__
#ifndef __u64 //required by videodev.h
#define __u64 unsigned long long
#endif // __u64
#ifndef __s64 //required by videodev.h
#define __s64 signed long long
#endif // __s64


#ifndef pgoff_t
#define pgoff_t unsigned long
#endif

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>
#define VIDEO_MODE_PAL_Nc  3
#define VIDEO_MODE_PAL_M   4
#define VIDEO_MODE_PAL_N   5
#define VIDEO_MODE_NTSC_JP 6
#define __STRICT_ANSI__

#ifdef HAVE_LIBV4L2
#include <libv4l2.h>
#endif // HAVE_V4L2

#endif // __linux__

#include <qstring.h>
#include <qfile.h>
#include <qimage.h>
#include <q3valuevector.h>
#include <kcombobox.h>

#include "videoinput.h"

namespace Kopete {

namespace AV {

/**
@author Kopete Developers
*/
typedef enum
{
	VIDEODEV_DRIVER_NONE
#if defined( __linux__) && defined(ENABLE_AV)
        ,
	VIDEODEV_DRIVER_V4L
#ifdef V4L2_CAP_VIDEO_CAPTURE
        ,
	VIDEODEV_DRIVER_V4L2
#endif
#endif
} videodev_driver;

typedef enum
{
// Packed RGB formats
	PIXELFORMAT_NONE	= 0,
	PIXELFORMAT_GREY	= (1 << 0),
	PIXELFORMAT_RGB332	= (1 << 1),
	PIXELFORMAT_RGB444	= (1 << 2),
	PIXELFORMAT_RGB555	= (1 << 3),
	PIXELFORMAT_RGB565	= (1 << 4),
	PIXELFORMAT_RGB555X	= (1 << 5),
	PIXELFORMAT_RGB565X	= (1 << 6),
	PIXELFORMAT_BGR24	= (1 << 7),
	PIXELFORMAT_RGB24	= (1 << 8),
	PIXELFORMAT_BGR32	= (1 << 9),
	PIXELFORMAT_RGB32	= (1 << 10),

// Bayer RGB format
	PIXELFORMAT_SBGGR8	= (1 << 11),

// YUV formats
	PIXELFORMAT_YUYV	= (1 << 12),
	PIXELFORMAT_UYVY	= (1 << 13),
	PIXELFORMAT_YUV420P	= (1 << 14),
	PIXELFORMAT_YUV422P	= (1 << 15),

// Compressed formats
	PIXELFORMAT_JPEG	= (1 << 16),
	PIXELFORMAT_MPEG	= (1 << 17),

// Reserved formats
	PIXELFORMAT_DV		= (1 << 18),
	PIXELFORMAT_ET61X251	= (1 << 19),
	PIXELFORMAT_HI240	= (1 << 20),
	PIXELFORMAT_HM12	= (1 << 21),
	PIXELFORMAT_MJPEG	= (1 << 22),
	PIXELFORMAT_PWC1	= (1 << 23),
	PIXELFORMAT_PWC2	= (1 << 24),
	PIXELFORMAT_SN9C10X	= (1 << 25),
	PIXELFORMAT_WNVA	= (1 << 26),
	PIXELFORMAT_YYUV	= (1 << 27)

//	PIXELFORMAT_ALL		= 0x00003FFF
} pixel_format;

typedef enum
{
// One bit for each
	STANDARD_PAL_B		= (1 << 0),
	STANDARD_PAL_B1		= (1 << 1),
	STANDARD_PAL_G		= (1 << 2),
	STANDARD_PAL_H		= (1 << 3),
	STANDARD_PAL_I		= (1 << 4),
	STANDARD_PAL_D		= (1 << 5),
	STANDARD_PAL_D1		= (1 << 6),
	STANDARD_PAL_K		= (1 << 7),

	STANDARD_PAL_M		= (1 << 8),
	STANDARD_PAL_N		= (1 << 9),
	STANDARD_PAL_Nc		= (1 << 10),
	STANDARD_PAL_60		= (1 << 11),
// STANDARD_PAL_60 is a hybrid standard with 525 lines, 60 Hz refresh rate, and PAL color modulation with a 4.43 MHz color subcarrier. Some PAL video recorders can play back NTSC tapes in this mode for display on a 50/60 Hz agnostic PAL TV.

	STANDARD_NTSC_M		= (1 << 12),
	STANDARD_NTSC_M_JP	= (1 << 13),
	STANDARD_NTSC_443	= (1 << 14),
// STANDARD_NTSC_443 is a hybrid standard with 525 lines, 60 Hz refresh rate, and NTSC color modulation with a 4.43 MHz color subcarrier.
	STANDARD_NTSC_M_KR	= (1 << 15),

	STANDARD_SECAM_B	= (1 << 16),
	STANDARD_SECAM_D	= (1 << 17),
	STANDARD_SECAM_G	= (1 << 18),
	STANDARD_SECAM_H	= (1 << 19),
	STANDARD_SECAM_K	= (1 << 20),
	STANDARD_SECAM_K1	= (1 << 21),
	STANDARD_SECAM_L	= (1 << 22),
	STANDARD_SECAM_LC	= (1 << 23),

// ATSC/HDTV
	STANDARD_ATSC_8_VSB	= (1 << 24),
	STANDARD_ATSC_16_VSB	= (1 << 25),

// Some common needed stuff
	STANDARD_PAL_BG		= ( STANDARD_PAL_B   | STANDARD_PAL_B1   | STANDARD_PAL_G  ),
	STANDARD_PAL_DK		= ( STANDARD_PAL_D   | STANDARD_PAL_D1   | STANDARD_PAL_K  ),
	STANDARD_PAL		= ( STANDARD_PAL_BG  | STANDARD_PAL_DK   | STANDARD_PAL_H    | STANDARD_PAL_I  ),
	STANDARD_NTSC		= ( STANDARD_NTSC_M  | STANDARD_NTSC_M_JP| STANDARD_NTSC_M_KR ),
	STANDARD_SECAM_DK       = ( STANDARD_SECAM_D | STANDARD_SECAM_K  | STANDARD_SECAM_K1 ),
	STANDARD_SECAM		= ( STANDARD_SECAM_B | STANDARD_SECAM_G  | STANDARD_SECAM_H  | STANDARD_SECAM_DK | STANDARD_SECAM_L | STANDARD_SECAM_LC ),

// some merged standards */
	STANDARD_MN		= ( STANDARD_PAL_M  | STANDARD_PAL_N    | STANDARD_PAL_Nc  | STANDARD_NTSC ),
	STANDARD_B		= ( STANDARD_PAL_B  | STANDARD_PAL_B1   | STANDARD_SECAM_B ),
	STANDARD_GH		= ( STANDARD_PAL_G  | STANDARD_PAL_H    | STANDARD_SECAM_G | STANDARD_SECAM_H ),
	STANDARD_DK		= ( STANDARD_PAL_DK | STANDARD_SECAM_DK ),

	STANDARD_525_60		= ( STANDARD_PAL_M   | STANDARD_PAL_60   | STANDARD_NTSC     | STANDARD_NTSC_443),
	STANDARD_625_50		= ( STANDARD_PAL     | STANDARD_PAL_N    | STANDARD_PAL_Nc   | STANDARD_SECAM),
	STANDARD_ATSC		= ( STANDARD_ATSC_8_VSB | STANDARD_ATSC_16_VSB ),

	STANDARD_UNKNOWN	= 0,
	STANDARD_ALL		= ( STANDARD_525_60  | STANDARD_625_50)
} signal_standard;


typedef enum
{
	IO_METHOD_NONE,
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR
} io_method;

struct imagebuffer
{
	int height;
	int width;
	pixel_format pixelformat;
	QVector <uchar> data; // maybe it should be a rawbuffer instead of it? It could make us avoid a memory copy
};
struct rawbuffer // raw buffer
{
	uchar * start;
	size_t length;
};


class VideoDevice{
public:
	VideoDevice();
	virtual ~VideoDevice();
	int setFileName(QString filename);
	virtual int open();
	virtual bool isOpen();
	virtual int checkDevice();
	int showDeviceCapabilities();
	virtual int initDevice();
	int inputs();
	int width();
	int minWidth();
	int maxWidth();
	int height();
	int minHeight();
	int maxHeight();
	virtual int setSize( int newwidth, int newheight);

	virtual pixel_format setPixelFormat(pixel_format newformat);
	int pixelFormatCode(pixel_format pixelformat);
	pixel_format pixelFormatForPalette( int palette );
	int pixelFormatDepth(pixel_format pixelformat);
	QString pixelFormatName(pixel_format pixelformat);
	QString pixelFormatName(int pixelformat);
	int detectPixelFormats();

	__u64 signalStandardCode(signal_standard standard);
	QString signalStandardName(signal_standard standard);
	QString signalStandardName(int standard);
	int detectSignalStandards();

	int currentInput();
	int selectInput(int input);
	int setInputParameters();
	virtual int startCapturing();
	virtual int getFrame();
	virtual int getFrame(imagebuffer *imgbuffer);
	virtual int getImage(QImage *qimage);
	virtual int stopCapturing();
	virtual int close();

	float getBrightness();
	float setBrightness(float brightness);
	float getContrast();
	float setContrast(float contrast);
	float getSaturation();
	float setSaturation(float saturation);
	float getWhiteness();
	float setWhiteness(float whiteness);
	float getHue();
	float setHue(float Hue);

	bool getAutoBrightnessContrast();
	bool setAutoBrightnessContrast(bool brightnesscontrast);
	bool getAutoColorCorrection();
	bool setAutoColorCorrection(bool colorcorrection);
	bool getImageAsMirror();
	bool setImageAsMirror(bool imageasmirror);

	bool canCapture();
	bool canChromakey();
	bool canScale();
	bool canOverlay();
	bool canRead();
	bool canAsyncIO();
	bool canStream();

	void setUdi( const QString & );
	QString udi() const;
	QString m_model;
	QString m_name;
	size_t m_modelindex; // Defines what's the number of a device when more than 1 device of a given model is present;
	QString full_filename;
	videodev_driver m_driver;
	int descriptor;

//protected:
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
	struct v4l2_capability V4L2_capabilities;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	struct v4l2_fmtdesc fmtdesc; // Not sure if it must be here or inside detectPixelFormats(). Should inve
//	struct v4l2_input m_input;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_querymenu querymenu;
	void enumerateControls (void);
	void enumerateMenu (void);
#endif
	struct video_capability V4L_capabilities;
	struct video_buffer V4L_videobuffer;
#endif	
	QVector<Kopete::AV::VideoInput> m_input;
//	QFile file;
protected:
	int currentwidth, minwidth, maxwidth, currentheight, minheight, maxheight;

	QVector<rawbuffer> m_rawbuffers;
	unsigned int m_streambuffers;
	imagebuffer m_currentbuffer;
	int m_buffer_size;

	int m_current_input;
	pixel_format m_pixelformat;

	io_method m_io_method;
	bool m_videocapture;
	bool m_videochromakey;
	bool m_videoscale;
	bool m_videooverlay;
	bool m_videoread;
	bool m_videoasyncio;
	bool m_videostream;

	int xioctl(int request, void *arg);
	int errnoReturn(const char* s);
	int initRead();
	int initMmap();
	int initUserptr();

	QString m_udi;
};

}

}

#endif
