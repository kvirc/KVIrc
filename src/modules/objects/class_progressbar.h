#ifndef	_CLASS_PROGRESSBAR_H_

#define	_CLASS_PROGRESSBAR_H_
//
//   File : class_progressbar.h
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2005 Szymon Stefanek (pragma at kvirc dot net)
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

#include <qprogressbar.h>
#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_progressbar : public KviKvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_progressbar)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionSetProgress(KviKvsObjectFunctionCall *c);
	bool functionSetTotalSteps(KviKvsObjectFunctionCall *c);
	bool functionReset(KviKvsObjectFunctionCall *c);
	bool functionSetPercentageVisible(KviKvsObjectFunctionCall *c);
	bool functionSetCenterIndicator(KviKvsObjectFunctionCall *c);
	bool functionCenterIndicator(KviKvsObjectFunctionCall *c);
	bool functionPercentageVisible(KviKvsObjectFunctionCall *c);
};


#endif	//!_CLASS_PROGRESSBAR_H_

