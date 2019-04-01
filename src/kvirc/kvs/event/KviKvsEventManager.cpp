//=============================================================================
//
//   File : KviKvsEventManager.cpp
//   Creation date : Thu Aug 17 2000 13:59:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsEventManager.h"
#include "KviConfigurationFile.h"
#include "KviKvsScript.h"
#include "KviKvsVariant.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviModule.h"
#include "KviWindow.h"
#include "KviKvsVariantList.h"

#include <QRegExp>

/*
	@doc: events
	@type:
		language
	@keyterms:
		events,event handlers,event
	@title:
		Events
	@short:
		Events: user reactions
	@body:
		KVIrc triggers an event when a particular situation occurs.[br]
		You can define a set of event handlers for each event type.[br]
		An event handler is a snippet of user-defined code that gets executed when the event is triggered.[br]
		Event handlers can be created or destroyed by using the scriptcenter (graphic interface)
		or even from the commandline (or script) by using the [cmd]event[/cmd] command.[br]
		For example, the [event:onirc]OnIRC[/event] is triggered when the login operations have
		been terminated and you can consider yourself [i]completely[/i] on IRC. For example, you might
		want to [i]auto-join[/i] some channels. Nothing easier! The following snippet of code
		adds a handler to the OnIRC event that joins three channels:
		[example]
		[cmd]event[/cmd](OnIRC,autojoin)
		{
			[cmd]echo[/cmd] Auto-joining my preferred channels...
			[cmd]join[/cmd] #kvirc,#siena,#linux
		}
		[/example]
		Now try to connect to a server and you'll see that it joins automatically the three channels!.[br]
		You might also want to do some other actions just after the connection has been established,
		for example you might want to look immediately for a friend of yours by issuing a [cmd]whois[/cmd]
		to the server (you could use the notify list for that).[br]
		You can add the [cmd]whois[/cmd] request to the handler above or just create a new one:
		[example]
		[cmd]event[/cmd](OnIRC,lookforfred)
		{
			[cmd]echo[/cmd] Looking for fred...
			[cmd]whois[/cmd] fred
		}
		[/example]
		(An even nicer idea would be to use the [cmd]awhois[/cmd] command, but that's left to the reader as an exercise.[br]
		To remove an event handler you still use the [cmd]event[/cmd] command, but with an empty code block:
		[example]
		[cmd]event[/cmd](OnIRC,lookforfred){}[br]
		[/example]
		[br]
		Certain events will pass you some data in the positional parameters.[br]
		For example, when you are being banned from a channel, KVIrc triggers the [event:onmeban]OnMeBan[/event]
		event: you might be interested in WHO has banned you. KVIrc will pass the [i]ban source[/i] information
		in the positional parameters $0,$1 and $2.[br]
		(Please note that the parameters started from $1 in KVIrc versions older than 3.0.0!).[br]
		You may take a look at the list of available [doc:event_index_all]events[/doc].[br]
*/

KviKvsEventManager * KviKvsEventManager::m_pInstance = nullptr;

KviKvsEventManager::KviKvsEventManager()
{
	m_pInstance = this;
	for(auto & i : m_rawEventTable)
		i = nullptr;
}

KviKvsEventManager::~KviKvsEventManager()
{
	clear();
}

void KviKvsEventManager::init()
{
	if(KviKvsEventManager::instance())
	{
		qDebug("WARNING: trying to create KviKvsEventManager twice!");
		return;
	}
	(void)new KviKvsEventManager();
}

void KviKvsEventManager::done()
{
	if(!KviKvsEventManager::instance())
	{
		qDebug("WARNING: trying to destroy the KviKvsEventManager twice!");
		return;
	}
	delete KviKvsEventManager::instance();
}

unsigned int KviKvsEventManager::findAppEventIndexByName(const QString & szName)
{
	for(unsigned int u = 0; u < KVI_KVS_NUM_APP_EVENTS; u++)
	{
		if(KviQString::equalCI(szName, m_appEventTable[u].name()))
			return u;
		//Backwards compatibility >_<
		if((u == 4) && KviQString::equalCI(szName, "OnIrcConnectionEstablished"))
			return u;
	}
	return KVI_KVS_NUM_APP_EVENTS; // <-- invalid event number
}

