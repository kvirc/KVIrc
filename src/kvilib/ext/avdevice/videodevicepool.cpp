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

#include <QDir>
#include <QDebug>

#include "videodevice.h"
#include "videodevicepool.h"

#ifdef COMPILE_KDE_SUPPORT
	#include <klocale.h>
	#include <kglobal.h>
	#include <kconfig.h>
	#include <kconfiggroup.h>
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
: m_current_device(-1)
{
#ifdef COMPILE_KDE_SUPPORT
	connect( Solid::DeviceNotifier::instance(), SIGNAL(deviceAdded(const QString&)), SLOT(deviceAdded(const QString &)) );
	connect( Solid::DeviceNotifier::instance(), SIGNAL(deviceRemoved(const QString&)), SLOT(deviceRemoved(const QString &)) );
	/* NOTE: No locking needed as long as we don't connect with Qt::ConnectionType = Qt::DirectConnection
	         while the signals are emitted by other threads
	 */
	foreach( Solid::Device device, Solid::Device::listFromType(Solid::DeviceInterface::Video, QString()) )
		registerDevice( device );
#endif
}


VideoDevicePool::~VideoDevicePool()
{
	foreach ( VideoDevice* vd, m_videodevices )
		delete vd;
}




/*!
    \fn VideoDevicePool::open(int device)
 */
