//vim: ts=8
//   File : class_file.cpp
//   Creation date : Thu Sep 21 04:42:16 CEST 2000 by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

//#warning "Add QFILEINFO to this stuff ?"

#include "class_file.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_fileutils.h"
#include  <qstringlist.h>


// needed for $open()
const char * const mod_tbl[] =	{
					"Raw",
					"ReadOnly",
					"WriteOnly",
					"ReadWrite",
					"Append",
					"Truncate"
				  };

const int mod_cod[] =	{
				IO_Raw,
				IO_ReadOnly,
				IO_WriteOnly,
				IO_ReadWrite,
				IO_Append,
				IO_Truncate
			};

#define mod_num			(sizeof(mod_tbl) / sizeof(mod_tbl[0]))

/*

static KviScriptObjectClass * g_pFileClass = 0;

static KviScriptObject * fileClassCreateInstance(KviScriptObjectClass * c,
	KviScriptObject * p, const char * n)
{
	return new KviScriptFileObject(c, p, n);
}

KviScriptFileObject::KviScriptFileObject(KviScriptObjectClass * c, KviScriptObject * p,
	const char * n) : KviScriptObject(c, p, n)
{
	m_pFile = new QFile();
}

KviScriptFileObject::~KviScriptFileObject()
{
	delete m_pFile;
}



#define fileFuncReg(__nam, __func) \
	g_pFileClass->registerFunctionHandler(__nam, \
	(KviScriptObjectFunctionHandlerProc)(KVI_PTR2MEMBER(KviScriptFileObject::__func)), \
	0, true);

void KviScriptFileObject::registerSelf()
{
	KviScriptObjectClass * base = g_pScriptObjectController-> \
		lookupClass("object");
	__range_valid(base);

	g_pFileClass = new KviScriptObjectClass(base, "file", \
		fileClassCreateInstance, true);

	fileFuncReg("setName", functionSetName);
	fileFuncReg("name", functionName);
	fileFuncReg("open", functionOpen);
	fileFuncReg("isOpen", functionIsOpen);
	fileFuncReg("close", functionClose);
	fileFuncReg("flush", functionFlush);
	fileFuncReg("size", functionSize);
	fileFuncReg("atEnd", functionAtEnd);
	fileFuncReg("where", functionWhere);
	fileFuncReg("seek", functionSeek);
	fileFuncReg("putch", functionPutch);
	fileFuncReg("getch", functionGetch);
	fileFuncReg("ungetch", functionUngetch);
	fileFuncReg("readBlock", functionReadBlock);
	fileFuncReg("writeBlock", functionWriteBlock);
	fileFuncReg("readLine", functionReadLine);
	fileFuncReg("writeLine", functionWriteLine);
	fileFuncReg("hexWrite", functionHexWrite);
	fileFuncReg("hexRead", functionHexRead);
}

void KviScriptFileObject::unregisterSelf()
{
	delete g_pFileClass;
    g_pFileClass = 0;
}

bool KviScriptFileObject::functionSetName(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	ENTER_STACK_FRAME(c, "file::setName");
	KviStr * pS = p->safeFirst();
	if(pS->isEmpty())
		return c->error(KviError_invalidParameter,__tr("Empty string"));
	m_pFile->setName(QString::fromUtf8(pS->ptr()));
	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionName(KviCommand *, KviParameterList *, KviStr & b)
{
	b.append(m_pFile->name().utf8().data());
	return true;
}

bool KviScriptFileObject::functionOpen(KviCommand * c, KviParameterList * p, KviStr &)
{
	ENTER_STACK_FRAME(c, "file::open");
	if(kvi_strEqualCI(m_pFile->name().utf8().data(), ""))
		return c->error(KviError_noSuchFile, \
			__tr("Empty filename string"));

	int mod, sum = 0;
	if(!p->count())	// if no parameters given, default to ReadWrite | Append
		sum = IO_ReadOnly;
	else
	{
		for(unsigned int i = 0; i < p->count(); i++)
		{
			mod = 0;
			for(unsigned int j = 0; j < mod_num; j++)
			{
				if(kvi_strEqualCI(p->at(i)->ptr(), mod_tbl[j]))
				{
					mod = mod_cod[j];
					break;
				}
			}

			if(mod)
				sum = sum | mod;
			else
				c->warning(__tr("No such open mode: %s"), \
						p->at(i)->ptr());
		}
	}

	if(!m_pFile->open(sum))
		return c->error(KviError_unknownError);	// signalise with other
							// error ?
	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionIsOpen(KviCommand *, KviParameterList *, KviStr & b)
{
	b.append(m_pFile->isOpen() ? '1' : '0');
	return true;
}

bool KviScriptFileObject::functionClose(KviCommand * c, KviParameterList *, KviStr &)
{
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	m_pFile->close();
	return true;
}

bool KviScriptFileObject::functionFlush(KviCommand * c, KviParameterList *, KviStr &)
{
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	m_pFile->flush();
	return true;
}

bool KviScriptFileObject::functionSize(KviCommand *, KviParameterList *, KviStr & b)
{
	b.append(KviStr::Format, "%d", m_pFile->size());
	return true;
}

bool KviScriptFileObject::functionAtEnd(KviCommand * c, KviParameterList *,KviStr & b)
{
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	b.append(m_pFile->atEnd() ? '1' : '0');
	return true;
}

bool KviScriptFileObject::functionWhere(KviCommand * c, KviParameterList *,KviStr & b)
{
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	b.append(KviStr::Format, "%d", m_pFile->at());
	return true;
}

bool KviScriptFileObject::functionSeek(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	ENTER_STACK_FRAME(c, "file::seek");
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	if(!p->count())
		return c->error(KviError_notEnoughParameters);

	bool bOk;
	int val = p->getInt(&bOk);
	if(!bOk)
		return c->error(KviError_integerParameterExpected);

	if(val < 0)
		c->warning(__tr("Negative file index supplied !"));

	m_pFile->at(val);
	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionPutch(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	ENTER_STACK_FRAME(c, "file::putch");
	if(!p->count())
		return c->error(KviError_notEnoughParameters);

	if(p->first()->len() > 1)
		c->warning(__tr("Argument to long, using only first char"));
	else
		if(p->first()->len() == 0)
			c->warning(__tr("Argument length is 0 - empty string"));

	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	char ch = p->first()->at(0);
	if(m_pFile->putch(ch) < 0)
		c->warning(__tr("Write error occured !"));	// c->error ?
	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionGetch(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	int chInt = m_pFile->getch();
	if(chInt < 0)
		c->warning(__tr("Read error occured !"));	// c->error ?
	else
	{
		char ch = chInt;
		b.append(ch);
	}

	return true;
}

bool KviScriptFileObject::functionUngetch(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	ENTER_STACK_FRAME(c, "file::ungetch");
	if(!p->count())
		return c->error(KviError_notEnoughParameters);

	if(p->first()->len() > 1)
		c->warning(__tr("Argument to long, using only first char"));
	else
		if(p->first()->len() == 0)
			c->warning(__tr("Argument length is 0 - empty string"));

	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	char ch = p->first()->at(0);
	if(m_pFile->ungetch(ch) < 0)
		c->warning(__tr("An error occured !"));		// c->error ?
	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionReadBlock(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	ENTER_STACK_FRAME(c, "file::readBlock");
	if(!p->count())
		return c->error(KviError_notEnoughParameters);

	bool ok;
	int len = p->getInt(&ok);
	if(!ok)
		return c->error(KviError_integerParameterExpected);
	if(len < 0)
		c->warning(__tr("Length is a negative number !"));

	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	char * buff = new char[len + 1];
	m_pFile->flush(); // advice from QFile man page (to avoid trash)
	int rlen = m_pFile->readBlock(buff, len);
	buff[rlen] = '\0';
	b.append(buff);
	delete[] buff;
	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionWriteBlock(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	ENTER_STACK_FRAME(c, "file::writeBlock");

	if(p->count() < 2)
		return c->error(KviError_notEnoughParameters);

	bool ok;
	int len = p->getInt(&ok);
//	KviStr * pS = p->safeNext();
	KviStr * pS = p->at(1);
	if(!ok)
		return c->error(KviError_integerParameterExpected);
	if(len < 0)
		c->warning(__tr("Length is a negative number !"));

	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	int rlen = m_pFile->writeBlock(pS->ptr(), len);
	b.append(KviStr::Format, "%d", rlen);
	m_pFile->flush();
	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionReadLine(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
//	ENTER_STACK_FRAME(c, "file::readLine");

	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	KviStr tmp;
	kvi_readLine(m_pFile,tmp);
	b.append(tmp);

	return true;

//	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionWriteLine(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	ENTER_STACK_FRAME(c, "file::writeLine");

	if(!p->count())
		return c->error(KviError_notEnoughParameters);

	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	QTextStream ts(m_pFile);
	ts << p->first()->ptr();

	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionHexWrite(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	ENTER_STACK_FRAME(c, "file::hexWrite");
	if(!p->count())
		return c->error(KviError_notEnoughParameters);

	char * val = 0;
	int len = p->first()->hexToBuffer(&val,false);

	if(len < 1)
	{
		c->warning(__tr("Nothing to write"));
		return c->leaveStackFrame();
	}

	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	if(m_pFile->putch(*val) < 0)
		c->warning(__tr("Write error occured !"));	// c->error ?

	KviStr::freeBuffer(val);

	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionHexRead(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
 	if(!m_pFile->isOpen())
 		c->warning(__tr("File is not open !"));

	char ch = m_pFile->getch();

	KviStr tmp;
 	tmp.bufferToHex(&ch, 1);
 	b.append(tmp);

 	return true;
}

*/



