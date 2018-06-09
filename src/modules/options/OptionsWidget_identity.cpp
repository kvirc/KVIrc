//============================================================================
//
//   File : OptionsWidget_identity.cpp
//   Creation date : Tue Nov 14 2000 23:06:53 CEST by Szymon Stefanek
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
//============================================================================

#include "OptionsWidget_identity.h"

#include "kvi_defaults.h"
#include "kvi_settings.h"
#include "kvi_fileextensions.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviWindow.h"
#include "KviConsoleWindow.h"
#include "KviOptionsWidget.h"
#include "KviFileDialog.h"
#include "KviIconManager.h"
#include "KviHttpRequest.h"
#include "KviIdentityProfileSet.h"
#include "KviTalToolTip.h"
#include "KviTalHBox.h"

#include <QTreeWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>
#include <QValidator>
#include <QCloseEvent>
#include <QFileInfo>

NickAlternativesDialog::NickAlternativesDialog(QWidget * par, const QString & n1, const QString & n2, const QString & n3)
    : QDialog(par)
{
	QGridLayout * g = new QGridLayout(this);

	setWindowTitle(__tr2qs_ctx("Nickname Alternatives - KVIrc", "options"));

	QLabel * l = new QLabel(this);
	l->setText(__tr2qs_ctx("Here you can choose up to three nickname "
	                       "alternatives to the primary one. KVIrc will use the alternatives "
	                       "if the primary nick is already used by someone else on a particular "
	                       "IRC network.",
	    "options"));
	l->setWordWrap(true);

	g->addWidget(l, 0, 0, 1, 3);

	l = new QLabel(this);
	l->setText(__tr2qs_ctx("Alt. nickname 1:", "options"));
	g->addWidget(l, 1, 0);
	m_pNickEdit1 = new QLineEdit(this);
	g->addWidget(m_pNickEdit1, 1, 1, 1, 2);
	m_pNickEdit1->setText(n1);

	l = new QLabel(this);
	l->setText(__tr2qs_ctx("Alt. nickname 2:", "options"));
	g->addWidget(l, 2, 0);
	m_pNickEdit2 = new QLineEdit(this);
	g->addWidget(m_pNickEdit2, 2, 1, 1, 2);
	m_pNickEdit2->setText(n2);

	l = new QLabel(this);
	l->setText(__tr2qs_ctx("Alt. nickname 3:", "options"));
	g->addWidget(l, 3, 0);
	m_pNickEdit3 = new QLineEdit(this);
	g->addWidget(m_pNickEdit3, 3, 1, 1, 2);
	m_pNickEdit3->setText(n3);
	KviTalHBox * h = new KviTalHBox(this);
	h->setSpacing(8);
	g->addWidget(h, 4, 2);

	QPushButton * pb = new QPushButton(__tr2qs_ctx("Cancel", "options"), h);
	connect(pb, SIGNAL(clicked()), this, SLOT(reject()));

	pb = new QPushButton(__tr2qs_ctx("OK", "options"), h);
	pb->setDefault(true);
	connect(pb, SIGNAL(clicked()), this, SLOT(accept()));

	g->setColumnStretch(0, 1);
}

NickAlternativesDialog::~NickAlternativesDialog()
    = default;

void NickAlternativesDialog::fill(QString & n1, QString & n2, QString & n3)
{
	n1 = m_pNickEdit1->text();
	n2 = m_pNickEdit2->text();
	n3 = m_pNickEdit3->text();
}

AvatarDownloadDialog::AvatarDownloadDialog(QWidget * par, const QString & szUrl)
    : QDialog(par)
{
	setWindowTitle(__tr2qs_ctx("Avatar Download - KVIrc", "options"));

	m_szUrl = szUrl;

	QGridLayout * g = new QGridLayout(this);

	m_pOutput = new QLabel(__tr2qs_ctx("Please wait while the avatar is being downloaded", "options"), this);
	g->addWidget(m_pOutput, 0, 0, 1, 2);

	QPushButton * b = new QPushButton(__tr2qs_ctx("Abort", "options"), this);
	g->addWidget(b, 1, 1);
	connect(b, SIGNAL(clicked()), this, SLOT(cancelClicked()));

	m_pRequest = new KviHttpRequest();

	QTimer::singleShot(0, this, SLOT(startDownload()));

	g->setRowStretch(0, 1);
	g->setColumnStretch(0, 1);

	setMinimumSize(250, 120);
}

AvatarDownloadDialog::~AvatarDownloadDialog()
{
	delete m_pRequest;
}

void AvatarDownloadDialog::startDownload()
{
	connect(m_pRequest, SIGNAL(terminated(bool)), this, SLOT(downloadTerminated(bool)));
	connect(m_pRequest, SIGNAL(status(const QString &)), this, SLOT(downloadMessage(const QString &)));

	QString tmp = m_szUrl;
	g_pIconManager->urlToCachedFileName(tmp);
	g_pApp->getLocalKvircDirectory(m_szLocalFileName, KviApplication::Avatars, tmp);

	m_pRequest->setExistingFileAction(KviHttpRequest::RenameExisting);
	if(!m_pRequest->get(KviUrl(m_szUrl), KviHttpRequest::StoreToFile, m_szLocalFileName.toUtf8().data()))
	{
		m_szErrorMessage = __tr2qs_ctx("Failed to start the download", "options");
		reject();
	}
}

