#ifndef _CLASSEDITOR_H_
#define _CLASSEDITOR_H_
//=============================================================================
//
//   File : ClassEditorWindow.h
//   Creation date : Mon Feb 15 2010 14:35:55 CEST by Carbone Alessandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Alessandro Carbone (elfonol at gmail dot com)
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

/**
* \file ClassEditorWindow.h
* \author Alessandro Carbone
* \brief Class Editor window
*/

#include "KviWindow.h"
#include "KviCString.h"
#include "KviPointerList.h"
#include "KviHeapObject.h"
#include "KviKvsObjectClass.h"

#include <QWidget>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QTreeWidget>
#include <QMenu>

class QLineEdit;
class QStringList;
class QPushButton;
class QCheckBox;
class QLabel;
class QSplitter;
class KviScriptEditor;

/**
* \class ClassEditorTreeWidget
* \brief Describes the tree widget hierarchy
*/
class ClassEditorTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	/**
	* \brief Construct the tree widget
	* \param pParent The parent widget
	* \return ClassEditorTreeWidget
	*/
	ClassEditorTreeWidget(QWidget * pParent);

	/**
	* \brief Destroys the tree widget
	*/
	~ClassEditorTreeWidget();

protected:
	void mousePressEvent(QMouseEvent * e) override;
signals:
	/**
	* \brief Emitted when we press the mouse right button
	* \param pItem The item clicked
	* \param pos The position of the cursor
	* \return void
	*/
	void rightButtonPressed(QTreeWidgetItem * pItem, QPoint pos);
};

/**
* \class ClassEditorTreeWidgetItem
* \brief Describes an item inside the class editor tree widget
*/
class ClassEditorTreeWidgetItem : public QTreeWidgetItem, public KviHeapObject
{
public:
	/**
	* \enum Type
	* \brief Contains the type of the class item
	*/
	enum Type
	{
		Class,     /**< The item is a class */
		Namespace, /**< The item is a namespace */
		Method     /**< The item is a member function */
	};

public:
	/**
	* \brief Constructs the class editor tree widget item
	* \param pTreeWidget The parent tree widget
	* \param eType The type of the item
	* \param szName The name of the item
	* \return ClassEditorTreeWidgetItem
	*/
	ClassEditorTreeWidgetItem(QTreeWidget * pTreeWidget, Type eType, const QString & szName);

	/**
	* \brief Constructs the class editor tree widget item
	* \param pParentItem The parent tree widget item
	* \param eType The type of the item
	* \param szName The name of the item
	* \return ClassEditorTreeWidgetItem
	*/
	ClassEditorTreeWidgetItem(ClassEditorTreeWidgetItem * pParentItem, Type eType, const QString & szName);

	/**
	* \brief Destroys the class editor tree widget item
	*/
	~ClassEditorTreeWidgetItem(){};

protected:
	Type m_eType;
	QString m_szName;
	QString m_szBuffer;
	QString m_szInheritsClassName;
	QString m_szReminder;
	bool m_bClassModified;
	bool m_bInternal;
	int m_iPos;

public:
	/**
	* \brief Sets the class name on which the class inherits from
	* \param szInheritsClassName The parent class name
	* \return void
	*/
	void setInheritsClass(QString szInheritsClassName) { m_szInheritsClassName = szInheritsClassName; };

	/**
	* \brief Returns the class name of the classes we inherit from
	* \return QString
	*/
	QString inheritsClass() { return m_szInheritsClassName; };

	/**
	* \brief Returns the name of the item
	* \return const QString &
	*/
	const QString & name() { return m_szName; };

	/**
	* \brief Sets the name of the item
	* \param szName The name of the item :P
	* \return void
	*/
	void setName(const QString & szName);

	/**
	* \brief Sets the list of parameters the function accept
	* \param szRem The parameters' list
	* \return void
	*/
	void setReminder(const QString & szRem) { m_szReminder = szRem; };

	/**
	* \brief Returns the parameters list
	* \return QString
	*/
	QString reminder() { return m_szReminder; };

	/**
	* \brief Sets the state of the class as non-built
	* \param bModified Whether the class is modified
	* \return void
	*/
	void setClassNotBuilt(bool bModified);

	/**
	* \brief Returns true if we class is not built yet, false otherwise
	* \return bool
	*/
	bool classNotBuilt() { return m_bClassModified; };

	/**
	* \brief Sets the function as internal
	* \param bInternal Whether the function is internal or not
	* \return void
	*/
	void setInternalFunction(bool bInternal) { m_bInternal = bInternal; };

