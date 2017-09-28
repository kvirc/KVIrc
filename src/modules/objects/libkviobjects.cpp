//=============================================================================
//
//   File : libkviobjects.cpp
//   Creation date : Wed Sep 09 2000 20:59:01 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KvsObject_button.h"
#include "KvsObject_checkBox.h"
#include "KvsObject_comboBox.h"
#include "KvsObject_dialog.h"
#include "KvsObject_colorDialog.h"

#include "KvsObject_dockWindow.h"
#include "KvsObject_file.h"
#include "KvsObject_ftp.h"
#include "KvsObject_groupBox.h"
#include "KvsObject_buttonGroup.h"
#include "KvsObject_hBox.h"
#include "KvsObject_http.h"
#include "KvsObject_label.h"
#include "KvsObject_layout.h"
#include "KvsObject_lcd.h"
#include "KvsObject_lineEdit.h"
#include "KvsObject_list.h"
#include "KvsObject_listWidget.h"
#include "KvsObject_mainWindow.h"
#include "KvsObject_memoryBuffer.h"
#include "KvsObject_menuBar.h"
#include "KvsObject_multiLineEdit.h"
#include "KvsObject_painter.h"
#include "KvsObject_pixmap.h"
#include "KvsObject_popupMenu.h"
#include "KvsObject_process.h"
#include "KvsObject_progressBar.h"
#include "KvsObject_radioButton.h"
#include "KvsObject_slider.h"
#include "KvsObject_socket.h"
#include "KvsObject_spinBox.h"
#include "KvsObject_tabWidget.h"
#include "KvsObject_tableWidget.h"
#include "KvsObject_textBrowser.h"
#include "KvsObject_toolBar.h"
#include "KvsObject_toolButton.h"
#include "KvsObject_trayIcon.h"
#include "KvsObject_treeWidget.h"
#include "KvsObject_treeWidgeteItem.h"
#include "KvsObject_vBox.h"
#if defined(COMPILE_WEBKIT_SUPPORT)
#include "KvsObject_webView.h"
#endif
#include "KvsObject_widget.h"
#include "KvsObject_window.h"
#include "KvsObject_wizard.h"
#include "KvsObject_workspace.h"
#include "KvsObject_wrapper.h"
#include "KvsObject_xmlreader.h"
#include "KvsObject_sql.h"
#include "KvsObject_dateTimeEdit.h"

#include "KviIconManager.h"
#include "KviWindow.h"
#include "KviControlCodes.h"
#include "KviApplication.h"
#include "KviModule.h"
#include "KviLocale.h"
#include "KviError.h"
#include "KviCommandFormatter.h"
#include "kvi_out.h"
#include "KviFileUtils.h"

#include <QDir>
#include <QImage>

static void dumpChildObjects(KviWindow * pWnd, QObject * parent, const char * spacing, bool bWidgetsOnly, KviKvsArray * n, int & idx);

