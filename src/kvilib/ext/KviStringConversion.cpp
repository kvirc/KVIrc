//=============================================================================
//
//   File : KviStringConversion.cpp
//   Creation date : Thu Oct 20 2000 14:12:21 CEST by Szymon Stefanek
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

#define _KVI_STRINGCONVERSION_CPP_
#include "KviStringConversion.h"
#include "KviCString.h"
#include "KviMessageTypeSettings.h"
#include "KviPixmap.h"
#include "KviQString.h"

#include <QColor>
#include <QFont>
#include <QRect>
#include <QString>
#include <QStringList>
#include <cstdio>

QString g_szGlobalDir;
QString g_szLocalDir;

namespace KviStringConversion
{
	void init(const QString & szGlobalDir, const QString & szLocalDir)
	{
		g_szGlobalDir = szGlobalDir;
		g_szLocalDir = szLocalDir;
	}

	void encodePath(QString & szBuffer)
	{
		if(!szBuffer.isEmpty())
		{
			if(!g_szLocalDir.isEmpty())
			{
				if(szBuffer.indexOf(g_szLocalDir, 0, Qt::CaseSensitive) == 0)
				{
					szBuffer.remove(0, g_szLocalDir.length());
					szBuffer.prepend("local://");
				}
			}
			if(!g_szGlobalDir.isEmpty())
			{
				if(szBuffer.indexOf(g_szGlobalDir, 0, Qt::CaseSensitive) == 0)
				{
					szBuffer.remove(0, g_szGlobalDir.length());
					szBuffer.prepend("global://");
				}
			}
		}
	}

	void decodePath(QString & szBuffer)
	{
		if(!szBuffer.isEmpty())
		{
			if(!g_szLocalDir.isEmpty())
			{
				if(szBuffer.indexOf("local://", 0, Qt::CaseSensitive) == 0)
				{
					szBuffer.remove(0, 8);
					szBuffer.prepend(g_szLocalDir);
				}
			}
			if(!g_szGlobalDir.isEmpty())
			{
				if(szBuffer.indexOf("global://", 0, Qt::CaseSensitive) == 0)
				{
					szBuffer.remove(0, 9);
					szBuffer.prepend(g_szGlobalDir);
				}
			}
		}
	}

	void encodePath(QStringList & buffer)
	{
		for(auto & it : buffer)
			encodePath(it);
	}

	void decodePath(QStringList & buffer)
	{
		for(auto & it : buffer)
			decodePath(it);
	}

	void toString(const bool bValue, QString & szBuffer)
	{
		szBuffer = bValue ? '1' : '0';
	}

	bool fromString(const QString & szValue, bool & bBuffer)
	{
		if(szValue.isEmpty())
			bBuffer = false;
		else
			bBuffer = !((KviQString::equalCS(szValue, "0")) || (KviQString::equalCI(szValue, "false")));
		return true;
	}

	void toString(const int iValue, QString & szBuffer)
	{
		szBuffer.setNum(iValue);
	}

	bool fromString(const QString & szValue, int & iBuffer)
	{
		bool bOk;
		iBuffer = szValue.toInt(&bOk);
		return bOk;
	}

	void toString(const unsigned int uValue, QString & szBuffer)
	{
		szBuffer.setNum(uValue);
	}

	bool fromString(const QString & szValue, unsigned int & uBuffer)
	{
		bool bOk;
		uBuffer = szValue.toUInt(&bOk);
		return bOk;
	}

	void toString(const QRect & rValue, QString & szBuffer)
	{
		szBuffer.sprintf("%d,%d,%d,%d", rValue.x(), rValue.y(), rValue.width(), rValue.height());
	}

	bool fromString(const QString & szValue, QRect & buffer)
	{
		QByteArray tmp = szValue.toUtf8();
		const char * c = tmp.data();
		if(!c)
			return false;
		int l, t, w, h;
		if(sscanf(c, "%d,%d,%d,%d", &l, &t, &w, &h) != 4)
			return false;
		buffer.setRect(l, t, w, h);
		return true;
	}