void AvatarDownloadDialog::closeEvent(QCloseEvent * e)
{
	m_szErrorMessage = __tr2qs_ctx("Download aborted by user", "options");
	e->ignore();
	reject();
}

void AvatarDownloadDialog::cancelClicked()
{
	m_szErrorMessage = __tr2qs_ctx("Download aborted by user", "options");
	reject();
}

void AvatarDownloadDialog::downloadMessage(const QString & szMsg)
{
	if(!szMsg.isEmpty())
	{
		QString txt = szMsg;
		m_pOutput->setText(szMsg);
	}
}

void AvatarDownloadDialog::downloadTerminated(bool bSuccess)
{
	if(bSuccess)
	{
		accept();
	}
	else
	{
		m_szErrorMessage = m_pRequest->lastError();
		reject();
	}
}

AvatarSelectionDialog::AvatarSelectionDialog(QWidget * par, const QString & szInitialPath)
    : QDialog(par)
{
	setWindowTitle(__tr2qs_ctx("Choose Avatar - KVIrc", "options"));

	QVBoxLayout * layout = new QVBoxLayout(this);

	QString msg = __tr2qs_ctx("Please select an avatar image.\n\n"
	                          "The full path to a local file or an image on the Web can be used.\n"
	                          "If you wish to use a local image file, click the \"Browse\" button to select the desired file.\n\n"
	                          "The full URL for an image (including http:// or https://) can also be entered manually.",
	    "options");

	QLabel * l = new QLabel(msg);
	l->setMinimumWidth(250);
	l->setWordWrap(true);

	layout->addWidget(l);

	m_pLineEdit = new QLineEdit;
	m_pLineEdit->setText(szInitialPath);

	m_pLineEdit->setMinimumWidth(450);

	QHBoxLayout * pLineEditLayout = new QHBoxLayout;

	pLineEditLayout->addWidget(m_pLineEdit, 1);

	QPushButton * b = new QPushButton(__tr2qs_ctx("&Browse...", "options"));
	b->setFixedWidth(80);
	connect(b, SIGNAL(clicked()), this, SLOT(chooseFileClicked()));
	pLineEditLayout->addWidget(b, 1);

	layout->addLayout(pLineEditLayout);

	QHBoxLayout * h = new QHBoxLayout;

	h->setAlignment(Qt::AlignRight);
	layout->addLayout(h);
	b = new QPushButton(__tr2qs_ctx("&OK", "options"));
	b->setFixedWidth(80);
	b->setDefault(true);
	connect(b, SIGNAL(clicked()), this, SLOT(okClicked()));
	h->addWidget(b);

	b = new QPushButton(__tr2qs_ctx("Cancel", "options"));
	b->setFixedWidth(80);
	connect(b, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	h->addWidget(b);
}

AvatarSelectionDialog::~AvatarSelectionDialog()
    = default;

void AvatarSelectionDialog::okClicked()
{
	m_szAvatarName = m_pLineEdit->text().trimmed();
	accept();
}

void AvatarSelectionDialog::cancelClicked()
{
	reject();
}

void AvatarSelectionDialog::chooseFileClicked()
{
	QString tmp;
	if(KviFileDialog::askForOpenFileName(tmp, __tr2qs_ctx("Select a File - KVIrc", "options"), QString(), KVI_FILTER_IMAGE, false, true, this))
	{
		m_pLineEdit->setText(tmp);
	}
}

void AvatarSelectionDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	reject();
}

OptionsWidget_identity::OptionsWidget_identity(QWidget * parent)
    : KviOptionsWidget(parent)
{
}

OptionsWidget_identity::~OptionsWidget_identity()
    = default;

