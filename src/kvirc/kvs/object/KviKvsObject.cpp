//=============================================================================
//
//   File : KviKvsObject.cpp
//   Creation date : Wed 08 Oct 2003 02:31:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_out.h"
#include "KviKvsObject.h"
#include "KviKvsHash.h"
#include "KviKvsKernel.h"
#include "KviWindow.h"
#include "KviApplication.h"
#include "KviModuleManager.h"
#include "KviConsoleWindow.h"
#include "KviLocale.h"
#include "KviError.h"
#include "KviControlCodes.h"
#include "KviIconManager.h"
#include "KviMemory.h"
#include "KviKvsObjectController.h"
#include "KviKvsObjectFunctionCall.h"
#include "KviKvsObjectFunctionHandlerImpl.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QTimer>
#include <QIcon>
#include <QPointer>

#include <ctime>

/*
		@doc: objects
		@title:
			Object scripting
		@short:
			Object scripting overview
		@keyterms:
			objects, object scripting, complex data structures
		@body:
			[big]Introduction[/big]
			The KVIrc scripting language is not object oriented in nature.
			Anyway, objects are a highlevel abstraction that allow
			to write complex code in a [i]clean[/i] way.
			So I've added at least some pseudo-object support.
			[big]Basic concepts[/big]
			Objects are arranged in tree structures.
			Each object that you create is either toplevel object or a child
			of another object. A toplevel object is a parentless one.
			Obviously all objects can have child objects.
			[br][br]
			When an object is destroyed, all its child objects are also destroyed.
			The toplevel objects are automatically destroyed when KVIrc quits.
			The objects are global to the entire application (this is different
			from previous KVIrc releases where the objects were local
			to the current frame window and arranged in a single tree
			with a builtin root object).
			[br][br]
			Each object is an instance of a class that defines its features.
			Each object has also a name, that is not necessary unique and is assigned to
			by the programmer; the name is just a mnemonic expedient, and
			you may also not need it.
			[br][br]
			Each object is identified by an [b]OPAQUE UNIQUE ID[/b].
			The ID is assigned by KVIrc and can be held in any variable.
			You can think the object ID as a [i]handle for the object[/i] or the object's pointer.
			Any action performed on the object will require its ID.
			[big]Creation and destruction[/big]
			To create an object you must use the [fnc]$new[/fnc]()
			function. [fnc]$new[/fnc]() requires three parameters:[br]
			- The object class (more about object classes later in this document)[br]
			- The ID of the parent object (this can be 0 for toplevel objects).[br]
			- The object name (eventually empty)
			[example]
				%myobject = [fnc]$new[/fnc]([class]object[/class],0,theName)
			[/example]
			[fnc]$new[/fnc]() returns the ID of the newly created object, or
			the STRING [b]0[/b] if the object creation fails
			(it is a string because the object ID's are generally strings, and 0 is an "invalid object ID").
			In well written scripts it is not common that the object creation fails, anyway
			you can check if the creation has failed in the following way:
			[example]
				[cmd]if[/cmd](%myobject)
					[cmd]echo[/cmd] "Object created!"
				else 
					[cmd]echo[/cmd] "Object creation failed!"
			[/example]
			You can also test the object ID's for equality:
			[example]
				[cmd]if[/cmd](%myobject == %anotherobject)[cmd]
					echo[/cmd] "This is the same object!";
			[/example]
			The parent object ID is optional, if not specified it is assumed to be 0.
			The object name is optional, but it may help you later in finding the object.
			[br][br]
			To destroy an object use the [cmd]delete[/cmd] command. (In previous versions
			this command was named [i]destroy[/i] and delete is currently aliased to that name too).
			[example]
				[cmd]delete[/cmd] %myobject
			[/example]
			If the destroyed object has child objects, these are destroyed too.
			[big]Fields: objects as pseudo-structures[/big]
			All the objects can contain variable fields.
			You can set an object's field by using the object scope operator "-&gt;":
			[example]
				%myobject-&gt;%fieldVariable = dataString
			[/example]
			To unset a field set it with empty data string (just like with a normal variable).
			To retrieve the field data use the object scope operator in the same way:
			[example]
				[cmd]echo[/cmd] %myobject-&gt;%fieldVariable
			[/example]
			The '-&gt;' operator has been stolen from the C language.
			In the KVIrc scripting language it switches from the global namespace
			to the object's one.[br]
			So in the above example %fieldVariable is owned by the object.[br]
			The first character of the variable name has no special meaning in the
			object namespace (in the global namespace the variables starting
			with an uppercase letter are global to the application, the other ones are local
			to the command sequence). The variable names are completely case insensitive.
			[br][br]
			Any [doc:operators]operator[/doc] can be used with the object field variables:
			[example]
				%myobject-&gt;%fieldVariable = 0
				%myobject-&gt;%fieldVarialbe ++
				[cmd]if[/cmd]0(%myobject-&gt;%fieldVariable != 1)
					[cmd]echo[/cmd] KVIrc is drunk, maybe a reboot will help?
			[/example]
			You can simulate C structures [i]on the fly[/i] by using objects and fields:
			[example]
				# Create a user description on the fly
				%myobj = [fnc]$new[/fnc]([class]object[/class],0,userDescription)
				# Set the fields
				%myobj-&gt;%nickname = Pragma
				%myobj-&gt;%username = daemon
				%myobj-&gt;%hostname = pippo.pragma.org
				%myobj-&gt;%info = Pragma goes always sleep too late
				%myobj-&gt;%info [doc:operators]&lt;&lt;[/doc] and wakes up too late too!
				# Call an (user defined) alias that stores the data to a file
				storetofile %myobj
				# Destroy the object
				[cmd]delete[/cmd] %myobj
			[/example]
			The field variables can be also dictionaries:
			[example]
				%theobj-&gt;%field[key] = something
			[/example]
			Unlike in C, there is no need to declare object fields.[br]
			If you have ever used other high level object-oriented languages, you may be used to declaring different types of
			variables: instance variables, which per definition define an object's state (at least partly) and local variables,
			which can be used in any function and will be only valid in the very current scope. This does and does not apply
			to KVI++.[br]
			Local variables can be used as normal and the scope of those variables will (naturally) be limited to the scope of
			the function they are defined in.
			[example]
				[cmd]class[/cmd](test,[class]object[/class])
				{
					test()
					{
						%test = "will this persist?"
					}

					anotherfunc() 
					{
						[cmd]echo[/cmd] "var: %test"
					}
				}

				%myObject = [fnc]$new[/fnc](test,0)
				%myObject-&gt;$test()
				[comment]# Behold! This will only print "var: "![/comment]
				%myObject-&gt;$anotherfunc()
			[/example]
			Instance variables, however, which are managed in the object's [i]field[/i] can be accessed at any time by anyone.
			[b]Warning:[/b] every script or object is potentially able to change the values of your field variables!
			They may also add or unset (empty) previously not used or used fields.[br]
			As earlier said, there is no need to declare object fields, as KVIrc will keep track of them. Even more precisely
			said, you [b]can not[/b] declare them in the class file itself (some later example will tell you otherwise,
			just keep in mind to ignore the pseudo code, as it does not reflect how KVI++ is really working in respect of
			fields.)[br]
			However, there is one way to declare and define object fields: using the constructor (please see below, if you
			are interesting in learning about this function), it is possible to [i]declare[/i] (really only for the human being
			reading the code) and more important initialize object fields. For more information, see the Constructor section
			below.[br]
			Any object can have any field variable; an [i]unset[/i] field is equivalent to an [i]empty[/i] field.[br]
			Note:[br]
			The KVIrc scripting language is not typed.
			Any object class (be patient... I'll explain classes in a while) identifier can be stored in any KVIrc variable:
			it is not possible to find out the object features by [i]examining[/i] its identifier.
			This may make the usage of objects a bit [i]unclear[/i];
			However, with some experience you will be able to use the objects in a very powerful way.
			The type-safety can be also simulated by a careful usage of object names;
			in the above example, the %myobj object was created with the [i]userDescription[/i] name.
			The storetofile alias could check the passed object's name and refuse to work
			if that does not match "userDescription".
			[br][br]
			A more complex use of fields will be described later in this document.
			[big]Member functions[/big]
			Just like in C++, the objects have member functions.
			For example, the [i]object[/i] class (again... read on) objects export the [classfnc:object]$name[/classfnc]()
			and [classfnc:object]$className[/classfnc]() functions.
			[example]
				%tmp = [fnc]$new[/fnc]([class]object[/class],0,myobject)
				[cmd]echo[/cmd] The object's name is %tmp-&gt;[classfnc:object]$name[/classfnc](), the class name is %tmp-&gt;[classfnc:object]$className[/classfnc]()
				# Destroy the object
				[cmd]delete[/cmd] %tmp
			[/example]
			Another cool function exported by the [class:object]object[/class] class is the
			[classfnc:object]$children[/classfnc]() function.
			It returns a comma separated list of child identifiers.
			[example]
				%tmp = [fnc]$new[/fnc]([class]object[/class],0,myobject)
				%tmpchild = [fnc]$new[/fnc]([class]object[/class],%tmp,child1)
				%tmpchild = [fnc]$new[/fnc]([class]object[/class],%tmp,child2)
				%tmpchild = [fnc]$new[/fnc]([class]object[/class],%tmp,child3)
				[cmd]echo[/cmd] The object's child list is: %tmp-&gt;[classfnc:object]$children[/classfnc]()
				# Destroy the object and the child
				[cmd]delete[/cmd] %tmp
			[/example]
			There are two special functions for each objects: the [i]constructor[/i] and the [i]destructor[/i].
			You will find more information on constructors and destructors later in this document,
			for now it's enough that you know that these functions are called automatically by KVIrc:
			the constructor is called when the object is created and the destructor is called when the
			object is being destroyed with [cmd]delete[/cmd].[br][br]
			[br]
			The object functions can be reimplemented on-the-fly
			by using the [cmd]privateimpl[/cmd] command: you can simply modify the behaviour of the function
			by writing your own function body.
			(This is an uncommon feature: unlike many other languages, you can reimplement object
			functions at run-time, when the object has been already created.)
			[br][br]
			A more complex example
			[example]
				%tmp = [fnc]$new[/fnc]([class]object[/class],0,myobject)
				[cmd]foreach[/cmd](%i,1,2,3)
				{
					%tmpchild = [fnc]$new[/fnc]([class]object[/class],%tmp,child%i)
					[cmd]privateimpl[/cmd](%tmpchild,destructor)
					{
						[cmd]echo[/cmd] Object [fnc]$this[/fnc] ([fnc]$this[/fnc]-&gt;[classfnc:object]$name[/classfnc]()) destroyed;
					}
				}
				[cmd]privateimpl[/cmd](%tmp,destructor)
				{
					%count = 0;
					[cmd]foreach[/cmd](%t,[fnc]$this[/fnc]-&gt;[classfnc:object]$children[/classfnc]())
					{
						[cmd]echo[/cmd] Children : %t-&gt;[classfnc:object]$name[/classfnc]() with class %t-&gt;[classfnc:object]$class[/classfnc]()
						%count++
					}
					[cmd]echo[/cmd] Just before destroying my %count child items.
				}
				[comment]# Destroy the object and it's child items[/comment]
				[cmd]delete[/cmd] %tmp
			[/example]
			In the example above four objects have been created.
			A [i]parent[/i] object named [i]myobject[/i], and three child objects.
			The destructor has been reimplemented for each child object,
			to make it [i]say[/i] its name (Please note the usage of [fnc]$this[/fnc]).
			In the parent destructor the child objects have been counted and listed.[br]
			Then the parent object is destroyed causing to:[br]
			- trigger the parent destructor.[br]
			- destroy all the child items (and consequently trigger all the [i]individual[/i] destructors).[br][br]
			[br]
			Not all the object functions must return a value:
			If a function does not return a meaningful value, or you just want to ignore it, you can call it in the following way:
			[example]
				%anyobject-&gt;$functionname()
			[/example]
			[big]Classes[/big]
			As said before, all objects are instances of a specific class.
			This concept is common to almost all object oriented languages.
			A class is a collection of methods that define an object's behaviour.
			It's not easy to explain it, so I'll try with an example:[br]
			[b]Please note, that this is pseudo code. KVI++ does by no mean employs
			a [i]field[/i] directive as shown below![/b]
			[example]
				class HostAddress
				{
					field hostname
					function ipnumber()
					function isLocalhost()
				}
			[/example]
			The above class is a representation of a host address.
			You create an [b]instance of this class[/b] and set the hostname field, for example,
			to www.kernel.org.
			The object is now able to give you information about the hostname in a transparent way:
			You can call the ipnumber() function, and the object will return you the
			digits and dots representation of www.kernel.org.
			The isLocalhost() function will return true if the hostname refers to the local machine
			The object internal job is hidden from the user, but probably it will be a huge job.
			To obtain the IP number from the hostname, the object will probably have to perform a DNS call (usually a complex task).
			To check if the hostname references the local machine, the object will have to obtain the local hostname
			from the system (in some [i]unspecified[/i] way) and then compare it with the given [i]hostname[/i] field.
			[br][br]
			The internal job of the object is defined by the "implementation of the class".
			Obviously, the programmer that creates the class has to write that implementation.
			[example]
				class HostAddress
				{
					field hostname
					function ipnumber()
					{
						find the nearest DNS server
						make the DNS call
						wait for the response
						decode the response
					}
					function isLocalhost()
					{
						query the kernel for the local hostname
						compare the obtained hostname with the hostname field
					}
				}
			[/example]
			In the above example I have [i]implemented[/i] the two functions in pseudo code.
			[br][br]
			Let's go back to the real world.
			[br][br]
			KVirc contains a [doc:classes]set of built-in ready-to-use classes[/doc].
			The basic class is [class]object[/class]: all the other classes are derived from this (more about
			object inheritance later in this doc).
			[br][br]
			Another available class is [class]socket[/class] that is an interface to the real system sockets.
			An instance of the [class]socket[/class] class can connect and communicate with other hosts on the net.
			[br][br]
			The [b]class definitions are GLOBAL to the entire application[/b]: all server windows share them.
			[br][br]
			So now we can say that in KVIrc [b]a CLASS is a collection of features that define the behaviour of an object.
			The user interface to the class are the member functions and the events.[/b]
			[big]Inheritance[/big]
			Someone asked for derived classes?[br]
			Here we go:[br]
			The [cmd]class[/cmd] command allows you to define new object classes.
			In KVI++, A new class must be always derived from some other class: the lowest possible
			level of inheritance is 1: deriving from class [class]object[/class].
			[example]
				[cmd]class[/cmd](helloworld,[class]object[/class])
				{
					sayhello()
					{
						[cmd]echo[/cmd] Hello world!
					}
				}
			[/example]
			The above class is named [i]helloworld[/i]. It inherits the [class]object[/class] class.
			This means that it acquires all the [class]object[/class] functions: [classfnc:object]$name[/classfnc](),
			[classfnc:object]$class[/classfnc](), [classfnc:object]$children[/classfnc]()...
			Additionally, it has the $sayhello() function, that [i]echoes Hello world[/i] to the console.
			Now you can create an instance of this class:
			[example]
				%instance = [fnc]$new[/fnc](helloworld)
				%instance-&gt;$sayhello()
			[/example]
			You should see [i]Hello world[/i] printed in the console.
			Easy job... let's make the things a bit more complex now:
			derive another class from helloworld and make it say [i]hello[/i] in two different languages:
			[example]
			[cmd]class[/cmd](localizedhelloworld,helloworld)
			{
				[comment]# define the setlanguage function[/comment]
				[comment]# note that <$0 = language> is just a programmer reminder[/comment]
				setlanguage(<$0 = language>)
				{
					[cmd]if[/cmd](($0 == english) || ($0 == italian))
					{
						[fnc:$this]$$[/fnc]-&gt;%lang = $0
						[cmd]return[/cmd] 1
					} 
					else 
					{
						[cmd]echo[/cmd] I don't know that language ($0)
						[cmd]echo[/cmd] defaulting to English
						[fnc:$this]$$[/fnc]-&gt;%lang = english
						[cmd]return[/cmd] 0
					}
				}

				sayhello()
				{
					[cmd]if[/cmd]([fnc:$this]$$[/fnc]-&gt;%lang == italian)
						[cmd]echo[/cmd] Ciao mondo!
					else 
						[fnc:$this]$$[/fnc]-&gt;$helloworld:sayhello()
				}
			}
			[/example]
			Now you can call:
			[example]
				%m = [fnc]$new[/fnc](localizedhelloworld)
				%m-&gt;$setLanguage(italian)
				%m-&gt;$sayhello()
				%m-&gt;$setLanguage(english)
				%m-&gt;$sayhello()
				%m-&gt;$setLanguage(turkish)
				%m-&gt;$sayhello()
				[cmd]delete[/cmd] %myobj
			[/example]
			The class defined above is inherited from the previously defined helloworld class:
			so it inherits the [i]object[/i] class functions and events and the sayhello function from [i]helloworld[/i].
			In addition a setlanguage function is defined that stores in a variable the language name passed
			as a parameter (after checking its validity). ($0 evaluates to the first parameter passed)
			If the language is unknown the setlanguage function will return 0 (false).
			Now we want to be able to say [i]hello world[/i] in Italian and English.
			So we [b]override[/b] the inherited sayhello function.
			[i]To override[/i] means [i]to reimplement[/i]: if you call %object-&gt;$sayhello() and %object
			contains the ID of an instance of class [i]localizedhelloworld[/i], the new implementation of that function will be called (executed).
			The inherited sayhello was able to say [i]hello world[/i] only in English, so we can still use it in the new implementation
			without rewriting its contents. So if the language set is not Italian we assume that it is English and
			call the [b]base class implementation[/b].
			[example]
				[fnc]$this[/fnc]-&gt;$helloworld:sayhello()
				[comment]# equivalent to $$-&gt;$helloworld:sayhello(),[/comment]
				[comment]# to $this-&gt;$helloworld::sayhello(),[/comment]
				[comment]# and to $$-&gt;$helloworld::sayhello()[/comment]
			[/example]
			otherwise the language is Italian and we say [i]hello[/i] in Italian :).
			So, to call a base class implementation of a function we prepend the base class name before the function name in the call.
			The base class name could be also [class]object[/class] in this case, but the [class]object[/class] class has no [i]sayhello[/i] function defined
			so it would result in an error.[br][br]
			In the above example, all the values of [fnc]$this[/fnc]-&gt;%language
			that are not equal to Italian are assumed to be English.
			This is not always true, for example, just after the object creation the %language variable field
			is effectively empty. The above class works correctly in this case, but we might want to have always
			a coherent state of the field variables, so we need another concept: the class [b]constructor[/b]
			that will be discussed in the next paragraph.[br][br]
			Note: multiple inheritance (inheritance from more than one base class) is not implemented, KVIrc is not a compiler. :)[br][br]
			Objects are much more powerful...[br][br]
			Do a [cmd]clearobjects[/cmd] to cleanup the old class definitions and read on.
			[big]Constructors and destructors[/big]
			The class constructor is a [b]function[/b] that is called automatically just after the object
			has been created internally by KVIrc and just before the [fnc]$new[/fnc]
			function returns. It should be used to setup the internal object state.[br]
			The constructor can and should list and initialize all the necessary object fields.
			[example]
				[cmd]class[/cmd](myObject,[class]object[/class])
				{
					constructor()
					{
						[fnc]$this[/fnc]-&gt;%test = "This is a sample object field."
					}
				}

				%myObject = [fnc]$new[/fnc](myObject,[class]object[/class])
				[cmd]echo[/cmd] %myObject-&gt;%test
			[/example]
			Will thus print "This is a sample object field."[br]
			Unlike in C++, in KVIrc, the constructor CAN return a value:[br]
			If it returns 0 it signals the object creation failure: the object
			is immediately destroyed and [fnc]$new[/fnc]() returns 0 to the caller.
			Any other return value is treated as success, so the object is effectively
			created and [fnc]$new[/fnc]() returns its ID to the caller.[br]
			This said, KVI++ will automatically return a value of 1 and you should [b]never[/b]
			return a value other than 0 if something bad happened (like a mandatory parameter was not given in the $new()
			call or the like.) KVIrc will also issue a warning message and remind you of this when a non-zero value is
			returned.[br]
			All the builtin classes have a constructor defined that will almost never fail (only if we run out of memory),
			so you can avoid to check the [fnc]$new[/fnc]() return value
			when creating the instances of the built-in classes.[br][br]
			In derived classes you can override the constructor to setup your object's state.[br]
			You should [b]always call the base class constructor[/b] in your overridden one, to setup
			the base class state, and propagate its return value (eventually modified if the base class
			constructor is successful but your derived class initialization fails).[br]
			This very basic example will illustrate how to do this (please read the paragraph about inheriting classes
			above first):
			[example]
				[cmd]class[/cmd](baseObject,[class]object[/class])
				{
					constructor()
					{
						[cmd]echo[/cmd] "baseObject or derived object created."
					}
				}

				[cmd]class[/cmd](derivedObject,baseObject)
				{
					constructor()
					{
						[cmd]echo[/cmd] "derivedObject object created."
						[fnc]$this[/fnc]->$baseObject::constructor()
					}
				}
			[/example]
			In practice, the builtin class constructors do nothing other than setting the return
			value to 1 so you can even avoid to call them, but in any other case you must do it.[br][br]
			This is different from C (for example), where the constructors are called (more or less)
			automatically.
			[big]Signals and slots[/big]
			The signals and slots are a powerful mean of inter-object communication.
			A signal is emitted by an object to notify a change in its state.
			For example, the [class:button]button class[/class] emits the
			[classsignal:button]clicked[/classsignal] signal when the user clicks the button.[br][br]
			A signal is emitted by an object and can be received and handled by any other existing object
			(including the object that emits the signal).[br]
			The handler function for a signal is called "slot".[br]
			It is just a convention: in fact, a slot is a normal object function (and any object function can be a slot).
			More than one slot can be connected to a single signal, and more signals can be connected to a single slot.[br]
			In this way, many objects can be notified of a change in a single object, as well as a single object
			can easily handle state-changes for many objects.[br]
			The signal/slot behaviour could be easily implemented by a careful usage of object functions.
			[big]So why signals and slots?[/big]
			Because signals are much more powerful in many situations.
			The signals have no equivalent in C/C++... but they have been implemented in many high-level
			C/C++ libraries and development kits (including the system-wide signal/handler mechanism implemented
			by all the modern kernels and used in inter-process communication).[br]
	*/

