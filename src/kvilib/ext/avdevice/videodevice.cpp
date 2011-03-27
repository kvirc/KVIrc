//=============================================================================
//
//   File : videodevice.cpp
//   Creation date : Tue Nov 10 18:08:09 2009 GMT by Fabio Bas
//
//   This file is part of the KVIrc irc client distribution
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
    Copyright (c) 2010      by Frank Schaefer                 <fschaefer.oss@googlemail.com>

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

#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <QDebug>

#include "videoinput.h"
#include "videodevice.h"

#include "bayer.h"
#include "sonix_compress.h"

#include "KviLocale.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

namespace Kopete {

namespace AV {

VideoDevice::VideoDevice()
{
	descriptor = -1;
	m_streambuffers  = 0;
	m_current_input = 0;
}


VideoDevice::~VideoDevice()
{
	close();
}

/*!
    \fn void VideoDevice::setupControls()
    \brief Sets up the supported video-controls for the current input
    
    Determines which video-controls are supported for the current input,
    reads all needed informations about them and stores the data to the local members
 */
void VideoDevice::setupControls()
{
#ifndef COMPILE_DISABLE_AVDEVICE
	bool driver_vflip = false;
	bool driver_hflip = false;
#endif
	m_numericCtrls.clear();
	m_booleanCtrls.clear();
	m_menuCtrls.clear();
	m_actionCtrls.clear();

	switch(m_driver)
	{
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			struct v4l2_queryctrl qctrl;
			CLEAR (qctrl);
			// Get standard controls:
			for (quint32 k = V4L2_CID_BASE; k < V4L2_CID_LASTP1; k++) // NOTE: DO NOT USE qctrl.id DIRECTLY !
			{
				qctrl.id = k;
				if (0 == xioctl(VIDIOC_QUERYCTRL, &qctrl))
				{
					if (qctrl.flags & V4L2_CTRL_FLAG_DISABLED)
						continue;
					if (qctrl.id == V4L2_CID_VFLIP)
						driver_vflip = true;
					if (qctrl.id == V4L2_CID_HFLIP)
						driver_hflip = true;
					const char *name_uni = getUnifiedV4L2StdCtrlName(qctrl.id);
					if (name_uni && strlen(name_uni))
						strncpy((char*)qctrl.name, name_uni, 32); // NOTE: v4l2_queryctrl.name is _u8[32]
					saveV4L2ControlData(qctrl);
				}
				else
				{
					if (errno == EINVAL)
						continue;
					qDebug() << "VIDIOC_QUERYCTRL failed (" << errno << ").";
				}
			}
			// Get custom controls:
			for (quint32 k = V4L2_CID_PRIVATE_BASE; ; k++)
			{
				qctrl.id = k;
				if (0 == xioctl(VIDIOC_QUERYCTRL, &qctrl))
				{
					if (qctrl.flags & V4L2_CTRL_FLAG_DISABLED)
						continue;
					saveV4L2ControlData(qctrl);
				}
				else
				{
					if (errno == EINVAL)
						break;
					qDebug() << "VIDIOC_QUERYCTRL failed (" << errno << ").";
				}
			}
			break;
#endif
#ifdef COMPILE_V4L1_CODE

		case VIDEODEV_DRIVER_V4L:
			{
				NumericVideoControl numCtrl;
				numCtrl.value_min = 0;
				numCtrl.value_max = 65535;
				numCtrl.value_step = 1;
				numCtrl.value_default = 32767;
				numCtrl.id = IMGCTRL_ID_V4L1_BRIGHTNESS;
				numCtrl.name = __tr("Brightness");
				m_numericCtrls.push_back( numCtrl );
				numCtrl.id = IMGCTRL_ID_V4L1_HUE;
				numCtrl.name = __tr("Hue");
				m_numericCtrls.push_back( numCtrl );
				numCtrl.id = IMGCTRL_ID_V4L1_COLOR;
				numCtrl.name = __tr("Color");
				m_numericCtrls.push_back( numCtrl );
				numCtrl.id = IMGCTRL_ID_V4L1_CONTRAST;
				numCtrl.name = __tr("Contrast");
				m_numericCtrls.push_back( numCtrl );
				numCtrl.id = IMGCTRL_ID_V4L1_WHITENESS;
				numCtrl.name = __tr("Whiteness");
				m_numericCtrls.push_back( numCtrl );
			}
			break;
#endif //COMPILE_V4L1_CODE
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	
#ifndef COMPILE_DISABLE_AVDEVICE
	// Software controls:
	BooleanVideoControl boolCtrl;
	boolCtrl.value_default = 0;
	boolCtrl.id = IMGCTRL_ID_SOFT_AUTOBRIGHTNESSCONTRASTCORR;
	boolCtrl.name = __tr("Automatic Brightness/Contrast Correction");
	m_booleanCtrls.push_back( boolCtrl );
	boolCtrl.id = IMGCTRL_ID_SOFT_AUTOCOLORCORR;
	boolCtrl.name = __tr("Automatic Color Correction");
	m_booleanCtrls.push_back( boolCtrl );
	if (!driver_vflip)
	{
		boolCtrl.id = IMGCTRL_ID_SOFT_VFLIP;
		boolCtrl.name = __tr("Vertical Flip");
		m_booleanCtrls.push_back( boolCtrl );
	}
	if (!driver_hflip)
	{
		boolCtrl.id = IMGCTRL_ID_SOFT_HFLIP;
		boolCtrl.name = __tr("Horizontal Flip");
		m_booleanCtrls.push_back( boolCtrl );
	}
#endif
}

#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
/*!
    \fn bool VideoDevice::getMenuCtrlOptions(quint32 id, quint32 maxindex, QStringList * options)
    \param id ID of the video-control
    \param maxindex Highest menu index
    \param options Pointer to the QStringList which will recieve the option-strings of the video-control
    \return The success of the operation
    \brief Reads the available options provided by a menu-video-control
 */
bool VideoDevice::getMenuCtrlOptions(quint32 id, quint32 maxindex, QStringList * options)
{
	QStringList opt;
	struct v4l2_querymenu ctrlmenu;
	CLEAR (ctrlmenu);
	options->clear();
	ctrlmenu.id = id;
	for (quint32 k = 0; k <= maxindex; k++)	// DO NOT USE ctrlmenu.index DIRECTLY !
	{
		ctrlmenu.index = k;
		if (0 == xioctl(VIDIOC_QUERYMENU, &ctrlmenu))
		{
			opt.push_back( __tr( (char*)(ctrlmenu.name) ) );
			qDebug() << "option" << k << ":" << (char*)(ctrlmenu.name);
		}
		else
		{
			qDebug() << "VIDIOC_QUERYMENU failed (" << errno << ").";
			return false;
		}
	}
	*options = opt;
	return true;
	/* NOTE: According to V4L2-spec v0.24:
	         v4l2_querymenu.minimum = start-index = 0, v4l2_querymenu.maximum = last index
	         => DO DRIVERS REALLY SET THESE VALUES CORRECTLY ???
	 */
}


/*!
    \fn void VideoDevice::saveV4L2ControlData(struct v4l2_queryctrl qctrl)
    \param qctrl The struct v4l2_queryctrl of the V4L2-control that should be saved
    \brief Saves the data of a V4L2-video-control
 */
void VideoDevice::saveV4L2ControlData(struct v4l2_queryctrl qctrl)
{
	NumericVideoControl numericCtrl;
	BooleanVideoControl booleanCtrl;
	MenuVideoControl menuCtrl;
	ActionVideoControl actionCtrl;
	QStringList options;
	switch (qctrl.type)
	{
		case V4L2_CTRL_TYPE_INTEGER:
			numericCtrl.id = qctrl.id;
			numericCtrl.name = __tr((char*)qctrl.name);
			numericCtrl.value_min = qctrl.minimum;
			numericCtrl.value_max = qctrl.maximum;
			numericCtrl.value_default = qctrl.default_value;
			numericCtrl.value_step = qctrl.step;
			m_numericCtrls.push_back( numericCtrl );
			break;
		case V4L2_CTRL_TYPE_BOOLEAN:
			booleanCtrl.id = qctrl.id;
			booleanCtrl.name = __tr((char*)qctrl.name);
			booleanCtrl.value_default = qctrl.default_value;
			m_booleanCtrls.push_back( booleanCtrl );
			break;
		case V4L2_CTRL_TYPE_MENU:
			if (getMenuCtrlOptions( qctrl.id, qctrl.maximum, &options ))
			{
				menuCtrl.id = qctrl.id;
				menuCtrl.name = __tr((char*)qctrl.name);
				menuCtrl.index_default = qctrl.default_value;
				menuCtrl.options = options;
				m_menuCtrls.push_back( menuCtrl );
			}
			break;
		case V4L2_CTRL_TYPE_BUTTON:
			actionCtrl.id = qctrl.id;
			actionCtrl.name = __tr((char*)qctrl.name);
			m_actionCtrls.push_back( actionCtrl );
			break;
		// Since kernel 2.6.18:
#ifdef V4L2_CTRL_TYPE_INTEGER64
		case V4L2_CTRL_TYPE_INTEGER64:
#endif
#ifdef V4L2_CTRL_TYPE_CTRL_CLASS
		case V4L2_CTRL_TYPE_CTRL_CLASS:
#endif
		default:
			break;
	}
// TODO	V4L2_CTRL_TYPE_INTEGER64, V4L2_CTRL_TYPE_CTRL_CLASS
}
#endif
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
    \fn QString VideoDevice::fileName()
 */
QString VideoDevice::fileName()
{
	return full_filename;
}

/*!
    \fn VideoDevice::open()
 */
int VideoDevice::open()
{
    /// @todo implement me

	qDebug() << "called";
	if(-1 != descriptor)
	{
		qDebug() << "Device is already open";
		return EXIT_SUCCESS;
	}
#ifdef HAVE_LIBV4L2
	descriptor = ::v4l2_open (QFile::encodeName(full_filename), O_RDWR | O_NONBLOCK, 0);
#else
	descriptor = ::open (QFile::encodeName(full_filename), O_RDWR | O_NONBLOCK, 0);
#endif
	if(isOpen())
	{
		qDebug() << "File " << full_filename << " was opened successfuly";
		if(EXIT_FAILURE==checkDevice())
		{
			qDebug() << "File " << full_filename << " could not be opened";
			close();
			return EXIT_FAILURE;
		}
	}
	else
	{
		qDebug() << "Unable to open file " << full_filename << "Err: "<< errno;
		return EXIT_FAILURE;
	}

	initDevice();
	selectInput(m_current_input);
	qDebug() << "exited successfuly";
	return EXIT_SUCCESS;
}

bool VideoDevice::isOpen()
{
	if(-1 == descriptor)
	{
//		qDebug() << "VideoDevice::isOpen() File is not open";
		return false;
	}
//	qDebug() << "VideoDevice::isOpen() File is open";
	return true;
}

int VideoDevice::checkDevice()
{
	qDebug() << "checkDevice() called.";
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
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE

		CLEAR(V4L2_capabilities);

		if (-1 != xioctl (VIDIOC_QUERYCAP, &V4L2_capabilities))
		{
			if (!(V4L2_capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE))
			{
				qDebug() << "checkDevice(): " << full_filename << " is not a video capture device.";
				m_driver = VIDEODEV_DRIVER_NONE;
				return EXIT_FAILURE;
			}
			m_videocapture=true;
			qDebug() << "checkDevice(): " << full_filename << " is a V4L2 device.";
			m_driver = VIDEODEV_DRIVER_V4L2;
			m_model=QString::fromLocal8Bit((const char*)V4L2_capabilities.card);


// Detect maximum and minimum resolution supported by the V4L2 device. VIDIOC_ENUM_FRAMESIZES is still experimental.
			CLEAR (fmt);
			fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == xioctl (VIDIOC_G_FMT, &fmt))
			{
				qDebug() << "VIDIOC_G_FMT failed (" << errno << ").";
			}
			fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			fmt.fmt.pix.width       = 32767;
			fmt.fmt.pix.height      = 32767;
			fmt.fmt.pix.field       = V4L2_FIELD_ANY;
			if (-1 == xioctl (VIDIOC_S_FMT, &fmt))
			{
				qDebug() << "Detecting maximum size with VIDIOC_S_FMT failed (" << errno << ").Returned maxwidth: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
				// Note VIDIOC_S_FMT may change width and height.
			}
			else
			{
				maxwidth  = fmt.fmt.pix.width;
				maxheight = fmt.fmt.pix.height;
			}
			if (-1 == xioctl (VIDIOC_G_FMT, &fmt))
			{
				qDebug() << "VIDIOC_G_FMT failed (" << errno << ").";
			}
			fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			fmt.fmt.pix.width       = 1;
			fmt.fmt.pix.height      = 1;
			fmt.fmt.pix.field       = V4L2_FIELD_ANY;
			if (-1 == xioctl (VIDIOC_S_FMT, &fmt))
			{
				qDebug() << "Detecting minimum size with VIDIOC_S_FMT failed (" << errno << ").Returned maxwidth: " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
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
					qDebug() << "Input " << loop << ": " << tempinput.name << " (tuner: " << ((videoinput.type & V4L2_INPUT_TYPE_TUNER) != 0) << ")";
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
			qDebug() << "checkDevice(): " << full_filename << " is not a V4L2 device.";
		}

#endif

#ifdef COMPILE_V4L1_CODE
		CLEAR(V4L_capabilities);

		if(m_driver==VIDEODEV_DRIVER_NONE)
		{
			qDebug() << "checkDevice(): " << full_filename << " Trying V4L API.";
			if (-1 == xioctl (VIDIOCGCAP, &V4L_capabilities))
			{
				perror ("ioctl (VIDIOCGCAP)");
				m_driver = VIDEODEV_DRIVER_NONE;
				return EXIT_FAILURE;
			}
			else
			{
				qDebug() << full_filename << " is a V4L device.";
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
//				qDebug() << "libkopete (avdevice):     Inputs : " << V4L_capabilities.channels;
//				qDebug() << "libkopete (avdevice):     Audios : " << V4L_capabilities.audios;
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
//						qDebug() << "libkopete (avdevice): Input " << loop << ": " << tempinput.name << " (tuner: " << ((videoinput.flags & VIDEO_VC_TUNER) != 0) << ")";
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
#endif //COMPILE_V4L1_CODE
#endif
		m_name=m_model; // Take care about changing the name to be different from the model itself...

		detectPixelFormats();
// TODO: Now we must execute the proper initialization according to the type of the driver.
		qDebug() << "checkDevice() exited successfuly.";
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}


/*!
    \fn VideoDevice::showDeviceCapabilities()
 */
int VideoDevice::showDeviceCapabilities()
{
	qDebug() << "showDeviceCapabilities() called.";
	if(isOpen())
	{
/*		qDebug() << "libkopete (avdevice): Driver: " << (const char*)V4L2_capabilities.driver << " "
			<< ((V4L2_capabilities.version>>16) & 0xFF) << "."
			<< ((V4L2_capabilities.version>> 8) & 0xFF) << "."
			<< ((V4L2_capabilities.version    ) & 0xFF) << endl;
		qDebug() << "libkopete (avdevice): Device: " << name;
		qDebug() << "libkopete (avdevice): Capabilities:";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE)
			qDebug() << "libkopete (avdevice):     Video capture";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VIDEO_OUTPUT)
			qDebug() << "libkopete (avdevice):     Video output";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VIDEO_OVERLAY)
			qDebug() << "libkopete (avdevice):     Video overlay";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VBI_CAPTURE)
			qDebug() << "libkopete (avdevice):     VBI capture";
		if(V4L2_capabilities.capabilities & V4L2_CAP_VBI_OUTPUT)
			qDebug() << "libkopete (avdevice):     VBI output";
		if(V4L2_capabilities.capabilities & V4L2_CAP_RDS_CAPTURE)
			qDebug() << "libkopete (avdevice):     RDS capture";
		if(V4L2_capabilities.capabilities & V4L2_CAP_TUNER)
			qDebug() << "libkopete (avdevice):     Tuner IO";
		if(V4L2_capabilities.capabilities & V4L2_CAP_AUDIO)
			qDebug() << "libkopete (avdevice):     Audio IO";
;*/
		qDebug() << "Device model: " << m_model;
		qDebug() << "Device name : " << m_name;
		qDebug() << "Capabilities:";
		if(canCapture())
		{
			qDebug() << "    Video capture";
		}
		if(canRead())
		{
			qDebug() << "        Read";
		}
		if(canAsyncIO())
		{
			qDebug() << "        Asynchronous input/output";
		}
		if(canStream())
		{
			qDebug() << "        Streaming";
		}
		if(canChromakey())
		{
			qDebug() << "    Video chromakey";
		}
		if(canScale())
		{
			qDebug() << "    Video scales";
		}
		if(canOverlay())
		{
			qDebug() << "    Video overlay";
		}
//		qDebug() << "libkopete (avdevice):     Audios : " << V4L_capabilities.audios;
		qDebug() << "    Max res: " << maxWidth() << " x " << maxHeight();
		qDebug() << "    Min res: " << minWidth() << " x " << minHeight();
		qDebug() << "    Inputs : " << inputs();
		for (int loop=0; loop < inputs(); loop++)
			qDebug() << "Input " << loop << ": " << m_input[loop].name << " (tuner: " << m_input[loop].hastuner << ")";
		qDebug() << "showDeviceCapabilities() exited successfuly.";
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
	qDebug() << "initDevice() started";
	if(-1 == descriptor)
	{
		qDebug() << "initDevice() Device is not open";
		return EXIT_FAILURE;
	}
	m_io_method = IO_METHOD_NONE;
	switch(m_driver)
	{
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			if(V4L2_capabilities.capabilities & V4L2_CAP_READWRITE)
			{
				m_videoread=true;
				m_io_method = IO_METHOD_READ;
				qDebug() << "    Read/Write interface";
			}
			if(V4L2_capabilities.capabilities & V4L2_CAP_ASYNCIO)
			{
				m_videoasyncio=true;
				qDebug() << "    Async IO interface";
			}
			if(V4L2_capabilities.capabilities & V4L2_CAP_STREAMING)
			{
				m_videostream=true;
				m_io_method = IO_METHOD_MMAP;
//				m_io_method = IO_METHOD_USERPTR;
				qDebug() << "    Streaming interface";
			}
			if(m_io_method==IO_METHOD_NONE)
			{
				qDebug() << "initDevice() Found no suitable input/output method for " << full_filename;
				return EXIT_FAILURE;
			}
			break;
#endif
#ifdef COMPILE_V4L1_CODE
		case VIDEODEV_DRIVER_V4L:
			m_videoread=true;
			m_io_method=IO_METHOD_READ;
			if(-1 != xioctl(VIDIOCGFBUF,&V4L_videobuffer))
			{
//				m_videostream=true;
//				m_io_method = IO_METHOD_MMAP;
				qDebug() << "    Streaming interface";
			}
			break;
#endif //COMPILE_V4L1_CODE
#endif
		case VIDEODEV_DRIVER_NONE:
		default:

			break;
	}

// Select video input, video standard and tune here.
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	CLEAR (cropcap);
	CLEAR (crop);
	
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
	qDebug() << "initDevice() exited successfuly";
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
qDebug() << "setSize(" << newwidth << ", " << newheight << ") called.";
	if(isOpen())
	{
// It should not be there. It must remain in a completely distict place, cause this method should not change the pixelformat.
// It shouldn't try to find a suitable pixel format this way. It should use values discovered by - detectPixelFormats() - to choose a valid one.
		qDebug() <<  "Trying YUY422P";
		errno = 0;
		if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_YUV422P) && errno != EBUSY)
		{
			qDebug() <<  "Device doesn't seem to support YUV422P format. Trying YUYV.";
			if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_YUYV) && errno != EBUSY)
			{
				qDebug() <<  "Device doesn't seem to support YUYV format. Trying UYVY.";
				if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_UYVY) && errno != EBUSY)
				{
					qDebug() <<  "Device doesn't seem to support UYVY format. Trying YUV420P.";
					if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_YUV420P) && errno != EBUSY)
					{
						qDebug() <<  "Device doesn't seem to support YUV420P format. Trying RGB24.";
						if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_RGB24) && errno != EBUSY)
						{
							qDebug() <<  "Device doesn't seem to support RGB24 format. Trying BGR24.";
							if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_BGR24) && errno != EBUSY)
							{
								qDebug() <<  "Device doesn't seem to support RGB24 format. Trying RGB32.";
								if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_RGB32) && errno != EBUSY)
								{
									qDebug() <<  "Device doesn't seem to support RGB32 format. Trying BGR32.";
									if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_BGR32) && errno != EBUSY)
									{
										qDebug() <<  "Device doesn't seem to support BGR32 format. Trying SBGGR8.";
										if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_SBGGR8) && errno != EBUSY)
										{
											qDebug() <<  "Device doesn't seem to support SBGGR8 format. Trying SN9C10X.";
											if(PIXELFORMAT_NONE == setPixelFormat(PIXELFORMAT_SN9C10X) && errno != EBUSY)
											{
												qDebug() <<  "Device doesn't seem to support BGR32 format. Fallback to it is not yet implemented.";
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
		if (errno == EBUSY) {
			qDebug() << "Can't change the video size: device in use";
			return EXIT_FAILURE;
		}

		if(newwidth  > maxwidth ) newwidth  = maxwidth;
		if(newheight > maxheight) newheight = maxheight;
		if(newwidth  < minwidth ) newwidth  = minwidth;
		if(newheight < minheight) newheight = minheight;

		currentwidth  = newwidth;
		currentheight = newheight;

//qDebug() << "width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << width() << "x" << height();
// Change resolution for the video device
		switch(m_driver)
		{
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
			case VIDEODEV_DRIVER_V4L2:
//				CLEAR (fmt);
				if (-1 == xioctl (VIDIOC_G_FMT, &fmt))
				{
					qDebug() << "VIDIOC_G_FMT failed (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
				}
				fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				fmt.fmt.pix.width       = width();
				fmt.fmt.pix.height      = height();
				fmt.fmt.pix.field       = V4L2_FIELD_ANY;
				if (-1 == xioctl (VIDIOC_S_FMT, &fmt))
				{
					qDebug() << "VIDIOC_S_FMT failed (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
					// Note VIDIOC_S_FMT may change width and height.
				}
				else
				{
// Buggy driver paranoia.
qDebug() << "VIDIOC_S_FMT worked (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
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
#ifdef COMPILE_V4L1_CODE
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
qDebug() << "------------- width: " << V4L_videowindow.width << " Height: " << V4L_videowindow.height << " Clipcount: " << V4L_videowindow.clipcount << " -----------------";

//				qDebug() << "libkopete (avdevice): V4L_picture.palette: " << V4L_picture.palette << " Depth: " << V4L_picture.depth;

/*				if(-1 == xioctl(VIDIOCGFBUF,&V4L_videobuffer))
					qDebug() << "libkopete (avdevice): VIDIOCGFBUF failed (" << errno << "): Device cannot stream";*/

				}
				break;
#endif //COMPILE_V4L1_CODE
#endif
			case VIDEODEV_DRIVER_NONE:
			default:
				break;
		}
		m_buffer_size = width() * height() * pixelFormatDepth(m_pixelformat) / 8;
qDebug() << "------------------------- ------- -- m_buffer_size: " << m_buffer_size << " !!! -- ------- -----------------------------------------";

		m_currentbuffer.pixelformat=m_pixelformat;
		m_currentbuffer.data.resize(m_buffer_size);

		switch (m_io_method)
		{
			case IO_METHOD_NONE:                    break;
			case IO_METHOD_READ:    initRead ();    break;
			case IO_METHOD_MMAP:    initMmap ();    break;
			case IO_METHOD_USERPTR: initUserptr (); break;
		}

qDebug() << "setSize(" << newwidth << ", " << newheight << ") exited successfuly.";
		return EXIT_SUCCESS;
	}
qDebug() << "setSize(" << newwidth << ", " << newheight << ") Device is not open.";
	return EXIT_FAILURE;
}













