//=============================================================================
//
//   File : KvsObject_xmlreader.cpp
//   Created on Tue 27 Dec 2005 00:14:09 by Szymon Stefanek
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

#include "KvsObject_xmlreader.h"
#include "KvsObject_memoryBuffer.h"
#include "KviLocale.h"
#include "KviKvsVariantList.h"
#include "KviKvsHash.h"

/*
	@doc: xmlreader
	@keyterms:
		xml
	@title:
		xmlreader class
	@type:
		class
	@short:
		A simple XML document parser
	@inherits:
		[class]object[/class]
	@description:
		This class implements a really simple XML document parser.
		You will usually derive your own class from this one and reimplement
		some of the events that it triggers.
		You will typically reimplement [classfnc:xmlreader]$onElementStart[/classfnc]()
		and [classfnc:xmlreader]$onElementEnd[/classfnc]() that will be called
		during the execution of [classfnc:xmlreader]$parse[/classfnc]() in an order
		reflecting the order of elements in the parsed document.
	@functions:
		!fn: <boolean> $parse(<xml_data:string_or_memorybuffer_object>)
		Call this function to parse a string that contains an XML document.
		A typical call for this method will look like:
		[example]
			%x = [fnc]$new[/fnc](xmlreader)
			%x->$parse([fnc]$file.read[/fnc]("/home/somefile.xml"))
		[/example]
		During the call the <xml_data> string will be parsed and the
		relevant on* events (see below) will be triggered.
		$parse will return $true when the parsing terminates successfully
		or $false if it aborts for some reason (unrecoverable error
		in the document, user abort etc.).
		If this function return $false then you can call $lastError() to
		obtain a descriptive error message.
		!fn: <string> $lastError()
		Returns the last error occurred inside the parser.
		You will typically call this function when $parse() above returns $false.
		!fn: <boolean> $onDocumentStart()
		This function is called when the document parsing starts.
		You can reimplement it in order to handle this notification.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		!fn: <boolean> $onDocumentEnd()
		This function is called when the document parsing terminates successfully.
		You can reimplement it in order to handle this notification.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		!fn: <boolean> $onElementStart(<qualified_name:string>,<attributes:hash>,<namespace:string>,<local_name:string>)
		This function is called when an element opening tag is encountered.
		The <qualified_name> of the tag is passed as the first parameter.
		The <attributes> are passed in the form of a hash with attribute
		values indexed by their names.
		When the <qualified_name> contains a namespace then it is also reported
		in the split <namespace> <local_name> pair.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		!fn: <boolean> $onElementEnd(<qualified_name:string>,<namespace:string>,<local_name:string>)
		This function is called when an element closing tag is encountered.
		The <qualified_name> of the tag is passed as the first parameter.
		When the <qualified_name> contains a namespace then it is also reported
		in the split <namespace> <local_name> pair.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		!fn: <boolean> $onText($0 = <text:string>)
		This function is called when a chunk of text is encountered inside the document.
		The parsed <text> chunk is passed as the first parameter.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		!fn: <boolean> $onWarning(<message:string>)
		This function is called when the parser generates a recoverable error.
		The error <message> is passed as the first parameter.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		!fn: <boolean> $onError(<message:string>)
		This function is called when the parser generates an unrecoverable error.
		The error <message> is passed as the first parameter.
		The document parsing can't continue.
		The default implementation does nothing besides returning $true.
*/

#ifndef QT_NO_XML

#include <QXmlStreamReader>

class KviXmlHandler
{
protected:
	KvsObject_xmlReader * m_pReader;
	QString m_szErrorString;

public:
	explicit KviXmlHandler(KvsObject_xmlReader * pReader)
	    : m_pReader(pReader)
	{
	}

private:
	bool kvsCodeFailure()
	{
		m_szErrorString = __tr2qs_ctx("Error in KVS class implementation: processing aborted", "objects");
		return false;
	}
	bool kvsCodeAbort()
	{
		m_szErrorString = __tr2qs_ctx("Processing aborted", "objects");
		return false;
	}
	bool handleKvsCallReturnValue(KviKvsVariant * pRetVal)
	{
		if(!pRetVal->asBoolean())
			return kvsCodeAbort();
		return true;
	}

public:
	bool parse(QXmlStreamReader & reader)
	{
		while(!reader.atEnd())
		{
			bool shouldContinue = true;
			switch(reader.readNext())
			{
				case QXmlStreamReader::StartDocument:
					shouldContinue = startDocument();
					break;
				case QXmlStreamReader::EndDocument:
					shouldContinue = endDocument();
					break;
				case QXmlStreamReader::StartElement:
					shouldContinue = startElement(reader.namespaceUri().toString(), reader.name().toString(), reader.qualifiedName().toString(), reader.attributes());
					break;
				case QXmlStreamReader::EndElement:
					shouldContinue = endElement(reader.namespaceUri().toString(), reader.name().toString(), reader.qualifiedName().toString());
					break;
				case QXmlStreamReader::Characters:
				case QXmlStreamReader::EntityReference:
					shouldContinue = characters(reader.text().toString());
					break;
				default:
					break;
			}
			if(!shouldContinue)
			{
				reader.raiseError(m_szErrorString);
				break;
			}
		}
		if(!reader.hasError())
			return true;

		QString error = __tr2qs_ctx("Error near line %1, column %2", "objects");
		m_pReader->fatalError(error.arg(reader.lineNumber()).arg(reader.columnNumber()) + ": " + reader.errorString());
		return false;
	}

private:
	bool startDocument()
	{
		KviKvsVariant ret;
		if(!m_pReader->callFunction(m_pReader, "onDocumentStart", &ret))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}