static bool objects_module_cleanup(KviModule *)
{
// Don't attempt to change the order of these calls.
// Derived classes must be unregistered before the base ones.
#if defined(COMPILE_WEBKIT_SUPPORT)
	KvsObject_webView::unregisterSelf();
#endif
	KvsObject_memoryBuffer::unregisterSelf();
	KvsObject_process::unregisterSelf();
	KvsObject_trayIcon::unregisterSelf();
	KvsObject_ftp::unregisterSelf();
	KvsObject_http::unregisterSelf();
	KvsObject_socket::unregisterSelf();
	KvsObject_xmlReader::unregisterSelf();
	KvsObject_wrapper::unregisterSelf();
	KvsObject_file::unregisterSelf();
	KvsObject_progressBar::unregisterSelf();
	KvsObject_dockWindow::unregisterSelf();
	KvsObject_wizard::unregisterSelf();
	KvsObject_window::unregisterSelf();
	KvsObject_toolButton::unregisterSelf();
	KvsObject_toolBar::unregisterSelf();
	KvsObject_tabWidget::unregisterSelf();
	KvsObject_spinBox::unregisterSelf();
	KvsObject_slider::unregisterSelf();
	KvsObject_radioButton::unregisterSelf();
	KvsObject_popupMenu::unregisterSelf();
	KvsObject_pixmap::unregisterSelf();
	KvsObject_painter::unregisterSelf();
	KvsObject_mainWindow::unregisterSelf();
	KvsObject_treeWidgetItem::unregisterSelf();
	KvsObject_treeWidget::unregisterSelf();
	KvsObject_listWidget::unregisterSelf();
	KvsObject_comboBox::unregisterSelf();
	KvsObject_workspace::unregisterSelf();
	KvsObject_menuBar::unregisterSelf();
	KvsObject_lineEdit::unregisterSelf();
	KvsObject_lcd::unregisterSelf();
	KvsObject_layout::unregisterSelf();
	KvsObject_hBox::unregisterSelf();
	KvsObject_vBox::unregisterSelf();
	KvsObject_label::unregisterSelf();
	KvsObject_groupBox::unregisterSelf();
	KvsObject_colorDialog::unregisterSelf();
	KvsObject_dialog::unregisterSelf();
	KvsObject_checkBox::unregisterSelf();
	KvsObject_textBrowser::unregisterSelf();
	KvsObject_textedit::unregisterSelf();
	KvsObject_button::unregisterSelf();
	KvsObject_dateTimeEdit::unregisterSelf();
	KvsObject_tableWidget::unregisterSelf();
	KvsObject_widget::unregisterSelf();
	KvsObject_list::unregisterSelf();
	KvsObject_sql::unregisterSelf();
	KvsObject_buttonGroup::unregisterSelf();
	return true;
}

static bool objects_kvs_cmd_killClass(KviKvsModuleCommandCall * c)
{
	/*
		@doc: objects.killclass
		@title:
			objects.killclass
		@type:
			command
		@short:
			Removes a class definition
		@syntax:
			objects.killclass [-q] <classname:string>
		@switches:
			!sw: -q | --quiet
			Suppresses any warning message
		@description:
			Removes the definition of the class <class_name>.[br]
			All the instances of the class are killed (thus child objects are killed too).[br]
			All the derived class definitions are removed as well.[br]
			Builtin class definitions cannot be removed and this commands
			kills only all the instances of that class (derived class definitions
			and instances in this case are [b]not[/b] killed).[br]
		@seealso:
			[cmd]class[/cmd], [cmd]objects.clearObjects[/cmd], [fnc]$classDefined[/fnc](),
			[doc:objects]Object scripting[/doc]
	*/

	QString szClass;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("class", KVS_PT_NONEMPTYSTRING, 0, szClass)
	KVSM_PARAMETERS_END(c)
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClass);
	if(pClass)
	{
		if(pClass->isBuiltin())
		{
			KviKvsKernel::instance()->objectController()->killAllObjectsWithClass(pClass);
		}
		else
		{
			KviKvsKernel::instance()->objectController()->deleteClass(pClass); //Yahoo!!
		}
	}
	else if(!c->hasSwitch('q', "quiet"))
		c->warning(__tr2qs_ctx("Class '%Q' is not defined", "objects"), &szClass);
	return true;
}

static bool objects_kvs_cmd_clearObjects(KviKvsModuleCommandCall * c)
{
	/*
		@doc: objects.clearObjects
		@title:
			objects.clearObjects
		@type:
			command
		@short:
			Removes all the user class definitions
		@syntax:
			objects.clearObjects [-i]
		@description:
			Removes the definition of all the user classes and kill all the object
			instances (also instances of the builtin classes).[br]
			If the -i switch is used, only the object instances are cleared
			but the class definitions are left unchanged.[br]
			This command should be used only for debugging & testing purposes
			and in particular should be [b]never[/b] used from inside an object
			function handler (leads to SIGSEGV :).[br]
		@seealso:
			[cmd]class[/cmd], [cmd]objects.killclass[/cmd],
			[doc:objects]Object scripting[/doc]
	*/

	KviKvsKernel::instance()->objectController()->clearInstances();
	if(!c->switches()->find('i', "ii"))
		KviKvsKernel::instance()->objectController()->clearUserClasses();
	return true;
}

