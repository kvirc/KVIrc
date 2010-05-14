/*
    videodevicepool.cpp  -  Kopete Video Device Low-level Support

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

#define ENABLE_AV

#include <assert.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <qdir.h>
#include <solid/device.h>
#include <solid/devicenotifier.h>
#include <solid/deviceinterface.h>
#include <solid/video.h>


#include "videodevice.h"
#include "videodevicepool.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

namespace Kopete {

namespace AV {

VideoDevicePool *VideoDevicePool::s_self = NULL;
__u64 VideoDevicePool::m_clients = 0;

VideoDevicePool* VideoDevicePool::self()
{
//	qDebug() << "libkopete (avdevice): self() called";
	if (s_self == NULL)
	{
		s_self = new VideoDevicePool;
		if (s_self)
			m_clients = 0;
	}
//	qDebug() << "libkopete (avdevice): self() exited successfuly";
	return s_self;
}

VideoDevicePool::VideoDevicePool()
: m_current_device(0)
{
	connect( Solid::DeviceNotifier::instance(), SIGNAL(deviceAdded(const QString&)), SLOT(deviceAdded(const QString &)) );
	connect( Solid::DeviceNotifier::instance(), SIGNAL(deviceRemoved(const QString&)), SLOT(deviceRemoved(const QString &)) );
}


VideoDevicePool::~VideoDevicePool()
{
}




/*!
    \fn VideoDevicePool::open(int device)
 */
int VideoDevicePool::open(int device)
{
    /// @todo implement me
	qDebug() << "called with device" << device;
	m_ready.lock();
	if (!m_videodevice.size())
	{
		qDebug() << "open(): No devices found. Must scan for available devices." << m_current_device;
		scanDevices();
	}
	if (!m_videodevice.size() || (device >= m_videodevice.size()))
	{
		qDebug() << "open(): Device not found. bailing out." << m_current_device;
		m_ready.unlock();
		return EXIT_FAILURE;
	}
	int current_device = m_current_device;
	if (device < 0)
	{
		qDebug() << "Trying to load saved device (using default device if not available)";
		loadSelectedDevice();	// Set m_current_device to saved device (if device available)
	}
	else
		m_current_device = device;
	int isopen = EXIT_FAILURE;
	if ((m_current_device != current_device) || !isOpen())
	{
		if (isOpen())
		{
			if (EXIT_SUCCESS == m_videodevice[current_device].close())
				m_clients--;
			else
			{
				m_ready.unlock();
				return EXIT_FAILURE;
			}
		}
		isopen = m_videodevice[m_current_device].open();
		if (isopen == EXIT_SUCCESS)
		{
			loadDeviceConfig(); // Load and apply device parameters
			m_clients++;
		}
	}
	else
	{
		isopen = EXIT_SUCCESS;
		m_clients++;
	}
	qDebug() << "Number of clients: " << m_clients;
	m_ready.unlock();
	return isopen;
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
		qDebug() << "VideoDevicePool::setSize() fallback for no device.";
		m_buffer.width=newwidth;
		m_buffer.height=newheight;
		m_buffer.pixelformat=	PIXELFORMAT_RGB24;
		m_buffer.data.resize(m_buffer.width*m_buffer.height*3);
		qDebug() << "VideoDevicePool::setSize() buffer size: "<< m_buffer.data.size();
	}
	return EXIT_SUCCESS;
}

/*!
    \fn int VideoDevicePool::close()
    \return The success of the operation: EXIT_SUCCESS or EXIT_FAILURE
    \brief Closes the device
 */
int VideoDevicePool::close()
{
	int ret = EXIT_FAILURE;
	if (m_current_device >= m_videodevice.size())
	{
		qDebug() << "Current device out of range.";
	}
	else if (!m_clients)
	{
		ret = EXIT_SUCCESS;
	}
	else if (m_clients > 1)
	{
		m_clients--;
		qDebug() << "The video device is still in use:" << m_clients << "clients";
		ret = EXIT_SUCCESS;
	}
	else
	{
		ret = m_videodevice[m_current_device].close();
		if (EXIT_SUCCESS == ret)
			m_clients--;
	}
	return ret;
}

/*!
    \fn VideoDevicePool::startCapturing()
 */