	/**
	* \brief Returns true if the function is set as internal, false otherwise
	* \return bool
	*/
	bool isInternalFunction() { return m_bInternal; };

	/**
	* \brief Returns the type of the item
	* \return Type
	*/
	Type type() { return m_eType; };

	/**
	* \brief Sets the type of the item
	* \param eType The type!
	* \return void
	*/
	void setType(Type eType);

	/**
	* \brief Returns true if the item is a class, false otherwise
	* \return bool
	*/
	bool isClass() { return m_eType == Class; };

	/**
	* \brief Returns true if the item is a namespace, false otherwise
	* \return bool
	*/
	bool isNamespace() { return m_eType == Namespace; };

	/**
	* \brief Returns true if the item is a member function, false otherwise
	* \return bool
	*/
	bool isMethod() { return m_eType == Method; };

	/**
	* \brief Saves the code of the item in a buffer
	* \param szBuffer The buffer ;)
	* \return void
	*/
	void setBuffer(const QString & szBuffer) { m_szBuffer = szBuffer; };

	/**
	* \brief Returns the buffer containing the code of the item
	* \return const QString &
	*/
	const QString & buffer() { return m_szBuffer; };

	/**
	* \brief Returns the cursor's position
	* \return const int &
	*/
	const int & cursorPosition() { return m_iPos; };

	/**
	* \brief Sets the cursor position
	* \param iPos The position of the cursor
	* \return void
	*/
	void setCursorPosition(const int & iPos) { m_iPos = iPos; };
};

/**
* \class ClassEditorWidget
* \brief The Class Editor module
*/
class ClassEditorWidget : public QWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the class editor object
	* \param pParent The parent widget
	* \return ClassEditorWidget
	*/
	ClassEditorWidget(QWidget * pParent);

	/**
	* \brief Destroys the class editor object
	*/
	~ClassEditorWidget();

public:
	KviScriptEditor * m_pEditor;
	ClassEditorTreeWidget * m_pTreeWidget;
	QLabel * m_pClassNameLabel;
	QPushButton * m_pClassNameRenameButton;
	QLabel * m_pFunctionNameLabel;
	QPushButton * m_pFunctionNameRenameButton;
	QLabel * m_pReminderLabel;

	ClassEditorTreeWidgetItem * m_pLastEditedItem;
	ClassEditorTreeWidgetItem * m_pLastClickedItem;
	QMenu * m_pContextPopup;
	QSplitter * m_pSplitter;
	QString m_szDir;
	bool m_bSaving;

protected:
	KviPointerHashTable<QString, ClassEditorTreeWidgetItem> * m_pClasses;

public:
	/**
	* \brief Called to save the window properties
	* \param pCfg The configuration file
	* \return void
	*/
	void saveProperties(KviConfigurationFile * pCfg);

	/**
	* \brief Called to load the window properties
	* \param pCfg The configuration file
	* \return void
	*/
	void loadProperties(KviConfigurationFile * pCfg);

	/**
	* \brief Builds the class!
	* \return void
	*/
	void build();

	/**
	* \brief Saves non-built classes to a file
	* \return void
	*/
	void saveNotBuiltClasses();

	/**
	* \brief Loads non-built classes to a file
	* \return void
	*/
	void loadNotBuiltClasses();

	/**
	* \brief Searches a function
	* \param szFunctionName The name of the function
	* \param pClass The class in which the function is
	* \return ClassEditorTreeWidgetItem *
	*/
	ClassEditorTreeWidgetItem * findFunction(const QString & szFunctionName, ClassEditorTreeWidgetItem * pClass);

	/**
	* \brief Exports class to a file
	* \param bSelectedOnly Whether to export only selected files
	* \param bSingleFiles Whether to export to single files
	* \return void
	*/
	void exportClasses(bool bSelectedOnly, bool bSingleFiles = false);

	/**
	* \brief Exports the current selection to single files
	* \param pList The list of file to export
	* \return void
	*/
	void exportSelectionInSinglesFiles(KviPointerList<ClassEditorTreeWidgetItem> * pList);

	/**
	* \brief Removes the item from the tree
	* \param pItem The item to remove
	* \return void
	*/
	void cutItem(ClassEditorTreeWidgetItem * pItem);

