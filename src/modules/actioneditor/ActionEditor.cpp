//=============================================================================
//
//   File : ActionEditor.cpp
//   Creation date : Tue 29 Dec 2004 02:45:59 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "ActionEditor.h"

#include "KviIconManager.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviImageDialog.h"
#include "KviConfigurationFile.h"
#include "KviFileUtils.h"
#include "KviScriptEditor.h"
#include "kvi_debug.h"
#include "KviApplication.h"
#include "KviKvsAliasManager.h"
#include "KviFileDialog.h"
#include "KviActionManager.h"
#include "KviAction.h"
#include "KviKvsUserAction.h"
#include "KviCustomToolBarManager.h"
#include "KviTalVBox.h"
#include "KviTalGroupBox.h"
#include "KviTalIconAndRichTextItemDelegate.h"
#include "kvi_fileextensions.h"

#include <QList>
#include <QSplitter>
#include <QLayout>
#include <QToolTip>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPainter>
#include <QTabWidget>
#include <QLabel>

extern ActionEditorWindow * g_pActionEditorWindow;

static QString g_szLastEditedAction;

#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)

ActionEditorTreeWidgetItem::ActionEditorTreeWidgetItem(QTreeWidget * v, ActionData * a)
    : QTreeWidgetItem(v)
{
	m_pActionData = a;
	m_pTreeWidget = v;
	//setFlags(Qt::ItemIsUserSelectable);
	QString t = "<b>" + m_pActionData->m_szName + "</b>";
	t += R"(<br><font color="#454545" size="-1">)" + m_pActionData->m_szVisibleName + "</font>";
	m_szKey = m_pActionData->m_szName.toUpper();
	setText(0, t);
	QPixmap * p = g_pIconManager->getBigIcon(m_pActionData->m_szBigIcon);
	if(p)
		setIcon(0, *p);
}

ActionEditorTreeWidgetItem::~ActionEditorTreeWidgetItem()
{
	delete m_pActionData;
}

QString ActionEditorTreeWidgetItem::key(int, bool) const
{
	return m_szKey;
}

