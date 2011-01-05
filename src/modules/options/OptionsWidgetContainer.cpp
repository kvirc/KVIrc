//=============================================================================
//
//   File : OptionsWidgetContainer.cpp
//   Creation date : Wed Nov 21 17:09:49 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidgetContainer.h"
#include "OptionsInstanceManager.h"

#include "KviLocale.h"
#include "KviIconManager.h"
#include "KviApplication.h"
#include "KviTalToolTip.h"

#include <QLayout>
#include <QPushButton>
#include <QDesktopWidget>
#include <QEvent>
#include <QGridLayout>
#include <QCloseEvent>
#include <QIcon>


extern OptionsInstanceManager * g_pOptionsInstanceManager;

OptionsWidgetContainer::OptionsWidgetContainer(QWidget * par,bool bModal)
: QWidget(par)
{
	m_pLayout = new QGridLayout(this);

	setObjectName("container");

	m_pOptionsWidget = NULL;

	if(bModal)
		setWindowModality(Qt::ApplicationModal);
}

OptionsWidgetContainer::~OptionsWidgetContainer()
{
	if(m_pOptionsWidget)
		delete m_pOptionsWidget;
}

void OptionsWidgetContainer::setLeftCornerWidget(QWidget * pWidget)
{
	if(!pWidget)
		return;
	m_pLayout->addWidget(pWidget,1,0);
}

void OptionsWidgetContainer::optionsWidgetDestroyed()
{
	m_pOptionsWidget = NULL;
}

void OptionsWidgetContainer::childEvent(QChildEvent * e)
{
	// This handles reparents which may happen when
	// options.edit and options.dialog are used without closing
	// the corresponding widgets.
	if(m_pOptionsWidget && (e->type() == QEvent::ChildRemoved))
	{
		if(e->child() == static_cast<QObject *>(m_pOptionsWidget))
		{
			QObject::disconnect(m_pOptionsWidget,SIGNAL(destroyed()),this,SLOT(optionsWidgetDestroyed()));
			m_pOptionsWidget = NULL;
		}
	}
	
	QWidget::childEvent(e);
}

void OptionsWidgetContainer::setup(KviOptionsWidget * w)
{
	if(m_pOptionsWidget)
		QObject::disconnect(m_pOptionsWidget,SIGNAL(destroyed()),this,SLOT(optionsWidgetDestroyed()));

	m_pLayout->addWidget(w,0,0,1,3);
	//g->addMultiCellWidget(w,0,0,0,2);

	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK","options"),this);
	KviTalToolTip::add(b,__tr2qs_ctx("Close this dialog, accepting all changes.","options"));
	//b->setMinimumWidth(m_pCancel->sizeHint().width());
	m_pLayout->addWidget(b,1,1);
	b->setDefault(true);
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));

	m_pCancel = new QPushButton(__tr2qs_ctx("Cancel","options"),this);
	KviTalToolTip::add(m_pCancel,__tr2qs_ctx("Close this dialog, discarding all changes.","options"));
	m_pLayout->addWidget(m_pCancel,1,2);
	connect(m_pCancel,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	m_pCancel->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));


	m_pLayout->setRowStretch(0,1);
	m_pLayout->setColumnStretch(0,1);

	OptionsWidgetInstanceEntry * e = g_pOptionsInstanceManager->findInstanceEntry(w->metaObject()->className());
	if(e)
	{
		setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(e->eIcon))));
		setWindowTitle(e->szName);
	}

	m_pOptionsWidget = w;

	if(m_pOptionsWidget)
		QObject::connect(m_pOptionsWidget,SIGNAL(destroyed()),this,SLOT(optionsWidgetDestroyed()));
}

void OptionsWidgetContainer::closeEvent(QCloseEvent *e)
{
	e->ignore();
	cancelClicked();
}

void OptionsWidgetContainer::showEvent(QShowEvent *e)
{
	if(parent() == 0)
	{
		QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
		move((rect.width() - width())/2,(rect.height() - height())/2);
	}
	QWidget::showEvent(e);
	m_pCancel->setFocus();
}

void OptionsWidgetContainer::reject()
{
	cancelClicked();
}

void OptionsWidgetContainer::okClicked()
{
	if(m_pOptionsWidget)
		m_pOptionsWidget->commit();
	g_pApp->saveOptions();
	delete this;
}

void OptionsWidgetContainer::cancelClicked()
{
	deleteLater();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_OptionsWidgetContainer.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
