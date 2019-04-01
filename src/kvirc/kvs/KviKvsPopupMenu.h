#ifndef _KVI_KVS_POPUPMENU_H_
#define _KVI_KVS_POPUPMENU_H_
//=============================================================================
//
//   File : KviKvsPopupMenu.h
//   Creation date : Wed 07 Jan 2004 05:02:57 by Szymon Stefanek
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
#include "KviWindow.h"
#include "KviKvsHash.h"
#include "KviKvsVariantList.h"
#include "KviKvsScript.h"
#include "KviKvsRunTimeContext.h"

#include <QLabel>
#include <QPixmap>
#include <QMenu>

class KviKvsPopupMenu;

class KVIRC_API KviKvsPopupMenuTopLevelData
{
	friend class KviKvsPopupMenu;

public:
	KviKvsPopupMenuTopLevelData(KviKvsVariantList * pParameters, KviWindow * pWindow);
	~KviKvsPopupMenuTopLevelData();
	enum LockStatus
	{
		Unlocked,
		SoftLocked,
		HardLocked
	};

protected:
	KviKvsExtendedRunTimeData * m_pExtendedRunTimeData = nullptr;
	KviKvsVariantList * m_pParameters = nullptr;
	KviWindow * m_pWindow = nullptr;
	LockStatus m_eLocked = Unlocked;
	bool m_bTestMode = false;

public:
	KviKvsExtendedRunTimeData * extendedRunTimeData() const { return m_pExtendedRunTimeData; }
	KviKvsVariantList * parameters() const { return m_pParameters; }
	bool isSoftLocked() const { return m_eLocked != Unlocked; }
	bool isHardLocked() const { return m_eLocked == HardLocked; }
	void setLocked(LockStatus eLocked) { m_eLocked = eLocked; }
	KviWindow * window() const { return m_pWindow; }
	void setWindow(KviWindow * pWindow) { m_pWindow = pWindow; }
	bool testMode() const { return m_bTestMode; }
	void setTestMode(bool bTestMode) { m_bTestMode = bTestMode; }
};

class KVIRC_API KviKvsPopupMenuItem
{
	friend class KviKvsPopupMenu;

public:
	enum Type
	{
		Item,
		Menu,
		Label,
		Separator,
		ExtMenu
	};

protected:
	KviKvsPopupMenuItem(Type t, const QString & szItemName, const QString & szCondition);
	KviKvsPopupMenuItem(Type t, QString szItemName, const KviKvsScript * pCondition);

public:
	virtual ~KviKvsPopupMenuItem();

protected:
	QString m_szItemName;
	Type m_eType;
	KviKvsScript * m_pKvsCondition = nullptr;

public:
	// this doesn't trigger errors, only warnings
	bool evaluateCondition(KviKvsPopupMenuTopLevelData * pData);

	KviKvsPopupMenuItem::Type type() const { return m_eType; }

	const QString & name() const { return m_szItemName; }

	bool isItem() const { return m_eType == Item; }
	bool isSeparator() const { return m_eType == Separator; }
	bool isLabel() const { return m_eType == Label; }
	bool isExtMenu() const { return m_eType == ExtMenu; }
	bool isMenu() const { return m_eType == Menu; }

	KviKvsScript * kvsCondition() const { return m_pKvsCondition; }
	virtual KviKvsScript * kvsIcon() const;
	virtual KviKvsScript * kvsText() const;
	virtual KviKvsScript * kvsCode() const;

	virtual void fill(KviKvsPopupMenu * pMenu, KviKvsPopupMenuTopLevelData * pData, int iIdx) = 0;
	virtual void clear();
	virtual KviKvsPopupMenuItem * clone() const = 0;
};

class KVIRC_API KviKvsPopupMenuItemSeparator : public KviKvsPopupMenuItem
{
	friend class KviKvsPopupMenu;

protected:
	KviKvsPopupMenuItemSeparator(const QString & szItemName, const QString & szCondition);
	KviKvsPopupMenuItemSeparator(const QString & szItemName, const KviKvsScript * pCondition);

public:
	~KviKvsPopupMenuItemSeparator();

public:
	void fill(KviKvsPopupMenu * pMenu, KviKvsPopupMenuTopLevelData * pData, int iIdx) override;
	KviKvsPopupMenuItem * clone() const override;
};

