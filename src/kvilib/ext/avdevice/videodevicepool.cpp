//=============================================================================
//
//   File : videodevicepool.cpp
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
    videodevicepool.cpp  -  Kopete Video Device Low-level Support

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

#include <assert.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <qdir.h>

#include "videodevice.h"
#include "videodevicepool.h"

#ifdef COMPILE_KDE_SUPPORT
	#include <solid/device.h>
	#include <solid/devicenotifier.h>
	#include <solid/deviceinterface.h>
	#include <solid/video.h>
#endif

#define CLEAR(x) memset (&(x), 0, sizeof (x))

namespace Kopete {

namespace AV {

VideoDevicePool *VideoDevicePool::s_self = NULL;
__u64 VideoDevicePool::m_clients = 0;

VideoDevicePool* VideoDevicePool::self()
{
//	kDebug() << "libkopete (avdevice): self() called";
	if (s_self == NULL)
	{
		s_self = new VideoDevicePool;
		if (s_self)
			m_clients = 0;
	}
//	kDebug() << "libkopete (avdevice): self() exited successfuly";
	return s_self;
}

VideoDevicePool::VideoDevicePool()
: m_current_device(0)
{
#ifdef COMPILE_KDE_SUPPORT
	connect( Solid::DeviceNotifier::instance(), SIGNAL(deviceAdded(const QString&)), SLOT(deviceAdded(const QString &)) );
	connect( Solid::DeviceNotifier::instance(), SIGNAL(deviceRemoved(const QString&)), SLOT(deviceRemoved(const QString &)) );
#endif
}


VideoDevicePool::~VideoDevicePool()
{
}




/*!
    \fn VideoDevicePool::open()
 */
int VideoDevicePool::open()
{
    /// @todo implement me
qDebug("VideoDevicePool::open()");
	m_ready.lock();
	if(!m_videodevice.size())
	{
		kDebug() << "open(): No devices found. Must scan for available devices." << m_current_device;
		scanDevices();
	}
	if(!m_videodevice.size())
	{
		kDebug() << "open(): No devices found. bailing out." << m_current_device;
		m_ready.unlock();
		return EXIT_FAILURE;
	}
	if(m_current_device >= m_videodevice.size())
	{
		kDebug() << "open(): Device out of scope (" << m_current_device << "). Defaulting to the first one.";
		m_current_device = 0;
	}
	int isopen = m_videodevice[currentDevice()].open();
	if ( isopen == EXIT_SUCCESS)
	{
		loadConfig(); // Temporary hack. The open() seems to clean the input parameters. Need to find a way to fix it.
		
	}
	m_clients++;
	kDebug() << "Number of clients: " << m_clients;
	m_ready.unlock();
	return isopen;
}

/*!
    \fn VideoDevicePool::open(int device)
 */
int VideoDevicePool::open(int device)
{
    /// @todo implement me
	kDebug() << "open(" << device << ") called.";
	if(device >= m_videodevice.size())
	{
		kDebug() << "open(" << device <<"): Device does not exist.";
		return EXIT_FAILURE;
	}
	close();
	kDebug() << "open(" << device << ") Setting m_current_Device to " << device;
	m_current_device = device;
	kDebug() << "open(" << device << ") Calling open().";
	saveConfig();

	return open();
}

bool VideoDevicePool::isOpen()
{
	return m_videodevice[currentDevice()].isOpen();
}

/*!
    \fn VideoDevicePool::showDeviceCapabilities(int device)
 */
int VideoDevicePool::showDeviceCapabilities(unsigned int device)
{
	return m_videodevice[device].showDeviceCapabilities();
}

int VideoDevicePool::width()
{
	return m_videodevice[currentDevice()].width();
}

int VideoDevicePool::minWidth()
{
	return m_videodevice[currentDevice()].minWidth();
}

int VideoDevicePool::maxWidth()
{
	return m_videodevice[currentDevice()].maxWidth();
}

int VideoDevicePool::height()
{
	return m_videodevice[currentDevice()].height();
}

int VideoDevicePool::minHeight()
{
	return m_videodevice[currentDevice()].minHeight();
}

int VideoDevicePool::maxHeight()
{
	return m_videodevice[currentDevice()].maxHeight();
}

int VideoDevicePool::setSize( int newwidth, int newheight)
{
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].setSize(newwidth, newheight);
	else
	{
		kDebug() << "VideoDevicePool::setSize() fallback for no device.";
		m_buffer.width=newwidth;
		m_buffer.height=newheight;
		m_buffer.pixelformat=	PIXELFORMAT_RGB24;
		m_buffer.data.resize(m_buffer.width*m_buffer.height*3);
		kDebug() << "VideoDevicePool::setSize() buffer size: "<< m_buffer.data.size();
	}
	return EXIT_SUCCESS;
}

/*!
    \fn VideoDevicePool::close()
 */
int VideoDevicePool::close()
{
    /// @todo implement me
	if(m_clients)
		m_clients--;
	if((currentDevice() < m_videodevice.size())&&(!m_clients))
		return m_videodevice[currentDevice()].close();
	if(m_clients)
		kDebug() << "VideoDevicePool::close() The video device is still in use.";
	if(currentDevice() >= m_videodevice.size())
		kDebug() << "VideoDevicePool::close() Current device out of range.";
	return EXIT_FAILURE;
}

/*!
    \fn VideoDevicePool::startCapturing()
 */
int VideoDevicePool::startCapturing()
{
	kDebug() << "startCapturing() called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].startCapturing();
	return EXIT_FAILURE;
}


/*!
    \fn VideoDevicePool::stopCapturing()
 */
int VideoDevicePool::stopCapturing()
{
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].stopCapturing();
	return EXIT_FAILURE;
}