KviKvsEvent * KviKvsEventManager::findAppEventByName(const QString & szName)
{
	for(unsigned int u = 0; u < KVI_KVS_NUM_APP_EVENTS; u++)
	{
		if(KviQString::equalCI(szName, m_appEventTable[u].name()))
			return &(m_appEventTable[u]);
		//Backwards compatibility >_<
		if((u == 4) && KviQString::equalCI(szName, "OnIrcConnectionEstablished"))
			return &(m_appEventTable[u]);
	}
	return nullptr;
}

bool KviKvsEventManager::addAppHandler(unsigned int uEvIdx, KviKvsEventHandler * h)
{
	if(uEvIdx >= KVI_KVS_NUM_APP_EVENTS)
		return false;
	m_appEventTable[uEvIdx].addHandler(h);
	return true;
}

bool KviKvsEventManager::addRawHandler(unsigned int uRawIdx, KviKvsEventHandler * h)
{
	if(uRawIdx >= KVI_KVS_NUM_RAW_EVENTS)
		return false;
	if(!m_rawEventTable[uRawIdx])
	{
		m_rawEventTable[uRawIdx] = new KviPointerList<KviKvsEventHandler>();
		m_rawEventTable[uRawIdx]->setAutoDelete(true);
	}
	m_rawEventTable[uRawIdx]->append(h);
	return true;
}

bool KviKvsEventManager::addRawHandler(unsigned int uRawIdx, const KviKvsEventHandler & h)
{
	if(uRawIdx >= KVI_KVS_NUM_RAW_EVENTS)
		return false;
	KviKvsEventHandler * copy = new KviKvsEventHandler(h);
	return addRawHandler(uRawIdx, copy);
}

bool KviKvsEventManager::removeScriptAppHandler(unsigned int uEvIdx, const QString & szName)
{
	if(uEvIdx >= KVI_KVS_NUM_APP_EVENTS)
		return false;
	KviKvsEventHandler * h;
	if(!(m_appEventTable[uEvIdx].handlers()))
		return false;
	for(h = m_appEventTable[uEvIdx].handlers()->first(); h; h = m_appEventTable[uEvIdx].handlers()->next())
	{
		if(h->type() == KviKvsEventHandler::Script)
		{
			if(KviQString::equalCI(((KviKvsScriptEventHandler *)h)->name(), szName))
			{
				m_appEventTable[uEvIdx].removeHandler(h);
				return true;
			}
		}
	}
	return false;
}

KviKvsScriptEventHandler * KviKvsEventManager::findScriptRawHandler(unsigned int uEvIdx, const QString & szName)
{
	if(uEvIdx >= KVI_KVS_NUM_RAW_EVENTS)
		return nullptr;
	if(!m_rawEventTable[uEvIdx])
		return nullptr;
	KviKvsEventHandler * h;
	for(h = m_rawEventTable[uEvIdx]->first(); h; h = m_rawEventTable[uEvIdx]->next())
	{
		if(h->type() == KviKvsEventHandler::Script)
		{
			if(KviQString::equalCI(((KviKvsScriptEventHandler *)h)->name(), szName))
			{
				return (KviKvsScriptEventHandler *)h;
			}
		}
	}
	return nullptr;
}

KviKvsScriptEventHandler * KviKvsEventManager::findScriptAppHandler(unsigned int uEvIdx, const QString & szName)
{
	if(uEvIdx >= KVI_KVS_NUM_APP_EVENTS)
		return nullptr;
	KviKvsEventHandler * h;
	if(!(m_appEventTable[uEvIdx].handlers()))
		return nullptr;
	for(h = m_appEventTable[uEvIdx].handlers()->first(); h; h = m_appEventTable[uEvIdx].handlers()->next())
	{
		if(h->type() == KviKvsEventHandler::Script)
		{
			if(KviQString::equalCI(((KviKvsScriptEventHandler *)h)->name(), szName))
			{
				return (KviKvsScriptEventHandler *)h;
			}
		}
	}
	return nullptr;
}

bool KviKvsEventManager::enableScriptAppHandler(unsigned int uEvIdx, const QString & szName, bool bEnable)
{
	KviKvsScriptEventHandler * h = findScriptAppHandler(uEvIdx, szName);
	if(!h)
		return false;
	h->setEnabled(bEnable);
	return true;
}

