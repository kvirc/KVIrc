//=============================================================================
//
//   File : KviKvsParameterProcessor.cpp
//   Creation date : Sun 17 Apr 2005 16:47:09 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
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

#include "KviKvsParameterProcessor.h"
#include "KviKvsVariantList.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsArray.h"
#include "KviKvsArrayCast.h"
#include "KviKvsHash.h"
#include "KviKvsObject.h"
#include "KviQString.h"
#include "KviLocale.h"
#include "KviPointerList.h"

#include <QStringList>

namespace KviKvsParameterProcessor
{
	void setDefaultValue(KviKvsParameterProcessor::ParameterFormat * pFmtArray)
	{
		switch(pFmtArray->uType)
		{
			case KVS_PT_STRING:
			case KVS_PT_NONEMPTYSTRING:
				*((QString *)(pFmtArray->pContainer)) = QString();
				break;
			case KVS_PT_INT:
				*((kvs_int_t *)(pFmtArray->pContainer)) = 0;
				break;
			case KVS_PT_UINT:
				*((kvs_uint_t *)(pFmtArray->pContainer)) = 0;
				break;
			case KVS_PT_DOUBLE:
				*((kvs_real_t *)(pFmtArray->pContainer)) = 0.0;
				break;
			case KVS_PT_BOOL:
				*((bool *)(pFmtArray->pContainer)) = false;
				break;
			case KVS_PT_HASH:
				*((KviKvsHash **)(pFmtArray->pContainer)) = nullptr;
				break;
			case KVS_PT_ARRAY:
				*((KviKvsArray **)(pFmtArray->pContainer)) = nullptr;
				break;
			case KVS_PT_ARRAYCAST:
				((KviKvsArrayCast *)(pFmtArray->pContainer))->clear();
				break;
			case KVS_PT_VARIANT:
				*((KviKvsVariant **)(pFmtArray->pContainer)) = nullptr;
				break;
			case KVS_PT_CSTRING:
			case KVS_PT_NONEMPTYCSTRING:
				*((QByteArray *)(pFmtArray->pContainer)) = nullptr;
				break;
			case KVS_PT_STRINGLIST:
				((QStringList *)(pFmtArray->pContainer))->clear();
				break;
			case KVS_PT_VARIANTLIST:
				((KviKvsVariantList *)(pFmtArray->pContainer))->clear();
				((KviKvsVariantList *)(pFmtArray->pContainer))->setAutoDelete(false);
				break;
			case KVS_PT_HOBJECT:
				*((kvs_hobject_t *)(pFmtArray->pContainer)) = (kvs_hobject_t) nullptr;
				break;
			case KVS_PT_IGNORE:
				// ignore :)
				break;
			default:
				qDebug("Internal error in KviKvsParameterProcessor::setDefaultValue(): unknown parameter type %d", pFmtArray->uType);
				break;
		}
	}

	bool handleParameterTypeError(KviKvsRunTimeContext * pContext, KviKvsParameterProcessor::ParameterFormat * pFmtArray, KviKvsVariant * v, const char * szExpectedType)
	{
		if(pFmtArray->uFlags & KVS_PF_OPTIONAL)
		{
			if(v->isEmpty())
			{
				setDefaultValue(pFmtArray);
				return true; // empty optional
			}
		}

		QString szError = QString(__tr2qs_ctx("Invalid data type for parameter \"%1\"", "kvs")).arg(pFmtArray->szName);
		szError += ": ";

		if(v->isString())
		{
			QString tmp = v->string();
			if(tmp.isEmpty())
			{
				KviQString::appendFormatted(szError, __tr2qs_ctx("found empty string where type '%s' was expected", "kvs"), &tmp, szExpectedType);
			}
			else
			{
				if(tmp.length() > 15)
				{
					tmp.truncate(15);
					tmp.append("...");
				}
				KviQString::appendFormatted(szError, __tr2qs_ctx("found string value \"%Q\" where type '%s' was expected", "kvs"), &tmp, szExpectedType);
			}
		}
		else
		{
			QString tmp;
			v->getTypeName(tmp);
			KviQString::appendFormatted(szError, __tr2qs_ctx("found type %Q where type '%s' was expected", "kvs"), &tmp, szExpectedType);
		}
		pContext->error(szError);
		return false;
	}