/*
	@doc:	file
	@keyterms:
		file object class, creating files
	@title:
		file class
	@type:
		class
	@short:
		I/O device that operates on files
	@inherits:
		[class]object[/class]
	@description:
		This object class provides access to files - basic I/O
		operations, copying, removing etc...
	@functions:
		!fn: $setName(<file_name:string>)
		Sets the file name to <file_name>. It does [b]not move[/b] the file,
		it just changes the file the object is "pointing to". You can
		not change names of already open files.[br]
		See also: [classfnc]$open[/classfnc](),
		[classfnc]$name[/classfnc]().
		!fn: <string> $name()
		Returns name set by [classfnc]$setName[/classfnc]().[br]
		See also: [classfnc]$setName[/classfnc]().
		!fn: $open(<mode1:string>, <mode2:string>)
		Attempts to open the file in specified mode or modes "sum".
		Valid modes are:[br]
		[pre]
		Raw          - raw, non-buffered access[br]
		ReadOnly     - opens the file read-only[br]
		WriteOnly    - opens the file write-only[br]
		ReadWrite    - opens the file in read-write mode[br]
		Append       - opens the file in append mode. The file index is set to the end of the file.[br]
		Truncate     - truncates the file[br]
		[/pre]
		If you call this function without any parameters, the file is
		opened in ReadOnly mode.[br]
		When working with buffered files, data is not written directly
		to the file at once. You must call [classfnc]$flush[/classfnc]() to force it.[br]
		See also: [classfnc]$close[/classfnc](),
		[classfnc]$flush[/classfnc]().
		!fn: <boolean> $isOpen()
		Returns '1' if the file is open, '0' otherwise.
		!fn: $close()
		Closes the file, flushing the buffers first.[br]
		See also: [classfnc]$flush[/classfnc]().
		!fn: $flush()
		Flushes the file buffer to disk. Calling this after opening the
		file in 'Raw' mode doesn't make much sense.[br]
		See also: [classfnc]$open[/classfnc](),
		[classfnc]$close[/classfnc]().
		!fn: <integer> $size()
		Returns current file size.
		!fn: <boolean> $atEnd()
		Returns '1' if you have reached end of the file, '0' otherwise.
		[br]See also: [classfnc]$seek[/classfnc](),
		[classfnc]$where[/classfnc]().
		!fn: <integer> $where()
		Returns current position in the file (file index).[br]
		See also: [classfnc]$seek[/classfnc]().
		!fn: $seek(<index:integer>)
		Sets the file index to <index>.[br]
		See also: [classfnc]$where[/classfnc](),
		[classfnc]$atEnd[/classfnc]().
		!fn: $putch(<char>)
		Writes character <char> to the file and increments file
		index.[br]
		See also: [classfnc]$getch[/classfnc](),
		[classfnc]$ungetch[/classfnc]().
		!fn: <char> $getch()
		Reads a character from the file and increments file index.[br]
		See also: [classfnc]$putch[/classfnc](),
		[classfnc]$ungetch[/classfnc]().
		!fn: $ungetch(<char>)
		Puts the character <char> back to the file and
		decrements the file index. This is usually called to undo a
		$getch() call.[br]
		See also: [classfnc]$getch[/classfnc],
		[classfnc]$putch[/classfnc]().
		!fn: $readLine(<text_line:string>)
		Reads a line of text from the file and increments file index.
		!fn: $writeLine(<text_line:string>)
		Appends a line of text to the end of the file.
		$readBlock()
		$writeBlock()
		$writeHex()
		$readHex()
*/


