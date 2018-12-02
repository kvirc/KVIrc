//=============================================================================
//
//   File : KviSelectors.cpp
//   Creation date : Mon Nov 13 2000 15:22:12 CEST by Szymon Stefanek
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

#define _KVI_SELECTORS_CPP_

#include "KviSelectors.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviControlCodes.h"
#include "KviFileDialog.h"
#include "KviKvsScript.h"

#include <QPainter>
#include <QLayout>
#include <QColorDialog>
#include <QGroupBox>
#include <QPalette>
#include <QFontDialog>
#include <QAbstractItemView>
#include <QMenu>

#include <algorithm>
#include <limits>

KviBoolSelector::KviBoolSelector(QWidget * par, const QString & txt, bool * pOption, bool bEnabled)
    : QCheckBox(txt, par), KviSelectorInterface()
{
	setEnabled(bEnabled);
	setChecked(*pOption);
	m_pOption = pOption;
}

void KviBoolSelector::setEnabled(bool bEnabled)
{
	QCheckBox::setEnabled(bEnabled);
}

void KviBoolSelector::setNotEnabled(bool bNotEnabled)
{
	setEnabled(!bNotEnabled);
}

void KviBoolSelector::commit()
{
	*m_pOption = isChecked();
}

KviUIntSelector::KviUIntSelector(QWidget * par, const QString & txt, unsigned int * pOption,
    unsigned int uLowBound, unsigned int uHighBound, unsigned int uDefault, bool bEnabled, bool bShortInt)
    : KviTalHBox(par), KviSelectorInterface()
{
	m_pLabel = new QLabel(txt, this);
	m_pSpinBox = new QSpinBox(this);

	m_bIsShortInt = bShortInt;

	setEnabled(bEnabled);

	m_pOption = pOption;

	// QSpinBox only holds integers and we want unsigned integers, so make sure the limits are correct
	if(m_bIsShortInt)
	{
		m_uLowBound = std::min({ uLowBound, static_cast<unsigned int>(std::numeric_limits<unsigned short int>::max()), static_cast<unsigned int>(std::numeric_limits<int>::max()) });
		m_uHighBound = std::min({ uHighBound, static_cast<unsigned int>(std::numeric_limits<unsigned short int>::max()), static_cast<unsigned int>(std::numeric_limits<int>::max()) });
	}
	else
	{
		m_uLowBound = std::min(uLowBound, static_cast<unsigned int>(std::numeric_limits<int>::max()));
		m_uHighBound = std::min(uHighBound, static_cast<unsigned int>(std::numeric_limits<int>::max()));
	}

	m_uDefault = uDefault;

	m_pSpinBox->setMinimum(m_uLowBound);
	m_pSpinBox->setMaximum(m_uHighBound);

	m_pSpinBox->setValue(bShortInt ? (unsigned int)*((unsigned short int *)pOption) : *pOption);

	setMargin(0);
	setStretchFactor(m_pLabel, 1);
}

void KviUIntSelector::setPrefix(const QString & txt)
{
	m_pSpinBox->setPrefix(txt);
}

void KviUIntSelector::setSuffix(const QString & txt)
{
	m_pSpinBox->setSuffix(txt);
}

void KviUIntSelector::commit()
{
	unsigned int val = (unsigned int)m_pSpinBox->value();

	if(m_bIsShortInt)
		*((unsigned short int *)m_pOption) = (unsigned short int)val;
	else
		*m_pOption = val;
}

void KviUIntSelector::setEnabled(bool bEnabled)
{
	KviTalHBox::setEnabled(bEnabled);
	m_pLabel->setEnabled(bEnabled);
	m_pSpinBox->setEnabled(bEnabled);
}

