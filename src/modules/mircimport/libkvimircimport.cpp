//
//   File : libkvimircimport.cpp
//   Creation date : Tue Aug 27 01:20:35 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "libkvimircimport.h"
#include "kvi_module.h"

#include "kvi_filedialog.h"
#include "kvi_locale.h"
#include "kvi_config.h"
#include "kvi_app.h"


#include <qmessagebox.h>
#include <qdir.h>
#include <qpushbutton.h>
#include "kvi_tal_vbox.h"

KviMircServersIniImport * g_pMircServersIniImport = 0;
KviRemoteMircServersIniImport * g_pRemoteMircServersIniImport = 0;


KviMircServersIniImport::KviMircServersIniImport(KviModuleExtensionDescriptor * d)
: KviMexServerImport(d)
{
}

KviMircServersIniImport::~KviMircServersIniImport()
{
	g_pMircServersIniImport = 0;
}

int KviMircServersIniImport::doImport(const char * filename)
{
	KviConfig cfg(filename,KviConfig::Read);
	int iCount = 0;
	if(cfg.hasGroup("servers"))
	{
		cfg.setGroup("servers");
		int i = 0;
		KviStr key;
		KviStr entry;
		do {
			key.sprintf("n%d",i);
			entry = cfg.readEntry(key.ptr(),"");
			if(entry.hasData())
			{
				KviStr description;
				KviStr serv;
				KviStr port;
				kvi_u32_t uPort = 0;
				// <description>SERVER:<server:port>GROUP:<network>
				int idx = entry.findFirstIdx("SERVER:");
				if(idx != -1)
				{
					description = entry.left(idx);
					entry.cutLeft(idx + 7);
					idx = entry.findFirstIdx("GROUP:");
					if(idx != -1)
					{
						port = entry.left(idx);
						entry.cutLeft(idx + 6);
					}
					idx = port.findFirstIdx(':');
					if(idx != -1)
					{
						serv = port.left(idx);
						port.cutLeft(idx + 1);
						bool bOk;
						uPort = port.toUInt(&bOk);
						if(!bOk)uPort = 6667;
					} else {
						serv = port;
						uPort = 6667;
					}
				}
				if(entry.isEmpty())entry = __tr("Standalone Servers");
				if(serv.hasData())
				{
					KviIrcServer s;
					s.m_szHostname = serv;
					s.m_szDescription = description;
					s.m_uPort = uPort;
					iCount++;
					emit server(s,entry.ptr());
				}
				++i;
			}
		} while(entry.hasData());
	} else {
		KviStr tmp(KviStr::Format,__tr("%s doesn't look like a servers.ini file.\nImport failed."),filename);
		QMessageBox::warning(0,__tr2qs("Warning - KVIrc"),__tr2qs(tmp.ptr()));
	}
	return iCount;
}

void KviMircServersIniImport::start()
{
	//KviStr buffer;
	QString buffer;
	if(!KviFileDialog::askForOpenFileName(buffer,__tr("Choose a servers.ini file"),0,"*.ini",false,true))return;

	doImport(buffer);
	delete this;
}

void KviMircServersIniImport::die()
{
	delete this;
}







#define KVI_WWWMIRCCOUK_SERVERSINI "http://www.mirc.co.uk/servers.ini"



KviRemoteMircServerImportWizard::KviRemoteMircServerImportWizard(KviRemoteMircServersIniImport * f)
: KviTalWizard(0)
{
	QString capt = __tr2qs("Remote mIRC servers.ini Import Wizard");
	setCaption(capt);


	m_pRequest = 0;
	m_pFilter = f;

	QLabel * l = new QLabel(this);
	l->setText(__tr2qs("<center><b>Welcome!</b><br><br>This wizard will guide you in the process of " \
			"downloading a list of IRC servers. Please click \"<b>Next</b>\" to begin the operation.</center>"));
	addPage(l,capt);


	KviTalVBox * vb = new KviTalVBox(this);
	l = new QLabel(vb);
	l->setText(__tr2qs("<center>Here you can modify the URL that the list will be downloaded from. In most cases the default URL is acceptable.</center>"));

	vb->setStretchFactor(l,1);

	m_pUrlEdit = new QLineEdit(vb);

	m_pUrlEdit->setText(KVI_WWWMIRCCOUK_SERVERSINI);

	addPage(vb,__tr2qs("URL Selection"));

	vb = new KviTalVBox(this);

	l = new QLabel(__tr2qs("Please wait while the list is being downloaded"),vb);
	vb->setStretchFactor(l,1);

	m_pOutput = new QLabel(vb);
	m_pOutput->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

	addPage(vb,__tr2qs("List Download"));

	setBackEnabled(vb,false);
	setNextEnabled(vb,false);
	setFinishEnabled(vb,true);

	connect(this,SIGNAL(selected(const QString &)),this,SLOT(pageSelected(const QString &)));
}

KviRemoteMircServerImportWizard::~KviRemoteMircServerImportWizard()
{
	if(m_pRequest)delete m_pRequest;
}

void KviRemoteMircServerImportWizard::pageSelected(const QString &title)
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

void KviRemoteMircServerImportWizard::done(int r)
{
	m_pFilter->die();
}


