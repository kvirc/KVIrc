//=============================================================================
//
//   File : OptionsWidget_proxy.cpp
//   Creation date : Mon Jun 24 2000 22:02:11 by Szymon Stefanek
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

#include "OptionsWidget_proxy.h"

#include "KviLocale.h"
#include "KviIconManager.h"
#include "KviProxy.h"
#include "KviProxyDataBase.h"
#include "KviIpEditor.h"
#include "KviNetUtils.h"
#include "kvi_settings.h"
#include "KviOptions.h"
#include "KviTalGroupBox.h"
#include "KviTalToolTip.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCursor>
#include <QIcon>
#include <QToolButton>
#include <QMenu>

#include <memory>
#include <utility>
#include <vector>

ProxyOptionsTreeWidgetItem::ProxyOptionsTreeWidgetItem(QTreeWidget * parent, const QPixmap & pm, KviProxy * prx)
    : QTreeWidgetItem(parent)
{
	qDebug("Creating item");
	setText(0, prx->hostname());
	setIcon(0, QIcon(pm));
	m_pProxyData = new KviProxy(*prx);
}

ProxyOptionsTreeWidgetItem::~ProxyOptionsTreeWidgetItem()
{
	qDebug("Deleting item");
	delete m_pProxyData;
}

OptionsWidget_proxy::OptionsWidget_proxy(QWidget * parent)
    : KviOptionsWidget(parent, "proxy_options_widget")
{
	createLayout();

	m_pUseProxySelector = addBoolSelector(0, 0, 1, 0, __tr2qs_ctx("Use proxy for all connections", "options"), KviOption_boolUseProxyHost);
	QString szTip = __tr2qs_ctx("When enabled, the selected proxy will be used as the default for all connections. "
	                            "An alternative option to define a specific per-server proxy, is also located "
	                            "in the advanced server dialog in the connection tab.", "options");

	mergeTip(m_pUseProxySelector, szTip);

	m_pTreeWidget = new QTreeWidget(this);
	addWidgetToLayout(m_pTreeWidget, 0, 1, 0, 1);
	m_pTreeWidget->setHeaderLabel(__tr2qs_ctx("Proxy", "options"));
	m_pTreeWidget->setRootIsDecorated(true);
	m_pTreeWidget->setAllColumnsShowFocus(true);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
	    this, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
	    this, SLOT(customContextMenuRequested(const QPoint &)));

	QString tiptxt = __tr2qs_ctx("This is the list of available proxy servers.<br>"
	                             "Right-click on the list to add or remove proxies.", "options");

	mergeTip(m_pTreeWidget, tiptxt);
	mergeTip(m_pTreeWidget->viewport(), tiptxt);

	KviTalVBox * vbox = new KviTalVBox(this);
	addWidgetToLayout(vbox, 1, 1, 1, 1);

	QToolButton * tb = new QToolButton(vbox);
	tb->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NewProxy))));
	tb->setAutoRaise(true);
	connect(tb, SIGNAL(clicked()), this, SLOT(newProxy()));
	mergeTip(tb, __tr2qs_ctx("New proxy", "options"));

	tb = new QToolButton(vbox);
	tb->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Remove))));
	tb->setAutoRaise(true);
	connect(tb, SIGNAL(clicked()), this, SLOT(removeCurrent()));
	mergeTip(tb, __tr2qs_ctx("Remove proxy", "options"));

	QFrame * lll = new QFrame(vbox);
	vbox->setStretchFactor(lll, 100);

	KviTalGroupBox * gbox = addGroupBox(0, 2, 1, 2, Qt::Horizontal, __tr2qs_ctx("Configuration", "options"));

	m_pProxyLabel = new QLabel(__tr2qs_ctx("Proxy:", "options"), gbox);
	m_pProxyEdit = new QLineEdit(gbox);
	m_pPortLabel = new QLabel(__tr2qs_ctx("Port:", "options"), gbox);
	m_pPortEdit = new QLineEdit(gbox);
	m_pIpLabel = new QLabel(__tr2qs_ctx("IP address:", "options"), gbox);
	m_pIpEditor = new KviIpEditor(gbox, KviIpEditor::IPv4);
	m_pUserLabel = new QLabel(__tr2qs_ctx("Username:", "options"), gbox);
	m_pUserEdit = new QLineEdit(gbox);
	m_pPassLabel = new QLabel(__tr2qs_ctx("Password:", "options"), gbox);
	m_pPassEdit = new QLineEdit(gbox);
	m_pProtocolLabel = new QLabel(__tr2qs_ctx("Protocol:", "options"), gbox);
	m_pProtocolBox = new QComboBox(gbox);

	QStringList l = KviProxy::getSupportedProtocolNames();

	m_pProtocolBox->addItems(l);

	m_pIPv6Check = new QCheckBox(__tr2qs_ctx("Use IPv6 protocol", "options"), gbox);
	connect(m_pIPv6Check, SIGNAL(toggled(bool)), this, SLOT(ipV6CheckToggled(bool)));
