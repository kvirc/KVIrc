//=============================================================================
//
//   File : aliaseditor.cpp
//   Creation date : Tue 29 Dec 2004 02:45:59 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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
#include "actioneditor.h"

#include "kvi_styled_controls.h"
#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_imagedialog.h"
#include "kvi_config.h"
#include "kvi_filedialog.h"
#include "kvi_fileutils.h"
#include "kvi_scripteditor.h"
#include "kvi_debug.h"
#include "kvi_app.h"
#include "kvi_qstring.h"
#include "kvi_kvs_aliasmanager.h"
#include "kvi_filedialog.h"
#include "kvi_actionmanager.h"
#include "kvi_action.h"
#include "kvi_kvs_useraction.h"
#include "kvi_customtoolbarmanager.h"

#include <qsplitter.h>
#include <qlayout.h>
#include "kvi_tal_vbox.h"
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qtooltip.h>
#include <qsimplerichtext.h>
#include <qpainter.h>
#include <qheader.h>
#include <qtabwidget.h>
#include <qgroupbox.h>
#include <qvaluelist.h>
#include <qlabel.h>

extern KviActionEditorWindow * g_pActionEditorWindow;

static QString g_szLastEditedAction;

#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 150
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)

KviActionEditorListViewItem::KviActionEditorListViewItem(QListView * v,KviActionData * a)
: QListViewItem(v,"")
{
	m_pActionData = a;
	m_pListView = v;
	m_pIcon = 0;
	setupForActionData();
}

KviActionEditorListViewItem::~KviActionEditorListViewItem()
{
	if(m_pIcon)delete m_pIcon;
	delete m_pActionData;
}

void KviActionEditorListViewItem::setupForActionData()
{
	QString t = "<b>" + m_pActionData->m_szName + "</b>";
	t += "<br><font color=\"#808080\" size=\"-1\">" + m_pActionData->m_szVisibleName + "</font>";
	m_szKey = m_pActionData->m_szName.upper();
	m_pText = new QSimpleRichText(t,m_pListView->font());
	if(m_pIcon)delete m_pIcon;
	QPixmap * p = g_pIconManager->getBigIcon(m_pActionData->m_szBigIcon);
	if(p)m_pIcon = new QPixmap(*p);
	else {
		p = g_pIconManager->getImage("kvi_bigicon_unknown.png");
		if(p)m_pIcon = new QPixmap(*p);
		else m_pIcon = new QPixmap(LVI_ICON_SIZE,LVI_ICON_SIZE);
	}
	setup();
}

QString KviActionEditorListViewItem::key(int,bool) const
{
	return m_szKey;
}

void KviActionEditorListViewItem::setup()
{
	QListViewItem::setup();
	int iWidth = m_pListView->visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	iWidth -= LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER;
	m_pText->setWidth(iWidth);
	int iHeight = m_pText->height() + (2 * LVI_BORDER);
	if(iHeight < (LVI_ICON_SIZE + (2 * LVI_BORDER)))iHeight = LVI_ICON_SIZE + (2 * LVI_BORDER);
	setHeight(iHeight);
}

void KviActionEditorListViewItem::paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align)
{
	p->fillRect(QRect(0,0,width,height()),isSelected() ? cg.highlight() : cg.base());
	p->drawPixmap(LVI_BORDER,LVI_BORDER,*m_pIcon);
	int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
	int www = m_pListView->visibleWidth() - (afterIcon + LVI_BORDER);
	m_pText->setWidth(www);
	m_pText->draw(p,afterIcon,LVI_BORDER,QRect(afterIcon,LVI_BORDER,www,height() - (LVI_BORDER * 2)),cg);
}