KviStringSelector::KviStringSelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled)
    : KviTalHBox(par), KviSelectorInterface()
{
	m_pLabel = new QLabel(txt, this);
	m_pLineEdit = new QLineEdit(this);
	QString tmp = *pOption;
	m_pLineEdit->setText(tmp);

	setSpacing(4);
	setMargin(0);
	setStretchFactor(m_pLineEdit, 1);

	m_pOption = pOption;

	setEnabled(bEnabled);
}

KviStringSelector::~KviStringSelector()
    = default;

void KviStringSelector::commit()
{
	QString tmp = m_pLineEdit->text();
	*m_pOption = tmp;
}

void KviStringSelector::setEnabled(bool bEnabled)
{
	KviTalHBox::setEnabled(bEnabled);
	m_pLineEdit->setEnabled(bEnabled);
	m_pLabel->setEnabled(bEnabled);
}

void KviStringSelector::setText(const QString & text)
{
	m_pLineEdit->setText(text);
}

KviPasswordSelector::KviPasswordSelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled)
    : KviTalHBox(par), KviSelectorInterface()
{
	setSpacing(0);
	setMargin(0);

	m_pLabel = new QLabel(txt, this);
	m_pLineEdit = new QLineEdit(this);
	addSpacing(4);
	m_pCheckBox = new QCheckBox(__tr2qs("Reveal password"), this);
	m_pLineEdit->setEchoMode(QLineEdit::Password);
	m_pCheckBox->setCheckState(Qt::Unchecked);
	connect(m_pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkToggled(int)));

	QString tmp = *pOption;
	m_pLineEdit->setText(tmp);

	setStretchFactor(m_pLineEdit, 1);

	m_pOption = pOption;

	setEnabled(bEnabled);
}

void KviPasswordSelector::checkToggled(int state)
{
	if(state == Qt::Unchecked)
		m_pLineEdit->setEchoMode(QLineEdit::Password);
	else
		m_pLineEdit->setEchoMode(QLineEdit::Normal);
}

KviPasswordSelector::~KviPasswordSelector()
    = default;

void KviPasswordSelector::commit()
{
	QString tmp = m_pLineEdit->text();
	*m_pOption = tmp;
}

void KviPasswordSelector::setEnabled(bool bEnabled)
{
	KviTalHBox::setEnabled(bEnabled);
	m_pLineEdit->setEnabled(bEnabled);
	m_pLabel->setEnabled(bEnabled);
	m_pCheckBox->setEnabled(bEnabled);
}

void KviPasswordSelector::setText(const QString & text)
{
	m_pLineEdit->setText(text);
}

KviPasswordLineEdit::KviPasswordLineEdit(QWidget * par)
    : KviTalHBox(par)
{
	setSpacing(0);
	setMargin(0);

	m_pLineEdit = new QLineEdit(this);
	addSpacing(4);
	m_pCheckBox = new QCheckBox(__tr2qs("Reveal password"), this);
	m_pLineEdit->setEchoMode(QLineEdit::Password);
	m_pCheckBox->setCheckState(Qt::Unchecked);
	connect(m_pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkToggled(int)));
	setStretchFactor(m_pLineEdit, 1);
}

void KviPasswordLineEdit::checkToggled(int state)
{
	if(state == Qt::Unchecked)
		m_pLineEdit->setEchoMode(QLineEdit::Password);
	else
		m_pLineEdit->setEchoMode(QLineEdit::Normal);
}

KviPasswordLineEdit::~KviPasswordLineEdit()
    = default;

QString KviPasswordLineEdit::text()
{
	return m_pLineEdit->text();
}

void KviPasswordLineEdit::setEnabled(bool bEnabled)
{
	KviTalHBox::setEnabled(bEnabled);
	m_pLineEdit->setEnabled(bEnabled);
	m_pCheckBox->setEnabled(bEnabled);
}

void KviPasswordLineEdit::setText(const QString & text)
{
	m_pLineEdit->setText(text);
}

