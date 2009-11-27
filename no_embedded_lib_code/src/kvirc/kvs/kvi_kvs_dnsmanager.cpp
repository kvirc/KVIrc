//=============================================================================
//
//   File : kvi_kvs_dnsmanager.cpp
//   Creation date : Wed 04 Aug 2004 04:38:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_kvs_dnsmanager.h"
#include "kvi_dns.h"
#include "kvi_app.h"
#include "kvi_window.h"
#include "kvi_out.h"
#include "kvi_iconmanager.h"
#include "kvi_kvs_script.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_string.h"
#include "kvi_locale.h"
#include "kvi_error.h"

KviKvsDnsManager * KviKvsDnsManager::m_pInstance = 0;


KviKvsDnsObject::KviKvsDnsObject(KviDns * pDns,KviWindow * pWnd,const QString &szQuery,bool bRebindOnWindowClose,KviKvsScript * pCallback,KviKvsVariantList * pParameterList)
{
	m_pDns = pDns;
	m_pWindow = pWnd;
	m_szQuery = szQuery;
	m_pCallback = pCallback;
	m_pParameterList = pParameterList;
	m_bRebindOnWindowClose = bRebindOnWindowClose;
}

KviKvsDnsObject::~KviKvsDnsObject()
{
	QObject::disconnect(m_pDns,0,0,0);
	if(m_pDns->isRunning())
	{
		//g_pApp->collectGarbage(m_pDns);
		m_pDns->deleteLater();
	} else {
		delete m_pDns;
	}
	if(m_pCallback)delete m_pCallback;
	if(m_pParameterList)delete m_pParameterList;
}


KviKvsDnsManager::KviKvsDnsManager()
: QObject()
{
	m_pDnsObjects = 0;
}

KviKvsDnsManager::~KviKvsDnsManager()
{
	if(m_pDnsObjects)delete m_pDnsObjects;
}

void KviKvsDnsManager::init()
{
	if(KviKvsDnsManager::m_pInstance)
	{
		debug("Trying to double init() the dns manager!");
		return;
	}
	KviKvsDnsManager::m_pInstance = new KviKvsDnsManager();
}

void KviKvsDnsManager::done()
{
	if(!KviKvsDnsManager::m_pInstance)
	{
		debug("Trying to call done() on a non existing dns manager!");
		return;
	}
	delete KviKvsDnsManager::m_pInstance;
	KviKvsDnsManager::m_pInstance = 0;
}

void KviKvsDnsManager::addDns(KviKvsDnsObject * pObject)
{
	if(!m_pDnsObjects)
	{
		m_pDnsObjects = new KviPointerHashTable<void *,KviKvsDnsObject>;
		m_pDnsObjects->setAutoDelete(true);
	}
	m_pDnsObjects->replace(pObject->dns(),pObject);
	connect(pObject->dns(),SIGNAL(lookupDone(KviDns *)),this,SLOT(dnsLookupTerminated(KviDns *)));
}

void KviKvsDnsManager::dnsLookupTerminated(KviDns * pDns)
{
	KviKvsDnsObject * o = m_pDnsObjects->find(pDns);
	if(!o)
	{
		debug("KviKvsDnsManager::dnsLookupTerminated(): can't find the KviKvsDnsObject structure");
		return;
	}

	if(!g_pApp->windowExists(o->window()))
	{
		if(o->rebindOnWindowClose())
		{
			o->setWindow(g_pActiveWindow);
		} else {
			// just kill it
			m_pDnsObjects->remove(pDns);
			return;
		}
	}

	if(o->callback())
	{
		KviKvsScript copy(*(o->callback()));
		if(!o->parameterList())
			o->setParameterList(new KviKvsVariantList());

		if(o->dns()->state() == KviDns::Failure)
		{
			// $4... is the magic data
			o->parameterList()->prepend(new KviKvsVariant()); // $3
			o->parameterList()->prepend(new KviKvsVariant(KviError::getDescription(o->dns()->error()))); // $2
			o->parameterList()->prepend(new KviKvsVariant((kvs_int_t)0)); // $1
		} else {
			QString * fh = o->dns()->hostnameList()->first();
			QString * fi = o->dns()->ipAddressList()->first();

			// $4... is the magic data
			o->parameterList()->prepend(new KviKvsVariant(fh ? *fh : QString("?.?"))); // $3
			o->parameterList()->prepend(new KviKvsVariant(fi ? *fi : QString("?.?.?.?"))); // $2
			o->parameterList()->prepend(new KviKvsVariant((kvs_int_t)1)); // $1
		}
		o->parameterList()->prepend(new KviKvsVariant(o->query())); // $0

		copy.run(o->window(),o->parameterList(),0,KviKvsScript::PreserveParams);

	} else {
		QString szQuery = o->dns()->query();
		o->window()->output(KVI_OUT_HOSTLOOKUP,__tr2qs_ctx("DNS Lookup result for query \"%Q\"","kvs"),&szQuery);

		if(o->dns()->state() == KviDns::Failure)
		{
			QString szErr = KviError::getDescription(o->dns()->error());
			o->window()->output(KVI_OUT_HOSTLOOKUP,__tr2qs_ctx("Error: %Q","kvs"),&szErr);
		} else {
			int idx = 1;
			for(QString * h = o->dns()->hostnameList()->first();h;h = o->dns()->hostnameList()->next())
			{
				o->window()->output(KVI_OUT_HOSTLOOKUP,__tr2qs_ctx("Hostname %d: %Q","kvs"),idx,h);
				idx++;
			}
			idx = 1;
			for(QString * a = o->dns()->ipAddressList()->first();a;a = o->dns()->ipAddressList()->next())
			{
				o->window()->output(KVI_OUT_HOSTLOOKUP,__tr2qs_ctx("IP address %d: %Q","kvs"),idx,a);
				idx++;
			}
		}
	}

	m_pDnsObjects->remove(pDns);
}
