#ifndef _KVI_CRYPTCONTROLLER_H_
#define _KVI_CRYPTCONTROLLER_H_
//=============================================================================
//
//   File : KviCryptController.h
//   Creation date : Fri Nov 03 2000 14:11:03 CEST by Szymon Stefanek
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

#include "kvi_settings.h"

#if defined(COMPILE_CRYPT_SUPPORT)
#include "KviCryptEngineDescription.h"
#include "KviCryptEngineManager.h"
#include "KviHeapObject.h"
#include "KviTalListWidget.h"
#include "KviWindowToolWidget.h"

#include <QString>

class KviCryptEngine;
class KviWindow;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QWidget;

class KVIRC_API KviCryptSessionInfo : public KviHeapObject
{
public:
	KviCryptEngine * m_pEngine;
	QString m_szEngineName;
	bool m_bDoEncrypt;
	bool m_bDoDecrypt;
};

class KVIRC_API KviEngineListBoxItem : public KviTalListWidgetText
{
	friend class KviCryptController;

public:
	KviEngineListBoxItem(KviTalListWidget * lb, KviCryptEngineDescription * d, const char * modName);
	~KviEngineListBoxItem();

public:
	QString m_szName;
	QString m_szAuthor;
	QString m_szDescription;
	QString m_szModuleName;
	int m_iFlags;
};

class KVIRC_API KviCryptController : public KviWindowToolWidget
{
	Q_OBJECT
public:
	KviCryptController(QWidget * pParent, KviWindowToolPageButton * pButton, KviWindow * pWnd, KviCryptSessionInfo * pInfo);
	~KviCryptController();

protected:
	KviWindow * m_pWindow;
	KviTalListWidget * m_pListBox;
	QPushButton * m_pOkButton;
	QCheckBox * m_pEnableCheck;
	QTextEdit * m_pDescriptionText;
	QLabel * m_pAuthorLabel;
	QCheckBox * m_pEnableEncrypt;
	QLabel * m_pEncryptKeyLabel;
	QLineEdit * m_pEncryptKeyEdit;
	QCheckBox * m_pEncryptHexKeyCheck;
	QCheckBox * m_pEnableDecrypt;
	QLabel * m_pDecryptKeyLabel;
	QLineEdit * m_pDecryptKeyEdit;
	QCheckBox * m_pDecryptHexKeyCheck;
	KviEngineListBoxItem * m_pLastItem;
	KviCryptSessionInfo * m_pSessionInfo;
private slots:
	void enableCheckToggled(bool bChecked);
	void engineHighlighted(QListWidgetItem * pItem, QListWidgetItem *);
	void okClicked();

public:
	KviCryptSessionInfo * getNewSessionInfo();
	static KviCryptSessionInfo * allocateCryptSessionInfo();
	static void destroyCryptSessionInfo(KviCryptSessionInfo ** ppInfo);

private:
	void fillEngineList();
	void noEnginesAvailable();
	void enableWidgets(bool bEnabled);
	bool initializeEngine(KviCryptEngine * pEngine);
signals:
	void done();
};
#endif //COMPILE_CRYPT_SUPPORT

#endif //!_KVI_CRYPTCONTROLLER_H_