int VideoDevicePool::startCapturing()
{
	qDebug() << "startCapturing() called.";
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
    \fn QList<NumericVideoControl> VideoDevicePool::getSupportedNumericControls()
    \return A list of all supported numeric controls for the current input
    \brief Returns the supported numeric controls for the current input
 */
QList<NumericVideoControl> VideoDevicePool::getSupportedNumericControls()
{
	if (currentDevice() < m_videodevice.size() )
		return m_videodevice[currentDevice()].getSupportedNumericControls();
	else
		return QList<NumericVideoControl>();
}

/*!
    \fn QList<BooleanVideoControl> VideoDevicePool::getSupportedBooleanControls()
    \return A list of all supported boolean controls for the current input
    \brief Returns the supported boolean controls for the current input
 */
QList<BooleanVideoControl> VideoDevicePool::getSupportedBooleanControls()
{
	if (currentDevice() < m_videodevice.size() )
		return m_videodevice[currentDevice()].getSupportedBooleanControls();
	else
		return QList<BooleanVideoControl>();
}

/*!
    \fn QList<MenuVideoControl> VideoDevicePool::getSupportedMenuControls()
    \return A list of all supported menu-controls for the current input
    \brief Returns the supported menu-controls for the current input
 */
QList<MenuVideoControl> VideoDevicePool::getSupportedMenuControls()
{
	if (currentDevice() < m_videodevice.size() )
		return m_videodevice[currentDevice()].getSupportedMenuControls();
	else
		return QList<MenuVideoControl>();
}

/*!
    \fn QList<ActionVideoControl> VideoDevicePool::getSupportedActionControls()
    \return A list of all supported action-controls for the current input
    \brief Returns the supported action-controls for the current input
 */
QList<ActionVideoControl> VideoDevicePool::getSupportedActionControls()
{
	if (currentDevice() < m_videodevice.size() )
		return m_videodevice[currentDevice()].getSupportedActionControls();
	else
		return QList<ActionVideoControl>();
}

/*!
    \fn int VideoDevicePool::getControlValue(quint32 ctrl_id, qint32 * value)
    \param ctrl_id ID of the video-control
    \param value Pointer to the variable, which recieves the value of the querried video-control.
                 For boolean controls, the value is 0 or 1.
                 For menu-controls, the value is the index of the currently selected option.
    \return The result-code, currently EXIT_SUCCESS or EXIT_FAILURE
    \brief Reads the value of a video-control
 */
int VideoDevicePool::getControlValue(quint32 ctrl_id, qint32 * value)
{
	if (currentDevice() < m_videodevice.size() )
		return m_videodevice[currentDevice()].getControlValue(ctrl_id, value);
	else
		return EXIT_FAILURE;
}

/*!
    \fn int VideoDevicePool::setControlValue(quint32 ctrl_id, qint32 value)
    \param ctrl_id ID of the video-control
    \param value The value that should be set.
                 For boolean controls, the value must be 0 or 1.
                 For menu-controls, the value must be the index of the option.
                 For action-controls, the value is ignored.
    \return The result-code, currently EXIT_SUCCESS or EXIT_FAILURE
    \brief Sets the value of a video-control
 */
int VideoDevicePool::setControlValue(quint32 ctrl_id, qint32 value)
{
	if (currentDevice() < m_videodevice.size() )
		return m_videodevice[currentDevice()].setControlValue(ctrl_id, value);
	else
		return EXIT_FAILURE;
}


/*!
    \fn VideoDevicePool::getFrame()
 */
int VideoDevicePool::getFrame()
{
//	qDebug() << "VideoDevicePool::getFrame() called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].getFrame();
	else
	{
		qDebug() << "VideoDevicePool::getFrame() fallback for no device.";
		for(int loop=0; loop < m_buffer.data.size(); loop+=3)
		{
			m_buffer.data[loop]   = 255;
			m_buffer.data[loop+1] = 0;
			m_buffer.data[loop+2] = 0;
		}
	}
	qDebug() << "VideoDevicePool::getFrame() exited successfuly.";

	return EXIT_SUCCESS;
}

/*!
    \fn VideoDevicePool::getQImage(QImage *qimage)
 */
int VideoDevicePool::getImage(QImage *qimage)
{
//	qDebug() << "VideoDevicePool::getImage() called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].getImage(qimage);
	else
	{
		qDebug() << "VideoDevicePool::getImage() fallback for no device.";

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
					qDebug() << "VideoDevicePool::getImage() fallback for no device - RGB24.";
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
	qDebug() << "VideoDevicePool::getImage() exited successfuly.";
	return EXIT_SUCCESS;
}

/*!
    \fn Kopete::AV::VideoDevicePool::selectInput(int input)
 */
int VideoDevicePool::selectInput(int newinput)
{
	qDebug() << "VideoDevicePool::selectInput(" << newinput << ") called.";
	if(m_videodevice.size())
		return m_videodevice[currentDevice()].selectInput(newinput);
	else
		return 0;
}

/*!
    \fn Kopete::AV::VideoDevicePool::fillDeviceQComboBox(QComboBox *combobox)
 */
int VideoDevicePool::fillDeviceQComboBox(QComboBox *combobox)
{
    /// @todo implement me
	qDebug() << "Called.";
// check if QComboBox is a valid pointer.
	if (combobox != NULL)
	{
		combobox->clear();
		qDebug() << "Combobox cleaned.";
		if(m_videodevice.size())
		{
			for (int loop=0; loop < m_videodevice.size(); loop++)
			{
				combobox->addItem(m_videodevice[loop].m_name);
				qDebug() << "Added device " << loop << ": " << m_videodevice[loop].m_name;
			}
			combobox->setCurrentIndex(currentDevice());
			combobox->setEnabled(true);
			return EXIT_SUCCESS;
		}
		combobox->setEnabled(false);
	}
	return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevicePool::fillInputQComboBox(QComboBox *combobox)
 */
int VideoDevicePool::fillInputQComboBox(QComboBox *combobox)
{
    /// @todo implement me
	qDebug() << "Called.";
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
					qDebug() << "Added input " << loop << ": " << m_videodevice[currentDevice()].m_input[loop].name << " (tuner: " << m_videodevice[currentDevice()].m_input[loop].hastuner << ")";
				}
				combobox->setCurrentIndex(currentInput());
				combobox->setEnabled(true);
				return EXIT_SUCCESS;
			}
		}
		combobox->setEnabled(false);
	}
	return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevicePool::fillStandardQComboBox(QComboBox *combobox)
 */
