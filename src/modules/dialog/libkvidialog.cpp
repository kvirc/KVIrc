//=============================================================================
//
//   File : libkvidialog.cpp
//   Creation date : Sat Sep 15 2001 01:13:25 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "libkvidialog.h"

#include "KviLocale.h"
#include "KviModule.h"
#include "KviModuleManager.h"
#include "KviError.h"
#include "KviApplication.h"
#include "KviIconManager.h"
#include "KviConsoleWindow.h"
#include "KviIconManager.h"
#include "KviKvsScript.h"
#include "KviMessageBox.h"
#include "KviTalHBox.h"

#include <QMessageBox>
#include <QLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDesktopWidget>
#include <QEvent>
#include <QCloseEvent>

#include <QTextEdit>

static KviPointerList<QWidget> * g_pDialogModuleDialogList;

KviKvsCallbackMessageBox::KviKvsCallbackMessageBox(
    const QString & szCaption,
    const QString & szText,
    const QString & szIcon,
    const QString & szButton0,
    const QString & szButton1,
    const QString & szButton2,
    const QString & szCode,
    KviKvsVariantList * pMagicParams,
    KviWindow * pWindow, bool modal)
    : QMessageBox(nullptr),
      KviKvsCallbackObject("dialog.message", pWindow, szCode, pMagicParams, 0)
{
	setObjectName("dialog_message");
	setWindowTitle(szCaption);
	setText(szText);
	setIcon(QMessageBox::NoIcon);
	setModal(modal);
	QMessageBox::StandardButtons buttons;
	bool btn = false;
	if(!szButton0.isEmpty())
	{
		btn = true;
		buttons = QMessageBox::Yes;
	}
	if(!szButton1.isEmpty())
	{
		btn = true;
		buttons |= QMessageBox::No;
	}
	if(!szButton2.isEmpty())
	{
		btn = true;
		buttons |= QMessageBox::Cancel;
	}

	if(!btn)
		buttons = QMessageBox::Ok;
	setStandardButtons(buttons);
	setDefaultButton(QMessageBox::Yes);
	if(szButton2.isEmpty())
		setEscapeButton(QMessageBox::No);
	else
		setEscapeButton(QMessageBox::Cancel);

	g_pDialogModuleDialogList->append(this);

	QPixmap * pix = g_pIconManager->getImage(szIcon);

	if(pix)
		setIconPixmap(*pix);
	else
	{
		if(KviQString::equalCI(szIcon, "information"))
			setIcon(QMessageBox::Information);
		else if(KviQString::equalCI(szIcon, "warning"))
			setIcon(QMessageBox::Warning);
		else if(KviQString::equalCI(szIcon, "critical"))
			setIcon(QMessageBox::Critical);
	}
	if(!szButton0.isEmpty())
		setButtonText(QMessageBox::Yes, szButton0);
	if(!szButton1.isEmpty())
		setButtonText(QMessageBox::No, szButton1);
	if(!szButton2.isEmpty())
		setButtonText(QMessageBox::Cancel, szButton2);
}

KviKvsCallbackMessageBox::~KviKvsCallbackMessageBox()
{
	g_pDialogModuleDialogList->removeRef(this);
}

void KviKvsCallbackMessageBox::done(int code)
{
	QMessageBox::done(code);

	kvs_int_t iVal = 0;

	switch(code)
	{
		case QMessageBox::No:
			iVal = 1;
			break;
		case QMessageBox::Cancel:
			iVal = 2;
			break;
		case 0:
			// user closed the dialog, fake an "escape button" press
			if(standardButtons() & QMessageBox::Cancel)
				iVal = 2;
			else
				iVal = 1;
			break;
	}

	KviKvsVariantList params;
	params.append(new KviKvsVariant(iVal));

	execute(&params);
	deleteLater();
}