void KviRemoteMircServerImportWizard::start()
{
	KviStr url = m_pUrlEdit->text();
	if(url.isEmpty())url = KVI_WWWMIRCCOUK_SERVERSINI;

	finishButton()->setEnabled(false);
	if(m_pRequest)delete m_pRequest;

	m_pRequest = new KviHttpRequest();
	connect(m_pRequest,SIGNAL(terminated(bool)),this,SLOT(getListTerminated(bool)));
	connect(m_pRequest,SIGNAL(status(const QString &)),this,SLOT(getListMessage(const QString &)));

	QString szTmp;
	g_pApp->getTmpFileName(szTmp,"servers.ini");
	m_szTmpFileName = szTmp;
	if(!m_pRequest->get(KviUrl(url.ptr()),KviHttpRequest::StoreToFile,m_szTmpFileName.ptr()))
	{
		delete m_pRequest;
		m_pRequest = 0;
		m_pOutput->setText(__tr2qs("Failed to start the server list transfer :("));
		finishButton()->setEnabled(true);
	}
}

void KviRemoteMircServerImportWizard::getListMessage(const QString &message)
{
	if(!message.isEmpty())m_pOutput->setText(message);
}


void KviRemoteMircServerImportWizard::getListTerminated(bool bSuccess)
{
	if(!m_pRequest)return;
	if(bSuccess)
	{
		m_pOutput->setText(__tr2qs("File downloaded: processing ..."));
		m_pOutput->repaint();
#ifndef COMPILE_ON_WINDOWS
		g_pApp->syncX();
#endif //!COMPILE_ON_WINDOWS

		int iCount = m_pFilter->doImport(m_szTmpFileName.ptr());

		QString tmp;
		if(iCount > 0)
			tmp = __tr2qs("%1 servers imported succesfully").arg(iCount);
		else
			tmp = __tr2qs("No servers imported");
		m_pOutput->setText(tmp);

		QDir d;
		d.remove(QString(m_szTmpFileName.ptr()));
	} else m_pOutput->setText(m_pRequest->lastError());

	delete m_pRequest;
	m_pRequest = 0;
	cancelButton()->setEnabled(false);
	finishButton()->setEnabled(true);
}


KviRemoteMircServersIniImport::KviRemoteMircServersIniImport(KviModuleExtensionDescriptor * d)
: KviMircServersIniImport(d)
{
	m_pWizard = 0;
}

KviRemoteMircServersIniImport::~KviRemoteMircServersIniImport()
{
	if(m_pWizard)delete m_pWizard;
	g_pRemoteMircServersIniImport = 0;
}

void KviRemoteMircServersIniImport::start()
{
	if(m_pWizard)delete m_pWizard;
	m_pWizard = new KviRemoteMircServerImportWizard(this);
	m_pWizard->show();
}

void KviRemoteMircServersIniImport::die()
{
	delete this;
}


static KviModuleExtension * mircimport_local_filter_alloc(KviModuleExtensionAllocStruct * s)
{
	if(g_pMircServersIniImport)delete g_pMircServersIniImport;
	g_pMircServersIniImport = new KviMircServersIniImport(s->pDescriptor);
	return g_pMircServersIniImport;
}

static KviModuleExtension * mircimport_remote_filter_alloc(KviModuleExtensionAllocStruct * s)
{
	if(g_pRemoteMircServersIniImport)delete g_pRemoteMircServersIniImport;
	g_pRemoteMircServersIniImport = new KviRemoteMircServersIniImport(s->pDescriptor);
	return g_pRemoteMircServersIniImport;
}

static bool mircimport_module_init(KviModule * m)
{

	KviStr szPath;
	QPixmap * pix = 0;
	if(g_pApp->findImage(szPath,"kvi_mircimport.png"))
	{
		pix = new QPixmap(szPath.ptr());
		if(pix->isNull())
		{
			delete pix;
			pix = 0;
		}
	}

	KviModuleExtensionDescriptor * d = m->registerExtension("serverimport",
							"mIRC servers.ini import filter",
							__tr("Import from servers.ini"),
							mircimport_local_filter_alloc);

	if(d && pix)d->setIcon(*pix);

	d = m->registerExtension("serverimport",
							"Remote mIRC servers.ini import filter",
							__tr("Import from http://www.mirc.co.uk/servers.ini"),
							mircimport_remote_filter_alloc);

	if(d && pix)d->setIcon(*pix);

	if(pix)delete pix;
	return true;
}

static bool mircimport_module_cleanup(KviModule *m)
{
	if(g_pMircServersIniImport)delete g_pMircServersIniImport;
	if(g_pRemoteMircServersIniImport)delete g_pRemoteMircServersIniImport;
    g_pMircServersIniImport = 0;
    g_pRemoteMircServersIniImport = 0;

	return true;
}

static bool mircimport_module_can_unload(KviModule * m)
{
	return ((!g_pMircServersIniImport) && (!g_pRemoteMircServersIniImport));
}

/*
KVIMODULEEXPORTFUNC KviIrcServerImport * mircimport_module_createIrcServerImport(const char *filterName)
{
	if(kvi_strEqualCI(filterName,__tr("Import from servers.ini")))
	{
		if(!g_pMircServersIniImport)g_pMircServersIniImport = new KviMircServersIniImport();
		return g_pMircServersIniImport;
	} else {
		if(!g_pRemoteMircServersIniImport)g_pRemoteMircServersIniImport = new KviRemoteMircServersIniImport("http://www.mirc.co.uk/servers.ini");
		return g_pRemoteMircServersIniImport;
	}
	return 0;
}
*/

KVIRC_MODULE(
	"File",                                                 // module name
	"1.0.0",                                                // module version
	"Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"mIRC servers.ini importer",
	mircimport_module_init,
	mircimport_module_can_unload,
	0,
	mircimport_module_cleanup
)

#include "libkvimircimport.moc"
