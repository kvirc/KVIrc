#ifndef _OPTW_PROXY_H_
#define _OPTW_PROXY_H_
//=============================================================================
//
//   File : optw_proxy.h
//   Creation date : Mon Jun 24 2000 21:58:25 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_optionswidget.h"
#include <QTreeWidget>

#include <QCheckBox>

class QLabel;
class QLineEdit;
class QComboBox;
class KviIpEditor;
class KviTalPopupMenu;
class KviProxy;
class KviProxyDataBase;

extern KVIRC_API KviProxyDataBase * g_pProxyDataBase;

class KviProxyOptionsTreeWidgetItem : public QTreeWidgetItem
{
public:
	KviProxyOptionsTreeWidgetItem(QTreeWidget *parent,const QPixmap &pm,KviProxy * prx);
	~KviProxyOptionsTreeWidgetItem();
public:
	KviProxy * m_pProxyData;
};


#define KVI_OPTIONS_WIDGET_ICON_KviProxyOptionsWidget KVI_SMALLICON_PROXY
#define KVI_OPTIONS_WIDGET_NAME_KviProxyOptionsWidget __tr2qs_no_lookup("Proxy Hosts")
#define KVI_OPTIONS_WIDGET_PARENT_KviProxyOptionsWidget KviConnectionOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviProxyOptionsWidget __tr2qs_no_lookup("connection,servers")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviProxyOptionsWidget 70000

class KviProxyOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviProxyOptionsWidget(QWidget * parent);
	~KviProxyOptionsWidget();
protected:
	QTreeWidget    * m_pTreeWidget;
	QLabel       * m_pProxyLabel;
	QLineEdit    * m_pProxyEdit;
	QLabel       * m_pIpLabel;
	KviIpEditor  * m_pIpEditor;
	QLabel       * m_pUserLabel;
	QLineEdit    * m_pUserEdit;
	QLabel       * m_pPassLabel;
	QLineEdit    * m_pPassEdit;
	QLabel       * m_pPortLabel;
	QLineEdit    * m_pPortEdit;
	QLabel       * m_pProtocolLabel;
	QComboBox    * m_pProtocolBox;
	KviTalPopupMenu   * m_pContextPopup;
	QCheckBox    * m_pIPv6Check;
	KviProxyOptionsTreeWidgetItem * m_pLastEditedItem;
private:

	void fillProxyList();
	void saveLastItem();
protected slots:
	void currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *prev);
	void customContextMenuRequested(const QPoint &pnt);
	void newProxy();
	void removeCurrent();
	void ipV6CheckToggled(bool bEnabled);
public:
	virtual void commit();
};

#endif //!_OPTW_PROXY_H_
