#ifndef _OPTW_PROXY_H_
#define _OPTW_PROXY_H_
//=============================================================================
//
//   File : OptionsWidget_proxy.h
//   Creation date : Mon Jun 24 2000 21:58:25 by Szymon Stefanek
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

#include "KviOptionsWidget.h"
#include "KviIconManager.h"

#include <QTreeWidget>
#include <QCheckBox>

class QLabel;
class QLineEdit;
class QComboBox;
class KviIpEditor;
class QMenu;
class KviProxy;
class KviProxyDataBase;
class KviBoolSelector;

extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;

class ProxyOptionsTreeWidgetItem : public QTreeWidgetItem
{
public:
	ProxyOptionsTreeWidgetItem(QTreeWidget * parent, const QPixmap & pm, KviProxy * prx);
	~ProxyOptionsTreeWidgetItem();

public:
	KviProxy * m_pProxyData;
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_proxy KviIconManager::Proxy
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_proxy __tr2qs_no_lookup("Proxy Hosts")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_proxy OptionsWidget_connection
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_proxy __tr2qs_no_lookup("connection,servers")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_proxy 70000

class OptionsWidget_proxy : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_proxy(QWidget * parent);

protected:
	QTreeWidget * m_pTreeWidget;
	QLabel * m_pProxyLabel;
	QLineEdit * m_pProxyEdit;
	QLabel * m_pIpLabel;
	KviIpEditor * m_pIpEditor;
	QLabel * m_pUserLabel;
	QLineEdit * m_pUserEdit;
	QLabel * m_pPassLabel;
	QLineEdit * m_pPassEdit;
	QLabel * m_pPortLabel;
	QLineEdit * m_pPortEdit;
	QLabel * m_pProtocolLabel;
	QComboBox * m_pProtocolBox;
	QMenu * m_pContextPopup;
	QCheckBox * m_pIPv6Check;
	KviBoolSelector * m_pUseProxySelector;
	ProxyOptionsTreeWidgetItem * m_pLastEditedItem;

private:
	void fillProxyList();
	void saveLastItem();
	void enableDisableUseProxySelector();
protected slots:
	void currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem * prev);
	void customContextMenuRequested(const QPoint & pnt);
	void newProxy();
	void removeCurrent();
	void ipV6CheckToggled(bool bEnabled);

public:
	virtual void commit();
};

#endif //!_OPTW_PROXY_H_
