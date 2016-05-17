//=============================================================================
//
//   File : KviIrcView_tools.cpp
//   Creation date : Sat Oct 9 2004 16:03:01 by Szymon Stefanek
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

#include "KviIrcView_tools.h"
#include "KviIrcView.h"
#include "KviIconManager.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviMemory.h"
#include "KviMessageBox.h"
#include "KviFileDialog.h"
#include "KviApplication.h"
#include "KviThemedLineEdit.h"
#include "KviShortcut.h"
#include "KviWindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCursor>
#include <QEvent>
#include <QMouseEvent>
#include <QHeaderView>
#include <QScrollBar>
#include <QComboBox>
#include <QIcon>
#include <QMenu>

//
// Tool widget implementation
//

KviIrcMessageCheckListItem::KviIrcMessageCheckListItem(QTreeWidget * par, KviIrcViewToolWidget * w, int id)
    : QTreeWidgetItem(par)
{
	m_iId = id;
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	setCheckState(0, Qt::Checked);
	setIcon(0, *(g_pIconManager->getSmallIcon(KVI_OPTION_MSGTYPE(id).pixId())));
	setText(0, __tr2qs_no_xgettext(KVI_OPTION_MSGTYPE(id).type()));
	m_pToolWidget = w;
}

KviIrcMessageCheckListItem::~KviIrcMessageCheckListItem()
    = default;

