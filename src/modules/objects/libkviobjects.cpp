//=============================================================================
//
//   File : libkviobjects.cpp
//   Creation date : Wed Sep 09 2000 20:59:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2009 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "class_button.h"
#include "class_checkbox.h"
#include "class_combobox.h"
#include "class_dialog.h"
#include "class_dockwindow.h"
#include "class_file.h"
#include "class_ftp.h"
#include "class_groupbox.h"
#include "class_buttongroup.h"
#include "class_hbox.h"
#include "class_http.h"
#include "class_label.h"
#include "class_layout.h"
#include "class_lcd.h"
#include "class_lineedit.h"
#include "class_list.h"
#include "class_listwidget.h"
#include "class_mainwindow.h"
#include "class_memorybuffer.h"
#include "class_menubar.h"
#include "class_multilineedit.h"
#include "class_painter.h"
#include "class_pixmap.h"
#include "class_popupmenu.h"
#include "class_process.h"
#include "class_progressbar.h"
#include "class_radiobutton.h"
#include "class_slider.h"
#include "class_socket.h"
#include "class_spinbox.h"
#include "class_tabwidget.h"
#include "class_tablewidget.h"
#include "class_tbrowser.h"
#include "class_toolbar.h"
#include "class_toolbutton.h"
#include "class_treewidget.h"
#include "class_treewidgetitem.h"
#include "class_vbox.h"
#include "class_widget.h"
#include "class_window.h"
#include "class_wizard.h"
#include "class_workspace.h"
#include "class_wrapper.h"
#include "class_xmlreader.h"
#include "class_sql.h"
#include "class_datetimeedit.h"


#include "kvi_iconmanager.h"
#include "kvi_window.h"
#include "kvi_out.h"
#include "kvi_mirccntrl.h"
#include "kvi_app.h"
#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_cmdformatter.h"
#include "kvi_out.h"
#include "kvi_app.h"
#include "kvi_fileutils.h"

#include <QDir>
#include <QImage>


static void dumpChildObjects(KviWindow *pWnd, QObject *parent, const char *spacing, bool bWidgetsOnly, KviKvsArray *n, int &idx);

