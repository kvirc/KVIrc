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
#ifdef COMPILE_USE_QT4
#define	IO_RAW QIODevice::Unbuffered
#define IO_READONLY QIODevice::ReadOnly
#define IO_WRITEONLY QIODevice::WriteOnly
#define IO_READWRITE QIODevice::ReadWrite
#define IO_APPEND QIODevice::Append
#define IO_TRUNCATE QIODevice::Truncate
//#define IO_TRANSLATE QIODevice::Text
#else
#define	IO_RAW IO_Raw
#define IO_READONLY IO_ReadOnly
#define IO_WRITEONLY IO_WriteOnly
#define IO_READWRITE IO_ReadWrite
#define IO_APPEND IO_Append
#define IO_TRUNCATE IO_Truncate
#endif
#ifdef COMPILE_USE_QT4
const QIODevice::OpenMode mod_cod[] = {
#else
const int mod_cod[] =	{
#endif
				IO_RAW,
				IO_READONLY,
				IO_WRITEONLY,
				IO_READWRITE,
				IO_APPEND,
				IO_TRUNCATE
			};

#define mod_num			(sizeof(mod_tbl) / sizeof(mod_tbl[0]))

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
	#ifdef COMPILE_USE_QT4
	QIODevice::OpenMode mod,sum;
	if (modes.empty()) sum = IO_READONLY; // if no parameters given, default to ReadWrite | Append
	else
	{
		for ( int idx=0;idx<modes.count();idx++)
		{
			mod = IO_ReadOnly;
			for(unsigned int j = 0; j < mod_num; j++)
			{
				if(KviQString::equalCI(modes.at(idx), mod_tbl[j]))
				{
					mod=mod_cod[j];
					break;
				}
			}
			if(mod!=IO_ReadOnly)
				sum = sum | mod;
			else
				c->warning(__tr2qs("No such open mode: '%Q'"),&modes.at(idx));
		}
	}
#else
	int mod,sum=0;
if (!modes.first()) sum = IO_READONLY; // if no parameters given, default to ReadWrite | Append
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
#endif
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
