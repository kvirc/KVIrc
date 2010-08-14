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

	KviEngineListBoxItem::KviEngineListBoxItem(KviTalListWidget * lb,KviCryptEngineDescription * d,const char * modName)
	: KviTalListWidgetText(lb,d->szName)
	{
		m_szName = d->szName;
		m_szAuthor = d->szAuthor;
		m_szDescription = d->szDescription;
		m_iFlags = d->iFlags;
		m_szModuleName = modName;
		setText(d->szName);
	}

	KviEngineListBoxItem::~KviEngineListBoxItem()
	{
	}

	KviCryptController::KviCryptController(QWidget * par,KviWindowToolPageButton* button,const char *,KviWindow * wnd,KviCryptSessionInfo * cur)
	: KviWindowToolWidget(par,button)
	{
		// Load the known encryption modules
		(void)g_pModuleManager->loadModulesByCaps("crypt");

		m_pWindow = wnd;

		setFocusPolicy(Qt::ClickFocus);

		QGridLayout *g = new QGridLayout(this);

		QLabel *l = new QLabel(this);
		l->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LOCKED)));
		g->addWidget(l,0,0);
		l = new QLabel(__tr2qs("Cryptography/text transformation"),this);
		g->addWidget(l,0,1,1,3);

		QFrame * frm = new QFrame(this);
		frm->setFrameStyle(QFrame::HLine | QFrame::Sunken);
		g->addWidget(frm,1,0,1,4);

		m_pEnableCheck = new QCheckBox(__tr2qs("Use the crypt engine"),this);
		g->addWidget(m_pEnableCheck,2,0,1,4);
		connect(m_pEnableCheck,SIGNAL(toggled(bool)),this,SLOT(enableCheckToggled(bool)));

		m_pListBox = new KviTalListWidget(this);
		connect(m_pListBox,SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),this,SLOT(engineHighlighted(QListWidgetItem *, QListWidgetItem *)));
		g->addWidget(m_pListBox,3,0,6,1);

		m_pDescriptionLabel = new QLabel(this);
		m_pDescriptionLabel->setWordWrap(true);
		m_pDescriptionLabel->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
		m_pDescriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		g->addWidget(m_pDescriptionLabel,3,1,1,3);

		m_pAuthorLabel = new QLabel(this);
		m_pAuthorLabel->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
		g->addWidget(m_pAuthorLabel,4,1,1,3);

		m_pEnableEncrypt = new QCheckBox(__tr2qs("Enable encryption"),this);
		g->addWidget(m_pEnableEncrypt,5,1,1,3);

		m_pEncryptKeyLabel = new QLabel(__tr2qs("Encrypt key:"),this);
		g->addWidget(m_pEncryptKeyLabel,6,1);

		m_pEncryptKeyEdit = new QLineEdit(this);
		g->addWidget(m_pEncryptKeyEdit,6,2);

		m_pEncryptHexKeyCheck = new QCheckBox(__tr2qs("Hex"),this);
		g->addWidget(m_pEncryptHexKeyCheck,6,3);

		m_pEnableDecrypt = new QCheckBox(__tr2qs("Enable decryption"),this);
		g->addWidget(m_pEnableDecrypt,7,1,1,3);

		m_pDecryptKeyLabel = new QLabel(__tr2qs("Decrypt key:"),this);
		g->addWidget(m_pDecryptKeyLabel,8,1);

		m_pDecryptKeyEdit = new QLineEdit(this);
		g->addWidget(m_pDecryptKeyEdit,8,2);

		m_pDecryptHexKeyCheck = new QCheckBox(__tr2qs("Hex"),this);
		g->addWidget(m_pDecryptHexKeyCheck,8,3);

		m_pOkButton = new QPushButton(__tr2qs("OK"),this);
		g->addWidget(m_pOkButton,9,0,1,4);
		connect(m_pOkButton,SIGNAL(clicked()),this,SLOT(okClicked()));

		g->setRowStretch(3,1);
		g->setColumnStretch(2,1);

		m_pLastItem = 0;
		m_pSessionInfo = 0;

		fillEngineList();

		if(cur)
		{
			QListWidgetItem * it = m_pListBox->findItems(QString(cur->szEngineName),Qt::MatchFixedString).first();
			if(it)
			{
				m_pEnableCheck->setChecked(true);
				m_pListBox->setCurrentItem(it);
				m_pEnableEncrypt->setChecked(cur->bDoEncrypt);
				m_pEnableDecrypt->setChecked(cur->bDoDecrypt);
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
			g_pCryptEngineManager->deallocateEngine(m_pSessionInfo->pEngine);
			delete m_pSessionInfo;
		}
		// Unload the unused ones...
		g_pModuleManager->cleanupUnusedModules();
	}

	void KviCryptController::fillEngineList()
	{
		const KviPointerHashTable<QString,KviCryptEngineDescription> * a = g_pCryptEngineManager->engineDict();
		if(a)
		{
			KviPointerHashTableIterator<QString,KviCryptEngineDescription> it(*a);
			while(it.current())
			{
				KviStr modName = it.current()->providerHandle ? ((KviModule *)(it.current()->providerHandle))->name() : "";
				(void)(new KviEngineListBoxItem(m_pListBox,it.current(),modName.ptr()));
				++it;
			}
			if(m_pListBox->count() != 0)return;
		}
		noEnginesAvailable();
	}

	void KviCryptController::engineHighlighted(QListWidgetItem *it, QListWidgetItem *)
	{
		if(it)
		{
			KviEngineListBoxItem * eit = (KviEngineListBoxItem *)it;
			m_pAuthorLabel->setText(eit->m_szAuthor.ptr());
			QString des = "<p>";
			des += eit->m_szDescription.ptr();
			des += "<br><br>";
			des += __tr2qs("If you don't want to encrypt a particular text line then just start it with the CTRL+P prefix");
			m_pDescriptionLabel->setText(des);
			m_pEnableEncrypt->setEnabled(eit->m_iFlags & KVI_CRYPTENGINE_CAN_ENCRYPT);
			m_pEncryptKeyLabel->setEnabled((eit->m_iFlags & KVI_CRYPTENGINE_CAN_ENCRYPT) &&
				(eit->m_iFlags & KVI_CRYPTENGINE_WANT_ENCRYPT_KEY));
			m_pEncryptKeyEdit->setEnabled((eit->m_iFlags & KVI_CRYPTENGINE_CAN_ENCRYPT) &&
				(eit->m_iFlags & KVI_CRYPTENGINE_WANT_ENCRYPT_KEY));
			m_pEnableEncrypt->setChecked(m_pEncryptKeyEdit->isEnabled());
			m_pEnableDecrypt->setEnabled(eit->m_iFlags & KVI_CRYPTENGINE_CAN_DECRYPT);
			m_pEncryptHexKeyCheck->setEnabled(m_pEncryptKeyEdit->isEnabled());
			m_pEncryptHexKeyCheck->setChecked(false);
			m_pDecryptKeyLabel->setEnabled((eit->m_iFlags & KVI_CRYPTENGINE_CAN_DECRYPT) &&
				(eit->m_iFlags & KVI_CRYPTENGINE_WANT_DECRYPT_KEY));
			m_pDecryptKeyEdit->setEnabled((eit->m_iFlags & KVI_CRYPTENGINE_CAN_DECRYPT) &&
				(eit->m_iFlags & KVI_CRYPTENGINE_WANT_DECRYPT_KEY));
			m_pEnableDecrypt->setChecked(m_pDecryptKeyEdit->isEnabled());
			m_pDecryptHexKeyCheck->setEnabled(m_pDecryptKeyEdit->isEnabled());
			m_pDecryptHexKeyCheck->setChecked(false);
			m_pLastItem = eit;
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
					if(m_pLastItem->m_szModuleName.hasData())(void)g_pModuleManager->getModule(m_pLastItem->m_szModuleName.ptr());
					m_pSessionInfo->pEngine = g_pCryptEngineManager->allocateEngine(m_pLastItem->m_szName.ptr());
					if(!m_pSessionInfo->pEngine)
					{
						m_pWindow->output(KVI_OUT_SYSTEMERROR,__tr2qs("Crypt: Can't create an engine instance: crypting disabled"));
						delete m_pSessionInfo;
						m_pSessionInfo = 0;
					} else {
						// initialize the engine
						if(!initializeEngine(m_pSessionInfo->pEngine))
						{
							KviStr errStr = m_pSessionInfo->pEngine->lastError();
							g_pCryptEngineManager->deallocateEngine(m_pSessionInfo->pEngine);
							delete m_pSessionInfo;
							m_pSessionInfo = 0;
							m_pWindow->output(KVI_OUT_SYSTEMERROR,__tr2qs("Crypt: Can't initialize the engine :%s"),errStr.ptr());
						} else {
							// ok, engine ready and waiting...
							m_pSessionInfo->szEngineName = m_pLastItem->m_szName;
							m_pSessionInfo->bDoEncrypt = m_pEnableEncrypt->isChecked();
							m_pSessionInfo->bDoDecrypt = m_pEnableDecrypt->isChecked();
						}
					}
				} else m_pWindow->output(KVI_OUT_SYSTEMERROR,__tr2qs("Crypt: You have to enable encryption and/or decryption for the engine to work"));
			}
		}
		emit done();
	}

	bool KviCryptController::initializeEngine(KviCryptEngine * eng)
	{
		KviStr m_szEncryptKey;
		KviStr m_szDecryptKey;

		char * encKey = 0;
		int encKeyLen = 0;

		if(m_pEnableEncrypt->isChecked())
		{
			m_szEncryptKey = m_pEncryptKeyEdit->text();
			if(m_pEncryptHexKeyCheck->isChecked())
			{
				char * tmpKey;
				encKeyLen = m_szEncryptKey.hexToBuffer(&tmpKey,false);
				if(encKeyLen > 0)
				{
					encKey = (char *)kvi_malloc(encKeyLen);
					kvi_memmove(encKey,tmpKey,encKeyLen);
					KviStr::freeBuffer(tmpKey);
				}
			} else {
				encKey = (char *)kvi_malloc(m_szEncryptKey.len());
				kvi_memmove(encKey,m_szEncryptKey.ptr(),m_szEncryptKey.len());
				encKeyLen = m_szEncryptKey.len();
			}
		}

		char * decKey = 0;
		int decKeyLen = 0;

		if(m_pEnableDecrypt->isChecked())
		{
			m_szDecryptKey = m_pDecryptKeyEdit->text();
			if(m_pDecryptHexKeyCheck->isChecked())
			{
				char * tmpKey;
				decKeyLen = m_szDecryptKey.hexToBuffer(&tmpKey,false);
				if(decKeyLen > 0)
				{
					decKey = (char *)kvi_malloc(decKeyLen);
					kvi_memmove(decKey,tmpKey,decKeyLen);
					KviStr::freeBuffer(tmpKey);
				}
			} else {
				decKey = (char *)kvi_malloc(m_szDecryptKey.len());
				kvi_memmove(decKey,m_szDecryptKey.ptr(),m_szDecryptKey.len());
				decKeyLen = m_szDecryptKey.len();
			}
		}

		bool bRet = eng->init(encKey,encKeyLen,decKey,decKeyLen);
		if(encKey)kvi_free(encKey);
		if(decKey)kvi_free(decKey);

		return bRet;
	}

	KviCryptSessionInfo * KviCryptController::getNewSessionInfo()
	{
		KviCryptSessionInfo * inf = m_pSessionInfo;
		m_pSessionInfo = 0;
		return inf;
	}

	KviCryptSessionInfo * KviCryptController::allocateCryptSessionInfo()
	{
		// this is mainly for modules wanting to alloc this
		return new KviCryptSessionInfo();
	}

	void KviCryptController::destroyCryptSessionInfo(KviCryptSessionInfo ** inf)
	{
		if(!(*inf))return;
		(*inf)->pEngine->disconnect(); // disconnect every signal (this is mainly for destroyed())
		g_pCryptEngineManager->deallocateEngine((*inf)->pEngine); // kill the engine
		delete *inf;
		*inf = 0;
	}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_cryptcontroller.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
#endif //COMPILE_CRYPT_SUPPORT
