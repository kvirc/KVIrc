#ifndef _KVI_KVS_TREENODE_SPECIALCOMMANDDEFPOPUP_H_
#define _KVI_KVS_TREENODE_SPECIALCOMMANDDEFPOPUP_H_
//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommandDefpopup.h
//   Creation date : Fri 02 Jan 2004 13:54:00 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviKvsTreeNode.h"
#include "KviKvsTreeNodeSpecialCommand.h"

class KviKvsTreeNodeExpression;
class KviKvsTreeNodeInstruction;
class KviKvsRunTimeContext;
class KviKvsVariant;
class KviKvsPopupMenu;

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupLabel : public KviKvsTreeNode
{
public:
	QString m_szItemName;

public:
	KviKvsTreeNodeSpecialCommandDefpopupLabel(const QChar * pLocation, const QString & szItemName)
	    : KviKvsTreeNode(pLocation), m_szItemName(szItemName){};
	~KviKvsTreeNodeSpecialCommandDefpopupLabel(){};

public:
	void setItemName(const QString & szItemName) { m_szItemName = szItemName; };
	virtual bool execute(KviKvsRunTimeContext * c, KviKvsPopupMenu * p) = 0;
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupConditionalLabel : public KviKvsTreeNodeSpecialCommandDefpopupLabel
{
protected:
	QString m_szCondition; // may be null!
public:
	KviKvsTreeNodeSpecialCommandDefpopupConditionalLabel(const QChar * pLocation, const QString & szCondition, const QString & szItemName)
	    : KviKvsTreeNodeSpecialCommandDefpopupLabel(pLocation, szItemName), m_szCondition(szCondition){};
	~KviKvsTreeNodeSpecialCommandDefpopupConditionalLabel(){};

public:
	void setCondition(const QString & szCondition) { m_szCondition = szCondition; };
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator : public KviKvsTreeNodeSpecialCommandDefpopupConditionalLabel
{
public:
	KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator(const QChar * pLocation, const QString & szCondition, const QString & szItemName)
	    : KviKvsTreeNodeSpecialCommandDefpopupConditionalLabel(pLocation, szCondition, szItemName){};
	~KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator(){};

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c, KviKvsPopupMenu * p);
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon : public KviKvsTreeNodeSpecialCommandDefpopupConditionalLabel
{
protected:
	QString m_szText;
	QString m_szIcon;

public:
	KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon(const QChar * pLocation, const QString & szCondition, const QString & szText, const QString & szIcon, const QString & szItemName)
	    : KviKvsTreeNodeSpecialCommandDefpopupConditionalLabel(pLocation, szCondition, szItemName), m_szText(szText), m_szIcon(szIcon){};
	~KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon(){};

public:
	void setText(const QString & szText) { m_szText = szText; };
	void setIcon(const QString & szIcon) { m_szIcon = szIcon; };
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup : public KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon
{
protected:
	QString m_szName;

public:
	KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup(const QChar * pLocation, const QString & szCondition, const QString & szText, const QString & szIcon, const QString & szName, const QString & szItemName)
	    : KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon(pLocation, szCondition, szText, szIcon, szItemName), m_szName(szName){};
	~KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup(){};

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c, KviKvsPopupMenu * p);
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupLabelItem : public KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon
{
protected:
	QString m_szInstruction;

public:
	KviKvsTreeNodeSpecialCommandDefpopupLabelItem(const QChar * pLocation, const QString & szCondition, const QString & szText, const QString & szIcon, const QString & szInstruction, const QString & szItemName)
	    : KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon(pLocation, szCondition, szText, szIcon, szItemName), m_szInstruction(szInstruction){};
	~KviKvsTreeNodeSpecialCommandDefpopupLabelItem(){};

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c, KviKvsPopupMenu * p);
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupLabelLabel : public KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon
{
public:
	KviKvsTreeNodeSpecialCommandDefpopupLabelLabel(const QChar * pLocation, const QString & szCondition, const QString & szText, const QString & szIcon, const QString & szItemName)
	    : KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon(pLocation, szCondition, szText, szIcon, szItemName){};
	~KviKvsTreeNodeSpecialCommandDefpopupLabelLabel(){};

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c, KviKvsPopupMenu * p);
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue : public KviKvsTreeNodeSpecialCommandDefpopupLabel
{
protected:
	QString m_szInstruction;

public:
	KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue(const QChar * pLocation, const QString & szInstruction, const QString & szItemName)
	    : KviKvsTreeNodeSpecialCommandDefpopupLabel(pLocation, szItemName), m_szInstruction(szInstruction){};
	~KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue(){};

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c, KviKvsPopupMenu * p);
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue : public KviKvsTreeNodeSpecialCommandDefpopupLabel
{
protected:
	QString m_szInstruction;

public:
	KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue(const QChar * pLocation, const QString & szInstruction, const QString & szItemName)
	    : KviKvsTreeNodeSpecialCommandDefpopupLabel(pLocation, szItemName), m_szInstruction(szInstruction){};
	~KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue(){};

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c, KviKvsPopupMenu * p);
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopupLabelPopup : public KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon
{
public:
	KviKvsTreeNodeSpecialCommandDefpopupLabelPopup(const QChar * pLocation);
	~KviKvsTreeNodeSpecialCommandDefpopupLabelPopup();

protected:
	KviPointerList<KviKvsTreeNodeSpecialCommandDefpopupLabel> * m_pLabels;

public:
	bool isEmpty() { return m_pLabels->isEmpty(); };
	void addLabel(KviKvsTreeNodeSpecialCommandDefpopupLabel * pLabel);
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c, KviKvsPopupMenu * p);
	bool fill(KviKvsRunTimeContext * c, KviKvsPopupMenu * p);
};

class KVIRC_API KviKvsTreeNodeSpecialCommandDefpopup : public KviKvsTreeNodeSpecialCommand
{
public:
	KviKvsTreeNodeSpecialCommandDefpopup(const QChar * pLocation, KviKvsTreeNodeData * pPopupName, KviKvsTreeNodeSpecialCommandDefpopupLabelPopup * pMainPopup);
	~KviKvsTreeNodeSpecialCommandDefpopup();

private:
	KviKvsTreeNodeData * m_pPopupName; // can't be zero
	KviKvsTreeNodeSpecialCommandDefpopupLabelPopup * m_pMainPopup;

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

#endif //!_KVI_KVS_TREENODE_SPECIALCOMMANDDEFPOPUP_H_
