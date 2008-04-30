#ifndef _KVI_TAL_GROUPBOX_H_
#define _KVI_TAL_GROUPBOX_H_

//=============================================================================
//
//   File : kvi_tal_groupbox.h
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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

/*

class KVILIB_API KviTalGroupBox : public Q3GroupBox
{
	Q_OBJECT
public:
	KviTalGroupBox(QWidget * parent = 0)
	: Q3GroupBox(parent) {};
	KviTalGroupBox(const QString & title,QWidget * parent = 0)
	: Q3GroupBox(title,parent) {};
	KviTalGroupBox(int strips,Qt::Orientation orientation,QWidget * parent = 0)
	: Q3GroupBox(strips,orientation,parent) {};
	KviTalGroupBox(int strips,Qt::Orientation orientation,const QString & title,QWidget * parent = 0)
	: Q3GroupBox(strips,orientation,title,parent) {};
	~KviTalGroupBox() {};
};
*/
#include <QGroupBox>
#include <QLayout>

class KVILIB_API KviTalGroupBox : public QGroupBox
{
	Q_OBJECT
public:
	KviTalGroupBox(QWidget * pParent,char* name=0);
	~KviTalGroupBox();
	KviTalGroupBox(QWidget * pParent = 0)
	: QGroupBox (pParent){m_pLayout=0;};
	KviTalGroupBox(const QString & title,QWidget * pParent = 0)
	: QGroupBox (title,pParent){m_pLayout=0;};
	KviTalGroupBox(int strips,Qt::Orientation orientation,QWidget * parent = 0);
	KviTalGroupBox(int strips,Qt::Orientation orientation,const QString & title,QWidget * parent = 0);
	void setInsideMargin(int margin){if (m_pLayout) m_pLayout->setMargin(margin);};
	void setInsideSpacing(int spacing){if (m_pLayout) m_pLayout->setSpacing(spacing);};
	int insideMargin(){if (m_pLayout)return m_pLayout->margin();return 0;};
	int insideSpacing(){if (m_pLayout)return m_pLayout->spacing();return 0;};
	void addSpace(int space)
	{
		if (m_pLayout){
			if (mOrientation==Qt::Vertical)
				((QHBoxLayout*)m_pLayout)->addSpacing(space);
			else 
				((QVBoxLayout*)m_pLayout)->addSpacing(space);
		}
	};

	/*
	void setStretchFactor(QWidget *,int);
	void setSpacing(int);
	void setMargin(int);
	void setAlignment(Qt::Alignment);
	*/

protected:
	Qt::Orientation mOrientation;
	virtual void childEvent(QChildEvent * e);
private:
	QLayout * m_pLayout;
};
#endif // _KVI_TAL_GROUPBOX_H_
