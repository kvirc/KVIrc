#ifndef _OPTW_IDENTITY_H_
#define _OPTW_IDENTITY_H_

//=============================================================================
//
//   File : optw_identity.h
//   Creation date : Tue Nov 14 2000 23:06:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
#include "kvi_settings.h"
#include "kvi_selectors.h"
#include "kvi_pixmap.h"

#include <QDialog>

class QCheckBox;
class QLineEdit;
class QPushButton;
class QComboBox;
class KviHttpRequest;
class QTreeWidget;
class KviIdentityProfile;

class KviNickAlternativesDialog : public QDialog
{
	friend class KviIdentityGeneralOptionsWidget;
	Q_OBJECT
protected:
	KviNickAlternativesDialog(QWidget * par,const QString &n1,const QString &n2,const QString &n3);
	~KviNickAlternativesDialog();
protected:
	QLineEdit * m_pNickEdit1;
	QLineEdit * m_pNickEdit2;
	QLineEdit * m_pNickEdit3;
protected:
	void fill(QString &n1,QString &n2,QString &n3);
};

class KviAvatarSelectionDialog : public QDialog
{
	friend class KviIdentityAvatarOptionsWidget;
	Q_OBJECT
public:
	KviAvatarSelectionDialog(QWidget * par,const QString &szInitialPath);
	~KviAvatarSelectionDialog();
protected:
	QLineEdit   * m_pLineEdit;
	QString       m_szAvatarName;
protected:
	virtual void closeEvent(QCloseEvent * e);
	const QString &avatarName(){ return m_szAvatarName; };
protected slots:
	void okClicked();
	void cancelClicked();
	void chooseFileClicked();
};


class KviAvatarDownloadDialog : public QDialog
{
	friend class KviIdentityAvatarOptionsWidget;
	Q_OBJECT
public:
	KviAvatarDownloadDialog(QWidget * par,const QString &szUrl);
	~KviAvatarDownloadDialog();
protected:
	KviHttpRequest * m_pRequest;
	QLabel         * m_pOutput;
	QString          m_szErrorMessage;
	QString          m_szLocalFileName;
	QString          m_szUrl;
protected:
	virtual void closeEvent(QCloseEvent * e);
	const QString & localFileName(){ return m_szLocalFileName; };
	const QString & errorMessage(){ return m_szErrorMessage; };
protected slots:
	void startDownload();
	void cancelClicked();
	void downloadTerminated(bool bSuccess);
	void downloadMessage(const QString &szMsg);
};




class KviIdentityProfileEditor : public QDialog
{
	friend class KviIdentityProfileOptionsWidget;
	Q_OBJECT
public:
	KviIdentityProfileEditor(QWidget * pParent);
	~KviIdentityProfileEditor();
protected:
	QString       m_szName;
	QString       m_szNetwork;
	QString       m_szNick;
	QString       m_szAltNick;
	QString       m_szUserName;
	QString       m_szRealName;
	QLineEdit   * m_pNameEdit;
	QLineEdit   * m_pNetworkEdit;
	QLineEdit   * m_pNickEdit;
	QLineEdit   * m_pAltNickEdit;
	QLineEdit   * m_pUserNameEdit;
	QLineEdit   * m_pRealNameEdit;
	QPushButton * m_pBtnOk;
public:
	bool editProfile(KviIdentityProfile * pProfile);
protected slots:
	void toggleButton(const QString & szText);
};



#define KVI_OPTIONS_WIDGET_ICON_KviIdentityOptionsWidget KVI_SMALLICON_WHO
#define KVI_OPTIONS_WIDGET_NAME_KviIdentityOptionsWidget __tr2qs_no_lookup("Identity")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIdentityOptionsWidget 99998
#define KVI_OPTIONS_WIDGET_CONTAINER_KviIdentityOptionsWidget true


class KviIdentityOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIdentityOptionsWidget(QWidget * parent);
	~KviIdentityOptionsWidget();
};