SingleActionEditor::SingleActionEditor(QWidget * par, ActionEditor * ed)
    : QWidget(par)
{
	m_pActionEditor = ed;
	m_pActionData = nullptr;

	QGridLayout * g = new QGridLayout(this);

	QLabel * l = new QLabel(__tr2qs_ctx("Name:", "editor"), this);
	g->addWidget(l, 0, 0);
	m_pNameEdit = new QLineEdit(this);
	g->addWidget(m_pNameEdit, 0, 1);
	m_pNameEdit->setToolTip(__tr2qs_ctx("Internal unique name for the action", "editor"));

	l = new QLabel(__tr2qs_ctx("Label:", "editor"), this);
	g->addWidget(l, 1, 0);
	m_pVisibleNameEdit = new QLineEdit(this);
	g->addWidget(m_pVisibleNameEdit, 1, 1);
	m_pVisibleNameEdit->setToolTip(__tr2qs_ctx("Visible name for this action.<br>This string will be displayed to the user so it is a good idea to use $tr() here", "editor"));

	QTabWidget * tw = new QTabWidget(this);
	g->addWidget(tw, 2, 0, 1, 2);

	// code tab
	QWidget * tab = new QWidget(tw);
	QGridLayout * gl = new QGridLayout(tab);

	m_pScriptEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pScriptEditor, 0, 0);
	m_pScriptEditor->setToolTip(__tr2qs_ctx("Action code", "editor"));

	tw->addTab(tab, __tr2qs_ctx("Code", "editor"));

	// properties tab
	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	l = new QLabel(__tr2qs_ctx("Category:", "editor"), tab);
	gl->addWidget(l, 0, 0);
	m_pCategoryCombo = new QComboBox(tab);
	gl->addWidget(m_pCategoryCombo, 0, 1, 1, 3);
	m_pCategoryCombo->setToolTip(__tr2qs_ctx("Choose the category that best fits for this action", "editor"));

	l = new QLabel(__tr2qs_ctx("Description:", "editor"), tab);
	gl->addWidget(l, 1, 0);
	m_pDescriptionEdit = new QLineEdit(tab);
	gl->addWidget(m_pDescriptionEdit, 1, 1, 1, 3);
	m_pDescriptionEdit->setToolTip(__tr2qs_ctx("Visible short description for this action.<br>This string will be displayed to the user so it is a good idea to use $tr() here", "editor"));

	l = new QLabel(__tr2qs_ctx("Small icon:", "editor"), tab);
	gl->addWidget(l, 2, 0);
	m_pSmallIconEdit = new QLineEdit(tab);
	gl->addWidget(m_pSmallIconEdit, 2, 1);
	m_pSmallIconButton = new QToolButton(tab);
	m_pSmallIconButton->setMinimumSize(QSize(20, 20));
	connect(m_pSmallIconButton, SIGNAL(clicked()), this, SLOT(chooseSmallIcon()));
	gl->addWidget(m_pSmallIconButton, 2, 2);
	QString s = __tr2qs_ctx("The small icon associated to this action.<br>"
	                        "It will appear at least in the popup menus when this action is inserted.<br>"
	                        "It has to be 16x16 pixels.", "editor");

	m_pSmallIconEdit->setToolTip(s);
	m_pSmallIconButton->setToolTip(s);

	l = new QLabel(__tr2qs_ctx("Big icon:", "editor"), tab);
	gl->addWidget(l, 3, 0);
	m_pBigIconEdit = new QLineEdit(tab);
	gl->addWidget(m_pBigIconEdit, 3, 1);
	m_pBigIconButton = new QToolButton(tab);
	m_pBigIconButton->setMinimumSize(QSize(48, 48));
	m_pBigIconButton->setIconSize(QSize(32, 32));
	connect(m_pBigIconButton, SIGNAL(clicked()), this, SLOT(chooseBigIcon()));
	gl->addWidget(m_pBigIconButton, 3, 2, 2, 2);
	s = __tr2qs_ctx("The big icon associated to this action.<br>"
	                "It will appear at least in the toolbar buttons when this action is inserted.<br>"
	                "It has to be 32x32 pixels.", "editor");

	m_pBigIconEdit->setToolTip(s);
	m_pBigIconButton->setToolTip(s);

	l = new QLabel(__tr2qs_ctx("Key sequence:", "editor"), tab);
	gl->addWidget(l, 4, 0, 2, 1);
	m_pKeySequenceEdit = new QLineEdit(tab);
	gl->addWidget(m_pKeySequenceEdit, 4, 1, 2, 1);
	m_pKeySequenceEdit->setToolTip(__tr2qs_ctx("Optional keyboard sequence that will activate this action.<br>"
	                                           "The sequence has to be expressed as a string of up to four key codes separated by commas "
	                                           "eventually combined with the modifiers \"Ctrl\", \"Shift\", \"Alt\" and \"Meta\".<br>"
	                                           "Examples of such sequences are \"Ctrl+X\", \"Ctrl+Alt+Z\", \"Ctrl+X,Ctrl+C\".", "editor"));
	l = new QLabel(tab);
	gl->addWidget(l, 6, 0, 1, 4);

	gl->setColumnStretch(1, 1);
	gl->setRowStretch(6, 1);

	tw->addTab(tab, __tr2qs_ctx("Properties", "editor"));

	// flags tab
	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	m_pNeedsContextCheck = new QCheckBox(__tr2qs_ctx("Needs IRC context", "editor"), tab);
	connect(m_pNeedsContextCheck, SIGNAL(toggled(bool)), this, SLOT(needsContextCheckToggled(bool)));
	m_pNeedsContextCheck->setToolTip(__tr2qs_ctx("Check this option if this action should be enabled only when "
	                                             "the active window belongs to an IRC context", "editor"));

	gl->addWidget(m_pNeedsContextCheck, 0, 0, 1, 4);

	l = new QLabel(tab);
	l->setMinimumWidth(40);
	gl->addWidget(l, 1, 0);

	m_pNeedsConnectionCheck = new QCheckBox(__tr2qs_ctx("Needs IRC connection", "editor"), tab);
	connect(m_pNeedsConnectionCheck, SIGNAL(toggled(bool)), this, SLOT(needsConnectionCheckToggled(bool)));
	m_pNeedsConnectionCheck->setToolTip(__tr2qs_ctx("Check this option if this action should be enabled only when "
	                                                "the active window has an active IRC connection", "editor"));

	gl->addWidget(m_pNeedsConnectionCheck, 1, 0, 1, 4);

	l = new QLabel(tab);
	l->setMinimumWidth(40);
	gl->addWidget(l, 2, 1);

	m_pEnableAtLoginCheck = new QCheckBox(__tr2qs_ctx("Enable at login", "editor"), tab);
	m_pEnableAtLoginCheck->setToolTip(__tr2qs_ctx("Check this option if this action should be enabled also during the login operations. "
	                                              "i.e. when the logical IRC connection hasn't been established yet.", "editor"));

	gl->addWidget(m_pEnableAtLoginCheck, 2, 0, 1, 4);

	m_pSpecificWindowsCheck = new QCheckBox(__tr2qs_ctx("Enable only in specified windows", "editor"), tab);
	connect(m_pSpecificWindowsCheck, SIGNAL(toggled(bool)), this, SLOT(specificWindowsCheckToggled(bool)));
	m_pSpecificWindowsCheck->setToolTip(__tr2qs_ctx("Check this option if this action should be enabled only when "
	                                                "the active window is of a specified type", "editor"));

	gl->addWidget(m_pSpecificWindowsCheck, 3, 0, 1, 4);

	m_pWindowConsoleCheck = new QCheckBox(__tr2qs_ctx("Enable in console windows", "editor"), tab);
	m_pWindowConsoleCheck->setToolTip(__tr2qs_ctx("Check this option if this action should be enabled only when "
	                                              "the active window is a console", "editor"));

	connect(m_pWindowConsoleCheck, SIGNAL(toggled(bool)), this, SLOT(consoleWindowsCheckToggled(bool)));
	gl->addWidget(m_pWindowConsoleCheck, 4, 1, 1, 4);

	m_pConsoleOnlyIfUsersSelectedCheck = new QCheckBox(__tr2qs_ctx("Only if there are selected users", "editor"), tab);
	m_pConsoleOnlyIfUsersSelectedCheck->setToolTip(__tr2qs_ctx("This will enable the action only if there are "
	                                                           "selected users in the active window", "editor"));

	gl->addWidget(m_pConsoleOnlyIfUsersSelectedCheck, 5, 1, 1, 4);

	m_pWindowChannelCheck = new QCheckBox(__tr2qs_ctx("Enable in channel windows", "editor"), tab);
	m_pWindowChannelCheck->setToolTip(__tr2qs_ctx("Check this option if this action should be enabled only when "
	                                              "the active window is a channel", "editor"));

	connect(m_pWindowChannelCheck, SIGNAL(toggled(bool)), this, SLOT(channelWindowsCheckToggled(bool)));
	gl->addWidget(m_pWindowChannelCheck, 6, 1, 1, 4);

	m_pChannelOnlyIfUsersSelectedCheck = new QCheckBox(__tr2qs_ctx("Only if there are selected users", "editor"), tab);
	m_pChannelOnlyIfUsersSelectedCheck->setToolTip(__tr2qs_ctx("This will enable the action only if there are "
	                                                           "selected users in the active window", "editor"));

	gl->addWidget(m_pChannelOnlyIfUsersSelectedCheck, 7, 1, 1, 4);

	m_pWindowQueryCheck = new QCheckBox(__tr2qs_ctx("Enable in query windows", "editor"), tab);
	m_pWindowQueryCheck->setToolTip(__tr2qs_ctx("Check this option if this action should be enabled only when "
	                                            "the active window is a query", "editor"));

	connect(m_pWindowQueryCheck, SIGNAL(toggled(bool)), this, SLOT(queryWindowsCheckToggled(bool)));
	gl->addWidget(m_pWindowQueryCheck, 8, 1, 1, 4);

	m_pQueryOnlyIfUsersSelectedCheck = new QCheckBox(__tr2qs_ctx("Only if there are selected users", "editor"), tab);
	m_pQueryOnlyIfUsersSelectedCheck->setToolTip(__tr2qs_ctx("This will enable the action only if there are "
	                                                         "selected users in the active window", "editor"));

	gl->addWidget(m_pQueryOnlyIfUsersSelectedCheck, 9, 1, 1, 4);

	m_pWindowDccChatCheck = new QCheckBox(__tr2qs_ctx("Enable in DCC chat windows", "editor"), tab);
	m_pWindowDccChatCheck->setToolTip(__tr2qs_ctx("Check this option if this action should be enabled only when "
	                                              "the active window is a DCC chat", "editor"));

	gl->addWidget(m_pWindowDccChatCheck, 10, 1, 1, 4);

	l = new QLabel(tab);
	gl->addWidget(l, 11, 0, 1, 4);
	gl->setColumnStretch(3, 1);
	gl->setRowStretch(11, 1);

	tw->addTab(tab, __tr2qs_ctx("Flags", "editor"));
	tw->setCurrentIndex(0);

	g->setRowStretch(2, 1);
	g->setColumnStretch(1, 1);

	KviPointerHashTableIterator<QString, KviActionCategory> it(*(KviActionManager::instance()->categories()));
	while(KviActionCategory * ac = it.current())
	{
		m_pCategoryCombo->addItem(ac->visibleName() + " (" + ac->name() + ")");
		++it;
	}
}