pixel_format VideoDevice::setPixelFormat(pixel_format newformat)
{
	pixel_format ret = PIXELFORMAT_NONE;
//qDebug() << "called.";
// Change the pixel format for the video device
	switch(m_driver)
	{
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
//			CLEAR (fmt);
			if (-1 == xioctl (VIDIOC_G_FMT, &fmt))
                        {
//				return errnoReturn ("VIDIOC_S_FMT");
//				qDebug() << "VIDIOC_G_FMT failed (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
			}
			else
				m_pixelformat = pixelFormatForPalette(fmt.fmt.pix.pixelformat);
		
			fmt.fmt.pix.pixelformat = pixelFormatCode(newformat);
			if (-1 == xioctl (VIDIOC_S_FMT, &fmt))
			{
//				qDebug() << "VIDIOC_S_FMT failed (" << errno << ").Returned width: " << pixelFormatName(fmt.fmt.pix.pixelformat) << " " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
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
#ifdef COMPILE_V4L1_CODE
		case VIDEODEV_DRIVER_V4L:
			{
			struct video_picture V4L_picture;
			if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
			{
				qDebug() << "VIDIOCGPICT failed (" << errno << ").";
			}
//			qDebug() << "V4L_picture.palette: " << V4L_picture.palette << " Depth: " << V4L_picture.depth;
			V4L_picture.palette = pixelFormatCode(newformat);
			V4L_picture.depth   = pixelFormatDepth(newformat);
			if(-1 == xioctl(VIDIOCSPICT,&V4L_picture))
			{
//				qDebug() << "Device seems to not support " << pixelFormatName(newformat) << " format. Fallback to it is not yet implemented.";
			}

			if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
			{
				qDebug() << "VIDIOCGPICT failed (" << errno << ").";
			}

//			qDebug() << "V4L_picture.palette: " << V4L_picture.palette << " Depth: " << V4L_picture.depth;
			m_pixelformat=pixelFormatForPalette(V4L_picture.palette);
			if (m_pixelformat == newformat)
				ret = newformat;

			}
			break;
#endif //COMPILE_V4L1_CODE
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
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
			case VIDEODEV_DRIVER_V4L2:
				if (-1 == ioctl (descriptor, VIDIOC_S_INPUT, &newinput))
				{
					perror ("VIDIOC_S_INPUT");
					return EXIT_FAILURE;
				}
				setupControls();
				break;
#endif
#ifdef COMPILE_V4L1_CODE
			case VIDEODEV_DRIVER_V4L:
				struct video_channel V4L_input;
				V4L_input.channel=newinput;
				V4L_input.norm=4; // Hey, it's plain wrong! It should be input's signal standard!
				if (-1 == ioctl (descriptor, VIDIOCSCHAN, &V4L_input))
				{
					perror ("ioctl (VIDIOCSCHAN)");
					return EXIT_FAILURE;
				}
				setupControls();
				break;
#endif //COMPILE_V4L1_CODE
#endif
			case VIDEODEV_DRIVER_NONE:
			default:
				break;
		}
		qDebug() << "Selected input " << newinput << " (" << m_input[newinput].name << ")";
		m_current_input = newinput;
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/*!
    \fn VideoDevice::startCapturing()
 */
int VideoDevice::startCapturing()
{

	qDebug() << "called.";
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
#ifndef COMPILE_DISABLE_AVDEVICE
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
#ifndef COMPILE_DISABLE_AVDEVICE
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

		qDebug() << "exited successfuly.";
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

#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
	struct v4l2_buffer v4l2buffer;
#endif
#endif
// 	qDebug() << "getFrame() called.";
	if(isOpen())
	{
		switch (m_io_method)
		{
			case IO_METHOD_NONE: // Device cannot capture frames
				return EXIT_FAILURE;
				break;
			case IO_METHOD_READ:
// 				qDebug() << "Using IO_METHOD_READ.File descriptor: " << descriptor << " Buffer address: " << &m_currentbuffer.data[0] << " Size: " << m_currentbuffer.data.size();
				if (m_currentbuffer.data.isEmpty())
					return EXIT_FAILURE;

#ifdef HAVE_LIBV4L2
				bytesread = v4l2_read (descriptor, &m_currentbuffer.data[0], m_currentbuffer.data.size());
#else
				bytesread = read (descriptor, &m_currentbuffer.data[0], m_currentbuffer.data.size());
#endif
				if (-1 == bytesread) // must verify this point with ov511 driver.
				{
					if (errno == EAGAIN)
					{
//						qDebug() << "No new frame available.";
						return EXIT_FAILURE;
					}
					else
						return errnoReturn ("read");
				}
				if((int)m_currentbuffer.data.size() < bytesread)
				{
					qDebug() << "IO_METHOD_READ returned less bytes (" << bytesread << ") than it was asked for (" << m_currentbuffer.data.size() <<").";
				}
				break;
			case IO_METHOD_MMAP:
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
				CLEAR (v4l2buffer);
				v4l2buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				v4l2buffer.memory = V4L2_MEMORY_MMAP;
				if (-1 == xioctl (VIDIOC_DQBUF, &v4l2buffer))
				{
					if (errno == EAGAIN)
					{
//						qDebug() << "No new frame available.";
						return EXIT_FAILURE;
					}
					else
						return errnoReturn ("VIDIOC_DQBUF");
				}
/*				if (v4l2buffer.index < m_streambuffers)
					return EXIT_FAILURE;*/ //it was an assert()
// qDebug() << "m_rawbuffers[" << v4l2buffer.index << "].start: " << (void *)m_rawbuffers[v4l2buffer.index].start << "   Size: " << m_currentbuffer.data.size();
				if (m_currentbuffer.data.isEmpty() ||
//					v4l2buffer.index < 0 ||  	// is always false: v4l2buffer.index is unsigned
					(uint) m_rawbuffers.size() <= v4l2buffer.index)
					return EXIT_FAILURE;

				if (m_rawbuffers[v4l2buffer.index].length < (uint)m_currentbuffer.data.size())
				{
					qDebug() <<  "Buffer size mismatch: expecting raw buffer length to be" << m_currentbuffer.data.size() << "but it was" << m_rawbuffers[v4l2buffer.index].length;
					return EXIT_FAILURE;
				}

				memcpy(&m_currentbuffer.data[0], m_rawbuffers[v4l2buffer.index].start, m_currentbuffer.data.size());
				if (-1 == xioctl (VIDIOC_QBUF, &v4l2buffer))
					return errnoReturn ("VIDIOC_QBUF");
#endif
#endif
				break;
			case IO_METHOD_USERPTR:
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
				{
					unsigned int i;
					CLEAR (v4l2buffer);
					v4l2buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					v4l2buffer.memory = V4L2_MEMORY_USERPTR;
					if (-1 == xioctl (VIDIOC_DQBUF, &v4l2buffer))
					{
						if (errno == EAGAIN)
						{
//							qDebug() << "No new frame available.";
							return EXIT_FAILURE;
						}
						else
							return errnoReturn ("VIDIOC_DQBUF");
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
// 		qDebug() << "exited successfuly.";
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
//qDebug() << "Capturing in " << pixelFormatName(m_currentbuffer.pixelformat);
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

	if (m_current_input >= m_input.size() )
		return EXIT_SUCCESS;

// Proccesses image for automatic Brightness/Contrast/Color correction
	if (m_input[m_current_input].img_softcorr_autobrightnesscontrast || m_input[m_current_input].img_softcorr_autocolor)
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
		if(!m_input[m_current_input].img_softcorr_autocolor)
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

		qDebug() << " R: " << R << " G: " << G << " B: " << B << " A: " << A << " global: " << global <<
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

	if (m_input[m_current_input].img_softcorr_vflip || m_input[m_current_input].img_softcorr_hflip)
		*qimage = qimage->mirrored(m_input[m_current_input].img_softcorr_vflip, m_input[m_current_input].img_softcorr_hflip);
  
	return EXIT_SUCCESS;
}

/*!
    \fn VideoDevice::stopCapturing()
 */
int VideoDevice::stopCapturing()
{
    /// @todo implement me
	qDebug() << "called.";
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
								qDebug() << "unable to munmap.";
							}
						}
					}
				}
#endif
				break;
		}
		qDebug() << "exited successfuly.";
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}


/*!
    \fn int VideoDevice::close()
    \return The success of the operation: EXIT_SUCCESS or EXIT_FAILURE
    \brief Closes the device
    
    Stops capturing, resets internal data and closes the device.
 */
int VideoDevice::close()
{
	qDebug() << "called.";
	if (isOpen())
	{
		qDebug() << "Device is open. Trying to properly shutdown the device.";
		stopCapturing();
#ifdef HAVE_LIBV4L2
		int ret = ::v4l2_close(descriptor);
#else
		int ret = ::close(descriptor);
#endif
		if (ret == -1)
		{
			qDebug() << "::close() failed with errno" << errno << strerror(errno);
			return EXIT_FAILURE;
		}
		else
			qDebug() << "Device successfully closed.";
	}
	descriptor = -1;
	m_numericCtrls.clear();
	m_booleanCtrls.clear();
	m_menuCtrls.clear();
	m_actionCtrls.clear();
	m_driver=VIDEODEV_DRIVER_NONE;
	return EXIT_SUCCESS;
}


/*!
    \fn QList<NumericVideoControl> VideoDevice::getSupportedNumericControls()
    \return A list of all supported numeric controls for the current input
    \brief Returns the supported numeric controls for the current input
 */
QList<NumericVideoControl> VideoDevice::getSupportedNumericControls()
{
	return m_numericCtrls;
}

/*!
    \fn QList<BooleanVideoControl> VideoDevice::getSupportedBooleanControls()
    \return A list of all supported boolean controls for the current input
    \brief Returns the supported boolean controls for the current input
 */
QList<BooleanVideoControl> VideoDevice::getSupportedBooleanControls()
{
	return m_booleanCtrls;
}

/*!
    \fn QList<MenuVideoControl> VideoDevice::getSupportedMenuControls()
    \return A list of all supported menu-controls for the current input
    \brief Returns the supported menu-controls for the current input
 */
QList<MenuVideoControl> VideoDevice::getSupportedMenuControls()
{
	return m_menuCtrls;
}

/*!
    \fn QList<ActionVideoControl> VideoDevice::getSupportedActionControls()
    \return A list of all supported action-controls for the current input
    \brief Returns the supported action-controls for the current input
 */
QList<ActionVideoControl> VideoDevice::getSupportedActionControls()
{
	return m_actionCtrls;
}

/*!
    \fn int VideoDevice::getControlValue(quint32 ctrl_id, qint32 * value)
    \param ctrl_id ID of the video-control
    \param value Pointer to the variable, which recieves the value of the querried video-control.
                 For boolean controls, the value is 0 or 1.
                 For menu-controls, the value is the index of the currently selected option.
    \return The result-code, currently EXIT_SUCCESS or EXIT_FAILURE
    \brief Reads the value of a video-control
 */
int VideoDevice::getControlValue(quint32 ctrl_id, qint32 * value)
{
	qDebug() << "Control-ID" << ctrl_id << ", Value" << &value;

	if (!isOpen())
		return EXIT_FAILURE;

#ifndef COMPILE_DISABLE_AVDEVICE
	if (ctrl_id == IMGCTRL_ID_SOFT_AUTOBRIGHTNESSCONTRASTCORR)
	{
		if (m_current_input < m_input.size() )
		{
			*value = m_input[m_current_input].img_softcorr_autobrightnesscontrast;
			qDebug() << "Reported current value is" << *value << ".";
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
	else if (ctrl_id == IMGCTRL_ID_SOFT_AUTOCOLORCORR)
	{
		if (m_current_input < m_input.size() )
		{
			*value = m_input[m_current_input].img_softcorr_autocolor;
			qDebug() << "Reported current value is" << *value << ".";
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
	else if (ctrl_id == IMGCTRL_ID_SOFT_VFLIP)
	{
		if (m_current_input < m_input.size() )
		{
			*value = m_input[m_current_input].img_softcorr_vflip;
			qDebug() << "Reported current value is" << *value << ".";
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
	else if (ctrl_id == IMGCTRL_ID_SOFT_HFLIP)
	{
		if (m_current_input < m_input.size() )
		{
			*value = m_input[m_current_input].img_softcorr_hflip;
			qDebug() << "Reported current value is" << *value << ".";
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
#endif

	switch(m_driver)
	{
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			{
				struct v4l2_queryctrl queryctrl;
				struct v4l2_control control;

				CLEAR (queryctrl);
				queryctrl.id = ctrl_id;

				if (-1 == xioctl(VIDIOC_QUERYCTRL, &queryctrl))
				{
					if (errno != EINVAL)
						qDebug() << "VIDIOC_QUERYCTRL failed (" << errno << ").";
					else
						qDebug() << "Device doesn't support the control.";
				}
				else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				{
					qDebug() << "Control is disabled.";
				}
				else
				{
					CLEAR (control);
					control.id = ctrl_id;
					if (-1 == xioctl(VIDIOC_G_CTRL, &control))
						qDebug() <<  "VIDIOC_G_CTRL failed (" << errno << ").";
					else
					{
						*value = control.value;
						qDebug() << "Reported current value is" << control.value << ".";
						return EXIT_SUCCESS;
					}
				}
			}
			break;
#endif
#ifdef COMPILE_V4L1_CODE
		case VIDEODEV_DRIVER_V4L:
			struct video_picture V4L_picture;
			if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
			{
				qDebug() << "VIDIOCGPICT failed (" << errno << ").";
				return EXIT_FAILURE;
			}
			switch (ctrl_id)
			{
				case IMGCTRL_ID_V4L1_BRIGHTNESS:
					*value = V4L_picture.brightness;
					break;
				case IMGCTRL_ID_V4L1_HUE:
					*value = V4L_picture.hue;
					break;
				case IMGCTRL_ID_V4L1_COLOR:
					*value = V4L_picture.colour;
					break;
				case IMGCTRL_ID_V4L1_CONTRAST:
					*value = V4L_picture.contrast;
					break;
				case IMGCTRL_ID_V4L1_WHITENESS:
					*value = V4L_picture.whiteness;
					break;
				default:
					return EXIT_FAILURE;
			}
			qDebug() << "Reported current value is" << *value << ".";
			return EXIT_SUCCESS;
#endif //COMPILE_V4L1_CODE
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return EXIT_FAILURE;
}

/*!
    \fn int VideoDevice::setControlValue(quint32 ctrl_id, qint32 value)
    \param ctrl_id ID of the video-control
    \param value The value that should be set.
                 For boolean controls, the value must be 0 or 1.
                 For menu-controls, the value must be the index of the option.
                 For action-controls, the value is ignored.
    \return The result-code, currently EXIT_SUCCESS or EXIT_FAILURE
    \brief Sets the value of a video-control
 */
int VideoDevice::setControlValue(quint32 ctrl_id, qint32 value)
{
	qDebug() << "Control-ID" << ctrl_id << ", Value" << value;

	if (!isOpen())
		return EXIT_FAILURE;

#ifndef COMPILE_DISABLE_AVDEVICE
	if (ctrl_id == IMGCTRL_ID_SOFT_AUTOBRIGHTNESSCONTRASTCORR)
	{
		if (m_current_input < m_input.size() )
		{
			m_input[m_current_input].img_softcorr_autobrightnesscontrast = value;
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
	else if (ctrl_id == IMGCTRL_ID_SOFT_AUTOCOLORCORR)
	{
		if (m_current_input < m_input.size() )
		{
			m_input[m_current_input].img_softcorr_autocolor = value;
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
	else if (ctrl_id == IMGCTRL_ID_SOFT_VFLIP)
	{
		if (m_current_input < m_input.size() )
		{
			m_input[m_current_input].img_softcorr_vflip = value;
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
	else if (ctrl_id == IMGCTRL_ID_SOFT_HFLIP)
	{
		if (m_current_input < m_input.size() )
		{
			m_input[m_current_input].img_softcorr_hflip = value;
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
#endif

	switch(m_driver)
	{
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			{
				struct v4l2_queryctrl queryctrl;
				struct v4l2_control control;

				CLEAR (queryctrl);
				queryctrl.id = ctrl_id;

				if (-1 == xioctl(VIDIOC_QUERYCTRL, &queryctrl))
				{
					if (errno != EINVAL)
						qDebug() << "VIDIOC_QUERYCTRL failed (" << errno << ").";
					else
						qDebug() << "Device doesn't support the control.";
				}
				else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
				{
					qDebug() << "Control is disabled.";
				}
				else
				{
					CLEAR (control);
					control.id = ctrl_id;
					if (value < queryctrl.minimum)
					{
						control.value = queryctrl.minimum;
						qDebug() << "Value exceeds lower limit ! Setting to minimum:" << control.value;
					}
					else if (value > queryctrl.maximum)
					{
						control.value = queryctrl.maximum;
						qDebug() << "Value exceeds upper limit ! Setting to maximum:" << control.value;
					}
					else
					{
						control.value = value;
					}
					if (-1 == xioctl(VIDIOC_S_CTRL, &control))
						qDebug() <<  "VIDIOC_S_CTRL failed (" << errno << ").";
					else
						return EXIT_SUCCESS;
				}
			}
			break;
#endif
#ifdef COMPILE_V4L1_CODE
		case VIDEODEV_DRIVER_V4L:
			struct video_picture V4L_picture;
			if(-1 == xioctl(VIDIOCGPICT, &V4L_picture))
			{
				qDebug() << "VIDIOCGPICT failed (" << errno << ").";
				return EXIT_FAILURE;
			}
			if (value < 0)
			{
				value = 0;
				qDebug() << "Value exceeds lower limit ! Setting to minimum: 0";
			}
			else if (value > 65535)
			{
				value = 65535;
				qDebug() << "Value exceeds upper limit ! Setting to maximum: 65535";
			}
			// TODO: consider step, too ?
			switch (ctrl_id)
			{
				case IMGCTRL_ID_V4L1_BRIGHTNESS:
					V4L_picture.brightness = value;
					break;
				case IMGCTRL_ID_V4L1_HUE:
					V4L_picture.hue = value;
					break;
				case IMGCTRL_ID_V4L1_COLOR:
					V4L_picture.colour = value;
					break;
				case IMGCTRL_ID_V4L1_CONTRAST:
					V4L_picture.contrast = value;
					break;
				case IMGCTRL_ID_V4L1_WHITENESS:
					V4L_picture.whiteness = value;
					break;
				default:
					return EXIT_FAILURE;
			}
			if(-1 == xioctl(VIDIOCSPICT,&V4L_picture))
			{
				qDebug() << "Device seems to not support adjusting image brightness. Fallback to it is not yet implemented.";
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
#endif //COMPILE_V4L1_CODE
#endif
		case VIDEODEV_DRIVER_NONE:
		default:
			break;
	}
	return EXIT_FAILURE;
}

pixel_format VideoDevice::pixelFormatForPalette( int palette )
{
	switch(m_driver)
	{
#ifndef COMPILE_DISABLE_AVDEVICE
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
#ifdef COMPILE_V4L1_CODE
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
#endif //COMPILE_V4L1_CODE
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
#ifndef COMPILE_DISABLE_AVDEVICE
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
#ifdef COMPILE_V4L1_CODE
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
#endif //COMPILE_V4L1_CODE
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
#ifndef COMPILE_DISABLE_AVDEVICE
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
#ifdef COMPILE_V4L1_CODE
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
#endif //COMPILE_V4L1_CODE
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
#ifndef COMPILE_DISABLE_AVDEVICE
#ifdef V4L2_CAP_VIDEO_CAPTURE
		case VIDEODEV_DRIVER_V4L2:
			struct v4l2_fmtdesc fmtdesc;
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
					qDebug () << fmtdesc.pixelformat << "  " << pixelFormatName(fmtdesc.pixelformat); // Need a cleanup. PixelFormatForPalette is a really bad name
					fmtdesc.index++;
				}
			}
//			break;
#endif
#ifdef COMPILE_V4L1_CODE
		case VIDEODEV_DRIVER_V4L:
// TODO: THis thing can be used to detec what pixel formats are supported in a API-independent way, but V4L2 has VIDIOC_ENUM_PIXFMT.
// The correct thing to do is to isolate these calls and do a proper implementation for V4L and another for V4L2 when this thing will be migrated to a plugin architecture.

// Packed RGB formats
			qDebug() << "Supported pixel formats:";
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB332))	{ qDebug() << pixelFormatName(PIXELFORMAT_RGB332); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB444))	{ qDebug() << pixelFormatName(PIXELFORMAT_RGB444); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB555))	{ qDebug() << pixelFormatName(PIXELFORMAT_RGB555); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB565))	{ qDebug() << pixelFormatName(PIXELFORMAT_RGB565); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB555X))	{ qDebug() << pixelFormatName(PIXELFORMAT_RGB555X); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB565X))	{ qDebug() << pixelFormatName(PIXELFORMAT_RGB565X); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_BGR24))	{ qDebug() << pixelFormatName(PIXELFORMAT_BGR24); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB24))	{ qDebug() << pixelFormatName(PIXELFORMAT_RGB24); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_BGR32))	{ qDebug() << pixelFormatName(PIXELFORMAT_BGR32); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_RGB32))	{ qDebug() << pixelFormatName(PIXELFORMAT_RGB32); }

// Bayer RGB format
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_SBGGR8))	{ qDebug() << pixelFormatName(PIXELFORMAT_SBGGR8); }

// YUV formats
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_GREY))	{ qDebug() << pixelFormatName(PIXELFORMAT_GREY); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_YUYV))	{ qDebug() << pixelFormatName(PIXELFORMAT_YUYV); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_UYVY))	{ qDebug() << pixelFormatName(PIXELFORMAT_UYVY); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_YUV420P))	{ qDebug() << pixelFormatName(PIXELFORMAT_YUV420P); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_YUV422P))	{ qDebug() << pixelFormatName(PIXELFORMAT_YUV422P); }