	void toString(const QString & szValue, QString & szBuffer)
	{
		szBuffer = szValue;
	}

	bool fromString(const QString & szValue, QString & szBuffer)
	{
		szBuffer = szValue;
		return true;
	}

	void toString(const KviPixmap & pixmap, QString & szBuffer)
	{
		szBuffer = pixmap.path();
		encodePath(szBuffer);
	}

	bool fromString(const QString & szValue, KviPixmap & buffer)
	{
		QString szPath(szValue);
		decodePath(szPath);
		if(szPath.isEmpty())
		{
			buffer.setNull();
			return true;
		}
		else
		{
			return buffer.load(szPath);
		}
	}

	void toString(const KviMessageTypeSettings & mValue, QString & szBuffer)
	{
		szBuffer.sprintf("%d,%u,%u,%d,%d", mValue.m_iPixId, mValue.m_cForeColor, mValue.m_cBackColor, mValue.m_bLogEnabled, mValue.m_iLevel);
	}

	bool fromString(const QString & szValue, KviMessageTypeSettings & buffer)
	{
		int iId, iLog, iLevel;
		unsigned int uFore, uBack;
		QByteArray tmp = szValue.toUtf8();
		char * cx = tmp.data();
		if(!cx)
			return false;
		if(sscanf(cx, "%d,%u,%u,%d,%d", &iId, &uFore, &uBack, &iLog, &iLevel) != 5)
			return false;
		buffer = KviMessageTypeSettings(buffer.m_szType, iId, uFore, uBack, iLog, iLevel);
		return true;
	}

	void toString(const QColor & value, QString & szBuffer)
	{
		szBuffer = value.name();
	}

	bool fromString(const QString & szValue, QColor & buffer)
	{
		buffer.setNamedColor(szValue);
		return true;
	}

	void toString(const QFont & font, QString & szBuffer)
	{
		QString szFamily(font.family());
		szBuffer.sprintf("%s,%d,%d,%d", szFamily.toUtf8().data(), font.pointSize(), font.styleHint(), font.weight());
		QString szOptions;
		if(font.bold())
			szOptions.append('b');
		if(font.italic())
			szOptions.append('i');
		if(font.underline())
			szOptions.append('u');
		if(font.strikeOut())
			szOptions.append('s');
		if(font.fixedPitch())
			szOptions.append('f');

		if(!szOptions.isEmpty())
		{
			szBuffer.append(',');
			szBuffer.append(szOptions);
		}
	}

	bool fromString(const QString & szValue, QFont & buffer)
	{
		KviCString str = szValue;
		KviCString family, pointSize, styleHint, weight, options;
		str.getToken(family, ',');
		str.getToken(pointSize, ',');
		str.getToken(styleHint, ',');
		str.getToken(weight, ',');
		if(!family.isEmpty())
			buffer.setFamily(family.ptr());
		int i;
		bool bOk;
		i = pointSize.toInt(&bOk);
		if(bOk && (i > 0))
			buffer.setPointSize(i);
		i = styleHint.toInt(&bOk);
		if(bOk && (i >= 0))
			buffer.setStyleHint((QFont::StyleHint)i);
		i = weight.toInt(&bOk);
		if(bOk && (i >= 0))
			buffer.setWeight(i);

		buffer.setBold(str.contains("b"));
		buffer.setItalic(str.contains("i"));
		buffer.setUnderline(str.contains("u"));
		buffer.setStrikeOut(str.contains("s"));
		buffer.setFixedPitch(str.contains("f"));
		return true;
	}

	void toString(const QStringList & list, QString & szBuffer)
	{
		szBuffer = list.join(",");
	}

	bool fromString(const QString & szValue, QStringList & buffer)
	{
		buffer = szValue.split(",");
		return true;
	}
}