protected:
	//void recursiveSearchReplace(const QString &szSearch,ClassEditorTreeWidgetItem * it,bool bReplace=false,const QString &szReplace="n");

	void exportClassBuffer(QString & szBuffer, ClassEditorTreeWidgetItem * pItem);

	void oneTimeSetup();
	void createFullClass(KviKvsObjectClass * pClass, ClassEditorTreeWidgetItem * pItem, const QString & szClassName);

	void saveLastEditedItem();
	void appendSelectedClassItems(KviPointerList<ClassEditorTreeWidgetItem> * pList);
	void appendSelectedClassItemsRecursive(KviPointerList<ClassEditorTreeWidgetItem> * pList, QTreeWidgetItem * pStartFrom);

	void appendAllClassItems(KviPointerList<ClassEditorTreeWidgetItem> * pList);
	void appendAllClassItemsRecursive(KviPointerList<ClassEditorTreeWidgetItem> * l, QTreeWidgetItem * pStartFrom);

	void openParentItems(QTreeWidgetItem * pItem);
	void activateItem(QTreeWidgetItem * pItem);
	bool hasSelectedItems();
	bool askForClassName(QString & szClassName, QString & szInheritsClassName, bool bEdit);
	bool askForNamespaceName(const QString & szAction, const QString & szText, const QString & szInitialText, QString & szNameBuffer);
	bool askForFunction(QString & szFunctionName, QString & szReminder, bool * pbInternal, const QString & szClassName, bool bRenameMode);
	void searchInheritedClasses(const QString szClass, KviPointerList<ClassEditorTreeWidgetItem> & lInheritsedClasses);

	/**
	* \brief Returns true if the class exists, false otherwise
	* \param szName The name of the class
	* \return bool
	*/
	bool classExists(QString & szName);

	/**
	* \brief Renames a class
	* \param pClass The class to rename
	* \return void
	*/
	void renameClass(ClassEditorTreeWidgetItem * pClass);

	/**
	* \brief Renames a namespace
	* \param pNamespace The namespace to rename
	* \return void
	*/
	void renameNamespace(ClassEditorTreeWidgetItem * pNamespace);

	bool removeItem(ClassEditorTreeWidgetItem * pItem, KviPointerList<ClassEditorTreeWidgetItem> & lRemovedItems, bool * pbYesToAll);
	void updateClassHierarchy(ClassEditorTreeWidgetItem * pClass);

	void removeItemChildren(ClassEditorTreeWidgetItem * pItem, KviPointerList<ClassEditorTreeWidgetItem> & lRemovedItems);
	void buildFullItemPath(ClassEditorTreeWidgetItem * pItem, QString & szBuffer);
	QString buildFullClassName(ClassEditorTreeWidgetItem * pItem);
	ClassEditorTreeWidgetItem * createFullItem(const QString & szFullName);
	ClassEditorTreeWidgetItem * createFullNamespace(const QString & szFullName);

	/**
	* \brief Searches an item
	* \param szName The name of the item to find
	* \return ClassEditorTreeWidgetItem *
	*/
	ClassEditorTreeWidgetItem * findItem(const QString & szName);

	ClassEditorTreeWidgetItem * findTopLevelItem(const QString & szName);
	ClassEditorTreeWidgetItem * createFullItemRecursive(QTreeWidgetItem * pCurrent, QStringList & lNameSpaces, int & iLevel, bool bCreateNameSpace = 0);
protected slots:
	void currentItemChanged(QTreeWidgetItem * pItem, QTreeWidgetItem *);
	void customContextMenuRequested(QPoint);
	void newClass();
	void newNamespace();
	void newMemberFunction();
	void renameFunction();
	ClassEditorTreeWidgetItem * newItem(QString & szName, ClassEditorTreeWidgetItem::Type eType);

	void renameItem();

	void exportAll();
	void exportSelectedSepFiles();
	void exportSelected();
	void slotFind();
	void slotFindWord(const QString &);
	void searchReplace(const QString & szSearch, bool bReplace = false, const QString & szReplace = "n");

	/**
	* \brief Called when we collapse an item
	* \return void
	*/
	void slotCollapseItems();

	/**
	* \brief Called when we collapse an item recursively
	* \param pItem The starting item to collapse
	* \return void
	*/
	void recursiveCollapseItems(ClassEditorTreeWidgetItem * pItem);

	/**
	* \brief Called when we want to remove all selected items
	* \return void
	*/
	void removeSelectedItems();

	/**
	* \brief Called when we replace text everywhere
	* \param szBefore The text before the replace
	* \param szAfter The text after the replace
	* \return void
	*/
	void slotReplaceAll(const QString & szBefore, const QString & szAfter);
};