int VideoDevicePool::open(int device)
{
	qDebug() << "called with device" << device;
	if (!m_videodevices.size() || (device >= m_videodevices.size()))
	{
		qDebug() << "Device not found.";
		return EXIT_FAILURE;
	}
	if (device < 0)
	{
		qDebug() << "Trying to load saved device.";
		device = getSavedDevice();
		if (device < 0)
		{
			if (m_current_device < 0)
				device = 0;
			else
				device = m_current_device;
			qDebug() << "Saved device is not available, using default device:" << device;
		}
	}
	int isopen = EXIT_FAILURE;
	if ((device != m_current_device) || !isOpen())
	{
		if (isOpen())
		{
			if (EXIT_SUCCESS == m_videodevices[m_current_device]->close())
				m_clients--;
			else
				return EXIT_FAILURE;
		}
		isopen = m_videodevices[device]->open();
		if (isopen == EXIT_SUCCESS)
		{
			m_current_device = device;
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
	return isopen;
}

bool VideoDevicePool::isOpen()
{
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->isOpen();
	else
		return false;
}

/*!
    \fn VideoDevicePool::showDeviceCapabilities(int device)
 */
int VideoDevicePool::showDeviceCapabilities(int device)
{
	if (device < 0)
		device = m_current_device;
	if ((device >= 0) && (device < m_videodevices.size()))
		return m_videodevices[device]->showDeviceCapabilities();
	else
		return EXIT_FAILURE;
}

int VideoDevicePool::width()
{
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->width();
	else
		return 0;
}

int VideoDevicePool::minWidth()
{
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->minWidth();
	else
		return 0;
}

int VideoDevicePool::maxWidth()
{
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->maxWidth();
	else
		return 0;
}

int VideoDevicePool::height()
{
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->height();
	else
		return 0;
}

int VideoDevicePool::minHeight()
{
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->minHeight();
	else
		return 0;
}

int VideoDevicePool::maxHeight()
{
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->maxHeight();
	else
		return 0;
}

int VideoDevicePool::setSize( int newwidth, int newheight)
{
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->setSize(newwidth, newheight);
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
	if ((m_current_device < 0) || (m_current_device >= m_videodevices.size()))
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
		ret = m_videodevices[m_current_device]->close();
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
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->startCapturing();
	else
		return EXIT_FAILURE;
}


/*!
    \fn VideoDevicePool::stopCapturing()
 */
int VideoDevicePool::stopCapturing()
{
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->stopCapturing();
	else
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
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->getSupportedNumericControls();
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
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->getSupportedBooleanControls();
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
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->getSupportedMenuControls();
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
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->getSupportedActionControls();
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
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->getControlValue(ctrl_id, value);
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
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->setControlValue(ctrl_id, value);
	else
		return EXIT_FAILURE;
}


/*!
    \fn VideoDevicePool::getFrame()
 */
int VideoDevicePool::getFrame()
{
//	qDebug() << "VideoDevicePool::getFrame() called.";
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->getFrame();
	else
		return EXIT_FAILURE;
}

/*!
    \fn VideoDevicePool::getQImage(QImage *qimage)
 */
int VideoDevicePool::getImage(QImage *qimage)
{
//	qDebug() << "VideoDevicePool::getImage() called.";
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->getImage(qimage);
	else
		return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevicePool::selectInput(int input)
 */
int VideoDevicePool::selectInput(int newinput)
{
	qDebug() << "VideoDevicePool::selectInput(" << newinput << ") called.";
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->selectInput(newinput);
	else
		return EXIT_FAILURE;
}

/*!
    \fn Kopete::AV::VideoDevicePool::fillDeviceQComboBox(QComboBox *combobox)
 */
void VideoDevicePool::fillDeviceQComboBox(QComboBox *combobox)
{
	qDebug() << "Called.";
	if (combobox == NULL)
		return;
	combobox->clear();
	if (m_videodevices.size())
	{
		for (int loop = 0; loop < m_videodevices.size(); loop++)
		{
			combobox->addItem(m_videodevices[loop]->m_name);
			qDebug() << "Added device" << loop << ":  " << m_videodevices[loop]->m_name;
		}
		combobox->setCurrentIndex(m_current_device);
	}
	combobox->setEnabled(m_videodevices.size());
}

/*!
    \fn Kopete::AV::VideoDevicePool::fillInputQComboBox(QComboBox *combobox)
 */
void VideoDevicePool::fillInputQComboBox(QComboBox *combobox)
{
	qDebug() << "Called.";
	if (combobox == NULL)
		return;
	combobox->clear();
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
	{
		if (m_videodevices[m_current_device]->inputs() > 0)
		{
			for (int loop = 0; loop < m_videodevices[m_current_device]->inputs(); loop++)
			{
				combobox->addItem(m_videodevices[m_current_device]->m_input[loop].name);
				qDebug() << "Added input" << loop << ":  " << m_videodevices[m_current_device]->m_input[loop].name
				         << " (tuner: " << m_videodevices[m_current_device]->m_input[loop].hastuner << ")";
			}
			combobox->setCurrentIndex(m_videodevices[m_current_device]->currentInput());
		}
	}
	combobox->setEnabled(combobox->count());
}

/*!
    \fn Kopete::AV::VideoDevicePool::fillStandardQComboBox(QComboBox *combobox)
 */
void VideoDevicePool::fillStandardQComboBox(QComboBox *combobox)
{
	qDebug() << "Called.";
	if (combobox == NULL)
		return;
	combobox->clear();
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
	{
		if (m_videodevices[m_current_device]->inputs() > 0)
		{
			for (unsigned int loop = 0; loop < 25; loop++)
			{
				if (m_videodevices[m_current_device]->m_input[currentInput()].m_standards & (1 << loop))
				{
					combobox->addItem(m_videodevices[m_current_device]->signalStandardName(1 << loop));
					qDebug() << "Added signal standard" << loop << ":  " << m_videodevices[m_current_device]->signalStandardName(1 << loop);
				}
				  
			}
			combobox->setCurrentIndex(0);	// FIXME: set to actual signal standard
		}  
	}
	combobox->setEnabled(combobox->count());
}
#ifdef COMPILE_KDE_SUPPORT
bool VideoDevicePool::registerDevice( Solid::Device & device )
{
	qDebug() << "called, UDI is:\n   " << device.udi();
	const Solid::Device * vendorDevice = &device;
	while (vendorDevice->isValid() && vendorDevice->vendor().isEmpty())
		vendorDevice = new Solid::Device(vendorDevice->parentUdi());
	/* NOTE: The device we register has usually an empty vendor string and a less meaningfull product string.
	   So we go up to the first parent device that has a non-empty vendor string,
	   because we find the expected strings describing the device there.
	 */
	if (vendorDevice->isValid())
		qDebug() << "vendor:" << vendorDevice->vendor() << ", product:" << vendorDevice->product();
	else
		qDebug() << "vendor:" << device.vendor() << ", product:" << device.product();
 
	if (device.isValid())
	{
		Solid::Video * solidVideoDevice = device.as<Solid::Video>();
		if (solidVideoDevice)
		{
			QStringList protocols = solidVideoDevice->supportedProtocols();
			if (protocols.contains("video4linux"))
			{
				QStringList drivers = solidVideoDevice->supportedDrivers("video4linux");
				if (drivers.contains("video4linux"))
				{
					VideoDevice* videodevice = new VideoDevice;
					videodevice->setUdi( device.udi() );
					videodevice->setFileName(solidVideoDevice->driverHandle("video4linux").toString());
					qDebug() << "V4L device path is" << solidVideoDevice->driverHandle("video4linux").toString();
					if (EXIT_SUCCESS == videodevice->open())
					{
						bool cap = videodevice->canCapture();
						videodevice->close();
						if (cap)
						{
							if (m_videodevices.size() == 0)
								m_current_device = 0;
							m_videodevices.push_back(videodevice);
							qDebug() << "Device is a valid video device, adding it to video device pool.";
							return true;
						}
						else
							qDebug() << "Device does not support capturing.";
					}
					else
						qDebug() << "Device could not be opened.";
					delete videodevice;
				}
			}
		}
		else
			qDebug() << "Device is not a video device.";
	}
	else
		qDebug() << "Not a valid Solid device: device is not available in the system.";
	return false;
}
#endif
/*!
    \fn Kopete::AV::VideoDevicePool::size()
 */
int VideoDevicePool::size()
{
    /// @todo implement me
	return m_videodevices.size();
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
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->udi();
	else
		return QString();
}

/*!
    \fn Kopete::AV::VideoDevicePool::currentInput()
 */
int VideoDevicePool::currentInput()
{
    /// @todo implement me
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->currentInput();
	else
		return -1;
}

/*!
    \fn Kopete::AV::VideoDevicePool::currentInput()
 */
int VideoDevicePool::inputs()
{
    /// @todo implement me
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
		return m_videodevices[m_current_device]->inputs();
	else
		return 0;
}

/*!
    \fn int Kopete::AV::VideoDevicePool::getSavedDevice()
    \brief Returns the index of the saved device
 */
int VideoDevicePool::getSavedDevice()
{
#ifdef COMPILE_KDE_SUPPORT
	qDebug() << "called";
	if (m_videodevices.size())
	{
		KConfigGroup config(KGlobal::config(), "Video Device Settings");
		QString currentdevice = config.readEntry("Current Device", QString());
		qDebug() << "Device name:" << config.readEntry( QString::fromLocal8Bit( "Device %1 Name" ).arg( currentdevice ), QString("NOT SAVED") );
		if (!currentdevice.isEmpty())
		{
			qDebug() << "Saved device:" << currentdevice;
			QVector<VideoDevice*>::iterator vditerator;
			for( vditerator = m_videodevices.begin(); vditerator != m_videodevices.end(); ++vditerator )
			{
				if ((*vditerator)->udi() == currentdevice)
				{
					int devIndex = std::distance (m_videodevices.begin(), vditerator);
					qDebug() << "Saved device is available, device index is" << devIndex;
					return devIndex;
				}
			}
			qDebug() << "Saved device is not available.";
		}
		else
			qDebug() << "No device saved.";
	}
#endif
	return -1;
}

/*!
    \fn void Kopete::AV::VideoDevicePool::loadDeviceConfig()
    \brief Loads and applies the configuration for the currently selected device
    
    Loads the input and the values for all video-controls and applies them.
 */
void VideoDevicePool::loadDeviceConfig()
{
#ifdef COMPILE_KDE_SUPPORT
	qDebug() << "called";
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
	{
		KConfigGroup config(KGlobal::config(), "Video Device Settings");
		// Load input and apply
		const QString key_currentinput = QString::fromLocal8Bit( "Device %1 Current Input" ).arg( m_videodevices[m_current_device]->udi() );
		const int currentinput = config.readEntry(key_currentinput, 0);
		qDebug() << "Setting input to" << currentinput;
		if (currentinput != m_videodevices[m_current_device]->currentInput())
			m_videodevices[m_current_device]->selectInput(currentinput);
		// Load video-controls and apply
		quint32 ctrl_id;
		qint32 ctrl_value;
		QString ctrl_key;
		bool ok = false;
		const QString key_control_start = QString::fromLocal8Bit( "Device %1 Input %2 Control " ).arg( m_videodevices[m_current_device]->udi() ).arg( m_videodevices[m_current_device]->currentInput() );
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
					m_videodevices[m_current_device]->setControlValue(ctrl_id, ctrl_value);
				}
				else
					qDebug() << "Saved value for control" << ctrl_id << "is invalid:" << tmpstr;
			}
			else
				qDebug() << "Invalid key:" << ctrl_key;
		}
	}
	/* TODO: load and apply signal standard */
#endif
}

