#ifndef _KVI_MODEEDITOR_H_
#define _KVI_MODEEDITOR_H_
//=============================================================================
//
//   File : KviModeEditor.h
//   Creation date : Sat Apr 14 2001 13:50:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviWindowToolWidget.h"

#include <QMap>

class QCheckBox;
class QLineEdit;
class QPushButton;
class QString;
class KviIrcConnectionServerInfo;
class KviChannelWindow;

//
// class KviModeEditor
//

class KVIRC_API KviModeEditor : public KviWindowToolWidget
{
	Q_OBJECT
public:
	KviModeEditor(QWidget * par, KviWindowToolPageButton * button, const char * name, KviChannelWindow * pChan);
	~KviModeEditor();

protected: // fields
	KviChannelWindow * m_pChannel;
	QPushButton * m_pButton;
	QMap<char, QCheckBox *> m_pCheckBoxes;
	QMap<char, QLineEdit *> m_pLineEdits;

protected:
	const QString * getModeDescription(char cMode);
	bool modeNeedsParameterOnlyWhenSet(char cMode);
signals:
	void setMode(const QString & szMode);
	void done();
protected slots:
	void checkBoxToggled(bool bChecked);
	void commit();
	void updateOpStatus();
};

#endif //_KVI_MODEEDITOR_H_
