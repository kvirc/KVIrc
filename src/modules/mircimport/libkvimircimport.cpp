//=============================================================================
//
//   File : libkvimircimport.cpp
//   Creation date : Tue Aug 27 01:20:35 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "libkvimircimport.h"

#include "KviModule.h"
#include "KviFileDialog.h"
#include "KviFileUtils.h"
#include "KviMainWindow.h"
#include "kvi_fileextensions.h"
#include "KviLocale.h"
#include "KviConfigurationFile.h"
#include "KviApplication.h"
#include "KviTalVBox.h"
#include "KviIrcServer.h"

#include <QDir>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

KviMircServersIniImport * g_pMircServersIniImport = nullptr;
KviRemoteMircServersIniImport * g_pRemoteMircServersIniImport = nullptr;

KviMircServersIniImport::KviMircServersIniImport(KviModuleExtensionDescriptor * d)
    : KviMexServerImport(d)
{
}

KviMircServersIniImport::~KviMircServersIniImport()
{
	g_pMircServersIniImport = nullptr;
}

int KviMircServersIniImport::doImport(const QString & filename)
{
	KviConfigurationFile cfg(filename, KviConfigurationFile::Read, true);
	int iCount = 0;
	if(cfg.hasGroup("servers"))
	{
		cfg.setGroup("servers");
		int i = 0;
		QString key;
		QString entry;
		do
		{
			key = QString("n%1").arg(i);
			entry = cfg.readEntry(key, "");
			if(!entry.isEmpty())
			{
				QString description;
				QString serv;
				QString port;
				kvi_u32_t uPort = 0;
				// <description>SERVER:<server:port>GROUP:<network>
				int idx = entry.indexOf("SERVER:", 0, Qt::CaseSensitive);
				if(idx != -1)
				{
					description = entry.left(idx);
					entry.remove(0, idx + 7);
					idx = entry.indexOf("GROUP:", 0, Qt::CaseSensitive);
					if(idx != -1)
					{
						port = entry.left(idx);
						entry.remove(0, idx + 6);
					}
					idx = port.indexOf(':', 0, Qt::CaseSensitive);
					if(idx != -1)
					{
						serv = port.left(idx);
						port.remove(0, idx + 1);
						bool bOk;
						uPort = port.toUInt(&bOk);
						if(!bOk)
							uPort = 6667;
					}
					else
					{
						serv = port;
						uPort = 6667;
					}
				}
				if(entry.isEmpty())
					entry = __tr2qs("Standalone Servers");
				if(!serv.isEmpty())
				{
					KviIrcServer s;
					s.setHostName(serv);
					s.setDescription(description);
					s.setPort(uPort);
					iCount++;
					emit server(s, entry);
				}
				++i;
			}
		} while(!entry.isEmpty());
	}
	else
	{
		QString szTmp = QString(__tr2qs("%1 doesn't look like a servers.ini file.\nImport failed.")).arg(filename);
		QMessageBox::warning(nullptr, __tr2qs("File Import - KVIrc"), szTmp);
	}
	return iCount;
}

void KviMircServersIniImport::start()
{
	//KviCString buffer;
	QString buffer;
	if(!KviFileDialog::askForOpenFileName(buffer, __tr("Select a File - KVIrc"), nullptr, KVI_FILTER_INI, false, true, g_pMainWindow))
		return;

	doImport(buffer);
	delete this;
}

void KviMircServersIniImport::die()
{
	delete this;
}

#define KVI_WWWMIRCCOUK_SERVERSINI "http://www.mirc.co.uk/servers.ini"

KviRemoteMircServerImportWizard::KviRemoteMircServerImportWizard(KviRemoteMircServersIniImport * f)
    : KviTalWizard(nullptr)
{
	QString capt = __tr2qs("Remote mIRC servers.ini - Import Wizard");
	setWindowTitle(capt);

	setModal(true);

	m_pRequest = nullptr;
	m_pFilter = f;

	QLabel * l = new QLabel(this);
	l->setWordWrap(true);
	l->setText(__tr2qs("<center><b>Welcome!</b></center><br>This wizard will guide you in the process of "
	                   "downloading a list of IRC servers. Please click \"<b>Next</b>\" to begin the operation."));
	addPage(l, capt);

	KviTalVBox * vb = new KviTalVBox(this);
	l = new QLabel(vb);
	l->setWordWrap(true);
	l->setText(__tr2qs("Here you can modify the URL that the list will be downloaded from. In most cases the default URL is acceptable."));

	vb->setStretchFactor(l, 1);

	m_pUrlEdit = new QLineEdit(vb);
	m_pUrlEdit->setText(KVI_WWWMIRCCOUK_SERVERSINI);

	addPage(vb, __tr2qs("URL Selection"));

	vb = new KviTalVBox(this);

	l = new QLabel(__tr2qs("Please wait while the list is being downloaded"), vb);
	vb->setStretchFactor(l, 1);

	m_pOutput = new QLabel(vb);
	m_pOutput->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

	addPage(vb, __tr2qs("List Download"));

	setBackEnabled(vb, false);
	setNextEnabled(vb, false);
	setFinishEnabled(vb, true);

	connect(this, SIGNAL(pageChanged(const QString &)), this, SLOT(pageSelected(const QString &)));
}

KviRemoteMircServerImportWizard::~KviRemoteMircServerImportWizard()
{
	if(m_pRequest)
		delete m_pRequest;
}