KviIdentityGeneralOptionsWidget::KviIdentityGeneralOptionsWidget(QWidget * parent)
    : KviOptionsWidget(parent)
{
	m_szAltNicknames[0] = KVI_OPTION_STRING(KviOption_stringNickname2);
	m_szAltNicknames[1] = KVI_OPTION_STRING(KviOption_stringNickname3);
	m_szAltNicknames[2] = KVI_OPTION_STRING(KviOption_stringNickname4);

	createLayout();
	layout()->setMargin(10);

	KviTalGroupBox * gbox = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("Basic Properties", "options"));
	KviTalHBox * hb = new KviTalHBox(gbox);

	KviStringSelector * sel = addStringSelector(hb, __tr2qs_ctx("Nickname:", "options"), KviOption_stringNickname1);
	sel->setMinimumLabelWidth(120);
	mergeTip(sel, __tr2qs_ctx("Your <b>nickname</b> is your primary form of identification on IRC.<br>"
	                          "Since servers cannot accept multiple users sharing the same nickname "
	                          "(case insensitive), you can provide alternative nicknames to be used in case "
	                          "the server refuses to accept the default one.",
	                  "options"));

	QValidator * v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*"), hb);
	sel->setValidator(v);

	QPushButton * pb = new QPushButton(__tr2qs_ctx("Alternatives...", "options"), hb);
	connect(pb, SIGNAL(clicked()), this, SLOT(setNickAlternatives()));

	sel = addStringSelector(gbox, __tr2qs_ctx("Username:", "options"), KviOption_stringUsername);
	sel->setMinimumLabelWidth(120);
	mergeTip(sel, __tr2qs_ctx("This is the <b>username</b> that you will use to connect to the server.<br>"
	                          "In the past, it was used as a form of authentication, but it normally has no special use now.<br>"
	                          "In addition to your nickname, you are identified on IRC by your <b>username@hostname</b>.<br>"
	                          "Basically, you can enter any word you like here.",
	                  "options"));

	sel = addStringSelector(gbox, __tr2qs_ctx("Real name:", "options"), KviOption_stringRealname);
	sel->setMinimumLabelWidth(120);

	mergeTip(sel, __tr2qs_ctx("This text will appear when someone does a /WHOIS on you.<br>"
	                          "It is intended to be your real name, but people tend to put random quotes and phrases here too.",
	                  "options"));

	QString szOptionalCtcpUserInfo = __tr2qs_ctx("This field is optional and will appear as part of the CTCP USERINFO reply.", "options");
	QString szTrailing = "<br><br>" + szOptionalCtcpUserInfo;

	gbox = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("Profile", "options"));

	hb = new KviTalHBox(gbox);

	QLabel * l = new QLabel(__tr2qs_ctx("Age:", "options"), hb);
	l->setMinimumWidth(120);

	m_pAgeCombo = new QComboBox(hb);
	QString szTip1 = __tr2qs_ctx("Here you can specify your age.", "options") + szTrailing;
	KviTalToolTip::add(l, szTip1);
	KviTalToolTip::add(m_pAgeCombo, szTip1);
	m_pAgeCombo->addItem(__tr2qs_ctx("Unspecified", "options"));
	unsigned int i;
	for(i = 1; i < 120; i++)
	{
		QString tmp;
		tmp.setNum(i);
		m_pAgeCombo->insertItem(m_pAgeCombo->count(), tmp);
	}

	bool bOk;
	i = KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge).toUInt(&bOk);
	if(!bOk)
		i = 0;
	if(i > 120)
		i = 120;
	m_pAgeCombo->setCurrentIndex(i);

	hb->setStretchFactor(m_pAgeCombo, 1);

	hb = new KviTalHBox(gbox);

	l = new QLabel(__tr2qs_ctx("Gender:", "options"), hb);
	l->setMinimumWidth(120);

	m_pGenderCombo = new QComboBox(hb);
	QString szTip2 = __tr2qs_ctx("Here you can specify your gender.", "options") + szTrailing;
	KviTalToolTip::add(l, szTip2);
	KviTalToolTip::add(m_pGenderCombo, szTip2);
	m_pGenderCombo->addItem(__tr2qs_ctx("Unspecified", "options"));
	m_pGenderCombo->addItem(__tr2qs_ctx("Female", "options"));
	m_pGenderCombo->addItem(__tr2qs_ctx("Male", "options"));

	if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender), "Male"))
		m_pGenderCombo->setCurrentIndex(2);
	else if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender), "Female"))
		m_pGenderCombo->setCurrentIndex(1);
	else
		m_pGenderCombo->setCurrentIndex(0);

	hb->setStretchFactor(m_pGenderCombo, 1);

	sel = addStringSelector(gbox, __tr2qs_ctx("Location:", "options"), KviOption_stringCtcpUserInfoLocation);
	sel->setMinimumLabelWidth(120);
	mergeTip(sel, __tr2qs_ctx("You can describe here your approximate physical location. "
	                          "Something like \"Region, Country\" will be OK. Please note that this information will be viewable "
	                          "by anyone so putting more data (like the exact address), generally, <b>is not a good idea</b>.",
	                  "options")
	        + szTrailing);

	sel = addStringSelector(gbox, __tr2qs_ctx("Languages:", "options"), KviOption_stringCtcpUserInfoLanguages);
	sel->setMinimumLabelWidth(120);
	mergeTip(sel, __tr2qs_ctx("You can put here the short names of the languages you can speak. "
	                          "An example might be \"EN, IT\" that would mean that you speak both Italian and English.",
	                  "options")
	        + szTrailing);

	sel = addStringSelector(gbox, __tr2qs_ctx("Other:", "options"), KviOption_stringCtcpUserInfoOther);
	sel->setMinimumLabelWidth(120);
	mergeTip(sel, __tr2qs_ctx("You can put here some additional personal data. "
	                          "It might be a funny quote or your homepage URL... "
	                          "Please note that this information will be viewable "
	                          "by anyone so <b>don't put any sensible data</b> (passwords, telephone or credit card numbers).",
	                  "options")
	        + szTrailing);

	addRowSpacer(0, 2, 0, 2);
}

