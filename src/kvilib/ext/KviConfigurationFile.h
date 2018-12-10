#ifndef _KVI_CONFIG_H_INCLUDED_
#define _KVI_CONFIG_H_INCLUDED_

//=============================================================================
//
//   File : KviConfigurationFile.h
//   Creation date : Thu Jan 14 1999 18:01:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviHeapObject.h"
#include "KviPointerHashTable.h"

#include <QList>
#include <QFont>
#include <QString>
#include <QStringList>

#define KVI_CONFIG_DEFAULT_GROUP "KVIrc"

class KviPixmap;
class KviMessageTypeSettings;
class QColor;
class QRect;

typedef KviPointerHashTable<QString, QString> KviConfigurationFileGroup;
typedef KviPointerHashTableIterator<QString, QString> KviConfigurationFileGroupIterator;
typedef KviPointerHashTableIterator<QString, KviConfigurationFileGroup> KviConfigurationFileIterator;

class KVILIB_API KviConfigurationFile : public KviHeapObject
{
public:
	enum FileMode
	{
		Read = 1,
		Write = 2,
		ReadWrite = 3
	};

public:
	KviConfigurationFile(const QString & filename, FileMode f /* = ReadWrite*/, bool bLocal8Bit = false);
	KviConfigurationFile(const char * filename, FileMode f /* = ReadWrite*/, bool bLocal8Bit = false);
	~KviConfigurationFile();

private:
	bool m_bLocal8Bit;
	KviPointerHashTable<QString, KviConfigurationFileGroup> * m_pDict;
	QString m_szFileName;
	bool m_bDirty;
	QString m_szStrBuffer;
	QString m_szGroup;
	bool m_bPreserveEmptyGroups;
	bool m_bReadOnly;

private:
	bool load();
	bool save();
	KviConfigurationFileGroup * getCurrentGroup();

public:
	//
	// Useful when saving...
	// Normally this class does not save empty groups
	// and setGroup() is never a config modification.
	// If the group is not existing it will be effectively
	// created only at the first attempt to read from it or write to it.
	// With this flag set to true the KviConfigurationFile class will
	// write down also the empty groups, and calls to setGroup()
	// will create the groups if not existing yet (and set the config data
	// as modified).
	//
	void preserveEmptyGroups(bool bPreserve) { m_bPreserveEmptyGroups = bPreserve; };
	const QString & fileName() { return m_szFileName; };
	bool readOnly() { return m_bReadOnly; };
	void setReadOnly(bool bReadOnly) { m_bReadOnly = bReadOnly; };
	bool dirty() { return m_bDirty; };
	bool saveIfDirty();

	//
	// This sets the save path for the config file
	// In this way you can load a system-wide read-only config file
	// as default configuration, alter its settings and save it to the
	// user local configuration directory
	void setSavePath(const QString & savePath) { m_szFileName = savePath; };
	KviPointerHashTable<QString, KviConfigurationFileGroup> * dict() { return m_pDict; };

	void clearDirtyFlag() { m_bDirty = false; };
	void clear();
	void clearGroup(const QString & szGroup);
	void clearKey(const QString & szKey);
	unsigned int groupsCount() { return m_pDict->count(); };
	bool sync() { return save(); };
	bool hasKey(const QString & szKey);
	bool hasGroup(const QString & szGroup);
	void setGroup(const QString & szGroup);
	//void getContentsString(KviCString &buffer);
	const QString & group() { return m_szGroup; };
	void writeEntry(const QString & szKey, const QString & szValue);
	void writeEntry(const QString & szKey, const char * szValue)
	{
		writeEntry(szKey, QString::fromUtf8(szValue));
	};
	QString readEntry(const QString & szKey, const QString & szDefault = QString());
	// HACK for KviOptions.. (FIXME)
	QString readKviCStringEntry(const QString & szKey, const KviCString & szDefault)
	{
		return readEntry(szKey, szDefault.ptr());
	};
	//void writeEntry(const char *szKey,KviCString &szValue);
	//const char * readEntry(const char *szKey,KviCString &szDefault);
	void writeEntry(const QString & szKey, const KviPixmap & pixmap);
	KviPixmap readPixmapEntry(const QString & szKey, const KviPixmap & pixDef);
	void writeEntry(const QString & szKey, const KviMessageTypeSettings & msg);
	KviMessageTypeSettings readMsgTypeEntry(const QString & szKey, const KviMessageTypeSettings & msgDef);
	void writeEntry(const QString & szKey, const QColor & clr);
	QColor readColorEntry(const QString & szKey, const QColor & clr);
	void writeEntry(const QString & szKey, QFont & fnt);
	QFont readFontEntry(const QString & szKey, const QFont & fnt);
	void writeEntry(const QString & szKey, bool bTrue);
	bool readBoolEntry(const QString & szKey, bool bTrue);
	QRect readRectEntry(const QString & szKey, const QRect & rct);
	void writeEntry(const QString & szKey, const QRect & rct);
	QStringList readStringListEntry(const QString & szKey, const QStringList & list = QStringList());
	void writeEntry(const QString & szKey, const QStringList & list);
	QList<int> readIntListEntry(const QString &, const QList<int> & list);
	void writeEntry(const QString & szKey, const QList<int> & list);
	//void writeEntry(const char *szKey,unsigned long lValue);
	//unsigned long readULongEntry(const char *szKey,unsigned long lDefault);
	//void writeEntry(const char *szKey,long lValue);
	//long readLongEntry(const char *szKey,long lDefault);
	void writeEntry(const QString & szKey, int iValue);
	int readIntEntry(const QString & szKey, int iDefault);
	void writeEntry(const QString & szKey, unsigned short int usValue);
	unsigned short int readUShortEntry(const QString & szKey, unsigned short int usDefault);
	void writeEntry(const QString & szKey, unsigned int iValue);
	unsigned int readUIntEntry(const QString & szKey, unsigned int iDefault);
	void writeEntry(const QString & szKey, char iValue);
	char readCharEntry(const QString & szKey, char iDefault);
	void writeEntry(const QString & szKey, unsigned char iValue);
	unsigned char readUCharEntry(const QString & szKey, unsigned char iDefault);

	static void getFontProperties(KviCString & buffer, QFont * fnt);
	static void setFontProperties(KviCString & str, QFont * fnt);

};

#endif //!_KVI_CONFIG_H_INCLUDED_
