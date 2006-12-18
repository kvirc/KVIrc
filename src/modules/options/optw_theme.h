#ifndef _OPTW_THEME_H_
#define _OPTW_THEME_H_

//
//   File : optw_theme.h
//   Creation date : Sun May 25 2003 18:03:11 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_optionswidget.h"
#include <qdialog.h>
#include <qsimplerichtext.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qcursor.h>

#define KVI_OPTIONS_WIDGET_ICON_KviThemeOptionsWidget KVI_SMALLICON_GUI
#define KVI_OPTIONS_WIDGET_NAME_KviThemeOptionsWidget __tr2qs_no_lookup("Load & Save")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviThemeOptionsWidget 90000
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviThemeOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviThemeOptionsWidget "themeLoader"

class QLineEdit;
class QTextEdit;
class QPushButton;
class QLabel;
class QCheckBox;
typedef struct _KviThemeInfoTag KviThemeInfo;

class KviSaveThemeDialog : public QDialog
{
	Q_OBJECT
public:
	KviSaveThemeDialog(QWidget *par);
	~KviSaveThemeDialog();
protected:
	QLineEdit   * m_pAuthorEdit;
	QLineEdit   * m_pThemeNameEdit;
	QLineEdit   * m_pVersionEdit;
	QTextEdit   * m_pDescriptionEdit;
	QPushButton * m_pOkButton;
	QComboBox   * m_pFormatCombo;
	QCheckBox   * m_pIncludeMsgcolors;

private:
	void saveThemeInNewFormat(KviThemeInfo& info);
	void saveThemeInOldFormat(KviThemeInfo& info);
	void saveThemeInXmlFormat(KviThemeInfo& info);
protected slots:
	void saveTheme();
	void themeNameChanged(const QString &txt);
};

class KviThemeListBoxItem : public QListBoxText
{
public:
	KviThemeListBoxItem(QListBox * box,KviThemeInfo * inf);
	~KviThemeListBoxItem();
public:
	KviThemeInfo * m_pThemeInfo;
	QSimpleRichText * m_pText;
public:
	KviThemeInfo * themeInfo(){ return m_pThemeInfo; };
	virtual int height ( const QListBox * lb ) const ;
protected:
	virtual void paint ( QPainter * painter );
};


class KviThemeOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviThemeOptionsWidget(QWidget * parent);
	~KviThemeOptionsWidget();
protected:
	QListBox           * m_pListBox;
	QPopupMenu         * m_pContextPopup; 
	KviSaveThemeDialog * m_pSaveThemeDialog;
protected:
	virtual void commit();
	void fillThemeBox(const QString &szDir);
protected slots:
	void saveCurrentTheme();
	void getMoreThemes();
	void installFromXml();
	void fillThemeBox();
	void deleteTheme();
	void applyTheme ( QListBoxItem * );
	void applyCurrentTheme();
	void contextMenuRequested ( QListBoxItem * item, const QPoint & pos );
};

#endif //!_OPTW_THEME_H_