KviIdentityGeneralOptionsWidget::~KviIdentityGeneralOptionsWidget()
    = default;

void KviIdentityGeneralOptionsWidget::setNickAlternatives()
{
	NickAlternativesDialog * dlg = new NickAlternativesDialog(this, m_szAltNicknames[0], m_szAltNicknames[1], m_szAltNicknames[2]);
	if(dlg->exec() != QDialog::Accepted)
		return;
	dlg->fill(m_szAltNicknames[0], m_szAltNicknames[1], m_szAltNicknames[2]);
	delete dlg;
}

void KviIdentityGeneralOptionsWidget::commit()
{
	KviOptionsWidget::commit();

	if(KVI_OPTION_STRING(KviOption_stringRealname).isEmpty())
		KVI_OPTION_STRING(KviOption_stringRealname) = KVI_DEFAULT_REALNAME;
	if(KVI_OPTION_STRING(KviOption_stringUsername).isEmpty())
		KVI_OPTION_STRING(KviOption_stringUsername) = KVI_DEFAULT_USERNAME;

	KVI_OPTION_STRING(KviOption_stringNickname2) = m_szAltNicknames[0];
	KVI_OPTION_STRING(KviOption_stringNickname3) = m_szAltNicknames[1];
	KVI_OPTION_STRING(KviOption_stringNickname4) = m_szAltNicknames[2];

	int i = m_pAgeCombo->currentIndex();
	if(i < 0)
		i = 0;
	if(i > 120)
		i = 120;
	if(i <= 0)
		KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge) = "";
	else
		KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge)
		    .setNum(i);

	switch(m_pGenderCombo->currentIndex())
	{
		case 1:
			// this should be in English
			KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender) = "Female";
			break;
		case 2:
			// this should be in English
			KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender) = "Male";
			break;
		default:
			KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender) = "";
			break;
	}
}

OptionsWidget_identityAvatar::OptionsWidget_identityAvatar(QWidget * parent)
    : KviOptionsWidget(parent)
{
	createLayout();
	layout()->setMargin(10);

	m_pLocalAvatar = new KviPixmap(KVI_OPTION_PIXMAP(KviOption_pixmapMyAvatar));

	bool bHaveAvatar = (!KVI_OPTION_STRING(KviOption_stringMyAvatar).isEmpty()) && m_pLocalAvatar->pixmap();

	QString szTip = __tr2qs_ctx("Here you can choose your avatar image.<br>"
	                            "It will be visible by other people that request it.<br>"
	                            "Choose a nice image of yourself, possibly avoiding obscenity and offending images.<br>"
	                            "A good idea is to choose a relatively small file of around 150 Kb max, because "
	                            "most clients have a limit on the size of avatars being downloaded.<br>"
	                            "The image also should be smaller than 800x600 pixels since it will have to be viewable in everyone's monitor.",
	    "options");

	m_pUseAvatarCheck = new QCheckBox(__tr2qs_ctx("Use avatar:", "options"), this);
	addWidgetToLayout(m_pUseAvatarCheck, 0, 0, 0, 0);
	m_pUseAvatarCheck->setChecked(bHaveAvatar);
	mergeTip(m_pUseAvatarCheck, szTip);

	m_pAvatarPreview = new KviPixmapPreview(this);
	addWidgetToLayout(m_pAvatarPreview, 0, 1, 0, 1);
	m_pAvatarPreview->setPixmap(m_pLocalAvatar);
	m_pAvatarPreview->setEnabled(bHaveAvatar);
	connect(m_pUseAvatarCheck, SIGNAL(toggled(bool)), m_pAvatarPreview, SLOT(setEnabled(bool)));
	mergeTip(m_pAvatarPreview, szTip);

	KviTalHBox * hb = new KviTalHBox(this);
	hb->setSpacing(4);
	addWidgetToLayout(hb, 0, 2, 0, 2);

	m_pAvatarNameEdit = new QLineEdit(hb);
	m_pAvatarNameEdit->setReadOnly(true);
	m_pAvatarNameEdit->setEnabled(bHaveAvatar);
	m_pAvatarNameEdit->setText(KVI_OPTION_STRING(KviOption_stringMyAvatar));
	connect(m_pUseAvatarCheck, SIGNAL(toggled(bool)), m_pAvatarNameEdit, SLOT(setEnabled(bool)));

	m_pChooseAvatarButton = new QPushButton(__tr2qs_ctx("Choose...", "options"), hb);
	m_pChooseAvatarButton->setEnabled(bHaveAvatar);
	connect(m_pUseAvatarCheck, SIGNAL(toggled(bool)), m_pChooseAvatarButton, SLOT(setEnabled(bool)));

	connect(m_pChooseAvatarButton, SIGNAL(clicked()), this, SLOT(chooseAvatar()));

	layout()->setRowStretch(1, 2);
}

OptionsWidget_identityAvatar::~OptionsWidget_identityAvatar()
{
	delete m_pLocalAvatar;
}

