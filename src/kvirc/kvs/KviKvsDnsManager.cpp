//=============================================================================
//
//   File : KviKvsDnsManager.cpp
//   Creation date : Wed 04 Aug 2004 04:38:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsDnsManager.h"
#include "KviDnsResolver.h"
#include "KviApplication.h"
#include "KviWindow.h"
#include "kvi_out.h"
#include "KviIconManager.h"
#include "KviKvsScript.h"
#include "KviKvsVariantList.h"
#include "KviCString.h"
#include "KviLocale.h"
#include "KviError.h"

KviKvsDnsManager * KviKvsDnsManager::m_pInstance = nullptr;

KviKvsDnsObject::KviKvsDnsObject(KviDnsResolver * pDns, KviWindow * pWnd, const QString & szQuery, bool bRebindOnWindowClose, KviKvsScript * pCallback, KviKvsVariantList * pParameterList)
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
	QObject::disconnect(m_pDns, nullptr, nullptr, nullptr);
	if(m_pDns->isRunning())
	{
		//g_pApp->collectGarbage(m_pDns);
		m_pDns->deleteLater();
	}
	else
	{
		delete m_pDns;
	}
	if(m_pCallback)
		delete m_pCallback;
	if(m_pParameterList)
		delete m_pParameterList;
}

KviKvsDnsManager::KviKvsDnsManager()
    : QObject()
{
	m_pDnsObjects = nullptr;
}

KviKvsDnsManager::~KviKvsDnsManager()
{
	if(m_pDnsObjects)
		delete m_pDnsObjects;
}

void KviKvsDnsManager::init()
{
	if(KviKvsDnsManager::m_pInstance)
	{
		qDebug("Trying to double init() the DNS manager!");
		return;
	}
	KviKvsDnsManager::m_pInstance = new KviKvsDnsManager();
}

void KviKvsDnsManager::done()
{
	if(!KviKvsDnsManager::m_pInstance)
	{
		qDebug("Trying to call done() on a non existing DNS manager!");
		return;
	}
	delete KviKvsDnsManager::m_pInstance;
	KviKvsDnsManager::m_pInstance = nullptr;
}

void KviKvsDnsManager::addDns(KviKvsDnsObject * pObject)
{
	if(!m_pDnsObjects)
	{
		m_pDnsObjects = new KviPointerHashTable<void *, KviKvsDnsObject>;
		m_pDnsObjects->setAutoDelete(true);
	}
	m_pDnsObjects->replace(pObject->dns(), pObject);
	connect(pObject->dns(), SIGNAL(lookupDone(KviDnsResolver *)), this, SLOT(dnsLookupTerminated(KviDnsResolver *)));
}

void KviKvsDnsManager::dnsLookupTerminated(KviDnsResolver * pDns)
{
	KviKvsDnsObject * o = m_pDnsObjects->find(pDns);
	if(!o)
	{
		qDebug("KviKvsDnsManager::dnsLookupTerminated(): can't find the KviKvsDnsObject structure");
		return;
	}

	if(!g_pApp->windowExists(o->window()))
	{
		if(o->rebindOnWindowClose())
		{
			o->setWindow(g_pActiveWindow);
		}
		else
		{
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

		if(o->dns()->state() == KviDnsResolver::Failure)
		{
			// $4... is the magic data
			o->parameterList()->prepend(new KviKvsVariant());                        // $3
			o->parameterList()->prepend(new KviKvsVariant(o->dns()->errorString())); // $2
			o->parameterList()->prepend(new KviKvsVariant((kvs_int_t)0));            // $1
		}
		else
		{
			QString szHostName = o->dns()->hostName();
			const auto & strL = o->dns()->ipAddressList();
			const QString & fi = strL.empty() ? QString("?.?.?.?") : strL.front();

			// $4... is the magic data
			o->parameterList()->prepend(new KviKvsVariant(szHostName.isEmpty() ? QString("?.?") : szHostName)); // $3
			o->parameterList()->prepend(new KviKvsVariant(fi));                                                 // $2
			o->parameterList()->prepend(new KviKvsVariant((kvs_int_t)1));                                       // $1
		}
		o->parameterList()->prepend(new KviKvsVariant(o->query())); // $0

		copy.run(o->window(), o->parameterList(), nullptr, KviKvsScript::PreserveParams);
	}
	else
	{
		QString szQuery = o->dns()->query();
		o->window()->output(KVI_OUT_HOSTLOOKUP, __tr2qs_ctx("DNS lookup result for query \"%Q\"", "kvs"), &szQuery);

		if(o->dns()->state() == KviDnsResolver::Failure)
		{
			QString szErr = o->dns()->errorString();
			o->window()->output(KVI_OUT_HOSTLOOKUP, __tr2qs_ctx("Error: %Q", "kvs"), &szErr);
		}
		else
		{
			QString szHostName = o->dns()->hostName();
			o->window()->output(KVI_OUT_HOSTLOOKUP, __tr2qs_ctx("Hostname: %Q", "kvs"), &szHostName);
			int idx = 1;
			for(const auto & a : o->dns()->ipAddressList())
			{
				o->window()->output(KVI_OUT_HOSTLOOKUP, __tr2qs_ctx("IP address %d: %Q", "kvs"), idx, &a);
				idx++;
			}
		}
	}

	m_pDnsObjects->remove(pDns);
}