#ifndef COMPILE_IPV6_SUPPORT
	m_pIPv6Check->setEnabled(false);
#endif
	m_pLastEditedItem = nullptr;

	fillProxyList();

	layout()->setRowStretch(0, 1);
	layout()->setColumnStretch(0, 1);

	m_pContextPopup = new QMenu(this);
}

void OptionsWidget_proxy::enableDisableUseProxySelector()
{
	m_pUseProxySelector->setEnabled(m_pTreeWidget->currentItem());
	if(m_pTreeWidget->topLevelItemCount() < 1)
		m_pUseProxySelector->setChecked(false);
}

void OptionsWidget_proxy::ipV6CheckToggled(bool bEnabled)
{
	m_pIpEditor->setAddressType(bEnabled ? KviIpEditor::IPv6 : KviIpEditor::IPv4);
}

void OptionsWidget_proxy::fillProxyList()
{
	ProxyOptionsTreeWidgetItem * prx;

	std::vector<std::unique_ptr<KviProxy>> & l = g_pProxyDataBase->proxyList();

	for(auto & p : l)
	{
		prx = new ProxyOptionsTreeWidgetItem(m_pTreeWidget, *(g_pIconManager->getSmallIcon(KviIconManager::Proxy)), p.get());
		if(p.get() == g_pProxyDataBase->currentProxy())
		{
			prx->setSelected(true);
			m_pTreeWidget->setCurrentItem(prx);
			m_pTreeWidget->scrollToItem(prx);
		}
	}
	if(!(g_pProxyDataBase->currentProxy()))
		currentItemChanged(nullptr, nullptr);

	enableDisableUseProxySelector();
}

void OptionsWidget_proxy::currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *)
{
	if(m_pLastEditedItem)
		saveLastItem();
	m_pLastEditedItem = (ProxyOptionsTreeWidgetItem *)it;

	m_pProxyLabel->setEnabled(m_pLastEditedItem);
	m_pProxyEdit->setEnabled(m_pLastEditedItem);
	m_pIpLabel->setEnabled(m_pLastEditedItem);
	m_pIpEditor->setEnabled(m_pLastEditedItem);
	m_pUserLabel->setEnabled(m_pLastEditedItem);
	m_pUserEdit->setEnabled(m_pLastEditedItem);
	m_pPassLabel->setEnabled(m_pLastEditedItem);
	m_pPassEdit->setEnabled(m_pLastEditedItem);
	m_pProtocolLabel->setEnabled(m_pLastEditedItem);
	m_pProtocolBox->setEnabled(m_pLastEditedItem);
	m_pPortLabel->setEnabled(m_pLastEditedItem);
	m_pPortEdit->setEnabled(m_pLastEditedItem);

#ifdef COMPILE_IPV6_SUPPORT
	m_pIPv6Check->setEnabled(m_pLastEditedItem);
#else
	m_pIPv6Check->setEnabled(false);
#endif
	if(m_pLastEditedItem)
	{
		m_pProxyEdit->setText(m_pLastEditedItem->m_pProxyData->hostname());

		for(int i = 0; i < m_pProtocolBox->count(); i++)
		{
			KviCString txt = m_pProtocolBox->itemText(i);
			if(kvi_strEqualCI(m_pLastEditedItem->m_pProxyData->protocolName().toUtf8().data(), txt.ptr()))
			{
				m_pProtocolBox->setCurrentIndex(i);
				break;
			}
		}

#ifdef COMPILE_IPV6_SUPPORT
		m_pIPv6Check->setChecked(m_pLastEditedItem->m_pProxyData->isIPv6());
		m_pIpEditor->setAddressType(m_pLastEditedItem->m_pProxyData->isIPv6() ? KviIpEditor::IPv6 : KviIpEditor::IPv4);
#else
		m_pIPv6Check->setChecked(false);
		m_pIpEditor->setAddressType(KviIpEditor::IPv4);
#endif
		if(!m_pIpEditor->setAddress(m_pLastEditedItem->m_pProxyData->ip()))
		{
#ifdef COMPILE_IPV6_SUPPORT
			m_pIpEditor->setAddress(m_pLastEditedItem->m_pProxyData->isIPv6() ? "0:0:0:0:0:0:0:0" : "0.0.0.0");
#else
			m_pIpEditor->setAddress("0.0.0.0");
#endif
		}

		m_pUserEdit->setText(m_pLastEditedItem->m_pProxyData->user());
		m_pPassEdit->setText(m_pLastEditedItem->m_pProxyData->pass());
		KviCString tmp(KviCString::Format, "%u", m_pLastEditedItem->m_pProxyData->port());
		m_pPortEdit->setText(tmp.ptr());
	}
	else
	{
		m_pProxyEdit->setText("");
		m_pUserEdit->setText("");
		m_pPassEdit->setText("");
		m_pPortEdit->setText("");
		m_pIpEditor->setAddress("0.0.0.0");
		m_pIPv6Check->setEnabled(false);
	}
	enableDisableUseProxySelector();
}