class KVIRC_API KviKvsPopupMenuItemWithTextAndIcon : public KviKvsPopupMenuItem
{
protected:
	KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Type t, const QString & szItemName, const QString & szText, const QString & szIcon, const QString & szCondition);
	KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Type t, const QString & szItemName, const KviKvsScript * pText, const KviKvsScript * pIcon, const KviKvsScript * pCondition);

public:
	~KviKvsPopupMenuItemWithTextAndIcon();

protected:
	KviKvsScript * m_pKvsText = nullptr;
	KviKvsScript * m_pKvsIcon = nullptr;

public:
	KviKvsScript * kvsIcon() const override;
	KviKvsScript * kvsText() const override;
	// this just returns a string, eventually empty
	QString evaluateText(KviKvsPopupMenuTopLevelData * pData);
	// this just returns the icon, eventually
	QPixmap * evaluateIcon(KviKvsPopupMenuTopLevelData * pData);
};

class KviKvsPopupMenuItemLabel;

// this acts as signal relay
class KVIRC_API KviKvsPopupMenuItemLabelHelper : public QObject
{
	Q_OBJECT
	friend class KviKvsPopupMenuItemLabel;

protected:
	KviKvsPopupMenuItemLabelHelper(KviKvsPopupMenuItemLabel * pItem);
	~KviKvsPopupMenuItemLabelHelper();

protected:
	KviKvsPopupMenuItemLabel * m_pItem = nullptr;
protected slots:
	void labelDestroyed();
};

class KVIRC_API KviKvsPopupMenuItemLabel : public KviKvsPopupMenuItemWithTextAndIcon
{
	friend class KviKvsPopupMenuItemLabelHelper;
	friend class KviKvsPopupMenu;

protected:
	KviKvsPopupMenuItemLabel(const QString & szItemName, const QString & szText, const QString & szIcon, const QString & szCondition);
	KviKvsPopupMenuItemLabel(const QString & szItemName, const KviKvsScript * pText, const KviKvsScript * pIcon, const KviKvsScript * pCondition);

public:
	~KviKvsPopupMenuItemLabel();

protected:
	QLabel * m_pLabel = nullptr;
	KviKvsPopupMenuItemLabelHelper * m_pSignalRelay = nullptr;

public:
	void fill(KviKvsPopupMenu * pMenu, KviKvsPopupMenuTopLevelData * pData, int iIdx) override;
	KviKvsPopupMenuItem * clone() const override;
	void clear() override;

protected:
	void labelDestroyed();
};

class KVIRC_API KviKvsPopupMenuItemItem : public KviKvsPopupMenuItemWithTextAndIcon
{
	friend class KviKvsPopupMenu;

protected:
	KviKvsPopupMenuItemItem(const QString & szItemName, const QString & szCode, const QString & szText, const QString & szIcon, const QString & szCondition);
	KviKvsPopupMenuItemItem(const QString & szItemName, const KviKvsScript * pCode, const KviKvsScript * pText, const KviKvsScript * pIcon, const KviKvsScript * pCondition);

public:
	~KviKvsPopupMenuItemItem();

protected:
	KviKvsScript * m_pKvsCode = nullptr;

protected:
	KviKvsScript * kvsCode() const override;

public:
	void fill(KviKvsPopupMenu * pMenu, KviKvsPopupMenuTopLevelData * pData, int iIdx) override;
	KviKvsPopupMenuItem * clone() const override;
};

class KVIRC_API KviKvsPopupMenuItemMenu : public KviKvsPopupMenuItemWithTextAndIcon
{
	friend class KviKvsPopupMenu;

protected:
	KviKvsPopupMenuItemMenu(const QString & szItemName, KviKvsPopupMenu * pMenu, const QString & szText, const QString & szIcon, const QString & szCondition);
	KviKvsPopupMenuItemMenu(const QString & szItemName, KviKvsPopupMenu * pMenu, const KviKvsScript * pText, const KviKvsScript * pIcon, const KviKvsScript * pCondition);

public:
	~KviKvsPopupMenuItemMenu();

protected:
	KviKvsPopupMenu * m_pMenu = nullptr;

public:
	KviKvsPopupMenu * menu() const { return m_pMenu; }
	void fill(KviKvsPopupMenu * pMenu, KviKvsPopupMenuTopLevelData * pData, int iIdx) override;
	void clear() override;
	KviKvsPopupMenuItem * clone() const override;
};

