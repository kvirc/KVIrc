//=============================================================================
//
//   File : kvi_cryptcontroller.cpp
//   Creation date : Fri Nov 03 2000 14:16:33 CEST by Szymon Stefanek
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


#include "kvi_settings.h"


#ifdef COMPILE_CRYPT_SUPPORT
	#include "kvi_cryptcontroller.h"
	#include "kvi_window.h"
	#include "kvi_locale.h"
	#include "kvi_out.h"
	#include "kvi_iconmanager.h"
	#include "kvi_modulemanager.h"
	#include "kvi_module.h"
	#include "kvi_malloc.h"
	#include "kvi_memmove.h"
	#include "kvi_toolwindows_container.h"
	#include "kvi_pointerhashtable.h"

	#include <QLayout>

	// kvi_app.cpp
	extern KVIRC_API KviCryptEngineManager * g_pCryptEngineManager;
	extern KVIRC_API KviModuleManager      * g_pModuleManager;

	KviEngineListBoxItem::KviEngineListBoxItem(KviTalListWidget * pList, KviCryptEngineDescription * pDesc, const char * pcModName)
	: KviTalListWidgetText(pList,pDesc->m_szName)
	{
		m_szName = pDesc->m_szName;
		m_szAuthor = pDesc->m_szAuthor;
		m_szDescription = pDesc->m_szDescription;
		m_iFlags = pDesc->m_iFlags;
		m_szModuleName = pcModName;
		setText(pDesc->m_szName);
	}

	KviEngineListBoxItem::~KviEngineListBoxItem()
	{
	}

	KviCryptController::KviCryptController(QWidget * pParent, KviWindowToolPageButton * pButton, KviWindow * pWnd, KviCryptSessionInfo * pInfo)
	: KviWindowToolWidget(pParent,pButton)
	{
		// Load the known encryption modules
		(void)g_pModuleManager->loadModulesByCaps("crypt");

		m_pWindow = pWnd;

		setFocusPolicy(Qt::ClickFocus);

		QGridLayout * pLayout = new QGridLayout(this);

		QLabel * pLabel = new QLabel(this);
		pLabel->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LOCKED)));
		pLayout->addWidget(pLabel,0,0);
		pLabel = new QLabel(__tr2qs("Cryptography/text transformation"),this);
		pLayout->addWidget(pLabel,0,1,1,3);

		QFrame * pFrame = new QFrame(this);
		pFrame->setFrameStyle(QFrame::HLine | QFrame::Sunken);
		pLayout->addWidget(pFrame,1,0,1,4);

		m_pEnableCheck = new QCheckBox(__tr2qs("Use the crypt engine"),this);
		pLayout->addWidget(m_pEnableCheck,2,0,1,4);
		connect(m_pEnableCheck,SIGNAL(toggled(bool)),this,SLOT(enableCheckToggled(bool)));

		m_pListBox = new KviTalListWidget(this);
		connect(m_pListBox,SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),this,SLOT(engineHighlighted(QListWidgetItem *, QListWidgetItem *)));
		pLayout->addWidget(m_pListBox,3,0,6,1);

		m_pDescriptionLabel = new QLabel(this);
		m_pDescriptionLabel->setWordWrap(true);
		m_pDescriptionLabel->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
		m_pDescriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		pLayout->addWidget(m_pDescriptionLabel,3,1,1,3);

		m_pAuthorLabel = new QLabel(this);
		m_pAuthorLabel->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
		pLayout->addWidget(m_pAuthorLabel,4,1,1,3);

		m_pEnableEncrypt = new QCheckBox(__tr2qs("Enable encryption"),this);
		pLayout->addWidget(m_pEnableEncrypt,5,1,1,3);

		m_pEncryptKeyLabel = new QLabel(__tr2qs("Encrypt key:"),this);
		pLayout->addWidget(m_pEncryptKeyLabel,6,1);

		m_pEncryptKeyEdit = new QLineEdit(this);
		pLayout->addWidget(m_pEncryptKeyEdit,6,2);

		m_pEncryptHexKeyCheck = new QCheckBox(__tr2qs("Hex"),this);
		pLayout->addWidget(m_pEncryptHexKeyCheck,6,3);

		m_pEnableDecrypt = new QCheckBox(__tr2qs("Enable decryption"),this);
		pLayout->addWidget(m_pEnableDecrypt,7,1,1,3);

		m_pDecryptKeyLabel = new QLabel(__tr2qs("Decrypt key:"),this);
		pLayout->addWidget(m_pDecryptKeyLabel,8,1);

		m_pDecryptKeyEdit = new QLineEdit(this);
		pLayout->addWidget(m_pDecryptKeyEdit,8,2);

		m_pDecryptHexKeyCheck = new QCheckBox(__tr2qs("Hex"),this);
		pLayout->addWidget(m_pDecryptHexKeyCheck,8,3);

		m_pOkButton = new QPushButton(__tr2qs("OK"),this);
		pLayout->addWidget(m_pOkButton,9,0,1,4);
		connect(m_pOkButton,SIGNAL(clicked()),this,SLOT(okClicked()));

		pLayout->setRowStretch(3,1);
		pLayout->setColumnStretch(2,1);

		m_pLastItem = 0;
		m_pSessionInfo = 0;

		fillEngineList();

		if(pInfo)
		{
			QListWidgetItem * pItem = m_pListBox->findItems(QString(pInfo->m_szEngineName),Qt::MatchFixedString).first();
			if(pItem)
			{
				m_pEnableCheck->setChecked(true);
				m_pListBox->setCurrentItem(pItem);
				m_pEnableEncrypt->setChecked(pInfo->m_bDoEncrypt);
				m_pEnableDecrypt->setChecked(pInfo->m_bDoDecrypt);
			} else enableWidgets(false);
		} else {
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
		const KviPointerHashTable<QString,KviCryptEngineDescription> * pHash = g_pCryptEngineManager->engineDict();
		if(pHash)
		{
			KviPointerHashTableIterator<QString,KviCryptEngineDescription> it(*pHash);
			while(it.current())
			{
				QString szModName = it.current()->m_providerHandle ? ((KviModule *)(it.current()->m_providerHandle))->name() : "";
				(void)(new KviEngineListBoxItem(m_pListBox,it.current(),szModName.toUtf8().data()));
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
			szDesc += __tr2qs("If you don't want to encrypt a particular text line then just start it with the CTRL+P prefix");
			m_pDescriptionLabel->setText(szDesc);
			m_pEnableEncrypt->setEnabled(pEngine->m_iFlags & KVI_CRYPTENGINE_CAN_ENCRYPT);
			m_pEncryptKeyLabel->setEnabled((pEngine->m_iFlags & KVI_CRYPTENGINE_CAN_ENCRYPT) &&
				(pEngine->m_iFlags & KVI_CRYPTENGINE_WANT_ENCRYPT_KEY));
			m_pEncryptKeyEdit->setEnabled((pEngine->m_iFlags & KVI_CRYPTENGINE_CAN_ENCRYPT) &&
				(pEngine->m_iFlags & KVI_CRYPTENGINE_WANT_ENCRYPT_KEY));
			m_pEnableEncrypt->setChecked(m_pEncryptKeyEdit->isEnabled());
			m_pEnableDecrypt->setEnabled(pEngine->m_iFlags & KVI_CRYPTENGINE_CAN_DECRYPT);
			m_pEncryptHexKeyCheck->setEnabled(m_pEncryptKeyEdit->isEnabled());
			m_pEncryptHexKeyCheck->setChecked(false);
			m_pDecryptKeyLabel->setEnabled((pEngine->m_iFlags & KVI_CRYPTENGINE_CAN_DECRYPT) &&
				(pEngine->m_iFlags & KVI_CRYPTENGINE_WANT_DECRYPT_KEY));
			m_pDecryptKeyEdit->setEnabled((pEngine->m_iFlags & KVI_CRYPTENGINE_CAN_DECRYPT) &&
				(pEngine->m_iFlags & KVI_CRYPTENGINE_WANT_DECRYPT_KEY));
			m_pEnableDecrypt->setChecked(m_pDecryptKeyEdit->isEnabled());
			m_pDecryptHexKeyCheck->setEnabled(m_pDecryptKeyEdit->isEnabled());
			m_pDecryptHexKeyCheck->setChecked(false);
			m_pLastItem = pEngine;
			enableWidgets(true);
		} else m_pLastItem = 0;
	}

	void KviCryptController::enableCheckToggled(bool bChecked)
	{
		enableWidgets(bChecked);
	}

	void KviCryptController::enableWidgets(bool bEnabled)
	{
		m_pListBox->setEnabled(bEnabled);
		m_pAuthorLabel->setEnabled(bEnabled && m_pLastItem);
		m_pDescriptionLabel->setEnabled(bEnabled && m_pLastItem);
		bool bCanDecrypt = m_pLastItem ? m_pLastItem->m_iFlags & KVI_CRYPTENGINE_CAN_DECRYPT : false;
		bool bCanEncrypt = m_pLastItem ? m_pLastItem->m_iFlags & KVI_CRYPTENGINE_CAN_ENCRYPT : false;
		m_pEnableEncrypt->setEnabled(bEnabled && bCanEncrypt);
		m_pEnableDecrypt->setEnabled(bEnabled && bCanDecrypt);
		bool bWantDecryptKey = m_pLastItem ? (bCanDecrypt && (m_pLastItem->m_iFlags & KVI_CRYPTENGINE_WANT_DECRYPT_KEY)) : false;
		bool bWantEncryptKey = m_pLastItem ? (bCanEncrypt && (m_pLastItem->m_iFlags & KVI_CRYPTENGINE_WANT_ENCRYPT_KEY)) : false;
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
		m_pDescriptionLabel->setText(__tr2qs("Sorry, no crypt engines available"));
		m_pDescriptionLabel->setEnabled(true); // we want this text to be visible.
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
					if(!m_pLastItem->m_szModuleName.isEmpty())(void)g_pModuleManager->getModule(m_pLastItem->m_szModuleName.toUtf8().data());
					m_pSessionInfo->m_pEngine = g_pCryptEngineManager->allocateEngine(m_pLastItem->m_szName.toUtf8().data());
					if(!m_pSessionInfo->m_pEngine)
					{
						m_pWindow->output(KVI_OUT_SYSTEMERROR,__tr2qs("Crypt: Can't create an engine instance: crypting disabled"));
						delete m_pSessionInfo;
						m_pSessionInfo = 0;
					} else {
						// initialize the engine
						if(!initializeEngine(m_pSessionInfo->m_pEngine))
						{
							QString szErrStr = m_pSessionInfo->m_pEngine->lastError();
							g_pCryptEngineManager->deallocateEngine(m_pSessionInfo->m_pEngine);
							delete m_pSessionInfo;
							m_pSessionInfo = 0;
							m_pWindow->output(KVI_OUT_SYSTEMERROR,__tr2qs("Crypt: Can't initialize the engine :%s"),szErrStr.toUtf8().data());
						} else {
							// ok, engine ready and waiting...
							m_pSessionInfo->m_szEngineName = m_pLastItem->m_szName;
							m_pSessionInfo->m_bDoEncrypt = m_pEnableEncrypt->isChecked();
							m_pSessionInfo->m_bDoDecrypt = m_pEnableDecrypt->isChecked();
						}
					}
				} else m_pWindow->output(KVI_OUT_SYSTEMERROR,__tr2qs("Crypt: You have to enable encryption and/or decryption for the engine to work"));
			}
		}
		emit done();
	}

	bool KviCryptController::initializeEngine(KviCryptEngine * pEngine)
	{
		KviStr szEncryptKey;
		KviStr szDecryptKey;

		char * pcEncKey = 0;
		char * pcDecKey = 0;
		int iEncKeyLen = 0;
		int iDecKeyLen = 0;

		if(m_pEnableEncrypt->isChecked())
		{
			szEncryptKey = m_pEncryptKeyEdit->text();
			if(m_pEncryptHexKeyCheck->isChecked())
			{
				char * pcTmpKey;
				iEncKeyLen = szEncryptKey.hexToBuffer(&pcTmpKey,false);
				if(iEncKeyLen > 0)
				{
					pcEncKey = (char *)kvi_malloc(iEncKeyLen);
					kvi_memmove(pcEncKey,pcTmpKey,iEncKeyLen);
					KviStr::freeBuffer(pcTmpKey);
				}
			} else {
				pcEncKey = (char *)kvi_malloc(szEncryptKey.len());
				kvi_memmove(pcEncKey,szEncryptKey.ptr(),szEncryptKey.len());
				iEncKeyLen = szEncryptKey.len();
			}
		}

		if(m_pEnableDecrypt->isChecked())
		{
			szDecryptKey = m_pDecryptKeyEdit->text();
			if(m_pDecryptHexKeyCheck->isChecked())
			{
				char * pcTmpKey;
				iDecKeyLen = szDecryptKey.hexToBuffer(&pcTmpKey,false);
				if(iDecKeyLen > 0)
				{
					pcDecKey = (char *)kvi_malloc(iDecKeyLen);
					kvi_memmove(pcDecKey,pcTmpKey,iDecKeyLen);
					KviStr::freeBuffer(pcTmpKey);
				}
			} else {
				pcDecKey = (char *)kvi_malloc(szDecryptKey.len());
				kvi_memmove(pcDecKey,szDecryptKey.ptr(),szDecryptKey.len());
				iDecKeyLen = szDecryptKey.len();
			}
		}

		bool bRet = pEngine->init(pcEncKey,iEncKeyLen,pcDecKey,iDecKeyLen);
		if(pcEncKey)
			kvi_free(pcEncKey);
		if(pcDecKey)
			kvi_free(pcDecKey);

		return bRet;
	}

	KviCryptSessionInfo * KviCryptController::getNewSessionInfo()
	{
		KviCryptSessionInfo * pInfo = m_pSessionInfo;
		m_pSessionInfo = 0;
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
		(*ppInfo)->m_pEngine->disconnect(); // disconnect every signal (this is mainly for destroyed())
		g_pCryptEngineManager->deallocateEngine((*ppInfo)->m_pEngine); // kill the engine
		delete *ppInfo;
		*ppInfo = 0;
	}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_cryptcontroller.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
#endif //COMPILE_CRYPT_SUPPORT