// Compressed formats
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_JPEG))	{ qDebug() << pixelFormatName(PIXELFORMAT_JPEG); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_MPEG))	{ qDebug() << pixelFormatName(PIXELFORMAT_MPEG); }

// Reserved formats
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_DV))		{ qDebug() << pixelFormatName(PIXELFORMAT_DV); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_ET61X251))	{ qDebug() << pixelFormatName(PIXELFORMAT_ET61X251); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_HI240))	{ qDebug() << pixelFormatName(PIXELFORMAT_HI240); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_HM12))	{ qDebug() << pixelFormatName(PIXELFORMAT_HM12); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_MJPEG))	{ qDebug() << pixelFormatName(PIXELFORMAT_MJPEG); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_PWC1))	{ qDebug() << pixelFormatName(PIXELFORMAT_PWC1); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_PWC2))	{ qDebug() << pixelFormatName(PIXELFORMAT_PWC2); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_SN9C10X))	{ qDebug() << pixelFormatName(PIXELFORMAT_SN9C10X); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_WNVA))	{ qDebug() << pixelFormatName(PIXELFORMAT_WNVA); }
			if(PIXELFORMAT_NONE != setPixelFormat(PIXELFORMAT_YYUV))	{ qDebug() << pixelFormatName(PIXELFORMAT_YYUV); }
			break;
