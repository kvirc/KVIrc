#ifndef _OPTW_SERVERS_H_
#define _OPTW_SERVERS_H_
//=============================================================================
//
//   File : OptionsWidget_servers.h
//   Creation date : Wed Jun 12 2000 03:29:01 by Szymon Stefanek
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
class KviIrcServer;
class QMenu;
class KviScriptEditor;
class KviNickServRule;
class NickServRuleEditor;
class KviIrcServer;
class KviIrcNetwork;
class KviIrcNetworkData;
class KviMexServerImport;

class IrcServerOptionsTreeWidgetItem : public QTreeWidgetItem
{
public:
	IrcServerOptionsTreeWidgetItem(QTreeWidget * parent, const QPixmap & pm, const KviIrcNetwork * n);
	IrcServerOptionsTreeWidgetItem(QTreeWidgetItem * parent, const QPixmap & pm, const KviIrcServer * s);
	~IrcServerOptionsTreeWidgetItem();

public:
	KviIrcServer * m_pServerData;
	KviIrcNetwork * m_pNetworkData;

public:
	KviIrcServer * serverData() const
	{
		return m_pServerData;
	}

	KviIrcNetwork * networkData() const
	{
		return m_pNetworkData;
	}

public:
	void updateVisibleStrings();
};

class IrcNetworkDetailsWidget : public QDialog
{
	Q_OBJECT
public:
	IrcNetworkDetailsWidget(QWidget * par, KviIrcNetwork * d);
	~IrcNetworkDetailsWidget();

protected:
	KviScriptEditor * m_pOnConnectEditor;
	KviScriptEditor * m_pOnLoginEditor;

	QLineEdit * m_pUserEditor;
	KviPasswordLineEdit * m_pPassEditor;
	QLineEdit * m_pNickEditor;
	QLineEdit * m_pAlternativeNickEditor;
	QLineEdit * m_pRealEditor;
	QLineEdit * m_pDescEditor;

	QComboBox * m_pEncodingEditor;
	QComboBox * m_pTextEncodingEditor;

	QCheckBox * m_pAutoConnectCheck;

	QTreeWidget * m_pNickServTreeWidget;
	QCheckBox * m_pNickServCheck;
	QPushButton * m_pAddRuleButton;
	QPushButton * m_pDelRuleButton;
	QPushButton * m_pEditRuleButton;

	QStringList m_lstChannels;
	KviChannelListSelector * m_pChannelListSelector;

public:
	void fillData(KviIrcNetwork * d);
protected slots:
	void enableDisableNickServControls();
	void addNickServRule();
	void delNickServRule();
	void editNickServRule();
};

class IrcServerDetailsWidget : public QDialog
{
	Q_OBJECT
public:
	IrcServerDetailsWidget(QWidget * par, KviIrcServer * s);
	~IrcServerDetailsWidget();

protected:
	QString m_szHostname;
	QLabel * m_pHeaderLabel;
	KviScriptEditor * m_pOnConnectEditor;
	KviScriptEditor * m_pOnLoginEditor;
	QLineEdit * m_pUserEditor;
	KviPasswordLineEdit * m_pPassEditor;
	KviPasswordLineEdit * m_pSaslPassEditor;
	QLineEdit * m_pNickEditor;
	QLineEdit * m_pAlternativeNickEditor;
	QLineEdit * m_pSaslNickEditor;
	QLineEdit * m_pRealEditor;
	QLineEdit * m_pDescEditor;
	QLineEdit * m_pIdEditor;
	QComboBox * m_pLinkFilterEditor;
	QComboBox * m_pEncodingEditor;
	QComboBox * m_pTextEncodingEditor;
	KviIpEditor * m_pIpEditor;
	QCheckBox * m_pCacheIpCheck;
	QCheckBox * m_pUseSSLCheck;
	QCheckBox * m_pEnableCAPCheck;
	QCheckBox * m_pEnableSTARTTLSCheck;
	QCheckBox * m_pEnableSASLCheck;
	QCheckBox * m_pUseDefaultInitUMode;
	QCheckBox * m_pIMode;
	QCheckBox * m_pWMode;
	QCheckBox * m_pSMode;
	QCheckBox * m_pUseAutoConnect;
	QCheckBox * m_pUseIPV6Check;
	QLineEdit * m_pPortEditor;
	QStringList m_lstChannels;
	KviChannelListSelector * m_pChannelListSelector;
	QComboBox * m_pSaslMethodComboBox;