SingleActionEditor::~SingleActionEditor()
    = default;

void SingleActionEditor::channelWindowsCheckToggled(bool)
{
	m_pChannelOnlyIfUsersSelectedCheck->setEnabled(m_pWindowChannelCheck->isChecked());
	m_pChannelOnlyIfUsersSelectedCheck->setChecked(false);
}

void SingleActionEditor::consoleWindowsCheckToggled(bool)
{
	m_pConsoleOnlyIfUsersSelectedCheck->setEnabled(m_pWindowConsoleCheck->isChecked());
	m_pConsoleOnlyIfUsersSelectedCheck->setChecked(false);
}

void SingleActionEditor::queryWindowsCheckToggled(bool)
{
	m_pQueryOnlyIfUsersSelectedCheck->setEnabled(m_pWindowQueryCheck->isChecked());
	m_pQueryOnlyIfUsersSelectedCheck->setChecked(false);
}

void SingleActionEditor::needsContextCheckToggled(bool)
{
	m_pEnableAtLoginCheck->setChecked(false);
	m_pEnableAtLoginCheck->setEnabled(false);
	m_pNeedsConnectionCheck->setChecked(false);
	m_pNeedsConnectionCheck->setEnabled(m_pNeedsContextCheck->isChecked());
}

void SingleActionEditor::needsConnectionCheckToggled(bool)
{
	m_pEnableAtLoginCheck->setChecked(false);
	m_pEnableAtLoginCheck->setEnabled(m_pNeedsConnectionCheck->isChecked());
}

