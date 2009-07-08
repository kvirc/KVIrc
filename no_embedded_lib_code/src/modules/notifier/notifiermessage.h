#ifndef _NOTIFIERMESSAGE_H_
#define _NOTIFIERMESSAGE_H_
//=============================================================================
//
//   File : notifiermessage.h
//   Creation date : Mar 02 Nov 2004 02:41:18 by Iacopo Palazzi
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2004-2008 Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
//   Copyright (C) 2009 Fabio Bas < ctrlaltca at gmail dot com >
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

#include <QPixmap>
#include <QString>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>

class KviNotifierMessage : public QWidget
{
	friend class KviNotifierWindow;
public:
	KviNotifierMessage(QPixmap * pPixmap, const QString &szText);
	~KviNotifierMessage();
private:
	QString  	  m_szText;
	QPixmap		* m_pPixmap;
	QHBoxLayout	* m_pHBox;
	QLabel		* m_pLabel0;
	QLabel		* m_pLabel1;
public:
	static QString convertToHtml(const QString &text);
	inline QString text() const { return m_szText; };
	inline QPixmap* pixmap() const { return m_pPixmap; };

};

#endif //!_NOTIFIERMESSAGE_H_