static bool objects_kvs_cmd_connect(KviKvsModuleCommandCall * c)
{
	/*
		@doc: objects.connect
		@title:
			objects.connect
		@type:
			command
		@short:
			Connects a signal to a slot
		@syntax:
			objects.connect <source_object:hobject> <signal_name:string> <target_object:hobject> <slot_name:string>
		@description:
			Connects the <source_object>'s signal <signal_name> to the
			<target_object>'s slot <slot_name>.
			When one of the two objects is destroyed, the signal/slot
			connection is automatically removed.[br]
			WARNING: this command name collides with the [doc:RFC2812]RFC2812[/doc]
			CONNECT IRC Op command: this IRC command is available to operators only
			and is rather rarely used: you can use it by the means of [doc:raw]raw[/doc].
		@seealso:
			[cmd]class[/cmd], [cmd]object.disconnect[/cmd], [doc:objects]Object scripting[/doc]
	*/

	KviKvsObject * obSrc;
	KviKvsObject * obTrg;
	QString szSignal, szSlot;
	kvs_hobject_t hSrc, hTrg;
	KVSM_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("object source", KVS_PT_HOBJECT, 0, hSrc)
	KVSM_PARAMETER("signal name", KVS_PT_NONEMPTYSTRING, 0, szSignal)
	KVSO_PARAMETER("object target", KVS_PT_HOBJECT, 0, hTrg)
	KVSM_PARAMETER("slot", KVS_PT_NONEMPTYSTRING, 0, szSlot)
	KVSM_PARAMETERS_END(c)
	obTrg = KviKvsKernel::instance()->objectController()->lookupObject(hTrg);
	obSrc = KviKvsKernel::instance()->objectController()->lookupObject(hSrc);
	if(!obTrg)
	{
		c->warning(__tr2qs_ctx("non-existent target object for objects.connect", "objects"));
		return true;
	}
	if(!obSrc)
	{
		c->warning(__tr2qs_ctx("non-existent source object for objects.connect", "objects"));
		return true;
	}
	if(!obSrc->connectSignal(szSignal, obTrg, szSlot))
	{
		c->warning(__tr2qs_ctx("non-existent target slot '%Q' for objects.connect", "objects"), &szSlot);
		return true;
	}
	return true;
}

static bool objects_kvs_fnc_exists(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.exists
		@title:
			$objects.exists
		@type:
			function
		@short:
			Check if an object exists.
		@syntax:
			<boolean> $objects.exists(<object id:hobject>)
		@description:
			Check if an objects exists and return a boolean value.
		@seealso:
			[doc:objects]Object scripting[/doc]
	*/

	kvs_hobject_t hObj;
	KVSM_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("object", KVS_PT_HOBJECT, 0, hObj)
	KVSM_PARAMETERS_END(c)
	KviKvsObject * ob = KviKvsKernel::instance()->objectController()->lookupObject(hObj);
	c->returnValue()->setBoolean(ob ? true : false);
	return true;
}