void SingleActionEditor::specificWindowsCheckToggled(bool)
{
	m_pWindowConsoleCheck->setChecked(false);
	m_pWindowChannelCheck->setChecked(false);
	m_pWindowQueryCheck->setChecked(false);
	m_pWindowDccChatCheck->setChecked(false);
	m_pQueryOnlyIfUsersSelectedCheck->setChecked(false);
	m_pChannelOnlyIfUsersSelectedCheck->setChecked(false);
	m_pConsoleOnlyIfUsersSelectedCheck->setChecked(false);
	m_pQueryOnlyIfUsersSelectedCheck->setEnabled(false);
	m_pChannelOnlyIfUsersSelectedCheck->setEnabled(false);
	m_pConsoleOnlyIfUsersSelectedCheck->setEnabled(false);
	bool b = m_pSpecificWindowsCheck->isChecked();
	m_pWindowConsoleCheck->setEnabled(b);
	m_pWindowChannelCheck->setEnabled(b);
	m_pWindowQueryCheck->setEnabled(b);
	m_pWindowDccChatCheck->setEnabled(b);
}

void SingleActionEditor::displaySmallIcon(const QString & szIconId)
{
	QPixmap * pix = g_pIconManager->getImage(szIconId);

	if(pix)
	{
		m_pSmallIconEdit->setText(szIconId);
		m_pSmallIconButton->setIcon(QIcon(*pix));
	}
	else
	{
		m_pSmallIconEdit->setText(QString());
		m_pSmallIconButton->setIcon(QIcon());
	}
}

void SingleActionEditor::displayBigIcon(const QString & szIconId)
{
	QPixmap * pix = g_pIconManager->getImage(szIconId);

	if(pix)
	{
		m_pBigIconEdit->setText(szIconId);
		m_pBigIconButton->setIcon(QIcon(*pix));
	}
	else
	{
		m_pBigIconEdit->setText(QString());
		m_pBigIconButton->setIcon(QIcon());
	}
}

void SingleActionEditor::chooseSmallIcon()
{
	if(!m_pActionData)
		return;
	KviImageDialog * d = new KviImageDialog(this);
	int ret = d->exec();
	QString s = d->selectedImage();
	delete d;
	if(ret != QDialog::Accepted)
		return;

	displaySmallIcon(s);
}

void SingleActionEditor::chooseBigIcon()
{
	if(!m_pActionData)
		return;
	KviImageDialog * d = new KviImageDialog(this, QString(), KID_TYPE_ALL, KID_TYPE_FULL_PATH);
	int ret = d->exec();
	QString s = d->selectedImage();
	delete d;
	if(ret != QDialog::Accepted)
		return;

	displayBigIcon(s);
}