/*
	@doc: dialog.message
	@type:
		command
	@title:
		dialog.message
	@short:
		Shows a message box
	@syntax:
		dialog.message [-b] (<caption>,<message_text>,<icon>,<button0>[,<button1>[,<button2>[,<magic1>[,<magic2>[...]]]]])
		{
			<callback_command>
		}
	@description:
		Shows a message dialog box with the specified <caption>, <message_text>, <icon> and
		buttons.[br]
		<caption> is a text string that will appear in the caption of the dialog box.[br]
		<message_text> is a text string that will appear in the dialog box and can contain HTML formatting.[br]
		<icon> is an [doc:image_id]image identifier[/doc] that defines an icon to be placed in the dialog box.
		<icon> can be a relative or absolute path to an image file, a signed number (in that case it defines
		an internal KVIrc image) or one of the special strings [i]critical[/i], [i]information[/i] and [i]warning[/i].[br]
		<button0> is the text of the first button (on the left).[br]
		<button1> is the text of the second button (if empty or specified, only one button will appear in the dialog).[br]
		<button2> is the text of the third button (if empty or specified, only two buttons will appear in the dialog).[br]
		The first button is the default button - it is activated when the user presses the
		enter key. The third (or the second if only two buttons are present) is treated as the escape button
		and is activated when the user presses the Esc key or closes the dialog with the window manager close button.[br]
		If one of the button text strings starts with a [i]default=[/i] prefix then that button is assumed
		to be the default button of the dialog.[br]
		If one of the button text strings starts with a [i]escape=[/i] prefix then that button is assumed
		to be the escape button of the dialog.[br]
		<magic1>, <magic2>... are the magic parameters - evaluated at dialog.message call time and passed
		to the <callback_command> as positional parameters.[br]
		If the -b or -modal switch is specified the dialog will have blocking modal behaviour -
		it will appear above its parent widget and block its input until the dialog is closed.[br]
		Once the dialog is displayed, the user will click one of the buttons. At this point the dialog
		is hidden and the <callback_command> is executed, passing the number of the button clicked
		as $0 and the magic parameters as positional parameters $1, $2, $3.[br]
	@examples:
		[example]
		[comment]# Just a warning dialog[/comment]
		dialog.message("Warning","You're being <b>warned</b>",warning,"OK"){ echo The user clicked OK; }
		[comment]# A question[/comment]
		dialog.message("And now?","What do you want to do?",information,"Go home","Watch TV","Scream")
		{
			if($0 == 0)echo "The user wants to go home"
			else if($0 == 1)echo "The user wants to watch TV"
			else echo "The user wants to scream!"
		}
		[/example]
*/

static bool dialog_kvs_cmd_message(KviKvsModuleCallbackCommandCall * c)
{
	QString szCaption, szMessage, szIcon, szButton0, szButton1, szButton2;
	KviKvsVariantList params;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("caption", KVS_PT_STRING, 0, szCaption)
	KVSM_PARAMETER("message", KVS_PT_STRING, 0, szMessage)
	KVSM_PARAMETER("icon", KVS_PT_STRING, 0, szIcon)
	KVSM_PARAMETER("button0", KVS_PT_STRING, KVS_PF_OPTIONAL, szButton0)
	KVSM_PARAMETER("button1", KVS_PT_STRING, KVS_PF_OPTIONAL, szButton1)
	KVSM_PARAMETER("button2", KVS_PT_STRING, KVS_PF_OPTIONAL, szButton2)
	KVSM_PARAMETER("magic", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL, params)
	KVSM_PARAMETERS_END(c)
	bool modal;
	if(c->hasSwitch('b', "modal"))
		modal = true;
	else
		modal = false;
	QString szCmd = c->callback()->code();

	KviKvsCallbackMessageBox * box = new KviKvsCallbackMessageBox(
	    szCaption, szMessage, szIcon, szButton0, szButton1, szButton2, szCmd, &params, c->window(), modal);
	box->show();

	return true;
}