static bool objects_kvs_fnc_instances(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.instances
		@title:
			$objects.instances
		@type:
			function
		@short:
			List object instances
		@syntax:
			<array> $objects.instances(<class name:string>[,<flags:string>])
		@description:
			Returns an array with the instance handles of
			of objects of the specified class. <flags> is a
			string containing modifiers for the function behaviour.[br]
			If <flags> contains the letter "s" then
			only non-subclassed instances of the specified
			class are returned. This means that if B is
			a subclass of A then when asked for instances
			of A without the flag "s" this function will return
			both instances of A and B, and with the flag "s"
			this function will return only instances of A (and not B).
			If the class name does not define a valid class
			then an empty array is returned and a warning is
			printed unless the "q" flag is used.
		@examples:
			[example]
			foreach(%x,$objects.instances(object))
			{
				echo "OBJECT WITH CLASS" %x->$className() "AND NAME \""%x->$name()"\""
			}
			[/example]
		@seealso:
			[doc:objects]Object scripting[/doc]
	*/

	QString szClassName;
	QString szFlags;
	KVSM_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("class name", KVS_PT_NONEMPTYSTRING, 0, szClassName)
	KVSO_PARAMETER("flags", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlags)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * pArry = new KviKvsArray();
	c->returnValue()->setArray(pArry);

	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
	if(!pClass)
	{
		if(!szFlags.contains(QChar('q')))
			c->warning(__tr2qs_ctx("The class '%Q' doesn't exist", "objects"), &szClassName);
		return true;
	}
	KviPointerHashTable<void *, KviKvsObject> * od = KviKvsKernel::instance()->objectController()->objectDict();

	KviPointerHashTableIterator<void *, KviKvsObject> it(*od);
	kvs_uint_t uIdx = 0;
	if(szFlags.contains(QChar('s')))
	{
		while(KviKvsObject * ob = it.current())
		{
			if(ob->getExactClass() == pClass)
			{
				pArry->set(uIdx, new KviKvsVariant(ob->handle()));
				uIdx++;
			}
			++it;
		}
	}
	else
	{
		while(KviKvsObject * ob = it.current())
		{
			if(ob->inheritsClass(pClass))
			{
				pArry->set(uIdx, new KviKvsVariant(ob->handle()));
				uIdx++;
			}
			++it;
		}
	}
	return true;
}

static bool objects_kvs_fnc_variables(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.variables
		@title:
			$objects.variables
		@type:
			function
		@short:
			List object's variables.
		@syntax:
			<hash> $objects.variables(<hobject>)
		@description:
			Returns an hash with the object's variables(useful only for debugging).
		@seealso:
			[doc:objects]Object scripting[/doc]
	*/

	kvs_hobject_t hObj;
	KVSM_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("object", KVS_PT_HOBJECT, 0, hObj)
	KVSM_PARAMETERS_END(c)
	KviKvsObject * ob = KviKvsKernel::instance()->objectController()->lookupObject(hObj);
	if(!ob)
	{
		c->warning(__tr2qs_ctx("Object doesn't exist", "objects"));
		return true;
	}
	KviPointerHashTableIterator<QString, KviKvsVariant> it(*ob->dataContainer()->dict());
	KviKvsHash * pHash = new KviKvsHash();
	c->returnValue()->setHash(pHash);
	while(KviKvsVariant * t = it.current())
	{
		pHash->set(it.currentKey(), new KviKvsVariant(*t));
		++it;
	}
	return true;
}

static bool objects_kvs_fnc_classAllHandlers(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.classAllHandlers
		@title:
			$objects.classAllHandlers
		@type:
			function
		@short:
			List all the functions .
		@syntax:
			<hash> $objects.classAllHandlers(<class name:string>)
		@description:
			Returns an hash with the class's functions(useful only for debugging).
		@seealso:
			[doc:objects]Object scripting[/doc]
	*/

	QString szClassName;
	KVSM_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("class name", KVS_PT_NONEMPTYSTRING, 0, szClassName)
	KVSM_PARAMETERS_END(c)
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
	if(!pClass)
	{
		c->warning(__tr2qs_ctx("The class '%Q' doesn't exist", "objects"), &szClassName);
		return true;
	}

	KviPointerHashTableIterator<QString, KviKvsObjectFunctionHandler> it(*pClass->getHandlers());
	KviKvsHash * pHash = new KviKvsHash();
	c->returnValue()->setHash(pHash);
	while(it.current())
	{
		QString szCode;
		KviKvsObjectFunctionHandler * handler = pClass->lookupFunctionHandler(it.currentKey());
		pClass->getFunctionCode(szCode, *handler);
		pHash->set(it.currentKey(), new KviKvsVariant(szCode));
		++it;
	}

	return true;
}