	bool endDocument()
	{
		KviKvsVariant ret;
		if(!m_pReader->callFunction(m_pReader, "onDocumentEnd", &ret))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}

	bool startElement(const QString & szNamespaceUri, const QString & szLocalName, const QString & szQualifiedName, const QXmlStreamAttributes & attrs)
	{
		KviKvsVariant ret;
		KviKvsVariantList par;
		par.setAutoDelete(true);
		par.append(new KviKvsVariant(szQualifiedName));
		KviKvsHash * pHash = new KviKvsHash();
		par.append(new KviKvsVariant(pHash));
		par.append(new KviKvsVariant(szNamespaceUri));
		par.append(new KviKvsVariant(szLocalName));
	for(const auto & attribute : attrs)
			pHash->set(attribute.qualifiedName().toString(), new KviKvsVariant(attribute.value().toString()));
		if(!m_pReader->callFunction(m_pReader, "onElementStart", &ret, &par))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}

	bool endElement(const QString & szNamespaceUri, const QString & szLocalName, const QString & szQualifiedName)
	{
		KviKvsVariant ret;
		KviKvsVariantList par;
		par.setAutoDelete(true);
		par.append(new KviKvsVariant(szQualifiedName));
		par.append(new KviKvsVariant(szNamespaceUri));
		par.append(new KviKvsVariant(szLocalName));
		if(!m_pReader->callFunction(m_pReader, "onElementEnd", &ret, &par))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}

	bool characters(const QString & szChars)
	{
		KviKvsVariant ret;
		KviKvsVariantList par;
		par.setAutoDelete(true);
		par.append(new KviKvsVariant(szChars));
		if(!m_pReader->callFunction(m_pReader, "onText", &ret, &par))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}
};

#endif // !QT_NO_XML

KVSO_BEGIN_REGISTERCLASS(KvsObject_xmlReader, "xmlreader", "object")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_xmlReader, lastError)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_xmlReader, parse)

KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KvsObject_xmlReader, "onDocumentStart")
KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KvsObject_xmlReader, "onDocumentEnd")
KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KvsObject_xmlReader, "onElementStart")
KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KvsObject_xmlReader, "onElementEnd")
KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KvsObject_xmlReader, "onText")
KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KvsObject_xmlReader, "onWarning")
KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KvsObject_xmlReader, "onError")
KVSO_END_REGISTERCLASS(KvsObject_xmlReader)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_xmlReader, KviKvsObject)
KVSO_END_CONSTRUCTOR(KvsObject_xmlReader)

KVSO_BEGIN_DESTRUCTOR(KvsObject_xmlReader)
KVSO_END_DESTRUCTOR(KvsObject_xmlReader)

void KvsObject_xmlReader::fatalError(const QString & szError)
{
	m_szLastError = szError;

	KviKvsVariantList vArgs;
	vArgs.setAutoDelete(true);
	vArgs.append(new KviKvsVariant(m_szLastError));
	callFunction(this, "onError", &vArgs);
}

KVSO_CLASS_FUNCTION(xmlReader, parse)
{
	KviKvsVariant * pVariantData;

	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("string_or_memorybuffer_object", KVS_PT_VARIANT, 0, pVariantData)
	KVSO_PARAMETERS_END(c)
#ifdef QT_NO_XML
	fatalError(__tr2qs_ctx("XML support not available in the Qt library"));
	c->returnValue()->setBoolean(false);
#else
	m_szLastError = "";
	KviXmlHandler handler(this);
	QXmlStreamReader reader;

	if(pVariantData->isHObject())
	{
		KviKvsObject * pObject;
		kvs_hobject_t hObject;
		pVariantData->asHObject(hObject);
		pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
		if(!pObject)
		{
			c->warning(__tr2qs_ctx("Data parameter is not an object", "objects"));
			return true;
		}
		if(pObject->inheritsClass("memorybuffer"))
		{
			reader.addData(*((KvsObject_memoryBuffer *)pObject)->pBuffer());
		}
		else
		{
			c->warning(__tr2qs_ctx("Data parameter is not a memorybuffer object", "objects"));
			return true;
		}
	}
	else if(pVariantData->isString())
	{
		QString szString;
		pVariantData->asString(szString);
		reader.addData(szString);
	}
	else
	{
		c->warning(__tr2qs_ctx("Data is not a memorybuffer object or string", "objects"));
		return true;
	}
	c->returnValue()->setBoolean(handler.parse(reader));
#endif
	return true;
}

KVSO_CLASS_FUNCTION(xmlReader, lastError)
{
	c->returnValue()->setString(m_szLastError);
	return true;
}
