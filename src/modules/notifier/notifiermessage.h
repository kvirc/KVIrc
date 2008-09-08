#ifndef _NOTIFIERMESSAGE_H_
#define _NOTIFIERMESSAGE_H_
//=============================================================================
//
//   File : notifiermessage.h
//   Created on mar 02 nov 2004 02:41:18 by
//		Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================


#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_pointerlist.h"
#include "kvi_time.h"

#include <QWidget>
#include <QRect>
#include <QTimer>
#include <QPixmap>
#include <QImage>
#include <QFont>
#include <QColor>
#include <QLineEdit>
#include <QMenu>
#include <QTextDocument>

class QPopupMenu;
class KviWindow;
class KviNotifierWindow;

class KviNotifierMessage
{
	friend class KviNotifierWindow;
public:
	KviNotifierMessage(KviNotifierWindow * pNotifierWindow, QPixmap * pImage, const QString &szText);
	~KviNotifierMessage();

// ================================
// Put members declaration below...
// ================================
private:

public:

protected:
	KviWindow       * m_pWindow;
	QPixmap         * m_pImage;
	QTextDocument   * m_pText;
	bool              m_bHistoric;

// ================================
// Put methods declaration below...
// ================================
private:

public:
	bool historic(){ return m_bHistoric; };
	void setHistoric();
	void setWindow(KviWindow * pWnd){ m_pWindow = pWnd; };
	QPixmap * image(){ return m_pImage; };
	QTextDocument * text(){ return m_pText; };

protected:


};

#define NUM_OLD_COLORS 6

#endif //!_NOTIFIERMESSAGE_H_