void OptionsWidget_identityAvatar::commit()
{
	KviOptionsWidget::commit();

	QString szAvatarName = m_pAvatarNameEdit->text();

	if(m_pUseAvatarCheck->isChecked() && m_pLocalAvatar->pixmap() && (!szAvatarName.isEmpty()))
	{
		KVI_OPTION_STRING(KviOption_stringMyAvatar) = szAvatarName;
		KVI_OPTION_PIXMAP(KviOption_pixmapMyAvatar) = *m_pLocalAvatar;
	}
	else
	{
		KVI_OPTION_STRING(KviOption_stringMyAvatar) = "";
		KVI_OPTION_PIXMAP(KviOption_pixmapMyAvatar) = KviPixmap();
	}
	g_pApp->setAvatarFromOptions();
}

void OptionsWidget_identityAvatar::chooseAvatar()
{
	QString szCurrent = m_pAvatarNameEdit->text();

	AvatarSelectionDialog dlg(this, szCurrent);
	if(dlg.exec() != QDialog::Accepted)
		return;

	szCurrent = dlg.avatarName().trimmed();

	if(KviQString::equalCIN(szCurrent, "http://", 7) || KviQString::equalCIN(szCurrent, "https://", 8))
	{
		// this is a url
		// first check if we have it in the cache
		KviAvatar * a = g_pIconManager->getAvatar(QString(), szCurrent);
		if(!a)
		{
			// no cache... try downloading
			AvatarDownloadDialog dlg(this, szCurrent);
			if(dlg.exec() == QDialog::Accepted)
			{
				// downloaded!
				// try to load it
				if(!m_pLocalAvatar->load(dlg.localFileName()))
				{
					// unloadable
					szCurrent = "";
					QMessageBox::warning(this, __tr2qs_ctx("Failed to Load Avatar - KVIrc", "options"),
					    __tr2qs_ctx("Failed to load the avatar image.<br>"
					                "It may be an inaccessible file or an "
					                "unsupported image format.",
					                         "options"),
					    QMessageBox::Ok, QMessageBox::NoButton);
				} // else loaded!
			}
			else
			{
				// unloadable
				QString szTmp = QString(__tr2qs_ctx("Failed to download the avatar image.<br>"
				                                    "<b>%1</b>",
				                            "options"))
				                    .arg(dlg.errorMessage());
				QMessageBox::warning(this, __tr2qs_ctx("Avatar Download Failed - KVIrc", "options"), szTmp, QMessageBox::Ok, QMessageBox::NoButton);
				szCurrent = "";
				m_pLocalAvatar->setNull();
			}
		}
		else
		{
			// else OK.. got it in the cache
			m_pLocalAvatar->set(*(a->pixmap()), a->localPath());
			delete a;
		}
	}
	else
	{
		// this is a local path
		if(m_pLocalAvatar->load(szCurrent))
		{
			// local path, loaded
			QString tmp = szCurrent;
			int idx = tmp.lastIndexOf("/");
			if(idx != -1)
			{
				szCurrent = tmp.right(tmp.length() - (idx + 1));
				tmp = szCurrent;
			}
			idx = tmp.lastIndexOf("\\");
			if(idx != -1)
			{
				szCurrent = tmp.right(tmp.length() - (idx + 1));
				tmp = szCurrent;
			}
		}
		else
		{
			// unloadable
			QMessageBox::warning(this, __tr2qs_ctx("Failed to Load Avatar - KVIrc", "options"),
			    __tr2qs_ctx("Failed to load the avatar image.<br>"
			                "It may be an inaccessible file or an "
			                "unsupported image format.",
			                         "options"),
			    QMessageBox::Ok, QMessageBox::NoButton);
			szCurrent = "";
		}
	}

	if(m_pLocalAvatar->pixmap())
	{
		if((m_pLocalAvatar->pixmap()->width() > 1024) || (m_pLocalAvatar->pixmap()->height() > 768))
		{
			QMessageBox::warning(
			    this,
			    __tr2qs_ctx("File Dimensions - KVIrc", "options"),
			    __tr2qs_ctx(
			        "The avatar you have chosen is bigger than 1024x768 pixels.<br>"
			        "Such a big image will not be seen on all the user monitors<br>"
			        "and will probably be scaled by the remote clients with poor quality<br>"
			        "algorithms to improve performance. You *should* scale it manually<br>"
			        "to a sane size (like 800x600) or choose a different image.",
			        "options"));
		}
		else
		{
			QFileInfo inf(m_pLocalAvatar->path());
			if(inf.size() > 524288)
			{
				QMessageBox::warning(
				    this,
				    __tr2qs_ctx("File Size - KVIrc", "options"),
				    __tr2qs_ctx(
				        "The avatar you have chosen is bigger than 500 KiB<br>"
				        "and most clients will refuse to download it.<br>"
				        "You *should* either scale it, use a different storage<br>"
				        "format or choose a different image.",
				        "options"));
			}
		}
	}

	m_pAvatarNameEdit->setText(szCurrent);
	m_pAvatarPreview->setPixmap(m_pLocalAvatar);
}

