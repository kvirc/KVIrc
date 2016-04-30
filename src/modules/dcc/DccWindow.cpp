//=============================================================================
//
//   File : DccWindow.cpp
//   Creation date : Fri Jul 26 02:04:40 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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

#include "DccWindow.h"

#include "KviTalHBox.h"

DccWindow::DccWindow(KviWindow::Type eType, const char * name, DccDescriptor * d)
    : KviWindow(eType, name, d->console())
{
	m_pDescriptor = d;
	m_pDescriptor->setWindow(this);
	m_pMarshal = nullptr;
	m_pButtonBox = new KviTalHBox(this);
	createTextEncodingButton(m_pButtonBox);
}

DccWindow::~DccWindow()
{
	if(m_pMarshal)
		delete m_pMarshal;
	if(m_pDescriptor)
		delete m_pDescriptor;
}

KviWindow * DccWindow::dccMarshalOutputWindow()
{
	return this;
}

const char * DccWindow::dccMarshalOutputContextString()
{
	static const char * static_context = "DCC";
	return static_context;
}
