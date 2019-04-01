//=============================================================================
//
//   File : TermWidget.cpp
//   Creation date : Thu Aug 10 2000 17:42:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "TermWidget.h"
#include "TermWindow.h"

#include "KviModule.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviLocale.h"

#include <QLabel>
#include <QToolTip>
#include <QTimer>

#ifdef COMPILE_KDE4_SUPPORT

#include "klibloader.h"
#include "kparts/part.h"
#include "kparts/factory.h"
#include <kde_terminal_interface.h>

#include <unordered_set>

extern KviModule * g_pTermModule;
extern std::unordered_set<TermWidget *> g_pTermWidgetList;
extern std::unordered_set<TermWindow *> g_pTermWindowList;

TermWidget::TermWidget(QWidget * par, bool bIsStandalone)
    : QFrame(par)
{
	setObjectName("term_widget");
	if(bIsStandalone)
		g_pTermWidgetList.insert(this);
	m_bIsStandalone = bIsStandalone;

	m_pKonsolePart = nullptr;
	m_pKonsoleWidget = nullptr;

	if(bIsStandalone)
	{
		m_pHBox = new KviTalHBox(this);
		m_pTitleLabel = new QLabel(__tr2qs("Terminal Emulator"), m_pHBox);
		m_pTitleLabel->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
		m_pCloseButton = new QPushButton("", m_pHBox);
		m_pCloseButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Close)));
		m_pCloseButton->setToolTip(__tr2qs("Close this window"));
		m_pHBox->setStretchFactor(m_pTitleLabel, 2);
		connect(m_pCloseButton, SIGNAL(clicked()), this, SLOT(closeClicked()));
	}
	else
	{
		m_pHBox = nullptr;
		m_pTitleLabel = nullptr;
		m_pCloseButton = nullptr;
	}

	setFrameStyle(QFrame::Sunken | QFrame::Panel);

	KPluginFactory * pKonsoleFactory = KPluginLoader("libkonsolepart").factory();

	if(pKonsoleFactory)
	{
		m_pKonsolePart = static_cast<KParts::ReadOnlyPart *>(pKonsoleFactory->create<QObject>(this, this));

		if(m_pKonsolePart)
		{
			// start the terminal
			qobject_cast<TerminalInterface *>(m_pKonsolePart)->showShellInDir(QString());

			m_pKonsoleWidget = m_pKonsolePart->widget();

			setFocusProxy(m_pKonsoleWidget);
			m_pKonsoleWidget->show();

			connect(m_pKonsolePart, SIGNAL(destroyed()), this, SLOT(konsoleDestroyed()));
		}
		else
		{
			m_pKonsoleWidget = new QLabel(__tr2qs("Can't create the terminal emulation part"), this);
		}
	}
	else
	{
		m_pKonsoleWidget = new QLabel(__tr2qs("Can't retrieve the terminal emulation factory"), this);
	}
}

TermWidget::~TermWidget()
{
	if(m_pKonsoleWidget)
		disconnect(m_pKonsoleWidget, SIGNAL(destroyed()), this, SLOT(konsoleDestroyed()));

	if(m_bIsStandalone)
		g_pTermWidgetList.erase(this);
	if(g_pTermWindowList.empty() && g_pTermWidgetList.empty())
		g_pTermModule->unlock();
}

void TermWidget::resizeEvent(QResizeEvent *)
{
	int hght = 0;
	if(m_bIsStandalone)
	{
		hght = m_pCloseButton->sizeHint().height();
		m_pHBox->setGeometry(1, 1, width() - 2, hght + 1);
	}
	if(m_pKonsoleWidget)
		m_pKonsoleWidget->setGeometry(1, hght + 1, width() - 2, height() - (hght + 2));
}

void TermWidget::closeClicked()
{
	// this is called only in standalone  mode
	delete this;
}

void TermWidget::konsoleDestroyed()
{
	m_pKonsoleWidget = nullptr;
	m_pKonsolePart = nullptr;
	hide();
	QTimer::singleShot(0, this, SLOT(autoClose()));
}

void TermWidget::autoClose()
{
	if(m_bIsStandalone)
		delete this;
	else
		((KviWindow *)parent())->close();
}

void TermWidget::changeTitle(int, const QString & str)
{
	if(m_bIsStandalone)
		m_pTitleLabel->setText(str);
}

void TermWidget::notifySize(int, int)
{
}

void TermWidget::changeColumns(int)
{
}

QSize TermWidget::sizeHint() const
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

	return QSize(wdth + 2, hght + 2);
}

#endif //COMPILE_KDE4_SUPPORT