// Implementation of the methods that get / set input's adjustment parameters
/*!
    \fn VideoDevicePool::getBrightness()
 */
float VideoDevicePool::getBrightness()
{
  if (currentDevice() < m_videodevice.size() )
	return m_videodevice[currentDevice()].getBrightness();
  else
	return 0;
}

/*!
    \fn VideoDevicePool::setBrightness(float brightness)
 */
float VideoDevicePool::setBrightness(float brightness)
{
    if (currentDevice() < m_videodevice.size() )
	  return m_videodevice[currentDevice()].setBrightness(brightness);
	else
	  return 0;
}

/*!
    \fn VideoDevicePool::getContrast()
 */
float VideoDevicePool::getContrast()
{
    if (currentDevice() < m_videodevice.size() )
	  return m_videodevice[currentDevice()].getContrast();
	else
	  return 0;
}

/*!
    \fn VideoDevicePool::setContrast(float contrast)
 */
float VideoDevicePool::setContrast(float contrast)
{
    if (currentDevice() < m_videodevice.size() )
	  return m_videodevice[currentDevice()].setContrast(contrast);
	else
	  return 0;
}

/*!
    \fn VideoDevicePool::getSaturation()
 */
float VideoDevicePool::getSaturation()
{
    if (currentDevice() < m_videodevice.size() )
	  return m_videodevice[currentDevice()].getSaturation();
	else
	  return 0;
}

/*!
    \fn VideoDevicePool::setSaturation(float saturation)
 */
float VideoDevicePool::setSaturation(float saturation)
{
    if (currentDevice() < m_videodevice.size() )
	  return m_videodevice[currentDevice()].setSaturation(saturation);
	else
	  return 0;
}

/*!
    \fn VideoDevicePool::getWhiteness()
 */
float VideoDevicePool::getWhiteness()
{
    if (currentDevice() < m_videodevice.size() )
	  return m_videodevice[currentDevice()].getWhiteness();
	else
	  return 0;
}

/*!
    \fn VideoDevicePool::setWhiteness(float whiteness)
 */