int VideoDevicePool::fillStandardQComboBox(QComboBox *combobox)
{
    /// @todo implement me
	qDebug() << "Called.";
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
				qDebug() << "StandardQComboBox: Added input " << loop << ": " << m_videodevice[currentDevice()].m_input[loop].name << " (tuner: " << m_videodevice[currentDevice()].m_input[loop].hastuner << ")";*/
				}
				combobox->setCurrentIndex(0);	// FIXME: set to actual signal standard
				combobox->setEnabled(combobox->count());
				return EXIT_SUCCESS;
			}
		}
		combobox->setEnabled(false);
	}
	return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevicePool::scanDevices()
 */
int VideoDevicePool::scanDevices()
{
    /// @todo implement me

	if (m_videodevice.isEmpty()) {
		qDebug() << "called";
#ifndef COMPILE_DISABLE_AVDEVICE
		foreach (Solid::Device device,
				Solid::Device::listFromType(Solid::DeviceInterface::Video, QString())) {
			registerDevice( device );
		}

#endif
		qDebug() << "exited successfuly";
	} else {
		qDebug() << "Not scanning: initial device list already loaded";
	}
	return EXIT_SUCCESS;
}

void VideoDevicePool::registerDevice( Solid::Device & device )
{
	qDebug() << "New video device at " << device.udi();
	const Solid::Device * vendorDevice = &device;
	while ( vendorDevice->isValid() && vendorDevice->vendor().isEmpty() )
	{
		vendorDevice = new Solid::Device( vendorDevice->parentUdi() );
	}
	if ( vendorDevice->isValid() )
	{
		qDebug() << "vendor: " << vendorDevice->vendor() << ", product: " << vendorDevice->product();
	}
	Solid::Video * solidVideoDevice = device.as<Solid::Video>();
	if ( solidVideoDevice ) {
		QStringList protocols = solidVideoDevice->supportedProtocols();
		if ( protocols.contains( "video4linux" ) )
		{
			QStringList drivers = solidVideoDevice->supportedDrivers( "video4linux" );
			if ( drivers.contains( "video4linux" ) )
			{
				qDebug() << "V4L device path is" << solidVideoDevice->driverHandle( "video4linux" ).toString();
				VideoDevice videodevice;
				videodevice.setUdi( device.udi() );
				videodevice.setFileName(solidVideoDevice->driverHandle( "video4linux" ).toString());
				qDebug() << "Found device " << videodevice.fileName();
				videodevice.open();
				if(videodevice.isOpen())
				{
					qDebug() << "File " << videodevice.fileName() << " was opened successfuly";
					videodevice.close();
					m_videodevice.push_back(videodevice);
				}
			}
		}
	}
}

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
int VideoDevicePool::size()
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
    \fn Kopete::AV::VideoDevicePool::currentDeviceUdi()
 */
