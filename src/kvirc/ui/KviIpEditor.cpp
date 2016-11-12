//=============================================================================
//
//   File : KviIpEditor.cpp
//   Creation date : Wed Jun 12 2000 14:16:49 by Szymon Stefanek
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

#include "KviIpEditor.h"

#include <QHostAddress>
#include <QRegExpValidator>
#include <QString>

// FIXME: #warning "THIS COULD GO INTO libkvioptions ?"

KviIpEditor::KviIpEditor(QWidget * parent, AddressType addrType, const QString & ipAddr, const char * name)
    : QLineEdit(parent)
{
	setObjectName(name);
	setAddressType(addrType);
	setAddress(ipAddr);
}

void KviIpEditor::setAddressType(AddressType addrType)
{
	if(addrType == IPv4)
	{
		m_addrType = addrType;
		QString tmp = text();
		QRegExp rx("(?:(?:25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9])\\.){3}(?:25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9])");
		setValidator(new QRegExpValidator(rx, this));
		setText(QHostAddress{QHostAddress::AnyIPv4}.toString());
		if (!tmp.isEmpty())
			setAddress(tmp);
	}
	else if (addrType == IPv6)
	{
		m_addrType = addrType;
		QString tmp = text();
		QRegExp rx("[0-9a-fA-F:]*|::[fF]{4}:(?:(?:25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9])\\.){3}(?:25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9])");
		setValidator(new QRegExpValidator(rx, this));
		setText(QHostAddress{QHostAddress::AnyIPv6}.toString());
		if (!tmp.isEmpty())
			setAddress(tmp);
	}
}

bool KviIpEditor::isValid() const
{
	return !QHostAddress{text()}.isNull();
}

bool KviIpEditor::setAddress(const QString & ipAddr)
{
	QHostAddress addr{ipAddr};
	if(addr.isNull())
		return false;

	if(m_addrType == IPv4)
		setText(QHostAddress{addr.toIPv4Address()}.toString());
	else
		setText(QHostAddress{addr.toIPv6Address()}.toString());

	return true;
}

QString KviIpEditor::address() const
{
	QHostAddress addr{text()};
	if(addr.isNull())
		return "";
	if(m_addrType == IPv6)
		return QHostAddress{addr.toIPv6Address()}.toString();
	else
		return QHostAddress{addr.toIPv4Address()}.toString();
}
