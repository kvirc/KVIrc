#ifndef _OPTW_SERVERS_H_
#define _OPTW_SERVERS_H_

//=============================================================================
//
//   File : optw_servers.h
//   Creation date : Wed Jun 12 2000 03:29:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#include "kvi_optionswidget.h"
#include "kvi_tal_treewidget.h"

#include <QDialog>
#include <QButtonGroup>
#include <QTabWidget>

class QLabel;
class QLineEdit;
class KviStyledCheckBox;
class QComboBox;
class QPushButton;
class KviStyledToolButton;
class KviIpEditor;
class KviIrcServer;
class KviTalPopupMenu;
class KviScriptEditor;
class KviNickServRule;
class KviNickServRuleEditor;
class KviIrcServer;
class KviIrcNetwork;
class KviIrcNetworkData;
class KviMexServerImport;


class KviServerOptionsTreeWidgetItem : public KviTalTreeWidgetItem
{
public:
	KviServerOptionsTreeWidgetItem(KviTalTreeWidget *parent,const QPixmap &pm,const KviIrcNetwork *n);
	KviServerOptionsTreeWidgetItem(KviTalTreeWidgetItem *parent,const QPixmap &pm,const KviIrcServer *s);
	~KviServerOptionsTreeWidgetItem();
public:
	KviIrcServer * m_pServerData;
	KviIrcNetwork * m_pNetworkData;
public:
	void updateVisibleStrings();
};


class KviNetworkDetailsWidget : public QDialog
{
	Q_OBJECT
public:
	KviNetworkDetailsWidget(QWidget * par,KviIrcNetwork * d);
	~KviNetworkDetailsWidget();
protected:
	KviScriptEditor       * m_pOnConnectEditor;
	KviScriptEditor       * m_pOnLoginEditor;

	QLineEdit             * m_pUserEditor;
	QLineEdit             * m_pPassEditor;
	QLineEdit             * m_pNickEditor;
	QLineEdit             * m_pRealEditor;
	QLineEdit             * m_pDescEditor;

	QComboBox             * m_pEncodingEditor;
	
	KviStyledCheckBox             * m_pAutoConnectCheck;
	
	KviTalTreeWidget        * m_pNickServTreeWidget;
	KviStyledCheckBox             * m_pNickServCheck;
	QPushButton           * m_pAddRuleButton;
	QPushButton           * m_pDelRuleButton;
	QPushButton           * m_pEditRuleButton;

	QStringList             m_lstChannels;
	KviCahnnelListSelector * m_pChannelListSelector;
public:
	void fillData(KviIrcNetwork * d);
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
	KviServerDetailsWidget(QWidget * par,KviIrcServer * s);
	~KviServerDetailsWidget();
protected:
	QString                 m_szHostname;
	QLabel                * m_pHeaderLabel;
	KviScriptEditor       * m_pOnConnectEditor;
	KviScriptEditor       * m_pOnLoginEditor;
	QLineEdit             * m_pUserEditor;
	QLineEdit             * m_pPassEditor;
	QLineEdit             * m_pNickEditor;
	QLineEdit             * m_pRealEditor;
	QLineEdit             * m_pDescEditor;
	QLineEdit             * m_pIdEditor;
	QComboBox             * m_pLinkFilterEditor;
	QComboBox             * m_pEncodingEditor;
	KviIpEditor           * m_pIpEditor;
	KviStyledCheckBox             * m_pCacheIpCheck;
	KviStyledCheckBox             * m_pUseSSLCheck;
	KviStyledCheckBox             * m_pUseDefaultInitUMode;
	KviStyledCheckBox             * m_pIMode;
	KviStyledCheckBox             * m_pWMode;
	KviStyledCheckBox             * m_pSMode;
	KviStyledCheckBox             * m_pUseAutoConnect;
	KviStyledCheckBox             * m_pUseIPV6Check;
	QLineEdit             * m_pPortEditor;
	QStringList             m_lstChannels;
	KviCahnnelListSelector * m_pChannelListSelector;
	
	QComboBox             * m_pProxyEditor;
protected slots:
	void useDefaultInitUModeToggled(bool);
	void portEditorTextChanged(const QString &);
	void useIPV6CheckToggled(bool);
public:
	void setHeaderLabelText();
	void fillData(KviIrcServer *s);
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
	KviTalTreeWidget                    * m_pTreeWidget;
	QLabel                       * m_pSrvNetLabel;
	QLineEdit                    * m_pSrvNetEdit;
	//QLabel                       * m_pPortLabel;
	//QLineEdit                    * m_pPortEdit;
	QPushButton                  * m_pDetailsButton;
	KviTalPopupMenu                   * m_pRecentPopup;
	KviTalPopupMenu                   * m_pContextPopup;
	KviTalPopupMenu                   * m_pImportPopup;
	KviIrcServer                 * m_pClipboard;
	//KviStyledCheckBox                    * m_pIpV6Check;
	QPushButton                  * m_pConnectCurrent;
	QPushButton                  * m_pConnectNew;
	KviServerOptionsTreeWidgetItem * m_pLastEditedItem;
	KviServerDetailsWidget       * m_pServerDetailsDialog;
	KviNetworkDetailsWidget      * m_pNetworkDetailsDialog;
	KviMexServerImport           * m_pImportFilter;

	KviStyledToolButton                  * m_pNewServerButton;
	KviStyledToolButton                  * m_pNewNetworkButton;
	KviStyledToolButton                  * m_pRemoveButton;
	KviStyledToolButton                  * m_pCopyServerButton;
	KviStyledToolButton                  * m_pPasteServerButton;
	KviStyledToolButton                  * m_pImportButton;
private:
	void fillServerList();
	void saveLastItem();
	KviServerOptionsTreeWidgetItem * findNetItem(const QString &netname);
protected slots:
	void importerDead();
	void importServer(const KviIrcServer & s,const QString &network);
	void importPopupAboutToShow();
	void currentItemChanged(KviTalTreeWidgetItem *cur,KviTalTreeWidgetItem *prev);
	void customContextMenuRequested(const QPoint &pnt);
	void newNetwork();
	void removeCurrent();
	void newServer();
	void copyServer();
	void pasteServer();
	void clearList();
	void itemDoubleClicked(KviTalTreeWidgetItem*, int );
	void detailsClicked();
	void connectCurrentClicked();
	void recentServersPopupAboutToShow();
	void recentServersPopupClicked(int id);
	void importPopupActivated(int id);
public:
	virtual void commit();
};

#endif //!_OPTW_SERVERS_H_