static bool objects_kvs_fnc_classes(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.classes
		@title:
			$objects.classes
		@type:
			function
		@short:
			List user defined classes
		@syntax:
			<array> $objects.classes()
		@description:
			Returns an array with the user defined classes.
		@seealso:
			[doc:objects]Object scripting[/doc]
	*/

	KviKvsArray * pArry = new KviKvsArray();
	c->returnValue()->setArray(pArry);

	KviPointerHashTableIterator<QString, KviKvsObjectClass> it(*KviKvsKernel::instance()->objectController()->classDict());
	KviPointerHashTable<QString, bool> * classdict = new KviPointerHashTable<QString, bool>;
	classdict->setAutoDelete(false);
	bool bFake = true;
	while(KviKvsObjectClass * pClass = it.current())
	{
		if(!pClass->isBuiltin())
			classdict->insert(it.currentKey(), &bFake);
		++it;
	}
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath, KviApplication::Classes);
	QDir d(szPath);
	QStringList sl;
	sl = d.entryList(QDir::Files);
	for(auto szName : sl)
	{
		szName.replace("--", "::");
		if(szName != ".." && szName != ".")
			classdict->insert(szName.left(szName.length() - 4), &bFake);
	}
	KviKvsArray * pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);
	int idx = 0;
	KviPointerHashTableIterator<QString, bool> strIt(*classdict);
	while(strIt.current())
	{
		pArray->set(idx, new KviKvsVariant(strIt.currentKey()));
		idx++;
		++strIt;
	}
	delete classdict;
	return true;
}

static bool objects_kvs_fnc_name(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.name
		@title:
			$objects.name
		@type:
			function
		@short:
			Returns class name
		@syntax:
			<array> $objects.name()
		@description:
			Returns a string with the name of the class.
		@seealso:
			[doc:objects]Object scripting[/doc]
	*/

	KviKvsObject * obSrcClass;
	kvs_hobject_t hClass;
	KVSM_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("object class", KVS_PT_HOBJECT, 0, hClass)
	KVSM_PARAMETERS_END(c)

	obSrcClass = KviKvsKernel::instance()->objectController()->lookupObject(hClass);
	if(!obSrcClass)
	{
		c->warning(__tr2qs_ctx("non-existent class object for objects.name", "objects"));
		return true;
	}

	c->returnValue()->setString(obSrcClass->getName());
	return true;
}

static bool objects_kvs_cmd_disconnect(KviKvsModuleCommandCall * c)
{
	/*
		@doc: objects.disconnect
		@title:
			objects.disconnect
		@type:
			command
		@short:
			Disconnects a signal from a slot
		@syntax:
			disconnect <source_object:hobject> <signal_name:string> <target_object:hobject> <slot_name:string>
		@description:
			Disconnects the <source_object>'s signal <signal_name> from the
			<target_object>'s slot <slot_name>.
			When one of the two objects is destroyed, the signal/slot
			connection is automatically removed.
		@seealso:
			[cmd]class[/cmd], [cmd]objects.connect[/cmd], [doc:objects]Object scripting[/doc]
	*/

	KviKvsObject * obSrc;
	KviKvsObject * obTrg;
	QString szSignal, szSlot;
	kvs_hobject_t hSrc, hTrg;
	KVSM_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("object source", KVS_PT_HOBJECT, 0, hSrc)
	KVSM_PARAMETER("signal name", KVS_PT_NONEMPTYSTRING, 0, szSignal)
	KVSO_PARAMETER("object target", KVS_PT_HOBJECT, 0, hTrg)
	KVSM_PARAMETER("slot", KVS_PT_NONEMPTYSTRING, 0, szSlot)
	KVSM_PARAMETERS_END(c)
	obTrg = KviKvsKernel::instance()->objectController()->lookupObject(hTrg);
	obSrc = KviKvsKernel::instance()->objectController()->lookupObject(hSrc);
	if(!obTrg)
	{
		c->warning(__tr2qs_ctx("non-existent target object for objects.disconnect", "objects"));
		return true;
	}
	if(!obSrc)
	{
		c->warning(__tr2qs_ctx("non-existent source object for objects.disconnect", "objects"));
		return true;
	}
	obSrc->disconnectSignal(szSignal, obTrg, szSlot);
	return true;
}