KviKvsCallbackTextInput::KviKvsCallbackTextInput(
    const QString & szCaption,
    const QString & szLabel,
    const QString & szDefaultText,
    const QString & szIcon,
    bool bMultiLine,
    bool bPassword,
    const QString & szButton0,
    const QString & szButton1,
    const QString & szButton2,
    const QString & szCode,
    KviKvsVariantList * pMagicParams,
    KviWindow * pWindow, bool modal)
    : QDialog(), KviKvsCallbackObject("dialog.textinput", pWindow, szCode, pMagicParams, 0)
{
	setObjectName("dialog_textinput");
	g_pDialogModuleDialogList->append(this);
	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::KVIrc)));
	setModal(modal);
	setWindowTitle(szCaption);

	QGridLayout * g = new QGridLayout(this);

	QPixmap * pix = g_pIconManager->getImage(szIcon);

	if(pix)
	{
		QLabel * il = new QLabel(this);
		il->setPixmap(*pix);
		il->setAlignment(Qt::AlignCenter);
		g->addWidget(il, 0, 0);
		QLabel * tl = new QLabel(szLabel, this);
		g->addWidget(tl, 0, 1);
	}
	else
	{
		QLabel * tl = new QLabel(szLabel, this);
		g->addWidget(tl, 0, 0, 1, 2);
	}

	g->setColumnStretch(1, 1);

	m_bMultiLine = bMultiLine;
	m_bPassword = bPassword;

	if(m_bMultiLine)
	{
		m_pEdit = new QTextEdit(this);
		((QTextEdit *)m_pEdit)->setPlainText(szDefaultText);
		((QTextEdit *)m_pEdit)->selectAll();
	}
	else
	{
		m_pEdit = new QLineEdit(this);
		if(m_bPassword)
			((QLineEdit *)m_pEdit)->setEchoMode(QLineEdit::Password);
		((QLineEdit *)m_pEdit)->setText(szDefaultText);
		((QLineEdit *)m_pEdit)->selectAll();
	}

	g->addWidget(m_pEdit, 1, 1, 1, 1);

	KviTalHBox * box = new KviTalHBox(this);
	g->addWidget(box, 2, 1, 1, 2);

	m_iEscapeButton = -1;
	m_iDefaultButton = 0;

	if(!szButton0.isEmpty())
	{
		QString szB = szButton0;
		bool bDef = false;
		if(KviQString::equalCIN(szB, "default=", 8))
		{
			bDef = true;
			szB.remove(0, 8);
			m_iDefaultButton = 0;
		}
		else if(KviQString::equalCIN(szB, "escape=", 7))
		{
			szB.remove(0, 7);
			m_iEscapeButton = 0;
		}
		QPushButton * pb1 = new QPushButton(szB, box);
		if(bDef)
			pb1->setDefault(true);
		connect(pb1, SIGNAL(clicked()), this, SLOT(b0Clicked()));
	}

	if(!szButton1.isEmpty())
	{
		QString szB = szButton1;
		bool bDef = false;
		if(KviQString::equalCIN(szB, "default=", 8))
		{
			bDef = true;
			szB.remove(0, 8);
			m_iDefaultButton = 1;
		}
		else if(KviQString::equalCIN(szB, "escape=", 7))
		{
			szB.remove(0, 7);
			m_iEscapeButton = 1;
		}
		QPushButton * pb2 = new QPushButton(szB, box);
		if(bDef)
			pb2->setDefault(true);
		connect(pb2, SIGNAL(clicked()), this, SLOT(b1Clicked()));
	}

	if(!szButton2.isEmpty())
	{
		QString szB = szButton2;
		bool bDef = false;
		if(KviQString::equalCIN(szB, "default=", 8))
		{
			bDef = true;
			szB.remove(0, 8);
			m_iDefaultButton = 2;
		}
		else if(KviQString::equalCIN(szB, "escape=", 7))
		{
			szB.remove(0, 7);
			m_iEscapeButton = 2;
		}
		QPushButton * pb3 = new QPushButton(szB, box);
		if(bDef)
			pb3->setDefault(true);
		connect(pb3, SIGNAL(clicked()), this, SLOT(b2Clicked()));
	}

	if(m_iEscapeButton < 0)
	{
		//no escape button explicitly set, search for one:
		if(!szButton2.isEmpty())
			m_iEscapeButton = 2;
		else if(!szButton1.isEmpty())
			m_iEscapeButton = 1;
		else
			m_iEscapeButton = 0;
	}
}

KviKvsCallbackTextInput::~KviKvsCallbackTextInput()
{
	g_pDialogModuleDialogList->removeRef(this);
}