static bool objects_module_cleanup(KviModule *)
{
	// Don't attempt to change the order of these calls.
	// Derived classes must be unregistered before the base ones.

        KviKvsObject_memorybuffer::unregisterSelf();
	KviKvsObject_process::unregisterSelf();
	KviKvsObject_ftp::unregisterSelf();
	KviKvsObject_http::unregisterSelf();
	KviKvsObject_socket::unregisterSelf();
	KviKvsObject_xmlreader::unregisterSelf();
	KviKvsObject_wrapper::unregisterSelf();
	KviKvsObject_file::unregisterSelf();
	KviKvsObject_progressbar::unregisterSelf();
	KviKvsObject_dockwindow::unregisterSelf();
	KviKvsObject_wizard::unregisterSelf();
	KviKvsObject_window::unregisterSelf();
	KviKvsObject_toolbutton::unregisterSelf();
	KviKvsObject_toolbar::unregisterSelf();
	KviKvsObject_tabwidget::unregisterSelf();
	KviKvsObject_spinbox::unregisterSelf();
	KviKvsObject_slider::unregisterSelf();
	KviKvsObject_radiobutton::unregisterSelf();
	KviKvsObject_popupmenu::unregisterSelf();
	KviKvsObject_pixmap::unregisterSelf();
	KviKvsObject_painter::unregisterSelf();
	KviKvsObject_mainwindow::unregisterSelf();
	KviKvsObject_treewidgetitem::unregisterSelf();
	KviKvsObject_treewidget::unregisterSelf();
	KviKvsObject_listwidget::unregisterSelf();
	KviKvsObject_combobox::unregisterSelf();
	KviKvsObject_workspace::unregisterSelf();
	KviKvsObject_menubar::unregisterSelf();
	KviKvsObject_lineedit::unregisterSelf();
	KviKvsObject_lcd::unregisterSelf();
	KviKvsObject_layout::unregisterSelf();
	KviKvsObject_hbox::unregisterSelf();
	KviKvsObject_vbox::unregisterSelf();
	KviKvsObject_label::unregisterSelf();
	KviKvsObject_groupbox::unregisterSelf();
	KviKvsObject_dialog::unregisterSelf();
	KviKvsObject_checkbox::unregisterSelf();
	KviKvsObject_textbrowser::unregisterSelf();
	KviKvsObject_textedit::unregisterSelf();
	KviKvsObject_button::unregisterSelf();
        KviKvsObject_datetimeedit::unregisterSelf();
        KviKvsObject_tablewidget::unregisterSelf();
        KviKvsObject_widget::unregisterSelf();
	KviKvsObject_list::unregisterSelf();
        KviKvsObject_sql::unregisterSelf();
	KviKvsObject_buttongroup::unregisterSelf();

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
			All the instances of the class are killed (thus children objects are killed too).[br]
			All the derived class definitions are removed as well.[br]
			Builtin class definitions cannot be removed and this commands
			kills only all the instances of that class (derived class definitions
			and instances in this case are NOT killed).[br]
		@seealso:
			[cmd]class[/cmd], [cmd]objects.clear[/cmd], [fnc]$classDefined[/fnc](),
			[doc:objects]Objects documentation[/doc]
	*/


	QString szClass;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("class",KVS_PT_NONEMPTYSTRING,0,szClass)
	KVSM_PARAMETERS_END(c)
	KviKvsObjectClass * pClass=KviKvsKernel::instance()->objectController()->lookupClass(szClass);
	if(pClass)
	{
		if(pClass->isBuiltin())
		{
			KviKvsKernel::instance()->objectController()->killAllObjectsWithClass(pClass);
		} else {
			KviKvsKernel::instance()->objectController()->deleteClass(pClass); //Yahoo!!
		}
	}
	else if(!c->hasSwitch('q',"quiet"))c->warning(__tr2qs_ctx("Class '%Q' is not defined","objects"),&szClass);
	return true;
}

static bool objects_kvs_cmd_clearObjects(KviKvsModuleCommandCall * c)
{
	/*
		@doc: objects.clear
		@title:
			objects.clear
		@type:
			command
		@short:
			Removes all the user class definitions
		@syntax:
			objects.clear [-i]
		@description:
			Removes the definition of all the user classes and kill all the object
			instances (also instances of the builtin classes).[br]
			If the -i switch is used, only the object instances are cleared
			but the class definitions are left unchanged.[br]
			This command should be used only for debugging & testing purposes
			and in particular should be NEVER used from inside an object
			function handler (leads to SIGSEGV :).[br]
		@seealso:
			[cmd]class[/cmd], [cmd]objects.killclass[/cmd],
			[doc:objects]Objects documentation[/doc]
	*/

	KviKvsKernel::instance()->objectController()->clearInstances();
	if(!c->switches()->find('i',"ii")) KviKvsKernel::instance()->objectController()->clearUserClasses();
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
			WARNING: This command name collides with the [doc:rfc2812]RFC2812[/doc]
			CONNECT IRC Op command: this IRC command is available to operators only
			and is rather rarely used: you can use it by the means of [doc:raw]raw[/doc].
		@seealso:
			[cmd]class[/cmd], [cmd]object.disconnect[/cmd], [doc:objects]objects documentation[/doc]
	*/


	KviKvsObject *obSrc;
	KviKvsObject *obTrg;
	QString szSignal,szSlot;
	kvs_hobject_t hSrc,hTrg;
	KVSM_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("object source",KVS_PT_HOBJECT,0,hSrc)
		KVSM_PARAMETER("signal name",KVS_PT_NONEMPTYSTRING,0,szSignal)
		KVSO_PARAMETER("object target",KVS_PT_HOBJECT,0,hTrg)
		KVSM_PARAMETER("slot",KVS_PT_NONEMPTYSTRING,0,szSlot)
	KVSM_PARAMETERS_END(c)
	obTrg=KviKvsKernel::instance()->objectController()->lookupObject(hTrg);
	obSrc=KviKvsKernel::instance()->objectController()->lookupObject(hSrc);
	if(!obTrg)
	{
		c->warning(__tr2qs_ctx("Inexisting target object for objects.connect","objects"));
		return true;
	}
	if(!obSrc)
	{
		c->warning(__tr2qs_ctx("Inexisting source object for objects.connect","objects"));
		return true;
	}
	if(!obSrc->connectSignal(szSignal,obTrg,szSlot))
	{
		c->warning(__tr2qs_ctx("Inexisting target slot '%Q' for objects.connect","objects"),&szSlot);
		return true;
	}
	return true;
}