void SingleActionEditor::setActionData(ActionData * d)
{
	m_pActionData = d;

	if(d)
	{
		unsigned int uOldFlags = d->m_uFlags;
		d->m_uFlags = KviAction::validateFlags(d->m_uFlags);
		if(d->m_uFlags != uOldFlags)
			qDebug("invalid action flags in SingleActionEditor::setActionData(): %d fixed to %d", uOldFlags, d->m_uFlags);

		m_pNameEdit->setText(d->m_szName);
		m_pNameEdit->setEnabled(true);
		m_pVisibleNameEdit->setText(d->m_szVisibleName);
		m_pVisibleNameEdit->setEnabled(true);
		m_pCategoryCombo->setEnabled(true);
		int i;
		for(i = 0; i < m_pCategoryCombo->count(); i++)
		{
			QString t = m_pCategoryCombo->itemText(i);
			int idx = t.lastIndexOf('(');
			QString r = t.right(t.length() - (idx + 1));
			QString r2 = r.left(r.length() - 1).trimmed();
			if(r2 == d->m_szCategory)
			{
				m_pCategoryCombo->setCurrentIndex(i);
				break;
			}
		}
		if(i == m_pCategoryCombo->count())
		{
			m_pCategoryCombo->setCurrentIndex(0);
		}
		m_pScriptEditor->setText(d->m_szScriptCode);
		m_pScriptEditor->setEnabled(true);
		m_pKeySequenceEdit->setText(d->m_szKeySequence);
		m_pKeySequenceEdit->setEnabled(true);
		m_pDescriptionEdit->setText(d->m_szDescription);
		m_pDescriptionEdit->setEnabled(true);
		m_pSmallIconEdit->setEnabled(true);
		m_pBigIconEdit->setEnabled(true);
		displaySmallIcon(d->m_szSmallIcon);
		displayBigIcon(d->m_szBigIcon);
		m_pSmallIconButton->setEnabled(true);
		m_pBigIconButton->setEnabled(true);
		m_pNeedsContextCheck->setEnabled(true);
		bool b = (d->m_uFlags & KviAction::NeedsContext) || (d->m_uFlags & KviAction::NeedsConnection);
		m_pNeedsContextCheck->setChecked(b);

		m_pNeedsConnectionCheck->setEnabled(b);
		b = (d->m_uFlags & KviAction::NeedsContext) && (d->m_uFlags & KviAction::NeedsConnection);
		m_pNeedsConnectionCheck->setChecked(b);

		m_pEnableAtLoginCheck->setEnabled(b);
		m_pEnableAtLoginCheck->setChecked(b && (d->m_uFlags & KviAction::EnableAtLogin));

		m_pSpecificWindowsCheck->setEnabled(true);
		b = d->m_uFlags & KviAction::InternalWindowMask;
		m_pSpecificWindowsCheck->setChecked(b);

		m_pWindowConsoleCheck->setEnabled(b);
		m_pWindowChannelCheck->setEnabled(b);
		m_pWindowQueryCheck->setEnabled(b);
		m_pWindowDccChatCheck->setEnabled(b);
		b = d->m_uFlags & KviAction::WindowConsole;
		m_pWindowConsoleCheck->setChecked(b);
		m_pConsoleOnlyIfUsersSelectedCheck->setEnabled(b);
		m_pConsoleOnlyIfUsersSelectedCheck->setChecked(b && (d->m_uFlags & KviAction::WindowOnlyIfUsersSelected));
		b = d->m_uFlags & KviAction::WindowChannel;
		m_pWindowChannelCheck->setChecked(b);
		m_pChannelOnlyIfUsersSelectedCheck->setEnabled(b);
		m_pChannelOnlyIfUsersSelectedCheck->setChecked(b && (d->m_uFlags & KviAction::WindowOnlyIfUsersSelected));
		b = d->m_uFlags & KviAction::WindowQuery;
		m_pWindowQueryCheck->setChecked(b);
		m_pQueryOnlyIfUsersSelectedCheck->setEnabled(b);
		m_pQueryOnlyIfUsersSelectedCheck->setChecked(b && (d->m_uFlags & KviAction::WindowOnlyIfUsersSelected));
		m_pWindowDccChatCheck->setChecked(d->m_uFlags & KviAction::WindowDccChat);
	}
	else
	{
		m_pNameEdit->setText("");
		m_pNameEdit->setEnabled(false);
		m_pVisibleNameEdit->setText("");
		m_pVisibleNameEdit->setEnabled(false);
		m_pCategoryCombo->setEnabled(false);
		m_pScriptEditor->setText("");
		m_pScriptEditor->setEnabled(false);
		m_pKeySequenceEdit->setText("");
		m_pKeySequenceEdit->setEnabled(false);
		m_pDescriptionEdit->setText("");
		m_pDescriptionEdit->setEnabled(false);
		m_pSmallIconEdit->setText("");
		m_pSmallIconEdit->setEnabled(false);
		m_pBigIconEdit->setText("");
		m_pBigIconEdit->setEnabled(false);
		m_pSmallIconButton->setIcon(QIcon());
		m_pSmallIconButton->setEnabled(false);
		m_pBigIconButton->setIcon(QPixmap());
		m_pBigIconButton->setEnabled(false);
		m_pNeedsContextCheck->setChecked(false);
		m_pNeedsContextCheck->setEnabled(false);
		m_pNeedsConnectionCheck->setChecked(false);
		m_pNeedsConnectionCheck->setEnabled(false);
		m_pEnableAtLoginCheck->setChecked(false);
		m_pEnableAtLoginCheck->setEnabled(false);
		m_pWindowConsoleCheck->setChecked(false);
		m_pWindowConsoleCheck->setEnabled(false);
		m_pWindowChannelCheck->setChecked(false);
		m_pWindowChannelCheck->setEnabled(false);
		m_pWindowQueryCheck->setChecked(false);
		m_pWindowQueryCheck->setEnabled(false);
		m_pWindowDccChatCheck->setChecked(false);
		m_pWindowDccChatCheck->setEnabled(false);
		m_pSpecificWindowsCheck->setChecked(false);
		m_pSpecificWindowsCheck->setEnabled(false);
		m_pQueryOnlyIfUsersSelectedCheck->setChecked(false);
		m_pChannelOnlyIfUsersSelectedCheck->setChecked(false);
		m_pConsoleOnlyIfUsersSelectedCheck->setChecked(false);
		m_pQueryOnlyIfUsersSelectedCheck->setEnabled(false);
		m_pChannelOnlyIfUsersSelectedCheck->setEnabled(false);
		m_pConsoleOnlyIfUsersSelectedCheck->setEnabled(false);
	}
}