void OptionsWidget_proxy::saveLastItem()
{
	if(m_pLastEditedItem)
	{
		QString tmp = m_pProxyEdit->text();
		if(tmp.isEmpty())
			tmp = "irc.unknown.net";

		m_pLastEditedItem->setText(0, tmp);
		m_pLastEditedItem->m_pProxyData->setHostname(tmp);
#ifdef COMPILE_IPV6_SUPPORT
		m_pLastEditedItem->m_pProxyData->setIPv6(m_pIPv6Check->isChecked());
#else
		m_pLastEditedItem->m_pProxyData->setIPv6(false);
#endif
		m_pLastEditedItem->m_pProxyData->setIp("");

		if(m_pIpEditor->isValid())
		{
			QString tmpAddr = m_pIpEditor->address();
#ifdef COMPILE_IPV6_SUPPORT
			if(m_pIPv6Check->isChecked())
			{
				if(tmpAddr != "::" && KviNetUtils::isValidStringIPv6(tmpAddr))
					m_pLastEditedItem->m_pProxyData->setIp(tmpAddr);
			}
			else
			{
#endif
				if(tmpAddr != "0.0.0.0" && KviNetUtils::isValidStringIp(tmpAddr))
					m_pLastEditedItem->m_pProxyData->setIp(tmpAddr);
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
		}
		m_pLastEditedItem->m_pProxyData->setPass(m_pPassEdit->text());
		m_pLastEditedItem->m_pProxyData->setUser(m_pUserEdit->text());
		tmp = m_pPortEdit->text();
		bool bOk;
		kvi_u32_t uPort = tmp.toUInt(&bOk);
		if(!bOk)
			uPort = 1080;
		m_pLastEditedItem->m_pProxyData->setPort(uPort);
		m_pLastEditedItem->m_pProxyData->setNamedProtocol(m_pProtocolBox->currentText());
	}
}

void OptionsWidget_proxy::commit()
{
	saveLastItem();
	g_pProxyDataBase->clear();

	ProxyOptionsTreeWidgetItem * it;

	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		it = (ProxyOptionsTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		QString tmp = it->text(0);
		if(!tmp.isEmpty())
		{
			KviProxy *const proxy = new KviProxy(*(it->m_pProxyData));

			g_pProxyDataBase->insertProxy(std::unique_ptr<KviProxy>(proxy));

			if(it == m_pLastEditedItem)
				g_pProxyDataBase->setCurrentProxy(proxy);
		}
	}

	if(!g_pProxyDataBase->currentProxy() && !g_pProxyDataBase->proxyList().empty())
		g_pProxyDataBase->setCurrentProxy(g_pProxyDataBase->proxyList().front().get());

	KviOptionsWidget::commit();
}

void OptionsWidget_proxy::customContextMenuRequested(const QPoint & pos)
{
	QTreeWidgetItem * it = (QTreeWidgetItem *)m_pTreeWidget->itemAt(pos);
	m_pContextPopup->clear();
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::NewProxy)), __tr2qs_ctx("&New Proxy", "options"), this, SLOT(newProxy()));
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Remove)), __tr2qs_ctx("Re&move Proxy", "options"), this, SLOT(removeCurrent()))
	    ->setEnabled(it);
	m_pContextPopup->popup(QCursor::pos());
}

void OptionsWidget_proxy::newProxy()
{
	KviProxy prx;
	ProxyOptionsTreeWidgetItem * it = new ProxyOptionsTreeWidgetItem(m_pTreeWidget, *(g_pIconManager->getSmallIcon(KviIconManager::Proxy)), &prx);
	it->setSelected(true);
	m_pTreeWidget->setCurrentItem(it);
	m_pTreeWidget->scrollToItem(it);
}

void OptionsWidget_proxy::removeCurrent()
{
	if(m_pLastEditedItem)
	{
		QTreeWidgetItem * tmp = m_pLastEditedItem;
		m_pLastEditedItem = nullptr;
		delete tmp;

		QTreeWidgetItem * it = (QTreeWidgetItem *)m_pTreeWidget->topLevelItem(0);
		if(it)
		{
			it->setSelected(true);
		}
		else
		{
			currentItemChanged(nullptr, nullptr);
		}
	}
}
