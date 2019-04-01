#ifndef _KVI_SELECTORS_H_
#define _KVI_SELECTORS_H_
//=============================================================================
//
//   File : KviSelectors.h
//   Creation date : Mon Now 13 2000 15:21:10 CEST by Szymon Stefanek
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
#include "KviPixmap.h"
#include "KviTalHBox.h"
#include "KviTalListWidget.h"
#include "KviTalVBox.h"

#include <QCheckBox>
#include <QColor>
#include <QFont>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QValidator>

class KVIRC_API KviSelectorInterface
{
public:
	KviSelectorInterface(){};
	virtual ~KviSelectorInterface(){};

public:
	virtual void commit(){};
	virtual QString textForSearch() { return QString(); };
	virtual QWidget * widgetToHighlight() { return nullptr; };
};

class KVIRC_API KviBoolSelector : public QCheckBox, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviBoolSelector(QWidget * par, const QString & txt, bool * pOption, bool bEnabled);
	~KviBoolSelector(){};

private:
	bool * m_pOption;

public:
	virtual void commit();
	QString textForSearch() { return text(); };
	QWidget * widgetToHighlight() { return (QWidget *)this; };
public slots:
	void setNotEnabled(bool bNotEnabled);
	void setEnabled(bool bEnabled);
};

class KVIRC_API KviStringSelector : public KviTalHBox, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviStringSelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled);
	~KviStringSelector();

protected:
	QLabel * m_pLabel;
	QLineEdit * m_pLineEdit;
	QString * m_pOption;

public:
	void setValidator(QValidator * v) { m_pLineEdit->setValidator(v); };
	void setMinimumLabelWidth(unsigned int uWidth) { m_pLabel->setMinimumWidth(uWidth); };
	virtual void commit();
	void setText(const QString & text);
	QString textForSearch() { return m_pLabel->text(); };
	QWidget * widgetToHighlight() { return (QWidget *)m_pLabel; };
	QLineEdit * lineEdit() { return m_pLineEdit; };
	QString currentText() { return m_pLineEdit->text(); };
public slots:
	virtual void setEnabled(bool bEnabled);
};

class KVIRC_API KviPasswordSelector : public KviTalHBox, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviPasswordSelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled);
	~KviPasswordSelector();

protected:
	QLabel * m_pLabel;
	QString textForSearch() { return m_pLabel->text(); };
	QWidget * widgetToHighlight() { return (QWidget *)m_pLabel; };
	QLineEdit * m_pLineEdit;
	QCheckBox * m_pCheckBox;
	QString * m_pOption;

public:
	void setValidator(QValidator * v) { m_pLineEdit->setValidator(v); };
	void setMinimumLabelWidth(unsigned int uWidth) { m_pLabel->setMinimumWidth(uWidth); };
	virtual void commit();
	void setText(const QString & text);
public slots:
	virtual void setEnabled(bool bEnabled);
protected slots:
	void checkToggled(int state);
};

class KVIRC_API KviPasswordLineEdit : public KviTalHBox
{
	Q_OBJECT
public:
	KviPasswordLineEdit(QWidget * par);
	~KviPasswordLineEdit();

protected:
	QLineEdit * m_pLineEdit;
	QCheckBox * m_pCheckBox;

public:
	QString text();
	void setText(const QString & text);
public slots:
	virtual void setEnabled(bool bEnabled);
protected slots:
	void checkToggled(int state);
};

class KVIRC_API KviPixmapPreview : public QScrollArea
{
	Q_OBJECT
public:
	KviPixmapPreview(QWidget * par);
	~KviPixmapPreview();

protected:
	QLabel * m_pLabPixmap;

public:
	void setPixmap(KviPixmap * pix);
};

class KVIRC_API KviPixmapSelector : public QWidget, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviPixmapSelector(QWidget * par, const QString & txt, KviPixmap * pOption, bool bEnabled);
	~KviPixmapSelector();

private:
	QCheckBox * m_pCheckBox;
	QLabel * m_pFileNameLabel;
	KviPixmapPreview * m_pPreview;
	QPushButton * m_pChooseButton;
	KviPixmap * m_pOption;
	KviPixmap m_localPixmap;

public:
	void setImagePath(const QString & path);
	virtual void commit();
	virtual void setEnabled(bool bEnabled);
protected slots:
	void checkBoxToggled(bool bEnabled);
	void choosePixmap();
};

class KVIRC_API KviUIntSelector : public KviTalHBox, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviUIntSelector(QWidget * par, const QString & txt, unsigned int * pOption,
	    unsigned int uLowBound, unsigned int uHighBound, unsigned int uDefault, bool bEnabled, bool bShortInt = false);
	~KviUIntSelector(){};

private:
	QLabel * m_pLabel;

	QSpinBox * m_pSpinBox;
	unsigned int * m_pOption;
	unsigned int m_uLowBound;
	unsigned int m_uHighBound;
	unsigned int m_uDefault;
	bool m_bIsShortInt;

public:
	QString textForSearch() { return m_pLabel->text(); };
	QWidget * widgetToHighlight() { return (QWidget *)m_pLabel; };
	virtual void setPrefix(const QString & txt);
	virtual void setSuffix(const QString & txt);
	virtual void commit();
public slots:
	virtual void setEnabled(bool bEnabled);
};

class KVIRC_API KviFileSelector : public QWidget, public KviSelectorInterface
{
	Q_OBJECT
	Q_PROPERTY(QString tmpFile READ tmpFile WRITE setTmpFile)
public:
	KviFileSelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled, unsigned int uFlags = 0, const QString & szFilter = QString());
	~KviFileSelector(){};