	QComboBox * m_pProxyEditor;
protected slots:
	void useDefaultInitUModeToggled(bool);
	void portEditorTextChanged(const QString &);
	void useCacheIpCheckToggled(bool);
	void useIPV6CheckToggled(bool);
	void useSSLCheckToggled(bool);

public:
	void setHeaderLabelText();
	void fillData(KviIrcServer * s);
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_servers KviIconManager::ServerConfiguration
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_servers __tr2qs_no_lookup("Servers and Configuration")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_servers __tr2qs_no_lookup("connection")
//#define KVI_OPTIONS_WIDGET_NOPARENT_OptionsWidget_servers OptionsWidget_connection
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_servers 99000

class OptionsWidget_servers : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_servers(QWidget * parent);
	~OptionsWidget_servers();

protected:
	QTreeWidget * m_pTreeWidget;
	QLabel * m_pSrvNetLabel;
	QLineEdit * m_pSrvNetEdit;
	QLabel * m_pFilterLabel;
	QLineEdit * m_pFilterEdit;
	QPushButton * m_pDetailsButton;
	QMenu * m_pRecentPopup;
	QMenu * m_pContextPopup;
	QMenu * m_pImportPopup;
	KviIrcServer * m_pClipboard;
	QPushButton * m_pConnectCurrent;
	IrcServerOptionsTreeWidgetItem * m_pLastEditedItem;
	IrcServerDetailsWidget * m_pServerDetailsDialog;
	IrcNetworkDetailsWidget * m_pNetworkDetailsDialog;
	KviMexServerImport * m_pImportFilter;
	KviBoolSelector * m_pShowThisDialogAtStartupSelector;

	QToolButton * m_pNewServerButton;
	QToolButton * m_pNewNetworkButton;
	QToolButton * m_pRemoveButton;
	QToolButton * m_pCopyServerButton;
	QToolButton * m_pPasteServerButton;
	QToolButton * m_pImportButton;
	QToolButton * m_pFavoriteServerButton;
	bool m_bShowingFavoritesOnly;
	QToolButton * m_pShowFavoritesOnlyButton;

private:
	void fillServerList();
	void saveLastItem();
	IrcServerOptionsTreeWidgetItem * findNetItem(const QString & netname);
	void selectBestServerByUrl(const QString & szUrl);
protected slots:
	void slotShowThisDialogAtStartupSelectorDestroyed();
	void importerDead();
	void importServer(const KviIrcServer & s, const QString & network);
	void importPopupAboutToShow();
	void currentItemChanged(QTreeWidgetItem * cur, QTreeWidgetItem * prev);
	void customContextMenuRequested(const QPoint & pnt);
	void newNetwork();
	void removeCurrent();
	void newServer();
	void updateFavoritesFilter(bool bSet);
	void favoriteServer();
	void copyServer();
	void pasteServer();
	void clearList();
	void itemDoubleClicked(QTreeWidgetItem *, int);
	void detailsClicked();
	void connectCurrentClicked();
	void recentServersPopupAboutToShow();
	void recentServersPopupClicked(QAction * pAction);
	void importPopupActivated(QAction * pAction);
	void serverNetworkEditTextEdited(const QString & szNewText);
	void filterTextEdited(const QString & szNewText);

public:
	virtual void commit();
};

#endif //!_OPTW_SERVERS_H_
