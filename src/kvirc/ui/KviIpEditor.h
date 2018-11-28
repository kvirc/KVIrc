#ifndef _KVI_IPEDITOR_H_
#define _KVI_IPEDITOR_H_
//=============================================================================
//
//   File : KviIpEditor.h
//   Creation date : Wed Jun 12 2000 14:16:02 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include <QLineEdit>

class KVIRC_API KviIpEditor : public QLineEdit
{
	Q_OBJECT
public:
	enum AddressType
	{
		IPv4,
		IPv6
	};
	KviIpEditor(QWidget * parent, AddressType = IPv4, const QString & ipAddr = QString(), const char * name = nullptr);
	~KviIpEditor() = default;

private:
	AddressType m_addrType;

public:
	bool setAddress(const QString & ipAddr);
	QString address() const;
	void setAddressType(AddressType addrType);
	AddressType addressType() const { return m_addrType; }
	bool isValid() const;
};

#endif //_KVI_IPEDITOR_H_