/*
		@doc: object
		@keyterms:
			object class, object, class
		@title:
			object class
		@type:
			class
		@short:
			Base class for all the KVIrc objects
		@inherits:
			none
		@description:
			This is the base class for all builtin KVIrc object classes.
			It exports functions to retrieve an object's name, to iterate
			through child objects and to lookup a child object by name or class.
			Additionally, this class provides builtin timer functionality.
			The [classfnc]$constructor[/classfnc] and [classfnc]$destructor[/classfnc]
			functions are empty implementations that all the other classes inherit.
		@functions:
			!fn: $constructor()
			Constructor for this object class.
			The default implementation does nothing.
			!fn: $destructor()
			Destructor for this object class.
			The default implementation emits the signal [i][classsignal]destroyed[/classsignal][/i].
			!fn: $name()
			Returns the name of this object.
			!fn: $parent()
			Returns the parent object of this object or 0 if this object has no parent.
			!fn: $timerEvent(<timerId>)
			Handler for the timer events.
			The default implementation does nothing.
			See also [classfnc]$startTimer[/classfnc]()
			and [classfnc]$killTimer[/classfnc]().
			!fn: $startTimer(<timeout>)
			Starts a builtin timer for this object and returns its timer ID
			as a string or '-1' if <timeout> was invalid.
			The [classfnc]$timerEvent[/classfnc]() handler function
			will be called every <timeout> milliseconds until the timer is stopped by $killTimer().
			!fn: $killTimer(<timer id>)
			Stops the timer specified by <timer id>.
			!fn: $className()
			Returns the class name of this object instance
			!fn: $findChild(<class>,<name>)
			Returns the first child that matches <class> and <name>.
			If <class> is an empty string, any class matches,
			if <name> is an empty string, any name matches.
			This function traverses the entire child tree but is [b]not[/b] recursive.
			!fn: $childCount()
			Returns the number of child objects
			!fn: $emit(<signal_name>[,parameters])
			Emits the signal <signal_name> passing the optional [parameters].
			See the [doc:objects]objects documentation[/doc] for an overview of signals and slots.
			!fn: $children()
			Returns an array of child object identifiers.
			!fn: $signalSender()
			Returns the current signal sender when in a slot connected to a signal.
			In other contexts this function returns an empty string.
			You can safely use it to test if the current function has been
			triggered directly or from a signal emission.
			!fn: $signalName()
			Returns the name of the last signal that has triggered one of this object's slots.
			When called in a slot handler, the triggering signal name is returned.
			!fn: $property(<Qt property name>[,bNowarning:boolean])
			This is for really advanced scripting.[br]
			All KVIrc widgets are based on the Qt library ones.[br]
			The Qt library allows to set and read special properties.[br]
			You will have to take a look at the Qt documentation for each widget type
			to see the available property names.[br]
			The supported property types are: Rect, Size, Point, Color, String, CString,
			Int, UInt, Bool and enumeration types.[br]
			For example, the widget's x coordinate can be retrieved by using the [classfnc]$x[/classfnc]()
			function or by calling $property(x).[br]
			There are many properties that are available only through the [classfnc]$property[/classfnc]() call:[br]
			For example, you can find out if the widget accepts drops by calling [classfnc]$property[/classfnc](acceptDrops).[br]
			This function will be mainly useful in the [class]wrapper[/class] class.
			!fn: $setProperty(<Qt property>,<property value>)
			Sets a qt property for this widget.[br]
			This is for advanced scripting, and can control really many features of the Qt widgets.[br]
			For example, the [class]multilineedit[/class] widgets can be set to
			the [i]password[/i] echo mode only by using this function call:
			[example]
				%X=$new(lineedit, 0, a_name)[br]
				%X-&gt;$show()[br]
				%X-&gt;$setProperty(echoMode,Password)
			[/example]
			The available properties to be set are listed by [classfnc]$listProperties[/classfnc]()[br]
			and must appear in the list as writable.[br]
			This function will be mainly useful in the [class]wrapper[/class] class.
			!fn: $listProperties([bArray])
			Lists the properties of this object.[br]
			If <bArray> is $true then the function returns the properties
			as an array of descriptive strings, otherwise the properties are dumped to the
			active window. If <bArray> is not passed then it is assumed to be $false.
			This function will be mainly useful in the [class]wrapper[/class] class.
		@signals:
			!sg: destroyed()
			Emitted by the default implementation of [classfnc]$destructor[/classfnc].
			If you reimplement [classfnc]$destructor[/classfnc] in one of the derived
			classes (or as a private implementation), and still want this signal
			to be emitted you must emit it by yourself, or (better) call the base class
			destructor.
	*/

