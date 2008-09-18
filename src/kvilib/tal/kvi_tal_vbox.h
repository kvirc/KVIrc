#ifndef _KVI_TAL_VBOX_H_
#define _KVI_TAL_VBOX_H_

//=============================================================================
//
//   File : kvi_tal_vbox.h
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include <QWidget>
#include <QVBoxLayout>

class KVILIB_API KviTalVBox : public QWidget
{
	Q_OBJECT
public:
	KviTalVBox(QWidget * pParent,char* name=0);
	~KviTalVBox();
	void setStretchFactor(QWidget * child,int stretch);
	void setSpacing(int spacing);
	void setMargin(int margin);
	void setAlignment(Qt::Alignment);
	void addStretch(int stretch);
	void setAlignment(QWidget * child,Qt::Alignment alignment);
protected:
	virtual void childEvent(QChildEvent * e);
private:
	QVBoxLayout * m_pLayout;
};

#endif // _KVI_TAL_VBOX_H_
