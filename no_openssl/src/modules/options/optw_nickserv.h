#ifndef _OPTW_NICKSERV_H_
#define _OPTW_NICKSERV_H_

//=============================================================================
//
//   File : optw_nickserv.h
//   Creation date : Fri Aug 10 2001 03:37:21 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_settings.h"
#include "kvi_optionswidget.h"

#include <QDialog>
#include <QCheckBox>

class QLineEdit;
class QPushButton;
class KviNickServRule;
class QTreeWidget;

#define KVI_OPTIONS_WIDGET_ICON_KviNickServOptionsWidget KVI_SMALLICON_NICKSERV
#define KVI_OPTIONS_WIDGET_NAME_KviNickServOptionsWidget __tr2qs_no_lookup("NickServ")
#define KVI_OPTIONS_WIDGET_PARENT_KviNickServOptionsWidget KviToolsOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviNickServOptionsWidget __tr2qs_no_lookup("authentication,identify")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviNickServOptionsWidget 10000
//#define KVI_OPTIONS_WIDGET_HIDDEN_KviNickServOptionsWidget true

class KviNickServRuleEditor : public QDialog
{
	Q_OBJECT
public:
	KviNickServRuleEditor(QWidget * par,bool bUseServerMaskField = false);
	~KviNickServRuleEditor();
protected:
	QLineEdit   * m_pRegisteredNickEdit;
	QLineEdit   * m_pNickServMaskEdit;
	QLineEdit   * m_pMessageRegexpEdit;
	QLineEdit   * m_pIdentifyCommandEdit;
	QLineEdit   * m_pServerMaskEdit;
	QPushButton * m_pOkButton;
public:
	bool editRule(KviNickServRule * r);
protected:
	bool validate();
protected slots:
	void okPressed();
};

class KviNickServOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviNickServOptionsWidget(QWidget * parent);
	~KviNickServOptionsWidget();
protected:
	QTreeWidget * m_pNickServTreeWidget;
	QCheckBox        * m_pNickServCheck;
	QPushButton      * m_pAddRuleButton;
	QPushButton      * m_pDelRuleButton;
	QPushButton      * m_pEditRuleButton;
protected slots:
	void enableDisableNickServControls();
	void addNickServRule();
	void delNickServRule();
	void editNickServRule();
public:
	virtual void commit();
};

#endif //_OPTW_NICKSERV_H_