// we use a char * pointer just to store a number
// we don't use void * just because incrementing a void pointer doesn't look that good
static char * g_hNextObjectHandle = (char *)nullptr;

KviKvsObject::KviKvsObject(KviKvsObjectClass * pClass, KviKvsObject * pParent, const QString & szName)
    : QObject(pParent), m_szName{szName}, m_pClass{pClass}
{
	setObjectName(szName);

	if(g_hNextObjectHandle == nullptr)
		g_hNextObjectHandle++; // make sure it's never 0
	m_hObject = (kvs_hobject_t)g_hNextObjectHandle;
	g_hNextObjectHandle++;

	m_pChildList = new KviPointerList<KviKvsObject>;
	m_pChildList->setAutoDelete(false);

	m_pDataContainer = new KviKvsHash();

	if(pParent)
		pParent->registerChild(this);

	KviKvsKernel::instance()->objectController()->registerObject(this);
}

KviKvsObject::~KviKvsObject()
{
	if(!m_bDestructorCalled)
	{
		// Destructor not called yet.. something is wrong.
		if(!m_bAboutToDie)
		{
			// Ow... we are being deleted directly and not via die() or dieNow()
			// This means that we need to call the kvs destructor here which
			// is AFTER the C++ destructors of the derived classes.
			// This means that a list object, for instance, will be already
			// cleared in its kvs destructor which isn't that nice.

			// Complain to the developers so they fix the code.
			KVI_ASSERT_MSG(m_bAboutToDie, "You should never delete a KviKvsObject directly, call dieNow() instead");

			// If we pass the assert in some kind of build go ahead nicely
			m_bAboutToDie = true; // don't attempt to die twice
		}
		else
		{
			// This might be an "early" delete after a delayedDie() call.
			// It shouldn't happen very often but if it happens we can't do much about it... don't complain.
		}

		// Well... need to call the destructor.
		callDestructor();
	}
	else
	{
		// Destructor already called.
		m_bAboutToDie = true; // don't attempt to die twice
	}

	// Kill any child not deleted by the user
	while(m_pChildList->first())
		m_pChildList->first()->dieNow();

	// Ok, from this point we shouldn't be touched by any one via KVS
	// so we can start really deleting stuff...
	delete m_pChildList;

	// Disconnect all the signals
	for(;;)
	{
		if(!m_pSignalDict)
			break;
		KviPointerHashTableEntry<QString, KviKvsObjectConnectionList> * pSignalList = m_pSignalDict->firstEntry();
		if(!pSignalList)
			break;
		KviKvsObjectConnection * pConnection = pSignalList->data()->first();
		if(!pConnection)
			break;
		disconnectSignal(pSignalList->key(), pConnection);
	}

	// Disconnect all the slots
	for(;;)
	{
		if(!m_pConnectionList)
			break;
		KviKvsObjectConnection * pConnection = m_pConnectionList->first();
		if(!pConnection)
			break;
		// We need a copy of szSignal this since pConnection is deleted inside disconnectSignal()
		// and pConnection->szSignal dies too (but is referenced after the connection delete)
		QString szSignalCopy = pConnection->szSignal;
		pConnection->pSourceObject->disconnectSignal(szSignalCopy, pConnection);
	}

	// Detach from the kvs kernel
	KviKvsKernel::instance()->objectController()->unregisterObject(this);

	// Detach from the parent object
	if(parentObject())
		parentObject()->unregisterChild(this);

	// If we wrap a Qt object then detach and eventually delete
	if(m_pObject)
	{
		// detach
		disconnect(m_pObject, SIGNAL(destroyed()), this, SLOT(objectDestroyed()));
		// delete if we own it
		if(m_bObjectOwner)
			delete m_pObject;
	}

	// Kill member variables
	delete m_pDataContainer;
	// Kill function container
	if(m_pFunctionHandlers)
		delete m_pFunctionHandlers;

	// Bye bye :)
}

