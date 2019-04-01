//=============================================================================
//
//   File : AboutDialog.cpp
//   Creation date : Sun Jun 23 17:59:12 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include "AboutDialog.h"

#include "kvi_defaults.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviFileUtils.h"
#include "kvi_sourcesdate.h"
#include "KviBuildInfo.h"
#include "KviRuntimeInfo.h"

#include <QTextEdit>
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QRegExp>
#include <QEvent>
#include <QCloseEvent>

#ifdef COMPILE_SSL_SUPPORT
#include <openssl/crypto.h>
#endif //COMPILE_SSL_SUPPORT

extern AboutDialog * g_pAboutDialog;
/*
"<font color=\"#FFFF00\"><b>KVIrc public releases :</b></font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">0.9.0</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 25.01.1999</font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">1.0.0 'Millennium'</font><br>\n" \
"<font size=\"2\">\"The net in your hands\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 21.12.1999</font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">2.0.0 'Phoenix'</font><br>\n" \
"<font size=\"2\">\"The client that can't make coffee\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 30.05.2000</font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">2.1.0 'Dark Star'</font><br>\n" \
"<font size=\"2\">\"The client that can't make coffee\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 30.01.2001</font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">2.1.1 'Monolith'</font><br>\n" \
"<font size=\"2\">\"A breath of fresh net\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 01.05.2001</font><br>\n" \
"<br> 3.0.0-xmas build: 24-12-2001\n" \
"3.0.0-beta1: 24-06-2002\n "
"<font color=\"#FF0000\">3.0.0 'Avatar'</font><br>\n" \
"<font size=\"2\">\"No slogan yet\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: Still unknown</font><br>\n" \
*/

#include "abouttext.inc"