KviIrcViewToolWidget::KviIrcViewToolWidget(KviIrcView * pParent)
    : QWidget(pParent)
{
	m_pIrcView = pParent;
	setAutoFillBackground(true);
	setContentsMargins(0, 0, 0, 0);

	QHBoxLayout * pLayout = new QHBoxLayout(this);
	pLayout->setMargin(2);
	pLayout->setSpacing(2);

	QPushButton * pButton = new QPushButton(QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Close)), QString(), this);
	pButton->setFixedSize(16, 16);
	pButton->setFlat(true);
	connect(pButton, SIGNAL(clicked()), m_pIrcView, SLOT(toggleToolWidget()));
	pLayout->addWidget(pButton);

	m_pStringToFind = new KviThemedLineEdit(this, m_pIrcView->parentKviWindow(), "search_lineedit");
	pLayout->addWidget(m_pStringToFind);
	connect(m_pStringToFind, SIGNAL(returnPressed()), this, SLOT(findNext()));
	connect(m_pStringToFind, SIGNAL(textChanged(QString)), this, SLOT(findNextHelper(QString)));

	pButton = new QPushButton(__tr2qs("&Next"), this);
	pButton->setDefault(true);
	connect(pButton, SIGNAL(clicked()), this, SLOT(findNext()));
	pLayout->addWidget(pButton);

	pButton = new QPushButton(__tr2qs("&Previous"), this);
	connect(pButton, SIGNAL(clicked()), this, SLOT(findPrev()));
	pLayout->addWidget(pButton);

	m_pOptionsButton = new QPushButton(this);
	m_pOptionsButton->setText(__tr2qs("&Options"));
	pLayout->addWidget(m_pOptionsButton);

	m_pOptionsWidget = new QMenu(m_pOptionsButton);
	QGridLayout * pOptionsLayout = new QGridLayout(m_pOptionsWidget);
	pOptionsLayout->setSpacing(2);

	connect(m_pOptionsButton, SIGNAL(clicked()), this, SLOT(toggleOptions()));

	// Filter tab

	QLabel * pLabel = new QLabel(__tr2qs("Message Types"), m_pOptionsWidget); //groupbox title
	pOptionsLayout->addWidget(pLabel, 0, 0, 1, 2);

	m_pFilterView = new QTreeWidget(m_pOptionsWidget);
	m_pFilterView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pFilterView->setRootIsDecorated(false);
	//FIXME hardcoded size sucks
	m_pFilterView->setMinimumSize(QSize(200, 150));
	m_pFilterView->setColumnCount(1);
	m_pFilterView->header()->hide();
	pOptionsLayout->addWidget(m_pFilterView, 1, 0, 4, 2);

	m_pFilterItems = (KviIrcMessageCheckListItem **)KviMemory::allocate(KVI_NUM_MSGTYPE_OPTIONS * sizeof(KviIrcMessageCheckListItem *));

	for(int i = 0; i < KVI_NUM_MSGTYPE_OPTIONS; i++)
	{
		m_pFilterItems[i] = new KviIrcMessageCheckListItem(m_pFilterView, this, i);
	}

	pButton = new QPushButton(__tr2qs("Set &All"), m_pOptionsWidget);
	connect(pButton, SIGNAL(clicked()), this, SLOT(filterEnableAll()));
	pOptionsLayout->addWidget(pButton, 6, 0);

	pButton = new QPushButton(__tr2qs("Set &None"), m_pOptionsWidget);
	connect(pButton, SIGNAL(clicked()), this, SLOT(filterEnableNone()));
	pOptionsLayout->addWidget(pButton, 6, 1);

	pButton = new QPushButton(__tr2qs("&Load from..."), m_pOptionsWidget);
	connect(pButton, SIGNAL(clicked()), this, SLOT(filterLoad()));
	pOptionsLayout->addWidget(pButton, 7, 0);

	pButton = new QPushButton(__tr2qs("&Save As..."), m_pOptionsWidget);
	connect(pButton, SIGNAL(clicked()), this, SLOT(filterSave()));
	pOptionsLayout->addWidget(pButton, 7, 1);

	pLabel = new QLabel(__tr2qs("Pattern:"), m_pOptionsWidget);
	pOptionsLayout->addWidget(pLabel, 8, 0);
	m_pSearchMode = new QComboBox(m_pOptionsWidget);
	m_pSearchMode->insertItem(PlainText, __tr2qs("Plain Text"));
	m_pSearchMode->insertItem(Wildcards, __tr2qs("Wildcards"));
	m_pSearchMode->insertItem(RegExp, __tr2qs("RegExp"));
	pOptionsLayout->addWidget(m_pSearchMode, 8, 1);

	pLabel = new QLabel(__tr2qs("Match:"), m_pOptionsWidget);
	pOptionsLayout->addWidget(pLabel, 9, 0);

	m_pCaseSensitive = new QCheckBox(__tr2qs("&Case sensitive"), m_pOptionsWidget);
	pOptionsLayout->addWidget(m_pCaseSensitive, 9, 1);

	// Focusing the 'string to find' widget has been moved to the toggle function so that it happens whenever the widget is shown

	KviShortcut::create(Qt::Key_Escape, m_pIrcView, SLOT(toggleToolWidget()), nullptr, Qt::WidgetWithChildrenShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_SEARCH, m_pIrcView, SLOT(toggleToolWidget()), nullptr, Qt::WidgetWithChildrenShortcut);
}

KviIrcViewToolWidget::~KviIrcViewToolWidget()
{
	if(m_pIrcView)
		if(m_pIrcView->parentKviWindow())
			if(m_pIrcView->parentKviWindow()->input())
				m_pIrcView->parentKviWindow()->input()->setFocus();
}

void KviIrcViewToolWidget::toggleOptions()
{
	if(m_pOptionsWidget->isVisible())
	{
		m_pOptionsWidget->hide();
	}
	else
	{
		m_pOptionsWidget->adjustSize();
		QPoint p = QPoint(
		    m_pOptionsButton->pos().x() + m_pOptionsButton->width() - m_pOptionsWidget->width(),
		    m_pOptionsButton->pos().y() - m_pOptionsWidget->height());
		m_pOptionsWidget->popup(mapToGlobal(p));
	}
}

void KviIrcViewToolWidget::filterEnableAll()
{
	for(int i = 0; i < KVI_NUM_MSGTYPE_OPTIONS; i++)
		m_pFilterItems[i]->setOn(true);
}

void KviIrcViewToolWidget::filterEnableNone()
{
	for(int i = 0; i < KVI_NUM_MSGTYPE_OPTIONS; i++)
		m_pFilterItems[i]->setOn(false);
}