static bool objects_kvs_fnc_listObjects(KviKvsModuleFunctionCall * cmd)
{
	/*
		@doc: objects.dump
		@type:
			function
		@title:
			$objects.dump
		@keyterms:
			dump objects
		@short:
			Dump the KVIrc widgets.
		@syntax:
			<array> $objects.dump(<print:boolean>)
		@description:
			This function return an array containing a representation of the widgets tree in KVIrc;[br]
			If the <print> flag is enabled, the array will be printed in the current window, too.
		@examples:
			[example]
				%A[]=$objects.dump(0)
				%i=0
				while (%i != %A[]#)
				{
					echo %A[%i]
					%i++
				}
			[/example]
	*/

	bool bFlag;
	KVSO_PARAMETERS_BEGIN(cmd)
	KVSO_PARAMETER("print on current window", KVS_PT_BOOL, 0, bFlag)
	KVSO_PARAMETERS_END(cmd)
	if(bFlag)
		cmd->window()->output(80, "Objects dump:");
	QWidgetList list = g_pApp->topLevelWidgets();

	KviCString spacing = ">";
	QString szTemp;
	KviKvsArray * n = new KviKvsArray();
	int idx = 0;
	for(int i = 0; i < list.count(); i++)
	{
		if(list.at(i)->isWidgetType())
		{
			if(bFlag)
			{
				cmd->window()->output(80, "Ptr %u: top level object: %c%s%c, class %s, %s, rect = %d, %d, %d, %d",
				    list.at(i),
				    KviControlCodes::Bold, list.at(i)->objectName().toUtf8().data(), KviControlCodes::Bold,
				    list.at(i)->metaObject()->className(),
				    list.at(i)->isVisible() ? "visible" : "hidden",
				    list.at(i)->x(), list.at(i)->y(), list.at(i)->width(), list.at(i)->height());
			}
			QString szClass = list.at(i)->metaObject()->className();
			QString szObj = list.at(i)->objectName();
			QString szTemp;
			szTemp = szClass + "::" + szObj;
			KviKvsVariant v;
			v.setString(szTemp);
			n->set(idx, new KviKvsVariant(v));
			/*qDebug("string %s",szTemp.toUtf8().data());
			qDebug("class %s",szClass.toUtf8().data());
                        qDebug("Obj %s",szObj.toUtf8().data());*/
			idx++;

			dumpChildObjects(cmd->window(), list.at(i), spacing.ptr(), bFlag, n, idx);
		}
	}

	cmd->returnValue()->setArray(n);
	return true;
}

static void dumpChildObjects(KviWindow * pWnd, QObject * parent, const char * spacing, bool bFlag, KviKvsArray * n, int & idx)
{
	const QObjectList list = parent->children();
	if(!list.count())
		return;
	QString sp(spacing);
	sp.append(">");
	for(int i = 0; i < list.count(); i++)
	{
		if(list.at(i)->isWidgetType())
		{
			if(bFlag)
			{
				pWnd->output(80, "%sPtr %u: object: %c%s%c, class %s",
				    spacing, list.at(i), KviControlCodes::Bold,
				    list.at(i)->objectName().toUtf8().data(), KviControlCodes::Bold, list.at(i)->metaObject()->className());
			}
			QString szClass = list.at(i)->metaObject()->className();
			QString szObj = list.at(i)->objectName();
			QString szTemp;
			szTemp = spacing + szClass + "::" + szObj;
			KviKvsVariant v;
			v.setString(szTemp);
			n->set(idx, new KviKvsVariant(v));
			//qDebug("string %s",szTemp.toUtf8().data());
			idx++;
			dumpChildObjects(pWnd, list.at(i), sp.toUtf8().data(), bFlag, n, idx);
		}
	}
}

static bool objects_module_can_unload(KviModule *)
{
	// Actually this object remains locked in memory
	// FIXME: unlock when no more instances & derived classes!
	return false;
}