void KviKvsCallbackTextInput::b0Clicked()
{
	done(0 + 10);
}

void KviKvsCallbackTextInput::b1Clicked()
{
	done(1 + 10);
}

void KviKvsCallbackTextInput::b2Clicked()
{
	done(2 + 10);
}

void KviKvsCallbackTextInput::closeEvent(QCloseEvent * e)
{
	e->ignore();
	done(m_iEscapeButton + 10);
}

void KviKvsCallbackTextInput::done(int code)
{
	if(code >= 10)
	{
		code -= 10;
	}
	else
	{
		switch(code)
		{
			case QDialog::Accepted:
				code = m_iDefaultButton;
				break;
			default:
				code = m_iEscapeButton;
				break;
		}
	}

	QString txt;

	if(m_bMultiLine)
	{
		txt = ((QTextEdit *)m_pEdit)->toPlainText();
	}
	else
	{
		txt = ((QLineEdit *)m_pEdit)->text();
	}

	KviKvsVariantList params;
	params.append(new KviKvsVariant((kvs_int_t)code));
	params.append(new KviKvsVariant(txt));

	execute(&params);

	//QDialog::done(code);

	deleteLater();
}

void KviKvsCallbackTextInput::showEvent(QShowEvent * e)
{
	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	move((rect.width() - width()) / 2, (rect.height() - height()) / 2);

	QDialog::showEvent(e);
}

/*
	@doc: dialog.textinput
	@type:
		command
	@title:
		dialog.textinput
	@short:
		Shows a dialog that accepts user input as text
	@syntax:
		dialog.textinput [-d=<default text>] [-i=<icon>] [-m] [-b] [-p] (<caption>,<info_text>,<button0>[,<button1>[,<button2>[,<magic1>[,<magic2>[...]]]]])
		{
			<callback_command>
		}
	@switches:
		!sw: -d=<default_text> | --default=<default_text>
		Set the initial text input value to <default_text>
		!sw: -i=<icon> | --icon=<icon>
		Display the specified icon, to the left of the informational text
		!sw: -m | --multiline
		Input multi-line text instead of single line
		!sw: -p | --password
		Display asterisks instead of the characters actually entered
	@description:
		Shows a text input dialog box with the specified <caption>, <info_text>, <icon> and
		buttons.[br]
		<caption> is a text string that will appear in the caption of the dialog box.[br]
		<info_text> is a fixed text string that will appear in the dialog box and can contain HTML formatting.[br]
		<button0> is the text of the first button (on the left).[br]
		<button1> is the text of the second button (if empty or not given at all, only one button will appear in the dialog).[br]
		<button2> is the text of the third button (if empty or not given, only two buttons will appear in the dialog).[br]
		The first button is the default button - it is activated when the user presses the
		enter key. The third (or the second if only two buttons are present) is treated as the escape button
		and is activated when the user presses the Esc key or closes the dialog with the window manager close button.[br]
		If one of the button text strings starts with a [i]default=[/i] prefix then that button is assumed
		to be the default button of the dialog.[br]
		If one of the button text strings starts with a [i]escape=[/i] prefix then that button is assumed
		to be the escape button of the dialog.[br]
		If the -m switch is used, the dialog will be a multi-line text input, otherwise the user will be able to
		input only a single line of text.[br]
		If the -p switch is used, the text will be show as asterisks, useful for sensitive data (passwords).[br]
		If the -d switch is used, the initial text input value is set to <default text>.[br]
		If the -i switch is used, the dialog displays also the icon <icon> to the left of <info_text>.
		<icon> is an image identifier (a relative or absolute path to an image file, or a signed number that maps to an internal KVIrc image). [br]
		If the -b or -modal switch is specified the dialog will have blocking modal behaviour:
		it will appear above its parent widget and block its input until it's closed.[br]
		In that case <icon> is an [doc:image_id]image identifier[/doc] (can be a relative or absolute
		path to an image file or a signed number (in that case it defines an internal KVIrc image).[br]
		<magic1>,<magic2>... are the magic parameters: evaluated at dialog.textinput call time and passed
		to the <callback_command> as positional parameters.[br]
		Once the dialog has been shown, the user will click one of the buttons. At this point the dialog
		is hidden and the <callback_command> is executed passing the text input value in $1, the number of the button clicked
		as $0, and the magic parameters as positional parameters $2, $3, $4....[br]
	@examples:
		[example]
		[comment]# We need a single line reason[/comment]
		dialog.textinput -d="Working!" (Away,Please enter the away message,"OK","Cancel")
		{
			switch($0)
			{
				case(0):
					away $1-
				break;
				default:
					# Cancelled
				break;
			}
		}
		[/example]
*/

