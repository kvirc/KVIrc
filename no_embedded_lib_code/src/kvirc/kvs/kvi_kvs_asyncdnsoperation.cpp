//=============================================================================
//
//   File : kvi_kvs_asyncdnsoperation.cpp
//   Creation date : Sun 10 Jul 2005 04:36:15 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_kvs_asyncdnsoperation.h"
#include "kvi_dns.h"
#include "kvi_kvs_script.h"
#include "kvi_kvs_variant.h"
#include "kvi_app.h"
#include "kvi_out.h"
#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_kvs_variantlist.h"

#include <QTimer>


KviKvsAsyncDnsOperation::KviKvsAsyncDnsOperation(KviWindow * pWnd,QString &szQuery,KviDns::QueryType eType,KviKvsScript * pCallback,KviKvsVariant * pMagic)
: KviKvsAsyncOperation(pWnd)
{
	m_pDns = new KviDns();
	m_szQuery = szQuery;
	m_eType = eType;
	m_pCallback = pCallback;
	m_pMagic = pMagic;
	connect(m_pDns,SIGNAL(lookupDone(KviDns *)),this,SLOT(lookupTerminated(KviDns *)));
	if(!m_pDns->lookup(szQuery,eType))
		QTimer::singleShot(10,this,SLOT(dnsStartFailed()));
}

KviKvsAsyncDnsOperation::~KviKvsAsyncDnsOperation()
{
	if(m_pDns)delete m_pDns;
	if(m_pMagic)delete m_pMagic;
	if(m_pCallback)delete m_pCallback;
}

void KviKvsAsyncDnsOperation::dnsStartFailed()
{
	lookupTerminated(m_pDns);
}

void KviKvsAsyncDnsOperation::lookupTerminated(KviDns *)
{
	KviWindow * pWnd = window();
	if(!g_pApp->windowExists(pWnd))pWnd = g_pActiveWindow;

	if(m_pCallback)
	{
		KviKvsVariantList params;
		params.setAutoDelete(true);
		if(m_pDns->state() == KviDns::Failure)
		{
			params.append(new KviKvsVariant(m_szQuery));
			params.append(new KviKvsVariant((kvs_int_t)0));
			params.append(new KviKvsVariant(KviError::getDescription(m_pDns->error())));
			params.append(new KviKvsVariant());
			params.append(new KviKvsVariant(*m_pMagic));
		} else {
			QString * fh = m_pDns->hostnameList()->first();
			QString * fi = m_pDns->ipAddressList()->first();

			params.append(new KviKvsVariant(m_szQuery));
			params.append(new KviKvsVariant((kvs_int_t)1));
			params.append(new KviKvsVariant(fi ? *fi : QString("?.?.?.?")));
			params.append(new KviKvsVariant(fh ? *fh : QString("?.?")));
			params.append(new KviKvsVariant(*m_pMagic));
		}

		m_pCallback->run(pWnd,&params,0,KviKvsScript::PreserveParams);

		delete this;
		return;
	}

	// we have no callback : output the results
	QString szQuery = m_pDns->query();
	pWnd->output(KVI_OUT_HOSTLOOKUP,__tr2qs_ctx("DNS Lookup result for query \"%Q\"","kvs"),&szQuery);

	if(m_pDns->state() == KviDns::Failure)
	{
		QString strDescription(KviError::getDescription(m_pDns->error()));
		pWnd->output(KVI_OUT_HOSTLOOKUP,__tr2qs_ctx("Error: %Q","kvs"),&strDescription);
	} else {
		int idx = 1;
		for(QString * h = m_pDns->hostnameList()->first();h;h = m_pDns->hostnameList()->next())
		{
			pWnd->output(KVI_OUT_HOSTLOOKUP,__tr2qs_ctx("Hostname %d: %Q","kvs"),idx,h);
			idx++;
		}
		idx = 1;
		for(QString * a = m_pDns->ipAddressList()->first();a;a = m_pDns->ipAddressList()->next())
		{
			pWnd->output(KVI_OUT_HOSTLOOKUP,__tr2qs_ctx("IP address %d: %Q","kvs"),idx,a);
			idx++;
		}
	}

	delete this;
}