static bool objects_kvs_fnc_exists(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.exists
		@title:
			objects.exists
		@type:
			command
		@short:
			Check if an object exists.
		@syntax:
			<boolean> $object.exists(<obejct id:hobject>)
		@description:
			Check if an objects exists and return a boolean value.
		@seealso:
			[doc:objects]objects documentation[/doc]
	*/


	kvs_hobject_t hObj;
	KVSM_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("object",KVS_PT_HOBJECT,0,hObj)
	KVSM_PARAMETERS_END(c)
	KviKvsObject *ob=KviKvsKernel::instance()->objectController()->lookupObject(hObj);
	c->returnValue()->setBoolean(ob ? true : false);
	return true;
}

static bool objects_kvs_fnc_instances(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.instances
		@title:
			objects.instances
		@type:
			command
		@short:
			List object instances
		@syntax:
			<array> $object.instances(<class name:string>[,<flags:string>])
		@description:
			Returns an array with the instance handles of
			of objects of the specified class. <flags> is a
			string containing modifiers for the function behaviour.[br]
			If <flags> contains the letter "s" then
			only unsubclassed instances of the specified
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
			[doc:objects]objects documentation[/doc]
	*/

	QString szClassName;
	QString szFlags;
	KVSM_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("class name",KVS_PT_NONEMPTYSTRING,0,szClassName)
		KVSO_PARAMETER("flags",KVS_PT_STRING,KVS_PF_OPTIONAL,szFlags)
	KVSM_PARAMETERS_END(c)

	KviKvsArray * pArry = new KviKvsArray();
	c->returnValue()->setArray(pArry);

	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
	if(!pClass)
	{
		if(!szFlags.contains(QChar('q')))
			c->warning(__tr2qs_ctx("The class '%Q' does not exist","objects"),&szClassName);
		return true;
	}
	KviPointerHashTable<void *,KviKvsObject> * od = KviKvsKernel::instance()->objectController()->objectDict();

	KviPointerHashTableIterator<void *,KviKvsObject> it(*od);
	kvs_uint_t uIdx = 0;
	if(szFlags.contains(QChar('s')))
	{
		while(KviKvsObject * ob = it.current())
		{
			if(ob->getExactClass() == pClass)
			{
				pArry->set(uIdx,new KviKvsVariant(ob->handle()));
				uIdx++;
			}
			++it;
		}
	} else {
		while(KviKvsObject * ob = it.current())
		{
			if(ob->inheritsClass(pClass))
			{
				pArry->set(uIdx,new KviKvsVariant(ob->handle()));
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
			objects.variables
		@type:
			command
		@short:
			List object's variables.
		@syntax:
			<hash> objects.variables(<hobject>)
		@description:
			Returns an hash with the object's variables(useful only for debugging).
		@seealso:
			[doc:objects]objects documentation[/doc]
	*/

	kvs_hobject_t hObj;
	KVSM_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("object",KVS_PT_HOBJECT,0,hObj)
	KVSM_PARAMETERS_END(c)
	KviKvsObject *ob=KviKvsKernel::instance()->objectController()->lookupObject(hObj);
	if (!ob)
	{
		c->warning(__tr2qs_ctx("Object does not exists","objects"));
		return true;
	}
	KviPointerHashTableIterator<QString,KviKvsVariant> it(* ob->dataContainer()->dict());
	KviKvsHash* pHash = new KviKvsHash();
	c->returnValue()->setHash(pHash);
	while(KviKvsVariant * t = it.current())
	{
		pHash->set(it.currentKey(),new KviKvsVariant(*t));
		++it;
	}
	return true;



}

static bool objects_kvs_fnc_classAllHandlers(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.classAllHandlers
		@title:
			objects.classAllHandlers
		@type:
			function
		@short:
			List all the functions .
		@syntax:
			<hash> objects.classAllHandlers(<class name:string>)
		@description:
			Returns an hash with the class's functions(useful only for debugging).
		@seealso:
			[doc:objects]objects documentation[/doc]
	*/

	QString szClassName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("class name",KVS_PT_NONEMPTYSTRING,0,szClassName)
	KVSM_PARAMETERS_END(c)
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
	if(!pClass)
	{
		c->warning(__tr2qs_ctx("The class '%Q' does not exist","objects"),&szClassName);
		return true;
	}

	KviPointerHashTableIterator<QString,KviKvsObjectFunctionHandler>  it(* pClass->getHandlers());
	KviKvsHash* pHash = new KviKvsHash();
	c->returnValue()->setHash(pHash);
	while(it.current())
	{
		QString szCode;
		KviKvsObjectFunctionHandler *handler=pClass->lookupFunctionHandler(it.currentKey());
		pClass->getFunctionCode(szCode,*handler);
		pHash->set(it.currentKey(),new KviKvsVariant(szCode));
		++it;
	}

	return true;
}

static bool objects_kvs_fnc_classes(KviKvsModuleFunctionCall * c)
{
	/*
		@doc: objects.classes
		@title:
			objects.classes
		@type:
			command
		@short:
			List user defined classes
		@syntax:
			<array> $object.classes()
		@description:
			Returns an array with the user defined classes.
		@seealso:
			[doc:objects]objects documentation[/doc]
	*/

	KviKvsArray * pArry = new KviKvsArray();
	c->returnValue()->setArray(pArry);

	KviPointerHashTableIterator<QString,KviKvsObjectClass> it(*KviKvsKernel::instance()->objectController()->classDict());
	KviPointerHashTable<QString,bool> *classdict=new KviPointerHashTable<QString,bool>;
	classdict->setAutoDelete(false);
	bool bFake=true;
	while(KviKvsObjectClass * pClass=it.current())
	{
		if (!pClass->isBuiltin())classdict->insert(it.currentKey(),&bFake);
		++it;
	}
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath,KviApp::Classes);
	QDir d(szPath);
	QStringList sl;
	sl = d.entryList(QDir::Files);
	for(QStringList::Iterator it2 = sl.begin();it2 != sl.end();++it2)
	{
		QString szName=*it2;
		szName.replace("--","::");
		if (szName!=".." && szName!=".") classdict->insert(szName.left(szName.length()-4),&bFake);
	}
	KviKvsArray* pArray = new KviKvsArray();
	c->returnValue()->setArray(pArray);
	int idx=0;
	KviPointerHashTableIterator<QString,bool>  strIt(*classdict);
	while(strIt.current())
	{
		pArray->set(idx,new KviKvsVariant(strIt.currentKey()));
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
			objects.name
		@type:
			command
		@short:
			Returns class name
		@syntax:
			<array> $object.name()
		@description:
			Returns a string with the name of the class.
		@seealso:
			[doc:objects]objects documentation[/doc]
	*/

	KviKvsObject * obSrcClass;
	kvs_hobject_t hClass;
	KVSM_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("object class",KVS_PT_HOBJECT,0,hClass)
	KVSM_PARAMETERS_END(c)

	obSrcClass = KviKvsKernel::instance()->objectController()->lookupObject(hClass);
	if(!obSrcClass)
	{
		c->warning(__tr2qs_ctx("Inexisting class object for objects.name","objects"));
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
			[cmd]class[/cmd], [cmd]objects.connect[/cmd], [doc:objects]objects documentation[/doc]
	*/

	KviKvsObject *obSrc;
	KviKvsObject *obTrg;
	QString szSignal,szSlot;
	kvs_hobject_t hSrc,hTrg;
	KVSM_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("object source",KVS_PT_HOBJECT,0,hSrc)
		KVSM_PARAMETER("signal name",KVS_PT_NONEMPTYSTRING,0,szSignal)
		KVSO_PARAMETER("object target",KVS_PT_HOBJECT,0,hTrg)
		KVSM_PARAMETER("slot",KVS_PT_NONEMPTYSTRING,0,szSlot)
	KVSM_PARAMETERS_END(c)
	obTrg=KviKvsKernel::instance()->objectController()->lookupObject(hTrg);
	obSrc=KviKvsKernel::instance()->objectController()->lookupObject(hSrc);
	if(!obTrg)
	{
		c->warning(__tr2qs_ctx("Inexisting target object for objects.disconnect","objects"));
		return true;
	}
	if(!obSrc)
	{
		c->warning(__tr2qs_ctx("Inexisting source object for objects.disconnect","objects"));
		return true;
	}
	obSrc->disconnectSignal(szSignal,obTrg,szSlot);
	return true;
}




// ASSASSINS! YOU MUST INDENT IT WELL :D


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
		Dump the kvirc widgets.
	@syntax:
		<array> $objects.dump(<print:boolean>)
	@description:
		This function return an array containing a representation of the widgets tree in kvirc;[br]
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
		KVSO_PARAMETER("print on current window",KVS_PT_BOOL,0,bFlag)
	KVSO_PARAMETERS_END(cmd)
	if (bFlag) cmd->window()->output(80, "Objects dump:");
		QWidgetList list = g_pApp->topLevelWidgets();

	KviStr spacing = ">";
	QString  szTemp;
	KviKvsArray * n = new KviKvsArray();
	int idx=0;
	for(int i=0;i<list.count();i++)
	{
		if( list.at(i)->isWidgetType())
		{
			if (bFlag)
			{
				cmd->window()->output(80, "Ptr %u: top level object: %c%s%c, class %s, %s, rect = %d, %d, %d, %d",
				list.at(i),
				KVI_TEXT_BOLD, list.at(i)->objectName().data(), KVI_TEXT_BOLD,
				list.at(i)->metaObject()->className(),
				list.at(i)->isVisible() ? "visible" : "hidden",
				list.at(i)->x(), list.at(i)->y(), list.at(i)->width(), list.at(i)->height());
			}
			QString  szClass=list.at(i)->metaObject()->className();
			QString szObj=list.at(i)->objectName();
			QString szTemp;
			szTemp = szClass + "::" + szObj;
			KviKvsVariant v;
			v.setString(szTemp);
			n->set(idx,new KviKvsVariant(v));
                /*	debug ("string %s",szTemp.toUtf8().data());
			debug ("class %s",szClass.toUtf8().data());
                        debug ("Obj %s",szObj.toUtf8().data());*/
			idx++;

			dumpChildObjects(cmd->window(), list.at(i), spacing.ptr(), bFlag,n,idx);
		}
	}

	cmd->returnValue()->setArray(n);
	return true;
}

static void dumpChildObjects(KviWindow *pWnd, QObject *parent, const char *spacing, bool bFlag, KviKvsArray *n, int &idx)
{
	const QObjectList list = parent->children();
	if( !list.count() ) return;
	QString sp(spacing);
	sp.append(">");
	for(int i=0;i<list.count();i++)
	{
		if( list.at(i)->isWidgetType() )
		{
			if (bFlag)
			{
				pWnd->output(80, "%sPtr %u: object: %c%s%c, class %s",
					spacing, list.at(i), KVI_TEXT_BOLD,
					list.at(i)->objectName().data(), KVI_TEXT_BOLD, list.at(i)->metaObject()->className()
				);
			}
			QString szClass=list.at(i)->metaObject()->className();
			QString szObj=list.at(i)->objectName();
			QString szTemp;
			szTemp=spacing+szClass+"::"+szObj;
			KviKvsVariant v;
			v.setString(szTemp);
			n->set(idx,new KviKvsVariant(v));
                        //debug ("string %s",szTemp.toUtf8().data());
			idx++;
			dumpChildObjects(pWnd, list.at(i), sp.toUtf8().data(), bFlag, n, idx );
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
	KVSM_REGISTER_FUNCTION(m,"classAllHandlers",objects_kvs_fnc_classAllHandlers);
	KVSM_REGISTER_FUNCTION(m,"classes",objects_kvs_fnc_classes);
	KVSM_REGISTER_FUNCTION(m,"dump",objects_kvs_fnc_listObjects);
	KVSM_REGISTER_FUNCTION(m,"exists",objects_kvs_fnc_exists);
	KVSM_REGISTER_FUNCTION(m,"instances",objects_kvs_fnc_instances);
	KVSM_REGISTER_FUNCTION(m,"name",objects_kvs_fnc_name);
	KVSM_REGISTER_FUNCTION(m,"variables",objects_kvs_fnc_variables);

	// commands
	KVSM_REGISTER_SIMPLE_COMMAND(m,"clearObjects",objects_kvs_cmd_clearObjects);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"connect",objects_kvs_cmd_connect);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"disconnect",objects_kvs_cmd_disconnect);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"killClass",objects_kvs_cmd_killClass);

	// Don't attempt to change the order of these calls.
	// Derived classes must be registered after the base ones.

	KviKvsObject_buttongroup::registerSelf();

	KviKvsObject_sql::registerSelf();
	KviKvsObject_list::registerSelf();
	KviKvsObject_widget::registerSelf();
	KviKvsObject_tablewidget::registerSelf();
	KviKvsObject_datetimeedit::registerSelf();
	KviKvsObject_button::registerSelf();
	KviKvsObject_textedit::registerSelf();
	KviKvsObject_textbrowser::registerSelf();
	KviKvsObject_checkbox::registerSelf();
	KviKvsObject_dialog::registerSelf();
	KviKvsObject_groupbox::registerSelf();
	KviKvsObject_label::registerSelf();
	KviKvsObject_vbox::registerSelf();
	KviKvsObject_hbox::registerSelf();
	KviKvsObject_layout::registerSelf();
	KviKvsObject_lcd::registerSelf();
	KviKvsObject_lineedit::registerSelf();
	KviKvsObject_menubar::registerSelf();
	KviKvsObject_workspace::registerSelf();
	KviKvsObject_combobox::registerSelf();
	KviKvsObject_listwidget::registerSelf();
	KviKvsObject_treewidget::registerSelf();
	KviKvsObject_treewidgetitem::registerSelf();
	KviKvsObject_mainwindow::registerSelf();
	KviKvsObject_painter::registerSelf();
	KviKvsObject_pixmap::registerSelf();
	KviKvsObject_popupmenu::registerSelf();
	KviKvsObject_radiobutton::registerSelf();
	KviKvsObject_slider::registerSelf();
	KviKvsObject_spinbox::registerSelf();
	KviKvsObject_tabwidget::registerSelf();
	KviKvsObject_toolbar::registerSelf();
	KviKvsObject_toolbutton::registerSelf();
	KviKvsObject_window::registerSelf();
	KviKvsObject_wizard::registerSelf();
	KviKvsObject_dockwindow::registerSelf();
	KviKvsObject_progressbar::registerSelf();
	KviKvsObject_file::registerSelf();
	KviKvsObject_wrapper::registerSelf();
	KviKvsObject_xmlreader::registerSelf();
	KviKvsObject_socket::registerSelf();
	KviKvsObject_http::registerSelf();
	KviKvsObject_ftp::registerSelf();

	KviKvsObject_process::registerSelf();
	KviKvsObject_memorybuffer::registerSelf();

	return true;
}

KVIRC_MODULE(
	"Objects",
	"4.0.0",
	"Copyright (C) 2000-2006:\n" \
		"  Szymon Stefanek (pragma at kvirc dot net)\n" \
		"  Krzysztof Godlewski (kristoff at poczta dot wprost dot pl)\n" \
		"  Tonino Imbesi (grifisx at barmes dot org)\n" \
		"  Alessandro Carbone (elfonol at gmail dot com)",
	"Object classes for KVIrc scripting language\n",
	objects_module_init,
	objects_module_can_unload,
	0,
	objects_module_cleanup,
	"objects"
)