void KviKvsObject::callDestructor()
{
	KVI_ASSERT(m_bAboutToDie);        // this should be called by die(), dieNow() or ~KviKvsObject only, which set m_bAboutToDie to true.
	KVI_ASSERT(!m_bDestructorCalled); // calling code must take care of this

	m_bDestructorCalled = true;

	// Note that children are still alive: the user can clean them up manually
	callFunction(this, "destructor");
}

bool KviKvsObject::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	return true;
}

QWidget * KviKvsObject::parentScriptWidget()
{
	if(parentObject())
	{
		if(parentObject()->object())
		{
			if(parentObject()->object()->isWidgetType())
				return (QWidget *)(parentObject()->object());
		}
	}
	return nullptr;
}

void KviKvsObject::unregisterChild(KviKvsObject * pChild)
{
	m_pChildList->removeRef(pChild);
}

void KviKvsObject::registerChild(KviKvsObject * pChild)
{
	m_pChildList->append(pChild);
}

// SIGNAL/SLOT stuff

bool KviKvsObject::connectSignal(const QString & sigName, KviKvsObject * pTarget, const QString & slotName)
{
	if(!pTarget->lookupFunctionHandler(slotName))
		return false; // no such slot

	if(!m_pSignalDict)
	{
		m_pSignalDict = new KviPointerHashTable<QString, KviKvsObjectConnectionList>(7, false);
		m_pSignalDict->setAutoDelete(true);
	}

	KviKvsObjectConnectionList * l = m_pSignalDict->find(sigName);
	if(!l)
	{
		l = new KviKvsObjectConnectionList;
		l->setAutoDelete(true);
		m_pSignalDict->insert(sigName, l);
	}

	KviKvsObjectConnection * con = new KviKvsObjectConnection;

	con->pSourceObject = this;
	con->pTargetObject = pTarget;
	con->szSignal = sigName;
	con->szSlot = slotName;

	l->append(con);
	pTarget->registerConnection(con);
	return true;
}