bool KviKvsEventManager::removeModuleAppHandler(unsigned int uEvIdx, KviKvsModuleInterface * i)
{
	if(uEvIdx >= KVI_KVS_NUM_APP_EVENTS)
		return false;
	KviKvsEventHandler * h;
	if(!(m_appEventTable[uEvIdx].handlers()))
		return false;
	for(h = m_appEventTable[uEvIdx].handlers()->first(); h; h = m_appEventTable[uEvIdx].handlers()->next())
	{
		if(h->type() == KviKvsEventHandler::Module)
		{
			if(((KviKvsModuleEventHandler *)h)->moduleInterface() == i)
			{
				m_appEventTable[uEvIdx].removeHandler(h);
				return true;
			}
		}
	}
	return false;
}

void KviKvsEventManager::removeAllModuleAppHandlers(KviKvsModuleInterface * pIface)
{
	KviKvsEventHandler * h;
	for(auto & i : m_appEventTable)
	{
		if(!i.handlers())
			continue;

		KviPointerList<KviKvsEventHandler> l;
		l.setAutoDelete(false);
		for(h = i.handlers()->first(); h; h = i.handlers()->next())
		{
			if(h->type() == KviKvsEventHandler::Module)
			{
				if(((KviKvsModuleEventHandler *)h)->moduleInterface() == pIface)
				{
					l.append(h);
				}
			}
		}
		for(h = l.first(); h; h = l.next())
			i.removeHandler(h);
	}
}

void KviKvsEventManager::removeAllModuleRawHandlers(KviKvsModuleInterface * pIface)
{
	KviKvsEventHandler * h;
	for(auto & i : m_rawEventTable)
	{
		if(!i)
			continue;

		KviPointerList<KviKvsEventHandler> l;
		l.setAutoDelete(false);
		for(h = i->first(); h; h = i->next())
		{
			if(h->type() == KviKvsEventHandler::Module)
			{
				if(((KviKvsModuleEventHandler *)h)->moduleInterface() == pIface)
				{
					l.append(h);
				}
			}
		}
		for(h = l.first(); h; h = l.next())
			i->removeRef(h);
		if(i->isEmpty())
		{
			delete i;
			i = nullptr;
		}
	}
}

bool KviKvsEventManager::removeScriptRawHandler(unsigned int uEvIdx, const QString & szName)
{
	if(uEvIdx >= KVI_KVS_NUM_RAW_EVENTS)
		return false;
	if(!m_rawEventTable[uEvIdx])
		return false;
	KviKvsEventHandler * h;
	for(h = m_rawEventTable[uEvIdx]->first(); h; h = m_rawEventTable[uEvIdx]->next())
	{
		if(h->type() == KviKvsEventHandler::Script)
		{
			if(KviQString::equalCI(((KviKvsScriptEventHandler *)h)->name(), szName))
			{
				m_rawEventTable[uEvIdx]->removeRef(h);
				if(m_rawEventTable[uEvIdx]->isEmpty())
				{
					delete m_rawEventTable[uEvIdx];
					m_rawEventTable[uEvIdx] = nullptr;
				}
				return true;
			}
		}
	}
	return false;
}

bool KviKvsEventManager::enableScriptRawHandler(unsigned int uEvIdx, const QString & szName, bool bEnable)
{
	KviKvsScriptEventHandler * h = findScriptRawHandler(uEvIdx, szName);
	if(!h)
		return false;
	h->setEnabled(bEnable);
	return true;
}

bool KviKvsEventManager::removeModuleRawHandler(unsigned int uRawIdx, KviKvsModuleInterface * i)
{
	if(uRawIdx >= KVI_KVS_NUM_RAW_EVENTS)
		return false;
	if(!m_rawEventTable[uRawIdx])
		return false;
	KviKvsEventHandler * h;
	for(h = m_rawEventTable[uRawIdx]->first(); h; h = m_rawEventTable[uRawIdx]->next())
	{
		if(h->type() == KviKvsEventHandler::Module)
		{
			if(((KviKvsModuleEventHandler *)h)->moduleInterface() == i)
			{
				m_rawEventTable[uRawIdx]->removeRef(h);
				if(m_rawEventTable[uRawIdx]->isEmpty())
				{
					delete m_rawEventTable[uRawIdx];
					m_rawEventTable[uRawIdx] = nullptr;
				}
				return true;
			}
		}
	}
	return false;
}

void KviKvsEventManager::removeAllModuleHandlers(KviKvsModuleInterface * pIface)
{
	removeAllModuleAppHandlers(pIface);
	removeAllModuleRawHandlers(pIface);
}

void KviKvsEventManager::removeAllScriptAppHandlers()
{
	for(auto & i : m_appEventTable)
	{
		i.clearScriptHandlers();
	}
}