void KviIrcViewToolWidget::filterLoad()
{
	QString szFile;
	QString szInit;
	g_pApp->getLocalKvircDirectory(szInit, KviApplication::Filters);

	if(KviFileDialog::askForOpenFileName(szFile, __tr2qs("Select a File - KVIrc"), szInit, QString(), false, true, this)) // This is the window title
	{
		QFile f(szFile);
		if(f.open(QIODevice::ReadOnly))
		{
			char cBuffer[KVI_NUM_MSGTYPE_OPTIONS];
			KviMemory::set(cBuffer, 0, KVI_NUM_MSGTYPE_OPTIONS);
			f.read(cBuffer, KVI_NUM_MSGTYPE_OPTIONS);
			f.close();
			for(int i = 0; i < KVI_NUM_MSGTYPE_OPTIONS; i++)
			{
				m_pFilterItems[i]->setOn(cBuffer[i]);
			}
			forceRepaint();
		}
		else
		{
			KviMessageBox::warning(__tr2qs("Can't open the filter file %s for reading."), &szFile);
		}
	}
}

void KviIrcViewToolWidget::filterSave()
{
	QString szFile;
	QString szInit;
	g_pApp->getLocalKvircDirectory(szInit, KviApplication::Filters, "filter.kvf");
	if(KviFileDialog::askForSaveFileName(szFile, __tr2qs("Enter a Filename - KVIrc"), szInit, QString(), false, false, true, this)) // this is the window title
	{
		QFile f(szFile);
		if(f.open(QIODevice::WriteOnly))
		{
			char cBuffer[KVI_NUM_MSGTYPE_OPTIONS];
			for(int i = 0; i < KVI_NUM_MSGTYPE_OPTIONS; i++)
			{
				cBuffer[i] = messageEnabled(i) ? 1 : 0;
			}
			if(f.write(cBuffer, KVI_NUM_MSGTYPE_OPTIONS) < KVI_NUM_MSGTYPE_OPTIONS)
				KviMessageBox::warning(__tr2qs("Failed to write the filter file %Q (IO Error)"), &szFile);
			f.close();
		}
		else
			KviMessageBox::warning(__tr2qs("Can't open the filter file %Q for writing"), &szFile);
	}
}

void KviIrcViewToolWidget::focusStringToFind()
{
#ifndef COMPILE_ON_MAC
	m_pStringToFind->setFocus(); // this makes MacOSX version of Qt go nuts and crash
#endif                           //!COMPILE_ON_MAC
}

void KviIrcViewToolWidget::forceRepaint()
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_pIrcView->repaint();
#else
	m_pIrcView->paintEvent(nullptr);
#endif
}

void KviIrcViewToolWidget::setFindResult(const QString &)
{
	// 	m_pFindResult->setText(szText);
}

void KviIrcViewToolWidget::findPrev()
{
	SearchMode eMode = (SearchMode)m_pSearchMode->currentIndex();
	m_pIrcView->findPrev(m_pStringToFind->text(), m_pCaseSensitive->isChecked(), eMode != PlainText, eMode == RegExp);
}

void KviIrcViewToolWidget::findNextHelper(QString)
{
	m_pIrcView->m_pCursorLine = m_pIrcView->m_pFirstLine;
	findNext();
}

void KviIrcViewToolWidget::findNext()
{
	SearchMode eMode = (SearchMode)m_pSearchMode->currentIndex();
	m_pIrcView->findNext(m_pStringToFind->text(), m_pCaseSensitive->isChecked(), eMode != PlainText, eMode == RegExp);
}

//
// Link tip label implementation
//

KviIrcViewToolTip::KviIrcViewToolTip(KviIrcView * pView)
    : KviTalToolTip(pView)
{
	m_pView = pView;
}

KviIrcViewToolTip::~KviIrcViewToolTip()
    = default;

void KviIrcViewToolTip::maybeTip(const QPoint & pnt)
{
	m_pView->maybeTip(pnt);
}