void KviRemoteMircServerImportWizard::pageSelected(const QString & title)
{
	if(title == __tr2qs("List Download"))
	{
		start();
	}
}

void KviRemoteMircServerImportWizard::closeEvent(QCloseEvent *)
{
	m_pFilter->die();
}

void KviRemoteMircServerImportWizard::done(int)
{
	m_pFilter->die();
}

void KviRemoteMircServerImportWizard::start()
{
	QString url = m_pUrlEdit->text();
	if(url.isEmpty())
		url = KVI_WWWMIRCCOUK_SERVERSINI;

	finishButton()->setEnabled(false);
	if(m_pRequest)
		delete m_pRequest;

	m_pRequest = new KviHttpRequest();
	connect(m_pRequest, SIGNAL(terminated(bool)), this, SLOT(getListTerminated(bool)));
	connect(m_pRequest, SIGNAL(status(const QString &)), this, SLOT(getListMessage(const QString &)));

	g_pApp->getTmpFileName(m_szTmpFileName, "servers.ini");
	if(!m_pRequest->get(KviUrl(url), KviHttpRequest::StoreToFile, m_szTmpFileName))
	{
		delete m_pRequest;
		m_pRequest = nullptr;
		m_pOutput->setText(__tr2qs("Failed to start the server list transfer :("));
		finishButton()->setEnabled(true);
	}
}

void KviRemoteMircServerImportWizard::getListMessage(const QString & message)
{
	if(!message.isEmpty())
		m_pOutput->setText(message);
}

void KviRemoteMircServerImportWizard::getListTerminated(bool bSuccess)
{
	if(!m_pRequest)
		return;
	if(bSuccess)
	{
		m_pOutput->setText(__tr2qs("File downloaded: processing..."));
		m_pOutput->repaint();

		int iCount = m_pFilter->doImport(m_szTmpFileName);

		QString tmp;
		if(iCount > 0)
			tmp = __tr2qs("%1 servers imported successfully").arg(iCount);
		else
			tmp = __tr2qs("No servers imported");
		m_pOutput->setText(tmp);

		QDir d;
		d.remove(m_szTmpFileName);
	}
	else
		m_pOutput->setText(m_pRequest->lastError());

	delete m_pRequest;
	m_pRequest = nullptr;
	cancelButton()->setEnabled(false);
	finishButton()->setEnabled(true);
}

KviRemoteMircServersIniImport::KviRemoteMircServersIniImport(KviModuleExtensionDescriptor * d)
    : KviMircServersIniImport(d)
{
	m_pWizard = nullptr;
}

KviRemoteMircServersIniImport::~KviRemoteMircServersIniImport()
{
	if(m_pWizard)
		delete m_pWizard;
	g_pRemoteMircServersIniImport = nullptr;
}

void KviRemoteMircServersIniImport::start()
{
	if(m_pWizard)
		delete m_pWizard;
	m_pWizard = new KviRemoteMircServerImportWizard(this);
	m_pWizard->show();
}

void KviRemoteMircServersIniImport::die()
{
	delete this;
}

static KviModuleExtension * mircimport_local_filter_alloc(KviModuleExtensionAllocStruct * s)
{
	if(g_pMircServersIniImport)
		delete g_pMircServersIniImport;
	g_pMircServersIniImport = new KviMircServersIniImport(s->pDescriptor);
	return g_pMircServersIniImport;
}

static KviModuleExtension * mircimport_remote_filter_alloc(KviModuleExtensionAllocStruct * s)
{
	if(g_pRemoteMircServersIniImport)
		delete g_pRemoteMircServersIniImport;
	g_pRemoteMircServersIniImport = new KviRemoteMircServersIniImport(s->pDescriptor);
	return g_pRemoteMircServersIniImport;
}

static bool mircimport_module_init(KviModule * m)
{
	QString szPath;
	QPixmap * pix = nullptr;
	if(g_pApp->findImage(szPath, "kvi_mircimport.png"))
	{
		pix = new QPixmap(szPath);
		if(pix->isNull())
		{
			delete pix;
			pix = nullptr;
		}
	}

	KviModuleExtensionDescriptor * d = m->registerExtension("serverimport",
	    "mIRC servers.ini import filter",
	    __tr("Import from servers.ini"),
	    mircimport_local_filter_alloc);

	if(d && pix)
		d->setIcon(*pix);

	d = m->registerExtension("serverimport",
	    "Remote mIRC servers.ini import filter",
	    __tr("Import from http://www.mirc.co.uk/servers.ini"),
	    mircimport_remote_filter_alloc);

	if(d && pix)
		d->setIcon(*pix);

	if(pix)
		delete pix;
	return true;
}

static bool mircimport_module_cleanup(KviModule *)
{
	if(g_pMircServersIniImport)
		delete g_pMircServersIniImport;
	if(g_pRemoteMircServersIniImport)
		delete g_pRemoteMircServersIniImport;
	g_pMircServersIniImport = nullptr;
	g_pRemoteMircServersIniImport = nullptr;

	return true;
}

static bool mircimport_module_can_unload(KviModule *)
{
	return ((!g_pMircServersIniImport) && (!g_pRemoteMircServersIniImport));
}

KVIRC_MODULE(
    "mIRCimport",                                                   // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "mIRC servers.ini importer",
    mircimport_module_init,
    mircimport_module_can_unload,
    0,
    mircimport_module_cleanup,
    0)
