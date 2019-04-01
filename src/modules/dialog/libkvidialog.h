#ifndef _KVI_DIALOG_H_
#define _KVI_DIALOG_H_
//=============================================================================
//
//   File : libkvidialog.h
//   Creation date : Sat Sep 15 2001 01:13:25 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviWindow.h"
#include "KviFileDialog.h"
#include "KviImageDialog.h"
#include "KviKvsCallbackObject.h"

#include <QDialog>
#include <QMessageBox>
#include <QString>

class KviKvsCallbackMessageBox : public QMessageBox, public KviKvsCallbackObject
{
	Q_OBJECT
public:
	KviKvsCallbackMessageBox(
	    const QString & szCaption,
	    const QString & szText,
	    const QString & szIcon,
	    const QString & szButton0,
	    const QString & szButton1,
	    const QString & szButton2,
	    const QString & szCode,
	    KviKvsVariantList * pMagicParams,
	    KviWindow * pWindow, bool modal = false);
	~KviKvsCallbackMessageBox();
protected slots:
	void done(int code) override;
};

class KviKvsCallbackTextInput : public QDialog, public KviKvsCallbackObject
{
	Q_OBJECT
public:
	KviKvsCallbackTextInput(
	    const QString & szCaption,
	    const QString & szLabel,
	    const QString & szDefaultText,
	    const QString & szIcon,
	    bool bMultiLine,
	    bool bPassword,
	    const QString & szButton0,
	    const QString & szButton1,
	    const QString & szButton2,
	    const QString & szCode,
	    KviKvsVariantList * pMagicParams,
	    KviWindow * pWindow, bool modal = false);
	~KviKvsCallbackTextInput();

protected:
	bool m_bMultiLine;
	bool m_bPassword;
	QWidget * m_pEdit;
	int m_iEscapeButton;
	int m_iDefaultButton;

protected:
	void closeEvent(QCloseEvent * e) override;
	void showEvent(QShowEvent * e) override;
protected slots:
	void b0Clicked();
	void b1Clicked();
	void b2Clicked();
	void done(int code) override;
};

class KviKvsCallbackFileDialog : public KviFileDialog, public KviKvsCallbackObject
{
	Q_OBJECT
public:
	KviKvsCallbackFileDialog(
	    const QString & szCaption,
	    const QString & szInitialSelection,
	    const QString & szFilter,
	    const QString & szCode,
	    KviKvsVariantList * pMagicParams,
	    KviWindow * pWindow, bool modal = false);
	~KviKvsCallbackFileDialog();

protected:
	void done(int code) override;
};

class KviKvsCallbackImageDialog : public KviImageDialog, public KviKvsCallbackObject
{
	Q_OBJECT
public:
	KviKvsCallbackImageDialog(
	    const QString & szCaption,
	    const QString & szInitialSelection,
	    int iType,
	    int iMaxSize,
	    const QString & szCode,
	    KviKvsVariantList * pMagicParams,
	    KviWindow * pWindow, bool modal = false);
	~KviKvsCallbackImageDialog();

protected:
	void done(int code) override;
};

#endif //_KVI_DIALOG_H_