class KVIRC_API KviKvsPopupMenuItemExtMenu : public KviKvsPopupMenuItemWithTextAndIcon
{
	friend class KviKvsPopupMenu;

protected:
	KviKvsPopupMenuItemExtMenu(const QString & szItemName, QString szMenuName, const QString & szText, const QString & szIcon, const QString & szCondition);
	KviKvsPopupMenuItemExtMenu(const QString & szItemName, QString szMenuName, const KviKvsScript * pText, const KviKvsScript * pIcon, const KviKvsScript * pCondition);

public:
	~KviKvsPopupMenuItemExtMenu();

protected:
	QString m_szMenuName;
	KviKvsPopupMenu * m_pMenu = nullptr; // owned!
public:
	const QString & extName() const { return m_szMenuName; }
	void fill(KviKvsPopupMenu * pMenu, KviKvsPopupMenuTopLevelData * pData, int iIdx) override;
	void clear() override;
	KviKvsPopupMenuItem * clone() const override;
};

class KVIRC_API KviKvsPopupMenu : public QMenu
{
	Q_OBJECT
	friend class KviKvsPopupMenuItemMenu;
	friend class KviKvsPopupManager;
	friend class KviSinglePopupEditor;

public:
	KviKvsPopupMenu(const QString & szName);
	~KviKvsPopupMenu();

protected:
	KviPointerList<KviKvsPopupMenuItem> * m_pItemList = nullptr;

private:
	QString m_szName;
	KviKvsPopupMenu * m_pParentPopup = nullptr;
	KviPointerList<KviKvsScript> * m_pPrologues = nullptr;
	KviPointerList<KviKvsScript> * m_pEpilogues = nullptr;
	KviKvsPopupMenuTopLevelData * m_pTopLevelData = nullptr;
	// this is a temporary used to hack-in the activation from KviMenuBar
	KviKvsPopupMenuTopLevelData * m_pTempTopLevelData = nullptr;
	bool m_bSetupDone = false;

public:
	const QString & popupName() const { return m_szName; }
	void setPopupName(const QString & szName) { m_szName = szName; }
	void copyFrom(const KviKvsPopupMenu * src);
	KviKvsPopupMenuTopLevelData * topLevelData();
	KviKvsPopupMenu * topLevelPopup();
	KviPointerList<KviKvsPopupMenuItem> * itemList() const { return m_pItemList; }
	bool isSoftLocked();
	bool isHardLocked();
	void lock(KviKvsPopupMenuTopLevelData::LockStatus eLock);
	KviKvsPopupMenu * addPopup(const QString & szItemName, const QString & szText, const QString & szIcon, const QString & szCondition);
	void addSeparator(const QString & szItemName, const QString & szCondition);
	void addLabel(const QString & szItemName, const QString & szText, const QString & szIcon, const QString & szCondition);
	void addItem(const QString & szItemName, const QString & szCode, const QString & szText, const QString & szIcon, const QString & szCondition);
	void addExtPopup(const QString & szItemName, const QString & szPopupName, const QString & szText, const QString & szIcon, const QString & szCondition);
	void doPopup(const QPoint & pnt, KviWindow * wnd, KviKvsVariantList * pParams, bool bTestMode = false);
	bool isEmpty() const { return m_pItemList->isEmpty() && m_pPrologues->isEmpty() && m_pEpilogues->isEmpty(); }
	void doClear();
	void addPrologue(const QString & szItemName, const QString & szCode);
	void addEpilogue(const QString & szItemName, const QString & szCode);
	bool removeItemByName(const QString & szItemName, bool bRecursive);
	KviKvsPopupMenu * findChildPopupByName(const QString & szItemName);
	KviPointerList<KviKvsScript> * epilogues() const { return m_pEpilogues; }
	KviPointerList<KviKvsScript> * prologues() const { return m_pPrologues; }
	KviKvsPopupMenu * parentPopup() const { return m_pParentPopup; }
	void generateDefPopup(QString & buffer);
	void setParentPopup(KviKvsPopupMenu * par) { m_pParentPopup = par; }
	void generateDefPopupCore(QString & buffer);

protected:
	void addItemInternal(KviKvsPopupMenuItem * it);
	void executePrologues(KviKvsPopupMenuTopLevelData * pData);
	void executeEpilogues(KviKvsPopupMenuTopLevelData * pData);
	void load(const QString & prefix, KviConfigurationFile * cfg);
	void save(const QString & prefix, KviConfigurationFile * cfg);
private slots:
	void setupMenuContents();

protected:
	void clearMenuContents();
protected slots:
	void itemClicked(QAction * pAction);
signals:
	void testModeItemClicked(KviKvsPopupMenuItem * it);
};

#endif //!_KVI_KVS_POPUPMENU_H_