	bool process(KviKvsVariantList * pVariantList, KviKvsRunTimeContext * pContext, KviKvsParameterProcessor::ParameterFormat * pFmtArray)
	{
		KviKvsVariant * v = pVariantList->first();

		while(pFmtArray->szName)
		{
			if(!v)
			{
				// parameter not present
				// it MUST be optional
				if(!(pFmtArray->uFlags & KVS_PF_OPTIONAL))
				{
					// bad luck
					QString szError = QString(__tr2qs_ctx("Missing non-optional parameter \"%1\"", "kvs")).arg(pFmtArray->szName);
					pContext->error(szError);
					return false;
				}
				// ok, missing but optional (all the following are implicitly optional too)
				// set to default values
				do
				{
					setDefaultValue(pFmtArray);
					pFmtArray++;
				} while(pFmtArray->szName);
				return true;
			}
			// here we do only "light" casts: hard ones must be done explicitly by the user
			switch(pFmtArray->uType)
			{
				case KVS_PT_STRING:
					v->asString(*((QString *)(pFmtArray->pContainer)));
					if(pFmtArray->uFlags & KVS_PF_APPENDREMAINING)
					{
						v = pVariantList->next();
						while(v)
						{
							*((QString *)(pFmtArray->pContainer)) += QChar(' ');
							v->appendAsString(*((QString *)(pFmtArray->pContainer)));
							v = pVariantList->next();
						}
						return true;
					}
					break;
				case KVS_PT_STRINGLIST:
				{
					((QStringList *)(pFmtArray->pContainer))->clear();
					QString pSz;
					v->asString(pSz);
					((QStringList *)(pFmtArray->pContainer))->append(pSz);
					v = pVariantList->next();
					while(v)
					{
						v->asString(pSz);
						((QStringList *)(pFmtArray->pContainer))->append(pSz);
						v = pVariantList->next();
					}
					return true;
				}
				break;
				case KVS_PT_VARIANTLIST:
				{
					((KviKvsVariantList *)(pFmtArray->pContainer))->clear();
					((KviKvsVariantList *)(pFmtArray->pContainer))->setAutoDelete(false);
					((KviKvsVariantList *)(pFmtArray->pContainer))->append(v);
					v = pVariantList->next();
					while(v)
					{
						((KviKvsVariantList *)(pFmtArray->pContainer))->append(v);
						v = pVariantList->next();
					}
					return true;
				}
				break;
				case KVS_PT_NONEMPTYSTRING:
				{
					v->asString(*((QString *)(pFmtArray->pContainer)));
					bool bDoReturn = false;
					if(pFmtArray->uFlags & KVS_PF_APPENDREMAINING)
					{
						v = pVariantList->next();
						while(v)
						{
							*((QString *)(pFmtArray->pContainer)) += QChar(' ');
							v->appendAsString(*((QString *)(pFmtArray->pContainer)));
							v = pVariantList->next();
						}
						bDoReturn = true;
					}
					if(((QString *)(pFmtArray->pContainer))->isEmpty())
					{
						QString szError = QString(__tr2qs_ctx("Invalid data type for parameter \"%1\": found empty string while a non empty one was expected", "kvs")).arg(pFmtArray->szName);
						pContext->error(szError);
						return false;
					}
					if(bDoReturn)
						return true;
				}
				break;
				case KVS_PT_CSTRING:
				{
					QString tmp;
					v->asString(tmp);
					if(pFmtArray->uFlags & KVS_PF_APPENDREMAINING)
					{
						v = pVariantList->next();
						while(v)
						{
							*((QByteArray *)(pFmtArray->pContainer)) += ' ';
							v->appendAsString(tmp);
							v = pVariantList->next();
						}
						*((QByteArray *)(pFmtArray->pContainer)) = tmp.toUtf8();
						return true;
					}
					*((QByteArray *)(pFmtArray->pContainer)) = tmp.toUtf8();
				}
				break;
				case KVS_PT_NONEMPTYCSTRING:
				{
					QString tmp;
					v->asString(tmp);
					bool bDoReturn = false;
					if(pFmtArray->uFlags & KVS_PF_APPENDREMAINING)
					{
						v = pVariantList->next();
						while(v)
						{
							*((QByteArray *)(pFmtArray->pContainer)) += ' ';
							v->appendAsString(tmp);
							v = pVariantList->next();
						}
						*((QByteArray *)(pFmtArray->pContainer)) = tmp.toUtf8();
						bDoReturn = true;
					}
					*((QByteArray *)(pFmtArray->pContainer)) = tmp.toUtf8();
					if(((QByteArray *)(pFmtArray->pContainer))->isEmpty())
					{
						QString szError = QString(__tr2qs_ctx("Invalid data type for parameter \"%1\": found empty string while a non empty one was expected", "kvs")).arg(pFmtArray->szName);
						pContext->error(szError);
						return false;
					}
					if(bDoReturn)
						return true;
				}
				break;
				case KVS_PT_INT:
					if(!v->asInteger(*((kvs_int_t *)(pFmtArray->pContainer))))
					{
						if(!handleParameterTypeError(pContext, pFmtArray, v, "integer"))
							return false;
					}
					break;
				case KVS_PT_UINT:
				{
					kvs_int_t iTmp;
					if(!v->asInteger(iTmp))
					{
						if(!handleParameterTypeError(pContext, pFmtArray, v, "unsigned integer"))
							return false;
					}
					if(iTmp < 0)
					{
						QString szError = QString(__tr2qs_ctx("Invalid data type for parameter \"%1\": found signed integer \"%2\" where type 'unsigned integer' was expected", "kvs")).arg(pFmtArray->szName).arg(iTmp);
						pContext->error(szError);
						return false;
					}
					*((kvs_uint_t *)(pFmtArray->pContainer)) = (kvs_uint_t)iTmp;
				}
				break;
				case KVS_PT_DOUBLE:
					if(!v->asReal(*((kvs_real_t *)(pFmtArray->pContainer))))
					{
						if(!handleParameterTypeError(pContext, pFmtArray, v, "real"))
							return false;
					}
					break;
				case KVS_PT_HASH:
					if(!v->isHash())
					{
						if(!handleParameterTypeError(pContext, pFmtArray, v, "hash"))
							return false;
					}
					else
					{
						*((KviKvsHash **)(pFmtArray->pContainer)) = v->hash();
					}
					break;
				case KVS_PT_ARRAYCAST:
					v->castToArray((KviKvsArrayCast *)(pFmtArray->pContainer));
					break;
				case KVS_PT_ARRAY:
					if(!v->isArray())
					{
						if(!handleParameterTypeError(pContext, pFmtArray, v, "array"))
							return false;
					}
					else
					{
						*((KviKvsArray **)(pFmtArray->pContainer)) = v->array();
					}
					break;
				case KVS_PT_BOOL:
					// this never fails: anything is converted to a boolean
					*((bool *)(pFmtArray->pContainer)) = v->asBoolean();
					break;
				case KVS_PT_VARIANT:
					*((KviKvsVariant **)(pFmtArray->pContainer)) = v;
					break;
				case KVS_PT_HOBJECT:
					if(!v->asHObject(*((kvs_hobject_t *)(pFmtArray->pContainer))))
					{
						if(!handleParameterTypeError(pContext, pFmtArray, v, "hobject"))
							return false;
					}
					break;
				case KVS_PT_IGNORE:
					// ignore
					break;
				default:
					qDebug("Internal error in KviKvsParameterProcessor::processAsParameters(): unknown parameter type %d", pFmtArray->uType);
					return false;
					break;
			}
			pFmtArray++;
			v = pVariantList->next();
		}
		return true;
	}
};
