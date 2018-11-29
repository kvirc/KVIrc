#ifndef _OPTW_IDENTITY_H_
#define _OPTW_IDENTITY_H_
//=============================================================================
//
//   File : OptionsWidget_identity.h
//   Creation date : Tue Nov 14 2000 23:06:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
#include "kvi_settings.h"
#include "KviSelectors.h"
#include "KviPixmap.h"

#include <QDialog>

class QCheckBox;
class QLineEdit;
class QPushButton;
class QComboBox;
class KviHttpRequest;
class QTreeWidget;
class KviIdentityProfile;

class NickAlternativesDialog : public QDialog
{
	friend class KviIdentityGeneralOptionsWidget;
	Q_OBJECT
protected:
	NickAlternativesDialog(QWidget * par, const QString & n1, const QString & n2, const QString & n3);
	~NickAlternativesDialog();

protected:
	QLineEdit * m_pNickEdit1;
	QLineEdit * m_pNickEdit2;
	QLineEdit * m_pNickEdit3;

protected:
	void fill(QString & n1, QString & n2, QString & n3);
};

class AvatarSelectionDialog : public QDialog
{
	friend class OptionsWidget_identityAvatar;
	Q_OBJECT
public:
	AvatarSelectionDialog(QWidget * par, const QString & szInitialPath);
	~AvatarSelectionDialog();

protected:
	QLineEdit * m_pLineEdit;
	QString m_szAvatarName;

protected:
	void closeEvent(QCloseEvent * e) override;
	const QString & avatarName() { return m_szAvatarName; };
protected slots:
	void okClicked();
	void cancelClicked();
	void chooseFileClicked();
};

class AvatarDownloadDialog : public QDialog
{
	friend class OptionsWidget_identityAvatar;
	Q_OBJECT
public:
	AvatarDownloadDialog(QWidget * par, const QString & szUrl);
	~AvatarDownloadDialog();

protected:
	KviHttpRequest * m_pRequest;
	QLabel * m_pOutput;
	QString m_szErrorMessage;
	QString m_szLocalFileName;
	QString m_szUrl;

protected:
	void closeEvent(QCloseEvent * e) override;
	const QString & localFileName() { return m_szLocalFileName; };
	const QString & errorMessage() { return m_szErrorMessage; };
protected slots:
	void startDownload();
	void cancelClicked();
	void downloadTerminated(bool bSuccess);
	void downloadMessage(const QString & szMsg);
};

class IdentityProfileEditor : public QDialog
{
	friend class OptionsWidget_identityProfile;
	Q_OBJECT
public:
	IdentityProfileEditor(QWidget * pParent);
	~IdentityProfileEditor();

protected:
	QLineEdit * m_pNameEdit;
	QLineEdit * m_pNetworkEdit;
	QLineEdit * m_pNickEdit;
	QLineEdit * m_pAltNickEdit;
	QLineEdit * m_pUserNameEdit;
	QLineEdit * m_pRealNameEdit;
	QPushButton * m_pBtnOk;

public:
	bool editProfile(KviIdentityProfile * pProfile);
protected slots:
	void toggleButton();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_identity KviIconManager::Ident
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_identity __tr2qs_no_lookup("Identity")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_identity 99998
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_identity true

class OptionsWidget_identity : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_identity(QWidget * parent);
	~OptionsWidget_identity();
};

#define KVI_OPTIONS_WIDGET_ICON_KviIdentityGeneralOptionsWidget KviIconManager::Ident
#define KVI_OPTIONS_WIDGET_NAME_KviIdentityGeneralOptionsWidget __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviIdentityGeneralOptionsWidget 90000
#define KVI_OPTIONS_WIDGET_PARENT_KviIdentityGeneralOptionsWidget OptionsWidget_identity

class KviIdentityGeneralOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIdentityGeneralOptionsWidget(QWidget * pParent);
	~KviIdentityGeneralOptionsWidget();

protected:
	QString m_szAltNicknames[3];

	QComboBox * m_pAgeCombo;
	QComboBox * m_pGenderCombo;
protected slots:
	void setNickAlternatives();

protected:
	void commit() override;
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_identityAvatar KviIconManager::Avatar
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_identityAvatar __tr2qs_no_lookup("Avatar")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_identityAvatar 80000
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_identityAvatar OptionsWidget_identity

class OptionsWidget_identityAvatar : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_identityAvatar(QWidget * pParent);
	~OptionsWidget_identityAvatar();

protected:
	QCheckBox * m_pUseAvatarCheck;
	QLineEdit * m_pAvatarNameEdit;
	QPushButton * m_pChooseAvatarButton;
	KviPixmap * m_pLocalAvatar;
	KviPixmapPreview * m_pAvatarPreview;
protected slots:
	void chooseAvatar();

protected:
	void commit() override;
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_identityAdvanced KviIconManager::Gui
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_identityAdvanced __tr2qs_no_lookup("Advanced")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_identityAdvanced 70000
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_identityAdvanced OptionsWidget_identity

class OptionsWidget_identityAdvanced : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_identityAdvanced(QWidget * pParent);
	~OptionsWidget_identityAdvanced();

protected:
	bool m_bI;
	bool m_bW;
	bool m_bS;
	KviCString m_sModeStr;
	KviBoolSelector * m_pISelector;
	KviBoolSelector * m_pWSelector;
	KviBoolSelector * m_pSSelector;

protected:
	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_identityProfile KviIconManager::UserWindow
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_identityProfile __tr2qs_no_lookup("Profiles")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_identityProfile 60000
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_identityProfile OptionsWidget_identity

class OptionsWidget_identityProfile : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_identityProfile(QWidget * pParent);
	~OptionsWidget_identityProfile();

protected:
	QTreeWidget * m_pTreeWidget;
	QCheckBox * m_pProfilesCheck;
	QPushButton * m_pBtnAddProfile;
	QPushButton * m_pBtnEditProfile;
	QPushButton * m_pBtnDelProfile;
	IdentityProfileEditor * m_pEditor;
	int m_iCurrentEditedProfile;

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
