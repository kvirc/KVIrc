//=============================================================================
//
//   File : KviCryptController.cpp
//   Creation date : Fri Nov 03 2000 14:16:33 CEST by Szymon Stefanek
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

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptController.h"
#include "KviCryptEngine.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviIconManager.h"
#include "KviModuleManager.h"
#include "KviModule.h"
#include "KviMemory.h"
#include "KviWindowToolWidget.h"
#include "KviPointerHashTable.h"

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

// KviApplication.cpp
extern KVIRC_API KviCryptEngineManager * g_pCryptEngineManager;
extern KVIRC_API KviModuleManager * g_pModuleManager;

KviEngineListBoxItem::KviEngineListBoxItem(KviTalListWidget * pList, KviCryptEngineDescription * pDesc, const char * pcModName)
    : KviTalListWidgetText(pList, pDesc->m_szName)
{
	m_szName = pDesc->m_szName;
	m_szAuthor = pDesc->m_szAuthor;
	m_szDescription = pDesc->m_szDescription;
	m_iFlags = pDesc->m_iFlags;
	m_szModuleName = pcModName;
	setText(pDesc->m_szName);
}

KviEngineListBoxItem::~KviEngineListBoxItem()
    = default;

KviCryptController::KviCryptController(QWidget * pParent, KviWindowToolPageButton * pButton, KviWindow * pWnd, KviCryptSessionInfo * pInfo)
    : KviWindowToolWidget(pParent, pButton)
{
	// Load the known encryption modules
	(void)g_pModuleManager->loadModulesByCaps("crypt");

	m_pWindow = pWnd;

	setFocusPolicy(Qt::ClickFocus);

	QGridLayout * pLayout = new QGridLayout(this);

	QHBoxLayout * pTitleLayout = new QHBoxLayout;

	QLabel * pLabel = new QLabel(this);
	pLabel->setPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::Locked)));
	pTitleLayout->addWidget(pLabel, 0);
	pLabel = new QLabel(__tr2qs("Cryptography/text transformation"), this);
	pTitleLayout->addWidget(pLabel, 1);

	pLayout->addLayout(pTitleLayout, 0, 0);

	QFrame * pFrame = new QFrame(this);
	pFrame->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	pLayout->addWidget(pFrame, 1, 0, 1, 4);

	m_pEnableCheck = new QCheckBox(__tr2qs("Use the encryption engine"), this);
	pLayout->addWidget(m_pEnableCheck, 2, 0, 1, 4);
	connect(m_pEnableCheck, SIGNAL(toggled(bool)), this, SLOT(enableCheckToggled(bool)));

	m_pListBox = new KviTalListWidget(this);
	connect(m_pListBox, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(engineHighlighted(QListWidgetItem *, QListWidgetItem *)));
	pLayout->addWidget(m_pListBox, 3, 0, 6, 1);

	m_pDescriptionText = new QTextEdit(this);
	m_pDescriptionText->setReadOnly(true);
	m_pDescriptionText->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	m_pDescriptionText->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	pLayout->addWidget(m_pDescriptionText, 3, 1, 1, 3);

	m_pAuthorLabel = new QLabel(this);
	m_pAuthorLabel->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	pLayout->addWidget(m_pAuthorLabel, 4, 1, 1, 3);

	m_pEnableEncrypt = new QCheckBox(__tr2qs("Enable encryption"), this);
	pLayout->addWidget(m_pEnableEncrypt, 5, 1, 1, 3);

	m_pEncryptKeyLabel = new QLabel(__tr2qs("Encrypt key:"), this);
	pLayout->addWidget(m_pEncryptKeyLabel, 6, 1);

	m_pEncryptKeyEdit = new QLineEdit(this);
	pLayout->addWidget(m_pEncryptKeyEdit, 6, 2);

	m_pEncryptHexKeyCheck = new QCheckBox(__tr2qs("Hex"), this);
	pLayout->addWidget(m_pEncryptHexKeyCheck, 6, 3);

	m_pEnableDecrypt = new QCheckBox(__tr2qs("Enable decryption"), this);
	pLayout->addWidget(m_pEnableDecrypt, 7, 1, 1, 3);

	m_pDecryptKeyLabel = new QLabel(__tr2qs("Decrypt key:"), this);
	pLayout->addWidget(m_pDecryptKeyLabel, 8, 1);

	m_pDecryptKeyEdit = new QLineEdit(this);
	pLayout->addWidget(m_pDecryptKeyEdit, 8, 2);

	m_pDecryptHexKeyCheck = new QCheckBox(__tr2qs("Hex"), this);
	pLayout->addWidget(m_pDecryptHexKeyCheck, 8, 3);

	m_pOkButton = new QPushButton(__tr2qs("OK"), this);
	pLayout->addWidget(m_pOkButton, 9, 2, 1, 2);
	m_pOkButton->setMinimumWidth(80);
	connect(m_pOkButton, SIGNAL(clicked()), this, SLOT(okClicked()));
	m_pOkButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));

	pLayout->setRowStretch(3, 1);
	pLayout->setColumnStretch(2, 1);

	m_pLastItem = nullptr;
	m_pSessionInfo = nullptr;

	fillEngineList();

	if(pInfo)
	{
		QListWidgetItem * pItem = m_pListBox->findItems(QString(pInfo->m_szEngineName), Qt::MatchFixedString).first();
		if(pItem)
		{
			m_pEnableCheck->setChecked(true);
			m_pListBox->setCurrentItem(pItem);
			m_pEnableEncrypt->setChecked(pInfo->m_bDoEncrypt);
			m_pEnableDecrypt->setChecked(pInfo->m_bDoDecrypt);
		}
		else
			enableWidgets(false);
	}
	else
	{
		enableWidgets(false);
	}
}

