//=============================================================================
//
//   File : videoinput.cpp
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
    videoinput.cpp  -  Kopete Video Input Class

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

#include "videoinput.h"

namespace Kopete {

namespace AV {

VideoInput::VideoInput()
{
	kDebug() << "Executing Video Input's constructor!!!";
	m_brightness = 0.5;
	m_contrast = 0.5;
	m_saturation = 0.5;
	m_hue = 0.5;
	m_autobrightnesscontrast = false;
	m_autocolorcorrection = false;
}


VideoInput::~VideoInput()
{
}

float VideoInput::getBrightness()
{
//	kDebug() << " called.";
	return m_brightness;
}

float VideoInput::setBrightness(float brightness)
{
//	kDebug() << " called.";
	if ( brightness > 1 )
		brightness = 1;
	else
	if ( brightness < 0 )
		brightness = 0;
	m_brightness = brightness;
	return getBrightness();
}

float VideoInput::getContrast()
{
//	kDebug() << " called.";
	return m_contrast;
}

float VideoInput::setContrast(float contrast)
{
//	kDebug() << " called.";
	if ( contrast > 1 )
		contrast = 1;
	else
	if ( contrast < 0 )
		contrast = 0;
	m_contrast = contrast;
	return getContrast();
}

float VideoInput::getSaturation()
{
//	kDebug() << " called.";
	return m_saturation;
}

float VideoInput::setSaturation(float saturation)
{
//	kDebug() << " called.";
	if ( saturation > 1 )
		saturation = 1;
	else
	if ( saturation < 0 )
		saturation = 0;
	m_saturation = saturation;
	return getSaturation();
}

float VideoInput::getWhiteness()
{
//	kDebug() << " called.";
	return m_whiteness;
}

float VideoInput::setWhiteness(float whiteness)
{
//	kDebug() << " called.";
	if ( whiteness > 1 )
		whiteness = 1;
	else
	if ( whiteness < 0 )
		whiteness = 0;
	m_whiteness = whiteness;
	return getWhiteness();
}

float VideoInput::getHue()
{
//	kDebug() << " called.";
	return m_hue;
}

float VideoInput::setHue(float hue)
{
//	kDebug() << " called.";
	if ( hue > 1 )
		hue = 1;
	else
	if ( hue < 0 )
		hue = 0;
	m_hue = hue;
	return getHue();
}


bool VideoInput::getAutoBrightnessContrast()
{
//	kDebug() << " called.";
	return m_autobrightnesscontrast;
}

bool VideoInput::setAutoBrightnessContrast(bool brightnesscontrast)
{
//	kDebug() << " called.";
	m_autobrightnesscontrast = brightnesscontrast;
	return getAutoBrightnessContrast();
}

bool VideoInput::getAutoColorCorrection()
{
//	kDebug() << " called.";
	return m_autocolorcorrection;
}

bool VideoInput::setAutoColorCorrection(bool colorcorrection)
{
//	kDebug() << " called.";
	m_autocolorcorrection = colorcorrection;
	return getAutoColorCorrection();
}

bool VideoInput::getImageAsMirror()
{
//	kDebug() << " called.";
	return m_imageasmirror;
}

bool VideoInput::setImageAsMirror(bool imageasmirror)
{
//	kDebug() << " called.";
	m_imageasmirror = imageasmirror;
	return getImageAsMirror();
}

}

}