//---------------------------------------------------------------------------------

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_file,"file","object")
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"setName", functionsetName)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"name", functionname)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"open", functionopen)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"isOpen", functionisOpen)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"close", functionclose)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"flush", functionflush)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"size", functionsize)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"atEnd", functionatEnd)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"where", functionwhere)

	KVSO_REGISTER_HANDLER(KviKvsObject_file,"seek", functionseek)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"putch", functionputch)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"getch", functiongetch)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"ungetch", functionunGetch)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"readBlock", functionreadBlock)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"writeBlock", functionwriteBlock)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"readLine", functionreadLine)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"writeLine", functionwriteLine)
/*	KVSO_REGISTER_HANDLER(KviKvsObject_file,"hexWrite", functionHexWrite)
	KVSO_REGISTER_HANDLER(KviKvsObject_file,"hexRead", functionHexRead)
*/
  KVSO_END_REGISTERCLASS(KviKvsObject_file)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_file,KviKvsObject)
	m_pFile = new QFile();
KVSO_END_CONSTRUCTOR(KviKvsObject_file)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_file)
if (m_pFile) delete m_pFile;
m_pFile=0;

KVSO_END_CONSTRUCTOR(KviKvsObject_file)


bool KviKvsObject_file::functionsetName(KviKvsObjectFunctionCall *c)
{
	QString szName;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file_name",KVS_PT_STRING,0,szName)
	KVSO_PARAMETERS_END(c)
	if (m_pFile) m_pFile->setName(szName);
	return true;
}
bool KviKvsObject_file::functionname(KviKvsObjectFunctionCall *c)
{
	if (m_pFile) c->returnValue()->setString(m_pFile->name());
	return true;
}