KviCryptController::~KviCryptController()
{
	if(m_pSessionInfo)
	{
		// huh ?
		g_pCryptEngineManager->deallocateEngine(m_pSessionInfo->m_pEngine);
		delete m_pSessionInfo;
	}
	// Unload the unused ones...
	g_pModuleManager->cleanupUnusedModules();
}

void KviCryptController::fillEngineList()
{
	const KviPointerHashTable<QString, KviCryptEngineDescription> * pHash = g_pCryptEngineManager->engineDict();
	if(pHash)
	{
		KviPointerHashTableIterator<QString, KviCryptEngineDescription> it(*pHash);
		while(it.current())
		{
			QString szModName = it.current()->m_providerHandle ? ((KviModule *)(it.current()->m_providerHandle))->name() : "";
			(void)(new KviEngineListBoxItem(m_pListBox, it.current(), szModName.toUtf8().data()));
			++it;
		}
		if(m_pListBox->count() != 0)
			return;
	}
	noEnginesAvailable();
}

void KviCryptController::engineHighlighted(QListWidgetItem * pItem, QListWidgetItem *)
{
	if(pItem)
	{
		KviEngineListBoxItem * pEngine = (KviEngineListBoxItem *)pItem;
		m_pAuthorLabel->setText(pEngine->m_szAuthor.toUtf8().data());
		QString szDesc = "<p>";
		szDesc += pEngine->m_szDescription.toUtf8().data();
		szDesc += "<br><br>";
		szDesc += __tr2qs("If you don't want to encrypt a particular text line then just start it with the Ctrl+P prefix");
		m_pDescriptionText->setText(szDesc);
		m_pEnableEncrypt->setEnabled(pEngine->m_iFlags & KviCryptEngine::CanEncrypt);
		m_pEncryptKeyLabel->setEnabled((pEngine->m_iFlags & KviCryptEngine::CanEncrypt) && (pEngine->m_iFlags & KviCryptEngine::WantEncryptKey));
		m_pEncryptKeyEdit->setEnabled((pEngine->m_iFlags & KviCryptEngine::CanEncrypt) && (pEngine->m_iFlags & KviCryptEngine::WantEncryptKey));
		m_pEnableEncrypt->setChecked(m_pEncryptKeyEdit->isEnabled());
		m_pEnableDecrypt->setEnabled(pEngine->m_iFlags & KviCryptEngine::CanDecrypt);
		m_pEncryptHexKeyCheck->setEnabled(m_pEncryptKeyEdit->isEnabled());
		m_pEncryptHexKeyCheck->setChecked(false);
		m_pDecryptKeyLabel->setEnabled((pEngine->m_iFlags & KviCryptEngine::CanDecrypt) && (pEngine->m_iFlags & KviCryptEngine::WantDecryptKey));
		m_pDecryptKeyEdit->setEnabled((pEngine->m_iFlags & KviCryptEngine::CanDecrypt) && (pEngine->m_iFlags & KviCryptEngine::WantDecryptKey));
		m_pEnableDecrypt->setChecked(m_pDecryptKeyEdit->isEnabled());
		m_pDecryptHexKeyCheck->setEnabled(m_pDecryptKeyEdit->isEnabled());
		m_pDecryptHexKeyCheck->setChecked(false);
		m_pLastItem = pEngine;
		enableWidgets(true);
	}
	else
		m_pLastItem = nullptr;
}

