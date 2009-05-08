#ifndef _CLASS_PROCESS_H_
#define _CLASS_PROCESS_H_
//=============================================================================
//
//   File : class_process.h
//   Creation date : Fri Mar 18 21:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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


#include <QTimer>
#include "class_widget.h"


#include "object_macros.h"
#include "kvi_process.h"


class KviKvsObject_process : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_process)
protected:
	KviProcess * m_pProcess;
private:
	QStringList args;
public:

	bool addArgument(KviKvsObjectFunctionCall *c);
	bool startProcess(KviKvsObjectFunctionCall *c);
	bool readStdout(KviKvsObjectFunctionCall *c);
	bool readStderr(KviKvsObjectFunctionCall *c);
	bool writeToStdin(KviKvsObjectFunctionCall *c);
	bool closekill(KviKvsObjectFunctionCall *c);
	bool closeStdin(KviKvsObjectFunctionCall *c);
	bool kill(KviKvsObjectFunctionCall *c);
	bool tryTerminate(KviKvsObjectFunctionCall *c);
	bool normalExit(KviKvsObjectFunctionCall *c);
	bool isRunning(KviKvsObjectFunctionCall *c);

	bool readyReadStdoutEvent(KviKvsObjectFunctionCall *c);
	bool readyReadStderrEvent(KviKvsObjectFunctionCall *c);

protected slots:
	void slotReadStdout();
	void slotReadStderr();
};


#endif	// !_CLASS_PROCESS_H_