bool KviKvsObject_file::functionopen(KviKvsObjectFunctionCall *c)
{
	QStringList modes;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file_mode",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,modes)
	KVSO_PARAMETERS_END(c)
	if (!m_pFile) return true;
	if(m_pFile->name().isEmpty())
	{
		c->warning(__tr2qs("Empty filename string"));
		return true;
	}
	int mod,sum = 0;
	if (!modes.first()) sum = IO_ReadOnly; // if no parameters given, default to ReadWrite | Append
	else
	{
		for ( QStringList::Iterator it = modes.begin(); it != modes.end(); ++it )
		{

			mod = 0;
			for(unsigned int j = 0; j < mod_num; j++)
			{
				if(KviQString::equalCI((*it), mod_tbl[j]))
				{
					mod=mod_cod[j];
					break;
				}
			}
			if(mod)
				sum = sum | mod;
			else
				c->warning(__tr2qs("No such open mode: '%Q'"),&(*it));
		}
	}
	m_pFile->open(sum);
	return true;
}

bool KviKvsObject_file::functionisOpen(KviKvsObjectFunctionCall *c)
{
	if (m_pFile) c->returnValue()->setBoolean(m_pFile->isOpen());
	return true;
}

bool KviKvsObject_file::functionclose(KviKvsObjectFunctionCall *c)
{
	if (!m_pFile->isOpen()) c->warning(__tr2qs("File is not open!"));
	else m_pFile->close();
	return true;
}
bool KviKvsObject_file::functionflush(KviKvsObjectFunctionCall *c)
{
	if (!m_pFile->isOpen()) c->warning(__tr2qs("File is not open!"));
	else m_pFile->flush();
	return true;
}

bool KviKvsObject_file::functionsize(KviKvsObjectFunctionCall *c)
{
	if (m_pFile) c->returnValue()->setInteger((kvs_int_t)(m_pFile->size()));
	return true;
}
bool KviKvsObject_file::functionatEnd(KviKvsObjectFunctionCall *c)
{
	if (m_pFile) c->returnValue()->setInteger((kvs_int_t)(m_pFile->size()));
	return true;
}


bool KviKvsObject_file::functionwhere(KviKvsObjectFunctionCall *c)
{
	if (!m_pFile->isOpen()) c->warning(__tr2qs("File is not open!"));
	else c->returnValue()->setInteger((kvs_int_t)(m_pFile->at()));
	return true;
}
bool KviKvsObject_file::functionseek(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if (!m_pFile) return true;
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));
	else m_pFile->at(uIndex);
	return true;
}