#endif //COMPILE_V4L1_CODE
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
#ifndef COMPILE_DISABLE_AVDEVICE
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
#ifdef COMPILE_V4L1_CODE
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
#endif //COMPILE_V4L1_CODE
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
#ifndef COMPILE_DISABLE_AVDEVICE
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
#ifdef COMPILE_V4L1_CODE
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
#endif //COMPILE_V4L1_CODE
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
	qDebug() << "called.";
	if(isOpen())
	{
	switch(m_driver)
		{
#ifndef COMPILE_DISABLE_AVDEVICE
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
//						qDebug() << standard.name;
						qDebug() << signalStandardName(standard.id) << " (" << standard.id << ")" << V4L2_STD_NTSC;
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
#ifdef COMPILE_V4L1_CODE
			case VIDEODEV_DRIVER_V4L:
				break;
#endif //COMPILE_V4L1_CODE
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

	qDebug() << "called.";
	if(isOpen())
	{
		m_rawbuffers.resize(1);
		if (m_rawbuffers.size()==0)
		{
			fprintf (stderr, "Out of memory\n");
			return EXIT_FAILURE;
		}
		qDebug() << "m_buffer_size: " << m_buffer_size;

//		m_rawbuffers[0].pixelformat=m_pixelformat;
		m_rawbuffers[0].length = m_buffer_size;
		m_rawbuffers[0].start = (uchar *)malloc (m_buffer_size);

		if (!m_rawbuffers[0].start)
		{
			fprintf (stderr, "Out of memory\n");
			return EXIT_FAILURE;
		}
		qDebug() << "exited successfuly.";
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
		qDebug() << full_filename << " Trying to MMAP";
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
				qDebug() << full_filename << " does not support memory mapping";
				return EXIT_FAILURE;
			}
			else
			{
				return errnoReturn ("VIDIOC_REQBUFS");
			}
		}

		if (req.count < BUFFERS)
		{
			qDebug() << "Insufficient buffer memory on " << full_filename;
			return EXIT_FAILURE;
		}

		m_rawbuffers.resize(req.count);

		if (m_rawbuffers.size()==0)
		{
			qDebug() <<  "Out of memory";
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
		qDebug() << full_filename << " m_currentbuffer.data.size(): " << m_currentbuffer.data.size();
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
				qDebug() << full_filename << " does not support memory mapping";
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
				qDebug() <<  "Out of memory";
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


#if !defined(COMPILE_DISABLE_AVDEVICE) && defined(V4L2_CAP_VIDEO_CAPTURE)
/*!
    \fn const char * VideoDevice::getUnifiedV4L2StdCtrlName(quint32 std_ctrl_id)
    \param std_ctrl_id ID of the V4L2 standard video control
    \return The unified title of the V4L2 video control
    \brief Returns a unified title of a V4L2-(standard-)video-control and makes it translatable
 */
const char *VideoDevice::getUnifiedV4L2StdCtrlName(quint32 std_ctrl_id)
{
	switch (std_ctrl_id)
	{
		/* USER controls */
		case V4L2_CID_BRIGHTNESS: 		return __tr("Brightness");
		case V4L2_CID_CONTRAST: 		return __tr("Contrast");
		case V4L2_CID_SATURATION: 		return __tr("Saturation");
		case V4L2_CID_HUE: 			return __tr("Hue");
		case V4L2_CID_AUDIO_VOLUME: 		return __tr("Volume");
		case V4L2_CID_AUDIO_BALANCE: 		return __tr("Balance");
		case V4L2_CID_AUDIO_BASS: 		return __tr("Bass");
		case V4L2_CID_AUDIO_TREBLE: 		return __tr("Treble");
		case V4L2_CID_AUDIO_MUTE: 		return __tr("Mute");
		case V4L2_CID_AUDIO_LOUDNESS: 		return __tr("Loudness");
		case V4L2_CID_BLACK_LEVEL:		return __tr("Black Level");
		case V4L2_CID_AUTO_WHITE_BALANCE:	return __tr("Automatic White Balance");
		case V4L2_CID_DO_WHITE_BALANCE:		return __tr("Do White Balance");
		case V4L2_CID_RED_BALANCE:		return __tr("Red Balance");
		case V4L2_CID_BLUE_BALANCE:		return __tr("Blue Balance");
		case V4L2_CID_GAMMA:			return __tr("Gamma");
		case V4L2_CID_EXPOSURE:			return __tr("Exposure");
		case V4L2_CID_AUTOGAIN:			return __tr("Automatic Gain");
		case V4L2_CID_GAIN:			return __tr("Gain");
		case V4L2_CID_HFLIP:			return __tr("Horizontal Flip");
		case V4L2_CID_VFLIP:			return __tr("Vertical Flip");
		case V4L2_CID_HCENTER:			return __tr("Horizontal Center");
		case V4L2_CID_VCENTER:			return __tr("Vertical Center");
#ifdef V4L2_CID_POWER_LINE_FREQUENCY	// since kernel 2.6.25
		case V4L2_CID_POWER_LINE_FREQUENCY:	return __tr("Power Line Frequency");
		case V4L2_CID_HUE_AUTO:			return __tr("Automatic Hue");
		case V4L2_CID_WHITE_BALANCE_TEMPERATURE: return __tr("White Balance Temperature");
		case V4L2_CID_SHARPNESS:		return __tr("Sharpness");
		case V4L2_CID_BACKLIGHT_COMPENSATION:	return __tr("Backlight Compensation");
#endif
#ifdef V4L2_CID_CHROMA_AGC		// since kernel 2.6.26
		case V4L2_CID_CHROMA_AGC:		return __tr("Chroma AGC");
		case V4L2_CID_COLOR_KILLER:		return __tr("Color Killer");
#endif
#ifdef V4L2_CID_COLORFX			// since kernel 2.6.30
		case V4L2_CID_COLORFX:			return __tr("Color Effects");
#endif
#ifdef V4L2_CID_ROTATE			// since kernel 2.6.33
		case V4L2_CID_ROTATE:			return __tr("Rotate");
		case V4L2_CID_BG_COLOR:			return __tr("Background color");
#endif
		default:
			return NULL;
	}
	/* NOTE: Unification is necessary for translation, because drivers use individual descriptions
	         (different uppercase/lowercase combinations but also some strange interpretations).
	         Otherwise we would end up with too many different (or even untranslated !)strings.
	 */
	/* NOTE: We only mark the strings for translation, the translation is done for ALL controls
	         (including custom controls) at a common place
	 */
}
#endif

}

}
