#ifndef _KVI_SCRIPTEDITOR_H_
#define _KVI_SCRIPTEDITOR_H_
//=============================================================================
//
//   File : KviScriptEditor.h
//   Creation date : Sun Mar 28 1999 16:11:48 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include <QWidget>
#include <QByteArray>

class QLineEdit;

//
// This is the basic interface of a script editor widget
//
// Actually the REAL script editor is implemented in an
// external module called "libkvieditor", and it is a really
// huge class.
// The constructor of this thing is protected: forget
// about instantiating it directly
//

class KVIRC_API KviScriptEditor : public QWidget
{
	Q_OBJECT
protected:
	KviScriptEditor(QWidget * par) : QWidget(par){};

public:
	virtual void setText(const char * txt) { setText(QByteArray(txt)); };
	virtual void setText(const QByteArray & txt){};
	virtual void setText(const QString & txt);
	virtual void getText(QByteArray & txt){};
	virtual void getText(QString & txt);
	virtual void setInfoText(const QString & text){};
	virtual void setFindText(const QString & text){};
	virtual void setCursorPosition(int){};
	virtual void setReadOnly(bool bReadonly){};
	virtual int getCursor() { return 0; };
	virtual void setFindLineeditReadOnly(bool b){};
	virtual bool isModified() { return false; };
	virtual void setUnHighlightedText(const QString & szText){};
	virtual void setModified(bool){};
	static KviScriptEditor * getDummyEditor(QWidget * par) { return new KviScriptEditor(par); };
	static KviScriptEditor * createInstance(QWidget * par);
	static void destroyInstance(KviScriptEditor * e);
};

#endif //!_KVI_SCRIPTEDITOR_H_