public:
	enum Flags
	{
		ChooseSaveFileName = 1,
		DontConfirmOverwrite = 2,
		VerticalLayout = 4
	};

protected:
	QLabel * m_pLabel;
	QLineEdit * m_pLineEdit;
	QPushButton * m_pButton;
	QString * m_pOption;
	unsigned int m_uFlags;
	QString m_szFilter;
	QGridLayout * m_Layout;

public:
	QString tmpFile() { return m_pLineEdit->text(); };
	void setTmpFile(QString & szFile) { m_pLineEdit->setText(szFile); };
	virtual void commit();
	void setSelection(const QString & szSelection);
signals:
	void selectionChanged(const QString & szNewValue);
public slots:
	virtual void setEnabled(bool bEnabled);
private slots:
	void browseClicked();

protected:
	virtual void select();
};

class KVIRC_API KviDirectorySelector : public KviFileSelector
{
	Q_OBJECT
public:
	KviDirectorySelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled, unsigned int uFlags = 0);
	~KviDirectorySelector(){};

protected:
	virtual void select();
};

class KVIRC_API KviColorSelector : public KviTalHBox, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviColorSelector(QWidget * par, const QString & txt, QColor * pOption, bool bEnabled);
	~KviColorSelector(){};

private:
	QLabel * m_pLabel;
	QColor * m_pOption;
	QColor m_memColor;
	QPushButton * m_pButton;

public:
	QString textForSearch() { return m_pLabel->text(); };
	QWidget * widgetToHighlight() { return (QWidget *)m_pLabel; };
	virtual void commit();
	QColor getColor() { return m_memColor; };
	void forceColor(QColor clr);

private:
	void setButtonPalette(QColor * pOption);
private slots:
	void changeClicked();
public slots:
	virtual void setEnabled(bool bEnabled);
};

class KVIRC_API KviFontSelector : public KviTalHBox, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviFontSelector(QWidget * par, const QString & txt, QFont * pOption, bool bEnabled);
	~KviFontSelector(){};

private:
	QLabel * m_pLabel;
	QFont * m_pOption;
	QPushButton * m_pButton;

public:
	virtual void commit();
	QString textForSearch() { return m_pLabel->text(); };
	QWidget * widgetToHighlight() { return (QWidget *)m_pLabel; };
private:
	void setButtonFont(QFont * pOption);
public slots:
	virtual void setEnabled(bool bEnabled);
private slots:
	void changeClicked();
};

class KVIRC_API KviStringListSelector : public KviTalVBox, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviStringListSelector(QWidget * par, const QString & txt, QStringList * pOption, bool bEnabled);
	~KviStringListSelector();

private:
	QLabel * m_pLabel;
	KviTalListWidget * m_pListWidget;
	QLineEdit * m_pLineEdit;
	QPushButton * m_pAddButton;
	QPushButton * m_pRemoveButton;
	QStringList * m_pOption;

public:
	virtual void commit();
	QString textForSearch() { return m_pLabel->text(); };
	QWidget * widgetToHighlight() { return (QWidget *)m_pLabel; };
public slots:
	virtual void setEnabled(bool bEnabled);
private slots:
	void textChanged(const QString & str);
	void itemSelectionChanged();
	void addClicked();
	void removeClicked();
};

class QMenu;

class KVIRC_API KviMircTextColorSelector : public KviTalHBox, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviMircTextColorSelector(QWidget * par, const QString & txt, unsigned int * uFore, unsigned int * uBack, bool bEnabled);
	~KviMircTextColorSelector();

private:
	QLabel * m_pLabel;
	QPushButton * m_pButton;
	unsigned int * m_pUFore;
	unsigned int * m_pUBack;
	unsigned int m_uFore;
	unsigned int m_uBack;
	QMenu * m_pContextPopup;
	QMenu * m_pForePopup;
	QMenu * m_pBackPopup;

public:
	virtual void commit();
	virtual void setEnabled(bool bEnabled);
protected slots:
	void buttonClicked();
	void foreSelected(QAction * pAction);
	void backSelected(QAction * pAction);

protected:
	void setButtonPalette();
};

class KVIRC_API KviSoundSelector : public KviFileSelector
{
	Q_OBJECT
public:
	KviSoundSelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled);
	~KviSoundSelector();

protected:
	QPushButton * m_pPlayButton;
private slots:
	void playSound();

public:
	virtual void setEnabled(bool bEnabled);
};

class KVIRC_API KviChanTreeViewItem : public QTreeWidgetItem
{
private:
	QString m_szPass;

public:
	KviChanTreeViewItem(QTreeWidget * pList, QString szChan, QString szPass);
	~KviChanTreeViewItem(){};

	const QString & pass() { return m_szPass; }
};

class KVIRC_API KviChannelListSelector : public KviTalVBox, public KviSelectorInterface
{
	Q_OBJECT
public:
	KviChannelListSelector(QWidget * par, const QString & txt, QStringList * pOption, bool bEnabled);
	~KviChannelListSelector();

private:
	QLabel * m_pLabel;
	QTreeWidget * m_pTreeWidget;
	QLineEdit * m_pChanLineEdit;
	QLineEdit * m_pPassLineEdit;
	QPushButton * m_pAddButton;
	QPushButton * m_pRemoveButton;
	QStringList * m_pOption;

public:
	virtual void commit();
	QString textForSearch() { return m_pLabel->text(); };
	QWidget * widgetToHighlight() { return (QWidget *)m_pLabel; };
	virtual void setEnabled(bool bEnabled);
private slots:
	void textChanged(const QString & str);
	void itemSelectionChanged();
	void addClicked();
	void removeClicked();
};

#endif //!_KVI_SELECTORS_H_
