//=============================================================================
//
//   File : KvsObject_tabWidget.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KvsObject_tabWidget.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QTabWidget>

/*
	@doc:	tabWidget
	@keyterms:
		tabWidget object class,
	@title:
		tabWidget class
	@type:
		class
	@short:
		Provides a Tab Widget.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		The tabWidget class provides a stack of tabbed widgets.[br]
		Each tab is associated with a different widget (called a `page').[br]
		Only the current tab's page is shown in the page area; all the others tabs pages are hidden.[br]
		The user can show a different page by clicking on its tab.[br]
	@functions:
		!fn: $addTab(<widget:object>,<text_label:string>,[<icon_id>])
		Adds another page to the tab view with the text label and optional icon.
		!fn: $insertTab(<widget:object>,<tex_label:string>,<index:uinteger>)
		Inserts another tab and page to the tab view with name and label-text specified.[br]
		If index is not specified, the tab is simply appended.[br]
		Otherwise the new tab is inserted at the specified position.
		!fn: $setTabToolTip(<tab_widget:object>,<tooltip:string>)
		Sets the tab tool tip for the tabWidget to <tooltip>.
		!fn: $removeTabToolTip(<tab_widget:object>)
		Removes the tab tool tip for the tab. If the page does not have a tip, nothing happens.
		!fn: $setTabLabel(<tab_widget>, <label:string>)
		Sets the tab label for <tab_widget>  to label.
		!fn: $changeTab(<tab_widget:object>,<label:text>,[<icon_id>])
		Defines a new label, and optional icon, for the tab .
		!fn: $setCurrentPage(<index:uinteger>)
		Sets the index position of the current tab page to <index>.
		!fn: <integer> $currentPageIndex()
		Returns the current page index.
		!fn: <string> $tabLabel(<index:uinteger>)
		Returns the tab label of the <index>.
		!fn: <hobject> $widgetAt(<index:integer>)
		Returns the tab widget of the <index> or 0 if index is out of the tabs count.
		!fn: <index:integer> $indexOf(<widget:hobject>)
		Returns the <index> of the <widget> in the tabs.
		!fn: <string> $currentTabLabel()
		Returns the label of the current tab.
		!fn: $removePage(<tab_widget:object>)
		Remove the page <tab_widget>.
		!fn: $setTabPosition(<tab_position:string>)
		Sets TabPosition. Valid parameters are : Top, Bottom.
		!fn: <integer> $count()
		Returns the number of tabs in the tab bar.
		!fn: $setTabsClosable(<enable:bool>)
		Sets whether close buttons are automatically added to each tab
		!fn: $currentChangedEvent()
		This function is invoked when the current tab was changed.[br]
		The default implementation emits the [classfnc]$currentChange[/classfnc]() signal.
		!fn: $tabCloseRequestEvent()
		This function is invoked when when the close button on a tab is clicked.
		The index is the index of the tab that should be removed.[br]
		The default implementation emits the [classfnc]$tabCloseRequest[/classfnc]() signal.
		See also [classfnc]$setTabsClosable[/classfnc]().
	@examples:
		[example]
			[comment]# First we'll create the main tabWidget.[/comment]
			%Tabwidget=$new(tabWidget)
			%Tabwidget->$setToolTip("Example of TabWidget class")
			%Tabwidget->$setTabPosition(Top)
			%Tabwidget->$resize(300,200)
			[comment]# Now we'll create the new widgets and put they in to the main tabWidget.[/comment]
			%firsttab=$new(vbox)
			%secondtab=$new(vbox)
			[comment]# Now we'll create the item to put in to tab's pages.[/comment]
			[comment]# First tab[/comment]
			%hbox=$new(hbox,%firsttab)
			%labelbt=$new(label,%hbox)
			%labelbt->$settext(Botton Tab)
			%labeltt=$new(label,%hbox)
			%labeltt->$settext(Top Tab)
			%hbox=$new(hbox,%firsttab)
			%buttontb=$new(button,%hbox)
			%buttontb->$settext("To &Botton")
			%buttontt=$new(button,%hbox)
			%buttontt->$settext("To &Top")
			[comment]
			# Now we'll give a layout to all items.
			# This also allows to use privateimpl without making buttons global variables
			# Add the page to the main tab.
			[/comment]
			%Tabwidget->$addTab(%firsttab,Top&Button,33)
			[comment]# Implementing the action to do when the user click the buttons.[/comment]
			privateimpl(%buttontb,mousepressevent)
			{
				%Tabwidget->$setTabPosition(Bottom)
			}
			privateimpl(%buttontt,mousepressevent)
			{
				%Tabwidget->$setTabPosition(Top)
			}
			[comment]# We do the same work  with the second tab's page.[/comment]
			%labelwp=$new(label,%secondtab)
			%labelwp->$settext("Enjoy the new Class provided by")
			%labelwp->$setalignment("Center")
			%labelgen=$new(label,%secondtab)
			%labelgen->$settext(Grifisx \& Noldor)
			%labelgen->$setalignment("Center")
			%Tabwidget->$addTab(%secondtab,&About,50)
			[comment]# Let's show our example.[/comment]
			%Tabwidget->$show()
		[/example]
	@signals:
		!sg: $currentChange(<index:integer>)
		This signal is emitted by the default implementation of [classfnc]$currentChangedEvent[/classfnc]().[br]
		If you reimplement that function you will have to emit the signal manually (if you still need it).[br]
		!sg: $tabCloseRequest(<index:integer>)
		This signal is emitted by the default implementation of [classfnc]tabCloseRequestEvent[/classfnc]().[br]
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_tabWidget, "tabWidget", "widget")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, addTab)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, insertTab)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, setTabToolTip)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, removeTabToolTip)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, setTabLabel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, changeTab)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, setCurrentPage)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, currentPageIndex)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, tabLabel)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, currentTabLabel)

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, count)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, removePage)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, widgetAt)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, indexOf)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, setTabPosition)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, setTabsClosable)

// events
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, currentChangedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_tabWidget, tabCloseRequestEvent)

KVSO_END_REGISTERCLASS(KvsObject_tabWidget)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_tabWidget, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_tabWidget)

KVSO_BEGIN_DESTRUCTOR(KvsObject_tabWidget)
tabsList.clear();
KVSO_END_DESTRUCTOR(KvsObject_tabWidget)

bool KvsObject_tabWidget::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QTabWidget)
	connect(widget(), SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged(int)));
	connect(widget(), SIGNAL(tabCloseRequested(int)), this, SLOT(slotTabCloseRequest(int)));

	return true;
}
KVSO_CLASS_FUNCTION(tabWidget, setTabsClosable)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bClosable;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bool_flag", KVS_PT_BOOLEAN, 0, bClosable)
	KVSO_PARAMETERS_END(c)
	((QTabWidget *)widget())->setTabsClosable(bClosable);
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, addTab)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	QString szLabel, szIcon;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("label", KVS_PT_STRING, 0, szLabel)
	KVSO_PARAMETER("icon_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	QPixmap * pix = g_pIconManager->getImage(szIcon);
	if(pix)
	{
		((QTabWidget *)widget())->addTab(((QWidget *)(pObject->object())), QIcon(*pix), szLabel);
	}
	else
		((QTabWidget *)widget())->addTab(((QWidget *)(pObject->object())), szLabel);
	this->children();
	tabsList.append(hObject);
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, indexOf)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	QString szLabel, szIcon;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int iIdx = ((QTabWidget *)widget())->indexOf(((KvsObject_widget *)pObject)->widget());
	c->returnValue()->setInteger((kvs_int_t)iIdx);
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, widgetAt)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_INTEGER, 0, iIndex)
	KVSO_PARAMETERS_END(c)
	if(iIndex >= tabsList.count() || iIndex < 0)
		c->returnValue()->setHObject((kvs_hobject_t) nullptr);
	else
		c->returnValue()->setHObject(tabsList.at(iIndex));
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, insertTab)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	QString szLabel, szIcon;
	kvs_uint_t uIndex;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tab_widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("label", KVS_PT_STRING, 0, szLabel)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, uIndex)
	KVSO_PARAMETER("icon_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	QPixmap * pix = g_pIconManager->getImage(szIcon);
	if(pix)
		((QTabWidget *)widget())->insertTab(uIndex, (QWidget *)(pObject->object()), QIcon(*pix), szLabel);
	else
		((QTabWidget *)widget())->insertTab(uIndex, ((QWidget *)(pObject->object())), szLabel);
	tabsList.insert(uIndex, hObject);
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, setCurrentPage)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UNSIGNEDINTEGER, 0, iIndex)
	KVSO_PARAMETERS_END(c)
	((QTabWidget *)widget())->setCurrentIndex(iIndex);
	return true;
}
KVSO_CLASS_FUNCTION(tabWidget, setTabToolTip)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	QString szTooltip;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tab_widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("tooltip", KVS_PT_STRING, 0, szTooltip)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int iIdx = ((QTabWidget *)widget())->indexOf(((QWidget *)(pObject->object())));
	if(iIdx == -1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab ", "objects"));
		return true;
	}
	((QTabWidget *)widget())->setTabToolTip(iIdx, szTooltip);
	return true;
}
KVSO_CLASS_FUNCTION(tabWidget, removeTabToolTip)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tab_widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int iIdx = ((QTabWidget *)widget())->indexOf(((QWidget *)(pObject->object())));
	if(iIdx == -1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab ", "objects"));
		return true;
	}
	((QTabWidget *)widget())->setTabToolTip(iIdx, QString());
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, setTabLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	QString szLabel;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tab_widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("tooltip", KVS_PT_STRING, 0, szLabel)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int iIdx = ((QTabWidget *)widget())->indexOf(((QWidget *)(pObject->object())));
	if(iIdx == -1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab ", "objects"));
		return true;
	}
	((QTabWidget *)widget())->setTabText(iIdx, szLabel);
	return true;
}
KVSO_CLASS_FUNCTION(tabWidget, currentPageIndex)
{
	CHECK_INTERNAL_POINTER(widget())
	int index = ((QTabWidget *)widget())->currentIndex();
	c->returnValue()->setInteger(index);
	return true;
}
KVSO_CLASS_FUNCTION(tabWidget, tabLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_INT, 0, uIndex)
	KVSO_PARAMETERS_END(c)
	QString label = ((QTabWidget *)widget())->tabText(uIndex);
	c->returnValue()->setString(label);
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, count)
{
	CHECK_INTERNAL_POINTER(widget())
	int count = ((QTabWidget *)widget())->count();
	c->returnValue()->setInteger(count);
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, currentTabLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	int i = ((QTabWidget *)widget())->currentIndex();
	QString label = ((QTabWidget *)widget())->tabText(i);
	c->returnValue()->setString(label);
	return true;
}
KVSO_CLASS_FUNCTION(tabWidget, removePage)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tab_widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int iIdx = ((QTabWidget *)widget())->indexOf(((QWidget *)(pObject->object())));
	if(iIdx == -1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab ", "objects"));
		return true;
	}
	((QTabWidget *)widget())->removeTab(iIdx);
	tabsList.removeAt(iIdx);
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, changeTab)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * pObject;
	QString szLabel, szIcon;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tab_widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("label", KVS_PT_STRING, 0, szLabel)
	KVSO_PARAMETER("icon_id", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int iIdx = ((QTabWidget *)widget())->indexOf(((QWidget *)(pObject->object())));
	if(iIdx == -1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab", "objects"));
		return true;
	}
	QPixmap * pix = g_pIconManager->getImage(szIcon);
	if(pix)
		((QTabWidget *)widget())->setTabIcon(iIdx, QIcon(*pix));
	((QTabWidget *)widget())->setTabText(iIdx, szLabel);
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, setTabPosition)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szPos;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("tab_position", KVS_PT_STRING, 0, szPos)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szPos, "Top"))
		((QTabWidget *)widget())->setTabPosition(QTabWidget::North);
	else if(KviQString::equalCI(szPos, "Bottom"))
		((QTabWidget *)widget())->setTabPosition(QTabWidget::South);
	else
		c->warning(__tr2qs_ctx("Unknown position '%Q'", "objects"), &szPos);
	return true;
}

KVSO_CLASS_FUNCTION(tabWidget, currentChangedEvent)
{
	emitSignal("currentChanged", c, c->params());
	return true;
}

void KvsObject_tabWidget::slotCurrentChanged(int value)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)value));
	callFunction(this, "currentChangedEvent", &params);
}

KVSO_CLASS_FUNCTION(tabWidget, tabCloseRequestEvent)
{
	emitSignal("tabCloseRequested", c, c->params());
	return true;
}

void KvsObject_tabWidget::slotTabCloseRequest(int iIndex)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)iIndex));
	callFunction(this, "tabCloseRequestEvent", &params);
}
