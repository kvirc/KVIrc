#ifndef _CLASS_WIZARD_H_
#define _CLASS_WIZARD_H_
//=============================================================================
//
//   File : KvsObject_wizard.h
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include <KviTalWizard.h>
#include "KvsObject_widget.h"
#include "object_macros.h"

class KvsObject_wizard : public KvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_wizard)
public:
	QWidget * widget() { return (QWidget *)object(); };
	bool accept();
	bool reject();
	void nextClicked();
	void backClicked();

protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool addPage(KviKvsObjectFunctionCall * c);
	bool insertPage(KviKvsObjectFunctionCall * c);
	bool removePage(KviKvsObjectFunctionCall * c);
	bool setTitle(KviKvsObjectFunctionCall * c);
	bool setBackEnabled(KviKvsObjectFunctionCall * c);
	bool setNextEnabled(KviKvsObjectFunctionCall * c);
	bool setFinishEnabled(KviKvsObjectFunctionCall * c);
	bool setHelpEnabled(KviKvsObjectFunctionCall * c);

	bool setBackBtnText(KviKvsObjectFunctionCall * c);
	bool setNextBtnText(KviKvsObjectFunctionCall * c);
	bool setFinishBtnText(KviKvsObjectFunctionCall * c);
	bool setHelpBtnText(KviKvsObjectFunctionCall * c);

	bool nextClickedEvent(KviKvsObjectFunctionCall * c);
	bool backClickedEvent(KviKvsObjectFunctionCall * c);
};

class KviKvsMdmWizard : public KviTalWizard
{
	Q_OBJECT
public:
	KviKvsMdmWizard(QWidget * par, const char * name, KvsObject_wizard *);
	void accept();
	void reject();
	~KviKvsMdmWizard();

protected:
	KvsObject_wizard * m_pParentScript;
protected slots:
	void slotNextClicked();
	void slotBackClicked();
};
#endif //!_CLASS_WIZARD_H_