void KviKvsEventManager::removeAllScriptRawHandlers()
{
	for(auto & i : m_rawEventTable)
	{
		if(i)
		{
			KviPointerList<KviKvsEventHandler> dl;
			dl.setAutoDelete(false);
			KviKvsEventHandler * e;
			for(e = i->first(); e; e = i->next())
			{
				if(e->type() == KviKvsEventHandler::Script)
					dl.append(e);
			}

			for(e = dl.first(); e; e = dl.next())
			{
				i->removeRef(e);
			}

			if(i->isEmpty())
			{
				delete i;
				i = nullptr;
			}
		}
	}
}

void KviKvsEventManager::clearRawEvents()
{
	for(auto & i : m_rawEventTable)
	{
		if(i)
			delete i;
		i = nullptr;
	}
}

void KviKvsEventManager::clearAppEvents()
{
	for(auto & i : m_appEventTable)
	{
		i.clear();
	}
}

void KviKvsEventManager::clear()
{
	clearRawEvents();
	clearAppEvents();
}

bool KviKvsEventManager::triggerHandlers(KviPointerList<KviKvsEventHandler> * pHandlers, KviWindow * pWnd, KviKvsVariantList * pParams)
{
	if(!pHandlers)
		return false;

	bool bGotHalt = false;
	for(KviKvsEventHandler * h = pHandlers->first(); h; h = pHandlers->next())
	{
		switch(h->type())
		{
			case KviKvsEventHandler::Script:
			{
				if(((KviKvsScriptEventHandler *)h)->isEnabled())
				{
					KviKvsScript * s = ((KviKvsScriptEventHandler *)h)->script();
					KviKvsScript copy(*s);
					KviKvsVariant retVal;
					int iRet = copy.run(pWnd, pParams, &retVal, KviKvsScript::PreserveParams);
					if(!iRet)
					{
						// error! disable the handler if it's broken
						if(KVI_OPTION_BOOL(KviOption_boolDisableBrokenEventHandlers))
						{
							((KviKvsScriptEventHandler *)h)->setEnabled(false);
							pWnd->output(KVI_OUT_PARSERERROR, __tr2qs_ctx("Event handler %Q is broken: disabling", "kvs"), &(s->name()));
							emit eventHandlerDisabled(s->name());
						}
					}
					if(!bGotHalt)
						bGotHalt = (iRet & KviKvsScript::HaltEncountered);
				}
			}
			break;
			case KviKvsEventHandler::Module:
			{
				KviModule * m = (KviModule *)((KviKvsModuleEventHandler *)h)->moduleInterface();
				KviKvsModuleEventHandlerRoutine * proc = ((KviKvsModuleEventHandler *)h)->handlerRoutine();
				KviKvsVariant retVal;
				KviKvsRunTimeContext ctx(nullptr, pWnd, pParams, &retVal);
				KviKvsModuleEventCall call(m, &ctx, pParams);
				if(!(*proc)(&call))
					bGotHalt = true;
			}
			break;
		}
	}
	return bGotHalt;
}

void KviKvsEventManager::loadRawEvents(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Read);
	removeAllScriptRawHandlers();

	for(size_t i = 0; i < KVI_KVS_NUM_RAW_EVENTS; i++)
	{
		QString szTmp = QString("RAW%1").arg(i);
		if(cfg.hasGroup(szTmp))
		{
			cfg.setGroup(szTmp);
			unsigned int uHandlers = cfg.readUIntEntry("NHandlers", 0);
			if(uHandlers)
			{
				m_rawEventTable[i] = new KviPointerList<KviKvsEventHandler>();
				m_rawEventTable[i]->setAutoDelete(true);
				for(unsigned int uIdx = 0; uIdx < uHandlers; uIdx++)
				{
					szTmp = QString("Name%1").arg(uIdx);
					QString szName = cfg.readEntry(szTmp, "unnamed");
					szTmp = QString("Buffer%1").arg(uIdx);
					QString szCode = cfg.readEntry(szTmp, "");
					szTmp = QString("RawEvent%1::%2").arg(uIdx).arg(szName);
					KviKvsScriptEventHandler * pScript = new KviKvsScriptEventHandler(szName, szTmp, szCode);
					szTmp = QString("Enabled%1").arg(uIdx);
					pScript->setEnabled(cfg.readBoolEntry(szTmp, false));
					m_rawEventTable[i]->append(pScript);
				}
			}
		}
	}
}