OptionsWidget_identityAdvanced::OptionsWidget_identityAdvanced(QWidget * parent)
    : KviOptionsWidget(parent)
{
	m_pISelector = nullptr;
	m_pWSelector = nullptr;
	m_pSSelector = nullptr;

	m_sModeStr = KVI_OPTION_STRING(KviOption_stringDefaultUserMode);
	m_bI = m_sModeStr.contains('i');
	m_bW = m_sModeStr.contains('w');
	m_bS = m_sModeStr.contains('s');

	createLayout();
	layout()->setMargin(10);

	KviTalGroupBox * gbox = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("User Mode", "options"));
	m_pISelector = addBoolSelector(gbox, __tr2qs_ctx("Invisible (+i)", "options"), &m_bI);
	m_pSSelector = addBoolSelector(gbox, __tr2qs_ctx("Server notices (+s)", "options"), &m_bS);
	m_pWSelector = addBoolSelector(gbox, __tr2qs_ctx("WALLOPS (+w)", "options"), &m_bW);

	gbox = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("Default Messages", "options"));

	KviStringSelector * sel = addStringSelector(gbox, __tr2qs_ctx("Part message:", "options"), KviOption_stringPartMessage);
	sel->setMinimumLabelWidth(120);

	mergeTip(sel, __tr2qs_ctx("This is the default part message that will be used when you "
	                          "leave a channel by closing a channel window.",
	                  "options"));

	sel = addStringSelector(gbox, __tr2qs_ctx("Quit message:", "options"), KviOption_stringQuitMessage);
	sel->setMinimumLabelWidth(120);
	mergeTip(sel, __tr2qs_ctx("This is the default quit message that will be used when you "
	                          "quit your IRC session by closing the console window or disconnecting by pressing the disconnect button.",
	                  "options"));

	addRowSpacer(0, 2, 0, 2);
}

OptionsWidget_identityAdvanced::~OptionsWidget_identityAdvanced()
    = default;

void OptionsWidget_identityAdvanced::commit()
{
	KviOptionsWidget::commit();

	m_sModeStr = m_bI ? "i" : "";
	if(m_bS)
		m_sModeStr += "s";
	if(m_bW)
		m_sModeStr += "w";

	KVI_OPTION_STRING(KviOption_stringDefaultUserMode) = m_sModeStr.ptr();
}

OptionsWidget_identityProfile::OptionsWidget_identityProfile(QWidget * pParent)
    : KviOptionsWidget(pParent)
{
	setObjectName("identity_profiles_option_widget");
	m_pEditor = nullptr;
	m_iCurrentEditedProfile = -1;

	createLayout();
	QGridLayout * pLayout = layout();

	KviIdentityProfileSet * pSet = KviIdentityProfileSet::instance();
	bool bEnabled = pSet ? (pSet->isEnabled() && !pSet->isEmpty()) : false;

	m_pProfilesCheck = new QCheckBox(__tr2qs_ctx("Enable network profiles", "options"), this);
	KviTalToolTip::add(m_pProfilesCheck, __tr2qs_ctx("This check enables the use of network profiles", "options"));
	m_pProfilesCheck->setChecked(bEnabled);
	pLayout->addWidget(m_pProfilesCheck, 0, 0, 1, 3);
	connect(m_pProfilesCheck, SIGNAL(toggled(bool)), this, SLOT(toggleControls()));

	// Profiles list
	m_pTreeWidget = new QTreeWidget(this);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTreeWidget->setAllColumnsShowFocus(true);

	QStringList labels;
	labels.append(__tr2qs_ctx("Name", "options"));
	labels.append(__tr2qs_ctx("Network", "options"));
	labels.append(__tr2qs_ctx("Nickname", "options"));
	labels.append(__tr2qs_ctx("Alt. Nickname", "options"));
	labels.append(__tr2qs_ctx("Username", "options"));
	labels.append(__tr2qs_ctx("Real Name", "options"));
	m_pTreeWidget->setHeaderLabels(labels);
	m_pTreeWidget->setColumnWidth(0, 110);
	m_pTreeWidget->setColumnWidth(1, 110);
	m_pTreeWidget->setColumnWidth(2, 110);
	m_pTreeWidget->setColumnWidth(3, 130);
	m_pTreeWidget->setColumnWidth(4, 120);
	m_pTreeWidget->setColumnWidth(5, 120);

	KviTalToolTip::add(m_pTreeWidget, __tr2qs_ctx("This is a set of rules to use profiles.<br>"
	                                              "KVIrc will use them to handle the user connection "
	                                              "data before the data is sent to the IRC server.<br>"
	                                              "This is useful if a user wants to use different data "
	                                              "on different networks without changing them at every "
	                                              "connection attempt.", "options"));
			
	pLayout->addWidget(m_pTreeWidget, 1, 0, 1, 3);
	connect(m_pTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(toggleControls()));

	// Buttons box
	KviTalHBox * pBtnBox = new KviTalHBox(this);
	pLayout->addWidget(pBtnBox, 2, 0, 1, 3);

	m_pBtnAddProfile = new QPushButton(__tr2qs_ctx("Add Profile", "options"), pBtnBox);
	connect(m_pBtnAddProfile, SIGNAL(clicked()), this, SLOT(addProfileEntry()));

	m_pBtnEditProfile = new QPushButton(__tr2qs_ctx("Edit Profile", "options"), pBtnBox);
	connect(m_pBtnEditProfile, SIGNAL(clicked()), this, SLOT(editProfileEntry()));

	m_pBtnDelProfile = new QPushButton(__tr2qs_ctx("Delete Profile", "options"), pBtnBox);
	connect(m_pBtnDelProfile, SIGNAL(clicked()), this, SLOT(delProfileEntry()));

	// Fill in the treewidget
	if(pSet && pSet->profiles())
	{
		QTreeWidgetItem * pItem;
		KviPointerList<KviIdentityProfile> * pList = pSet->profiles();
		for(KviIdentityProfile * pProfile = pList->first(); pProfile; pProfile = pList->next())
		{
			pItem = new QTreeWidgetItem(m_pTreeWidget);
			pItem->setText(0, pProfile->name());
			pItem->setText(1, pProfile->network());
			pItem->setText(2, pProfile->nick());
			pItem->setText(3, pProfile->altNick());
			pItem->setText(4, pProfile->userName());
			pItem->setText(5, pProfile->realName());
		}
	}

	toggleControls();
}