void SingleActionEditor::commit()
{
	if(!m_pActionData)
		return;

	QString tmp = m_pNameEdit->text();
	if(tmp != m_pActionData->m_szName)
	{
		int idx = 1;
		while(KviActionManager::instance()->coreActionExists(tmp) || m_pActionEditor->actionExists(tmp))
		{
			QString tmp2;
			tmp2.setNum(idx);
			tmp = m_pNameEdit->text();
			tmp.append(tmp2);
			idx++;
		}
	}

	m_pActionData->m_szName = tmp;
	m_pScriptEditor->getText(m_pActionData->m_szScriptCode);
	m_pActionData->m_szVisibleName = m_pVisibleNameEdit->text();
	m_pActionData->m_szDescription = m_pDescriptionEdit->text();
	m_pActionData->m_szBigIcon = m_pBigIconEdit->text();
	m_pActionData->m_szSmallIcon = m_pSmallIconEdit->text();
	m_pActionData->m_szKeySequence = m_pKeySequenceEdit->text();
	QString szCat = m_pCategoryCombo->currentText();
	int idx = szCat.lastIndexOf(')');
	if(idx != -1)
		m_pActionData->m_szCategory = szCat.left(idx);
	else
		m_pActionData->m_szCategory = szCat;
	idx = m_pActionData->m_szCategory.lastIndexOf('(');
	if(idx != -1)
		m_pActionData->m_szCategory.remove(0, idx + 1);
	m_pActionData->m_uFlags = 0;
	if(m_pNeedsContextCheck->isChecked())
	{
		m_pActionData->m_uFlags |= KviAction::NeedsContext;
		if(m_pNeedsConnectionCheck->isChecked())
		{
			m_pActionData->m_uFlags |= KviAction::NeedsConnection;
			if(m_pEnableAtLoginCheck->isChecked())
				m_pActionData->m_uFlags |= KviAction::EnableAtLogin;
		}
	}
	if(m_pSpecificWindowsCheck->isChecked())
	{
		if(m_pWindowConsoleCheck->isChecked())
			m_pActionData->m_uFlags |= KviAction::WindowConsole;
		if(m_pWindowChannelCheck->isChecked())
			m_pActionData->m_uFlags |= KviAction::WindowChannel;
		if(m_pWindowQueryCheck->isChecked())
			m_pActionData->m_uFlags |= KviAction::WindowQuery;
		if(m_pWindowDccChatCheck->isChecked())
			m_pActionData->m_uFlags |= KviAction::WindowDccChat;
		if(m_pActionData->m_uFlags & KviAction::InternalWindowMask)
		{
			if(m_pConsoleOnlyIfUsersSelectedCheck->isChecked() || m_pChannelOnlyIfUsersSelectedCheck->isChecked() || m_pQueryOnlyIfUsersSelectedCheck->isChecked())
			{
				m_pActionData->m_uFlags |= KviAction::WindowOnlyIfUsersSelected;
			}
		}
	}

	unsigned int uOldFlags = m_pActionData->m_uFlags;
	m_pActionData->m_uFlags = KviAction::validateFlags(m_pActionData->m_uFlags);
	if(m_pActionData->m_uFlags != uOldFlags)
		qDebug("invalid action flags in SingleActionEditor::commit(): %d fixed to %d", uOldFlags, m_pActionData->m_uFlags);
}

ActionEditorTreeView::ActionEditorTreeView(QWidget * pParent)
    : QTreeWidget(pParent)
{
	setColumnCount(1);
	setHeaderLabel(__tr2qs_ctx("Action", "editor"));
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(true);
	int iWidth = viewport()->width();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)
		iWidth = LVI_MINIMUM_CELL_WIDTH;
	setRootIsDecorated(false);
	setColumnWidth(0, iWidth);
}

ActionEditorTreeView::~ActionEditorTreeView()
    = default;

void ActionEditorTreeView::resizeEvent(QResizeEvent * e)
{
	QTreeWidget::resizeEvent(e);
	int iWidth = viewport()->width();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)
		iWidth = LVI_MINIMUM_CELL_WIDTH;
	setColumnWidth(0, iWidth);
}

