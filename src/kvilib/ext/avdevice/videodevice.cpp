//=============================================================================
//
//   File : videodevice.cpp
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
    videodevice.cpp  -  Kopete Video Device Low-level Support

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

#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <kdebug.h>

#include "videoinput.h"
#include "videodevice.h"

#include "bayer.h"
#include "sonix_compress.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

namespace Kopete {

namespace AV {

VideoDevice::VideoDevice()
{
//	kDebug() << "libkopete (avdevice): VideoDevice() called";
	descriptor = -1;
	m_streambuffers  = 0;
	m_current_input = 0;
//	kDebug() << "libkopete (avdevice): VideoDevice() exited successfuly";
}


VideoDevice::~VideoDevice()
{
}

#ifdef V4L2_CAP_VIDEO_CAPTURE

void VideoDevice::enumerateControls (void)
{
// -----------------------------------------------------------------------------------------------------------------
// This must turn up to be a proper method to check for controls' existence.
	CLEAR (queryctrl);
// v4l2_queryctrl may zero the .id in some cases, even if the IOCTL returns EXIT_SUCCESS (tested with a bttv card, when testing for V4L2_CID_AUDIO_VOLUME).
// As of 6th Aug 2007, according to the V4L2 specification version 0.21, this behavior is undocumented, and the example 1-8 code found at
// http://www.linuxtv.org/downloads/video4linux/API/V4L2_API/spec/x519.htm fails because of this behavior with a bttv card.

	int currentid = V4L2_CID_BASE;

kDebug() << "Checking CID controls";

	for (currentid = V4L2_CID_BASE; currentid < V4L2_CID_LASTP1; currentid++)
//for (queryctrl.id = 9963776; queryctrl.id < 9963800; queryctrl.id++)
	{
		queryctrl.id = currentid;
		if (0 == xioctl (VIDIOC_QUERYCTRL, &queryctrl))
		{
			if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				continue;

kDebug() <<  " Control: " << QString::fromLocal8Bit((const char*)queryctrl.name) << " Values from " << queryctrl.minimum << " to " << queryctrl.maximum << " with steps of " << queryctrl.step << ". Default: " << queryctrl.default_value;

//			switch (queryctrl.type)
//			{
//				case V4L2_CTRL_TYPE_INTEGER : 
//			}
			if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
				enumerateMenu ();
		}
		else
		{
			if (errno == EINVAL)
				continue;

			perror ("VIDIOC_QUERYCTRL");
//			exit (EXIT_FAILURE);
		}
	}

kDebug() << "Checking CID private controls";

	for (currentid = V4L2_CID_PRIVATE_BASE;; currentid++)
	{
		queryctrl.id = currentid;
		if ( 0 == xioctl (VIDIOC_QUERYCTRL, &queryctrl))
		{
			if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				continue;

kDebug() << " Control: " << QString::fromLocal8Bit((const char*)queryctrl.name) << " Values from " << queryctrl.minimum << " to " << queryctrl.maximum << " with steps of " << queryctrl.step << ". Default: " << queryctrl.default_value;

			if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
				enumerateMenu ();
		}
		else
		{
			if (errno == EINVAL)
				break;

			perror ("VIDIOC_QUERYCTRL");
//			exit (EXIT_FAILURE);
		}
	}
}

void VideoDevice::enumerateMenu (void)
{
	kDebug() <<  "  Menu items:";

	memset (&querymenu, 0, sizeof (querymenu));
	querymenu.id = queryctrl.id;

	for (querymenu.index = queryctrl.minimum; querymenu.index <= (unsigned int) queryctrl.maximum; ++querymenu.index)
	{
		if (0 == xioctl (VIDIOC_QUERYMENU, &querymenu))
		{
			kDebug() <<  "  " << QString::fromLocal8Bit((const char*)querymenu.name);
                }
		else
		{
			perror ("VIDIOC_QUERYMENU");
			exit (EXIT_FAILURE);
		}
	}
}
#endif

/*!
    \fn VideoDevice::xioctl(int fd, int request, void *arg)
 */
int VideoDevice::xioctl(int request, void *arg)
{
	int r;

#ifdef HAVE_LIBV4L2
	do r = v4l2_ioctl (descriptor, request, arg);
#else
	do r = ioctl (descriptor, request, arg);
#endif
	while (-1 == r && EINTR == errno);
	return r;
}

/*!
    \fn VideoDevice::errnoReturn(const char* s)
 */
int VideoDevice::errnoReturn(const char* s)
{
    /// @todo implement me
	fprintf (stderr, "%s error %d, %s\n",s, errno, strerror (errno));
	return EXIT_FAILURE;
}

/*!
    \fn VideoDevice::setFileName(QString name)
 */
int VideoDevice::setFileName(QString filename)
{
    /// @todo implement me
	full_filename=filename;
	return EXIT_SUCCESS;
}

/*!
    \fn VideoDevice::open()
 */
int VideoDevice::open()
{
    /// @todo implement me

	kDebug() << "called";
	if(-1 != descriptor)
	{
		kDebug() << "Device is already open";
		return EXIT_SUCCESS;
	}
#ifdef HAVE_LIBV4L2
	descriptor = ::v4l2_open (QFile::encodeName(full_filename), O_RDWR, 0);
#else
	descriptor = ::open (QFile::encodeName(full_filename), O_RDWR, 0);
#endif
	if(isOpen())
	{
		kDebug() << "File " << full_filename << " was opened successfuly";
		if(EXIT_FAILURE==checkDevice())
		{
			kDebug() << "File " << full_filename << " could not be opened";
			close();
			return EXIT_FAILURE;
		}
	}
	else
	{
		kDebug() << "Unable to open file " << full_filename << "Err: "<< errno;
		return EXIT_FAILURE;
	}

	initDevice();
	selectInput(m_current_input);
	kDebug() << "exited successfuly";
	return EXIT_SUCCESS;
}

bool VideoDevice::isOpen()
{
	if(-1 == descriptor)
	{
//		kDebug() << "VideoDevice::isOpen() File is not open";
		return false;
	}
//	kDebug() << "VideoDevice::isOpen() File is open";
	return true;
}

int VideoDevice::checkDevice()
{
	kDebug() << "checkDevice() called.";
	if(isOpen())
	{
		m_videocapture=false;
		m_videochromakey=false;
		m_videoscale=false;
		m_videooverlay=false;
		m_videoread=false;
		m_videoasyncio=false;
		m_videostream=false;

		m_driver=VIDEODEV_DRIVER_NONE;
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE

		CLEAR(V4L2_capabilities);

		if (-1 != xioctl (VIDIOC_QUERYCAP, &V4L2_capabilities))
		{
			if (!(V4L2_capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE))
			{
				kDebug() << "checkDevice(): " << full_filename << " is not a video capture device.";
				m_driver = VIDEODEV_DRIVER_NONE;
				return EXIT_FAILURE;
			}
			m_videocapture=true;
			kDebug() << "checkDevice(): " << full_filename << " is a V4L2 device.";
			m_driver = VIDEODEV_DRIVER_V4L2;
			m_model=QString::fromLocal8Bit((const char*)V4L2_capabilities.card);


// Detect maximum and minimum resolution supported by the V4L2 device. VIDIOC_ENUM_FRAMESIZES is still experimental.
			CLEAR (fmt);
			fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == xioctl (VIDIOC_G_FMT, &fmt))
			{
				kDebug() << "VIDIOC_G_FMT failed (" << errno << ").";
			}
			fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			fmt.fmt.pix.width       = 32767;
			fmt.fmt.pix.height      = 32767;
			fmt.fmt.pix.field       = V4L2_FIELD_ANY;
			if (-1 == xioctl (VIDIOC_S_FMT, &fmt))
			{
				kDebug() << "Detecting maximum size with VIDIOC_S_FMT failed (" << errno << ").Returned maxwidth: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
				// Note VIDIOC_S_FMT may change width and height.
			}
			else
			{
				maxwidth  = fmt.fmt.pix.width;
				maxheight = fmt.fmt.pix.height;
			}
			if (-1 == xioctl (VIDIOC_G_FMT, &fmt))
			{
				kDebug() << "VIDIOC_G_FMT failed (" << errno << ").";
			}
			fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			fmt.fmt.pix.width       = 1;
			fmt.fmt.pix.height      = 1;
			fmt.fmt.pix.field       = V4L2_FIELD_ANY;
			if (-1 == xioctl (VIDIOC_S_FMT, &fmt))
			{
				kDebug() << "Detecting minimum size with VIDIOC_S_FMT failed (" << errno << ").Returned maxwidth: " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
				// Note VIDIOC_S_FMT may change width and height.
			}
			else
			{
				minwidth  = fmt.fmt.pix.width;
				minheight = fmt.fmt.pix.height;
			}

// Buggy driver paranoia
/*				min = fmt.fmt.pix.width * 2;
				if (fmt.fmt.pix.bytesperline < min)
					fmt.fmt.pix.bytesperline = min;
				min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
				if (fmt.fmt.pix.sizeimage < min)
					fmt.fmt.pix.sizeimage = min;
				m_buffer_size=fmt.fmt.pix.sizeimage ;*/

			int inputisok=EXIT_SUCCESS;
			m_input.clear();
			for(unsigned int loop=0; inputisok==EXIT_SUCCESS; loop++)
			{
				struct v4l2_input videoinput;
				CLEAR(videoinput);
				videoinput.index = loop;
				inputisok=xioctl(VIDIOC_ENUMINPUT, &videoinput);
				if(inputisok==EXIT_SUCCESS)
				{
					VideoInput tempinput;
					tempinput.name = QString::fromLocal8Bit((const char*)videoinput.name);
					tempinput.hastuner = videoinput.type & V4L2_INPUT_TYPE_TUNER;
detectSignalStandards();
					tempinput.m_standards = videoinput.std;
					m_input.push_back(tempinput);
					kDebug() << "Input " << loop << ": " << tempinput.name << " (tuner: " << ((videoinput.type & V4L2_INPUT_TYPE_TUNER) != 0) << ")";
					if((videoinput.type & V4L2_INPUT_TYPE_TUNER) != 0)
					{
//						_tunerForInput[name] = desc.tuner;
//						_isTuner = true;
					}
					else
					{
//						_tunerForInput[name] = -1;
					}
				}
			}
		}
		else
		{
// V4L-only drivers should return an EINVAL in errno to indicate they cannot handle V4L2 calls. Not every driver is compliant, so
// it will try the V4L api even if the error code is different than expected.
			kDebug() << "checkDevice(): " << full_filename << " is not a V4L2 device.";
		}

#endif

		CLEAR(V4L_capabilities);

		if(m_driver==VIDEODEV_DRIVER_NONE)
		{
			kDebug() << "checkDevice(): " << full_filename << " Trying V4L API.";
			if (-1 == xioctl (VIDIOCGCAP, &V4L_capabilities))
			{
				perror ("ioctl (VIDIOCGCAP)");
				m_driver = VIDEODEV_DRIVER_NONE;
				return EXIT_FAILURE;
			}
			else
			{
				kDebug() << full_filename << " is a V4L device.";
				m_driver = VIDEODEV_DRIVER_V4L;
				m_model=QString::fromLocal8Bit((const char*)V4L_capabilities.name);
				if(V4L_capabilities.type & VID_TYPE_CAPTURE)
					m_videocapture=true;
				if(V4L_capabilities.type & VID_TYPE_CHROMAKEY)
					m_videochromakey=true;
				if(V4L_capabilities.type & VID_TYPE_SCALES)
					m_videoscale=true;	
				if(V4L_capabilities.type & VID_TYPE_OVERLAY)
					m_videooverlay=true;
//				kDebug() << "libkopete (avdevice):     Inputs : " << V4L_capabilities.channels;
//				kDebug() << "libkopete (avdevice):     Audios : " << V4L_capabilities.audios;
				minwidth  = V4L_capabilities.minwidth;
				maxwidth  = V4L_capabilities.maxwidth;
				minheight = V4L_capabilities.minheight;
				maxheight = V4L_capabilities.maxheight;


				int inputisok=EXIT_SUCCESS;
				m_input.clear();
				for(int loop=0; loop < V4L_capabilities.channels; loop++)
				{
					struct video_channel videoinput;
					CLEAR(videoinput);
					videoinput.channel = loop;
					videoinput.norm    = 1;
					inputisok=xioctl(VIDIOCGCHAN, &videoinput);
					if(inputisok==EXIT_SUCCESS)
					{
						VideoInput tempinput;
						tempinput.name = QString::fromLocal8Bit((const char*)videoinput.name);
						tempinput.hastuner=videoinput.flags & VIDEO_VC_TUNER;
// TODO: The routine to detect the appropriate video standards for V4L must be placed here
						m_input.push_back(tempinput);
//						kDebug() << "libkopete (avdevice): Input " << loop << ": " << tempinput.name << " (tuner: " << ((videoinput.flags & VIDEO_VC_TUNER) != 0) << ")";
/*						if((input.type & V4L2_INPUT_TYPE_TUNER) != 0)
						{
//							_tunerForInput[name] = desc.tuner;
//							_isTuner = true;
						}
						else
						{
//							_tunerForInput[name] = -1;
						}
*/					}
				}

			}
		}
#endif
		m_name=m_model; // Take care about changing the name to be different from the model itself...

		detectPixelFormats();
#ifdef V4L2_CAP_VIDEO_CAPTURE
		enumerateControls();
#endif
// TODO: Now we must execute the proper initialization according to the type of the driver.
		kDebug() << "checkDevice() exited successfuly.";
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}


/*!
    \fn VideoDevice::showDeviceCapabilities()
 */
int VideoDevice::showDeviceCapabilities()
{
	kDebug() << "showDeviceCapabilities() called.";
	if(isOpen())
	{
/*		kDebug() << "libkopete (avdevice): Driver: " << (const char*)V4L2_capabilities.driver << " "
			<< ((V4L2_capabilities.version>>16) & 0xFF) << "."
			<< ((V4L2_capabilities.version>> 8) & 0xFF) << "."
			<< ((V4L2_capabilities.version    ) & 0xFF) << endl;
		kDebug() << "libkopete (avdevice): Device: " << name;
		kDebug() << "libkopete (avdevice): Capabilities:";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE)
			kDebug() << "libkopete (avdevice):     Video capture";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VIDEO_OUTPUT)
			kDebug() << "libkopete (avdevice):     Video output";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VIDEO_OVERLAY)
			kDebug() << "libkopete (avdevice):     Video overlay";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VBI_CAPTURE)
			kDebug() << "libkopete (avdevice):     VBI capture";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VBI_OUTPUT)
			kDebug() << "libkopete (avdevice):     VBI output";
		if(V4L2_capabilities.capabilities & V4L2_CAP_RDS_CAPTURE)
			kDebug() << "libkopete (avdevice):     RDS capture";
		if(V4L2_capabilities.capabilities & V4L2_CAP_TUNER)
			kDebug() << "libkopete (avdevice):     Tuner IO";
		if(V4L2_capabilities.capabilities & V4L2_CAP_AUDIO)
			kDebug() << "libkopete (avdevice):     Audio IO";
;*/
		kDebug() << "Device model: " << m_model;
		kDebug() << "Device name : " << m_name;
		kDebug() << "Capabilities:";
		if(canCapture())
		{
			kDebug() << "    Video capture";
		}
		if(canRead())
		{
			kDebug() << "        Read";
		}
		if(canAsyncIO())
		{
			kDebug() << "        Asynchronous input/output";
		}
		if(canStream())
		{
			kDebug() << "        Streaming";
		}
		if(canChromakey())
		{
			kDebug() << "    Video chromakey";
		}
		if(canScale())
		{
			kDebug() << "    Video scales";
		}
		if(canOverlay())
		{
			kDebug() << "    Video overlay";
		}
//		kDebug() << "libkopete (avdevice):     Audios : " << V4L_capabilities.audios;
		kDebug() << "    Max res: " << maxWidth() << " x " << maxHeight();
		kDebug() << "    Min res: " << minWidth() << " x " << minHeight();
		kDebug() << "    Inputs : " << inputs();
		for (int loop=0; loop < inputs(); loop++)
			kDebug() << "Input " << loop << ": " << m_input[loop].name << " (tuner: " << m_input[loop].hastuner << ")";
		kDebug() << "showDeviceCapabilities() exited successfuly.";
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/*!
    \fn VideoDevicePool::initDevice()
 */
int VideoDevice::initDevice()
{
    /// @todo implement me
	kDebug() << "initDevice() started";
	if(-1 == descriptor)
	{
		kDebug() << "initDevice() Device is not open";
		return EXIT_FAILURE;
	}
	m_io_method = IO_METHOD_NONE;
	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			if(V4L2_capabilities.capabilities & V4L2_CAP_READWRITE)
			{
				m_videoread=true;
				m_io_method = IO_METHOD_READ;
				kDebug() << "    Read/Write interface";
			}
			if(V4L2_capabilities.capabilities & V4L2_CAP_ASYNCIO)
			{
				m_videoasyncio=true;
				kDebug() << "    Async IO interface";
			}
			if(V4L2_capabilities.capabilities & V4L2_CAP_STREAMING)
			{
				m_videostream=true;
				m_io_method = IO_METHOD_MMAP;
//				m_io_method = IO_METHOD_USERPTR;
				kDebug() << "    Streaming interface";
			}
			if(m_io_method==IO_METHOD_NONE)
			{
				kDebug() << "initDevice() Found no suitable input/output method for " << full_filename;
				return EXIT_FAILURE;
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			m_videoread=true;
			m_io_method=IO_METHOD_READ;
			if(-1 != xioctl(VIDIOCGFBUF,&V4L_videobuffer))
			{
//				m_videostream=true;
//				m_io_method = IO_METHOD_MMAP;
				kDebug() << "    Streaming interface";
			}
			break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:

			break;
	}

// Select video input, video standard and tune here.
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == xioctl (VIDIOC_CROPCAP, &cropcap))
	{ // Errors ignored.
	}
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	crop.c = cropcap.defrect; // reset to default
	if (-1 == xioctl (VIDIOC_S_CROP, &crop))
	{
		switch (errno)
		{
			case EINVAL: break;  // Cropping not supported.
			default:     break;  // Errors ignored.
		}
	}
#endif
#endif

	showDeviceCapabilities();
	kDebug() << "initDevice() exited successfuly";
	return EXIT_SUCCESS;
}

int VideoDevice::inputs()
{
	return m_input.size();
}


int VideoDevice::width()
{
	return currentwidth;
}

int VideoDevice::minWidth()
{
	return minwidth;
}

int VideoDevice::maxWidth()
{
	return maxwidth;
}

int VideoDevice::height()
{
	return currentheight;
}

int VideoDevice::minHeight()
{
	return minheight;
}

int VideoDevice::maxHeight()
{
	return maxheight;
}

int VideoDevice::setSize( int newwidth, int newheight)
{
kDebug() << "setSize(" << newwidth << ", " << newheight << ") called.";
	if(isOpen())
	{
// It should not be there. It must remain in a completely distict place, cause this method should not change the pixelformat.
// It shouldn't try to find a suitable pixel format this way. It should use values discovered by - detectPixelFormats() - to choose a valid one.
		kDebug() <<  "Trying YUY422P";
		if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_YUV422P))
		{
			kDebug() <<  "Device doesn't seem to support YUV422P format. Trying YUYV.";
			if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_YUYV))
			{
				kDebug() <<  "Device doesn't seem to support YUYV format. Trying UYVY.";
				if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_UYVY))
				{
					kDebug() <<  "Device doesn't seem to support UYVY format. Trying YUV420P.";
					if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_YUV420P))
					{
						kDebug() <<  "Device doesn't seem to support YUV420P format. Trying RGB24.";
						if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_RGB24))
						{
							kDebug() <<  "Device doesn't seem to support RGB24 format. Trying BGR24.";
							if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_BGR24))
							{
								kDebug() <<  "Device doesn't seem to support RGB24 format. Trying RGB32.";
								if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_RGB32))
								{
									kDebug() <<  "Device doesn't seem to support RGB32 format. Trying BGR32.";
									if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_BGR32))
									{
										kDebug() <<  "Device doesn't seem to support BGR32 format. Trying SBGGR8.";
										if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_SBGGR8))
										{
											kDebug() <<  "Device doesn't seem to support SBGGR8 format. Trying SN9C10X.";
											if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_SN9C10X))
											{
												kDebug() <<  "Device doesn't seem to support BGR32 format. Fallback to it is not yet implemented.";
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		if(newwidth  > maxwidth ) newwidth  = maxwidth;
		if(newheight > maxheight) newheight = maxheight;
		if(newwidth  < minwidth ) newwidth  = minwidth;
		if(newheight < minheight) newheight = minheight;

		currentwidth  = newwidth;
		currentheight = newheight;

//kDebug() << "width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << width() << "x" << height();
// Change resolution for the video device
		switch(m_driver)
		{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
			case VIDEODEV_DRIVER_V4L2:
//				CLEAR (fmt);
				if (-1 == xioctl (VIDIOC_G_FMT, &fmt))
				{
					kDebug() << "VIDIOC_G_FMT failed (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
				}
				fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				fmt.fmt.pix.width       = width();
				fmt.fmt.pix.height      = height();
				fmt.fmt.pix.field       = V4L2_FIELD_ANY;
				if (-1 == xioctl (VIDIOC_S_FMT, &fmt))
				{
					kDebug() << "VIDIOC_S_FMT failed (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
					// Note VIDIOC_S_FMT may change width and height.
				}
				else
				{
// Buggy driver paranoia.
kDebug() << "VIDIOC_S_FMT worked (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
					unsigned int min = fmt.fmt.pix.width * 2;
					if (fmt.fmt.pix.bytesperline < min)
					{
						fmt.fmt.pix.bytesperline = min;
					}
					min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
					if (fmt.fmt.pix.sizeimage < min)
					{
						fmt.fmt.pix.sizeimage = min;
					}
					m_buffer_size=fmt.fmt.pix.sizeimage ;
				}
				break;
#endif
			case VIDEODEV_DRIVER_V4L:
				{
					struct video_window V4L_videowindow;

				if (xioctl (VIDIOCGWIN, &V4L_videowindow)== -1)
				{
					perror ("ioctl VIDIOCGWIN");
//					return (NULL);
				}
				V4L_videowindow.width  = width();
				V4L_videowindow.height = height();
				V4L_videowindow.clipcount=0;
				if (xioctl (VIDIOCSWIN, &V4L_videowindow)== -1)
				{
					perror ("ioctl VIDIOCSWIN");
//					return (NULL);
				}
kDebug() << "------------- width: " << V4L_videowindow.width << " Height: " << V4L_videowindow.height << " Clipcount: " << V4L_videowindow.clipcount << " -----------------";

//				kDebug() << "libkopete (avdevice): V4L_picture.palette: " << V4L_picture.palette << " Depth: " << V4L_picture.depth;

/*				if(-1 == xioctl(VIDIOCGFBUF,&V4L_videobuffer))
					kDebug() << "libkopete (avdevice): VIDIOCGFBUF failed (" << errno << "): Device cannot stream";*/

				}
				break;
#endif
			case VIDEODEV_DRIVER_NONE:
			default:
				break;
		}
		m_buffer_size = width() * height() * pixelFormatDepth(m_pixelformat) / 8;
kDebug() << "------------------------- ------- -- m_buffer_size: " << m_buffer_size << " !!! -- ------- -----------------------------------------";

		m_currentbuffer.pixelformat=m_pixelformat;
		m_currentbuffer.data.resize(m_buffer_size);

		switch (m_io_method)
		{
			case IO_METHOD_NONE:                    break;
			case IO_METHOD_READ:    initRead ();    break;
			case IO_METHOD_MMAP:    initMmap ();    break;
			case IO_METHOD_USERPTR: initUserptr (); break;
		}

kDebug() << "setSize(" << newwidth << ", " << newheight << ") exited successfuly.";
		return EXIT_SUCCESS;
	}