static bool objects_module_init(KviModule * m)
{
	// functions
	KVSM_REGISTER_FUNCTION(m, "classAllHandlers", objects_kvs_fnc_classAllHandlers);
	KVSM_REGISTER_FUNCTION(m, "classes", objects_kvs_fnc_classes);
	KVSM_REGISTER_FUNCTION(m, "dump", objects_kvs_fnc_listObjects);
	KVSM_REGISTER_FUNCTION(m, "exists", objects_kvs_fnc_exists);
	KVSM_REGISTER_FUNCTION(m, "instances", objects_kvs_fnc_instances);
	KVSM_REGISTER_FUNCTION(m, "name", objects_kvs_fnc_name);
	KVSM_REGISTER_FUNCTION(m, "variables", objects_kvs_fnc_variables);

	// commands
	KVSM_REGISTER_SIMPLE_COMMAND(m, "clearObjects", objects_kvs_cmd_clearObjects);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "connect", objects_kvs_cmd_connect);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "disconnect", objects_kvs_cmd_disconnect);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "killClass", objects_kvs_cmd_killClass);

	// Don't attempt to change the order of these calls.
	// Derived classes must be registered after the base ones.

	KvsObject_buttonGroup::registerSelf();
	KvsObject_sql::registerSelf();
	KvsObject_list::registerSelf();
	KvsObject_widget::registerSelf();
	KvsObject_tableWidget::registerSelf();
	KvsObject_dateTimeEdit::registerSelf();
	KvsObject_button::registerSelf();
	KvsObject_textedit::registerSelf();
	KvsObject_textBrowser::registerSelf();
	KvsObject_checkBox::registerSelf();
	KvsObject_dialog::registerSelf();
	KvsObject_colorDialog::registerSelf();
	KvsObject_groupBox::registerSelf();
	KvsObject_label::registerSelf();
	KvsObject_vBox::registerSelf();
	KvsObject_hBox::registerSelf();
	KvsObject_layout::registerSelf();
	KvsObject_lcd::registerSelf();
	KvsObject_lineEdit::registerSelf();
	KvsObject_menuBar::registerSelf();
	KvsObject_workspace::registerSelf();
	KvsObject_comboBox::registerSelf();
	KvsObject_listWidget::registerSelf();
	KvsObject_treeWidget::registerSelf();
	KvsObject_treeWidgetItem::registerSelf();
	KvsObject_mainWindow::registerSelf();
	KvsObject_painter::registerSelf();
	KvsObject_pixmap::registerSelf();
	KvsObject_popupMenu::registerSelf();
	KvsObject_radioButton::registerSelf();
	KvsObject_slider::registerSelf();
	KvsObject_spinBox::registerSelf();
	KvsObject_tabWidget::registerSelf();
	KvsObject_toolBar::registerSelf();
	KvsObject_toolButton::registerSelf();
	KvsObject_window::registerSelf();
	KvsObject_wizard::registerSelf();
	KvsObject_dockWindow::registerSelf();
	KvsObject_progressBar::registerSelf();
	KvsObject_file::registerSelf();
	KvsObject_wrapper::registerSelf();
	KvsObject_xmlReader::registerSelf();
	KvsObject_socket::registerSelf();
	KvsObject_http::registerSelf();
	KvsObject_ftp::registerSelf();
	KvsObject_trayIcon::registerSelf();
	KvsObject_process::registerSelf();
	KvsObject_memoryBuffer::registerSelf();
#if defined(COMPILE_WEBKIT_SUPPORT)
	KvsObject_webView::registerSelf();
#endif
	return true;
}

KVIRC_MODULE(
    "Objects",
    "4.0.0",
    "Copyright (C) 2000-2006:\n"
    "  Szymon Stefanek (pragma at kvirc dot net)\n"
    "  Krzysztof Godlewski (kristoff at poczta dot wprost dot pl)\n"
    "  Tonino Imbesi (grifisx at barmes dot org)\n"
    "  Alessandro Carbone (elfonol at gmail dot com)",
    "Object classes for KVIrc scripting language\n",
    objects_module_init,
    objects_module_can_unload,
    0,
    objects_module_cleanup,
    "objects")