void KviKvsEventManager::saveRawEvents(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Write);
	cfg.clear();

	for(size_t i = 0; i < KVI_KVS_NUM_RAW_EVENTS; i++)
	{
		if(m_rawEventTable[i])
		{
			QString szTmp = QString("RAW%1").arg(i);
			cfg.setGroup(szTmp);

			int iIdx = 0;
			for(KviKvsEventHandler * pEvent = m_rawEventTable[i]->first(); pEvent; pEvent = m_rawEventTable[i]->next())
			{
				if(pEvent->type() == KviKvsEventHandler::Script)
				{
					szTmp = QString("Name%1").arg(iIdx);
					cfg.writeEntry(szTmp, ((KviKvsScriptEventHandler *)pEvent)->name());
					szTmp = QString("Buffer%1").arg(iIdx);
					cfg.writeEntry(szTmp, ((KviKvsScriptEventHandler *)pEvent)->code());
					szTmp = QString("Enabled%1").arg(iIdx);
					cfg.writeEntry(szTmp, ((KviKvsScriptEventHandler *)pEvent)->isEnabled());
					iIdx++;
				}
			}
			cfg.writeEntry("NHandlers", iIdx);
		}
	}
}

void KviKvsEventManager::loadAppEvents(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Read);
	removeAllScriptAppHandlers();

	for(size_t i = 0; i < KVI_KVS_NUM_APP_EVENTS; i++)
	{
		QString szEventName(m_appEventTable[i].name());
		// Backwards compatibility >_<
		if((i == 4) && !cfg.hasGroup(szEventName))
			szEventName = "OnIrcConnectionEstablished";
		if(cfg.hasGroup(szEventName))
		{
			cfg.setGroup(szEventName);
			unsigned int uHandlers = cfg.readUIntEntry("NHandlers", 0);
			if(uHandlers)
			{
				for(unsigned int uIdx = 0; uIdx < uHandlers; uIdx++)
				{
					QString szTmp = QString("Name%1").arg(uIdx);
					QString szName = cfg.readEntry(szTmp, "unnamed");
					szTmp = QString("Buffer%1").arg(uIdx);
					QString szCode = cfg.readEntry(szTmp, "");
					szTmp = QString("Enabled%1").arg(uIdx);
					bool bEnabled = cfg.readBoolEntry(szTmp, false);
					QString szCntx = QString("%1::%2").arg(m_appEventTable[i].name(), szName);
					KviKvsScriptEventHandler * pEvent = new KviKvsScriptEventHandler(szName, szCntx, szCode, bEnabled);
					m_appEventTable[i].addHandler(pEvent);
				}
			}
		}
	}
}

void KviKvsEventManager::saveAppEvents(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Write);
	cfg.clear();
	bool bCompat = false;

	for(size_t i = 0; i < KVI_KVS_NUM_APP_EVENTS; i++)
	{
		if(m_appEventTable[i].hasHandlers())
		{
			QString szEventName(m_appEventTable[i].name());
			// Backwards compatibility >_<
			if((i == 4) && cfg.hasGroup(szEventName))
			{
				szEventName = "OnIRCConnectionEstablished";
				bCompat = true;
			}
			cfg.setGroup(szEventName);
			int iIdx = 0;
			for(KviKvsEventHandler * pEvent = m_appEventTable[i].handlers()->first(); pEvent; pEvent = m_appEventTable[i].handlers()->next())
			{
				if(pEvent->type() == KviKvsEventHandler::Script)
				{
					QString szTmp = QString("Name%1").arg(iIdx);
					cfg.writeEntry(szTmp, ((KviKvsScriptEventHandler *)pEvent)->name());
					szTmp = QString("Buffer%1").arg(iIdx);
					cfg.writeEntry(szTmp, ((KviKvsScriptEventHandler *)pEvent)->code());
					szTmp = QString("Enabled%1").arg(iIdx);
					cfg.writeEntry(szTmp, ((KviKvsScriptEventHandler *)pEvent)->isEnabled());
					iIdx++;
				}
			}
			cfg.writeEntry("NHandlers", iIdx);

			// Backwards compatibility >_<
			if((i == 4) && !bCompat)
				i--;
		}
	}
}

void KviKvsEventManager::cleanHandlerName(QString & szHandlerName)
{
	static QRegExp re(KVI_KVS_EVENT_HANDLER_NAME_INVALID_CHARS_REG_EXP);
	szHandlerName.replace(re, "");
	if (szHandlerName.isEmpty())
		szHandlerName = "unnamed";
}