static bool dialog_kvs_cmd_textinput(KviKvsModuleCallbackCommandCall * c)
{
	QString szCaption, szInfoText, szIcon, szDefaultText, szButton0, szButton1, szButton2;
	KviKvsVariantList params;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("caption", KVS_PT_STRING, 0, szCaption)
	KVSM_PARAMETER("info_text", KVS_PT_STRING, 0, szInfoText)
	KVSM_PARAMETER("button0", KVS_PT_STRING, KVS_PF_OPTIONAL, szButton0)
	KVSM_PARAMETER("button1", KVS_PT_STRING, KVS_PF_OPTIONAL, szButton1)
	KVSM_PARAMETER("button2", KVS_PT_STRING, KVS_PF_OPTIONAL, szButton2)
	KVSM_PARAMETER("magic", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL, params)
	KVSM_PARAMETERS_END(c)

	QString szCmd = c->callback()->code();

	c->switches()->getAsStringIfExisting('i', "icon", szIcon);
	c->switches()->getAsStringIfExisting('d', "default", szDefaultText);
	bool modal;
	if(c->hasSwitch('b', "modal"))
		modal = true;
	else
		modal = false;
	KviKvsCallbackTextInput * box = new KviKvsCallbackTextInput(
	    szCaption, szInfoText, szDefaultText, szIcon, c->switches()->find('m', "multiline"), c->switches()->find('p', "password"),
	    szButton0, szButton1, szButton2, szCmd, &params, c->window(), modal);
	box->show();

	return true;
}

KviKvsCallbackFileDialog::KviKvsCallbackFileDialog(
    const QString & szCaption,
    const QString & szInitialSelection,
    const QString & szFilter,
    const QString & szCode,
    KviKvsVariantList * pMagicParams,
    KviWindow * pWindow, bool modal)
    : KviFileDialog(
          szInitialSelection,
          szFilter,
          nullptr, // parent
          nullptr,
          modal),
      KviKvsCallbackObject("dialog.file", pWindow, szCode, pMagicParams, 0)
{
	g_pDialogModuleDialogList->append(this);
	setWindowTitle(szCaption);
	setObjectName("dialog_file");
}

KviKvsCallbackFileDialog::~KviKvsCallbackFileDialog()
{
	g_pDialogModuleDialogList->removeRef(this);
}

void KviKvsCallbackFileDialog::done(int code)
{
	KviFileDialog::done(code);
	KviKvsVariantList params;

	if(code == QDialog::Accepted)
	{
#ifdef COMPILE_KDE4_SUPPORT
		if(mode() == KFile::ExistingOnly)
#else
		if(fileMode() == QFileDialog::ExistingFiles)
#endif
		{
			KviKvsArray * a = new KviKvsArray();
			QStringList sl = selectedFiles();
			int idx = 0;
			for(auto & it : sl)
			{
				a->set(idx, new KviKvsVariant(it));
				idx++;
			}
			params.append(new KviKvsVariant(a));
		}
		else
		{
			params.append(new KviKvsVariant(selectedFiles().at(0)));
		}
	}
	else
	{
		params.append(new KviKvsVariant(QString("")));
	}

	hide(); // ensure we're hidden

	execute(&params);
	deleteLater();
}

