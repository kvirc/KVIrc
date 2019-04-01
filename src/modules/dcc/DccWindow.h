#ifndef _WINDOW_H_
#define _WINDOW_H_
//=============================================================================
//
//   File : DccWindow.h
//   Creation date : Fri Jul 26 02:04:39 2002 GMT by Szymon Stefanek
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

#include "DccDescriptor.h"
#include "DccMarshal.h"

#include "KviWindow.h"

class DccThread;

class DccWindow : public KviWindow, public DccMarshalOutputContext
{
	Q_OBJECT
public:
	DccWindow(KviWindow::Type eType, const char * name, DccDescriptor * d);
	~DccWindow();

protected:
	DccDescriptor * m_pDescriptor;
	DccMarshal * m_pMarshal;

public:
	DccDescriptor * descriptor() { return m_pDescriptor; };
	const DccMarshal * marshal() { return m_pMarshal; };
	virtual DccThread * getSlaveThread() { return nullptr; }
	virtual KviWindow * dccMarshalOutputWindow();
	virtual const char * dccMarshalOutputContextString();
};

#endif //_WINDOW_H_
