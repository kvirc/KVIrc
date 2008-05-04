//
//   File : container.cpp
//   Creation date : Wed Nov 21 17:09:49 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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
#include <QCloseEvent>


extern KviOptionsInstanceManager * g_pOptionsInstanceManager;

KviOptionsWidgetContainer::KviOptionsWidgetContainer(QWidget * par,bool bModal)
: QDialog(par,"container","options")
{
	m_pOptionsWidget = 0;
	setModal(bModal);
}

KviOptionsWidgetContainer::~KviOptionsWidgetContainer()
{
	if(m_pOptionsWidget)delete m_pOptionsWidget;
}

void KviOptionsWidgetContainer::setup(KviOptionsWidget * w)
{
	QGridLayout * g = new QGridLayout(this);

	g->addMultiCellWidget(w,0,0,0,2);


	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK","options"),this);
	KviTalToolTip::add(b,__tr2qs_ctx("Close this dialog, accepting all changes.","options"));
	//b->setMinimumWidth(m_pCancel->sizeHint().width());
	g->addWidget(b,1,1);
	b->setDefault(true);
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));

	m_pCancel = new QPushButton(__tr2qs_ctx("Cancel","options"),this);
	KviTalToolTip::add(m_pCancel,__tr2qs_ctx("Close this dialog, discarding all changes.","options"));
	g->addWidget(m_pCancel,1,2);
	connect(m_pCancel,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	m_pCancel->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));


	g->setRowStretch(0,1);
	g->setColumnStretch(0,1);

	KviOptionsWidgetInstanceEntry * e = g_pOptionsInstanceManager->findInstanceEntry(w->className());
	if(e)
	{
		//KviStr caption(KviStr::Format,"%s - KVIrc",e->szName);
		setIcon(*(g_pIconManager->getSmallIcon(e->iIcon)));
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
		move((g_pApp->desktop()->width() - width()) / 2,
			(g_pApp->desktop()->height() - height()) / 2);
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