float VideoDevicePool::setWhiteness(float whiteness)
{
    if (currentDevice() < m_videodevice.size() )
	  return m_videodevice[currentDevice()].setWhiteness(whiteness);
	else
	  return 0;
}

/*!
    \fn VideoDevicePool::getHue()
 */
float VideoDevicePool::getHue()
{
    if (currentDevice() < m_videodevice.size() )
	  return m_videodevice[currentDevice()].getHue();
	else
	  return 0;
}

/*!
    \fn VideoDevicePool::setHue(float hue)
 */
float VideoDevicePool::setHue(float hue)
{
    if (currentDevice() < m_videodevice.size() )
	  return m_videodevice[currentDevice()].setHue(hue);
	else
	  return 0;
}

/*!
    \fn VideoDevicePool::getAutoBrightnessContrast()
 */
bool VideoDevicePool::getAutoBrightnessContrast()
{
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].getAutoBrightnessContrast();
	return false;
}

/*!
    \fn VideoDevicePool::setAutoBrightnessContrast(bool brightnesscontrast)
 */
bool VideoDevicePool::setAutoBrightnessContrast(bool brightnesscontrast)
{
	kDebug() << "VideoDevicePool::setAutoBrightnessContrast(" << brightnesscontrast << ") called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].setAutoBrightnessContrast(brightnesscontrast);
	return false;
}

/*!
    \fn VideoDevicePool::getAutoColorCorrection()
 */
bool VideoDevicePool::getAutoColorCorrection()
{
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].getAutoColorCorrection();
	return false;
}

/*!
    \fn VideoDevicePool::setAutoColorCorrection(bool colorcorrection)
 */
bool VideoDevicePool::setAutoColorCorrection(bool colorcorrection)
{
	kDebug() << "VideoDevicePool::setAutoColorCorrection(" << colorcorrection << ") called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].setAutoColorCorrection(colorcorrection);
	return false;
}

/*!
    \fn VideoDevicePool::getIMageAsMirror()
 */
bool VideoDevicePool::getImageAsMirror()
{
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].getImageAsMirror();
	return false;
}

/*!
    \fn VideoDevicePool::setImageAsMirror(bool imageasmirror)
 */
bool VideoDevicePool::setImageAsMirror(bool imageasmirror)
{
	kDebug() << "VideoDevicePool::setImageAsMirror(" << imageasmirror << ") called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].setImageAsMirror(imageasmirror);
	return false;
}

/*!
    \fn VideoDevicePool::getFrame()
 */
int VideoDevicePool::getFrame()
{
//	kDebug() << "VideoDevicePool::getFrame() called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].getFrame();
	else
	{
		kDebug() << "VideoDevicePool::getFrame() fallback for no device.";
		for(int loop=0; loop < m_buffer.data.size(); loop+=3)
		{
			m_buffer.data[loop]   = 255;
			m_buffer.data[loop+1] = 0;
			m_buffer.data[loop+2] = 0;
		}
	}
	kDebug() << "VideoDevicePool::getFrame() exited successfuly.";

	return EXIT_SUCCESS;
}

/*!
    \fn VideoDevicePool::getQImage(QImage *qimage)
 */