/*
	@doc: dialog.file
	@type:
		command
	@title:
		dialog.file
	@short:
		Shows a file dialog
	@syntax:
		dialog.file [-b] (<mode>,<caption>[,<initial_selection[,<file_filter>[,<magic1>[,<magic2>[...]]]]]])
		{
			<callback_command>
		}
	@description:
		Shows an open file dialog box with the specified <caption>, <initial_selection>, and <file_filter>.[br]
		<mode> can be [i]open[/i], [i]openm[/i], [i]save[/i] or [i]dir[/i]:[br]
		[b]open[/b] causes the dialog to return an existing file[br]
		[b]openm[/b] is similar to open but allows returning multiple files as a comma separated list[br]
		[b]save[/b] causes the dialog to return any file name (no overwrite confirmation is built in the dialog!)[br]
		[b]dir[/b] causes the dialog to return an existing directory name[br]
		<mode> defaults to [i]open[/i].[br]
		<caption> is a text string that will appear in the caption of the dialog box.[br]
		<initial_selection> can be a directory or filename that will be initially selected in the dialog.[br]
		Only files matching <file_filter> are selectable. If filter is an empty string, all files are selectable.[br]
		In the filter string multiple filters can be specified separated by either two semicolons next to each
		other or separated by newlines. To add two filters, one to show all C++ files and one to show all
		header files, the filter string could look like [i]C++ Files (*.cpp *.cc *.C *.cxx *.c++);;Header Files (*.h *.hxx *.h++)[/i]
		<magic1>, <magic2>... are the magic parameters: evaluated at dialog.message call time and passed
		to the <callback_command> as positional parameters.[br]
		If the -b or -modal switch is specified the dialog will have non-blocking modal behaviour:
		it will appear above its parent widget and block its input until it's closed.[br]
		Once the dialog has been shown, the user will select an EXISTING file and click either
		Ok or Cancel. At this point the dialog is hidden and the <callback_command> is executed passing the selected file(s) as $0
		and the magic parameters as positional parameters $1, $2, $3....[br]
		If the user clicks [i]Cancel[/i] or does not select any file the positional parameter $0 will be empty.[br]
	@examples:
		[example]
			dialog.file(open,Choose an audio file,/home/pragma/TheAudio.au,"Audio files (*.au *.wav *.snd)")
			{
				if("$0" != "")run play $0
			}
		[/example]
*/

//#warning "Examples for these dialogs!"

static bool dialog_kvs_cmd_file(KviKvsModuleCallbackCommandCall * c)
{
	QString szMode, szCaption, szInitialSelection, szFilter;
	KviKvsVariantList params;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mode", KVS_PT_STRING, 0, szMode)
	KVSM_PARAMETER("caption", KVS_PT_STRING, 0, szCaption)
	KVSM_PARAMETER("initial_selection", KVS_PT_STRING, KVS_PF_OPTIONAL, szInitialSelection)
	KVSM_PARAMETER("filter", KVS_PT_STRING, KVS_PF_OPTIONAL, szFilter)
	KVSM_PARAMETER("magic", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL, params)
	KVSM_PARAMETERS_END(c)

	bool modal = c->hasSwitch('b', "modal");

	QString szCmd = c->callback()->code();

	KviKvsCallbackFileDialog * box = new KviKvsCallbackFileDialog(szCaption, szInitialSelection, szFilter, szCmd, &params, c->window(), modal);

	KviFileDialog::FileMode md = KviFileDialog::ExistingFile;

	if(KviQString::equalCI(szMode, "open"))
		md = KviFileDialog::ExistingFiles;
	else if(KviQString::equalCI(szMode, "save"))
		md = KviFileDialog::AnyFile;
	else if(KviQString::equalCI(szMode, "dir"))
		md = KviFileDialog::DirectoryOnly;

	box->setFileMode(md);

	box->show();

	return true;
}

KviKvsCallbackImageDialog::KviKvsCallbackImageDialog(
    const QString & szCaption,
    const QString & szInitialSelection,
    int iType,
    int iMaxSize,
    const QString & szCode,
    KviKvsVariantList * pMagicParams,
    KviWindow * pWindow, bool modal)
    : KviImageDialog(nullptr, szCaption, iType, 0, szInitialSelection, iMaxSize, modal), KviKvsCallbackObject("dialog.image", pWindow, szCode, pMagicParams, 0)
{
	g_pDialogModuleDialogList->append(this);
	setObjectName("dialog_image");
}

