//=============================================================================
//
//   File : KvsObject_colorDialog.cpp
//   Creation date : Wed 27 Apr 2011 15:42:05 CEST by Alessandro Carbone
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KvsObject_colorDialog.h"

#include <QColorDialog>

const char * const option_tbl[] = {
	"ShowAlphaChannel",
	"NoButtons",
	"DontUseNativeDialog"
};

const int option_cod[] = {
	QColorDialog::ShowAlphaChannel,
	QColorDialog::NoButtons,
	QColorDialog::DontUseNativeDialog
};

#define option_num (sizeof(option_tbl) / sizeof(option_tbl[0]))

/*
	@doc:	colordialog
	@title:
		colordialog class
	@type:
		class
	@short:
		Check box with a text description
	@inherits:
		[class]object[/class]
		[class]button[/class]
	@description:
		This widget provides a check box - it is a kind of a toggle
		button. It can have two states: on (checked) and off
		(unchecked).
	@functions:
		!fn: <bool> $isChecked()
		Returns [b]1[/b] if the check box is checked, and [b]0[/b] otherwise.[br]
		See also [classfnc]$setChecked[/classfnc]().
		!fn: $setChecked([<bChecked:bool>])
		Sets the check box 'checked state' to <bool>.
		See also [classfnc]$isChecked[/classfnc]().
		!fn: $setText([<text:string>])
		Sets the label text associated with the colorDialog to <text>.[br]
		!fn: $toggleEvent(<bToggled:bool>)
		Called by KVIrc when the colorDialog state is toggled.
		The default implementation emits the toggled(<bool>) signal.
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_colorDialog, "colorDialog", "dialog")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_colorDialog, setCurrentColor)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_colorDialog, setOptions)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_colorDialog, colorSelectedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_colorDialog, currentColorChangedEvent)
KVSO_END_REGISTERCLASS(KvsObject_colorDialog)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_colorDialog, KvsObject_dialog)

KVSO_END_CONSTRUCTOR(KvsObject_colorDialog)

KVSO_BEGIN_DESTRUCTOR(KvsObject_colorDialog)

KVSO_END_CONSTRUCTOR(KvsObject_colorDialog)

bool KvsObject_colorDialog::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	QColorDialog * d = new QColorDialog(parentScriptWidget());
	d->setObjectName(getName().toUtf8().data());
	setObject(d, true);
	connect(d, SIGNAL(colorSelected(const QColor &)), this, SLOT(slotColorSelected(const QColor &)));
	connect(d, SIGNAL(currentColorChanged(const QColor &)), this, SLOT(slotCurrentColorChanged(const QColor &)));
	return true;
}

KVSO_CLASS_FUNCTION(colorDialog, setCurrentColor)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iCol1, iCol2, iCol3, iOpacity;
	QString szColorMode, szColor;
	KviKvsVariant *var1, *var2, *var3;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("Color_1_Or_Colorname", KVS_PT_VARIANT, 0, var1)
	KVSO_PARAMETER("Color_2", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var2)
	KVSO_PARAMETER("Colo3_3", KVS_PT_VARIANT, KVS_PF_OPTIONAL, var3)
	KVSO_PARAMETER("opacity", KVS_PT_INT, KVS_PF_OPTIONAL, iOpacity)
	KVSO_PARAMETER("color_mode", KVS_PT_STRING, KVS_PF_OPTIONAL, szColorMode)
	KVSO_PARAMETERS_END(c)
	QColor col;
	if(!var1->asInteger(iCol1))
	{
		var1->asString(szColor);
		if(c->paramCount() < 2)
			iOpacity = 255;
		else
		{
			if(!var2->asInteger(iOpacity))
			{
				c->warning(__tr2qs_ctx("The opacity parameter didn't evaluate to integer", "objects"));
				return true;
			}
		}
		col.setNamedColor(szColor);
		col.setAlpha(iOpacity);
	}
	else
	{
		if(c->paramCount() < 3)
		{
			c->error(__tr2qs_ctx("Color name or triplet RGB/HSV value required", "objects"));
			return true;
		}
		if(!var2->asInteger(iCol2) || !var3->asInteger(iCol3))
		{
			c->error(__tr2qs_ctx("One of the triplet parameters didn't evaluate to an integer", "objects"));
			return true;
		}
		if(c->paramCount() < 4)
			iOpacity = 255;
		else
		{
			if(c->paramCount() > 4)
			{
				if(KviQString::equalCI(szColorMode, "HSV"))
					col.setHsv(iCol1, iCol2, iCol3, iOpacity);
				else
					col.setRgb(iCol1, iCol2, iCol3, iOpacity);
			}
		}
		col.setAlpha(iOpacity);
	}
	((QColorDialog *)widget())->setCurrentColor(col);
	return true;
}

KVSO_CLASS_FUNCTION(colorDialog, setOptions)
{
	CHECK_INTERNAL_POINTER(widget())
	QStringList szOptions;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("szOptions", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, szOptions)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	int colorDialogOption, sum = 0;
	for(auto & szOption : szOptions)
	{
		colorDialogOption = 0;
		for(unsigned int j = 0; j < option_num; j++)
		{
			if(KviQString::equalCI(szOption, option_tbl[j]))
			{
				colorDialogOption = option_cod[j];
				break;
			}
		}
		if(colorDialogOption)
			sum = sum | colorDialogOption;
		else
			c->warning(__tr2qs_ctx("Unknown szOptions '%Q'", "objects"), &szOption);
	}
	((QColorDialog *)widget())->setOptions(QColorDialog::ColorDialogOptions(sum));
	return true;
}

KVSO_CLASS_FUNCTION(colorDialog, currentColorChangedEvent)
{
	emitSignal("currentColorChanged", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(colorDialog, colorSelectedEvent)
{
	emitSignal("colorSelected", c, c->params());
	return true;
}

//slots
void KvsObject_colorDialog::slotCurrentColorChanged(const QColor & col)
{
	KviKvsHash * pHash = new KviKvsHash();
	KviKvsVariant * pColName = new KviKvsVariant(col.name());
	KviKvsVariant * pColAlpha = new KviKvsVariant((kvs_int_t)col.alpha());
	pHash->set("color", pColName);
	pHash->set("opacity", pColAlpha);
	KviKvsVariantList params(new KviKvsVariant(pHash));
	callFunction(this, "currentColorChangedEvent", &params);
}

void KvsObject_colorDialog::slotColorSelected(const QColor & col)
{
	KviKvsHash * pHash = new KviKvsHash();
	KviKvsVariant * pColName = new KviKvsVariant(col.name());
	KviKvsVariant * pColAlpha = new KviKvsVariant((kvs_int_t)col.alpha());
	pHash->set("color", pColName);
	pHash->set("alpha", pColAlpha);
	KviKvsVariantList params(new KviKvsVariant(pHash));
	callFunction(this, "colorSelectedEvent", &params);
}
