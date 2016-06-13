#ifndef _OPTW_NICKSERV_H_
#define _OPTW_NICKSERV_H_
//=============================================================================
//
//   File : OptionsWidget_nickserv.h
//   Creation date : Fri Aug 10 2001 03:37:21 CEST by Szymon Stefanek
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
#include "KviOptionsWidget.h"

#include <QDialog>
#include <QCheckBox>

class QLineEdit;
class QPushButton;
class KviNickServRule;
class QTreeWidget;

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_nickServ KviIconManager::NickServ
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_nickServ __tr2qs_no_lookup("NickServ")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_nickServ OptionsWidget_tools
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_nickServ __tr2qs_no_lookup("authentication,identify")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_nickServ 10000
//#define KVI_OPTIONS_WIDGET_HIDDEN_OptionsWidget_nickServ true

class NickServRuleEditor : public QDialog
{
	Q_OBJECT
public:
	NickServRuleEditor(QWidget * par, bool bUseServerMaskField = false);
	~NickServRuleEditor();

protected:
	QLineEdit * m_pRegisteredNickEdit;
	QLineEdit * m_pNickServMaskEdit;
	QLineEdit * m_pMessageRegexpEdit;
	QLineEdit * m_pIdentifyCommandEdit;
	QLineEdit * m_pServerMaskEdit;
	QPushButton * m_pOkButton;

public:
	bool editRule(KviNickServRule * r);

protected:
	bool validate();
protected slots:
	void okPressed();
};

class OptionsWidget_nickServ : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_nickServ(QWidget * parent);
	~OptionsWidget_nickServ();

protected:
	QTreeWidget * m_pNickServTreeWidget;
	QCheckBox * m_pNickServCheck;
	QPushButton * m_pAddRuleButton;
	QPushButton * m_pDelRuleButton;
	QPushButton * m_pEditRuleButton;
protected slots:
	void enableDisableNickServControls();
	void addNickServRule();
	void delNickServRule();
	void editNickServRule();

public:
	virtual void commit();
};

#endif //_OPTW_NICKSERV_H_