KviKvsCallbackImageDialog::~KviKvsCallbackImageDialog()
{
	g_pDialogModuleDialogList->removeRef(this);
}

void KviKvsCallbackImageDialog::done(int code)
{
	KviImageDialog::done(code);
	KviKvsVariantList params;

	if(code == QDialog::Accepted)
	{
		params.append(new KviKvsVariant(selectedImage()));
	}
	else
	{
		params.append(new KviKvsVariant(QString("")));
	}

	hide(); // ensure we're hidden

	execute(&params);
	deleteLater();
}

/*
	@doc: dialog.image
	@type:
		command
	@title:
		dialog.image
	@short:
		Shows a image dialog
	@syntax:
		dialog.image [-b] (<type>,<caption>,<initial_directory>,[<maxsize>,[,<magic1>[,<magic2>[...]]]]]])
		{
			<callback_command>
		}
	@description:
		Shows a dialog that allows selecting an [doc:image_id]image_id[doc].
		The <type> parameter must be a combination of the following flags:<br>
		[b]s[/b] : allow selecting from the KVIrc builtin small icons<br>
		[b]f[/b] : allow browsing the local directories<br>
		[b]a[/b] : all of the above<br>
		The default for <type> is 'a'.<br>
		<caption> is the caption string for the dialog.<br>
		<initial_directory> makes sense only if 'f' is specified (if <initial_directory> is empty
		then the last path used by the image dialog will be used).<br>
		<maxsize> is the maximum size of the images for that the preview will be generated:
		this is 256000 bytes by default (if unspecified). Don't make it a lot bigger: it can take a lot to make
		the thumbnails for bigger images (and it can eat a considerable amount of memory).<br>
		<magic1>, <magic2>... are the magic parameters: evaluated at dialog.image call time and passed
		to the <callback_command> as positional parameters.[br]
		If the -b or -modal switch is specified the dialog will have non-blocking modal behaviour:
		it will appear above its parent widget and block its input until it's closed.[br]
		Once the dialog has been shown, the user will select an [b]existing[/b] file and click either
		OK or Cancel. At this point the dialog is hidden and the <callback_command> is executed passing the selected file(s) as $0
		and the magic parameters as positional parameters $1, $2, $3....[br]
		If the user clicks [i]Cancel[/i] or does not select any image the positional parameter $0 will be empty.[br]
	@examples:
		[example]
			dialog.image(f,Choose an image file,/home/pragma/,"256000")
			{
				if("$0" != "")run okular $0
			}
		[/example]
*/

//#warning "Examples for these dialogs!"

static bool dialog_kvs_cmd_image(KviKvsModuleCallbackCommandCall * c)
{
	QString szType, szCaption, szInitialSelection;
	kvs_uint_t iMaxSize;
	KviKvsVariantList params;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("mode", KVS_PT_STRING, 0, szType)
	KVSM_PARAMETER("caption", KVS_PT_STRING, 0, szCaption)
	KVSM_PARAMETER("initial_directory", KVS_PT_STRING, 0, szInitialSelection)
	KVSM_PARAMETER("maxsize", KVS_PT_UINT, KVS_PF_OPTIONAL, iMaxSize)
	KVSM_PARAMETER("magic", KVS_PT_VARIANTLIST, KVS_PF_OPTIONAL, params)
	KVSM_PARAMETERS_END(c)
	bool modal;
	if(c->hasSwitch('b', "modal"))
		modal = true;
	else
		modal = false;
	QString szCmd = c->callback()->code();

	int iType = 0;

	if(szType.contains('s'))
		iType |= KID_TYPE_BUILTIN_IMAGES_SMALL;
	if(szType.contains('f'))
		iType |= KID_TYPE_FULL_PATH;
	if(szType.isEmpty())
		iType = KID_TYPE_ALL;

	if(iMaxSize < 1)
		iMaxSize = 256000;

	KviKvsCallbackImageDialog * box = new KviKvsCallbackImageDialog(szCaption, szInitialSelection, iType, iMaxSize, szCmd, &params, c->window(), modal);

	box->show();

	return true;
}

