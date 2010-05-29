#ifndef _OPTW_SERVERS_H_
#define _OPTW_SERVERS_H_

//=============================================================================
//
//   File : optw_servers.h
//   Creation date : Wed Jun 12 2000 03:29:01 by Szymon Stefanek
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

#include <QDialog>
#include <QButtonGroup>
#include <QTabWidget>
#include <QCheckBox>
#include <QToolButton>

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class KviIpEditor;
class KviServer;
class KviTalPopupMenu;
class KviScriptEditor;
class KviNickServRule;
class KviNickServRuleEditor;
class KviServer;
class KviNetwork;
class KviNetworkData;
class KviMexServerImport;


class KviServerOptionsTreeWidgetItem : public QTreeWidgetItem
{
public:
	KviServerOptionsTreeWidgetItem(QTreeWidget *parent,const QPixmap &pm,const KviNetwork *n);
	KviServerOptionsTreeWidgetItem(QTreeWidgetItem *parent,const QPixmap &pm,const KviServer *s);
	~KviServerOptionsTreeWidgetItem();
public:
	KviServer * m_pServerData;
	KviNetwork * m_pNetworkData;
public:
	KviServer * serverData() const
	{
		return m_pServerData;
	}
	
	KviNetwork * networkData() const
	{
		return m_pNetworkData;
	}
public:
	void updateVisibleStrings();
};


class KviNetworkDetailsWidget : public QDialog
{
	Q_OBJECT
public:
	KviNetworkDetailsWidget(QWidget * par,KviNetwork * d);
	~KviNetworkDetailsWidget();
protected:
	KviScriptEditor       * m_pOnConnectEditor;
	KviScriptEditor       * m_pOnLoginEditor;

	QLineEdit             * m_pUserEditor;
	KviPasswordLineEdit   * m_pPassEditor;
	QLineEdit             * m_pNickEditor;
	QLineEdit             * m_pRealEditor;
	QLineEdit             * m_pDescEditor;

	QComboBox             * m_pEncodingEditor;
	QComboBox             * m_pTextEncodingEditor;

	QCheckBox             * m_pAutoConnectCheck;

	QTreeWidget        * m_pNickServTreeWidget;
	QCheckBox             * m_pNickServCheck;
	QPushButton           * m_pAddRuleButton;
	QPushButton           * m_pDelRuleButton;
	QPushButton           * m_pEditRuleButton;

	QStringList             m_lstChannels;
	KviChannelListSelector * m_pChannelListSelector;
public:
	void fillData(KviNetwork * d);
protected slots:
	void enableDisableNickServControls();
	void addNickServRule();
	void delNickServRule();
	void editNickServRule();
};

class KviServerDetailsWidget : public QDialog
{
	Q_OBJECT
public:
	KviServerDetailsWidget(QWidget * par,KviServer * s);
	~KviServerDetailsWidget();
protected:
	QString                 m_szHostname;
	QLabel                * m_pHeaderLabel;
	KviScriptEditor       * m_pOnConnectEditor;
	KviScriptEditor       * m_pOnLoginEditor;
	QLineEdit             * m_pUserEditor;
	KviPasswordLineEdit   * m_pPassEditor;
	KviPasswordLineEdit   * m_pSaslPassEditor;
	QLineEdit             * m_pNickEditor;
	QLineEdit             * m_pSaslNickEditor;
	QLineEdit             * m_pRealEditor;
	QLineEdit             * m_pDescEditor;
	QLineEdit             * m_pIdEditor;
	QComboBox             * m_pLinkFilterEditor;
	QComboBox             * m_pEncodingEditor;
	QComboBox             * m_pTextEncodingEditor;
	KviIpEditor           * m_pIpEditor;
	QCheckBox             * m_pCacheIpCheck;
	QCheckBox             * m_pUseSSLCheck;
	QCheckBox             * m_pEnableCAPCheck;
	QCheckBox             * m_pEnableSTARTTLSCheck;
	QCheckBox             * m_pEnableSASLCheck;
	QCheckBox             * m_pUseDefaultInitUMode;
	QCheckBox             * m_pIMode;
	QCheckBox             * m_pWMode;
	QCheckBox             * m_pSMode;
	QCheckBox             * m_pUseAutoConnect;
	QCheckBox             * m_pUseIPV6Check;
	QLineEdit             * m_pPortEditor;
	QStringList             m_lstChannels;
	KviChannelListSelector * m_pChannelListSelector;

	QComboBox             * m_pProxyEditor;
protected slots:
	void useDefaultInitUModeToggled(bool);
	void portEditorTextChanged(const QString &);
	void useIPV6CheckToggled(bool);
public:
	void setHeaderLabelText();
	void fillData(KviServer *s);
};


#define KVI_OPTIONS_WIDGET_ICON_KviServerOptionsWidget KVI_SMALLICON_SERVER
#define KVI_OPTIONS_WIDGET_NAME_KviServerOptionsWidget __tr2qs_no_lookup("Servers")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviServerOptionsWidget __tr2qs_no_lookup("connection")
//#define KVI_OPTIONS_WIDGET_NOPARENT_KviServerOptionsWidget KviConnectionOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviServerOptionsWidget 99000

class KviServerOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviServerOptionsWidget(QWidget * parent);
	~KviServerOptionsWidget();
protected:
	QTreeWidget                  * m_pTreeWidget;
	QLabel                       * m_pSrvNetLabel;
	QLineEdit                    * m_pSrvNetEdit;
	//QLabel                     * m_pPortLabel;
	//QLineEdit                  * m_pPortEdit;
	QPushButton                  * m_pDetailsButton;
	KviTalPopupMenu              * m_pRecentPopup;
	KviTalPopupMenu              * m_pContextPopup;
	KviTalPopupMenu              * m_pImportPopup;
	KviServer                    * m_pClipboard;
	//QCheckBox                  * m_pIPv6Check;
	QPushButton                  * m_pConnectCurrent;
	QPushButton                  * m_pConnectNew;
	KviServerOptionsTreeWidgetItem * m_pLastEditedItem;
	KviServerDetailsWidget       * m_pServerDetailsDialog;
	KviNetworkDetailsWidget      * m_pNetworkDetailsDialog;
	KviMexServerImport           * m_pImportFilter;

	QToolButton                  * m_pNewServerButton;
	QToolButton                  * m_pNewNetworkButton;
	QToolButton                  * m_pRemoveButton;
	QToolButton                  * m_pCopyServerButton;
	QToolButton                  * m_pPasteServerButton;
	QToolButton                  * m_pImportButton;
private:
	void fillServerList();
	void saveLastItem();
	KviServerOptionsTreeWidgetItem * findNetItem(const QString &netname);
	void selectBestServerByUrl(const QString &szUrl);
protected slots:
	void importerDead();
	void importServer(const KviServer & s,const QString &network);
	void importPopupAboutToShow();
	void currentItemChanged(QTreeWidgetItem *cur,QTreeWidgetItem *prev);
	void customContextMenuRequested(const QPoint &pnt);
	void newNetwork();
	void removeCurrent();
	void newServer();
	void copyServer();
	void pasteServer();
	void clearList();
	void itemDoubleClicked(QTreeWidgetItem*, int );
	void detailsClicked();
	void connectCurrentClicked();
	void recentServersPopupAboutToShow();
	void recentServersPopupClicked(int id);
	void importPopupActivated(int id);
public:
	virtual void commit();
};

#endif //!_OPTW_SERVERS_H_
