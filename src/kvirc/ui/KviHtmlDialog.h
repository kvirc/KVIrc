#ifndef _KVI_HTMLDIALOG_H_
#define _KVI_HTMLDIALOG_H_
//=============================================================================
//
//   File : KviHtmlDialog.h
//   Creation date : Wed 03 Jan 2007 03:36:36 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviQString.h"
#include "kvi_settings.h"

#include <QPixmap>
#include <QDialog>
#include <QUrl>
#include <QTextBrowser>
#include <QHash>
#include <QPixmap>

class KviHtmlDialogData
{
public:
	KviHtmlDialogData()
	{
		m_pDoc = new QTextDocument();
	};
	~KviHtmlDialogData() { delete m_pDoc; };

	void addImageResource(const QString & key, const QPixmap & pix)
	{
		if(!m_pDoc)
			m_pDoc = new QTextDocument();
		QUrl url;
		url.setPath(key);
		m_pDoc->addResource(2, url, pix);
	}

	void addHtmlResource(const QString key, const QString value)
	{
		htmlResource.insert(key, value);
	}

	// input

	// mandatory fields
	enum Flags
	{
		ForceMinimumSize = 1
	};
	int iFlags;         // da flags :)
	int iDefaultButton; // the button to use when Enter is pressed (1,2 or 3)
	int iCancelButton;  // the button to use when Esc is pressed (1,2 or 3)
	QString szHtmlText; // Shouldn't be empty :D

	// optional fields
	QString szCaption;        // KVIrc is used when this is empty
	QString szUpperLabelText; // no label is shown if this is empty
	QString szLowerLabelText; // no label is shown if this is empty
	QString szButton1Text;    // OK is used if this is empty
	QString szButton2Text;    // no button is shown if this is empty
	QString szButton3Text;    // no button is shown if this is empty
	QTextDocument * m_pDoc;
	QHash<QString, QString> htmlResource;
	int iMinimumWidth;
	int iMinimumHeight;

	QPixmap pixIcon; // may be null

	// output
	int iSelectedButton; // returns 1,2 or 3
};

class KviTextBrowser : public QTextBrowser
{
public:
	KviTextBrowser(QWidget * par, KviHtmlDialogData * ht)
	    : QTextBrowser(par), m_pHt(ht){};
	~KviTextBrowser(){};
	virtual QVariant loadResource(int type, const QUrl & name)
	{
		QString p = m_pHt->htmlResource.value(name.path());
		qDebug("resource %s type %d and page %s", name.path().toUtf8().data(), type, p.toUtf8().data());
		if(!p.isEmpty())
			return QVariant(p);
		else
			return QVariant();

		//return QTextBrowser::loadResource(type,name);
	}

protected:
	KviHtmlDialogData * m_pHt;
};

class KVIRC_API KviHtmlDialog : public QDialog
{
	Q_OBJECT
public:
	// the dialog does NOT delete this structure and assumes that
	// it remains alive until the dialog closes (i.e. it may access
	// the structure in the destructor
	KviHtmlDialog(QWidget * pParent, KviHtmlDialogData * pData);
	~KviHtmlDialog();

protected:
	KviHtmlDialogData * m_pData;

public:
	// displays the dialog as modal and returns 1,2 or 3
	static int display(QWidget * pParent, KviHtmlDialogData * pData);
protected slots:
	void button1Pressed();
	void button2Pressed();
	void button3Pressed();

protected:
	virtual void reject();
};

#endif //!_KVI_HTMLDIALOG_H_
