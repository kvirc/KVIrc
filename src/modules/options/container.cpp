//=============================================================================
//
//   File : container.cpp
//   Creation date : Wed Nov 21 17:09:49 2001 GMT by Szymon Stefanek
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

#include "container.h"
#include "instances.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"
#include "kvi_tal_tooltip.h"

#include <QLayout>
#include <QPushButton>
#include <QDesktopWidget>
#include <QEvent>
#include <QGridLayout>
#include <QCloseEvent>
#include <QIcon>


extern KviOptionsInstanceManager * g_pOptionsInstanceManager;

KviOptionsWidgetContainer::KviOptionsWidgetContainer(QWidget * par,bool bModal)
: QDialog(par)
{
	m_pLayout = new QGridLayout(this);

	setObjectName("container");
	m_pOptionsWidget = 0;
	setModal(bModal);
}

KviOptionsWidgetContainer::~KviOptionsWidgetContainer()
{
	if(m_pOptionsWidget)delete m_pOptionsWidget;
}

void KviOptionsWidgetContainer::setLeftCornerWidget(QWidget * pWidget)
{
	if(!pWidget)
		return;
	m_pLayout->addWidget(pWidget,1,0);
}


void KviOptionsWidgetContainer::setup(KviOptionsWidget * w)
{
	m_pLayout->addWidget(w,0,0,1,3);
	//g->addMultiCellWidget(w,0,0,0,2);

	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK","options"),this);
	KviTalToolTip::add(b,__tr2qs_ctx("Close this dialog, accepting all changes.","options"));
	//b->setMinimumWidth(m_pCancel->sizeHint().width());
	m_pLayout->addWidget(b,1,1);
	b->setDefault(true);
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));

	m_pCancel = new QPushButton(__tr2qs_ctx("Cancel","options"),this);
	KviTalToolTip::add(m_pCancel,__tr2qs_ctx("Close this dialog, discarding all changes.","options"));
	m_pLayout->addWidget(m_pCancel,1,2);
	connect(m_pCancel,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	m_pCancel->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));


	m_pLayout->setRowStretch(0,1);
	m_pLayout->setColumnStretch(0,1);

	KviOptionsWidgetInstanceEntry * e = g_pOptionsInstanceManager->findInstanceEntry(w->metaObject()->className());
	if(e)
	{
		setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(e->iIcon))));
		setWindowTitle(e->szName);
	}

	m_pOptionsWidget = w;
}

void KviOptionsWidgetContainer::closeEvent(QCloseEvent *e)
{
	e->ignore();
	cancelClicked();
}

void KviOptionsWidgetContainer::showEvent(QShowEvent *e)
{
	if(parent() == 0)
	{
		QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
		move((rect.width() - width())/2,(rect.height() - height())/2);
	}
	QWidget::showEvent(e);
	m_pCancel->setFocus();
}

void KviOptionsWidgetContainer::reject()
{
	cancelClicked();
}

void KviOptionsWidgetContainer::okClicked()
{
	if(m_pOptionsWidget)m_pOptionsWidget->commit();
	g_pApp->saveOptions();
	delete this;
}

void KviOptionsWidgetContainer::cancelClicked()
{
	deleteLater();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_container.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
