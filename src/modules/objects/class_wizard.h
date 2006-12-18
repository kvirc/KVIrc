#ifndef	_CLASS_WIZARD_H_
#define	_CLASS_WIZARD_H_
//
//   File : class_wizard.h
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
#include <qwizard.h>
#include "class_widget.h"


#include "object_macros.h"

class KviKvsObject_wizard : public KviKvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_wizard)
public:
	QWidget * widget() { return (QWidget *)object(); };
	bool accept();
	bool reject();
	void nextClicked();
	void backClicked();


protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

    bool functionaddPage(KviKvsObjectFunctionCall *c);
	bool functioninsertPage(KviKvsObjectFunctionCall *c);
	bool functionremovePage(KviKvsObjectFunctionCall *c);
	bool functionsetTitle(KviKvsObjectFunctionCall *c);
	bool functionsetBackEnabled(KviKvsObjectFunctionCall *c);
	bool functionsetNextEnabled(KviKvsObjectFunctionCall *c);
	bool functionsetFinishEnabled(KviKvsObjectFunctionCall *c);
	bool functionsetHelpEnabled(KviKvsObjectFunctionCall *c);

	bool functionsetBackBtnText(KviKvsObjectFunctionCall *c);
	bool functionsetNextBtnText(KviKvsObjectFunctionCall *c);
	bool functionsetFinishBtnText(KviKvsObjectFunctionCall *c);
	bool functionsetHelpBtnText(KviKvsObjectFunctionCall *c);

	bool functionnextClickedEvent(KviKvsObjectFunctionCall *c);
	bool functionbackClickedEvent(KviKvsObjectFunctionCall *c);

};
class KviKvsMdmWizard :  public QWizard

{
	Q_OBJECT
public:
	KviKvsMdmWizard(QWidget * par,const char * name,KviKvsObject_wizard *);
	void accept();
	void reject();
	virtual ~KviKvsMdmWizard();
protected:
	KviKvsObject_wizard *m_pParentScript;
protected slots:
	void slotNextClicked();
	void slotBackClicked();
};
#endif	//!_CLASS_WIZARD_H_