KviPixmapPreview::KviPixmapPreview(QWidget * par)
    : QScrollArea(par)
{
	m_pLabPixmap = new QLabel(this);
	setWidget(m_pLabPixmap);
	setBackgroundRole(QPalette::Dark);
}

KviPixmapPreview::~KviPixmapPreview()
    = default;

void KviPixmapPreview::setPixmap(KviPixmap * pix)
{
	if(!pix->isNull())
	{
		m_pLabPixmap->resize(pix->pixmap()->size());
		m_pLabPixmap->setPixmap(*pix->pixmap());
	}
}

KviPixmapSelector::KviPixmapSelector(QWidget * par, const QString & txt, KviPixmap * pOption, bool bEnabled)
    : QWidget(par), KviSelectorInterface()
{
	QGridLayout * g = new QGridLayout(this);
	m_pOption = pOption;

	m_localPixmap = *pOption;

	m_pCheckBox = new QCheckBox(txt, this);
	m_pCheckBox->setChecked(m_localPixmap.pixmap());
	connect(m_pCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxToggled(bool)));

	g->addWidget(m_pCheckBox, 0, 0, 1, 2);

	m_pPreview = new KviPixmapPreview(this);
	m_pPreview->setPixmap(&m_localPixmap);
	g->addWidget(m_pPreview, 1, 0, 1, 2);

	m_pFileNameLabel = new QLabel(this);
	m_pFileNameLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	if(m_localPixmap.pixmap())
		m_pFileNameLabel->setText(m_localPixmap.path());
	g->addWidget(m_pFileNameLabel, 2, 0);

	m_pChooseButton = new QPushButton(__tr2qs("&Browse..."), this);
	g->addWidget(m_pChooseButton, 2, 1);
	connect(m_pChooseButton, SIGNAL(clicked()), this, SLOT(choosePixmap()));

	g->setRowStretch(1, 1);
	g->setColumnStretch(0, 1);
	g->setMargin(0);

	setEnabled(bEnabled);
}

KviPixmapSelector::~KviPixmapSelector()
    = default;

void KviPixmapSelector::checkBoxToggled(bool)
{
	setEnabled(isEnabled());
}

void KviPixmapSelector::commit()
{
	if(m_pCheckBox->isChecked())
		*m_pOption = m_localPixmap;
	else
		*m_pOption = KviPixmap(); // null pixmap
}

void KviPixmapSelector::choosePixmap()
{
	QString tmp;

	if(KviFileDialog::askForOpenFileName(tmp, __tr2qs("Select a File - KVIrc"), QString(), QString(), false, true, this))
		setImagePath(tmp);
}

void KviPixmapSelector::setImagePath(const QString & path)
{
	m_localPixmap.load(path);
	m_pPreview->setPixmap(&m_localPixmap);

	if(m_localPixmap.isNull())
	{
		QString tmp = __tr2qs("Unloadable");
		tmp += ": ";
		tmp += path;
		m_pFileNameLabel->setText(tmp);
		m_pCheckBox->setChecked(false);
	}
	else
	{
		m_pCheckBox->setChecked(true);
		m_pFileNameLabel->setText(path);
	}

	setEnabled(isEnabled());
}

void KviPixmapSelector::setEnabled(bool bEnabled)
{
	QWidget::setEnabled(bEnabled);

	m_pCheckBox->setEnabled(bEnabled);
	m_pPreview->setEnabled(bEnabled && m_pCheckBox->isChecked());
	m_pFileNameLabel->setEnabled(bEnabled && m_pCheckBox->isChecked());
	m_pChooseButton->setEnabled(bEnabled && m_pCheckBox->isChecked());
}

