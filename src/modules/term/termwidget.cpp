//
//   File : termwidget.cpp
//   Creation date : Thu Aug 10 2000 17:42:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2005 Szymon Stefanek (pragma at kvirc dot net)
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

#include "termwidget.h"
#include "termwindow.h"

#include "kvi_module.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"

#include <qlabel.h>
#include "kvi_list.h"
#include <qtooltip.h>
#include <qtimer.h>

#ifdef COMPILE_KDE_SUPPORT

#include "klibloader.h"
#include "kparts/part.h"
#include "kparts/factory.h"

extern KviModule            * g_pTermModule;
extern KviPtrList<KviTermWidget> * g_pTermWidgetList;
extern KviPtrList<KviTermWindow> * g_pTermWindowList;
extern KviStr                 g_szKonsoleLibraryName;

KviTermWidget::KviTermWidget(QWidget * par,KviFrame * lpFrm,bool bIsStandalone)
: QFrame(par,"term_widget")
{
	if(bIsStandalone)g_pTermWidgetList->append(this);
	m_bIsStandalone = bIsStandalone;

	m_pKonsolePart = 0;
	m_pKonsoleWidget = 0;

	if(bIsStandalone)
	{
		m_pHBox = new KviTalHBox(this);
		m_pTitleLabel = new QLabel(__tr2qs("Terminal emulator"),m_pHBox);
		m_pTitleLabel->setFrameStyle(QFrame::Raised | QFrame::WinPanel);
		m_pCloseButton = new QPushButton("",m_pHBox);
		m_pCloseButton->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLOSE)));
		QToolTip::add(m_pCloseButton,__tr2qs("Close this window"));
		m_pHBox->setStretchFactor(m_pTitleLabel,2);
		connect(m_pCloseButton,SIGNAL(clicked()),this,SLOT(closeClicked()));
	} else {
		m_pHBox = 0;
		m_pTitleLabel = 0;
		m_pCloseButton = 0;
	}

	setFrameStyle(QFrame::Sunken | QFrame::Panel);

	KParts::Factory * pKonsoleFactory = static_cast<KParts::Factory *>(
			KLibLoader::self()->factory(g_szKonsoleLibraryName.ptr()));

	if(pKonsoleFactory)
	{
//		debug("FACTORY %d",pKonsoleFactory);
		m_pKonsolePart = static_cast<KParts::Part *>(pKonsoleFactory->createPart(
				this,"terminal emulator",this,"the konsole part"));

		if(m_pKonsolePart)
		{
//			debug("PART %d",m_pKonsolePart);
			m_pKonsoleWidget = m_pKonsolePart->widget();
			connect(m_pKonsoleWidget,SIGNAL(destroyed()),this,SLOT(konsoleDestroyed()));
//			debug("Widget %d",m_pKonsoleWidget);
		} else {
			m_pKonsoleWidget = new QLabel(this,
				__tr2qs("Can't create the terminal emulation part"));
		}

	} else {
		m_pKonsoleWidget = new QLabel(this,
			__tr2qs("Can't retrieve the terminal emulation factory"));
	}
}

KviTermWidget::~KviTermWidget()
{
	if(m_pKonsoleWidget)
		disconnect(m_pKonsoleWidget,SIGNAL(destroyed()),this,SLOT(konsoleDestroyed()));

	if(m_bIsStandalone)g_pTermWidgetList->removeRef(this);
	if(g_pTermWindowList->isEmpty() && g_pTermWidgetList->isEmpty())g_pTermModule->unlock();

//	debug("DELETING KONSOLE WIDGET");
//	if(m_pKonsoleWidget)delete m_pKonsoleWidget; <--// Qt will delete it
//	debug("DELETING KONSOLE PART");
//	if(m_pKonsolePart)delete m_pKonsolePart; <--// the part will delete self when the widget will die
//	debug("KONSOLE PART DELETED");
}

void KviTermWidget::resizeEvent(QResizeEvent *e)
{
	int hght = 0;
	if(m_bIsStandalone)
	{
		hght = m_pCloseButton->sizeHint().height();
		m_pHBox->setGeometry(1,1,width() - 2,hght + 1);

	}
	if(m_pKonsoleWidget)m_pKonsoleWidget->setGeometry(1,hght + 1,width() - 2,height() - (hght + 2));
}

void KviTermWidget::closeClicked()
{
	// this is called only in standalone  mode
	delete this;
}

void KviTermWidget::konsoleDestroyed()
{
	m_pKonsoleWidget = 0;
	m_pKonsolePart = 0;
	hide();
	QTimer::singleShot(0,this,SLOT(autoClose()));
}

void KviTermWidget::autoClose()
{
	if(m_bIsStandalone)delete this;
	else ((KviWindow *)parent())->close();
}

void KviTermWidget::changeTitle(int i,const QString& str)
{
	if(m_bIsStandalone)m_pTitleLabel->setText(str);
}

void KviTermWidget::notifySize(int,int)
{
}

void KviTermWidget::changeColumns(int)
{
}

QSize KviTermWidget::sizeHint() const
{
	int hght = 0;
	int wdth = 0;
	if(m_pKonsoleWidget)
	{
		hght += m_pKonsoleWidget->sizeHint().height();
		wdth = m_pKonsoleWidget->sizeHint().width();
	}
	if(m_pCloseButton)
	{
		hght += m_pCloseButton->sizeHint().height();
	}
	
	return QSize(wdth + 2,hght + 2);
}


#endif //COMPILE_KDE_SUPPORT