int VideoDevicePool::getImage(QImage *qimage)
{
//	kDebug() << "VideoDevicePool::getImage() called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].getImage(qimage);
	else
	{
		kDebug() << "VideoDevicePool::getImage() fallback for no device.";

		// do NOT delete qimage here, as it is received as a parameter
		if (qimage->width() != width() || qimage->height() != height())
			*qimage = QImage(width(), height(), QImage::Format_RGB32);

		uchar *bits=qimage->bits();
		switch(m_buffer.pixelformat)
		{
			case PIXELFORMAT_NONE	: break;
			case PIXELFORMAT_GREY	: break;
			case PIXELFORMAT_RGB332	: break;
			case PIXELFORMAT_RGB555	: break;
			case PIXELFORMAT_RGB555X: break;
			case PIXELFORMAT_RGB565	: break;
			case PIXELFORMAT_RGB565X: break;
			case PIXELFORMAT_RGB24	:
				{
					kDebug() << "VideoDevicePool::getImage() fallback for no device - RGB24.";
					int step=0;
					for(int loop=0;loop < qimage->numBytes();loop+=4)
					{
						bits[loop]   = m_buffer.data[step];
						bits[loop+1] = m_buffer.data[step+1];
						bits[loop+2] = m_buffer.data[step+2];
						bits[loop+3] = 255;
						step+=3;
					}
				}
				break;
			case PIXELFORMAT_BGR24	: break;
				{
					int step=0;
					for(int loop=0;loop < qimage->numBytes();loop+=4)
					{
						bits[loop]   = m_buffer.data[step+2];
						bits[loop+1] = m_buffer.data[step+1];
						bits[loop+2] = m_buffer.data[step];
						bits[loop+3] = 255;
						step+=3;
					}
				}
				break;
			case PIXELFORMAT_RGB32	: memcpy(bits,&m_buffer.data[0], m_buffer.data.size());
				break;
			case PIXELFORMAT_BGR32	: break;
			case PIXELFORMAT_YUYV   : break;
			case PIXELFORMAT_UYVY   : break;
			case PIXELFORMAT_YUV420P: break;
			case PIXELFORMAT_YUV422P: break;
			default: break;
		}
	}
	kDebug() << "VideoDevicePool::getImage() exited successfuly.";
	return EXIT_SUCCESS;
}

/*!
    \fn Kopete::AV::VideoDevicePool::selectInput(int input)
 */
int VideoDevicePool::selectInput(int newinput)
{
	kDebug() << "VideoDevicePool::selectInput(" << newinput << ") called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].selectInput(newinput);
	else
		return 0;
}

/*!
    \fn Kopete::AV::VideoDevicePool::setInputParameters()
 */
int VideoDevicePool::setInputParameters()
{
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].setInputParameters();
	else
		return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevicePool::fillDeviceQComboBox(QComboBox *combobox)
 */