KviSingleActionEditor::KviSingleActionEditor(QWidget * par,KviActionEditor * ed)
: QWidget(par)
{
	m_pActionEditor = ed;
	m_pActionData = 0;

	QGridLayout * g = new QGridLayout(this,3,2,0,4);

	QLabel * l = new QLabel(__tr2qs("Name:"),this);
	g->addWidget(l,0,0);
	m_pNameEdit = new QLineEdit(this);
	g->addWidget(m_pNameEdit,0,1);
	QToolTip::add(m_pNameEdit,__tr2qs("Internal unique name for the action"));

	l = new QLabel(__tr2qs("Label:"),this);
	g->addWidget(l,1,0);
	m_pVisibleNameEdit = new QLineEdit(this);
	g->addWidget(m_pVisibleNameEdit,1,1);
	QToolTip::add(m_pVisibleNameEdit,__tr2qs("Visible name for this action.<br>This string will be displayed to the user so it is a good idea to use $tr() here"));

	QTabWidget * tw = new QTabWidget(this);
	g->addMultiCellWidget(tw,2,2,0,1);

	// code tab
	QWidget * tab = new QWidget(tw);
	QGridLayout * gl = new QGridLayout(tab,1,1,8,4);

	m_pScriptEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pScriptEditor,0,0);
	QToolTip::add(m_pScriptEditor,__tr2qs("Action code"));

	tw->addTab(tab,__tr2qs("Code"));

	// properties tab
	tab = new QWidget(tw);
	gl = new QGridLayout(tab,7,4,8,4);

	l = new QLabel(__tr2qs("Category:"),tab);
	gl->addWidget(l,0,0);
	m_pCategoryCombo = new QComboBox(false,tab);
	gl->addMultiCellWidget(m_pCategoryCombo,0,0,1,3);
	QToolTip::add(m_pCategoryCombo,__tr2qs("Choose the category that best fits for this action"));

	l = new QLabel(__tr2qs("Description:"),tab);
	gl->addWidget(l,1,0);
	m_pDescriptionEdit = new QLineEdit(tab);
	gl->addMultiCellWidget(m_pDescriptionEdit,1,1,1,3);
	QToolTip::add(m_pDescriptionEdit,__tr2qs("Visible short description for this action.<br>This string will be displayed to the user so it is a good idea to use $tr() here"));

	l = new QLabel(__tr2qs("Small Icon:"),tab);
	gl->addWidget(l,2,0);
	m_pSmallIconEdit = new QLineEdit(tab);
	gl->addWidget(m_pSmallIconEdit,2,1);
	m_pSmallIconButton = new QToolButton(tab);
	m_pSmallIconButton->setMinimumSize(QSize(20,20));
	connect(m_pSmallIconButton,SIGNAL(clicked()),this,SLOT(chooseSmallIcon()));
	gl->addWidget(m_pSmallIconButton,2,2);
	QString s= __tr2qs("The small icon associated to this action.<br>" \
				"It will appear at least in the popup menus when this action is inserted.<br>" \
				"It should be 16x16 pixels.");
	QToolTip::add(m_pSmallIconEdit,s);
	QToolTip::add(m_pSmallIconButton,s);

	l = new QLabel(__tr2qs("Big Icon:"),tab);
	gl->addWidget(l,3,0);
	m_pBigIconEdit = new QLineEdit(tab);
	gl->addWidget(m_pBigIconEdit,3,1);
	m_pBigIconButton = new QToolButton(tab);
	m_pBigIconButton->setMinimumSize(QSize(48,48));
	m_pBigIconButton->setUsesBigPixmap(true);
	connect(m_pBigIconButton,SIGNAL(clicked()),this,SLOT(chooseBigIcon()));
	gl->addMultiCellWidget(m_pBigIconButton,3,4,2,3);
	s = __tr2qs("The big icon associated to this action.<br>" \
				"It will appear at least in the toolbar buttons when this action is inserted.<br>" \
				"It should be 32x32 pixels.");
	QToolTip::add(m_pBigIconEdit,s);
	QToolTip::add(m_pBigIconButton,s);

	l = new QLabel(__tr2qs("Key Sequence:"),tab);
	gl->addMultiCellWidget(l,4,5,0,0);
	m_pKeySequenceEdit = new QLineEdit(tab);
	gl->addMultiCellWidget(m_pKeySequenceEdit,4,5,1,1);
	QToolTip::add(m_pKeySequenceEdit,__tr2qs("Optional keyboard sequence that will activate this action.<br>" \
						"The sequence should be expressed as a string of up to four key codes separated by commas " \
						"eventually combined with the modifiers \"Ctrl\",\"Shift\",\"Alt\" and \"Meta\".<br>" \
						"Examples of such sequences are \"Ctrl+X\", \"Ctrl+Alt+Z\", \"Ctrl+X,Ctrl+C\" ..."));

	l = new QLabel(tab);
	gl->addMultiCellWidget(l,6,6,0,3);

	gl->setColStretch(1,1);
	gl->setRowStretch(6,1);

	
	tw->addTab(tab,__tr2qs("Properties"));



	// flags tab
	tab = new QWidget(tw);
	gl = new QGridLayout(tab,12,4,8,4);


	m_pNeedsContextCheck = new KviStyledCheckBox(__tr2qs("Needs IRC Context"),tab);
	connect(m_pNeedsContextCheck,SIGNAL(toggled(bool)),this,SLOT(needsContextCheckToggled(bool)));
	QToolTip::add(m_pNeedsContextCheck,__tr2qs("Check this option if this action should be enabled only when " \
						"the active window belongs to an irc context"));
	gl->addMultiCellWidget(m_pNeedsContextCheck,0,0,0,3);

	l = new QLabel(tab);
	l->setMinimumWidth(40);
	gl->addWidget(l,1,0);

	m_pNeedsConnectionCheck = new KviStyledCheckBox(__tr2qs("Needs IRC Connection"),tab);
	connect(m_pNeedsConnectionCheck,SIGNAL(toggled(bool)),this,SLOT(needsConnectionCheckToggled(bool)));
	QToolTip::add(m_pNeedsConnectionCheck,__tr2qs("Check this option if this action should be enabled only when " \
						"the active window has an active IRC connection"));
	gl->addMultiCellWidget(m_pNeedsConnectionCheck,1,1,1,3);

	l = new QLabel(tab);
	l->setMinimumWidth(40);
	gl->addWidget(l,2,1);

	m_pEnableAtLoginCheck = new KviStyledCheckBox(__tr2qs("Enable at Login"),tab);
	QToolTip::add(m_pEnableAtLoginCheck,__tr2qs("Check this option if this action should be enabled also during " \
						"the login operations (so when the logical IRC connection hasn't been estabilished yet)"));
	gl->addMultiCellWidget(m_pEnableAtLoginCheck,2,2,2,3);

	m_pSpecificWindowsCheck = new KviStyledCheckBox(__tr2qs("Enable Only in Specified Windows"),tab);
	connect(m_pSpecificWindowsCheck,SIGNAL(toggled(bool)),this,SLOT(specificWindowsCheckToggled(bool)));
	QToolTip::add(m_pSpecificWindowsCheck,__tr2qs("Check this option if this action should be enabled only when " \
						"the active window is of a specified type"));
	gl->addMultiCellWidget(m_pSpecificWindowsCheck,3,3,0,3);

	m_pWindowConsoleCheck = new KviStyledCheckBox(__tr2qs("Enable in Console Windows"),tab);
	QToolTip::add(m_pWindowConsoleCheck,__tr2qs("Check this option if this action should be enabled only when " \
						"the active window is a console"));
	connect(m_pWindowConsoleCheck,SIGNAL(toggled(bool)),this,SLOT(channelQueryOrConsoleWindowCheckToggled(bool)));
	gl->addMultiCellWidget(m_pWindowConsoleCheck,4,4,1,3);

	m_pConsoleOnlyIfUsersSelectedCheck = new KviStyledCheckBox(__tr2qs("Only If There Are Selected Users"),tab);
	QToolTip::add(m_pConsoleOnlyIfUsersSelectedCheck,__tr2qs("This will enable the action only if there are " \
						"selected users in the active window"));
	gl->addMultiCellWidget(m_pConsoleOnlyIfUsersSelectedCheck,5,5,2,3);

	m_pWindowChannelCheck = new KviStyledCheckBox(__tr2qs("Enable in Channel Windows"),tab);
	QToolTip::add(m_pWindowChannelCheck,__tr2qs("Check this option if this action should be enabled only when " \
						"the active window is a channel"));
	connect(m_pWindowChannelCheck,SIGNAL(toggled(bool)),this,SLOT(channelQueryOrConsoleWindowCheckToggled(bool)));
	gl->addMultiCellWidget(m_pWindowChannelCheck,6,6,1,3);

	m_pChannelOnlyIfUsersSelectedCheck = new KviStyledCheckBox(__tr2qs("Only If There Are Selected Users"),tab);
	QToolTip::add(m_pChannelOnlyIfUsersSelectedCheck,__tr2qs("This will enable the action only if there are " \
						"selected users in the active window"));
	gl->addMultiCellWidget(m_pChannelOnlyIfUsersSelectedCheck,7,7,2,3);

	m_pWindowQueryCheck = new KviStyledCheckBox(__tr2qs("Enable in Query Windows"),tab);
	QToolTip::add(m_pWindowQueryCheck,__tr2qs("Check this option if this action should be enabled only when " \
						"the active window is a query"));
	connect(m_pWindowQueryCheck,SIGNAL(toggled(bool)),this,SLOT(channelQueryOrConsoleWindowCheckToggled(bool)));
	gl->addMultiCellWidget(m_pWindowQueryCheck,8,8,1,3);

	m_pQueryOnlyIfUsersSelectedCheck = new KviStyledCheckBox(__tr2qs("Only If There Are Selected Users"),tab);
	QToolTip::add(m_pQueryOnlyIfUsersSelectedCheck,__tr2qs("This will enable the action only if there are " \
						"selected users in the active window"));
	gl->addMultiCellWidget(m_pQueryOnlyIfUsersSelectedCheck,9,9,2,3);

	m_pWindowDccChatCheck = new KviStyledCheckBox(__tr2qs("Enable in DCC Chat Windows"),tab);
	QToolTip::add(m_pWindowDccChatCheck,__tr2qs("Check this option if this action should be enabled only when " \
						"the active window is a dcc chat"));
	gl->addMultiCellWidget(m_pWindowDccChatCheck,10,10,1,3);



	l = new QLabel(tab);
	gl->addMultiCellWidget(l,11,11,0,3);

	gl->setColStretch(3,1);
	gl->setRowStretch(11,1);
	
	tw->addTab(tab,__tr2qs("Flags"));



	tw->setCurrentPage(0);

	g->setRowStretch(2,1);
	g->setColStretch(1,1);


	KviDictIterator<KviActionCategory> it(*(KviActionManager::instance()->categories()));
	while(KviActionCategory * ac = it.current())
	{
		m_pCategoryCombo->insertItem(ac->visibleName() + " (" + ac->name() + ")");
		++it;
	}
}

