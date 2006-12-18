#ifndef _KVI_CRYPTCONTROLLER_H_
#define _KVI_CRYPTCONTROLLER_H_

//
//   File : kvi_cryptcontroller.h
//   Creation date : Fri Nov 03 2000 14:11:03 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"



#ifdef COMPILE_CRYPT_SUPPORT

	#include <qwidget.h>
	#include <qlistbox.h>
	#include <qpushbutton.h>
	#include <qcheckbox.h>
	#include <qlabel.h>
	#include <qlineedit.h>
	#include "kvi_styled_controls.h"
	
	#include "kvi_crypt.h"
	#include "kvi_heapobject.h"
	#include "kvi_toolwindows_container.h"
	
	class KviWindow;

	class KVIRC_API KviCryptSessionInfo : public KviHeapObject
	{
	public:
		KviCryptEngine * pEngine;
		KviStr           szEngineName;
		bool             bDoEncrypt;
		bool             bDoDecrypt;
	};


	class KVIRC_API KviEngineListBoxItem : public QListBoxText
	{
		friend class KviCryptController;
	public:
		KviEngineListBoxItem(QListBox * lb,KviCryptEngineDescription * d,const char * modName);
		~KviEngineListBoxItem();
	public:
		KviStr m_szName;
		KviStr m_szAuthor;
		KviStr m_szDescription;
		KviStr m_szModuleName;
		int m_iFlags;
	};

	
	class KVIRC_API KviCryptController : public KviWindowToolWidget
	{
		Q_OBJECT
	public:
		KviCryptController(QWidget * parent,KviWindowToolPageButton* button,const char * name,KviWindow * wnd,KviCryptSessionInfo * cur);
		~KviCryptController();
	protected:
		KviWindow            * m_pWindow;
		QListBox             * m_pListBox;
		QPushButton          * m_pOkButton;
		KviStyledCheckBox            * m_pEnableCheck;
		QLabel               * m_pDescriptionLabel;
		QLabel               * m_pAuthorLabel;
		KviStyledCheckBox            * m_pEnableEncrypt;
		QLabel               * m_pEncryptKeyLabel;
		QLineEdit            * m_pEncryptKeyEdit;
		KviStyledCheckBox            * m_pEncryptHexKeyCheck;
		KviStyledCheckBox            * m_pEnableDecrypt;
		QLabel               * m_pDecryptKeyLabel;
		QLineEdit            * m_pDecryptKeyEdit;
		KviStyledCheckBox            * m_pDecryptHexKeyCheck;
		KviEngineListBoxItem * m_pLastItem;
		KviCryptSessionInfo  * m_pSessionInfo;
	private slots:
		void enableCheckToggled(bool bChecked);
		void engineHighlighted(QListBoxItem *it);
		void okClicked();
	public:
		KviCryptSessionInfo * getNewSessionInfo();
		static KviCryptSessionInfo * allocateCryptSessionInfo();
		static void destroyCryptSessionInfo(KviCryptSessionInfo ** inf);
	private:
		void fillEngineList();
		void noEnginesAvailable();
		void enableWidgets(bool bEnabled);
		bool initializeEngine(KviCryptEngine * eng);
	signals:
		void done();
	};


#endif //COMPILE_CRYPT_SUPPORT


#endif //!_KVI_CRYPTCONTROLLER_H_
