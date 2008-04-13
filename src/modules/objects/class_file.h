#ifndef	_CLASS_FILE_H_
#define	_CLASS_FILE_H_
//vim: ts=8
//   File : class_file.h
//   Creation date : Thu Sep 21 04:43:01 CEST 2000 by Krzysztof Godlewski 
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
//



#include "kvi_string.h"

#include <QFile>

/*
//class KviCommand;
class KviParameterList;

class KviScriptFileObject : public KviScriptObject
{
	Q_OBJECT
public:
	KviScriptFileObject(KviScriptObjectClass *, KviScriptObject *, const char *);
	virtual ~KviScriptFileObject();

	static void registerSelf();
	static void unregisterSelf();

protected:
	bool functionSetName(KviCommand *, KviParameterList *, KviStr &);
	bool functionName(KviCommand *, KviParameterList *, KviStr &);
	bool functionOpen(KviCommand *, KviParameterList *, KviStr &);
	bool functionIsOpen(KviCommand *, KviParameterList *, KviStr &);
	bool functionClose(KviCommand *, KviParameterList *, KviStr &);
	bool functionFlush(KviCommand *, KviParameterList *, KviStr &);
	bool functionSize(KviCommand *, KviParameterList *, KviStr &);
	
	bool functionAtEnd(KviCommand *, KviParameterList *, KviStr &);
		// int QFile at() const
	bool functionWhere(KviCommand *, KviParameterList *, KviStr &);
		// void QFile at(int)
	bool functionSeek(KviCommand *, KviParameterList *, KviStr &);
	bool functionPutch(KviCommand *, KviParameterList *, KviStr &);
	bool functionGetch(KviCommand *, KviParameterList *, KviStr &);
	bool functionUngetch(KviCommand *, KviParameterList *, KviStr &);
	
	bool functionReadBlock(KviCommand *, KviParameterList *, KviStr &);
	bool functionWriteBlock(KviCommand *, KviParameterList *, KviStr &);

	bool functionReadLine(KviCommand *, KviParameterList *, KviStr &);
	bool functionWriteLine(KviCommand *, KviParameterList *, KviStr &);
	bool functionHexWrite(KviCommand *, KviParameterList *, KviStr &);
	bool functionHexRead(KviCommand *, KviParameterList *, KviStr &);
	
protected:
	QFile * m_pFile;
};

*/

#include "object_macros.h"

class KviKvsObject_file : public KviKvsObject
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_file)
protected:
  	bool functionsetName(KviKvsObjectFunctionCall *c);
	bool functionname(KviKvsObjectFunctionCall *c);
	bool functionopen(KviKvsObjectFunctionCall *c);
	bool functionisOpen(KviKvsObjectFunctionCall *c);
	bool functionclose(KviKvsObjectFunctionCall *c);
	bool functionflush(KviKvsObjectFunctionCall *c);
	bool functionsize(KviKvsObjectFunctionCall *c);
	
	bool functionatEnd(KviKvsObjectFunctionCall *c);
		// int QFile at() const
	bool functionwhere(KviKvsObjectFunctionCall *c);
		// void QFile at(int)
	bool functionseek(KviKvsObjectFunctionCall *c);
	bool functionputch(KviKvsObjectFunctionCall *c);
	bool functiongetch(KviKvsObjectFunctionCall *c);
	bool functionunGetch(KviKvsObjectFunctionCall *c);
	
	bool functionreadBlock(KviKvsObjectFunctionCall *c);
	bool functionwriteBlock(KviKvsObjectFunctionCall *c);

	bool functionreadLine(KviKvsObjectFunctionCall *c);
	bool functionwriteLine(KviKvsObjectFunctionCall *c);
/*	bool functionHexWrite(KviKvsObjectFunctionCall *c);
	bool functionHexRead(KviKvsObjectFunctionCall *c);
*/
protected:
	QFile * m_pFile;
};
#endif	// !_CLASS_FILE_H_