ActionEditor::ActionEditor(QWidget * par)
    : QWidget(par)
{

	QGridLayout * l = new QGridLayout(this);
	m_pSplitter = new QSplitter(Qt::Horizontal, this);
	m_pSplitter->setChildrenCollapsible(false);
	l->addWidget(m_pSplitter, 0, 0);

	KviTalVBox * box = new KviTalVBox(m_pSplitter);
	m_pTreeWidget = new ActionEditorTreeView(box);
	KviTalIconAndRichTextItemDelegate * itemDelegate = new KviTalIconAndRichTextItemDelegate(m_pTreeWidget);
	m_pTreeWidget->setItemDelegate(itemDelegate);
	//m_pTreeWidget->setShowSortIndicator(true);
	m_pTreeWidget->setFocusPolicy(Qt::StrongFocus);
	connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

	m_pNewActionButton = new QPushButton(__tr2qs_ctx("New Action", "editor"), box);
	connect(m_pNewActionButton, SIGNAL(clicked()), this, SLOT(newAction()));

	m_pDeleteActionsButton = new QPushButton(__tr2qs_ctx("Delete Actions", "editor"), box);
	connect(m_pDeleteActionsButton, SIGNAL(clicked()), this, SLOT(deleteActions()));

	m_pExportActionsButton = new QPushButton(__tr2qs_ctx("Export Actions...", "editor"), box);
	connect(m_pExportActionsButton, SIGNAL(clicked()), this, SLOT(exportActions()));

	box->setSpacing(1);

	m_pSingleActionEditor = new SingleActionEditor(m_pSplitter, this);

	ActionEditorTreeWidgetItem * last = nullptr;
	ActionEditorTreeWidgetItem * first = nullptr;

	KviPointerHashTableIterator<QString, KviAction> it(*(KviActionManager::instance()->actions()));
	while(KviAction * a = it.current())
	{
		if(a->isKviUserActionNeverOverrideThis())
		{
			ActionData * ad = new ActionData(
			    a->name(),
			    ((KviKvsUserAction *)a)->scriptCode(),
			    ((KviKvsUserAction *)a)->visibleNameCode(),
			    ((KviKvsUserAction *)a)->descriptionCode(),
			    a->category() ? a->category()->name() : KviActionManager::categoryGeneric()->name(),
			    a->bigIconId(),
			    a->smallIconId(),
			    a->flags(),
			    a->keySequence(),
			    nullptr);
			ActionEditorTreeWidgetItem * lvi = new ActionEditorTreeWidgetItem(m_pTreeWidget, ad);
			ad->m_pItem = lvi;
			if(ad->m_szName == g_szLastEditedAction)
				last = lvi;
			if(!first)
				first = lvi;
		}
		++it;
	}

	if(!last)
		last = first; // try to sleect the first one then

	if(last)
	{
		m_pTreeWidget->setCurrentItem(last);
		currentItemChanged(last, last);
	}
	else
	{
		currentItemChanged(nullptr, nullptr);
	}
}

ActionEditor::~ActionEditor()
    = default;

void ActionEditor::exportActions()
{
	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))
		szName += KVI_PATH_SEPARATOR;
	szName += "myactions.kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(
	       szFile,
	       __tr2qs_ctx("Choose a Filename - KVIrc", "editor"),
	       szName,
	       KVI_FILTER_SCRIPT,
	       true,
	       true,
	       true,
	       this))
		return;

	QString szCode;

	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		if(m_pTreeWidget->topLevelItem(i)->isSelected())
		{
			ActionData * a = ((ActionEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->actionData();

			KviKvsUserAction::exportToKvs(
			    szCode,
			    a->m_szName,
			    a->m_szScriptCode,
			    a->m_szVisibleName,
			    a->m_szDescription,
			    a->m_szCategory,
			    a->m_szBigIcon,
			    a->m_szSmallIcon,
			    a->m_uFlags,
			    a->m_szKeySequence);
		}
	}

	if(!KviFileUtils::writeFile(szFile, szCode))
	{
		QMessageBox::warning(this, __tr2qs_ctx("Writing to File Failed - KVIrc", "editor"), __tr2qs_ctx("Unable to write to the actions file.", "editor"), __tr2qs_ctx("OK", "editor"));
	}
}