void KviKvsObject::registerConnection(KviKvsObjectConnection * pConnection)
{
	if(!m_pConnectionList)
	{
		m_pConnectionList = new KviKvsObjectConnectionList;
		m_pConnectionList->setAutoDelete(false);
	}
	m_pConnectionList->append(pConnection);
}

bool KviKvsObject::disconnectSignal(const QString & sigName, KviKvsObject * pTarget, const QString & slotName)
{
	if(!m_pSignalDict)
		return false; //no such signal to disconnect

	KviKvsObjectConnectionList * l = m_pSignalDict->find(sigName);
	if(!l)
		return false;

	KviKvsObjectConnectionListIterator it(*l);

	while(KviKvsObjectConnection * sl = it.current())
	{
		if(sl->pTargetObject == pTarget)
		{
			if(KviQString::equalCI(sl->szSlot, slotName))
			{
				pTarget->unregisterConnection(sl);
				l->removeRef(sl);
				if(l->isEmpty())
					m_pSignalDict->remove(sigName);
				if(m_pSignalDict->isEmpty())
				{
					delete m_pSignalDict;
					m_pSignalDict = nullptr;
				}
				return true;
			}
		}
		++it;
	}
	return false;
}

bool KviKvsObject::disconnectSignal(const QString & sigName, KviKvsObjectConnection * pConnection)
{
	if(!m_pSignalDict)
		return false;
	KviKvsObjectConnectionList * l = m_pSignalDict->find(sigName);
	//KVI_ASSERT(l);
	if(!l)
		return false;
	pConnection->pTargetObject->unregisterConnection(pConnection);
	//KVI_ASSERT(l->findRef(pConnection) > -1);
	l->removeRef(pConnection);
	if(l->isEmpty())
		m_pSignalDict->remove(sigName);
	if(m_pSignalDict->isEmpty())
	{
		delete m_pSignalDict;
		m_pSignalDict = nullptr;
	}
	return true;
}

bool KviKvsObject::unregisterConnection(KviKvsObjectConnection * pConnection)
{
	if(!m_pConnectionList)
		return false;
	bool bOk = m_pConnectionList->removeRef(pConnection); // no auto delete !
	if(!bOk)
		return false;
	if(m_pConnectionList->isEmpty())
	{
		delete m_pConnectionList;
		m_pConnectionList = nullptr;
	}
	return true;
}

int KviKvsObject::emitSignal(const QString & sigName, KviKvsObjectFunctionCall * pOuterCall, KviKvsVariantList * pParams)
{
	if(!m_pSignalDict)
		return 0;

	KviKvsObjectConnectionList * l = m_pSignalDict->find(sigName);
	if(!l)
		return 0; // no slots registered

	KviKvsVariant retVal;

	// The objects we're going to disconnect
	KviPointerList<KviKvsObjectConnection> * pDis = nullptr;

	kvs_int_t emitted = 0;

	KviKvsObjectConnectionListIterator it(*l);

	while(KviKvsObjectConnection * s = it.current())
	{
		// save it, since s may be destroyed in the call!
		KviKvsObject * pTarget = s->pTargetObject;

		emitted++;

		kvs_hobject_t hTarget = pTarget->handle();
		kvs_hobject_t hOld = pTarget->signalSender();

		pTarget->setSignalSender(m_hObject);
		pTarget->setSignalName(sigName);

		if(!pTarget->callFunction(this, s->szSlot, QString(), pOuterCall->context(), &retVal, pParams))
		{
			if(KviKvsKernel::instance()->objectController()->lookupObject(hTarget) && it.current())
			{
				pOuterCall->warning(
				    __tr2qs_ctx("Broken slot '%Q' in target object '%Q::%Q' while emitting signal '%Q' from object '%Q::%Q': disconnecting", "kvs"),
				    &(s->szSlot),
				    &(s->pTargetObject->getClass()->name()),
				    &(s->pTargetObject->getName()),
				    &(sigName),
				    &(getClass()->name()),
				    &m_szName);

				if(!pDis)
				{
					pDis = new KviPointerList<KviKvsObjectConnection>;
					pDis->setAutoDelete(false);
				}
				pDis->append(s);
			}
			else
			{
				// else destroyed in the call! (already disconnected)
				pOuterCall->warning(
				    __tr2qs_ctx("Slot target object destroyed while emitting signal '%Q' from object '%Q::%Q'", "kvs"),
				    &(sigName),
				    &(getClass()->name()),
				    &m_szName);
			}
		}

		if(KviKvsKernel::instance()->objectController()->lookupObject(hTarget))
		{
			pTarget->setSignalSender(hOld);
		}

		++it;
	}

	if(pDis)
	{
		// we have some signals to disconnect (because they're broken)
		for(KviKvsObjectConnection * con = pDis->first(); con; con = pDis->next())
			disconnectSignal(sigName, con);
		delete pDis;
	}

	return emitted;
}

bool KviKvsObject::function_name(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setString(getName());
	return true;
}

bool KviKvsObject::function_parent(KviKvsObjectFunctionCall * c)
{
	KviKvsObject * o = parentObject();
	c->returnValue()->setHObject(o ? o->handle() : (kvs_hobject_t) nullptr);
	return true;
}

bool KviKvsObject::function_className(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setString(getClass()->name());
	return true;
}

bool KviKvsObject::function_childCount(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setInteger((kvs_int_t)(m_pChildList->count()));
	return true;
}

bool KviKvsObject::function_signalSender(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setHObject(m_hSignalSender);
	return true;
}

bool KviKvsObject::function_signalName(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setString(m_szSignalName);
	return true;
}

bool KviKvsObject::function_destructor(KviKvsObjectFunctionCall * c)
{
	emitSignal("destroyed", c);
	return true;
}

bool KviKvsObject::function_children(KviKvsObjectFunctionCall * c)
{
	KviKvsArray * a = new KviKvsArray();
	int id = 0;
	for(KviKvsObject * o = m_pChildList->first(); o; o = m_pChildList->next())
	{
		a->set(id, new KviKvsVariant(o->handle()));
		id++;
	}
	c->returnValue()->setArray(a);
	return true;
}

bool KviKvsObject::function_findChild(KviKvsObjectFunctionCall * c)
{
	QString szClass, szName;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("className", KVS_PT_STRING, KVS_PF_OPTIONAL, szClass)
	KVSO_PARAMETER("objectName", KVS_PT_STRING, KVS_PF_OPTIONAL, szName)
	KVSO_PARAMETERS_END(c)

	KviKvsObject * o = findChild(szClass, szName);
	c->returnValue()->setHObject(o ? o->handle() : (kvs_hobject_t) nullptr);

	return true;
}

bool KviKvsObject::function_emit(KviKvsObjectFunctionCall * c)
{
	QString szSignal;
	KviKvsVariantList vList;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("signal", KVS_PT_NONEMPTYSTRING, 0, szSignal)
	KVSO_PARAMETER("params", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL, vList)
	KVSO_PARAMETERS_END(c)

	emitSignal(szSignal, c, &vList);
	return true;
}

bool KviKvsObject::function_startTimer(KviKvsObjectFunctionCall * c)
{
	kvs_uint_t timeout;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("timeout", KVS_PT_UINT, 0, timeout)
	KVSO_PARAMETERS_END(c)

	c->returnValue()->setInteger((kvs_int_t)(startTimer(timeout)));
	return true;
}

bool KviKvsObject::function_killTimer(KviKvsObjectFunctionCall * c)
{
	kvs_int_t id;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("timerId", KVS_PT_INT, 0, id)
	KVSO_PARAMETERS_END(c)
	killTimer(id);
	return true;
}