kDebug() << "setSize(" << newwidth << ", " << newheight << ") Device is not open.";
	return EXIT_FAILURE;
}













pixel_format VideoDevice::setPixelFormat(pixel_format newformat)
{
	pixel_format ret = PIXELFORMAT_NONE;
//kDebug() << "called.";
// Change the pixel format for the video device
	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
//			CLEAR (fmt);
			if (-1 == xioctl (VIDIOC_G_FMT, &fmt))
                        {
//				return errnoReturn ("VIDIOC_S_FMT");
//				kDebug() << "VIDIOC_G_FMT failed (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
			}
			else
				m_pixelformat = pixelFormatForPalette(fmt.fmt.pix.pixelformat);
		
			fmt.fmt.pix.pixelformat = pixelFormatCode(newformat);
			if (-1 == xioctl (VIDIOC_S_FMT, &fmt))
			{
//				kDebug() << "VIDIOC_S_FMT failed (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
			}
			else
			{
				if (fmt.fmt.pix.pixelformat == (unsigned int) pixelFormatCode(newformat)) // Thih "if" (not what is contained within) is a fix for a bug in sn9c102 driver.
				{
					m_pixelformat = newformat;
					ret = m_pixelformat;
				}
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			{
			struct video_picture V4L_picture;
			if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
			{
				kDebug() << "VIDIOCGPICT failed (" << errno << ").";
			}
//			kDebug() << "V4L_picture.palette: " << V4L_picture.palette << " Depth: " << V4L_picture.depth;
			V4L_picture.palette = pixelFormatCode(newformat);
			V4L_picture.depth   = pixelFormatDepth(newformat);
			if(-1 == xioctl(VIDIOCSPICT,&V4L_picture))
			{
//				kDebug() << "Device seems to not support " << pixelFormatName(newformat) << " format. Fallback to it is not yet implemented.";
			}

			if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
			{
				kDebug() << "VIDIOCGPICT failed (" << errno << ").";
			}

//			kDebug() << "V4L_picture.palette: " << V4L_picture.palette << " Depth: " << V4L_picture.depth;
			m_pixelformat=pixelFormatForPalette(V4L_picture.palette);
			if (m_pixelformat == newformat)
				ret = newformat;

			}
			break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return ret;
}






/*!
    \fn Kopete::AV::VideoDevice::currentInput()
 */
int VideoDevice::currentInput()
{
    /// @todo implement me
	if(isOpen())
	{
		return m_current_input;
	}
	return 0;
}

/*!
    \fn Kopete::AV::VideoDevice::selectInput(int input)
 */
int VideoDevice::selectInput(int newinput)
{
    /// @todo implement me
	if(m_current_input >= inputs())
		return EXIT_FAILURE;

	if(isOpen())
	{
		switch (m_driver)
		{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
			case VIDEODEV_DRIVER_V4L2:
				if (-1 == ioctl (descriptor, VIDIOC_S_INPUT, &newinput))
				{
					perror ("VIDIOC_S_INPUT");
					return EXIT_FAILURE;
				}
				break;
#endif
			case VIDEODEV_DRIVER_V4L:
				struct video_channel V4L_input;
				V4L_input.channel=newinput;
				V4L_input.norm=4; // Hey, it's plain wrong! It should be input's signal standard!
				if (-1 == ioctl (descriptor, VIDIOCSCHAN, &V4L_input))
				{
					perror ("ioctl (VIDIOCSCHAN)");
					return EXIT_FAILURE;
				}
				break;
#endif
			case VIDEODEV_DRIVER_NONE:
			default:
				break;
		}
		kDebug() << "Selected input " << newinput << " (" << m_input[newinput].name << ")";
		m_current_input = newinput;
		setInputParameters();
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevice::setInputParameters()
 */
int VideoDevice::setInputParameters()
{
    /// @todo implement me
	if( (isOpen()) && (m_current_input < inputs() ) )
	{
		setBrightness( getBrightness() );
		setContrast( getContrast() );
		setSaturation( getSaturation() );
		setWhiteness( getWhiteness() );
		setHue( getHue() );
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/*!
    \fn VideoDevice::startCapturing()
 */
int VideoDevice::startCapturing()
{

	kDebug() << "called.";
	if(isOpen())
	{
		switch (m_io_method)
		{
			case IO_METHOD_NONE: // Device cannot capture frames
				return EXIT_FAILURE;
				break;
			case IO_METHOD_READ: // Nothing to do
				break;
			case IO_METHOD_MMAP:
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
				{
					unsigned int loop;
					for (loop = 0; loop < m_streambuffers; ++loop)
					{
						struct v4l2_buffer buf;
						CLEAR (buf);
						buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
						buf.memory = V4L2_MEMORY_MMAP;
						buf.index  = loop;
						if (-1 == xioctl (VIDIOC_QBUF, &buf))
							return errnoReturn ("VIDIOC_QBUF");
					}
					enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					if (-1 == xioctl (VIDIOC_STREAMON, &type))
						return errnoReturn ("VIDIOC_STREAMON");
				}
#endif
#endif
				break;
			case IO_METHOD_USERPTR:
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
				{
					unsigned int loop;
					for (loop = 0; loop < m_streambuffers; ++loop)
					{
						struct v4l2_buffer buf;
						CLEAR (buf);
						buf.type      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
						buf.memory    = V4L2_MEMORY_USERPTR;
						buf.m.userptr = (unsigned long) m_rawbuffers[loop].start;
						buf.length    = m_rawbuffers[loop].length;
						if (-1 == xioctl (VIDIOC_QBUF, &buf))
							return errnoReturn ("VIDIOC_QBUF");
					}
					enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					if (-1 == xioctl (VIDIOC_STREAMON, &type))
						return errnoReturn ("VIDIOC_STREAMON");
				}
#endif
#endif
				break;
		}

		kDebug() << "exited successfuly.";
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/*!
    \fn VideoDevice::getFrame()
 */
int VideoDevice::getFrame()
{
    /// @todo implement me
	ssize_t bytesread;

#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
	struct v4l2_buffer v4l2buffer;
#endif
#endif
// 	kDebug() << "getFrame() called.";
	if(isOpen())
	{
		switch (m_io_method)
		{
			case IO_METHOD_NONE: // Device cannot capture frames
				return EXIT_FAILURE;
				break;
			case IO_METHOD_READ:
// 				kDebug() << "Using IO_METHOD_READ.File descriptor: " << descriptor << " Buffer address: " << &m_currentbuffer.data[0] << " Size: " << m_currentbuffer.data.size();
				if (m_currentbuffer.data.isEmpty())
					return EXIT_FAILURE;

#ifdef HAVE_LIBV4L2
				bytesread = v4l2_read (descriptor, &m_currentbuffer.data[0], m_currentbuffer.data.size());
#else
				bytesread = read (descriptor, &m_currentbuffer.data[0], m_currentbuffer.data.size());
#endif
				if (-1 == bytesread) // must verify this point with ov511 driver.
				{
					kDebug() << "IO_METHOD_READ failed.";
					switch (errno)
					{
						case EAGAIN:
							return EXIT_FAILURE;
						case EIO: /* Could ignore EIO, see spec. fall through */
						default:
						return errnoReturn ("read");
					}
				}
				if((int)m_currentbuffer.data.size() < bytesread)
				{
					kDebug() << "IO_METHOD_READ returned less bytes (" << bytesread << ") than it was asked for (" << m_currentbuffer.data.size() <<").";
				}
				break;
			case IO_METHOD_MMAP:
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
				CLEAR (v4l2buffer);
				v4l2buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				v4l2buffer.memory = V4L2_MEMORY_MMAP;
				if (-1 == xioctl (VIDIOC_DQBUF, &v4l2buffer))
				{
					kDebug() << full_filename << " MMAPed getFrame failed.";
					switch (errno)
					{
						case EAGAIN:
						{
							kDebug() << full_filename << " MMAPed getFrame failed: EAGAIN. Pointer: ";
							return EXIT_FAILURE;
						}
						case EIO: /* Could ignore EIO, see spec. fall through */
						default:
							return errnoReturn ("VIDIOC_DQBUF");
					}
				}
/*				if (v4l2buffer.index < m_streambuffers)
					return EXIT_FAILURE;*/ //it was an assert()
// kDebug() << "m_rawbuffers[" << v4l2buffer.index << "].start: " << (void *)m_rawbuffers[v4l2buffer.index].start << "   Size: " << m_currentbuffer.data.size();
				if (m_currentbuffer.data.isEmpty() ||
//					v4l2buffer.index < 0 ||  	// is always false: v4l2buffer.index is unsigned
					(uint) m_rawbuffers.size() <= v4l2buffer.index)
					return EXIT_FAILURE;

				memcpy(&m_currentbuffer.data[0], m_rawbuffers[v4l2buffer.index].start, m_currentbuffer.data.size());
				if (-1 == xioctl (VIDIOC_QBUF, &v4l2buffer))
					return errnoReturn ("VIDIOC_QBUF");
#endif
#endif
				break;
			case IO_METHOD_USERPTR:
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
				{
					unsigned int i;
					CLEAR (v4l2buffer);
					v4l2buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					v4l2buffer.memory = V4L2_MEMORY_USERPTR;
					if (-1 == xioctl (VIDIOC_DQBUF, &v4l2buffer))
					{
						switch (errno)
						{
							case EAGAIN:
								return EXIT_FAILURE;
							case EIO: /* Could ignore EIO, see spec. fall through */
							default:
								return errnoReturn ("VIDIOC_DQBUF");
						}
					}
					if ((unsigned int) m_rawbuffers.size() < m_streambuffers)
						return EXIT_FAILURE;
					
					for (i = 0; i < m_streambuffers; ++i)
						if (v4l2buffer.m.userptr == (unsigned long) m_rawbuffers[i].start && v4l2buffer.length == m_rawbuffers[i].length)
							break;
					if (i < m_streambuffers)
						return EXIT_FAILURE;
					if (-1 == xioctl (VIDIOC_QBUF, &v4l2buffer))
					return errnoReturn ("VIDIOC_QBUF");
				}
#endif
#endif
				break;
		}
// 		kDebug() << "exited successfuly.";
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/*!
    \fn VideoDevice::getFrame(imagebuffer *imgbuffer)
 */
int VideoDevice::getFrame(imagebuffer *imgbuffer)
{
	if(imgbuffer)
	{
		getFrame();
		imgbuffer->height      = m_currentbuffer.height;
		imgbuffer->width       = m_currentbuffer.width;
		imgbuffer->pixelformat = m_currentbuffer.pixelformat;
		imgbuffer->data        = m_currentbuffer.data;
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevice::getImage(const QImage *qimage)
 */
int VideoDevice::getImage(QImage *qimage)
{
    /// @todo implement me

	// do NOT delete qimage here, as it is received as a parameter
	if (qimage->width() != width() || qimage->height() != height()) 
		*qimage = QImage(width(), height(), QImage::Format_RGB32);

	if (!m_currentbuffer.data.size())
		{
		//there is no data so if we continue something will try access it (as in bug 161536) and crash kopete
		//perhaps we should look at having the interface reflect when the camera isn't available? as it thinks 
		//it is for some reason, though the data size seems to be an ok check
		return EXIT_FAILURE;
		}

	uchar *bits=qimage->bits();
// kDebug() << "Capturing in " << pixelFormatName(m_currentbuffer.pixelformat);
	switch(m_currentbuffer.pixelformat)
	{
		case PIXELFORMAT_NONE	: break;

// Packed RGB formats
		case PIXELFORMAT_RGB332	:
			{
				int step=0;
				for(int loop=0;loop < qimage->numBytes();loop+=4)
				{
					bits[loop]   = (m_currentbuffer.data[step]>>5<<5)+(m_currentbuffer.data[step]>>5<<2)+(m_currentbuffer.data[step]>>6);
					bits[loop+1] = (m_currentbuffer.data[step]>>2<<5)+(m_currentbuffer.data[step]<<3>>5<<2)+(m_currentbuffer.data[step]<<3>>6);
					bits[loop+2] = (m_currentbuffer.data[step]<<6)+(m_currentbuffer.data[step]<<6>>2)+(m_currentbuffer.data[step]<<6>>4)+(m_currentbuffer.data[step]<<6>>6);
					bits[loop+3] = 255;
					step++;
				}
			}
			break;
		case PIXELFORMAT_RGB444	: break;
		case PIXELFORMAT_RGB555	: break;
		case PIXELFORMAT_RGB565	:
			{
				int step=0;
				for(int loop=0;loop < qimage->numBytes();loop+=4)
				{
					bits[loop]   = (m_currentbuffer.data[step]<<3)+(m_currentbuffer.data[step]<<3>>5);
					bits[loop+1] = ((m_currentbuffer.data[step+1])<<5)|m_currentbuffer.data[step]>>5;
					bits[loop+2] = ((m_currentbuffer.data[step+1])&248)+((m_currentbuffer.data[step+1])>>5);
					bits[loop+3] = 255;
					step+=2;
				}
			}
			break;
		case PIXELFORMAT_RGB555X: break;
		case PIXELFORMAT_RGB565X: break;
		case PIXELFORMAT_BGR24	:
			{
				int step=0;
				for(int loop=0;loop < qimage->numBytes();loop+=4)
				{
					bits[loop]   = m_currentbuffer.data[step+2];
					bits[loop+1] = m_currentbuffer.data[step+1];
					bits[loop+2] = m_currentbuffer.data[step];
					bits[loop+3] = 255;
					step+=3;
				}
			}
			break;
		case PIXELFORMAT_RGB24	:
			{
				int step=0;
				for(int loop=0;loop < qimage->numBytes();loop+=4)
				{
					bits[loop]   = m_currentbuffer.data[step];
					bits[loop+1] = m_currentbuffer.data[step+1];
					bits[loop+2] = m_currentbuffer.data[step+2];
					bits[loop+3] = 255;
					step+=3;
				}
			}
			break;
		case PIXELFORMAT_BGR32	:
			{
				int step=0;
				for(int loop=0;loop < qimage->numBytes();loop+=4)
				{
					bits[loop]   = m_currentbuffer.data[step+2];
					bits[loop+1] = m_currentbuffer.data[step+1];
					bits[loop+2] = m_currentbuffer.data[step];
					bits[loop+3] = m_currentbuffer.data[step+3];
					step+=4;
				}
			}
			break;
		case PIXELFORMAT_RGB32	: memcpy(bits,&m_currentbuffer.data[0], m_currentbuffer.data.size());
			break;

// Bayer RGB format
		case PIXELFORMAT_SBGGR8	:
		{
			unsigned char *d = (unsigned char *) malloc (width() * height() * 3);
			bayer2rgb24(d, &m_currentbuffer.data.first(), width(), height());
			int step=0;
			for(int loop=0;loop < qimage->numBytes();loop+=4)
			{
				bits[loop]   = d[step+2];
				bits[loop+1] = d[step+1];
				bits[loop+2] = d[step];
				bits[loop+3] = 255;
				step+=3;
			}
			free(d);
		}
		break;

// YUV formats
		case PIXELFORMAT_GREY	:
			{
				int step=0;
				for(int loop=0;loop < qimage->numBytes();loop+=4)
				{
					bits[loop]   = m_currentbuffer.data[step];
					bits[loop+1] = m_currentbuffer.data[step];
					bits[loop+2] = m_currentbuffer.data[step];
					bits[loop+3] = 255;
					step++;
				}
			}
			break;
		case PIXELFORMAT_YUYV:
		case PIXELFORMAT_UYVY:
		case PIXELFORMAT_YUV420P:
		case PIXELFORMAT_YUV422P:
		{
			uchar *yptr, *cbptr, *crptr;
			bool halfheight=false;
			bool packed=false;
// Adjust algorythm to specific YUV data arrangements.
			if (m_currentbuffer.pixelformat == PIXELFORMAT_YUV420P)
				halfheight=true;
			if (m_currentbuffer.pixelformat == PIXELFORMAT_YUYV)
			{
				yptr = &m_currentbuffer.data[0];
				cbptr = yptr + 1;
				crptr = yptr + 3;
				packed=true;
			}
			else if (m_currentbuffer.pixelformat == PIXELFORMAT_UYVY)
			{
				cbptr = &m_currentbuffer.data[0];
				yptr = cbptr + 1;
				crptr = cbptr + 2;
				packed=true;
			}
			else
			{
				yptr = &m_currentbuffer.data[0];
				cbptr = yptr + (width()*height());
				crptr = cbptr + (width()*height()/(halfheight ? 4:2));
			}
	
			for(int y=0; y<height(); y++)
			{
// Decode scanline
				for(int x=0; x<width(); x++)
				{
					int c,d,e;

					if (packed)
					{
						c = (yptr[x<<1])-16;
						d = (cbptr[x>>1<<2])-128;
						e = (crptr[x>>1<<2])-128;
					}
					else
					{
						c = (yptr[x])-16;
						d = (cbptr[x>>1])-128;
						e = (crptr[x>>1])-128;
					}
	
					int r = (298 * c           + 409 * e + 128)>>8;
					int g = (298 * c - 100 * d - 208 * e + 128)>>8;
					int b = (298 * c + 516 * d           + 128)>>8;
	
					if (r<0) r=0;   if (r>255) r=255;
					if (g<0) g=0;   if (g>255) g=255;
					if (b<0) b=0;   if (b>255) b=255;
	
					uint *p = (uint*)qimage->scanLine(y)+x;
					*p = qRgba(r,g,b,255);
	
				}
// Jump to next line
				if (packed)
				{
					yptr+=width()*2;
					cbptr+=width()*2;
					crptr+=width()*2;
				}
				else
				{
					yptr+=width();
					if (!halfheight || y&1)
					{
						cbptr+=width()/2;
						crptr+=width()/2;
					}
				}
			}
		}
		break;

// Compressed formats
		case PIXELFORMAT_JPEG	: break;
		case PIXELFORMAT_MPEG	: break;

// Reserved formats
		case PIXELFORMAT_DV	: break;
		case PIXELFORMAT_ET61X251:break;
		case PIXELFORMAT_HI240	: break;
		case PIXELFORMAT_HM12	: break;
		case PIXELFORMAT_MJPEG	: break;
		case PIXELFORMAT_PWC1	: break;
		case PIXELFORMAT_PWC2	: break;
		case PIXELFORMAT_SN9C10X:
		{
			unsigned char *s = new unsigned char [width() * height()];
			unsigned char *d = new unsigned char [width() * height() * 3];
			sonix_decompress_init();
			sonix_decompress(width(), height(), &m_currentbuffer.data.first(), s);
			bayer2rgb24(d, s, width(), height());
			int step=0;
			for(int loop=0;loop < qimage->numBytes();loop+=4)
			{
				bits[loop]   = d[step+2];
				bits[loop+1] = d[step+1];
				bits[loop+2] = d[step];
				bits[loop+3] = 255;
				step+=3;
			}
			delete[] s;
			delete[] d;
		}
		case PIXELFORMAT_WNVA	: break;
		case PIXELFORMAT_YYUV	: break;
	}

// Proccesses image for automatic Brightness/Contrast/Color correction
	if (getAutoBrightnessContrast()||getAutoColorCorrection())
	{
		unsigned long long R=0, G=0, B=0, A=0, global=0;
		int Rmax=0, Gmax=0, Bmax=0, Amax=0, globalmax=0;
		int Rmin=255, Gmin=255, Bmin=255, Amin=255, globalmin=255;
		int Rrange=255, Grange=255, Brange=255;

// Finds minimum and maximum intensity for each color component
		for(unsigned int loop=0;loop < (unsigned int) qimage->numBytes();loop+=4)
		{
			R+=bits[loop];
			G+=bits[loop+1];
			B+=bits[loop+2];
//			A+=bits[loop+3];
			if (bits[loop]   < Rmin) Rmin = bits[loop];
			if (bits[loop+1] < Gmin) Gmin = bits[loop+1];
			if (bits[loop+2] < Bmin) Bmin = bits[loop+2];
//			if (bits[loop+3] < Amin) Amin = bits[loop+3];
			if (bits[loop]   > Rmax) Rmax = bits[loop];
			if (bits[loop+1] > Gmax) Gmax = bits[loop+1];
			if (bits[loop+2] > Bmax) Bmax = bits[loop+2];
//			if (bits[loop+3] > Amax) Amax = bits[loop+3];
		}
		global = R + G + B;
// Finds overall minimum and maximum intensity
		if (Rmin > Gmin) globalmin = Gmin; else globalmin = Rmin; if (Bmin < globalmin) globalmin = Bmin;
		if (Rmax > Gmax) globalmax = Rmax; else globalmax = Gmax; if (Bmax > globalmax) globalmax = Bmax;
// If no color correction should be performed, simply level all the intensities so they're just the same.
// In fact color correction should use the R, G and B variables to detect color deviation and "bump up" the saturation,
// but it's computationally more expensive and the current way returns better results to the user.
		if(!getAutoColorCorrection())
		{
			Rmin = globalmin ; Rmax = globalmax;
			Gmin = globalmin ; Gmax = globalmax;
			Bmin = globalmin ; Bmax = globalmax;
//			Amin = globalmin ; Amax = globalmax;
		}
// Calculates ranges and prevent a division by zero later on.
			Rrange = Rmax - Rmin; if (Rrange == 0) Rrange = 255;
			Grange = Gmax - Gmin; if (Grange == 0) Grange = 255;
			Brange = Bmax - Bmin; if (Brange == 0) Brange = 255;
//			Arange = Amax - Amin; if (Arange == 0) Arange = 255;

		kDebug() << " R: " << R << " G: " << G << " B: " << B << " A: " << A << " global: " << global <<
			" Rmin: " << Rmin << " Gmin: " << Gmin << " Bmin: " << Bmin << " Amin: " << Amin << " globalmin: " << globalmin <<
			" Rmax: " << Rmax << " Gmax: " << Gmax << " Bmax: " << Bmax << " Amax: " << Amax << " globalmax: " << globalmax ;

		for(unsigned int loop=0;loop < (unsigned int) qimage->numBytes();loop+=4)
		{
			bits[loop]   = (bits[loop]   - Rmin) * 255 / (Rrange);
			bits[loop+1] = (bits[loop+1] - Gmin) * 255 / (Grange);
			bits[loop+2] = (bits[loop+2] - Bmin) * 255 / (Brange);
//			bits[loop+3] = (bits[loop+3] - Amin) * 255 / (Arange);
		}
	}
	return EXIT_SUCCESS;
}

/*!
    \fn VideoDevice::stopCapturing()
 */
int VideoDevice::stopCapturing()
{
    /// @todo implement me
	kDebug() << "called.";
	if(isOpen())
	{
		switch (m_io_method)
		{
			case IO_METHOD_NONE: // Device cannot capture frames
				return EXIT_FAILURE;
				break;
			case IO_METHOD_READ: // Nothing to do
				break;
			case IO_METHOD_MMAP:
			case IO_METHOD_USERPTR:
#ifdef V4L2_CAP_VIDEO_CAPTURE
				{
					enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					if (-1 == xioctl (VIDIOC_STREAMOFF, &type))
						return errnoReturn ("VIDIOC_STREAMOFF");

					if (m_io_method == IO_METHOD_MMAP)
					{
						unsigned int loop;
						for (loop = 0; loop < m_streambuffers; ++loop)
						{
#ifdef HAVE_LIBV4L2
							if (v4l2_munmap(m_rawbuffers[loop].start,m_rawbuffers[loop].length) != 0)
#else
							if (munmap(m_rawbuffers[loop].start,m_rawbuffers[loop].length) != 0)
#endif
							{
								kDebug() << "unable to munmap.";
							}
						}
					}
				}
#endif
				break;
		}
		kDebug() << "exited successfuly.";
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}


/*!
    \fn VideoDevice::close()
 */
int VideoDevice::close()
{
    /// @todo implement me
	kDebug() << " called.";
	if(isOpen())
	{
		kDebug() << " Device is open. Trying to properly shutdown the device.";
		stopCapturing();
#ifdef HAVE_LIBV4L2
		int ret = ::v4l2_close(descriptor);
#else
		int ret = ::close(descriptor);
#endif
		kDebug() << "::close() returns " << ret;
	}
	descriptor = -1;
	return EXIT_SUCCESS;
}

float VideoDevice::getBrightness()
{
  if (m_current_input < m_input.size() )
	return m_input[m_current_input].getBrightness();
  else
	return 0;
}

float VideoDevice::setBrightness(float brightness)
{
	kDebug() << "(" << brightness << ") called.";
	m_input[m_current_input].setBrightness(brightness); // Just to check bounds

	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			{
				struct v4l2_queryctrl queryctrl;
				struct v4l2_control control;

				CLEAR (queryctrl);
				queryctrl.id = V4L2_CID_BRIGHTNESS;

				if (-1 == xioctl (VIDIOC_QUERYCTRL, &queryctrl))
				{
					if (errno != EINVAL)
					{
						kDebug() <<  "VIDIOC_QUERYCTRL failed (" << errno << ").";
					} else
					{
						kDebug() << "Device doesn't support the Brightness control.";
					}
				} else
				if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				{
					kDebug() << "Brightness control is disabled.";
				} else
				{
					CLEAR (control);
					control.id = V4L2_CID_BRIGHTNESS;
					control.value = (__s32)((queryctrl.maximum - queryctrl.minimum)*getBrightness());
					control.value += queryctrl.minimum;

					if (-1 == xioctl (VIDIOC_S_CTRL, &control))
					{
						kDebug() <<  "VIDIOC_S_CTRL failed (" << errno << ").";
					}
				}
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			{
				struct video_picture V4L_picture;
				if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
				{
					kDebug() << "VIDIOCGPICT failed (" << errno << ").";
				}
				V4L_picture.brightness = uint(65535 * getBrightness());
				if(-1 == xioctl(VIDIOCSPICT,&V4L_picture))
				{
					kDebug() << "Device seems to not support adjusting image brightness. Fallback to it is not yet implemented.";
				}
			}
			break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return getBrightness();
}

float VideoDevice::getContrast()
{
  if (m_current_input < m_input.size() )
	return m_input[m_current_input].getContrast();
  else
	return 0;
}

float VideoDevice::setContrast(float contrast)
{
	kDebug() << "(" << contrast << ") called.";
	m_input[m_current_input].setContrast(contrast); // Just to check bounds

	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			{
				struct v4l2_queryctrl queryctrl;
				struct v4l2_control control;

				CLEAR (queryctrl);
				queryctrl.id = V4L2_CID_CONTRAST;

				if (-1 == xioctl (VIDIOC_QUERYCTRL, &queryctrl))
				{
					if (errno != EINVAL)
					{
						kDebug() <<  "VIDIOC_QUERYCTRL failed (" << errno << ").";
					} else
					{
						kDebug() << "Device doesn't support the Contrast control.";
					}
				} else
				if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				{
					kDebug() << "Contrast control is disabled.";
				} else
				{
					CLEAR (control);
					control.id = V4L2_CID_CONTRAST;
					control.value = (__s32)((queryctrl.maximum - queryctrl.minimum)*getContrast());
					control.value += queryctrl.minimum;

					if (-1 == xioctl (VIDIOC_S_CTRL, &control))
					{
						kDebug() <<  "VIDIOC_S_CTRL failed (" << errno << ").";
					}
				}
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			{
				struct video_picture V4L_picture;
				if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
				{
					kDebug() << "VIDIOCGPICT failed (" << errno << ").";
				}
				V4L_picture.contrast = uint(65535*getContrast());
				if(-1 == xioctl(VIDIOCSPICT,&V4L_picture)) 
				{
					kDebug() << "Device seems to not support adjusting image contrast. Fallback to it is not yet implemented.";
				}
			}
		break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return getContrast();
}

float VideoDevice::getSaturation()
{
  if (m_current_input < m_input.size() )
	return m_input[m_current_input].getSaturation();
  else
	return 0;
}

float VideoDevice::setSaturation(float saturation)
{
	kDebug() << "(" << saturation << ") called.";
	m_input[m_current_input].setSaturation(saturation); // Just to check bounds

	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			{
				struct v4l2_queryctrl queryctrl;
				struct v4l2_control control;

				CLEAR (queryctrl);
				queryctrl.id = V4L2_CID_SATURATION;

				if (-1 == xioctl (VIDIOC_QUERYCTRL, &queryctrl))
				{
					if (errno != EINVAL)
					{
						kDebug() <<  "VIDIOC_QUERYCTRL failed (" << errno << ").";
					} else
					{
						kDebug() << "Device doesn't support the Saturation control.";
					}
				} else
				if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				{
					kDebug() << "Saturation control is disabled.";
				} else
				{
					CLEAR (control);
					control.id = V4L2_CID_SATURATION;
					control.value = (__s32)((queryctrl.maximum - queryctrl.minimum)*getSaturation());
					control.value += queryctrl.minimum;

					if (-1 == xioctl (VIDIOC_S_CTRL, &control))
					{
						kDebug() <<  "VIDIOC_S_CTRL failed (" << errno << ").";
					}
				}
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			{
				struct video_picture V4L_picture;
				if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
				{
					kDebug() << "VIDIOCGPICT failed (" << errno << ").";
				}
				V4L_picture.colour = uint(65535*getSaturation());
				if(-1 == xioctl(VIDIOCSPICT,&V4L_picture))
				{
					kDebug() << "Device seems to not support adjusting image saturation. Fallback to it is not yet implemented.";
				}
			}
		break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return getSaturation();
}

float VideoDevice::getWhiteness()
{
  if (m_current_input < m_input.size() )
	return m_input[m_current_input].getWhiteness();
  else
	return 0;
}

float VideoDevice::setWhiteness(float whiteness)
{
	kDebug() << "(" << whiteness << ") called.";
	m_input[m_current_input].setWhiteness(whiteness); // Just to check bounds

	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			{
				struct v4l2_queryctrl queryctrl;
				struct v4l2_control control;

				CLEAR (queryctrl);
				queryctrl.id = V4L2_CID_WHITENESS;

				if (-1 == xioctl (VIDIOC_QUERYCTRL, &queryctrl))
				{
					if (errno != EINVAL)
					{
						kDebug() <<  "VIDIOC_QUERYCTRL failed (" << errno << ").";
					} else
					{
						kDebug() << "Device doesn't support the Whiteness control.";
					}
				} else
				if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				{
					kDebug() << "Whiteness control is disabled.";
				} else
				{
					CLEAR (control);
					control.id = V4L2_CID_WHITENESS;
					control.value = (__s32)((queryctrl.maximum - queryctrl.minimum)*getWhiteness());
					control.value += queryctrl.minimum;

					if (-1 == xioctl (VIDIOC_S_CTRL, &control))
					{
						kDebug() <<  "VIDIOC_S_CTRL failed (" << errno << ").";
					}
				}
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			{
				struct video_picture V4L_picture;
				if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
				{
					kDebug() << "VIDIOCGPICT failed (" << errno << ").";
				}
				V4L_picture.whiteness = uint(65535*getWhiteness());
				if(-1 == xioctl(VIDIOCSPICT,&V4L_picture))
				{
					kDebug() << "Device seems to not support adjusting white level. Fallback to it is not yet implemented.";
				}
			}
		break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return getWhiteness();
}

float VideoDevice::getHue()
{
  if (m_current_input < m_input.size() )
	return m_input[m_current_input].getHue();
  else
	return 0;
}

float VideoDevice::setHue(float hue)
{
	kDebug() << "(" << hue << ") called.";
	m_input[m_current_input].setHue(hue); // Just to check bounds

	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			{
				struct v4l2_queryctrl queryctrl;
				struct v4l2_control control;

				CLEAR (queryctrl);
				queryctrl.id = V4L2_CID_HUE;

				if (-1 == xioctl (VIDIOC_QUERYCTRL, &queryctrl))
				{
					if (errno != EINVAL)
					{
						kDebug() <<  "VIDIOC_QUERYCTRL failed (" << errno << ").";
					} else
					{
						kDebug() << "Device doesn't support the Hue control.";
					}
				} else
				if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				{
					kDebug() << "Hue control is disabled.";
				} else
				{
					CLEAR (control);
					control.id = V4L2_CID_HUE;
					control.value = (__s32)((queryctrl.maximum - queryctrl.minimum)*getHue());
					control.value += queryctrl.minimum;

					if (-1 == xioctl (VIDIOC_S_CTRL, &control))
					{
						kDebug() <<  "VIDIOC_S_CTRL failed (" << errno << ").";
					}
				}
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			{
				struct video_picture V4L_picture;
				if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
				{
					kDebug() << "VIDIOCGPICT failed (" << errno << ").";
				}
				V4L_picture.hue = uint(65535*getHue());
				if(-1 == xioctl(VIDIOCSPICT,&V4L_picture))
				{
					kDebug() << "Device seems to not support adjusting image hue. Fallback to it is not yet implemented.";
				}
			}
		break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return getHue();
}


bool VideoDevice::getAutoBrightnessContrast()
{
  if (m_current_input < m_input.size() )
	return m_input[m_current_input].getAutoBrightnessContrast();
  else
	return false;
}

bool VideoDevice::setAutoBrightnessContrast(bool brightnesscontrast)
{
	kDebug() << "VideoDevice::setAutoBrightnessContrast(" << brightnesscontrast << ") called.";
	if (m_current_input < m_input.size() ) 
	  {
		m_input[m_current_input].setAutoBrightnessContrast(brightnesscontrast);
		return m_input[m_current_input].getAutoBrightnessContrast();
	  }
	else
	  return false;
   
}

bool VideoDevice::getAutoColorCorrection()
{
  if (m_current_input < m_input.size() )
	return m_input[m_current_input].getAutoColorCorrection();
  else
	return false;
}

bool VideoDevice::setAutoColorCorrection(bool colorcorrection)
{
	kDebug() << "VideoDevice::setAutoColorCorrection(" << colorcorrection << ") called.";
	if (m_current_input < m_input.size() )
	  {
		m_input[m_current_input].setAutoColorCorrection(colorcorrection);
		return m_input[m_current_input].getAutoColorCorrection();
	  }
	else
	  return false;
}

bool VideoDevice::getImageAsMirror()
{
  if (m_current_input < m_input.size() )
	return m_input[m_current_input].getImageAsMirror();
  else
	return false;
}

bool VideoDevice::setImageAsMirror(bool imageasmirror)
{
	kDebug() << "VideoDevice::setImageAsMirror(" << imageasmirror << ") called.";
	if (m_current_input < m_input.size() ) 
	  {
		m_input[m_current_input].setImageAsMirror(imageasmirror);
		return m_input[m_current_input].getImageAsMirror();
	  }
	else
	  return false;
}

pixel_format VideoDevice::pixelFormatForPalette( int palette )
{
	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			switch(palette)
			{
				case 0 				: return PIXELFORMAT_NONE;	break;

// Packed RGB formats
				case V4L2_PIX_FMT_RGB332	: return PIXELFORMAT_RGB332;	break;
#if defined( V4L2_PIX_FMT_RGB444 )
				case V4L2_PIX_FMT_RGB444	: return PIXELFORMAT_RGB444;	break;
#endif
				case V4L2_PIX_FMT_RGB555	: return PIXELFORMAT_RGB555;	break;
				case V4L2_PIX_FMT_RGB565	: return PIXELFORMAT_RGB565;	break;
				case V4L2_PIX_FMT_RGB555X	: return PIXELFORMAT_RGB555X;	break;
				case V4L2_PIX_FMT_RGB565X	: return PIXELFORMAT_RGB565X;	break;
				case V4L2_PIX_FMT_BGR24		: return PIXELFORMAT_BGR24;	break;
				case V4L2_PIX_FMT_RGB24		: return PIXELFORMAT_RGB24;	break;
				case V4L2_PIX_FMT_BGR32		: return PIXELFORMAT_BGR32;	break;
				case V4L2_PIX_FMT_RGB32		: return PIXELFORMAT_RGB32;	break;

// Bayer RGB format
				case V4L2_PIX_FMT_SBGGR8	: return PIXELFORMAT_SBGGR8;	break;

// YUV formats
				case V4L2_PIX_FMT_GREY		: return PIXELFORMAT_GREY;	break;
				case V4L2_PIX_FMT_YUYV		: return PIXELFORMAT_YUYV;	break;
				case V4L2_PIX_FMT_UYVY		: return PIXELFORMAT_UYVY;	break;
				case V4L2_PIX_FMT_YUV420	: return PIXELFORMAT_YUV420P;	break;
				case V4L2_PIX_FMT_YUV422P	: return PIXELFORMAT_YUV422P;	break;

// Compressed formats
				case V4L2_PIX_FMT_JPEG		: return PIXELFORMAT_JPEG;	break;
				case V4L2_PIX_FMT_MPEG		: return PIXELFORMAT_MPEG;	break;

// Reserved formats
				case V4L2_PIX_FMT_DV		: return PIXELFORMAT_DV;	break;
				case V4L2_PIX_FMT_ET61X251	: return PIXELFORMAT_ET61X251;	break;
				case V4L2_PIX_FMT_HI240		: return PIXELFORMAT_HI240;	break;
#if defined( V4L2_PIX_FMT_HM12 )
				case V4L2_PIX_FMT_HM12		: return PIXELFORMAT_HM12;	break;
#endif
				case V4L2_PIX_FMT_MJPEG		: return PIXELFORMAT_MJPEG;	break;
				case V4L2_PIX_FMT_PWC1		: return PIXELFORMAT_PWC1;	break;
				case V4L2_PIX_FMT_PWC2		: return PIXELFORMAT_PWC2;	break;
				case V4L2_PIX_FMT_SN9C10X	: return PIXELFORMAT_SN9C10X;	break;
				case V4L2_PIX_FMT_WNVA		: return PIXELFORMAT_WNVA;	break;
				case V4L2_PIX_FMT_YYUV		: return PIXELFORMAT_YYUV;	break;
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			switch(palette)
			{
				case 0				: return PIXELFORMAT_NONE;	break;
				case VIDEO_PALETTE_GREY		: return PIXELFORMAT_GREY;	break;
				case VIDEO_PALETTE_HI240	: return PIXELFORMAT_RGB332;	break;
				case VIDEO_PALETTE_RGB555	: return PIXELFORMAT_RGB555;	break;
				case VIDEO_PALETTE_RGB565	: return PIXELFORMAT_RGB565;	break;
				case VIDEO_PALETTE_RGB24	: return PIXELFORMAT_RGB24;	break;
				case VIDEO_PALETTE_RGB32	: return PIXELFORMAT_RGB32;	break;
				case VIDEO_PALETTE_YUYV		: return PIXELFORMAT_YUYV;	break;
				case VIDEO_PALETTE_UYVY		: return PIXELFORMAT_UYVY;	break;
				case VIDEO_PALETTE_YUV420	:
				case VIDEO_PALETTE_YUV420P	: return PIXELFORMAT_YUV420P;	break;
				case VIDEO_PALETTE_YUV422P	: return PIXELFORMAT_YUV422P;	break;
			}
			break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			return PIXELFORMAT_NONE;	break;
	}
	return PIXELFORMAT_NONE;
}

int VideoDevice::pixelFormatCode(pixel_format pixelformat)
{
	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			switch(pixelformat)
			{
				case PIXELFORMAT_NONE	: return 0;			break;

// Packed RGB formats
				case PIXELFORMAT_RGB332	: return V4L2_PIX_FMT_RGB332;	break;
#if defined( V4L2_PIX_FMT_RGB444 )
				case PIXELFORMAT_RGB444	: return V4L2_PIX_FMT_RGB444;	break;
#endif
				case PIXELFORMAT_RGB555	: return V4L2_PIX_FMT_RGB555;	break;
				case PIXELFORMAT_RGB565	: return V4L2_PIX_FMT_RGB565;	break;
				case PIXELFORMAT_RGB555X: return V4L2_PIX_FMT_RGB555X;	break;
				case PIXELFORMAT_RGB565X: return V4L2_PIX_FMT_RGB565X;	break;
				case PIXELFORMAT_BGR24	: return V4L2_PIX_FMT_BGR24;	break;
				case PIXELFORMAT_RGB24	: return V4L2_PIX_FMT_RGB24;	break;
				case PIXELFORMAT_BGR32	: return V4L2_PIX_FMT_BGR32;	break;
				case PIXELFORMAT_RGB32	: return V4L2_PIX_FMT_RGB32;	break;

// Bayer RGB format
				case PIXELFORMAT_SBGGR8	: return V4L2_PIX_FMT_SBGGR8;	break;

// YUV formats
				case PIXELFORMAT_GREY	: return V4L2_PIX_FMT_GREY;	break;
				case PIXELFORMAT_YUYV	: return V4L2_PIX_FMT_YUYV;	break;
				case PIXELFORMAT_UYVY	: return V4L2_PIX_FMT_UYVY;	break;
				case PIXELFORMAT_YUV420P: return V4L2_PIX_FMT_YUV420;	break;
				case PIXELFORMAT_YUV422P: return V4L2_PIX_FMT_YUV422P;	break;

// Compressed formats
				case PIXELFORMAT_JPEG	: return V4L2_PIX_FMT_JPEG;	break;
				case PIXELFORMAT_MPEG	: return V4L2_PIX_FMT_MPEG;	break;

// Reserved formats
				case PIXELFORMAT_DV	: return V4L2_PIX_FMT_DV;	break;
				case PIXELFORMAT_ET61X251:return V4L2_PIX_FMT_ET61X251;break;
				case PIXELFORMAT_HI240	: return V4L2_PIX_FMT_HI240;	break;
#if defined( V4L2_PIX_FMT_HM12 )
				case PIXELFORMAT_HM12	: return V4L2_PIX_FMT_HM12;	break;
#endif
				case PIXELFORMAT_MJPEG	: return V4L2_PIX_FMT_MJPEG;	break;
				case PIXELFORMAT_PWC1	: return V4L2_PIX_FMT_PWC1;	break;
				case PIXELFORMAT_PWC2	: return V4L2_PIX_FMT_PWC2;	break;
				case PIXELFORMAT_SN9C10X: return V4L2_PIX_FMT_SN9C10X;	break;
				case PIXELFORMAT_WNVA	: return V4L2_PIX_FMT_WNVA;	break;
				case PIXELFORMAT_YYUV	: return V4L2_PIX_FMT_YYUV;	break;
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			switch(pixelformat)
			{
				case PIXELFORMAT_NONE	: return 0;			break;

// Packed RGB formats
				case PIXELFORMAT_RGB332	: return VIDEO_PALETTE_HI240;	break;
				case PIXELFORMAT_RGB444	: return 0;			break;
				case PIXELFORMAT_RGB555	: return VIDEO_PALETTE_RGB555;	break;
				case PIXELFORMAT_RGB565	: return VIDEO_PALETTE_RGB565;	break;
				case PIXELFORMAT_RGB555X: return 0;			break;
				case PIXELFORMAT_RGB565X: return 0;			break;
				case PIXELFORMAT_BGR24	: return 0;			break;
				case PIXELFORMAT_RGB24	: return VIDEO_PALETTE_RGB24;	break;
				case PIXELFORMAT_BGR32	: return 0;			break;
				case PIXELFORMAT_RGB32	: return VIDEO_PALETTE_RGB32;	break;

// Bayer RGB format
				case PIXELFORMAT_SBGGR8	: return 0;			break;

// YUV formats
				case PIXELFORMAT_GREY	: return VIDEO_PALETTE_GREY;	break;
				case PIXELFORMAT_YUYV	: return VIDEO_PALETTE_YUYV;	break;
				case PIXELFORMAT_UYVY	: return VIDEO_PALETTE_UYVY;	break;
				case PIXELFORMAT_YUV420P: return VIDEO_PALETTE_YUV420;	break;
				case PIXELFORMAT_YUV422P: return VIDEO_PALETTE_YUV422P;	break;

// Compressed formats
				case PIXELFORMAT_JPEG	: return 0;			break;
				case PIXELFORMAT_MPEG	: return 0;			break;

// Reserved formats
				case PIXELFORMAT_DV	: return 0;			break;
				case PIXELFORMAT_ET61X251:return 0;			break;
				case PIXELFORMAT_HI240	: return VIDEO_PALETTE_HI240;	break;
				case PIXELFORMAT_HM12	: return 0;			break;
				case PIXELFORMAT_MJPEG	: return 0;			break;
				case PIXELFORMAT_PWC1	: return 0;			break;
				case PIXELFORMAT_PWC2	: return 0;			break;
				case PIXELFORMAT_SN9C10X: return 0;			break;
				case PIXELFORMAT_WNVA	: return 0;			break;
				case PIXELFORMAT_YYUV	: return 0;			break;
			}
			break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			return PIXELFORMAT_NONE;	break;
	}
	return PIXELFORMAT_NONE;
}

int VideoDevice::pixelFormatDepth(pixel_format pixelformat)
{
	switch(pixelformat)
	{
		case PIXELFORMAT_NONE	: return 0;	break;

// Packed RGB formats
		case PIXELFORMAT_RGB332	: return 8;	break;
		case PIXELFORMAT_RGB444	: return 16;	break;
		case PIXELFORMAT_RGB555	: return 16;	break;
		case PIXELFORMAT_RGB565	: return 16;	break;
		case PIXELFORMAT_RGB555X: return 16;	break;
		case PIXELFORMAT_RGB565X: return 16;	break;
		case PIXELFORMAT_BGR24	: return 24;	break;
		case PIXELFORMAT_RGB24	: return 24;	break;
		case PIXELFORMAT_BGR32	: return 32;	break;
		case PIXELFORMAT_RGB32	: return 32;	break;

// Bayer RGB format
		case PIXELFORMAT_SBGGR8	: return 0;	break;

// YUV formats
		case PIXELFORMAT_GREY	: return 8;	break;
		case PIXELFORMAT_YUYV	: return 16;	break;
		case PIXELFORMAT_UYVY	: return 16;	break;
		case PIXELFORMAT_YUV420P: return 16;	break;
		case PIXELFORMAT_YUV422P: return 16;	break;

// Compressed formats
		case PIXELFORMAT_JPEG	: return 0;	break;
		case PIXELFORMAT_MPEG	: return 0;	break;

// Reserved formats
		case PIXELFORMAT_DV	: return 0;	break;
		case PIXELFORMAT_ET61X251:return 0;	break;
		case PIXELFORMAT_HI240	: return 8;	break;
		case PIXELFORMAT_HM12	: return 0;	break;
		case PIXELFORMAT_MJPEG	: return 0;	break;
		case PIXELFORMAT_PWC1	: return 0;	break;
		case PIXELFORMAT_PWC2	: return 0;	break;
		case PIXELFORMAT_SN9C10X: return 0;	break;
		case PIXELFORMAT_WNVA	: return 0;	break;
		case PIXELFORMAT_YYUV	: return 0;	break;
	}
	return 0;
}

QString VideoDevice::pixelFormatName(pixel_format pixelformat)
{
	QString returnvalue;
	returnvalue = "None";
	switch(pixelformat)
	{
		case PIXELFORMAT_NONE	: returnvalue = "None";			break;

// Packed RGB formats
		case PIXELFORMAT_RGB332	: returnvalue = "8-bit RGB332";		break;
		case PIXELFORMAT_RGB444	: returnvalue = "8-bit RGB444";		break;
		case PIXELFORMAT_RGB555	: returnvalue = "16-bit RGB555";	break;
		case PIXELFORMAT_RGB565	: returnvalue = "16-bit RGB565";	break;
		case PIXELFORMAT_RGB555X: returnvalue = "16-bit RGB555X";	break;
		case PIXELFORMAT_RGB565X: returnvalue = "16-bit RGB565X";	break;
		case PIXELFORMAT_BGR24	: returnvalue = "24-bit BGR24";		break;
		case PIXELFORMAT_RGB24	: returnvalue = "24-bit RGB24";		break;
		case PIXELFORMAT_BGR32	: returnvalue = "32-bit BGR32";		break;
		case PIXELFORMAT_RGB32	: returnvalue = "32-bit RGB32";		break;

// Bayer RGB format
		case PIXELFORMAT_SBGGR8	: returnvalue = "Bayer RGB format";	break;

// YUV formats
		case PIXELFORMAT_GREY	: returnvalue = "8-bit Grayscale";	break;
		case PIXELFORMAT_YUYV	: returnvalue = "Packed YUV 4:2:2";	break;
		case PIXELFORMAT_UYVY	: returnvalue = "Packed YVU 4:2:2";	break;
		case PIXELFORMAT_YUV420P: returnvalue = "Planar YUV 4:2:0";	break;
		case PIXELFORMAT_YUV422P: returnvalue = "Planar YUV 4:2:2";	break;


// Compressed formats
		case PIXELFORMAT_JPEG	: returnvalue = "JPEG image";		break;
		case PIXELFORMAT_MPEG	: returnvalue = "MPEG stream";		break;

// Reserved formats
		case PIXELFORMAT_DV	: returnvalue = "DV (unknown)";		break;
		case PIXELFORMAT_ET61X251:returnvalue = "ET61X251";		break;
		case PIXELFORMAT_HI240	: returnvalue = "8-bit HI240 (RGB332)";	break;
		case PIXELFORMAT_HM12	: returnvalue = "Packed YUV 4:2:2";	break;
		case PIXELFORMAT_MJPEG	: returnvalue = "8-bit Grayscale";	break;
		case PIXELFORMAT_PWC1	: returnvalue = "PWC1";			break;
		case PIXELFORMAT_PWC2	: returnvalue = "PWC2";			break;
		case PIXELFORMAT_SN9C10X: returnvalue = "SN9C102";		break;
		case PIXELFORMAT_WNVA	: returnvalue = "Winnov Videum";	break;
		case PIXELFORMAT_YYUV	: returnvalue = "YYUV (unknown)";	break;
	}
	return returnvalue;
}

QString VideoDevice::pixelFormatName(int pixelformat)
{
	QString returnvalue;
	returnvalue = "None";
	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			switch(pixelformat)
			{
				case 0				: returnvalue = pixelFormatName(PIXELFORMAT_NONE);	break;

// Packed RGB formats
				case V4L2_PIX_FMT_RGB332	: returnvalue = pixelFormatName(PIXELFORMAT_RGB332);	break;
#if defined( V4L2_PIX_FMT_RGB444 )
				case V4L2_PIX_FMT_RGB444	: returnvalue = pixelFormatName(PIXELFORMAT_RGB444);	break;
#endif
				case V4L2_PIX_FMT_RGB555	: returnvalue = pixelFormatName(PIXELFORMAT_RGB555);	break;
				case V4L2_PIX_FMT_RGB565	: returnvalue = pixelFormatName(PIXELFORMAT_RGB565);	break;
				case V4L2_PIX_FMT_RGB555X	: returnvalue = pixelFormatName(PIXELFORMAT_RGB555X);	break;
				case V4L2_PIX_FMT_RGB565X	: returnvalue = pixelFormatName(PIXELFORMAT_RGB565X);	break;
				case V4L2_PIX_FMT_BGR24		: returnvalue = pixelFormatName(PIXELFORMAT_BGR24);	break;
				case V4L2_PIX_FMT_RGB24		: returnvalue = pixelFormatName(PIXELFORMAT_RGB24);	break;
				case V4L2_PIX_FMT_BGR32		: returnvalue = pixelFormatName(PIXELFORMAT_BGR32);	break;
				case V4L2_PIX_FMT_RGB32		: returnvalue = pixelFormatName(PIXELFORMAT_RGB32);	break;

// Bayer RGB format
				case V4L2_PIX_FMT_SBGGR8	: returnvalue = pixelFormatName(PIXELFORMAT_SBGGR8);	break;

// YUV formats
				case V4L2_PIX_FMT_GREY		: returnvalue = pixelFormatName(PIXELFORMAT_GREY);	break;
				case V4L2_PIX_FMT_YUYV		: returnvalue = pixelFormatName(PIXELFORMAT_YUYV);	break;
				case V4L2_PIX_FMT_UYVY		: returnvalue = pixelFormatName(PIXELFORMAT_UYVY);	break;
				case V4L2_PIX_FMT_YUV420	: returnvalue = pixelFormatName(PIXELFORMAT_YUV420P);	break;
				case V4L2_PIX_FMT_YUV422P	: returnvalue = pixelFormatName(PIXELFORMAT_YUV422P);	break;

// Compressed formats
				case V4L2_PIX_FMT_JPEG		: returnvalue = pixelFormatName(PIXELFORMAT_JPEG);	break;
				case V4L2_PIX_FMT_MPEG		: returnvalue = pixelFormatName(PIXELFORMAT_MPEG);	break;

// Reserved formats
				case V4L2_PIX_FMT_DV		: returnvalue = pixelFormatName(PIXELFORMAT_DV);	break;
				case V4L2_PIX_FMT_ET61X251	: returnvalue = pixelFormatName(PIXELFORMAT_ET61X251);	break;
				case V4L2_PIX_FMT_HI240		: returnvalue = pixelFormatName(PIXELFORMAT_HI240);	break;
#if defined( V4L2_PIX_FMT_HM12 )
				case V4L2_PIX_FMT_HM12		: returnvalue = pixelFormatName(PIXELFORMAT_HM12);	break;
#endif
				case V4L2_PIX_FMT_MJPEG		: returnvalue = pixelFormatName(PIXELFORMAT_MJPEG);	break;
				case V4L2_PIX_FMT_PWC1		: returnvalue = pixelFormatName(PIXELFORMAT_PWC1);	break;
				case V4L2_PIX_FMT_PWC2		: returnvalue = pixelFormatName(PIXELFORMAT_PWC2);	break;
				case V4L2_PIX_FMT_SN9C10X	: returnvalue = pixelFormatName(PIXELFORMAT_SN9C10X);	break;
				case V4L2_PIX_FMT_WNVA		: returnvalue = pixelFormatName(PIXELFORMAT_WNVA);	break;
				case V4L2_PIX_FMT_YYUV		: returnvalue = pixelFormatName(PIXELFORMAT_YYUV);	break;
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			switch(pixelformat)
			{
				case VIDEO_PALETTE_GREY		: returnvalue = pixelFormatName(PIXELFORMAT_GREY);	break;
				case VIDEO_PALETTE_HI240	: returnvalue = pixelFormatName(PIXELFORMAT_RGB332);	break;
				case VIDEO_PALETTE_RGB555	: returnvalue = pixelFormatName(PIXELFORMAT_RGB555);	break;
				case VIDEO_PALETTE_RGB565	: returnvalue = pixelFormatName(PIXELFORMAT_RGB565);	break;
				case VIDEO_PALETTE_RGB24	: returnvalue = pixelFormatName(PIXELFORMAT_RGB24);	break;
				case VIDEO_PALETTE_RGB32	: returnvalue = pixelFormatName(PIXELFORMAT_RGB32);	break;
				case VIDEO_PALETTE_YUYV		: returnvalue = pixelFormatName(PIXELFORMAT_YUYV);	break;
				case VIDEO_PALETTE_UYVY		: returnvalue = pixelFormatName(PIXELFORMAT_UYVY);	break;
				case VIDEO_PALETTE_YUV420	:
				case VIDEO_PALETTE_YUV420P	: returnvalue = pixelFormatName(PIXELFORMAT_YUV420P);	break;
				case VIDEO_PALETTE_YUV422P	: returnvalue = pixelFormatName(PIXELFORMAT_YUV422P);	break;
			}
			break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return returnvalue;
}

int VideoDevice::detectPixelFormats()
{
			int err = 0;
	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			fmtdesc.index = 0;
			fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			while ( err == 0 )
			{
				if (-1 == xioctl (VIDIOC_ENUM_FMT, &fmtdesc))
//				if (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) < 0 )
				{
					perror("VIDIOC_ENUM_FMT");
					err = errno;
				}
				else
				{	
					kDebug () << fmtdesc.pixelformat << "  " << pixelFormatName(fmtdesc.pixelformat); // Need a cleanup. PixelFormatForPalette is a really bad name
					fmtdesc.index++;
				}
			}
//			break;
#endif
		case VIDEODEV_DRIVER_V4L:
// TODO: THis thing can be used to detec what pixel formats are supported in a API-independent way, but V4L2 has VIDIOC_ENUM_PIXFMT.
// The correct thing to do is to isolate these calls and do a proper implementation for V4L and another for V4L2 when this thing will be migrated to a plugin architecture.

// Packed RGB formats
			kDebug() << "Supported pixel formats:";
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB332))	{ kDebug() << pixelFormatName(PIXELFORMAT_RGB332); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB444))	{ kDebug() << pixelFormatName(PIXELFORMAT_RGB444); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB555))	{ kDebug() << pixelFormatName(PIXELFORMAT_RGB555); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB565))	{ kDebug() << pixelFormatName(PIXELFORMAT_RGB565); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB555X))	{ kDebug() << pixelFormatName(PIXELFORMAT_RGB555X); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB565X))	{ kDebug() << pixelFormatName(PIXELFORMAT_RGB565X); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_BGR24))	{ kDebug() << pixelFormatName(PIXELFORMAT_BGR24); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB24))	{ kDebug() << pixelFormatName(PIXELFORMAT_RGB24); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_BGR32))	{ kDebug() << pixelFormatName(PIXELFORMAT_BGR32); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB32))	{ kDebug() << pixelFormatName(PIXELFORMAT_RGB32); }

// Bayer RGB format
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_SBGGR8))	{ kDebug() << pixelFormatName(PIXELFORMAT_SBGGR8); }

// YUV formats
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_GREY))	{ kDebug() << pixelFormatName(PIXELFORMAT_GREY); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_YUYV))	{ kDebug() << pixelFormatName(PIXELFORMAT_YUYV); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_UYVY))	{ kDebug() << pixelFormatName(PIXELFORMAT_UYVY); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_YUV420P))	{ kDebug() << pixelFormatName(PIXELFORMAT_YUV420P); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_YUV422P))	{ kDebug() << pixelFormatName(PIXELFORMAT_YUV422P); }

