//=============================================================================
//
//   File : KviKvsAsyncDnsOperation.cpp
//   Creation date : Sun 10 Jul 2005 04:36:15 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsAsyncDnsOperation.h"
#include "KviDnsResolver.h"
#include "KviKvsScript.h"
#include "KviKvsVariant.h"
#include "KviApplication.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviError.h"
#include "KviKvsVariantList.h"

#include <QTimer>

KviKvsAsyncDnsOperation::KviKvsAsyncDnsOperation(KviWindow * pWnd, QString & szQuery, KviDnsResolver::QueryType eType, KviKvsScript * pCallback, KviKvsVariant * pMagic)
    : KviKvsAsyncOperation(pWnd)
{
	m_pDns = new KviDnsResolver();
	m_szQuery = szQuery;
	m_eType = eType;
	m_pCallback = pCallback;
	m_pMagic = pMagic;
	connect(m_pDns, SIGNAL(lookupDone(KviDnsResolver *)), this, SLOT(lookupTerminated(KviDnsResolver *)));
	if(!m_pDns->lookup(szQuery, eType))
		QTimer::singleShot(10, this, SLOT(dnsStartFailed()));
}

KviKvsAsyncDnsOperation::~KviKvsAsyncDnsOperation()
{
	delete m_pDns;
	delete m_pMagic;
	delete m_pCallback;
}

void KviKvsAsyncDnsOperation::dnsStartFailed()
{
	lookupTerminated(m_pDns);
}

void KviKvsAsyncDnsOperation::lookupTerminated(KviDnsResolver *)
{
	KviWindow * pWnd = window();
	if(!g_pApp->windowExists(pWnd))
		pWnd = g_pActiveWindow;

	if(m_pCallback)
	{
		KviKvsVariantList params;
		params.setAutoDelete(true);
		if(m_pDns->state() == KviDnsResolver::Failure)
		{
			params.append(new KviKvsVariant(m_szQuery));
			params.append(new KviKvsVariant((kvs_int_t)0));
			params.append(new KviKvsVariant(m_pDns->errorString()));
			params.append(new KviKvsVariant());
			params.append(new KviKvsVariant(*m_pMagic));
		}
		else
		{
			QString szHostName = m_pDns->hostName();
			const auto & strL = m_pDns->ipAddressList();
			const QString & fi = strL.empty() ? QString("?.?.?.?") : strL.front();

			params.append(new KviKvsVariant(m_szQuery));
			params.append(new KviKvsVariant((kvs_int_t)1));
			params.append(new KviKvsVariant(fi));
			params.append(new KviKvsVariant(szHostName.isEmpty() ? QString("?.?") : szHostName));
			params.append(new KviKvsVariant(*m_pMagic));
		}

		m_pCallback->run(pWnd, &params, nullptr, KviKvsScript::PreserveParams);

		delete this;
		return;
	}

	// we have no callback : output the results
	QString szQuery = m_pDns->query();
	pWnd->output(KVI_OUT_HOSTLOOKUP, __tr2qs_ctx("DNS lookup result for query \"%Q\"", "kvs"), &szQuery);

	if(m_pDns->state() == KviDnsResolver::Failure)
	{
		QString strDescription(m_pDns->errorString());
		pWnd->output(KVI_OUT_HOSTLOOKUP, __tr2qs_ctx("Error: %Q", "kvs"), &strDescription);
	}
	else
	{
		QString szHostName = m_pDns->hostName();
		pWnd->output(KVI_OUT_HOSTLOOKUP, __tr2qs_ctx("Hostname: %Q", "kvs"), &szHostName);
		int idx = 1;
		for(const auto & a : m_pDns->ipAddressList())
		{
			pWnd->output(KVI_OUT_HOSTLOOKUP, __tr2qs_ctx("IP address %d: %Q", "kvs"), idx, &a);
			idx++;
		}
	}

	delete this;
}