int VideoDevicePool::fillDeviceQComboBox(QComboBox *combobox)
{
    /// @todo implement me
	kDebug() << "Called.";
// check if QComboBox is a valid pointer.
	if (combobox != NULL)
	{
		combobox->clear();
		kDebug() << "Combobox cleaned.";
		if(m_videodevice.size())
		{
			for (int loop=0; loop < m_videodevice.size(); loop++)
			{
				combobox->addItem(m_videodevice[loop].m_name);
				kDebug() << "Added device " << loop << ": " << m_videodevice[loop].m_name;
			}
			combobox->setCurrentIndex(currentDevice());
			return EXIT_SUCCESS;
		}
	}
	return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevicePool::fillInputQComboBox(QComboBox *combobox)
 */
int VideoDevicePool::fillInputQComboBox(QComboBox *combobox)
{
    /// @todo implement me
	kDebug() << "Called.";
	if (combobox != NULL)
	{
		combobox->clear();
		if ( !m_videodevice.isEmpty() && (currentDevice()>=0) && currentDevice() < m_videodevice.size() )
		{
			if(m_videodevice[currentDevice()].inputs()>0)
			{
				for (int loop=0; loop < m_videodevice[currentDevice()].inputs(); loop++)
				{
					combobox->addItem(m_videodevice[currentDevice()].m_input[loop].name);
					kDebug() << "Added input " << loop << ": " << m_videodevice[currentDevice()].m_input[loop].name << " (tuner: " << m_videodevice[currentDevice()].m_input[loop].hastuner << ")";
				}
				combobox->setCurrentIndex(currentInput());
				return EXIT_SUCCESS;
			}
		}
	}
	return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevicePool::fillStandardQComboBox(QComboBox *combobox)
 */
int VideoDevicePool::fillStandardQComboBox(QComboBox *combobox)
{
    /// @todo implement me
	kDebug() << "Called.";
	if (combobox != NULL)
	{
		combobox->clear();
		if ( !m_videodevice.isEmpty() && currentDevice() < m_videodevice.size() )
		{
			if(m_videodevice[currentDevice()].inputs()>0)
			{
				for (unsigned int loop=0; loop < 25; loop++)
				{
					if ( (m_videodevice[currentDevice()].m_input[currentInput()].m_standards) & (1 << loop) )
						combobox->addItem(m_videodevice[currentDevice()].signalStandardName( 1 << loop));
/*
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
				case STANDARD_NTSC_443	: return V4L2_STD_NTSC;		break; // Using workaround value because my videodev2.h header seems to not include this standard in struct __u64 v4l2_std_id
				case STANDARD_SECAM_B	: return V4L2_STD_SECAM_B;	break;
				case STANDARD_SECAM_D	: return V4L2_STD_SECAM_D;	break;
				case STANDARD_SECAM_G	: return V4L2_STD_SECAM_G;	break;
				case STANDARD_SECAM_H	: return V4L2_STD_SECAM_H;	break;
				case STANDARD_SECAM_K	: return V4L2_STD_SECAM_K;	break;
				case STANDARD_SECAM_K1	: return V4L2_STD_SECAM_K1;	break;
				case STANDARD_SECAM_L	: return V4L2_STD_SECAM_L;	break;
				case STANDARD_SECAM_LC	: return V4L2_STD_SECAM;	break; // Using workaround value because my videodev2.h header seems to not include this standard in struct __u64 v4l2_std_id
				case STANDARD_ATSC_8_VSB	: return V4L2_STD_ATSC_8_VSB;	break; // ATSC/HDTV Standard officially not supported by V4L2 but exists in videodev2.h
				case STANDARD_ATSC_16_VSB	: return V4L2_STD_ATSC_16_VSB;	break; // ATSC/HDTV Standard officially not supported by V4L2 but exists in videodev2.h
				case STANDARD_PAL_BG	: return V4L2_STD_PAL_BG;	break;
				case STANDARD_PAL_DK	: return V4L2_STD_PAL_DK;	break;
				case STANDARD_PAL	: return V4L2_STD_PAL;		break;
				case STANDARD_NTSC	: return V4L2_STD_NTSC;		break;
				case STANDARD_SECAM_DK	: return V4L2_STD_SECAM_DK;	break;
				case STANDARD_SECAM	: return V4L2_STD_SECAM;	break;
				case STANDARD_525_60	: return V4L2_STD_525_60;	break;
				case STANDARD_625_50	: return V4L2_STD_625_50;	break;
				case STANDARD_ALL	: return V4L2_STD_ALL;		break;

				combobox->insertItem(m_videodevice[currentDevice()].m_input[loop].name);
				kDebug() << "StandardQComboBox: Added input " << loop << ": " << m_videodevice[currentDevice()].m_input[loop].name << " (tuner: " << m_videodevice[currentDevice()].m_input[loop].hastuner << ")";*/
				}
				combobox->setCurrentIndex(currentInput());
				return EXIT_SUCCESS;
			}
		}
	}
	return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevicePool::scanDevices()
 */
int VideoDevicePool::scanDevices()
{
    /// @todo implement me

	kDebug() << "called";
#ifdef COMPILE_KDE_SUPPORT
	foreach (Solid::Device device,
			Solid::Device::listFromType(Solid::DeviceInterface::Video, QString())) {
		registerDevice( device );
	}

#endif
	kDebug() << "exited successfuly";
	return EXIT_SUCCESS;
}

#ifdef COMPILE_KDE_SUPPORT

void VideoDevicePool::registerDevice( Solid::Device & device )
{
	kDebug() << "New video device at " << device.udi();
	const Solid::Device * vendorDevice = &device;
	while ( vendorDevice->isValid() && vendorDevice->vendor().isEmpty() )
	{
		vendorDevice = new Solid::Device( vendorDevice->parentUdi() );
	}
	if ( vendorDevice->isValid() )
	{
		kDebug() << "vendor: " << vendorDevice->vendor() << ", product: " << vendorDevice->product();
	}
	Solid::Video * solidVideoDevice = device.as<Solid::Video>();
	if ( solidVideoDevice ) {
		QStringList protocols = solidVideoDevice->supportedProtocols();
		if ( protocols.contains( "video4linux" ) )
		{
			QStringList drivers = solidVideoDevice->supportedDrivers( "video4linux" );
			if ( drivers.contains( "video4linux" ) )
			{
				kDebug() << "V4L device path is" << solidVideoDevice->driverHandle( "video4linux" ).toString();
				VideoDevice videodevice;
				videodevice.setUdi( device.udi() );
				videodevice.setFileName(solidVideoDevice->driverHandle( "video4linux" ).toString());
				kDebug() << "Found device " << videodevice.full_filename;
				videodevice.open();
				if(videodevice.isOpen())
				{
					kDebug() << "File " << videodevice.full_filename << " was opened successfuly";
					videodevice.close();
					videodevice.m_modelindex=m_modelvector.addModel (videodevice.m_model); // Adds device to the device list and sets model number
					m_videodevice.push_back(videodevice);
				}
			}
		}
	}
}
#endif

/*!
    \fn Kopete::AV::VideoDevicePool::hasDevices()
 */
bool VideoDevicePool::hasDevices()
{
    /// @todo implement me
	if(m_videodevice.size())
		return true;
	return false;
}

/*!
    \fn Kopete::AV::VideoDevicePool::size()
 */
size_t VideoDevicePool::size()
{
    /// @todo implement me
	return m_videodevice.size();
}

/*!
    \fn Kopete::AV::VideoDevicePool::currentDevice()
 */
int VideoDevicePool::currentDevice()
{
    /// @todo implement me
	return m_current_device;
}

/*!
    \fn Kopete::AV::VideoDevicePool::currentInput()
 */
int VideoDevicePool::currentInput()
{
    /// @todo implement me
	return m_videodevice[currentDevice()].currentInput();
}

/*!
    \fn Kopete::AV::VideoDevicePool::currentInput()
 */
unsigned int VideoDevicePool::inputs()
{
    /// @todo implement me
	return m_videodevice[currentDevice()].inputs();
}

/*!
    \fn Kopete::AV::VideoDevicePool::loadConfig()
 */
void VideoDevicePool::loadConfig()
{
    /// @todo implement me
	kDebug() << "called";
	if((hasDevices())&&(m_clients==0))
	{
		KConfigGroup config(KGlobal::config(), "Video Device Settings");
		QString currentdevice = config.readEntry("Current Device", QString());
		kDebug() << "Current device: " << currentdevice;

//		m_current_device = 0; // Must check this thing because of the fact that multiple loadConfig in other methodas can do bad things. Watch out!

		VideoDeviceVector::iterator vditerator;
		for( vditerator = m_videodevice.begin(); vditerator != m_videodevice.end(); ++vditerator )
		{
			const QString modelindex = QString::fromLocal8Bit ( "Model %1 Device %2")  .arg ((*vditerator).m_name ) .arg ((*vditerator).m_modelindex);
			if(modelindex == currentdevice)
			{
				m_current_device = std::distance (m_videodevice.begin(), vditerator);
//				kDebug() << "This place will be used to set " << modelindex << " as the current device ( " << std::distance(m_videodevice.begin(), vditerator ) << " ).";
			}
			const QString name                = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Name")  .arg ((*vditerator).m_name ) .arg ((*vditerator).m_modelindex)), (*vditerator).m_model);
			const int currentinput            = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Current input")  .arg ((*vditerator).m_name ) .arg ((*vditerator).m_modelindex)), 0);
			kDebug() << "Device name: " << name;
			kDebug() << "Device current input: " << currentinput;
			(*vditerator).selectInput(currentinput);

			for (int input = 0 ; input < (*vditerator).m_input.size(); input++)
			{
				const float brightness = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Brightness").arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input)) , 0.5 );
				const float contrast   = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Contrast")  .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input)) , 0.5 );
				const float saturation = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Saturation").arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input)) , 0.5 );
				const float whiteness  = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Whiteness") .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input)) , 0.5 );
				const float hue        = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Hue")       .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input)) , 0.5 );
				const bool  autobrightnesscontrast = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 AutoBrightnessContrast") .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input)) , false );
				const bool  autocolorcorrection    = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 AutoColorCorrection")    .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input)) , false );
				const bool  imageasmirror          = config.readEntry((QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 mageAsMirror")           .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input)) , false );
				(*vditerator).setBrightness(brightness);
				(*vditerator).setContrast(contrast);
				(*vditerator).setSaturation(saturation);
				(*vditerator).setHue(hue);
				(*vditerator).setAutoBrightnessContrast(autobrightnesscontrast);
				(*vditerator).setAutoColorCorrection(autocolorcorrection);
				(*vditerator).setImageAsMirror(imageasmirror);
				kDebug() << "Brightness:" << brightness;
				kDebug() << "Contrast  :" << contrast;
				kDebug() << "Saturation:" << saturation;
				kDebug() << "Whiteness :" << whiteness;
				kDebug() << "Hue       :" << hue;
				kDebug() << "AutoBrightnessContrast:" << autobrightnesscontrast;
				kDebug() << "AutoColorCorrection   :" << autocolorcorrection;
				kDebug() << "ImageAsMirror         :" << imageasmirror;
			}
		}
	}
}

/*!
    \fn Kopete::AV::VideoDevicePool::saveConfig()
 */
void VideoDevicePool::saveConfig()
{
    /// @todo implement me
	kDebug() << "called";
	if(hasDevices())
	{
		KConfigGroup config(KGlobal::config(), "Video Device Settings");

/*		if(m_modelvector.size())
		{
			VideoDeviceModelPool::m_devicemodel::iterator vmiterator;
			for( vmiterator = m_modelvector.begin(); vmiterator != m_modelvector.end(); ++vmiterator )
			{
				kDebug() << "Device Model: " << (*vmiterator).name;
				kDebug() << "Device Count: " << (*vmiterator).count;
			}
		}
*/
// Stores what is the current video device in use
		const QString currentdevice = QString::fromLocal8Bit ( "Model %1 Device %2" ) .arg(m_videodevice[m_current_device].m_model) .arg(m_videodevice[m_current_device].m_modelindex);
		config.writeEntry( "Current Device", currentdevice);

		VideoDeviceVector::iterator vditerator;
		for( vditerator = m_videodevice.begin(); vditerator != m_videodevice.end(); ++vditerator )
		{
			kDebug() << "Model:" << (*vditerator).m_model << ":Index:" << (*vditerator).m_modelindex << ":Name:" << (*vditerator).m_name;
			kDebug() << "Model:" << (*vditerator).m_model << ":Index:" << (*vditerator).m_modelindex << ":Current input:" << (*vditerator).currentInput();

// Stores current input for the given video device
			const QString name                   = QString::fromLocal8Bit ( "Model %1 Device %2 Name")  .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex);
			const QString currentinput           = QString::fromLocal8Bit ( "Model %1 Device %2 Current input")  .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex);
			config.writeEntry( name,                   (*vditerator).m_name);
			config.writeEntry( currentinput,           (*vditerator).currentInput());

			for (int input = 0 ; input < (*vditerator).m_input.size(); input++)
			{
				kDebug() << "Model:" << (*vditerator).m_model << ":Index:" << (*vditerator).m_modelindex << ":Input:" << input << ":Brightness: " << (*vditerator).m_input[input].getBrightness();
				kDebug() << "Model:" << (*vditerator).m_model << ":Index:" << (*vditerator).m_modelindex << ":Input:" << input << ":Contrast  : " << (*vditerator).m_input[input].getContrast();
				kDebug() << "Model:" << (*vditerator).m_model << ":Index:" << (*vditerator).m_modelindex << ":Input:" << input << ":Saturation: " << (*vditerator).m_input[input].getSaturation();
				kDebug() << "Model:" << (*vditerator).m_model << ":Index:" << (*vditerator).m_modelindex << ":Input:" << input << ":Whiteness : " << (*vditerator).m_input[input].getWhiteness();
				kDebug() << "Model:" << (*vditerator).m_model << ":Index:" << (*vditerator).m_modelindex << ":Input:" << input << ":Hue       : " << (*vditerator).m_input[input].getHue();
				kDebug() << "Model:" << (*vditerator).m_model << ":Index:" << (*vditerator).m_modelindex << ":Input:" << input << ":Automatic brightness / contrast: " << (*vditerator).m_input[input].getAutoBrightnessContrast();
				kDebug() << "Model:" << (*vditerator).m_model << ":Index:" << (*vditerator).m_modelindex << ":Input:" << input << ":Automatic color correction     : " << (*vditerator).m_input[input].getAutoColorCorrection();

// Stores configuration about each channel
				const QString brightness             = QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Brightness")             .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input);
				const QString contrast               = QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Contrast")               .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input);
				const QString saturation             = QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Saturation")             .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input);
				const QString whiteness              = QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Whiteness")              .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input);
				const QString hue                    = QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 Hue")                    .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input);
				const QString autobrightnesscontrast = QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 AutoBrightnessContrast") .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input);
				const QString autocolorcorrection    = QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 AutoColorCorrection")    .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input);
				const QString imageasmirror          = QString::fromLocal8Bit ( "Model %1 Device %2 Input %3 ImageAsMirror")          .arg ((*vditerator).m_model ) .arg ((*vditerator).m_modelindex) .arg (input);
				config.writeEntry( brightness,             (double)(*vditerator).m_input[input].getBrightness());
				config.writeEntry( contrast,               (double)(*vditerator).m_input[input].getContrast());
				config.writeEntry( saturation,             (double)(*vditerator).m_input[input].getSaturation());
				config.writeEntry( whiteness,              (double)(*vditerator).m_input[input].getWhiteness());
				config.writeEntry( hue,                    (double)(*vditerator).m_input[input].getHue());
				config.writeEntry( autobrightnesscontrast, (*vditerator).m_input[input].getAutoBrightnessContrast());
				config.writeEntry( autocolorcorrection,    (*vditerator).m_input[input].getAutoColorCorrection());
				config.writeEntry( imageasmirror,          (*vditerator).m_input[input].getImageAsMirror());
			}
		}
		config.sync();
		kDebug();
	}
}

#ifdef COMPILE_KDE_SUPPORT

void VideoDevicePool::deviceAdded( const QString & udi )
{
	kDebug() << "("<< udi << ") called";
	Solid::Device dev( udi );
	if ( dev.is<Solid::Video>() )
	{
		registerDevice( dev );
		emit deviceRegistered( udi );
	}
}

void VideoDevicePool::deviceRemoved( const QString & udi )
{
	kDebug() << "("<< udi << ") called";
	int i = 0;
	foreach ( VideoDevice vd, m_videodevice )
	{
		if ( vd.udi() == udi )
		{
			kDebug() << "Video device '" << udi << "' has been removed!";
			emit deviceUnregistered( udi );
			// not sure if this is safe but at this point the device node is gone already anyway
			m_videodevice.remove( i );
		}
		else
		{
			i++;
		}
	}
}

#endif

} // namespace AV

} // namespace Kopete