QString VideoDevicePool::currentDeviceUdi()
{
	if (m_videodevice.size() && (m_current_device >= 0))
		return m_videodevice[m_current_device].udi();
	else
		return QString();
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
int VideoDevicePool::inputs()
{
    /// @todo implement me
	return m_videodevice[currentDevice()].inputs();
}

/*!
    \fn void Kopete::AV::VideoDevicePool::loadSelectedDevice()
    \brief Loads and selects the saved device
 */
void VideoDevicePool::loadSelectedDevice()
{
	qDebug() << "called";
	if (hasDevices())
	{
		KConfigGroup config(KGlobal::config(), "Video Device Settings");
		QString currentdevice = config.readEntry("Current Device", QString());
		qDebug() << "Device name:" << config.readEntry( QString::fromLocal8Bit( "Device %1 Name" ).arg( currentdevice ), QString("NOT SAVED") );
		if (!currentdevice.isEmpty())
		{
			qDebug() << "Saved device:" << currentdevice;
			VideoDeviceVector::iterator vditerator;
			for( vditerator = m_videodevice.begin(); vditerator != m_videodevice.end(); ++vditerator )
			{
				if ((*vditerator).udi() == currentdevice)
				{
					m_current_device = std::distance (m_videodevice.begin(), vditerator);
					qDebug() << "Saved device is available, setting device-index to" << m_current_device;
					return;
				}
			}
			qDebug() << "Saved device is not available.";
		}
		else
			qDebug() << "No device saved.";
	}
}

/*!
    \fn void Kopete::AV::VideoDevicePool::loadDeviceConfig()
    \brief Loads and applies the configuration for the currently selected device
    
    Loads the input and the values for all video-controls and applies them.
 */
void VideoDevicePool::loadDeviceConfig()
{
	qDebug() << "called";
	if (hasDevices())
	{
		KConfigGroup config(KGlobal::config(), "Video Device Settings");
		// Load input and apply
		const QString key_currentinput = QString::fromLocal8Bit( "Device %1 Current Input" ).arg( m_videodevice[m_current_device].udi() );
		const int currentinput = config.readEntry(key_currentinput, 0);
		qDebug() << "Setting input to" << currentinput;
		if (currentinput != m_videodevice[m_current_device].currentInput())
			m_videodevice[m_current_device].selectInput(currentinput);
		// Load video-controls and apply
		quint32 ctrl_id;
		qint32 ctrl_value;
		QString ctrl_key;
		bool ok = false;
		const QString key_control_start = QString::fromLocal8Bit( "Device %1 Input %2 Control " ).arg( m_videodevice[m_current_device].udi() ,  m_videodevice[m_current_device].currentInput() );
		QStringList ctrl_keys = config.keyList().filter(key_control_start);
		qDebug() << "Found" << ctrl_keys.size() << "saved values for video-controls";
		foreach (ctrl_key, ctrl_keys)
		{
			ctrl_id = QString(ctrl_key).remove(key_control_start).toUInt(&ok);
			if (ok)
			{
				/* NOTE: we do not read the value as int with readEntry() directly
				  because it doesn't tell us if the saved value was valid.
				  If not, we do NOT apply a standard value.
				*/
				QString tmpstr = config.readEntry(ctrl_key, QString());
				ctrl_value = tmpstr.toInt(&ok);
				if (ok && !tmpstr.isEmpty())
				{
					qDebug() << "Setting control" << ctrl_id << "to value" << ctrl_value;
					m_videodevice[m_current_device].setControlValue(ctrl_id, ctrl_value);
				}
				else
					qDebug() << "Saved value for control" << ctrl_id << "is invalid:" << tmpstr;
			}
			else
				qDebug() << "Invalid key:" << ctrl_key;
		}
	}
	/* TODO: load and apply signal standard */
}

/*!
    \fn void Kopete::AV::VideoDevicePool::saveCurrentDeviceConfig()
    \brief Saves the current device configuration
    
    Saves the current device, the current input and the current values for all supported video-controls.
 */
void VideoDevicePool::saveCurrentDeviceConfig()
{
	qDebug() << "called";
	if (hasDevices())
	{
		KConfigGroup config(KGlobal::config(), "Video Device Settings");
		// Save current device:
		qDebug() << "Current device:" << m_videodevice[m_current_device].udi();
		config.writeEntry( "Current Device", m_videodevice[m_current_device].udi() );
		// Save current device name (for debugging only):
		qDebug() << "Current device name:" << m_videodevice[m_current_device].m_name;
		const QString name = QString::fromLocal8Bit( "Device %1 Name" ).arg( m_videodevice[m_current_device].udi() );
		config.writeEntry( name, m_videodevice[m_current_device].m_name );
		// Open device if closed:
		bool wasClosed = false;
		if (!m_videodevice[m_current_device].isOpen())
		{
			qDebug() << "Device is currently closed, will be opened.";
			wasClosed = true;
			if (EXIT_SUCCESS != m_videodevice[m_current_device].open())
			{
				qDebug() << "Failed to open the device. Saving aborted.";
				config.sync();
				return;
			}
		}
		// Save current input:
		qDebug() << "Current input:" << m_videodevice[m_current_device].currentInput();
		const QString key_currentinput = QString::fromLocal8Bit( "Device %1 Current Input" ).arg( m_videodevice[m_current_device].udi() );
		config.writeEntry( key_currentinput, m_videodevice[m_current_device].currentInput() );
		// --- Save values of the controls ---:
		qint32 ctrl_value;
		const QString key_control_start = QString::fromLocal8Bit( "Device %1 Input %2 Control " ).arg( m_videodevice[m_current_device].udi() ,  m_videodevice[m_current_device].currentInput() );
		// Save values of the numeric controls:
		QList<NumericVideoControl> numCtrls = m_videodevice[m_current_device].getSupportedNumericControls();
		NumericVideoControl numCtrl;
		foreach (numCtrl, numCtrls)
		{
			if (EXIT_SUCCESS == m_videodevice[m_current_device].getControlValue(numCtrl.id, &ctrl_value))
			{
				qDebug() << "Numeric control:" << numCtrl.id << "value" << ctrl_value;
				config.writeEntry( key_control_start + QString::number(numCtrl.id), ctrl_value );
			}
			else
				qDebug() << "Error: couldn't get current value for numeric control" << numCtrl.id;
		}
		// Save values of the boolean controls:
		QList<BooleanVideoControl> boolCtrls = m_videodevice[m_current_device].getSupportedBooleanControls();
		BooleanVideoControl boolCtrl;
		foreach (boolCtrl, boolCtrls)
		{
			if (EXIT_SUCCESS == m_videodevice[m_current_device].getControlValue(boolCtrl.id, &ctrl_value))
			{
				qDebug() << "Boolean control:" << boolCtrl.id << "value" << ctrl_value;
				config.writeEntry( key_control_start + QString::number(boolCtrl.id), ctrl_value );
			}
			else
				qDebug() << "Error: couldn't get current value for boolean control" << numCtrl.id;
		}
		// Save values of the menu controls:
		QList<MenuVideoControl> menuCtrls = m_videodevice[m_current_device].getSupportedMenuControls();
		MenuVideoControl menuCtrl;
		foreach (menuCtrl, menuCtrls)
		{
			if (EXIT_SUCCESS == m_videodevice[m_current_device].getControlValue(menuCtrl.id, &ctrl_value))
			{
				qDebug() << "Menu-control:" << menuCtrl.id << "value" << ctrl_value;
				config.writeEntry( key_control_start + QString::number(menuCtrl.id), ctrl_value );
			}
			else
				qDebug() << "Error: couldn't get current value for menu-control" << numCtrl.id;
		}
		// NOTE: Action-video-controls don't have values, so there is nothing to save.
		// Close device again (if it was closed before):
		if (wasClosed)
		{
			if (EXIT_SUCCESS == m_videodevice[m_current_device].close())
				qDebug() << "Device successfully closed.";
			else
				qDebug() << "Error: failed to close the device.";
		}
		config.sync();
	}
	/* TODO: save signal standard */
}

void VideoDevicePool::deviceAdded( const QString & udi )
{
	qDebug() << "("<< udi << ") called";
	Solid::Device dev( udi );
	if ( dev.is<Solid::Video>() )
	{
		registerDevice( dev );
		emit deviceRegistered( udi );
	}
}

void VideoDevicePool::deviceRemoved( const QString & udi )
{
	qDebug() << "("<< udi << ") called";
	int i = 0;
	m_ready.lock();
	foreach ( VideoDevice vd, m_videodevice )
	{
		if ( vd.udi() == udi )
		{
			qDebug() << "Video device '" << udi << "' has been removed!";
			// not sure if this is safe but at this point the device node is gone already anyway
			m_videodevice.remove( i );
			if (m_current_device == i)
			{
				m_current_device = 0;
				m_clients = 0;
			}
			else if (m_current_device > i)
			{
				m_current_device--;
			}
			m_ready.unlock();
			emit deviceUnregistered( udi );
			/* NOTE: do not emit deviceUnregistered( udi ) with mutex locked ! => potential deadlock ! */
			return;
		}
		else
		{
			i++;
		}
	}
	m_ready.unlock();
}

} // namespace AV

} // namespace Kopete