KviSingleActionEditor::~KviSingleActionEditor()
{
}

void KviSingleActionEditor::channelQueryOrConsoleWindowCheckToggled(bool)
{
	m_pChannelOnlyIfUsersSelectedCheck->setEnabled(m_pWindowChannelCheck->isChecked());
	m_pChannelOnlyIfUsersSelectedCheck->setChecked(false);
	m_pQueryOnlyIfUsersSelectedCheck->setEnabled(m_pWindowQueryCheck->isChecked());
	m_pQueryOnlyIfUsersSelectedCheck->setChecked(false);
	m_pConsoleOnlyIfUsersSelectedCheck->setEnabled(m_pWindowConsoleCheck->isChecked());
	m_pConsoleOnlyIfUsersSelectedCheck->setChecked(false);
}

void KviSingleActionEditor::needsContextCheckToggled(bool)
{
	m_pEnableAtLoginCheck->setChecked(false);
	m_pEnableAtLoginCheck->setEnabled(false);
	m_pNeedsConnectionCheck->setChecked(false);
	m_pNeedsConnectionCheck->setEnabled(m_pNeedsContextCheck->isChecked());
}

void KviSingleActionEditor::needsConnectionCheckToggled(bool)
{
	m_pEnableAtLoginCheck->setChecked(false);
	m_pEnableAtLoginCheck->setEnabled(m_pNeedsConnectionCheck->isChecked());
}