OptionsWidget_identityProfile::~OptionsWidget_identityProfile()
    = default;

void OptionsWidget_identityProfile::toggleControls()
{
	bool bEnabled = m_pProfilesCheck->isChecked();
	m_pTreeWidget->setEnabled(bEnabled);
	m_pBtnAddProfile->setEnabled(bEnabled);

	bEnabled = bEnabled && (m_pTreeWidget->topLevelItemCount()) && m_pTreeWidget->currentItem();
	m_pBtnEditProfile->setEnabled(bEnabled);
	m_pBtnDelProfile->setEnabled(bEnabled);
}

void OptionsWidget_identityProfile::addProfileEntry()
{
	KviIdentityProfile profile;
	m_iCurrentEditedProfile = -1;

	if(m_pEditor)
		delete m_pEditor;
	m_pEditor = new IdentityProfileEditor(this);

	if(m_pEditor->editProfile(&profile))
	{
		QTreeWidgetItem * pItem = new QTreeWidgetItem(m_pTreeWidget);
		pItem->setText(0, profile.name());
		pItem->setText(1, profile.network());
		pItem->setText(2, profile.nick());
		pItem->setText(3, profile.altNick());
		pItem->setText(4, profile.userName());
		pItem->setText(5, profile.realName());
	}
}

void OptionsWidget_identityProfile::editProfileEntry()
{
	QTreeWidgetItem * pItem = (QTreeWidgetItem *)m_pTreeWidget->currentItem();
	if(!pItem)
		return;

	// Fill in the editor data
	KviIdentityProfile profile;
	profile.setName(pItem->text(0));
	profile.setNetwork(pItem->text(1));
	profile.setNick(pItem->text(2));
	profile.setAltNick(pItem->text(3));
	profile.setUserName(pItem->text(4));
	profile.setRealName(pItem->text(5));

	m_iCurrentEditedProfile = m_pTreeWidget->indexOfTopLevelItem(pItem);

	if(m_pEditor)
		delete m_pEditor;
	m_pEditor = new IdentityProfileEditor(this);
	if(m_pEditor->editProfile(&profile))
	{
		pItem->setText(0, profile.name());
		pItem->setText(1, profile.network());
		pItem->setText(2, profile.nick());
		pItem->setText(3, profile.altNick());
		pItem->setText(4, profile.userName());
		pItem->setText(5, profile.realName());
	}
}

void OptionsWidget_identityProfile::editProfileOkPressed()
{
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		if(m_pEditor->m_pNameEdit->text() == m_pTreeWidget->topLevelItem(i)->text(0) && i != m_iCurrentEditedProfile)
		{
			QMessageBox::warning(this, __tr2qs_ctx("Invalid Profile Rule - KVIrc", "options"), __tr2qs_ctx("There is already a profile with that name", "options"), __tr2qs_ctx("OK", "options"));
			return;
		}
	}

	m_pEditor->accept();
}

void OptionsWidget_identityProfile::delProfileEntry()
{
	QTreeWidgetItem * pItem = (QTreeWidgetItem *)m_pTreeWidget->currentItem();
	if(!pItem)
		return;

	delete pItem;
	toggleControls();
}

void OptionsWidget_identityProfile::commit()
{
	KviIdentityProfileSet::instance()->clear();
	if(m_pTreeWidget->topLevelItemCount())
	{
		KviIdentityProfileSet::instance()->setEnabled(m_pProfilesCheck->isChecked());
		QTreeWidgetItem * pItem;
		for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
		{
			pItem = (QTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);

			KviIdentityProfile * pProfile = new KviIdentityProfile();
			pProfile->setName(pItem->text(0));
			pProfile->setNetwork(pItem->text(1));
			pProfile->setNick(pItem->text(2));
			pProfile->setAltNick(pItem->text(3));
			pProfile->setUserName(pItem->text(4));
			pProfile->setRealName(pItem->text(5));

			KviIdentityProfileSet::instance()->addProfile(pProfile);
		}
	}

	KviOptionsWidget::commit();
}