bool KviKvsObject::function_listProperties(KviKvsObjectFunctionCall * c)
{
	bool bArray = false;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bArray", KVS_PT_BOOL, KVS_PF_OPTIONAL, bArray)
	KVSO_PARAMETERS_END(c)

	c->returnValue()->setNothing();

	KviKvsArray * a = bArray ? new KviKvsArray() : nullptr;

	KviWindow * w = c->context()->window();

	if(!bArray)
		w->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Listing Qt properties for object named '%Q' of KVS class %Q", "kvs"), &m_szName, &(m_pClass->name()));
	kvs_int_t cnt = 0;
	if(m_pObject)
	{
		const QMetaObject * o = m_pObject->metaObject();
		if(!bArray)
			w->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("Properties for Qt class %s", "kvs"), o->className());
		kvs_int_t idx = 0;
		QMetaProperty prop = o->property(idx);
		const QMetaProperty * p = &prop;
		while(p)
		{
			QString szOut;
			QString szName = p->name();
			QString szType = p->typeName();
			if(bArray)
				szOut = QString("%1, %2").arg(szName, szType);
			else
			{
				szOut = QString(__tr2qs_ctx("Property: %1%2%3, type %4", "kvs")).arg(QChar(KviControlCodes::Bold)).arg(szName).arg(QChar(KviControlCodes::Bold)).arg(szType);
				szOut.prepend(" ");
			}

			if(p->isEnumType())
			{
				szOut += ", enum(";
				szOut += ")";
			}

			// FIXME: QT4 Need to read better the docs and check the changes: there seem to be too many
			//        for me to fix now. Actually I need to get the whole executable working...
			if(p->isWritable())
				szOut += ", writable";
			if(bArray)
				a->set(cnt, new KviKvsVariant(szOut));
			else
				w->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, szOut);
			idx++;
			if(idx < o->propertyCount())
			{
				prop = o->property(idx);
				p = &prop;
			}
			else
				p = nullptr;
			cnt++;
		}
	}

	if(bArray)
		c->returnValue()->setArray(a);
	else
		w->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs_ctx("%d properties listed", "kvs"), cnt);
	return true;
}