void KviCryptController::enableCheckToggled(bool bChecked)
{
	enableWidgets(bChecked);
}

void KviCryptController::enableWidgets(bool bEnabled)
{
	m_pListBox->setEnabled(bEnabled);
	m_pAuthorLabel->setEnabled(bEnabled && m_pLastItem);
	m_pDescriptionText->setEnabled(bEnabled && m_pLastItem);
	bool bCanDecrypt = m_pLastItem ? m_pLastItem->m_iFlags & KviCryptEngine::CanDecrypt : false;
	bool bCanEncrypt = m_pLastItem ? m_pLastItem->m_iFlags & KviCryptEngine::CanEncrypt : false;
	m_pEnableEncrypt->setEnabled(bEnabled && bCanEncrypt);
	m_pEnableDecrypt->setEnabled(bEnabled && bCanDecrypt);
	bool bWantDecryptKey = m_pLastItem ? (bCanDecrypt && (m_pLastItem->m_iFlags & KviCryptEngine::WantDecryptKey)) : false;
	bool bWantEncryptKey = m_pLastItem ? (bCanEncrypt && (m_pLastItem->m_iFlags & KviCryptEngine::WantEncryptKey)) : false;
	m_pEncryptKeyLabel->setEnabled(bEnabled && m_pEnableEncrypt->isChecked() && bWantEncryptKey);
	m_pDecryptKeyLabel->setEnabled(bEnabled && m_pEnableDecrypt->isChecked() && bWantDecryptKey);
	m_pEncryptKeyEdit->setEnabled(m_pEncryptKeyLabel->isEnabled());
	m_pDecryptKeyEdit->setEnabled(m_pDecryptKeyLabel->isEnabled());
	m_pEncryptHexKeyCheck->setEnabled(m_pEncryptKeyLabel->isEnabled());
	m_pDecryptHexKeyCheck->setEnabled(m_pDecryptKeyLabel->isEnabled());
}

void KviCryptController::noEnginesAvailable()
{

	m_pEnableCheck->setEnabled(false);
	enableWidgets(false);
	m_pDescriptionText->setText(__tr2qs("Sorry, no encryption engines available"));
	m_pDescriptionText->setEnabled(true); // we want this text to be visible.
	m_pOkButton->setEnabled(false);
}

void KviCryptController::okClicked()
{
	if(m_pEnableCheck->isChecked())
	{
		if(m_pLastItem)
		{
			if(m_pEnableEncrypt->isChecked() || m_pEnableDecrypt->isChecked())
			{
				m_pSessionInfo = allocateCryptSessionInfo();
				// Reregister the module in case that it has been unloaded
				// while this dialog was open
				if(!m_pLastItem->m_szModuleName.isEmpty())
					(void)g_pModuleManager->getModule(m_pLastItem->m_szModuleName.toUtf8().data());
				m_pSessionInfo->m_pEngine = g_pCryptEngineManager->allocateEngine(m_pLastItem->m_szName.toUtf8().data());
				if(!m_pSessionInfo->m_pEngine)
				{
					m_pWindow->output(KVI_OUT_SYSTEMERROR, __tr2qs("Encryption: can't create an engine instance: encryption disabled"));
					delete m_pSessionInfo;
					m_pSessionInfo = nullptr;
				}
				else
				{
					// initialize the engine
					if(!initializeEngine(m_pSessionInfo->m_pEngine))
					{
						QString szErrStr = m_pSessionInfo->m_pEngine->lastError();
						g_pCryptEngineManager->deallocateEngine(m_pSessionInfo->m_pEngine);
						delete m_pSessionInfo;
						m_pSessionInfo = nullptr;
						m_pWindow->output(KVI_OUT_SYSTEMERROR, __tr2qs("Encryption: can't initialize the engine: %s"), szErrStr.toUtf8().data());
					}
					else
					{
						// ok, engine ready and waiting...
						m_pSessionInfo->m_szEngineName = m_pLastItem->m_szName;
						m_pSessionInfo->m_bDoEncrypt = m_pEnableEncrypt->isChecked();
						m_pSessionInfo->m_bDoDecrypt = m_pEnableDecrypt->isChecked();
					}
				}
			}
			else
				m_pWindow->output(KVI_OUT_SYSTEMERROR, __tr2qs("Encryption: you have to enable encryption and/or decryption for the engine to work"));
		}
	}
	emit done();
}