IdentityProfileEditor::IdentityProfileEditor(QWidget * pParent)
    : QDialog(pParent)
{
	setObjectName("identity_profile_editor");
	setWindowTitle(__tr2qs_ctx("Profile Editor - KVIrc", "options"));

	QGridLayout * pLayout = new QGridLayout(this);

	QLabel * pLabel = new QLabel(__tr2qs_ctx("Profile name:", "options"), this);
	pLayout->addWidget(pLabel, 0, 0);

	m_pNameEdit = new QLineEdit(this);
	KviTalToolTip::add(m_pNameEdit, __tr2qs_ctx("Put here the name of the profile", "options"));
	pLayout->addWidget(m_pNameEdit, 0, 1, 1, 2);
	connect(m_pNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(toggleButton()));

	pLabel = new QLabel(__tr2qs_ctx("Network name:", "options"), this);
	pLayout->addWidget(pLabel, 1, 0);

	m_pNetworkEdit = new QLineEdit(this);
	KviTalToolTip::add(m_pNetworkEdit, __tr2qs_ctx("Put here the name of the network", "options"));
	pLayout->addWidget(m_pNetworkEdit, 1, 1, 1, 2);

	pLabel = new QLabel(__tr2qs_ctx("Nickname:", "options"), this);
	pLayout->addWidget(pLabel, 2, 0);

	m_pNickEdit = new QLineEdit(this);
	KviTalToolTip::add(m_pNickEdit, __tr2qs_ctx("Put here the nickname you want to use", "options"));
	pLayout->addWidget(m_pNickEdit, 2, 1, 1, 2);

	pLabel = new QLabel(__tr2qs_ctx("Alt. nickname:", "options"), this);
	pLayout->addWidget(pLabel, 3, 0);

	m_pAltNickEdit = new QLineEdit(this);
	KviTalToolTip::add(m_pAltNickEdit, __tr2qs_ctx("Put here the alternative nickname you want to use", "options"));
	pLayout->addWidget(m_pAltNickEdit, 3, 1, 1, 2);

	pLabel = new QLabel(__tr2qs_ctx("Username:", "options"), this);
	pLayout->addWidget(pLabel, 4, 0);

	m_pUserNameEdit = new QLineEdit(this);
	KviTalToolTip::add(m_pUserNameEdit, __tr2qs_ctx("Put here the username you want to use", "options"));
	pLayout->addWidget(m_pUserNameEdit, 4, 1, 1, 2);

	pLabel = new QLabel(__tr2qs_ctx("Real name:", "options"), this);
	pLayout->addWidget(pLabel, 5, 0);

	m_pRealNameEdit = new QLineEdit(this);
	KviTalToolTip::add(m_pRealNameEdit, __tr2qs_ctx("Put here the real name you want to use", "options"));
	pLayout->addWidget(m_pRealNameEdit, 5, 1, 1, 2);

	KviTalHBox * pBox = new KviTalHBox(this);
	pBox->setAlignment(Qt::AlignRight);
	pLayout->addWidget(pBox, 6, 1, 1, 2);

	QPushButton * p = new QPushButton(__tr2qs_ctx("Cancel", "options"), pBox);
	//p->setMinimumWidth(100);
	connect(p, SIGNAL(clicked()), this, SLOT(reject()));

	m_pBtnOk = new QPushButton(__tr2qs_ctx("OK", "options"), pBox);
	m_pBtnOk->setEnabled(false);
	connect(m_pBtnOk, SIGNAL(clicked()), pParent, SLOT(editProfileOkPressed()));

	pLayout->setColumnStretch(1, 1);

	setMinimumWidth(250);

	toggleButton();
}

IdentityProfileEditor::~IdentityProfileEditor()
    = default;

bool IdentityProfileEditor::editProfile(KviIdentityProfile * pProfile)
{
	// Fill in the fields
	m_pNameEdit->setText(pProfile->name());
	m_pNetworkEdit->setText(pProfile->network());
	m_pNickEdit->setText(pProfile->nick());
	m_pAltNickEdit->setText(pProfile->altNick());
	m_pUserNameEdit->setText(pProfile->userName());
	m_pRealNameEdit->setText(pProfile->realName());

	// Ensure data
	m_pNameEdit->selectAll();

	// Show the dialog
	if(exec() != QDialog::Accepted)
		return false;

	// Store data in the object
	pProfile->setName(m_pNameEdit->text());
	pProfile->setNetwork(m_pNetworkEdit->text());
	pProfile->setNick(m_pNickEdit->text());
	pProfile->setAltNick(m_pAltNickEdit->text());
	pProfile->setUserName(m_pUserNameEdit->text());
	pProfile->setRealName(m_pRealNameEdit->text());

	return true;
}

void IdentityProfileEditor::toggleButton()
{
	m_pBtnOk->setEnabled(!m_pNameEdit->text().isEmpty());
}