// FIXME: #warning "Option for DIR_MUST_EXISTS...(this widget could be turned into a file selector too)"
KviFileSelector::KviFileSelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled, unsigned int uFlags, const QString & szFilter)
    : QWidget(par), KviSelectorInterface()
{
	m_Layout = new QGridLayout(this);

	m_uFlags = uFlags;
	m_szFilter = szFilter;
	m_pLabel = new QLabel(txt, this);

	if(uFlags & VerticalLayout)
		m_Layout->addWidget(m_pLabel, 0, 0, 1, 2);
	else
		m_Layout->addWidget(m_pLabel, 0, 0);

	m_pLineEdit = new QLineEdit(this);

	if(uFlags & VerticalLayout)
		m_Layout->addWidget(m_pLineEdit, 1, 0);
	else
		m_Layout->addWidget(m_pLineEdit, 0, 1);

	m_pLineEdit->setText(*pOption);

	m_pButton = new QPushButton(__tr2qs("&Browse..."), this);
	connect(m_pButton, SIGNAL(clicked()), this, SLOT(browseClicked()));

	if(uFlags & VerticalLayout)
		m_Layout->addWidget(m_pButton, 1, 1);
	else
		m_Layout->addWidget(m_pButton, 0, 2);

	m_Layout->setSpacing(4);

	if(uFlags & VerticalLayout)
		m_Layout->setColumnStretch(0, 1);
	else
		m_Layout->setColumnStretch(1, 1);

	m_pOption = pOption;

	m_Layout->setMargin(0);
	setEnabled(bEnabled);
}

void KviFileSelector::commit()
{
	*m_pOption = m_pLineEdit->text();
}

void KviFileSelector::setEnabled(bool bEnabled)
{
	QWidget::setEnabled(bEnabled);
	m_pLineEdit->setEnabled(bEnabled);
	m_pLabel->setEnabled(bEnabled);
	m_pButton->setEnabled(bEnabled);
}

void KviFileSelector::browseClicked()
{
	select();
}

void KviFileSelector::setSelection(const QString & szSelection)
{
	m_pLineEdit->setText(szSelection);
}

void KviFileSelector::select()
{
	//KviCString tmp;
	QString tmp = *m_pOption;
	if(m_uFlags & ChooseSaveFileName)
	{
		if(KviFileDialog::askForSaveFileName(tmp, __tr2qs("Choose a File - KVIrc"), tmp, m_szFilter, true,
		       !(m_uFlags & DontConfirmOverwrite), true, this))
		{
			m_pLineEdit->setText(tmp);
			emit selectionChanged(tmp);
		}
	}
	else
	{
		if(KviFileDialog::askForOpenFileName(tmp, __tr2qs("Choose a File - KVIrc"), tmp, m_szFilter, true, true, this))
		{
			m_pLineEdit->setText(tmp);
			emit selectionChanged(tmp);
		}
	}
}

KviDirectorySelector::KviDirectorySelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled, unsigned int uFlags)
    : KviFileSelector(par, txt, pOption, bEnabled, uFlags)
{
}

void KviDirectorySelector::select()
{
	QString szTmp;
	if(KviFileDialog::askForDirectoryName(szTmp, __tr2qs("Choose a Directory - KVIrc"), QString(), QString(), false, true, this))
	{
		m_pLineEdit->setText(szTmp);
		emit selectionChanged(szTmp);
	}
}