#define KVI_OPTIONS_WIDGET_ICON_KviIdentityGeneralOptionsWidget KVI_SMALLICON_WHO
#define KVI_OPTIONS_WIDGET_NAME_KviIdentityGeneralOptionsWidget __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIdentityGeneralOptionsWidget 90000
#define KVI_OPTIONS_WIDGET_PARENT_KviIdentityGeneralOptionsWidget KviIdentityOptionsWidget


class KviIdentityGeneralOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIdentityGeneralOptionsWidget(QWidget * pParent);
	~KviIdentityGeneralOptionsWidget();
protected:
	QString            m_szAltNicknames[3];

	QComboBox        * m_pAgeCombo;
	QComboBox        * m_pGenderCombo;
protected slots:
	void setNickAlternatives();
protected:
	virtual void commit();
};


#define KVI_OPTIONS_WIDGET_ICON_KviIdentityAvatarOptionsWidget KVI_SMALLICON_AVATAR
#define KVI_OPTIONS_WIDGET_NAME_KviIdentityAvatarOptionsWidget __tr2qs_no_lookup("Avatar")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIdentityAvatarOptionsWidget 80000
#define KVI_OPTIONS_WIDGET_PARENT_KviIdentityAvatarOptionsWidget KviIdentityOptionsWidget


class KviIdentityAvatarOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIdentityAvatarOptionsWidget(QWidget * pParent);
	~KviIdentityAvatarOptionsWidget();
protected:
	QCheckBox        * m_pUseAvatarCheck;
	QLineEdit        * m_pAvatarNameEdit;
	QPushButton      * m_pChooseAvatarButton;
	KviPixmap        * m_pLocalAvatar;
	KviPixmapPreview * m_pAvatarPreview;
protected slots:
	void chooseAvatar();
protected:
	virtual void commit();
};


#define KVI_OPTIONS_WIDGET_ICON_KviIdentityAdvancedOptionsWidget KVI_SMALLICON_SPY
#define KVI_OPTIONS_WIDGET_NAME_KviIdentityAdvancedOptionsWidget __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIdentityAdvancedOptionsWidget 70000
#define KVI_OPTIONS_WIDGET_PARENT_KviIdentityAdvancedOptionsWidget KviIdentityOptionsWidget


class KviIdentityAdvancedOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIdentityAdvancedOptionsWidget(QWidget * pParent);
	~KviIdentityAdvancedOptionsWidget();
protected:
	bool               m_bI;
	bool               m_bW;
	bool               m_bS;
	KviStr             m_sModeStr;
	KviBoolSelector  * m_pISelector;
	KviBoolSelector  * m_pWSelector;
	KviBoolSelector  * m_pSSelector;
protected:
	virtual void commit();
};


#define KVI_OPTIONS_WIDGET_ICON_KviIdentityProfileOptionsWidget KVI_SMALLICON_REGUSERS
#define KVI_OPTIONS_WIDGET_NAME_KviIdentityProfileOptionsWidget __tr2qs_no_lookup("Profiles")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIdentityProfileOptionsWidget 60000
#define KVI_OPTIONS_WIDGET_PARENT_KviIdentityProfileOptionsWidget KviIdentityOptionsWidget

class KviIdentityProfileOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIdentityProfileOptionsWidget(QWidget * pParent);
	~KviIdentityProfileOptionsWidget();
protected:
	QTreeWidget              * m_pTreeWidget;
	QCheckBox                * m_pProfilesCheck;
	QPushButton              * m_pBtnAddProfile;
	QPushButton              * m_pBtnEditProfile;
	QPushButton              * m_pBtnDelProfile;
	KviIdentityProfileEditor * m_pEditor;
	int                        m_iCurrentEditedProfile;
public:
	virtual void commit();
protected slots:
	void toggleControls();
	void addProfileEntry();
	void editProfileEntry();
	void delProfileEntry();
	void editProfileOkPressed();
};

#endif //_OPTW_IDENTITY_H_
