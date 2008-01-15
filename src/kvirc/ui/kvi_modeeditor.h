#ifndef _KVI_MODEEDITOR_H_
#define _KVI_MODEEDITOR_H_

//
//   File : kvi_modeeditor.h
//   Creation date : Sat Apr 14 2001 13:50:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include <qwidget.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlabel.h>

#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_styled_controls.h"
#include "kvi_toolwindows_container.h"

class KviConsole;

//////////////////////////////////////////////////////////////////////
// class KviModeEditor
//
//////////////////////////////////////////////////////////////////////

class KVIRC_API KviModeEditor : public KviWindowToolWidget
{
	Q_OBJECT
public:
	KviModeEditor(QWidget * par,KviWindowToolPageButton* button,const char * nam,KviConsole * c,const char * mode,const char * key,const char * limit);
	~KviModeEditor();
protected: // fields
	KviStr m_szMode;
	KviStr m_szKey;
	KviStr m_szLimit;
	KviPointerList<KviStyledCheckBox> * m_pCheckBoxes;
	KviStyledCheckBox * m_pLimitBox;
	QLineEdit * m_pLimitEdit;
	KviStyledCheckBox * m_pKeyBox;
	QLineEdit * m_pKeyEdit;
signals:
	void setMode(const char *);
	void done();
protected slots:
	void limitBoxToggled(bool bChecked);
	void keyBoxToggled(bool bChecked);
	void commit();
};

#endif //_KVI_MODEEDITOR_H_