// Compressed formats
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_JPEG))	{ kDebug() << pixelFormatName(PIXELFORMAT_JPEG); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_MPEG))	{ kDebug() << pixelFormatName(PIXELFORMAT_MPEG); }

// Reserved formats
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_DV))		{ kDebug() << pixelFormatName(PIXELFORMAT_DV); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_ET61X251))	{ kDebug() << pixelFormatName(PIXELFORMAT_ET61X251); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_HI240))	{ kDebug() << pixelFormatName(PIXELFORMAT_HI240); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_HM12))	{ kDebug() << pixelFormatName(PIXELFORMAT_HM12); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_MJPEG))	{ kDebug() << pixelFormatName(PIXELFORMAT_MJPEG); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_PWC1))	{ kDebug() << pixelFormatName(PIXELFORMAT_PWC1); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_PWC2))	{ kDebug() << pixelFormatName(PIXELFORMAT_PWC2); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_SN9C10X))	{ kDebug() << pixelFormatName(PIXELFORMAT_SN9C10X); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_WNVA))	{ kDebug() << pixelFormatName(PIXELFORMAT_WNVA); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_YYUV))	{ kDebug() << pixelFormatName(PIXELFORMAT_YYUV); }
			break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			return PIXELFORMAT_NONE;	break;
	}
	return PIXELFORMAT_NONE;
}