KviStringListSelector::KviStringListSelector(QWidget * par, const QString & txt, QStringList * pOption, bool bEnabled)
    : KviTalVBox(par), KviSelectorInterface()
{
	m_pLabel = new QLabel(txt, this);
	m_pListWidget = new KviTalListWidget(this);
	m_pLineEdit = new QLineEdit(this);
	connect(m_pLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
	connect(m_pLineEdit, SIGNAL(returnPressed()), this, SLOT(addClicked()));

	KviTalHBox * hBox = new KviTalHBox(this);

	m_pAddButton = new QPushButton(__tr2qs("A&dd"), hBox);
	connect(m_pAddButton, SIGNAL(clicked()), this, SLOT(addClicked()));
	m_pRemoveButton = new QPushButton(__tr2qs("Re&move"), hBox);
	connect(m_pRemoveButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
	m_pOption = pOption;
	m_pListWidget->addItems(*pOption);
	m_pListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	connect(m_pListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

	setSpacing(4);
	setStretchFactor(m_pListWidget, 1);
	setEnabled(bEnabled);
}

KviStringListSelector::~KviStringListSelector()
    = default;

void KviStringListSelector::itemSelectionChanged()
{
	bool bSomeSelected;
	if(m_pListWidget->selectedItems().count())
		bSomeSelected = true;
	else
		bSomeSelected = false;

	m_pRemoveButton->setEnabled(isEnabled() && bSomeSelected);
}

void KviStringListSelector::textChanged(const QString & str)
{
	m_pAddButton->setEnabled((str.trimmed().length() > 0) && isEnabled());
}

void KviStringListSelector::setEnabled(bool bEnabled)
{
	KviTalVBox::setEnabled(bEnabled);
	m_pLineEdit->setEnabled(bEnabled);
	m_pLabel->setEnabled(bEnabled);
	QString txt = m_pLineEdit->text();
	txt = txt.trimmed();
	m_pAddButton->setEnabled(bEnabled && (txt.length() > 0));

	bool bSomeSelected;
	if(m_pListWidget->selectedItems().count())
		bSomeSelected = true;
	else
		bSomeSelected = false;

	m_pRemoveButton->setEnabled(bEnabled && bSomeSelected);
	m_pListWidget->setEnabled(bEnabled);
}

void KviStringListSelector::commit()
{
	unsigned int uCount = m_pListWidget->count();
	m_pOption->clear();
	for(unsigned int u = 0; u < uCount; u++)
	{
		QString str = m_pListWidget->item(u)->text();
		str = str.trimmed();
		if(str.length() > 0)
			m_pOption->append(str);
	}
}

void KviStringListSelector::addClicked()
{
	QString str = m_pLineEdit->text();
	str = str.trimmed();
	if(str.length() > 0)
		m_pListWidget->insertItem(m_pListWidget->count(), str);
	m_pLineEdit->setText("");
}

void KviStringListSelector::removeClicked()
{
	int u = 0;
	while(u < m_pListWidget->count())
	{
		if(m_pListWidget->item(u)->isSelected())
		{
			m_pListWidget->takeItem(u);
			continue;
		}
		u++;
	}
}

KviColorSelector::KviColorSelector(QWidget * par, const QString & txt, QColor * pOption, bool bEnabled)
    : KviTalHBox(par), KviSelectorInterface()
{
	m_pLabel = new QLabel(txt, this);

	m_pButton = new QPushButton(" ", this);
	connect(m_pButton, SIGNAL(clicked()), this, SLOT(changeClicked()));

	setSpacing(4);
	setMargin(0);
	setStretchFactor(m_pLabel, 1);

	setButtonPalette(pOption);

	m_pOption = pOption;

	setEnabled(bEnabled);
}

void KviColorSelector::setButtonPalette(QColor * pOption)
{
	QPalette pal(*pOption, palette().window().color());
	m_memColor = *pOption;
	m_pButton->setPalette(pal);

	QPixmap pix(16, 16);
	pix.fill(*pOption);
	m_pButton->setIcon(pix);
}

void KviColorSelector::forceColor(QColor clr)
{
	setButtonPalette(&clr);
}

void KviColorSelector::changeClicked()
{
	QColor tmp = QColorDialog::getColor(m_memColor, nullptr, __tr2qs("Choose Color"), QColorDialog::ShowAlphaChannel);
	if(tmp.isValid())
		setButtonPalette(&tmp);
}

void KviColorSelector::commit()
{
	*m_pOption = m_memColor;
}

void KviColorSelector::setEnabled(bool bEnabled)
{
	KviTalHBox::setEnabled(bEnabled);
	m_pLabel->setEnabled(bEnabled);
	m_pButton->setEnabled(bEnabled);
}

KviFontSelector::KviFontSelector(QWidget * par, const QString & txt, QFont * pOption, bool bEnabled)
    : KviTalHBox(par), KviSelectorInterface()
{
	m_pLabel = new QLabel(txt, this);

	m_pButton = new QPushButton("", this);
	connect(m_pButton, SIGNAL(clicked()), this, SLOT(changeClicked()));

	setSpacing(4);
	setMargin(0);
	setStretchFactor(m_pLabel, 1);

	setButtonFont(pOption);
	m_pOption = pOption;

	setEnabled(bEnabled);
}

void KviFontSelector::setButtonFont(QFont * pOption)
{
	m_pButton->setText(pOption->family());
	m_pButton->setFont(*pOption);
}

void KviFontSelector::changeClicked()
{
	bool bOk;
	QFont tmp = QFontDialog::getFont(&bOk, m_pButton->font());
	if(bOk)
		setButtonFont(&tmp);
}

void KviFontSelector::commit()
{
	*m_pOption = m_pButton->font();
}

void KviFontSelector::setEnabled(bool bEnabled)
{
	KviTalHBox::setEnabled(bEnabled);
	m_pLabel->setEnabled(bEnabled);
	m_pButton->setEnabled(bEnabled);
}

KviMircTextColorSelector::KviMircTextColorSelector(QWidget * par, const QString & txt, unsigned int * uFore, unsigned int * uBack, bool bEnabled)
    : KviTalHBox(par), KviSelectorInterface()
{
	m_pLabel = new QLabel(txt, this);

	m_pButton = new QPushButton(__tr2qs("Sample Text"), this);
	connect(m_pButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

	setSpacing(4);
	setMargin(0);
	setStretchFactor(m_pLabel, 1);

	m_pUFore = uFore;
	m_pUBack = uBack;

	m_uBack = *uBack;
	m_uFore = *uFore;

	setButtonPalette();

	setEnabled(bEnabled);

	m_pContextPopup = new QMenu(this);

	QAction * pAction = nullptr;
	m_pForePopup = new QMenu(this);
	connect(m_pForePopup, SIGNAL(triggered(QAction *)), this, SLOT(foreSelected(QAction *)));
	int iColor;
	for(iColor = 0; iColor < KVI_MIRCCOLOR_MAX; iColor++)
	{
		QPixmap tmp(120, 16);
		tmp.fill(KVI_OPTION_MIRCCOLOR(iColor));
		pAction = m_pForePopup->addAction(tmp, QString("x"));
		pAction->setData(iColor);
	}

	pAction = m_pContextPopup->addAction(__tr2qs("Foreground"));
	pAction->setMenu(m_pForePopup);

	m_pBackPopup = new QMenu(this);
	connect(m_pBackPopup, SIGNAL(triggered(QAction *)), this, SLOT(backSelected(QAction *)));
	pAction = m_pBackPopup->addAction(__tr2qs("Transparent"));
	pAction->setData(KviControlCodes::Transparent);
	for(iColor = 0; iColor < KVI_MIRCCOLOR_MAX; iColor++)
	{
		QPixmap tmp(120, 16);
		tmp.fill(KVI_OPTION_MIRCCOLOR(iColor));
		pAction = m_pBackPopup->addAction(tmp, QString("x"));
		pAction->setData(iColor);
	}

	pAction = m_pContextPopup->addAction(__tr2qs("Background"));
	pAction->setMenu(m_pBackPopup);
}

KviMircTextColorSelector::~KviMircTextColorSelector()
    = default;

void KviMircTextColorSelector::commit()
{
	*m_pUFore = m_uFore;
	*m_pUBack = m_uBack;
}

void KviMircTextColorSelector::setEnabled(bool bEnabled)
{
	KviTalHBox::setEnabled(bEnabled);
	m_pLabel->setEnabled(bEnabled);
	m_pButton->setEnabled(bEnabled);
}

void KviMircTextColorSelector::setButtonPalette()
{
	QPalette pal;

	if(m_uBack > KVI_MIRCCOLOR_MAX)
	{
		if(m_uBack != KviControlCodes::Transparent)
			m_uBack = KviControlCodes::Transparent;
		pal = palette();
	}
	else
	{
		pal = QPalette(KVI_OPTION_MIRCCOLOR(m_uBack));
	}

	if(m_uFore > KVI_MIRCCOLOR_MAX)
		m_uFore = KVI_MIRCCOLOR_MAX;

	pal.setColor(QPalette::ButtonText, KVI_OPTION_MIRCCOLOR(m_uFore));
	pal.setColor(QPalette::Text, KVI_OPTION_MIRCCOLOR(m_uFore));

	m_pButton->setPalette(pal);
}

void KviMircTextColorSelector::buttonClicked()
{
	QPoint p = m_pButton->mapToGlobal(QPoint(0, m_pButton->height()));
	m_pContextPopup->popup(p);
}

void KviMircTextColorSelector::foreSelected(QAction * pAction)
{
	bool bOk = false;
	int id = pAction->data().toInt(&bOk);
	if(bOk)
		m_uFore = id;
	setButtonPalette();
}

void KviMircTextColorSelector::backSelected(QAction * pAction)
{
	bool bOk = false;
	int id = pAction->data().toInt(&bOk);
	if(bOk)
		m_uBack = id;
	setButtonPalette();
}

KviSoundSelector::KviSoundSelector(QWidget * par, const QString & txt, QString * pOption, bool bEnabled)
    : KviFileSelector(par, txt, pOption, bEnabled)
{
	m_pPlayButton = new QPushButton(__tr2qs("Play"), this);
	connect(m_pPlayButton, SIGNAL(clicked()), this, SLOT(playSound()));
	//parent layout
	if(m_uFlags & VerticalLayout)
		m_Layout->addWidget(m_pPlayButton, 2, 1);
	else
		m_Layout->addWidget(m_pPlayButton, 0, 3);
}

KviSoundSelector::~KviSoundSelector()
    = default;

void KviSoundSelector::playSound()
{
	KviKvsVariantList soundParams{new KviKvsVariant{m_pLineEdit->text()}};
	KviKvsScript::run("snd.play $0", nullptr, &soundParams);
}

void KviSoundSelector::setEnabled(bool bEnabled)
{
	KviFileSelector::setEnabled(bEnabled);
	m_pPlayButton->setEnabled(bEnabled);
}

KviChanTreeViewItem::KviChanTreeViewItem(QTreeWidget * pList, QString szChan, QString szPass)
    : QTreeWidgetItem(pList)
{
	m_szPass = szPass;
	QString mask;
	mask.fill('*', szPass.length());
	setText(0, szChan);
	setText(1, mask);
}

KviChannelListSelector::KviChannelListSelector(QWidget * par, const QString & txt, QStringList * pOption, bool bEnabled)
    : KviTalVBox(par), KviSelectorInterface()
{
	m_pLabel = new QLabel(txt, this);
	m_pTreeWidget = new QTreeWidget(this);
	m_pTreeWidget->setRootIsDecorated(false);
	m_pTreeWidget->setColumnCount(2);
	QStringList columnLabels;
	columnLabels.append(__tr2qs("Channel Name"));
	columnLabels.append(__tr2qs("Channel Password"));
	m_pTreeWidget->setHeaderLabels(columnLabels);
	m_pTreeWidget->setColumnWidth(0, 200);
	m_pTreeWidget->setColumnWidth(1, 200);

	QGroupBox * pEditsGroupBox = new QGroupBox(__tr2qs("Add channel"), this);
	QGridLayout * pChannelLayout = new QGridLayout(pEditsGroupBox);

	QString szMsg = __tr2qs("Name");
	szMsg.append(":");

	QLabel * pNameLabel = new QLabel(szMsg, pEditsGroupBox);
	pChannelLayout->addWidget(pNameLabel, 1, 0);

	m_pChanLineEdit = new QLineEdit(pEditsGroupBox);
	connect(m_pChanLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
	connect(m_pChanLineEdit, SIGNAL(returnPressed()), this, SLOT(addClicked()));
	pChannelLayout->addWidget(m_pChanLineEdit, 1, 1);

	szMsg = __tr2qs("Password");
	szMsg.append(":");

	QLabel * pPasswordLabel = new QLabel(szMsg, pEditsGroupBox);
	pChannelLayout->addWidget(pPasswordLabel, 2, 0);

	m_pPassLineEdit = new QLineEdit(pEditsGroupBox);
	m_pPassLineEdit->setEchoMode(QLineEdit::Password);
	connect(m_pPassLineEdit, SIGNAL(returnPressed()), this, SLOT(addClicked()));
	pChannelLayout->addWidget(m_pPassLineEdit, 2, 1);

	KviTalHBox * hBox = new KviTalHBox(this);

	m_pAddButton = new QPushButton(__tr2qs("A&dd"), hBox);
	connect(m_pAddButton, SIGNAL(clicked()), this, SLOT(addClicked()));
	m_pRemoveButton = new QPushButton(__tr2qs("Re&move"), hBox);
	connect(m_pRemoveButton, SIGNAL(clicked()), this, SLOT(removeClicked()));

	m_pOption = pOption;
	for(const auto & it : *pOption)
		new KviChanTreeViewItem(m_pTreeWidget, it.section(':', 0, 0), it.section(':', 1));

	m_pTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_pTreeWidget->setAllColumnsShowFocus(true);
	connect(m_pTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
	setSpacing(4);
	setStretchFactor(m_pTreeWidget, 1);
	setEnabled(bEnabled);

	// Default disable the add and remove buttons
	m_pAddButton->setEnabled(false);
	m_pRemoveButton->setEnabled(false);
}

KviChannelListSelector::~KviChannelListSelector()
    = default;

void KviChannelListSelector::commit()
{
	m_pOption->clear();
	KviChanTreeViewItem * pItem;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		pItem = (KviChanTreeViewItem *)m_pTreeWidget->topLevelItem(i);
		m_pOption->append(pItem->text(0) + ":" + pItem->pass());
	}
}

void KviChannelListSelector::setEnabled(bool bEnabled)
{
	m_pLabel->setEnabled(bEnabled);
	m_pTreeWidget->setEnabled(bEnabled);
	m_pChanLineEdit->setEnabled(bEnabled);
	m_pPassLineEdit->setEnabled(bEnabled);
	m_pAddButton->setEnabled(bEnabled);
	m_pRemoveButton->setEnabled(bEnabled);
}

void KviChannelListSelector::textChanged(const QString &)
{
	m_pAddButton->setEnabled(!m_pChanLineEdit->text().isEmpty());
}

void KviChannelListSelector::itemSelectionChanged()
{
	m_pRemoveButton->setEnabled(m_pTreeWidget->selectedItems().size() > 0);
}

void KviChannelListSelector::addClicked()
{
	if(!m_pChanLineEdit->text().isEmpty())
	{
		new KviChanTreeViewItem(m_pTreeWidget, m_pChanLineEdit->text().trimmed(), m_pPassLineEdit->text().trimmed());
		m_pChanLineEdit->clear();
		m_pPassLineEdit->clear();
	}
}

void KviChannelListSelector::removeClicked()
{
	KviPointerList<QTreeWidgetItem> lst;
	QList<QTreeWidgetItem *> items = m_pTreeWidget->selectedItems();
	for(int i = 0; i < items.count(); i++)
	{
		lst.append((QTreeWidgetItem *)items.at(i));
	}

	lst.setAutoDelete(true);
	lst.clear();
}