AboutDialog::AboutDialog()
    : KviTalTabDialog(nullptr)
{
	setWindowTitle(__tr2qs_ctx("About KVIrc", "about"));
	setOkButton(__tr2qs_ctx("Close", "about"));

	// About tab
	QString buffer;
	g_pApp->findImage(buffer, "kvi_release_art.png");

	QPixmap pix(buffer);

	QWidget * w = new QWidget(this);
	QGridLayout * g = new QGridLayout(w);

	QLabel * l = new QLabel(w);
	l->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	QPalette p = l->palette();
	p.setColor(backgroundRole(), Qt::black);
	l->setPalette(p);
	l->setAlignment(Qt::AlignCenter);
	l->setPixmap(pix);

	g->addWidget(l, 0, 0);

	QString aboutString = "<b>KVIrc " KVI_VERSION " '" KVI_RELEASE_NAME "'</b><br>";
	aboutString += __tr2qs_ctx("Forged by the <b>KVIrc Development Team</b>", "about");

	l = new QLabel(aboutString, w);
	l->setAlignment(Qt::AlignCenter);
	g->addWidget(l, 1, 0);

	addTab(w, __tr2qs_ctx("About", "about"));

	// Info tab
	w = new QWidget(this);
	g = new QGridLayout(w);

	QTextEdit * v = new QTextEdit(w);
	v->setReadOnly(true);
	g->addWidget(v, 0, 0);

	// Get info
	QString infoString = "<b>KVIrc " KVI_VERSION " '" KVI_RELEASE_NAME "'</b><br><br>";
	infoString += "<b>";
	infoString += __tr2qs_ctx("Runtime Info", "about");
	infoString += ":</b><br>";
	infoString += __tr2qs_ctx("System name", "about");
	infoString += ": ";
	infoString += KviRuntimeInfo::name();
#ifndef COMPILE_ON_MAC
	infoString += " ";
	infoString += KviRuntimeInfo::release();
#endif
	infoString += "<br>";
	infoString += __tr2qs_ctx("System version", "about");
	infoString += ": ";
	infoString += KviRuntimeInfo::version();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Architecture", "about");
	infoString += ": ";
	infoString += KviRuntimeInfo::machine();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Qt version", "about");
	infoString += ": ";
	infoString += KviRuntimeInfo::qtVersion();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Qt theme", "about");
	infoString += ": ";
	infoString += KviRuntimeInfo::qtTheme();
	infoString += "<br><br>";
	infoString += "<b>";
	infoString += __tr2qs_ctx("Build Info", "about");
	infoString += ":</b><br>";
	infoString += __tr2qs_ctx("Build date", "about");
	infoString += ": ";
	infoString += KviBuildInfo::buildDate();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Sources date", "about");
	infoString += ": ";
	infoString += KviBuildInfo::buildSourcesDate();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Revision number", "about");
	infoString += ": ";
	infoString += KviBuildInfo::buildRevision();
	infoString += "<br>";
	infoString += __tr2qs_ctx("CPU name", "about");
	infoString += ": ";
	infoString += KviBuildInfo::buildCPU();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Build command", "about");
	infoString += ": ";
	infoString += KviBuildInfo::buildCommand();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Build flags", "about");
	infoString += ": <br>&nbsp;&nbsp;&nbsp;";
	QString flags = KviBuildInfo::buildFlags();
	infoString += flags.replace(QRegExp(";"), "<br>&nbsp;&nbsp;&nbsp;");
	infoString += "<br>";
	infoString += __tr2qs_ctx("Compiler name", "about");
	infoString += ": ";
	infoString += KviBuildInfo::buildCompiler();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Compiler flags", "about");
	infoString += ": ";
	infoString += KviBuildInfo::buildCompilerFlags();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Qt version", "about");
	infoString += ": ";
	infoString += KviBuildInfo::qtVersion();
	infoString += "<br>";
	infoString += __tr2qs_ctx("Features", "about");
	infoString += ": ";
	infoString += KviBuildInfo::features();
#ifdef COMPILE_SSL_SUPPORT
	infoString += "<br>";
	infoString += __tr2qs_ctx("OpenSSL version", "about");
	infoString += ": ";
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
	infoString += SSLeay_version(SSLEAY_VERSION);
#else
	infoString += OpenSSL_version(OPENSSL_VERSION);
#endif
	infoString += "<br>";
	infoString += __tr2qs_ctx("OpenSSL compiler flags", "about");
	infoString += ": ";
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
	infoString += SSLeay_version(SSLEAY_CFLAGS);
#else
	infoString += OpenSSL_version(OPENSSL_CFLAGS);
#endif
	infoString += "<br>";
	infoString += __tr2qs_ctx("OpenSSL ", "about");
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
	infoString += SSLeay_version(SSLEAY_BUILT_ON);
#else
	infoString += OpenSSL_version(OPENSSL_BUILT_ON);
#endif
	infoString += "<br>";
#endif //COMPILE_SSL_SUPPORT

	v->setText(infoString);

	addTab(w, __tr2qs_ctx("Executable Information", "about"));

	// Honor & Glory tab
	w = new QWidget(this);
	g = new QGridLayout(w);

	v = new QTextEdit(w);
	v->setReadOnly(true);
	g->addWidget(v, 0, 0);

	v->setText(g_szAboutText);

	addTab(w, __tr2qs_ctx("Honor && Glory", "about"));

	// License tab
	w = new QWidget(this);
	g = new QGridLayout(w);

	v = new QTextEdit(w);
	v->setReadOnly(true);
	v->setWordWrapMode(QTextOption::NoWrap);
	g->addWidget(v, 0, 0);

	QString szLicense;

	QString szLicensePath;
	g_pApp->getGlobalKvircDirectory(szLicensePath, KviApplication::License, "ABOUT-LICENSE");

	if(!KviFileUtils::loadFile(szLicensePath, szLicense))
	{
		szLicense = __tr2qs_ctx("Oops! Can't find the license file.\n"
		                        "It MUST be included in the distribution...\n"
		                        "Please report to <pragma at kvirc dot net>",
		    "about");
	}

	v->setText(szLicense);

	addTab(w, __tr2qs_ctx("License", "about"));

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(closeButtonPressed()));
}

AboutDialog::~AboutDialog()
{
	g_pAboutDialog = nullptr;
}

void AboutDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	delete this;
}

void AboutDialog::closeButtonPressed()
{
	delete this;
}