__u64 VideoDevice::signalStandardCode(signal_standard standard)
{
	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			switch(standard)
			{
				case STANDARD_PAL_B	: return V4L2_STD_PAL_B;	break;
				case STANDARD_PAL_B1	: return V4L2_STD_PAL_B1;	break;
				case STANDARD_PAL_G	: return V4L2_STD_PAL_G;	break;
				case STANDARD_PAL_H	: return V4L2_STD_PAL_H;	break;
				case STANDARD_PAL_I	: return V4L2_STD_PAL_I;	break;
				case STANDARD_PAL_D	: return V4L2_STD_PAL_D;	break;
				case STANDARD_PAL_D1	: return V4L2_STD_PAL_D1;	break;
				case STANDARD_PAL_K	: return V4L2_STD_PAL_K;	break;

				case STANDARD_PAL_M	: return V4L2_STD_PAL_M;	break;
				case STANDARD_PAL_N	: return V4L2_STD_PAL_N;	break;
				case STANDARD_PAL_Nc	: return V4L2_STD_PAL_Nc;	break;
				case STANDARD_PAL_60	: return V4L2_STD_PAL_60;	break;

				case STANDARD_NTSC_M	: return V4L2_STD_NTSC_M;	break;
				case STANDARD_NTSC_M_JP	: return V4L2_STD_NTSC_M_JP;	break;
				case STANDARD_NTSC_443	: return V4L2_STD_NTSC_443;	break;
				case STANDARD_NTSC_M_KR	: return V4L2_STD_NTSC_M_KR;	break;

				case STANDARD_SECAM_B	: return V4L2_STD_SECAM_B;	break;
				case STANDARD_SECAM_D	: return V4L2_STD_SECAM_D;	break;
				case STANDARD_SECAM_G	: return V4L2_STD_SECAM_G;	break;
				case STANDARD_SECAM_H	: return V4L2_STD_SECAM_H;	break;
				case STANDARD_SECAM_K	: return V4L2_STD_SECAM_K;	break;
				case STANDARD_SECAM_K1	: return V4L2_STD_SECAM_K1;	break;
				case STANDARD_SECAM_L	: return V4L2_STD_SECAM_L;	break;
				case STANDARD_SECAM_LC	: return V4L2_STD_SECAM_LC;	break;

				case STANDARD_ATSC_8_VSB: return V4L2_STD_ATSC_8_VSB;	break;
				case STANDARD_ATSC_16_VSB:return V4L2_STD_ATSC_16_VSB;	break;

				case STANDARD_PAL_BG	: return V4L2_STD_PAL_BG;	break;
				case STANDARD_PAL_DK	: return V4L2_STD_PAL_DK;	break;
				case STANDARD_PAL	: return V4L2_STD_PAL;		break;
				case STANDARD_NTSC	: return V4L2_STD_NTSC;		break;
				case STANDARD_SECAM_DK	: return V4L2_STD_SECAM_DK;	break;
				case STANDARD_SECAM	: return V4L2_STD_SECAM;	break;

				case STANDARD_MN	: return V4L2_STD_MN;		break;
				case STANDARD_B		: return V4L2_STD_B;		break;
				case STANDARD_GH	: return V4L2_STD_GH;		break;
				case STANDARD_DK	: return V4L2_STD_DK;		break;

				case STANDARD_525_60	: return V4L2_STD_525_60;	break;
				case STANDARD_625_50	: return V4L2_STD_625_50;	break;
				case STANDARD_ATSC	: return V4L2_STD_ATSC;		break;

				case STANDARD_UNKNOWN	: return V4L2_STD_UNKNOWN;	break;
				case STANDARD_ALL	: return V4L2_STD_ALL;		break;
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			switch(standard)
			{
				case STANDARD_PAL_B	: return VIDEO_MODE_PAL;	break;
				case STANDARD_PAL_B1	: return VIDEO_MODE_PAL;	break;
				case STANDARD_PAL_G	: return VIDEO_MODE_PAL;	break;
				case STANDARD_PAL_H	: return VIDEO_MODE_PAL;	break;
				case STANDARD_PAL_I	: return VIDEO_MODE_PAL;	break;
				case STANDARD_PAL_D	: return VIDEO_MODE_PAL;	break;
				case STANDARD_PAL_D1	: return VIDEO_MODE_PAL;	break;
				case STANDARD_PAL_K	: return VIDEO_MODE_PAL;	break;

				case STANDARD_PAL_M	: return 5;	break;	// Undocumented value found to be compatible with V4L bttv driver
				case STANDARD_PAL_N	: return 6;	break;	// Undocumented value found to be compatible with V4L bttv driver
				case STANDARD_PAL_Nc	: return 4;	break;	// Undocumented value found to be compatible with V4L bttv driver
				case STANDARD_PAL_60	: return VIDEO_MODE_PAL;	break;

				case STANDARD_NTSC_M	: return VIDEO_MODE_NTSC;	break;
				case STANDARD_NTSC_M_JP	: return 7;	break;	// Undocumented value found to be compatible with V4L bttv driver
				case STANDARD_NTSC_443	: return VIDEO_MODE_NTSC;	break;
				case STANDARD_NTSC_M_KR	: return VIDEO_MODE_NTSC;	break;

				case STANDARD_SECAM_B	: return VIDEO_MODE_SECAM;	break;
				case STANDARD_SECAM_D	: return VIDEO_MODE_SECAM;	break;
				case STANDARD_SECAM_G	: return VIDEO_MODE_SECAM;	break;
				case STANDARD_SECAM_H	: return VIDEO_MODE_SECAM;	break;
				case STANDARD_SECAM_K	: return VIDEO_MODE_SECAM;	break;
				case STANDARD_SECAM_K1	: return VIDEO_MODE_SECAM;	break;
				case STANDARD_SECAM_L	: return VIDEO_MODE_SECAM;	break;
				case STANDARD_SECAM_LC	: return VIDEO_MODE_SECAM;	break;

				case STANDARD_ATSC_8_VSB: return VIDEO_MODE_AUTO;	break;
				case STANDARD_ATSC_16_VSB:return VIDEO_MODE_AUTO;	break;

				case STANDARD_PAL_BG	: return VIDEO_MODE_PAL;	break;
				case STANDARD_PAL_DK	: return VIDEO_MODE_PAL;	break;
				case STANDARD_PAL	: return VIDEO_MODE_PAL;	break;
				case STANDARD_NTSC	: return VIDEO_MODE_NTSC;	break;
				case STANDARD_SECAM_DK	: return VIDEO_MODE_SECAM;	break;
				case STANDARD_SECAM	: return VIDEO_MODE_SECAM;	break;

				case STANDARD_MN	: return VIDEO_MODE_AUTO;	break;
				case STANDARD_B		: return VIDEO_MODE_AUTO;	break;
				case STANDARD_GH	: return VIDEO_MODE_AUTO;	break;
				case STANDARD_DK	: return VIDEO_MODE_AUTO;	break;

				case STANDARD_525_60	: return VIDEO_MODE_PAL;	break;
				case STANDARD_625_50	: return VIDEO_MODE_SECAM;	break;
				case STANDARD_ATSC	: return VIDEO_MODE_AUTO;	break;

				case STANDARD_UNKNOWN	: return VIDEO_MODE_AUTO;	break;
				case STANDARD_ALL	: return VIDEO_MODE_AUTO;	break;
			}
			break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			return STANDARD_UNKNOWN;	break;
	}
	return STANDARD_UNKNOWN;
}