bool KviKvsObject::function_setProperty(KviKvsObjectFunctionCall * c)
{
	QString szName;
	KviKvsVariant * v;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("propertyName", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSO_PARAMETER("propertyValue", KVS_PT_VARIANT, 0, v)
	KVSO_PARAMETERS_END(c)

	c->returnValue()->setNothing();

	if(!m_pObject)
	{
		// there are no Qt properties at all
		c->warning(__tr2qs_ctx("The object named '%Q' of class '%Q' has no Qt properties", "kvs"), &m_szName, &(m_pClass->name()));
		return true;
	}

	int idx = m_pObject->metaObject()->indexOfProperty(szName.toUtf8().data());
	if(idx < 0)
	{
		c->warning(__tr2qs_ctx("No Qt property named '%Q' for object named '%Q' of class '%Q'", "kvs"), &szName, &m_szName, &(m_pClass->name()));
		return true;
	}

	QMetaProperty prop = m_pObject->metaObject()->property(idx);
	const QMetaProperty * p = &prop;
	if(!p->isValid())
	{
		c->warning(__tr2qs_ctx("Can't find property named '%Q' for object named '%Q' of class '%Q': the property is indexed but it doesn't really exist", "kvs"), &szName, &m_szName, &(m_pClass->name()));
		return true;
	}

	QVariant vv = m_pObject->property(szName.toUtf8().data());
	if(!vv.isValid())
	{
		c->warning(__tr2qs_ctx("Can't find property named '%Q' for object named '%Q' of class '%Q': the property is indexed and defined but the returned variant is not valid", "kvs"), &szName, &m_szName, &(m_pClass->name()));
		return true;
	}

	if(p->isEnumType())
	{
		QString szKey;
		v->asString(szKey);
		int val = p->enumerator().keyToValue(szKey.toUtf8().data());
		if(val < 0)
		{
			c->warning(__tr2qs_ctx("Value '%Q' for property '%Q' of object named '%Q' of class '%Q' is not valid", "kvs"), &szKey, &szName, &m_szName, &(m_pClass->name()));
			return true;
		}
		QVariant var(val);
		m_pObject->setProperty(szName.toUtf8().data(), var);
		return true;
	}

#define WRONG_TYPE(__therighttype)                                                                                                                                          \
	{                                                                                                                                                                       \
		c->warning(__tr2qs_ctx("The property is of type %s but the supplied argument can't be converted to that type (expecting '%s')", "kvs"), p->type(), __therighttype); \
		return true;                                                                                                                                                        \
	}

	switch(vv.type())
	{
		case QVariant::Int:
		{
			kvs_int_t i;
			if(!v->asInteger(i))
				WRONG_TYPE("integer")
			m_pObject->setProperty(szName.toUtf8().data(), QVariant((int)i));
		}
		break;
		case QVariant::UInt:
		{
			kvs_int_t i;
			if(!v->asInteger(i))
				WRONG_TYPE("unsigned integer")
			if(i < 0)
				WRONG_TYPE("unsigned integer")
			m_pObject->setProperty(szName.toUtf8().data(), QVariant((unsigned int)i));
		}
		break;
		case QVariant::Double:
		{
			kvs_real_t i;
			if(!v->asReal(i))
				WRONG_TYPE("real")
			m_pObject->setProperty(szName.toUtf8().data(), QVariant((double)i));
		}
		break;
		case QVariant::Bool:
			m_pObject->setProperty(szName.toUtf8().data(), QVariant(v->asBoolean()));
			break;
		case QVariant::String:
		{
			QString s;
			v->asString(s);
			m_pObject->setProperty(szName.toUtf8().data(), QVariant(s));
		}
		break;
		case QVariant::ByteArray:
		{
			QString s;
			v->asString(s);
			m_pObject->setProperty(szName.toUtf8().data(), QVariant(s.toUtf8()));
		}
		break;
		case QVariant::Point:
		{
			if(!v->isArray())
				WRONG_TYPE("array(integer,integer)")
			KviKvsArray * a = v->array();
			KviKvsVariant * x = a->at(0);
			KviKvsVariant * y = a->at(1);
			if(!x || !y)
				WRONG_TYPE("array(integer,integer)")
			kvs_int_t iX, iY;
			if(!x->asInteger(iX) || !y->asInteger(iY))
				WRONG_TYPE("array(integer,integer)")
			m_pObject->setProperty(szName.toUtf8().data(), QVariant(QPoint(iX, iY)));
		}
		break;
		case QVariant::Size:
		{
			if(!v->isArray())
				WRONG_TYPE("array(integer,integer)")
			KviKvsArray * a = v->array();
			KviKvsVariant * w = a->at(0);
			KviKvsVariant * h = a->at(1);
			if(!w || !h)
				WRONG_TYPE("array(integer,integer)")
			kvs_int_t iW, iH;
			if(!w->asInteger(iW) || !h->asInteger(iH))
				WRONG_TYPE("array(integer,integer)")
			m_pObject->setProperty(szName.toUtf8().data(), QVariant(QSize(iW, iH)));
		}
		break;
		case QVariant::Rect:
		{
			if(!v->isArray())
				WRONG_TYPE("array(integer,integer,integer,integer)")
			KviKvsArray * a = v->array();
			KviKvsVariant * x = a->at(0);
			KviKvsVariant * y = a->at(1);
			KviKvsVariant * w = a->at(2);
			KviKvsVariant * h = a->at(3);
			if(!x || !y || !w || !h)
				WRONG_TYPE("array(integer,integer,integer,integer)")
			kvs_int_t iX, iY, iW, iH;
			if(!x->asInteger(iX) || !y->asInteger(iY) || !w->asInteger(iW) || !h->asInteger(iH))
				WRONG_TYPE("array(integer,integer,integer,integer)")
			m_pObject->setProperty(szName.toUtf8().data(), QVariant(QRect(iX, iY, iW, iH)));
		}
		break;

		case QVariant::Color:
		{
			if(!v->isArray())
				WRONG_TYPE("array(integer,integer,integer)")
			KviKvsArray * a = v->array();
			KviKvsVariant * r = a->at(0);
			KviKvsVariant * g = a->at(1);
			KviKvsVariant * b = a->at(3);
			if(!r || !g || !b)
				WRONG_TYPE("array(integer,integer,integer)")
			kvs_int_t iR, iG, iB;
			if(!r->asInteger(iR) || !g->asInteger(iG) || !b->asInteger(iB))
				WRONG_TYPE("array(integer,integer,integer)")
			m_pObject->setProperty(szName.toUtf8().data(), QVariant(QColor(iR, iG, iB)));
		}
		break;
		case QVariant::Font:
		{
			if(!v->isArray())
				WRONG_TYPE("array(string,integer,string)")
			KviKvsArray * a = v->array();
			KviKvsVariant * ff = a->at(0);
			KviKvsVariant * ps = a->at(1);
			KviKvsVariant * fl = a->at(3);
			if(!ff || !ps)
				WRONG_TYPE("array(string,integer,string)")
			kvs_int_t iPs;
			if(!ps->asInteger(iPs))
				WRONG_TYPE("array(string,integer,string)")
			QString szFf, szFl;
			ff->asString(szFf);
			if(fl)
				fl->asString(szFl);
			QFont fnt;
			fnt.setFamily(szFf);
			fnt.setPointSize(iPs);
			if(szFl.indexOf('b', Qt::CaseInsensitive) != -1)
				fnt.setBold(true);
			if(szFl.indexOf('i', Qt::CaseInsensitive) != -1)
				fnt.setItalic(true);
			if(szFl.indexOf('u', Qt::CaseInsensitive) != -1)
				fnt.setUnderline(true);
			if(szFl.indexOf('o', Qt::CaseInsensitive) != -1)
				fnt.setOverline(true);
			if(szFl.indexOf('f', Qt::CaseInsensitive) != -1)
				fnt.setFixedPitch(true);
			if(szFl.indexOf('s', Qt::CaseInsensitive) != -1)
				fnt.setStrikeOut(true);
			m_pObject->setProperty(szName.toUtf8().data(), QVariant(fnt));
		}
		break;
		case QVariant::Pixmap | QVariant::Icon:
		{
			if(v->isHObject())
			{
				if(v->hobject() == (kvs_hobject_t) nullptr)
				{
					// null pixmap
					if(vv.type() == QVariant::Pixmap)
						m_pObject->setProperty(szName.toUtf8().data(), QVariant(QPixmap()));
					else
						m_pObject->setProperty(szName.toUtf8().data(), QVariant(QIcon()));
				}
				else
				{
					KviKvsObject * pix = KviKvsKernel::instance()->objectController()->lookupObject(v->hobject());
					if(!pix->inherits("KviScriptPixmapObject"))
						c->warning(__tr2qs_ctx("A pixmap object, an image_id or an image file path is required for this property", "kvs"));
					else
					{
						QVariant pixv = pix->property("pixmap");
						if(vv.type() == QVariant::Pixmap)
							m_pObject->setProperty(szName.toUtf8().data(), QVariant(pixv.value<QPixmap>()));
						else
							m_pObject->setProperty(szName.toUtf8().data(), QVariant(QIcon(pixv.value<QPixmap>())));
					}
				}
			}
			else
			{
				QString szStr;
				v->asString(szStr);
				QPixmap * pPix = g_pIconManager->getImage(szStr);
				if(pPix)
				{
					if(vv.type() == QVariant::Pixmap)
						m_pObject->setProperty(szName.toUtf8().data(), QVariant(*pPix));
					else
						m_pObject->setProperty(szName.toUtf8().data(), QVariant(QIcon(*pPix)));
				}
				else
					c->warning(__tr2qs_ctx("Can't find the requested image", "kvs"));
			}
		}

		break;

		default:
			c->warning(__tr2qs_ctx("Property '%Q' for object named '%Q' of class '%Q' has an unsupported data type", "kvs"), &szName, &m_szName, &(m_pClass->name()));
			c->returnValue()->setNothing();
			break;
	}
	return true;
}

bool KviKvsObject::function_property(KviKvsObjectFunctionCall * c)
{
	QString szName;
	bool bNoerror;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("propertyName", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSO_PARAMETER("bNowarning", KVS_PT_BOOL, KVS_PF_OPTIONAL, bNoerror)
	KVSO_PARAMETERS_END(c)

	if(!m_pObject)
	{
		// there are no Qt properties at all
		c->returnValue()->setNothing();
		if(!bNoerror)
			c->warning(__tr2qs_ctx("The object named '%Q' of class '%Q' has no Qt properties", "kvs"), &m_szName, &(m_pClass->name()));
		return true;
	}

	int idx = m_pObject->metaObject()->indexOfProperty(szName.toUtf8().data());
	if(idx < 0)
	{
		c->returnValue()->setNothing();
		if(!bNoerror)
			c->warning(__tr2qs_ctx("No Qt property named '%Q' for object named '%Q' of class '%Q'", "kvs"), &szName, &m_szName, &(m_pClass->name()));
		return true;
	}
	QMetaProperty prop = m_pObject->metaObject()->property(idx);
	const QMetaProperty * p = &prop;
	if(!p->isValid())
	{
		c->returnValue()->setNothing();
		c->warning(__tr2qs_ctx("Can't find property named '%Q' for object named '%Q' of class '%Q': the property is indexed but it doesn't really exist", "kvs"), &szName, &m_szName, &(m_pClass->name()));
		return true;
	}

	QVariant v = m_pObject->property(szName.toUtf8().data());
	if(!v.isValid())
	{
		c->returnValue()->setNothing();
		c->warning(__tr2qs_ctx("Can't find property named '%Q' for object named '%Q' of class '%Q': the property is indexed and defined but the returned variant is not valid", "kvs"), &szName, &m_szName, &(m_pClass->name()));
		return true;
	}

	if(p->isEnumType())
	{
		qDebug("Enum type prop %d %s", v.toInt(), p->enumerator().valueToKey(v.toInt()));
		c->returnValue()->setString(p->enumerator().valueToKey(v.toInt()));
		return true;
	}

	switch(v.type())
	{
		case QVariant::Int:
			c->returnValue()->setInteger((kvs_int_t)v.toInt());
			break;
		case QVariant::Double:
			c->returnValue()->setReal((kvs_int_t)v.toDouble());
			break;
		case QVariant::UInt:
			c->returnValue()->setInteger((kvs_int_t)v.toUInt());
			break;
		case QVariant::Bool:
			c->returnValue()->setBoolean(v.toBool());
			break;
		case QVariant::String:
			c->returnValue()->setString(v.toString());
			break;
		case QVariant::ByteArray:
			c->returnValue()->setString(QString::fromUtf8(v.toByteArray().data()));
			break;
		case QVariant::Point:
		{
			QPoint p = v.toPoint();
			KviKvsArray * a = new KviKvsArray();
			a->set(0, new KviKvsVariant((kvs_int_t)p.x()));
			a->set(1, new KviKvsVariant((kvs_int_t)p.y()));
			c->returnValue()->setArray(a);
		}
		break;
		case QVariant::Size:
		{
			QSize p = v.toSize();
			KviKvsArray * a = new KviKvsArray();
			a->set(0, new KviKvsVariant((kvs_int_t)p.width()));
			a->set(1, new KviKvsVariant((kvs_int_t)p.height()));
			c->returnValue()->setArray(a);
		}
		break;
		case QVariant::Rect:
		{
			QRect p = v.toRect();
			KviKvsArray * a = new KviKvsArray();
			a->set(0, new KviKvsVariant((kvs_int_t)p.x()));
			a->set(1, new KviKvsVariant((kvs_int_t)p.y()));
			a->set(2, new KviKvsVariant((kvs_int_t)p.width()));
			a->set(3, new KviKvsVariant((kvs_int_t)p.height()));
			c->returnValue()->setArray(a);
		}
		break;
		case QVariant::Color:
		{
			QColor clr = v.value<QColor>();
			KviKvsArray * a = new KviKvsArray();
			a->set(0, new KviKvsVariant((kvs_int_t)clr.red()));
			a->set(1, new KviKvsVariant((kvs_int_t)clr.green()));
			a->set(2, new KviKvsVariant((kvs_int_t)clr.blue()));
			c->returnValue()->setArray(a);
		}
		break;
		case QVariant::Font:
		{
			QFont f = v.value<QFont>();
			KviKvsArray * a = new KviKvsArray();
			a->set(0, new KviKvsVariant(f.family()));
			a->set(1, new KviKvsVariant((kvs_int_t)f.pointSize()));
			QString szFlags;
			if(f.bold())
				szFlags += "b";
			if(f.underline())
				szFlags += "u";
			if(f.overline())
				szFlags += "o";
			if(f.strikeOut())
				szFlags += "s";
			if(f.fixedPitch())
				szFlags += "f";
			if(f.italic())
				szFlags += "i";
			a->set(2, new KviKvsVariant(szFlags));
			c->returnValue()->setArray(a);
		}
		break;
		default:
			if(bNoerror)
				c->returnValue()->setString("Unsupported_data_type");
			else
			{
				c->warning(__tr2qs_ctx("Property '%Q' for object named '%Q' of class '%Q' has an unsupported data type", "kvs"), &szName, &m_szName, &(m_pClass->name()));
				c->returnValue()->setNothing();
			}
			break;
	}
	return true;
}

void KviKvsObject::killAllChildrenWithClass(KviKvsObjectClass * cl)
{
	KviPointerList<QPointer<KviKvsObject>> lDying;
	lDying.setAutoDelete(true);

	KviKvsObject * pObject;

	QPointer<KviKvsObject> guard(this);

	for(pObject = m_pChildList->first(); pObject; pObject = m_pChildList->next())
	{
		if(pObject->getClass() == cl)
		{
			lDying.append(new QPointer<KviKvsObject>(pObject));
		}
		else
		{
			pObject->killAllChildrenWithClass(cl);
			if(guard.isNull())
				break; // argh.. circular delete
		}
	}

	for(QPointer<KviKvsObject> * pObject = lDying.first(); pObject; pObject = lDying.next())
	{
		if(pObject->isNull())
			continue; // already dead ?
		(*pObject)->dieNow();
	}
}

bool KviKvsObject::inheritsClass(const QString & szClass)
{
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClass);
	if(pClass)
		return inheritsClass(pClass);
	else
		return false;
}
bool KviKvsObject::inheritsClass(KviKvsObjectClass * pClass)
{
	if(pClass == m_pClass)
		return true;
	KviKvsObjectClass * cl = m_pClass->m_pParentClass;
	while(cl)
	{
		if(cl == pClass)
			return true;
		else
			cl = cl->m_pParentClass;
	}
	return false;
}

KviKvsObjectClass * KviKvsObject::getClass(const QString & classOverride)
{
	if(classOverride.isEmpty())
		return m_pClass;
	KviKvsObjectClass * cl = m_pClass; // class override can be also THIS class
	// if object->$function() is a local override, class::object->$function()
	// is the class member function (not the local override)
	while(cl)
	{
		if(KviQString::equalCI(cl->name(), classOverride))
			break;
		else
			cl = cl->m_pParentClass;
	}
	return cl;
}

KviKvsObjectFunctionHandler * KviKvsObject::lookupFunctionHandler(const QString & funcName, const QString & classOverride)
{
	KviKvsObjectFunctionHandler * h = nullptr;

	if(classOverride.isEmpty() && m_pFunctionHandlers)
	{
		// lookup the local overrides
		h = m_pFunctionHandlers->find(funcName);
	}

	if(!h)
	{
		// not a local override function... lookup in the class
		KviKvsObjectClass * cl = getClass(classOverride);
		if(cl)
			return cl->lookupFunctionHandler(funcName);
	}

	return h;
}

bool KviKvsObject::die()
{
	if(m_bAboutToDie)
		return false; // hum.. recursive death attempt :D

	if(m_bInDelayedDeath)
		return false; // we're alreadyi dying soon, dude, no need to repeat it over and over again...

	m_bInDelayedDeath = true;

	QTimer::singleShot(0, this, SLOT(delayedDie()));
	return true;
}

bool KviKvsObject::dieNow()
{
	if(m_bAboutToDie)
		return false; // hum.. recursive death attempt :D

	m_bAboutToDie = true;

	KVI_ASSERT(!m_bDestructorCalled);
	callDestructor();
	delete this;

	return true;
}

void KviKvsObject::delayedDie()
{
	KVI_ASSERT(m_bInDelayedDeath); // must be true: never call it directly
	KVI_ASSERT(!m_bAboutToDie);    // if this is true something is wrong...

	m_bAboutToDie = true;

	KVI_ASSERT(!m_bDestructorCalled);
	callDestructor();
	delete this; // byez!
}

void KviKvsObject::setObject(QObject * o, bool bIsOwned)
{
	m_bObjectOwner = bIsOwned;
	m_pObject = o;
	o->installEventFilter(this);
	connect(m_pObject, SIGNAL(destroyed()), this, SLOT(objectDestroyed()));
}

void KviKvsObject::objectDestroyed()
{
	m_pObject = nullptr;
	die();
}

bool KviKvsObject::eventFilter(QObject *, QEvent *)
{
	return false; // do not stop
}

void KviKvsObject::timerEvent(QTimerEvent * e)
{
	KviKvsVariant * v = new KviKvsVariant();
	v->setInteger(e->timerId());
	KviKvsVariantList parms(v);

	callFunction(this, "timerEvent", &parms);
}

bool KviKvsObject::callFunction(KviKvsObject * pCaller, const QString & fncName, KviKvsVariant * pRetVal, KviKvsVariantList * pParams)
{
	KviKvsVariant rv;
	if(!pRetVal)
		pRetVal = &rv;
	KviKvsRunTimeContext ctx(nullptr, g_pApp->activeConsole(), KviKvsKernel::instance()->emptyParameterList(), pRetVal, nullptr);
	if(!pParams)
		pParams = KviKvsKernel::instance()->emptyParameterList();
	return callFunction(pCaller, fncName, QString(), &ctx, pRetVal, pParams);
}

bool KviKvsObject::callFunction(KviKvsObject * pCaller, const QString & fncName, KviKvsVariantList * pParams)
{
	KviKvsVariant fakeRetVal;
	return callFunction(pCaller, fncName, &fakeRetVal, pParams);
}

bool KviKvsObject::callFunction(
    KviKvsObject * pCaller,
    const QString & fncName,
    const QString & classOverride,
    KviKvsRunTimeContext * pContext,
    KviKvsVariant * pRetVal,
    KviKvsVariantList * pParams)
{
	KviKvsObjectFunctionHandler * h = lookupFunctionHandler(fncName, classOverride);

	if(!h)
	{
		if(classOverride.isEmpty())
			pContext->error(__tr2qs_ctx("Can't find object function $%Q for object named '%Q' of class '%Q'", "kvs"), &fncName, &m_szName, &(getClass()->name()));
		else
			pContext->error(__tr2qs_ctx("Can't find object function $%Q::%Q for object named '%Q' of class '%Q'", "kvs"), &classOverride, &fncName, &m_szName, &(getClass()->name()));
		return false;
	}

	if(h->flags() & KviKvsObjectFunctionHandler::Internal)
	{
		if(pCaller != this)
		{
			pContext->error(__tr2qs_ctx("Can't call internal object function $%Q (for object named '%Q' of class '%Q') from this context", "kvs"), &fncName, &m_szName, &(getClass()->name()));
			return false;
		}
	}

	KviKvsObjectFunctionCall fc(pContext, pParams, pRetVal);

	return h->call(this, &fc);
}

void KviKvsObject::registerPrivateImplementation(const QString & szFunctionName, const QString & szCode)
{
	if(szCode.isEmpty())
	{
		if(m_pFunctionHandlers)
		{
			m_pFunctionHandlers->remove(szFunctionName);
			if(m_pFunctionHandlers->isEmpty())
			{
				delete m_pFunctionHandlers;
				m_pFunctionHandlers = nullptr;
			}
		}
	}
	else
	{
		if(!m_pFunctionHandlers)
		{
			m_pFunctionHandlers = new KviPointerHashTable<QString, KviKvsObjectFunctionHandler>(7, false);
			m_pFunctionHandlers->setAutoDelete(true);
		}

		QString szContext = m_pClass->name();
		szContext += "[privateimpl]::";
		szContext += szFunctionName;

		m_pFunctionHandlers->replace(szFunctionName, new KviKvsObjectScriptFunctionHandler(szContext, szCode, QString("")));
	}
}

KviKvsObject * KviKvsObject::findChild(const QString & szClass, const QString & szName)
{
	for(KviKvsObject * o = m_pChildList->first(); o; o = m_pChildList->next())
	{
		if(szClass.isEmpty())
		{
			// any class matches
			if(szName.isEmpty())
				return o; // any name matches
			// name must match
			if(KviQString::equalCI(szName, o->objectName()))
				return o;
		}
		else
		{
			if(KviQString::equalCI(szClass, o->getClass()->name()))
			{
				if(szName.isEmpty())
					return o; // any name matches
				// name must match
				if(KviQString::equalCI(szName, o->objectName()))
					return o;
			}
		}
		KviKvsObject * c = o->findChild(szClass, szName);
		if(c)
			return c;
	}
	return nullptr;
}