/**
* \class ClassEditorWindow
* \brief This is the Class Editor window
*/
class ClassEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the class editor window
	* \return ClassEditorWindow
	*/
	ClassEditorWindow();

	/**
	* \brief Destroys the class editor window
	*/
	~ClassEditorWindow();

protected:
	ClassEditorWidget * m_pEditor;

protected:
	/**
	* \brief Returns the class editor small icon
	* \return QPixmap *
	*/
	QPixmap * myIconPtr() override;

	/**
	* \brief Sets the configuration group name as classeditor
	* \param szName The name of the group
	* \return void
	*/
	virtual void configGroupName(QString & szName);

	/**
	* \brief Called to save the window properties
	* \param pCfg The configuration file
	* \return void
	*/
	void saveProperties(KviConfigurationFile * pCfg) override;

	/**
	* \brief Called to load the window properties
	* \param pCfg The configuration file
	* \return void
	*/
	void loadProperties(KviConfigurationFile * pCfg) override;
protected slots:
	/**
	* \brief Called when we click the cancel button
	* \return void
	*/
	void cancelClicked();

	/**
	* \brief Called when we want to build the class
	* \return void
	*/
	void buildClicked();

	/**
	* \brief Called when we want to save the class
	* \return void
	*/
	void saveClicked();
};

/**
* \class KviClassEditorDialog
* \brief This class constructs the dialog to create a new class
*/
class KviClassEditorDialog : public QDialog
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the class dialog
	* \param pParent The parent widget
	* \param szName The name of the widget
	* \param pClasses The container of all classes of our namespace
	* \param szClassName The name of the class
	* \param szInheritsClassName The name of the classes in which this inherits from
	* \param bRenameMode Whether the dialog is in rename mode
	* \return KviClassEditorDialog
	*/
	KviClassEditorDialog(QWidget * pParent, const QString & szName, KviPointerHashTable<QString, ClassEditorTreeWidgetItem> * pClasses, const QString & szClassName, const QString & szInheritsClassName, bool bRenameMode = false);

	/**
	* \brief Destroys the class dialog
	*/
	~KviClassEditorDialog();

protected:
	QPushButton * m_pNewClassButton;
	QLineEdit * m_pClassNameLineEdit;
	QComboBox * m_pInheritsClassComboBox;

public:
	/**
	* \brief Returns the class name
	* \return QString
	*/
	QString className() { return m_pClassNameLineEdit->text(); };

	/**
	* \brief Returns the class name of the parent classes
	* \return QString
	*/
	QString inheritsClassName() { return m_pInheritsClassComboBox->currentText(); };
protected slots:
	/**
	* \brief Triggered when the name of the class change
	* \return void
	*/
	void textChanged(const QString &);
};

/**
* \class KviClassEditorFunctionDialog
* \brief This class constructs the dialog to create a new member function
*/
class KviClassEditorFunctionDialog : public QDialog
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the function dialog
	* \param pParent The parent widget
	* \param szName The name of the widget
	* \param szClassName The name of the class
	* \param szFunctionName The name of the function
	* \param szReminder The parameters of the function
	* \param bIsInternal Whether the function is set as internal
	* \param bRenameMode Whether the dialog is in rename mode
	* \return KviClassEditorFunctionDialog
	*/
	KviClassEditorFunctionDialog(QWidget * pParent, const QString & szName, const QString & szClassName, const QString & szFunctionName, const QString & szReminder, bool bIsInternal, bool bRenameMode = false);

	/**
	* \brief Destroys the function dialog
	*/
	~KviClassEditorFunctionDialog();

protected:
	QPushButton * m_pNewFunctionButton;
	QLineEdit * m_pFunctionNameLineEdit;
	QLineEdit * m_pReminderLineEdit;
	QCheckBox * m_pInternalCheckBox;

public:
	/**
	* \brief Returns the function name
	* \return QString
	*/
	QString functionName() { return m_pFunctionNameLineEdit->text(); };

	/**
	* \brief Returns the parameters of the function
	* \return QString
	*/
	QString reminder() { return m_pReminderLineEdit->text(); };

	/**
	* \brief Returns true if the function is set as internal, false otherwise
	* \return bool
	*/
	bool isInternalFunction() { return m_pInternalCheckBox->isChecked(); };
protected slots:
	/**
	* \brief Triggered when the name of the function change
	* \return void
	*/
	void textChanged(const QString &);
};
#endif //_CLASSEDITOR_H_