QString VideoDevice::signalStandardName(signal_standard standard)
{
	QString returnvalue;
	returnvalue = "None";
	switch(standard)
	{
		case STANDARD_PAL_B	: returnvalue = "PAL-B";	break;
		case STANDARD_PAL_B1	: returnvalue = "PAL-B1"; 	break;
		case STANDARD_PAL_G	: returnvalue = "PAL-G";	break;
		case STANDARD_PAL_H	: returnvalue = "PAL-H";	break;
		case STANDARD_PAL_I	: returnvalue = "PAL-I";	break;
		case STANDARD_PAL_D	: returnvalue = "PAL-D";	break;
		case STANDARD_PAL_D1	: returnvalue = "PAL-D1";	break;
		case STANDARD_PAL_K	: returnvalue = "PAL-K";	break;

		case STANDARD_PAL_M	: returnvalue = "PAL-M";	break;
		case STANDARD_PAL_N	: returnvalue = "PAL-N";	break;
		case STANDARD_PAL_Nc	: returnvalue = "PAL-Nc";	break;
		case STANDARD_PAL_60	: returnvalue = "PAL-60";	break;

		case STANDARD_NTSC_M	: returnvalue = "NTSC-M";	break;
		case STANDARD_NTSC_M_JP	: returnvalue = "NTSC-M(JP)";	break;
		case STANDARD_NTSC_443	: returnvalue = "NTSC-443";	break;
		case STANDARD_NTSC_M_KR	: returnvalue = "NTSC-M(KR)";	break;

		case STANDARD_SECAM_B	: returnvalue = "SECAM-B";	break;
		case STANDARD_SECAM_D	: returnvalue = "SECAM-D";	break;
		case STANDARD_SECAM_G	: returnvalue = "SECAM-G";	break;
		case STANDARD_SECAM_H	: returnvalue = "SECAM-H";	break;
		case STANDARD_SECAM_K	: returnvalue = "SECAM-K";	break;
		case STANDARD_SECAM_K1	: returnvalue = "SECAM-K1";	break;
		case STANDARD_SECAM_L	: returnvalue = "SECAM-L";	break;
		case STANDARD_SECAM_LC	: returnvalue = "SECAM-LC";	break;

		case STANDARD_ATSC_8_VSB: returnvalue = "ATSC-8-VSB";	break;
		case STANDARD_ATSC_16_VSB:returnvalue = "ATSC-16-VSB";	break;

		case STANDARD_PAL_BG	: returnvalue = "PAL-BG";	break;
		case STANDARD_PAL_DK	: returnvalue = "PAL-DK";	break;
		case STANDARD_PAL	: returnvalue = "PAL";		break;
		case STANDARD_NTSC	: returnvalue = "NTSC";		break;
		case STANDARD_SECAM_DK  : returnvalue = "SECAM-DK";	break;
		case STANDARD_SECAM	: returnvalue = "SECAM";	break;

		case STANDARD_MN	: returnvalue = "DK";		break;
		case STANDARD_B		: returnvalue = "B";		break;
		case STANDARD_GH	: returnvalue = "GH";		break;
		case STANDARD_DK	: returnvalue = "DK";		break;

		case STANDARD_525_60	: returnvalue = "525 lines 60Hz";break;
		case STANDARD_625_50	: returnvalue = "625 lines 50Hz";break;
		case STANDARD_ATSC	: returnvalue = "ATSC"; 	break;

		case STANDARD_UNKNOWN	: returnvalue = "Unknown";	break;
		case STANDARD_ALL	: returnvalue = "All";		break;
	}
	return returnvalue;
}