bool KviCryptController::initializeEngine(KviCryptEngine * pEngine)
{
	KviCString szEncryptKey;
	KviCString szDecryptKey;
	KviCString szEncKey = "";
	KviCString szDecKey = "";

	if(m_pEnableEncrypt->isChecked())
	{
		bool bEcb = false, bOld = false;
		szEncryptKey = m_pEncryptKeyEdit->text();
		if(kvi_strEqualCIN("ecb:", szEncryptKey.ptr(), 4) && (szEncryptKey.len() > 4))
		{
			szEncryptKey.cutLeft(4);
			bEcb = true;
		}
		else if(kvi_strEqualCIN("old:", szEncryptKey.ptr(), 4) && (szEncryptKey.len() > 4))
		{
			szEncryptKey.cutLeft(4);
			bOld = true;
		}
		else if(kvi_strEqualCIN("cbc:", szEncryptKey.ptr(), 4))
		{
			szEncryptKey.cutLeft(4);
		}

		if(m_pEncryptHexKeyCheck->isChecked())
		{
			char * pcTmpKey;
			if(szEncryptKey.hexToBuffer(&pcTmpKey, false))
			{
				szEncKey = pcTmpKey;
				KviCString::freeBuffer(pcTmpKey);
			}
		}
		else
		{
			szEncKey = szEncryptKey;
		}
		if(bEcb)
			szEncKey.prepend("ecb:");
		else if(bOld)
			szEncKey.prepend("old:");
	}

	if(m_pEnableDecrypt->isChecked())
	{
		bool bEcb = false, bOld = false;
		szDecryptKey = m_pDecryptKeyEdit->text();
		if(kvi_strEqualCIN("ecb:", szDecryptKey.ptr(), 4) && (szDecryptKey.len() > 4))
		{
			szDecryptKey.cutLeft(4);
			bEcb = true;
		}
		else if(kvi_strEqualCIN("old:", szDecryptKey.ptr(), 4) && (szDecryptKey.len() > 4))
		{
			szDecryptKey.cutLeft(4);
			bOld = true;
		}
		else if(kvi_strEqualCIN("cbc:", szDecryptKey.ptr(), 4))
		{
			szDecryptKey.cutLeft(4);
		}

		if(m_pDecryptHexKeyCheck->isChecked())
		{
			char * pcTmpKey;
			if(szDecryptKey.hexToBuffer(&pcTmpKey, false))
			{
				szDecKey = pcTmpKey;
				KviCString::freeBuffer(pcTmpKey);
			}
		}
		else
		{
			szDecKey = szDecryptKey;
		}
		if(bEcb)
			szDecKey.prepend("ecb:");
		else if(bOld)
			szDecKey.prepend("old:");
	}

	bool bRet = pEngine->init(szEncKey.ptr(), szEncKey.len(), szDecKey.ptr(), szDecKey.len());

	return bRet;
}

KviCryptSessionInfo * KviCryptController::getNewSessionInfo()
{
	KviCryptSessionInfo * pInfo = m_pSessionInfo;
	m_pSessionInfo = nullptr;
	return pInfo;
}

KviCryptSessionInfo * KviCryptController::allocateCryptSessionInfo()
{
	// this is mainly for modules wanting to alloc this
	return new KviCryptSessionInfo();
}

void KviCryptController::destroyCryptSessionInfo(KviCryptSessionInfo ** ppInfo)
{
	if(!(*ppInfo))
		return;
	(*ppInfo)->m_pEngine->disconnect();                            // disconnect every signal (this is mainly for destroyed())
	g_pCryptEngineManager->deallocateEngine((*ppInfo)->m_pEngine); // kill the engine
	delete *ppInfo;
	*ppInfo = nullptr;
}

#endif //COMPILE_CRYPT_SUPPORT
