#ifndef	_CLASS_PROCESS_H_
#define	_CLASS_PROCESS_H_
//mdm:
//   File : class_process.h
//   Creation date : Thu Feb 1 14:39:48 CEST 2005
//	 by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

	bool functionaddArgument(KviKvsObjectFunctionCall *c);
	bool functionstartProcess(KviKvsObjectFunctionCall *c);
	bool functionreadStdout(KviKvsObjectFunctionCall *c);
	bool functionreadStderr(KviKvsObjectFunctionCall *c);
	bool functionwriteToStdin(KviKvsObjectFunctionCall *c);
	bool functionclosekill(KviKvsObjectFunctionCall *c);
	bool functioncloseStdin(KviKvsObjectFunctionCall *c);
	bool functionkill(KviKvsObjectFunctionCall *c);
	bool functiontryTerminate(KviKvsObjectFunctionCall *c);
	bool functionnormalExit(KviKvsObjectFunctionCall *c);
	bool functionisRunning(KviKvsObjectFunctionCall *c);

	bool functionreadyReadStdoutEvent(KviKvsObjectFunctionCall *c);
	bool functionreadyReadStderrEvent(KviKvsObjectFunctionCall *c);

protected slots:
	void slotReadStdout();
	void slotReadStderr();
};


#endif	// !_CLASS_PROCESS_H_