/*!
    \fn void Kopete::AV::VideoDevicePool::saveCurrentDeviceConfig()
    \brief Saves the current device configuration
    
    Saves the current device, the current input and the current values for all supported video-controls.
 */
void VideoDevicePool::saveCurrentDeviceConfig()
{
#ifdef COMPILE_KDE_SUPPORT
	qDebug() << "called";
	if ((m_current_device >= 0) && (m_current_device < m_videodevices.size()))
	{
		KConfigGroup config(KGlobal::config(), "Video Device Settings");
		// Save current device:
		qDebug() << "Current device:" << m_videodevices[m_current_device]->udi();
		config.writeEntry( "Current Device", m_videodevices[m_current_device]->udi() );
		// Save current device name (for debugging only):
		qDebug() << "Current device name:" << m_videodevices[m_current_device]->m_name;
		const QString name = QString::fromLocal8Bit( "Device %1 Name" ).arg( m_videodevices[m_current_device]->udi() );
		config.writeEntry( name, m_videodevices[m_current_device]->m_name );
		// Open device if closed:
		bool wasClosed = false;
		if (!m_videodevices[m_current_device]->isOpen())
		{
			qDebug() << "Device is currently closed, will be opened.";
			wasClosed = true;
			if (EXIT_SUCCESS != m_videodevices[m_current_device]->open())
			{
				qDebug() << "Failed to open the device. Saving aborted.";
				config.sync();
				return;
			}
		}
		// Save current input:
		qDebug() << "Current input:" << m_videodevices[m_current_device]->currentInput();
		const QString key_currentinput = QString::fromLocal8Bit( "Device %1 Current Input" ).arg( m_videodevices[m_current_device]->udi() );
		config.writeEntry( key_currentinput, m_videodevices[m_current_device]->currentInput() );
		// --- Save values of the controls ---:
		qint32 ctrl_value;
		const QString key_control_start = QString::fromLocal8Bit( "Device %1 Input %2 Control " ).arg( m_videodevices[m_current_device]->udi() ).arg( m_videodevices[m_current_device]->currentInput() );
		// Save values of the numeric controls:
		QList<NumericVideoControl> numCtrls = m_videodevices[m_current_device]->getSupportedNumericControls();
		NumericVideoControl numCtrl;
		foreach (numCtrl, numCtrls)
		{
			if (EXIT_SUCCESS == m_videodevices[m_current_device]->getControlValue(numCtrl.id, &ctrl_value))
			{
				qDebug() << "Numeric control:" << numCtrl.id << "value" << ctrl_value;
				config.writeEntry( key_control_start + QString::number(numCtrl.id), ctrl_value );
			}
			else
				qDebug() << "Error: couldn't get current value for numeric control" << numCtrl.id;
		}
		// Save values of the boolean controls:
		QList<BooleanVideoControl> boolCtrls = m_videodevices[m_current_device]->getSupportedBooleanControls();
		BooleanVideoControl boolCtrl;
		foreach (boolCtrl, boolCtrls)
		{
			if (EXIT_SUCCESS == m_videodevices[m_current_device]->getControlValue(boolCtrl.id, &ctrl_value))
			{
				qDebug() << "Boolean control:" << boolCtrl.id << "value" << ctrl_value;
				config.writeEntry( key_control_start + QString::number(boolCtrl.id), ctrl_value );
			}
			else
				qDebug() << "Error: couldn't get current value for boolean control" << numCtrl.id;
		}
		// Save values of the menu controls:
		QList<MenuVideoControl> menuCtrls = m_videodevices[m_current_device]->getSupportedMenuControls();
		MenuVideoControl menuCtrl;
		foreach (menuCtrl, menuCtrls)
		{
			if (EXIT_SUCCESS == m_videodevices[m_current_device]->getControlValue(menuCtrl.id, &ctrl_value))
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
			if (EXIT_SUCCESS == m_videodevices[m_current_device]->close())
				qDebug() << "Device successfully closed.";
			else
				qDebug() << "Error: failed to close the device.";
		}
		config.sync();
	}
	/* TODO: save signal standard */
#endif
}

void VideoDevicePool::deviceAdded( const QString & udi )
{
	qDebug() << "called with UDI" << udi;
#ifdef COMPILE_KDE_SUPPORT
	Solid::Device dev( udi );
	if ( dev.is<Solid::Video>() )
	{
		qDebug() << "Device is a video device, trying to register it.";
		if ( registerDevice( dev ) )
			emit deviceRegistered( udi );
	}
	else
		qDebug() << "Device is not a video device";
#endif
}
void VideoDevicePool::deviceRemoved( const QString & udi )
{
	qDebug() << "called with UDI" << udi;
	int i = 0;
	foreach ( VideoDevice* vd, m_videodevices )
	{
		if ( vd->udi() == udi )
		{
			qDebug() << "Video device with UDI" << udi << "has been removed!";
			delete m_videodevices[i]; // NOTE: device is closed in destructor
			m_videodevices.remove( i );
			if (m_current_device == i)
			{
				if (m_videodevices.size())
					m_current_device = 0;
				else
					m_current_device = -1;
				m_clients = 0;
			}
			else if (m_current_device > i)
			{
				m_current_device--;
			}
			emit deviceUnregistered( udi );
			return;
		}
		else
			i++;
	}
}

} // namespace AV

} // namespace Kopete