QString VideoDevice::signalStandardName(int standard)
{
	QString returnvalue;
	returnvalue = "None";
	switch(m_driver)
	{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			switch(standard)
			{
				case V4L2_STD_PAL_B	: returnvalue = signalStandardName(STANDARD_PAL_B);	break;
				case V4L2_STD_PAL_B1	: returnvalue = signalStandardName(STANDARD_PAL_B1);	break;
				case V4L2_STD_PAL_G	: returnvalue = signalStandardName(STANDARD_PAL_G);	break;
				case V4L2_STD_PAL_H	: returnvalue = signalStandardName(STANDARD_PAL_H);	break;
				case V4L2_STD_PAL_I	: returnvalue = signalStandardName(STANDARD_PAL_I);	break;
				case V4L2_STD_PAL_D	: returnvalue = signalStandardName(STANDARD_PAL_D);	break;
				case V4L2_STD_PAL_D1	: returnvalue = signalStandardName(STANDARD_PAL_D1);	break;
				case V4L2_STD_PAL_K	: returnvalue = signalStandardName(STANDARD_PAL_K);	break;
				case V4L2_STD_PAL_M	: returnvalue = signalStandardName(STANDARD_PAL_M);	break;
				case V4L2_STD_PAL_N	: returnvalue = signalStandardName(STANDARD_PAL_N);	break;
				case V4L2_STD_PAL_Nc	: returnvalue = signalStandardName(STANDARD_PAL_Nc);	break;
				case V4L2_STD_PAL_60	: returnvalue = signalStandardName(STANDARD_PAL_60);	break;
				case V4L2_STD_NTSC_M	: returnvalue = signalStandardName(STANDARD_NTSC_M);	break;
				case V4L2_STD_NTSC_M_JP	: returnvalue = signalStandardName(STANDARD_NTSC_M_JP);	break;
				case V4L2_STD_NTSC_443	: returnvalue = signalStandardName(STANDARD_NTSC_443);	break; // Commented out because my videodev2.h header seems to not include this standard in struct __u64 v4l2_std_id
				case V4L2_STD_NTSC_M_KR	: returnvalue = signalStandardName(STANDARD_NTSC_M_KR);	break; // Commented out because my videodev2.h header seems to not include this standard in struct __u64 v4l2_std_id
				case V4L2_STD_SECAM_B	: returnvalue = signalStandardName(STANDARD_SECAM_B);	break;
				case V4L2_STD_SECAM_D	: returnvalue = signalStandardName(STANDARD_SECAM_D);	break;
				case V4L2_STD_SECAM_G	: returnvalue = signalStandardName(STANDARD_SECAM_G);	break;
				case V4L2_STD_SECAM_H	: returnvalue = signalStandardName(STANDARD_SECAM_H);	break;
				case V4L2_STD_SECAM_K	: returnvalue = signalStandardName(STANDARD_SECAM_K);	break;
				case V4L2_STD_SECAM_K1	: returnvalue = signalStandardName(STANDARD_SECAM_K1);	break;
				case V4L2_STD_SECAM_L	: returnvalue = signalStandardName(STANDARD_SECAM_L);	break;
				case V4L2_STD_SECAM_LC	: returnvalue = signalStandardName(STANDARD_SECAM_LC);	break;

				case V4L2_STD_ATSC_8_VSB: returnvalue = signalStandardName(STANDARD_ATSC_8_VSB);break;
				case V4L2_STD_ATSC_16_VSB:returnvalue = signalStandardName(STANDARD_ATSC_16_VSB);break;

				case V4L2_STD_PAL_BG	: returnvalue = signalStandardName(STANDARD_PAL_BG);	break;
				case V4L2_STD_PAL_DK	: returnvalue = signalStandardName(STANDARD_PAL_DK);	break;
				case V4L2_STD_PAL	: returnvalue = signalStandardName(STANDARD_PAL);	break;
				case V4L2_STD_NTSC	: returnvalue = signalStandardName(STANDARD_NTSC);	break;
				case V4L2_STD_SECAM_DK	: returnvalue = signalStandardName(STANDARD_SECAM_DK);	break;
				case V4L2_STD_SECAM	: returnvalue = signalStandardName(STANDARD_SECAM);	break;

				case V4L2_STD_MN	: returnvalue = signalStandardName(STANDARD_MN);	break;
				case V4L2_STD_B		: returnvalue = signalStandardName(STANDARD_B);		break;
				case V4L2_STD_GH	: returnvalue = signalStandardName(STANDARD_GH);	break;
				case V4L2_STD_DK	: returnvalue = signalStandardName(STANDARD_DK);	break;

				case V4L2_STD_525_60	: returnvalue = signalStandardName(STANDARD_525_60);	break;
				case V4L2_STD_625_50	: returnvalue = signalStandardName(STANDARD_625_50);	break;
				case V4L2_STD_ATSC	: returnvalue = signalStandardName(STANDARD_ATSC);	break;

				case V4L2_STD_UNKNOWN	: returnvalue = signalStandardName(STANDARD_UNKNOWN);	break;
				case V4L2_STD_ALL	: returnvalue = signalStandardName(STANDARD_ALL);	break;
			}
			break;
#endif
		case VIDEODEV_DRIVER_V4L:
			switch(standard)
			{
				case VIDEO_MODE_PAL	: returnvalue = signalStandardName(STANDARD_PAL);	break;
				case VIDEO_MODE_NTSC	: returnvalue = signalStandardName(STANDARD_NTSC);	break;
				case VIDEO_MODE_SECAM	: returnvalue = signalStandardName(STANDARD_SECAM);	break;
				case VIDEO_MODE_AUTO	: returnvalue = signalStandardName(STANDARD_ALL);	break;	// It must be disabled until I find a correct way to handle those non-standard bttv modes
//				case VIDEO_MODE_PAL_Nc	: returnvalue = signalStandardName(STANDARD_PAL_Nc);	break;	// Undocumented value found to be compatible with V4L bttv driver
				case VIDEO_MODE_PAL_M	: returnvalue = signalStandardName(STANDARD_PAL_M);	break;	// Undocumented value found to be compatible with V4L bttv driver
				case VIDEO_MODE_PAL_N	: returnvalue = signalStandardName(STANDARD_PAL_N);	break;	// Undocumented value found to be compatible with V4L bttv driver
				case VIDEO_MODE_NTSC_JP	: returnvalue = signalStandardName(STANDARD_NTSC_M_JP);	break;	// Undocumented value found to be compatible with V4L bttv driver
			}
			break;
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return returnvalue;
}

/*!
    \fn VideoDevice::detectSignalStandards()
// this must be done once for each _input_.
 */
int VideoDevice::detectSignalStandards()
{
	kDebug() << "called.";
	if(isOpen())
	{
	switch(m_driver)
		{
#if defined(__linux__) && defined(ENABLE_AV)
#ifdef V4L2_CAP_VIDEO_CAPTURE
			case VIDEODEV_DRIVER_V4L2:


				struct v4l2_input input;
				struct v4l2_standard standard;

				memset (&input, 0, sizeof (input));

				if (-1 == xioctl (VIDIOC_G_INPUT, &input.index)) {
					perror ("VIDIOC_G_INPUT");
					return EXIT_FAILURE;
				}

				if (-1 == xioctl (VIDIOC_ENUMINPUT, &input)) {
					perror ("VIDIOC_ENUM_INPUT");
					return EXIT_FAILURE;
				}

//			printf ("Current input %s supports:\n", input.name);

				memset (&standard, 0, sizeof (standard));
				standard.index = 0;

				while (0 == xioctl (VIDIOC_ENUMSTD, &standard)) {
					if (standard.id & input.std)
					{
//						kDebug() << standard.name;
						kDebug() << signalStandardName(standard.id) << " (" << standard.id << ")" << V4L2_STD_NTSC;
					}

					standard.index++;
				}

/* EINVAL indicates the end of the enumeration, which cannot be
   empty unless this device falls under the USB exception. */

				if (errno != EINVAL || standard.index == 0) {
					perror ("VIDIOC_ENUMSTD");
					return EXIT_FAILURE;
				}


				break;
#endif
			case VIDEODEV_DRIVER_V4L:
				break;
#endif
			case VIDEODEV_DRIVER_NONE:
			default:
				break;
		}
	//FIXME: return a real value
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/*!
    \fn VideoDevice::initRead()
 */
int VideoDevice::initRead()
{
    /// @todo implement me

	kDebug() << "called.";
	if(isOpen())
	{
		m_rawbuffers.resize(1);
		if (m_rawbuffers.size()==0)
		{
			fprintf (stderr, "Out of memory\n");
			return EXIT_FAILURE;
		}
		kDebug() << "m_buffer_size: " << m_buffer_size;

//		m_rawbuffers[0].pixelformat=m_pixelformat;
		m_rawbuffers[0].length = m_buffer_size;
		m_rawbuffers[0].start = (uchar *)malloc (m_buffer_size);

		if (!m_rawbuffers[0].start)
		{
			fprintf (stderr, "Out of memory\n");
			return EXIT_FAILURE;
		}
		kDebug() << "exited successfuly.";
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}


/*!
    \fn VideoDevice::initMmap()
 */
int VideoDevice::initMmap()
{
    /// @todo implement me
#define BUFFERS 2
	if(isOpen())
	{
		kDebug() << full_filename << " Trying to MMAP";
#ifdef V4L2_CAP_VIDEO_CAPTURE
		struct v4l2_requestbuffers req;

		CLEAR (req);

		req.count  = BUFFERS;
		req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = V4L2_MEMORY_MMAP;

		if (-1 == xioctl (VIDIOC_REQBUFS, &req))
		{
			if (EINVAL == errno)
			{
				kDebug() << full_filename << " does not support memory mapping";
				return EXIT_FAILURE;
			}
			else
			{
				return errnoReturn ("VIDIOC_REQBUFS");
			}
		}

		if (req.count < BUFFERS)
		{
			kDebug() << "Insufficient buffer memory on " << full_filename;
			return EXIT_FAILURE;
		}

		m_rawbuffers.resize(req.count);

		if (m_rawbuffers.size()==0)
		{
			kDebug() <<  "Out of memory";
			return EXIT_FAILURE;
		}

		for (m_streambuffers = 0; m_streambuffers < req.count; ++m_streambuffers)
		{
			struct v4l2_buffer v4l2buffer;

			CLEAR (v4l2buffer);

			v4l2buffer.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			v4l2buffer.memory = V4L2_MEMORY_MMAP;
			v4l2buffer.index  = m_streambuffers;

			if (-1 == xioctl (VIDIOC_QUERYBUF, &v4l2buffer))
				return errnoReturn ("VIDIOC_QUERYBUF");

			m_rawbuffers[m_streambuffers].length = v4l2buffer.length;
#ifdef HAVE_LIBV4L2
			m_rawbuffers[m_streambuffers].start = (uchar *) v4l2_mmap (NULL /* start anywhere */, v4l2buffer.length, PROT_READ | PROT_WRITE /* required */, MAP_SHARED /* recommended */, descriptor, v4l2buffer.m.offset);
#else
			m_rawbuffers[m_streambuffers].start = (uchar *) mmap (NULL /* start anywhere */, v4l2buffer.length, PROT_READ | PROT_WRITE /* required */, MAP_SHARED /* recommended */, descriptor, v4l2buffer.m.offset);
#endif

			if (MAP_FAILED == m_rawbuffers[m_streambuffers].start)
			return errnoReturn ("mmap");
		}
#endif
		m_currentbuffer.data.resize(m_rawbuffers[0].length); // Makes the imagesize.data buffer size equal to the rawbuffer size
		kDebug() << full_filename << " m_currentbuffer.data.size(): " << m_currentbuffer.data.size();
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}


/*!
    \fn VideoDevice::initUserptr()
 */
int VideoDevice::initUserptr()
{
    /// @todo implement me
	if(isOpen())
	{
#ifdef V4L2_CAP_VIDEO_CAPTURE
		struct v4l2_requestbuffers req;

		CLEAR (req);

		req.count  = 2;
		req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = V4L2_MEMORY_USERPTR;

		if (-1 == xioctl (VIDIOC_REQBUFS, &req))
		{
			if (EINVAL == errno)
			{
				kDebug() << full_filename << " does not support memory mapping";
				return EXIT_FAILURE;
			}
			else
			{
				return errnoReturn ("VIDIOC_REQBUFS");
			}
		}

		m_rawbuffers.resize(4);

		if (m_rawbuffers.size()==0)
		{
			fprintf (stderr, "Out of memory\n");
			return EXIT_FAILURE;
		}

		for (m_streambuffers = 0; m_streambuffers < 4; ++m_streambuffers)
		{
			m_rawbuffers[m_streambuffers].length = m_buffer_size;
			m_rawbuffers[m_streambuffers].start = (uchar *) malloc (m_buffer_size);

			if (!m_rawbuffers[m_streambuffers].start)
			{
				kDebug() <<  "Out of memory";
				return EXIT_FAILURE;
			}
		}
#endif
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool VideoDevice::canCapture()
{
	return m_videocapture;
}

bool VideoDevice::canChromakey()
{
	return m_videochromakey;
}

bool VideoDevice::canScale()
{
	return m_videoscale;
}

bool VideoDevice::canOverlay()
{
	return m_videooverlay;
}

bool VideoDevice::canRead()
{
	return m_videoread;
}

bool VideoDevice::canAsyncIO()
{
	return m_videoasyncio;
}

bool VideoDevice::canStream()
{
	return m_videostream;
}

void VideoDevice::setUdi( const QString & udi )
{
    m_udi = udi;
}

QString VideoDevice::udi() const
{
    return m_udi;
}

}

}