void ActionEditor::deleteActions()
{
	KviPointerList<ActionEditorTreeWidgetItem> l;
	l.setAutoDelete(false);

	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		if(m_pTreeWidget->topLevelItem(i)->isSelected())
			l.append((ActionEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
	}

	if(l.isEmpty())
		return;

	//if(QMessageBox::question(this,__tr2qs_ctx("Confirm Actions Deletion - KVIrc","editor"),__tr2qs_ctx("Do you really want to delete the selected actions?","editor"),__tr2qs_ctx("Yes","editor"),__tr2qs_ctx("No","editor")) != 0)
	//	return;

	for(ActionEditorTreeWidgetItem * i = l.first(); i; i = l.next())
	{
		if(m_pSingleActionEditor->actionData() == i->actionData())
			m_pSingleActionEditor->setActionData(nullptr);
		delete i;
	}
}

QString ActionEditor::nameForAutomaticAction(const QString & szTemplate)
{
	QString szRet;
	QString szT = szTemplate;
	szT.replace(" ", "");
	szT.replace(".", "_");

	int i = 1;
	do
	{
		szRet = QString("%1%2").arg(szT).arg(i);
		i++;
	} while(actionExists(szRet));

	return szRet;
}

void ActionEditor::newAction()
{
	QString szName = nameForAutomaticAction(__tr2qs_ctx("My Action", "editor"));
	QString szVis = __tr2qs_ctx("My Action", "editor");
	QString szDes = __tr2qs_ctx("Put here a short description of your action", "editor");

	szVis.prepend("$tr(\"");
	szVis.append("\")");
	szDes.prepend("$tr(\"");
	szDes.append("\")");

	ActionData * ad = new ActionData(szName,
	    QString(),
	    szVis,
	    szDes,
	    KviActionManager::categoryGeneric()->name(),
	    KVI_BIGICON_USERACTION,
	    QString(),
	    0,
	    QString(),
	    nullptr);

	ActionEditorTreeWidgetItem * lvi = new ActionEditorTreeWidgetItem(m_pTreeWidget, ad);
	ad->m_pItem = lvi;
	m_pTreeWidget->setCurrentItem(lvi);
	currentItemChanged(lvi, lvi);
}

bool ActionEditor::actionExists(const QString & szName)
{
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		if(((ActionEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->actionData()->m_szName == szName)
			return true;
	}
	return false;
}

void ActionEditor::currentItemChanged(QTreeWidgetItem * i, QTreeWidgetItem *)
{
	if(m_pSingleActionEditor->actionData())
		m_pSingleActionEditor->commit();

	m_pTreeWidget->update();

	ActionEditorTreeWidgetItem * it = (ActionEditorTreeWidgetItem *)i;
	if(!it)
	{
		m_pSingleActionEditor->setActionData(nullptr);
		m_pDeleteActionsButton->setEnabled(false);
		m_pExportActionsButton->setEnabled(false);
		return;
	}
	m_pDeleteActionsButton->setEnabled(true);
	m_pExportActionsButton->setEnabled(true);

	//if(!it->isSelected())m_pTreeWidget->setSelected(it,true);

	m_pSingleActionEditor->setActionData(it->actionData());
	g_szLastEditedAction = it->actionData()->m_szName;
}

void ActionEditor::commit()
{
	m_pSingleActionEditor->commit();

	// the hard part: update the actions without destroying them...

	KviActionManager::instance()->killAllKvsUserActions();

	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		ActionEditorTreeWidgetItem * it = (ActionEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		KviKvsUserAction * a = KviKvsUserAction::createInstance( // msvc workaround
		    KviActionManager::instance(),
		    it->actionData()->m_szName,
		    it->actionData()->m_szScriptCode,
		    it->actionData()->m_szVisibleName,
		    it->actionData()->m_szDescription,
		    it->actionData()->m_szCategory,
		    it->actionData()->m_szBigIcon,
		    it->actionData()->m_szSmallIcon,
		    it->actionData()->m_uFlags,
		    it->actionData()->m_szKeySequence);

		KviActionManager::instance()->registerAction(a);
	}

	KviCustomToolBarManager::instance()->updateVisibleToolBars();
}

ActionEditorWindow::ActionEditorWindow()
    : KviWindow(KviWindow::ScriptEditor, "actioneditor", nullptr)
{
	g_pActionEditorWindow = this;
	setFixedCaption(__tr2qs_ctx("Action Editor", "editor"));

	QGridLayout * g = new QGridLayout();

	m_pEditor = new ActionEditor(this);
	g->addWidget(m_pEditor, 0, 0, 1, 4);

	QPushButton * btn = new QPushButton(__tr2qs_ctx("OK", "editor"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(okClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g->addWidget(btn, 1, 1);

	btn = new QPushButton(__tr2qs_ctx("Apply", "editor"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(applyClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g->addWidget(btn, 1, 2);

	btn = new QPushButton(__tr2qs_ctx("Cancel", "editor"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));
	g->addWidget(btn, 1, 3);

	g->setRowStretch(0, 1);
	g->setColumnStretch(0, 1);
	setLayout(g);
}

ActionEditorWindow::~ActionEditorWindow()
{
	g_pActionEditorWindow = nullptr;
}

void ActionEditorWindow::okClicked()
{
	m_pEditor->commit();
	close();
}

void ActionEditorWindow::applyClicked()
{
	m_pEditor->commit();
}

void ActionEditorWindow::cancelClicked()
{
	close();
}

QPixmap * ActionEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::ActionEditor);
}

void ActionEditorWindow::getConfigGroupName(QString & szName)
{
	szName = "actioneditor";
}

void ActionEditorWindow::saveProperties(KviConfigurationFile * cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("Splitter", m_pEditor->m_pSplitter->sizes());
}

void ActionEditorWindow::loadProperties(KviConfigurationFile * cfg)
{
	int w = width();
	KviWindow::loadProperties(cfg);
	QList<int> def;
	def.append((w * 25) / 100);
	def.append((w * 75) / 100);
	m_pEditor->m_pSplitter->setSizes(cfg->readIntListEntry("Splitter", def));
}