bool KviKvsObject_file::functionputch(KviKvsObjectFunctionCall *c)
{
	QString szChar;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("char",KVS_PT_STRING,KVS_PF_OPTIONAL,szChar)
	KVSO_PARAMETERS_END(c)
	if (m_pFile)
	{
		if (szChar.length()>1)c->warning(__tr2qs("Argument to long, using only first char"));
		const char *ch=szChar;
		if (m_pFile->putch(ch[0])<0) c->warning(__tr2qs("Write error occured !"));
	}

	return true;
}
bool KviKvsObject_file::functiongetch(KviKvsObjectFunctionCall *c)
{
	if (!m_pFile) return true;
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));
	else
	{
		int chInt=m_pFile->getch();
		if (chInt<0) 	c->warning(__tr("Read error occured !"));	// c->error ?
		QString szChar = QChar(chInt);
		c->returnValue()->setString(szChar);
	}
	return true;
}
bool KviKvsObject_file::functionunGetch(KviKvsObjectFunctionCall *c)
{
	QString szChar;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("char",KVS_PT_STRING,0,szChar)
	KVSO_PARAMETERS_END(c)
	if (!m_pFile) return true;
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));
	else
	{
		if (szChar.length()>1) c->warning(__tr2qs("Argument to long, using only first char"));
		const char *ch=szChar;
		if (m_pFile->ungetch(ch[0])<0) c->warning(__tr2qs("An error occured !"));// c->error ?
	}
	return true;
}
bool KviKvsObject_file::functionreadBlock(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uLen;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("lenght",KVS_PT_UNSIGNEDINTEGER,0,uLen)
	KVSO_PARAMETERS_END(c)
	if (!m_pFile) return true;
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));
	else
	{
		char * buff = new char[uLen + 1];
		m_pFile->flush(); // advice from QFile man page (to avoid trash)
		int rlen = m_pFile->readBlock(buff, uLen);
		buff[rlen] = '\0';
		QString szBlock(buff);
		c->returnValue()->setString(szBlock);
	}
	return true;
}

bool KviKvsObject_file::functionwriteBlock(KviKvsObjectFunctionCall *c)
{
	QString szBlock;
	kvs_uint_t uLen;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("lenght",KVS_PT_UNSIGNEDINTEGER,0,uLen)
		KVSO_PARAMETER("text_block",KVS_PT_STRING,0,szBlock)
	KVSO_PARAMETERS_END(c)
	if (!m_pFile) return true;
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));
	const char *block=szBlock;
	int rlen = m_pFile->writeBlock(block, uLen);
	c->returnValue()->setInteger(rlen);
	m_pFile->flush();
	return true;
}
bool KviKvsObject_file::functionreadLine(KviKvsObjectFunctionCall *c)
{
	if (!m_pFile) return true;
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));
	else
	{
		QString buffer;
		KviFileUtils::readLine(m_pFile,buffer);
		c->returnValue()->setString(buffer);
	}
	return true;
}
bool KviKvsObject_file::functionwriteLine(KviKvsObjectFunctionCall *c)
{
	QString szLine;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text_line",KVS_PT_STRING,0,szLine)
	KVSO_PARAMETERS_END(c)
	if (!m_pFile) return true;
	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));
	else
	{
		QTextStream ts(m_pFile);
		ts << szLine;
	}
	return true;
}
// fixme
/*

bool KviScriptFileObject::functionHexWrite(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
	ENTER_STACK_FRAME(c, "file::hexWrite");
	if(!p->count())
		return c->error(KviError_notEnoughParameters);

	char * val = 0;
	int len = p->first()->hexToBuffer(&val,false);

	if(len < 1)
	{
		c->warning(__tr("Nothing to write"));
		return c->leaveStackFrame();
	}

	if(!m_pFile->isOpen())
		c->warning(__tr("File is not open !"));

	if(m_pFile->putch(*val) < 0)
		c->warning(__tr("Write error occured !"));	// c->error ?

	KviStr::freeBuffer(val);

	return c->leaveStackFrame();
}

bool KviScriptFileObject::functionHexRead(KviCommand * c, KviParameterList * p,
	KviStr & b)
{
 	if(!m_pFile->isOpen())
 		c->warning(__tr("File is not open !"));

	char ch = m_pFile->getch();

	KviStr tmp;
 	tmp.bufferToHex(&ch, 1);
 	b.append(tmp);

 	return true;
}

*/
#include "m_class_file.moc"