void KviSingleActionEditor::specificWindowsCheckToggled(bool)
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

void KviSingleActionEditor::chooseSmallIcon()
{
	if(!m_pActionData)return;
	KviImageDialog * d = new KviImageDialog(this);
	int ret = d->exec();
	QString s = d->selectedImage();
	delete d;
	if(ret != QDialog::Accepted)return;
	QPixmap * p = g_pIconManager->getImage(s.utf8().data());
	if(!p)return;
	m_pSmallIconEdit->setText(s);
	m_pSmallIconButton->setPixmap(*p);
}

void KviSingleActionEditor::chooseBigIcon()
{
	if(!m_pActionData)return;
	KviImageDialog * d = new KviImageDialog(this,QString::null,KID_TYPE_ALL,KID_TYPE_FULL_PATH);
	int ret = d->exec();
	QString s = d->selectedImage();
	delete d;
	if(ret != QDialog::Accepted)return;
	QPixmap * p = g_pIconManager->getBigIcon(s.utf8().data());
	if(!p)return;
	m_pBigIconEdit->setText(s);
	m_pBigIconButton->setPixmap(*p);
}

void KviSingleActionEditor::setActionData(KviActionData * d)
{
	m_pActionData = d;
	
	if(d)
	{
		unsigned int uOldFlags = d->m_uFlags;
		d->m_uFlags = KviAction::validateFlags(d->m_uFlags);
		if(d->m_uFlags != uOldFlags)
			debug("invalid action flags in KviSingleActionEditor::setActionData(): %d fixed to %d",uOldFlags,d->m_uFlags);
	
		m_pNameEdit->setText(d->m_szName);
		m_pNameEdit->setEnabled(true);
		m_pVisibleNameEdit->setText(d->m_szVisibleName);
		m_pVisibleNameEdit->setEnabled(true);
		m_pCategoryCombo->setEnabled(true);
		int i;
		for(i=0;i<m_pCategoryCombo->count();i++)
		{
			QString t = m_pCategoryCombo->text(i);
			int idx = t.findRev('(');
			QString r = t.right(t.length() - (idx + 1));
			QString r2 = r.left(r.length() - 1);
			r2.stripWhiteSpace();
			if(r2 == d->m_szCategory)
			{
				m_pCategoryCombo->setCurrentItem(i);
				break;
			}
		}
		if(i == m_pCategoryCombo->count())
		{
			m_pCategoryCombo->setCurrentItem(0);
		}
		m_pScriptEditor->setText(d->m_szScriptCode);
		m_pScriptEditor->setEnabled(true);
		m_pKeySequenceEdit->setText(d->m_szKeySequence);
		m_pKeySequenceEdit->setEnabled(true);
		m_pDescriptionEdit->setText(d->m_szDescription);
		m_pDescriptionEdit->setEnabled(true);
		m_pSmallIconEdit->setEnabled(true);
		m_pBigIconEdit->setEnabled(true);
		QPixmap * p = g_pIconManager->getImage(d->m_szSmallIcon);
		if(p)
		{
			m_pSmallIconEdit->setText(d->m_szSmallIcon);
			m_pSmallIconButton->setPixmap(*p);
		} else {
			m_pSmallIconEdit->setText("");
			m_pSmallIconButton->setPixmap(QPixmap());
		}
		p = g_pIconManager->getImage(d->m_szBigIcon);
		if(p)
		{
			m_pBigIconEdit->setText(d->m_szBigIcon);
			m_pBigIconButton->setPixmap(*p);
		} else {
			m_pBigIconEdit->setText("");
			m_pBigIconButton->setPixmap(QPixmap());
		}
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
	} else {
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
		m_pSmallIconButton->setPixmap(QPixmap());
		m_pSmallIconButton->setEnabled(false);
		m_pBigIconButton->setPixmap(QPixmap());
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

void KviSingleActionEditor::commit()
{
	if(!m_pActionData)return;
	
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
	int idx = szCat.findRev(')');
	if(idx != -1)m_pActionData->m_szCategory = szCat.left(idx);
	else m_pActionData->m_szCategory = szCat;
	idx = m_pActionData->m_szCategory.findRev('(');
	if(idx != -1)m_pActionData->m_szCategory.remove(0,idx+1);
	m_pActionData->m_uFlags = 0;
	if(m_pNeedsContextCheck->isChecked())
	{
		m_pActionData->m_uFlags |= KviAction::NeedsContext;
		if(m_pNeedsConnectionCheck->isChecked())
		{
			m_pActionData->m_uFlags |= KviAction::NeedsConnection;
			if(m_pEnableAtLoginCheck->isChecked())m_pActionData->m_uFlags |= KviAction::EnableAtLogin;
		}
		
	}
	if(m_pSpecificWindowsCheck->isChecked())
	{
		if(m_pWindowConsoleCheck->isChecked())m_pActionData->m_uFlags |= KviAction::WindowConsole;
		if(m_pWindowChannelCheck->isChecked())m_pActionData->m_uFlags |= KviAction::WindowChannel;
		if(m_pWindowQueryCheck->isChecked())m_pActionData->m_uFlags |= KviAction::WindowQuery;
		if(m_pWindowDccChatCheck->isChecked())m_pActionData->m_uFlags |= KviAction::WindowDccChat;
		if(m_pActionData->m_uFlags & KviAction::InternalWindowMask)
		{
			if(m_pConsoleOnlyIfUsersSelectedCheck->isChecked() ||
				m_pChannelOnlyIfUsersSelectedCheck->isChecked() ||
				m_pQueryOnlyIfUsersSelectedCheck->isChecked())
			{
				m_pActionData->m_uFlags |= KviAction::WindowOnlyIfUsersSelected;
			}
		}
	}

	unsigned int uOldFlags = m_pActionData->m_uFlags;
	m_pActionData->m_uFlags = KviAction::validateFlags(m_pActionData->m_uFlags);
	if(m_pActionData->m_uFlags != uOldFlags)
		debug("invalid action flags in KviSingleActionEditor::commit(): %d fixed to %d",uOldFlags,m_pActionData->m_uFlags);
}








KviActionEditorListView::KviActionEditorListView(QWidget * pParent)
: QListView(pParent)
{
	setSelectionMode(Extended);
	int iWidth = visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	addColumn(__tr2qs("Action"),iWidth);
	setSorting(0,true);
}

KviActionEditorListView::~KviActionEditorListView()
{
}


void KviActionEditorListView::resizeEvent(QResizeEvent * e)
{
	QListView::resizeEvent(e);
	int iWidth = visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	setColumnWidth(0,iWidth);
}








KviActionEditor::KviActionEditor(QWidget * par)
: QWidget(par)
{
	
	QGridLayout * l = new QGridLayout(this,1,1,2,2);

	m_pSplitter = new QSplitter(QSplitter::Horizontal,this);

	l->addWidget(m_pSplitter,0,0);
	
	
	KviTalVBox * box = new KviTalVBox(m_pSplitter);
	m_pListView = new KviActionEditorListView(box);
	//m_pListView->setMultiSelection(false);
	m_pListView->setShowSortIndicator(true);
	m_pListView->setFocusPolicy(QWidget::StrongFocus);
	connect(m_pListView,SIGNAL(currentChanged(QListViewItem *)),this,SLOT(currentChanged(QListViewItem *)));


	m_pNewActionButton = new QPushButton(__tr2qs("New Action"),box);
	connect(m_pNewActionButton,SIGNAL(clicked()),this,SLOT(newAction()));

	m_pDeleteActionsButton = new QPushButton(__tr2qs("Delete Actions"),box);
	connect(m_pDeleteActionsButton,SIGNAL(clicked()),this,SLOT(deleteActions()));

	m_pExportActionsButton = new QPushButton(__tr2qs("Export Actions..."),box);
	connect(m_pExportActionsButton,SIGNAL(clicked()),this,SLOT(exportActions()));

	box->setSpacing(1);

	m_pSingleActionEditor = new KviSingleActionEditor(m_pSplitter,this);

	KviActionEditorListViewItem * last = 0;
	KviActionEditorListViewItem * first = 0;

	KviDictIterator<KviAction> it(*(KviActionManager::instance()->actions()));
	while(KviAction * a = it.current())
	{
		if(a->isKviUserActionNeverOverrideThis())
		{
			KviActionData * ad = new KviActionData(a->name(),
						((KviKvsUserAction *)a)->scriptCode(),
						((KviKvsUserAction *)a)->visibleNameCode(),
						((KviKvsUserAction *)a)->descriptionCode(),
						a->category() ? a->category()->name() : KviActionManager::categoryGeneric()->name(),
						a->bigIconString(),
						((KviKvsUserAction *)a)->smallIconString(),
						a->flags(),
						a->keySequence(),
						0);
			KviActionEditorListViewItem * lvi = new KviActionEditorListViewItem(m_pListView,ad);
			ad->m_pItem = lvi;
			if(ad->m_szName == g_szLastEditedAction)
				last = lvi;
			if(!first)
				first = lvi;
		}
		++it;
	}

	if(!last)last = first; // try to sleect the first one then

	if(last)
	{
		m_pListView->setCurrentItem(last);
		currentChanged(last);
	} else {
		currentChanged(0);
	}

}

KviActionEditor::~KviActionEditor()
{
}

void KviActionEditor::exportActions()
{
	QString szName = QDir::homeDirPath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += "myactions.kvs";
	
	QString szFile;
	
	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose a Filename - KVIrc"),szName,QString::null,true,true,true))return;

	QString szCode;

	KviActionEditorListViewItem * it = (KviActionEditorListViewItem *)m_pListView->firstChild();
	while(it)
	{
		if(it->isSelected())
		{
			KviActionData * a = it->actionData();
			
			KviKvsUserAction::exportToKvs(szCode,
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

		it = (KviActionEditorListViewItem *)(it->nextSibling());
	}

	if(!KviFileUtils::writeFile(szFile,szCode))
	{
		QMessageBox::warning(this,__tr2qs("Write Failed - KVIrc"),__tr2qs("Unable to write to the actions file."),__tr2qs("OK"));
	}
}

void KviActionEditor::deleteActions()
{
	KviPtrList<KviActionEditorListViewItem> l;
	l.setAutoDelete(false);

	KviActionEditorListViewItem * it = (KviActionEditorListViewItem *)m_pListView->firstChild();
	while(it)
	{
		if(it->isSelected())
			l.append(it);

		it = (KviActionEditorListViewItem *)(it->nextSibling());
	}

	if(l.isEmpty())return;

	//if(QMessageBox::question(this,__tr2qsf("Confirm Deletion"),__tr2qsf("Do you really want to delete the selected actions ?"),__tr2qsf("Yes"),__tr2qsf("No")) != 0)
	//	return;
	
	for(KviActionEditorListViewItem * i = l.first();i;i = l.next())
	{
		if(m_pSingleActionEditor->actionData() == i->actionData())
			m_pSingleActionEditor->setActionData(0);
		delete i;
	}
}


QString KviActionEditor::nameForAutomaticAction(const QString &szTemplate)
{
	QString ret;
	QString szT = szTemplate;
	szT.replace(" ","");
	szT.replace(".","_");

	int i = 1;
	do {
		KviQString::sprintf(ret,"%Q%d",&szT,i);
		i++;
	} while(actionExists(ret));

	return ret;
}

void KviActionEditor::newAction()
{
	QString szName = nameForAutomaticAction(__tr2qs("My Action"));
	QString szVis = __tr2qs("My Action");
	QString szDes = __tr2qs("Put here a short description of your action");

	szVis.prepend("$tr(\"");
	szVis.append("\")");
	szDes.prepend("$tr(\"");
	szDes.append("\")");

	KviActionData * ad = new KviActionData(szName,
						QString::null,
						szVis,
						szDes,
						KviActionManager::categoryGeneric()->name(),
						QString::null,
						QString::null,
						0,
						QString::null,
						0);

	KviActionEditorListViewItem * lvi = new KviActionEditorListViewItem(m_pListView,ad);
	ad->m_pItem = lvi;
	m_pListView->setCurrentItem(lvi);
	currentChanged(lvi);
}

bool KviActionEditor::actionExists(const QString &szName)
{
	KviActionEditorListViewItem * it = (KviActionEditorListViewItem *)m_pListView->firstChild();
	while(it)
	{
		if(it->actionData()->m_szName == szName)return true;
		it = (KviActionEditorListViewItem *)(it->nextSibling());
	}
	return false;
}


void KviActionEditor::currentChanged(QListViewItem * i)
{
	if(m_pSingleActionEditor->actionData())
	{
		m_pSingleActionEditor->commit();
		m_pSingleActionEditor->actionData()->m_pItem->setupForActionData();
	}

	m_pListView->update();

	KviActionEditorListViewItem * it = (KviActionEditorListViewItem *)i;
	if(!it)
	{
		m_pSingleActionEditor->setActionData(0);
		m_pDeleteActionsButton->setEnabled(false);
		m_pExportActionsButton->setEnabled(false);
		return;
	}
	m_pDeleteActionsButton->setEnabled(true);
	m_pExportActionsButton->setEnabled(true);

	if(!it->isSelected())m_pListView->setSelected(it,true);

	m_pSingleActionEditor->setActionData(it->actionData());
	g_szLastEditedAction = it->actionData()->m_szName;
}

void KviActionEditor::commit()
{
	m_pSingleActionEditor->commit();

	// the hard part: update the actions without destroying them...

	KviActionManager::instance()->killAllKvsUserActions();

	KviActionEditorListViewItem * it = (KviActionEditorListViewItem *)m_pListView->firstChild();
	while(it)
	{
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

		it = (KviActionEditorListViewItem *)(it->nextSibling());
	}

	KviCustomToolBarManager::instance()->updateVisibleToolBars();
}



KviActionEditorWindow::KviActionEditorWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"actioneditor",0)
{
	g_pActionEditorWindow = this;
	setFixedCaption(__tr2qs("Action Editor"));

	m_pEditor = new KviActionEditor(this);

	m_pBase = new QWidget(this);
	QGridLayout * g = new QGridLayout(m_pBase,1,4,4,4);

	QPushButton * btn = new QPushButton(__tr2qs("OK"),m_pBase);
	btn->setMinimumWidth(80);
	connect(btn,SIGNAL(clicked()),this,SLOT(okClicked()));
	btn->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,0,1);

	btn = new QPushButton(__tr2qs("Apply"),m_pBase);
	btn->setMinimumWidth(80);
	connect(btn,SIGNAL(clicked()),this,SLOT(applyClicked()));
	btn->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,0,2);

	btn = new QPushButton(__tr2qs("Cancel"),m_pBase);
	btn->setMinimumWidth(80);
	connect(btn,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	btn->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	g->addWidget(btn,0,3);

	g->setColStretch(0,1);

}

KviActionEditorWindow::~KviActionEditorWindow()
{
	g_pActionEditorWindow = 0;
}

void KviActionEditorWindow::okClicked()
{
	m_pEditor->commit();
	close();
}

void KviActionEditorWindow::applyClicked()
{
	m_pEditor->commit();
}

void KviActionEditorWindow::cancelClicked()
{
	close();
}


QPixmap * KviActionEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_SCRIPTACTION);
}

void KviActionEditorWindow::resizeEvent(QResizeEvent *e)
{
	int hght = m_pBase->sizeHint().height();
	m_pEditor->setGeometry(0,0,width(),height()- hght);
	m_pBase->setGeometry(0,height() - hght,width(),hght);
}


void KviActionEditorWindow::getConfigGroupName(KviStr &szName)
{
	szName = "actioneditor";
}


void KviActionEditorWindow::saveProperties(KviConfig *cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("Splitter",m_pEditor->m_pSplitter->sizes());
}

void KviActionEditorWindow::loadProperties(KviConfig *cfg)
{
	int w = width();
	KviWindow::loadProperties(cfg);
	QValueList<int> def;
	def.append((w * 25) / 100);
	def.append((w * 75) / 100);
	m_pEditor->m_pSplitter->setSizes(cfg->readIntListEntry("Splitter",def));
}