/*
	@doc: dialog.yesno
	@type:
		function
	@title:
		$dialog.yesno
	@short:
		Shows a simple yes/no dialog
	@syntax:
		$dialog.yesno(<caption:string>,<szText:string>)
	@description:
		Shows a simple yes/no dialog. Returns 1 if user clicks [i]Yes[/i] and 0 if (s)he clicks [i]No[/i].
		Please note that this dialog is [b]blocking[/b]: it blocks execution of the script
		until the user has selected either YES or NO.
	@examples:
	@seealso:
*/

static bool dialog_kvs_fnc_yesno(KviKvsModuleFunctionCall * c)
{
	QString szCaption;
	QString szText;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("caption", KVS_PT_STRING, 0, szCaption)
	KVSM_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSM_PARAMETERS_END(c)

	c->enterBlockingSection();
	bool yes = KviMessageBox::yesNo(szCaption, szText); // this will happily crash on quit ?
	if(!c->leaveBlockingSection())
		return true; // just die
	c->returnValue()->setBoolean(yes);
	return true;
}

/*
	@doc: noblockingdialogs
	@type:
		generic
	@title:
		Why are there no blocking dialogs in KVIrc?
	@short:
		Technical answer
	@description:
		Why are there no blocking dialogs in KVIrc?[br]
		The answer is simple: because they're more confusing and tricky than it seems.[br]
		Blocking the entire program control flow while showing a dialog is
		rather a bad idea since we have to deal with external entities (servers and other users)
		that are [b]not[/b] blocked. This means that the blocking dialogs must block only the
		script control-flow but let the rest of the application running.
		Such blocking dialogs actually seem to simplify scripting because
		the programmer [i]feels[/i] that the control is always left in the script snippet that he is writing.
		This is actually confusing: the control IS in the script snippet but while the dialog
		is open the whole world can change: you can return from the dialog call and discover
		that the server connection no longer exists and the application is about to quit.[br]
		This may happen even with non-blocking dialogs,but in non-blocking mode you have
		a way to handle this event. Consider the following snippet of code:
		[example]
			echo My name is $?
		[/example]
		Where $? stands for a blocking input dialog that asks the user for some text.[br]
		When the input dialog returns the window that the echo was directed to no longer
		exists and you have no way to stop the echo! (Well... I could add extra code
		in the executable to handle all these situations but that would be really too expensive).[br]
		With object scripting this is actually dangerous: you might use a blocking dialog
		in an object signal handler and when returning discover that this object has been deleted!
		(The example refers to a simple object, but think about a complex hierarchy of objects
		where one random gets deleted...).[br]
		This is why the dialogs in KVIrc are non-blocking :)[br]
		That's REAL programming.
*/

static bool dialog_module_init(KviModule * m)
{
	g_pDialogModuleDialogList = new KviPointerList<QWidget>;
	g_pDialogModuleDialogList->setAutoDelete(false);

	KVSM_REGISTER_CALLBACK_COMMAND(m, "message", dialog_kvs_cmd_message);
	KVSM_REGISTER_CALLBACK_COMMAND(m, "textinput", dialog_kvs_cmd_textinput);
	KVSM_REGISTER_CALLBACK_COMMAND(m, "file", dialog_kvs_cmd_file);
	KVSM_REGISTER_CALLBACK_COMMAND(m, "image", dialog_kvs_cmd_image);
	KVSM_REGISTER_FUNCTION(m, "yesno", dialog_kvs_fnc_yesno);

	return true;
}

static bool dialog_module_cleanup(KviModule *)
{
	// Here we get a tragedy if g_iLocalEventLoops > 0!
	while(g_pDialogModuleDialogList->first())
		delete g_pDialogModuleDialogList->first();
	delete g_pDialogModuleDialogList;
	g_pDialogModuleDialogList = nullptr;
	return true;
}

static bool dialog_module_can_unload(KviModule *)
{
	return g_pDialogModuleDialogList->isEmpty();
}

KVIRC_MODULE(
    "KVIrc script dialogs",
    "4.0.0",
    "Szymon Stefanek <pragma at kvirc dot net>",
    "Adds the /dialog.* commands functionality\n",
    dialog_module_init,
    dialog_module_can_unload,
    0,
    dialog_module_cleanup,
    0)
