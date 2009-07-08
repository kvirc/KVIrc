#ifndef _WINDOW_H_
#define _WINDOW_H_
//=============================================================================
//
//   File : window.h
//   Creation date : Fri Jul 26 02:04:39 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "descriptor.h"
#include "marshal.h"

#include "kvi_window.h"

class KviDccWindow : public KviWindow , public KviDccMarshalOutputContext
{
	Q_OBJECT
public:
	KviDccWindow(int type,KviFrame * lpFrm,const char * name,KviDccDescriptor * d);
	~KviDccWindow();
protected:
	KviDccDescriptor       * m_pDescriptor;
	KviDccMarshal          * m_pMarshal;
public:
	KviDccDescriptor * descriptor(){ return m_pDescriptor; };
	const KviDccMarshal    * marshal(){ return m_pMarshal; };

	virtual KviWindow * dccMarshalOutputWindow();
	virtual const char * dccMarshalOutputContextString();
};

#endif //_WINDOW_H_
