#ifndef _PACKADDONDIALOG_H_
#define _PACKADDONDIALOG_H_
//=============================================================================
//
//   File : packaddondialog.h
//   Created on Sat 03 May 2008 01:40:44 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
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
#include "kvi_pointerlist.h"
#include "kvi_tal_wizard.h"
#include "kvi_tal_textedit.h"
#include <QLineEdit>
#include <QLabel>
#include <QWidget>

class KviFileSelector;
class KviPackAddonCreateInfoPackageWidget;
class KviPackAddonInfoWidget;

class KviPackAddonDialog : public KviTalWizard
{
	Q_OBJECT
public:
	KviPackAddonDialog(QWidget * pParent);
	~KviPackAddonDialog(){};
	KviPackAddonCreateInfoPackageWidget *m_pPackAddonInfoCreateWidget ;
	KviPackAddonInfoWidget *m_pPackAddonInfoWidget ;
protected:
	QString           m_szPackagePath;
	KviFileSelector * m_pPathSelector;
protected:
	virtual void accept();
	bool packAddon();
};

class KviPackAddonCreateInfoPackageWidget : public QWidget
{
	Q_OBJECT
public:
	KviPackAddonCreateInfoPackageWidget(KviPackAddonDialog *pParent);
	~KviPackAddonCreateInfoPackageWidget();
	QString packagerName(){return m_pPackagerNameEdit->text();};
	QString packageName(){return m_pPackageNameEdit->text();};
	QString packageVersion(){return m_pPackageVersionEdit->text();};
	QString packageDescription(){return m_pPackageDescriptionEdit->text();};
protected:
	QLineEdit *m_pPackagerNameEdit;
	QLineEdit *m_pPackageNameEdit;
	QLineEdit *m_pPackageVersionEdit;
	KviTalTextEdit *m_pPackageDescriptionEdit;


};

class KviPackAddonInfoWidget : public QWidget
{
public:
	KviPackAddonInfoWidget(KviPackAddonDialog *);
	~KviPackAddonInfoWidget();
protected:
	KviPackAddonDialog *m_pParent;
	QLabel *m_pLabelInfo;
	QLabel *m_pLabelAuthor;
	QLabel *m_pPackageName;
	QLabel *m_pPackageVersion;
	QLabel *m_pPackageDescription;
	void showEvent(QShowEvent *event);
};




#endif //!_PACKADDONDIALOG_H_
