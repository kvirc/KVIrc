//=============================================================================
//
//   File : toolbareditor.cpp
//   Creation date : Wed 01 Dec 2004 14:42:20 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "toolbareditor.h"

#include "kvi_app.h"
#include "kvi_actiondrawer.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_actionmanager.h"
#include "kvi_customtoolbar.h"
#include "kvi_customtoolbarmanager.h"
#include "kvi_customtoolbardescriptor.h"
#include "kvi_imagedialog.h"
#include "kvi_fileutils.h"
#include "kvi_filedialog.h"
#include "kvi_kvs_useraction.h"
#include "kvi_tal_tooltip.h"
#include "kvi_fileextensions.h"

#include <QPushButton>
#include <QLayout>
#include <QApplication>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QFrame>
#include <QDir>
#include <QTimer>
#include <QEvent>
#include <QDropEvent>
#include <QDesktopWidget>

KviCustomizeToolBarsDialog * KviCustomizeToolBarsDialog::m_pInstance = 0;
extern QRect g_rectToolBarEditorDialogGeometry;



KviTrashcanLabel::KviTrashcanLabel(QWidget * p)
: QLabel(p)
{
	setPixmap(*(g_pIconManager->getBigIcon("kvi_bigicon_trashcan.png")));
	KviTalToolTip::add(this,__tr2qs_ctx("Drop here the icons from the toolbars to remove them","editor"));
	setFrameStyle(QFrame::Sunken | QFrame::WinPanel);
	setAcceptDrops(true);
	setAlignment(Qt::AlignCenter);
	setMinimumSize(40,40);
	m_uFlashCount = 0;
	m_pFlashTimer = 0;
	m_clrOriginal =  palette().color(backgroundRole());
	setAutoFillBackground(true); //needed for flashing
	connect(KviActionManager::instance(),SIGNAL(removeActionsHintRequest()),this,SLOT(flash()));
}

KviTrashcanLabel::~KviTrashcanLabel()
{
	if(m_pFlashTimer)
	{
		m_pFlashTimer->stop();
		delete m_pFlashTimer;
	}
}

void KviTrashcanLabel::flash()
{
	m_uFlashCount = 0;
	if(m_pFlashTimer)return;
	m_pFlashTimer = new QTimer();
	connect(m_pFlashTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));
	m_pFlashTimer->start(200);
}

void KviTrashcanLabel::heartbeat()
{
	m_uFlashCount++;
	if(m_uFlashCount % 2){
		QPalette p = palette();
		p.setColor(backgroundRole(),QColor(0,0,0));
		setPalette(p);
	}
	else{
		QPalette p = palette();
		p.setColor(backgroundRole(),m_clrOriginal);
		setPalette(p);
	}
	update();
	if(m_uFlashCount == 8)
	{
		m_pFlashTimer->stop();
		delete m_pFlashTimer;
		m_pFlashTimer = 0;
	}
}

void KviTrashcanLabel::dropEvent(QDropEvent * e)
{
	if(e->mimeData()->hasText()) e->acceptProposedAction();
}

void KviTrashcanLabel::dragEnterEvent(QDragEnterEvent * e)
{
	if(e->mimeData()->hasText()) e->acceptProposedAction();
}


KviCustomToolBarPropertiesDialog::KviCustomToolBarPropertiesDialog(QWidget * p,const QString &szText,const QString &szId,const QString &szLabel,const QString &szIconId)
: QDialog(p)
{
	m_szId = szId;
	m_szOriginalId = szId;
	m_szLabel = szLabel;

	setWindowTitle(__tr2qs_ctx("ToolBar Properties","editor"));
	setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TOOLBAR))));
	setModal(true);

	QGridLayout * g = new QGridLayout(this);

	QLabel * l = new QLabel(szText,this);
	g->addWidget(l,0,0,1,6);

	l = new QLabel(__tr2qs_ctx("Label","editor") + ":",this);
	g->addWidget(l,1,0);

	m_pLabelEdit = new QLineEdit(this);
	g->addWidget(m_pLabelEdit,1,1,1,5);
	m_pLabelEdit->setText(szLabel);
	connect(m_pLabelEdit,SIGNAL(textChanged(const QString &)),this,SLOT(labelTextChanged(const QString &)));

	l = new QLabel(__tr2qs_ctx("Icon","editor") + ":",this);
	g->addWidget(l,2,0);

	m_pIconEdit = new QLineEdit(this);
	m_pIconEdit->setReadOnly(true);
	g->addWidget(m_pIconEdit,2,1,1,4);

	m_pIconButton = new QPushButton(this);
	g->addWidget(m_pIconButton,2,5,1,1);
	connect(m_pIconButton,SIGNAL(clicked()),this,SLOT(iconButtonClicked()));

	iconSelected(szIconId);

	m_pAdvanced = new QWidget(this);
	QGridLayout * ag = new QGridLayout(m_pAdvanced);

	l = new QLabel(__tr2qs_ctx("Id","editor") + ":",m_pAdvanced);
	l->setMinimumWidth(100);
	ag->addWidget(l,0,0);

	m_pIdEdit = new QLineEdit(m_pAdvanced);
	ag->addWidget(m_pIdEdit,0,1);
	ag->setRowStretch(0,1);

	m_pIdEdit->setText(szId);

	g->addWidget(m_pAdvanced,3,0,1,6);
	m_pAdvanced->hide();

	m_pLabelEdit->setFocus();

	QPushButton * pb = new QPushButton(__tr2qs_ctx("OK","editor"),this);
	connect(pb,SIGNAL(clicked()),this,SLOT(okClicked()));
	pb->setMinimumWidth(80);
	g->addWidget(pb,4,4,1,2);

	pb = new QPushButton(__tr2qs_ctx("Cancel","editor"),this);
	connect(pb,SIGNAL(clicked()),this,SLOT(reject()));
	pb->setMinimumWidth(80);
	g->addWidget(pb,4,3);

	m_pAdvancedButton = new QPushButton(__tr2qs_ctx("Advanced...","editor"),this);
	connect(m_pAdvancedButton,SIGNAL(clicked()),this,SLOT(advancedClicked()));
	m_pAdvancedButton->setMinimumWidth(100);
	g->addWidget(m_pAdvancedButton,4,0,1,2);

	g->setRowStretch(0,1);
	g->setColumnStretch(2,1);
}

KviCustomToolBarPropertiesDialog::~KviCustomToolBarPropertiesDialog()
{
}

void KviCustomToolBarPropertiesDialog::iconSelected(const QString &szIconId)
{
	QPixmap * pix = g_pIconManager->getImage(szIconId);

	if(pix)
	{
		m_pIconButton->setIcon(QIcon(*pix));
		m_szIconId = szIconId;
		m_pIconEdit->setText(szIconId);
	} else {
		m_pIconButton->setText("...");
		m_szIconId = "";
		m_pIconEdit->setText("");
	}
}

void KviCustomToolBarPropertiesDialog::iconButtonClicked()
{
	KviImageDialog * dlg = new KviImageDialog(this,__tr2qs_ctx("Please choose the icon for the ToolBar","editor"));
	if(dlg->exec() != QDialog::Accepted)
	{
		delete dlg;
		return;
	}
	QString s = dlg->selectedImage();
	delete dlg;
	iconSelected(s);
}

void KviCustomToolBarPropertiesDialog::labelTextChanged(const QString &s)
{
	if(m_szOriginalId.isEmpty())
	{
		QString szId = KviCustomToolBarManager::instance()->idForNewToolBar(s);
		m_pIdEdit->setText(szId);
	}
}

void KviCustomToolBarPropertiesDialog::okClicked()
{
	if(m_szLabel.isEmpty())
	{
		QMessageBox::information(this,__tr2qs_ctx("Invalid ToolBar Label","editor"),__tr2qs_ctx("The ToolBar Label can't be empty!","editor"),__tr2qs_ctx("OK","editor"));
		return;
	}

	if(m_szId.isEmpty())
	{
		m_szId = KviCustomToolBarManager::instance()->idForNewToolBar(m_szLabel);
	}

	if(KviCustomToolBarManager::instance()->find(m_szId))
	{
		if(m_szId != m_szOriginalId)
		{
			if(QMessageBox::information(this,__tr2qs_ctx("Duplicate ToolBar Id","editor"),
				__tr2qs_ctx("The specified ToolBar Id already exists.<br>" \
					"Would you like KVIrc to assign it automatically (so it doesn't "
					"collide with any other toolbar) or you prefer to do it manually ?","editor"),
				__tr2qs_ctx("Manually","editor"),__tr2qs_ctx("Automatically","editor")) == 0)return;
			m_szId = KviCustomToolBarManager::instance()->idForNewToolBar(m_szLabel);
		}
	}

	m_szId = m_pIdEdit->text();
	m_szLabel = m_pLabelEdit->text();
	accept();
}

void KviCustomToolBarPropertiesDialog::advancedClicked()
{
	if(m_pAdvanced->isVisible())
	{
		m_pAdvanced->hide();
		m_pAdvancedButton->setText(__tr2qs_ctx("Advanced...","editor"));
	} else {
		m_pAdvanced->show();
		m_pAdvancedButton->setText(__tr2qs_ctx("Hide Advanced","editor"));
	}

}


KviCustomizeToolBarsDialog::KviCustomizeToolBarsDialog(QWidget * p)
: QDialog(p)
{
	setObjectName("Toolbar_editor");
	setWindowTitle(__tr2qs_ctx("Customize Toolbars","editor"));
	setWindowIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TOOLBAR)));

	m_pInstance = this;

	QGridLayout * g = new QGridLayout(this);
	m_pDrawer = new KviActionDrawer(this);
	g->addWidget(m_pDrawer,0,0,7,1);

	QPushButton * b = new QPushButton(__tr2qs_ctx("New ToolBar","editor"),this);
	connect(b,SIGNAL(clicked()),this,SLOT(newToolBar()));
	g->addWidget(b,0,1);

	m_pDeleteToolBarButton = new QPushButton(__tr2qs_ctx("Delete ToolBar","editor"),this);
	connect(m_pDeleteToolBarButton,SIGNAL(clicked()),this,SLOT(deleteToolBar()));
	g->addWidget(m_pDeleteToolBarButton,1,1);

	m_pRenameToolBarButton = new QPushButton(__tr2qs_ctx("Edit ToolBar","editor"),this);
	connect(m_pRenameToolBarButton,SIGNAL(clicked()),this,SLOT(renameToolBar()));
	g->addWidget(m_pRenameToolBarButton,2,1);

	QFrame * f = new QFrame(this);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addWidget(f,3,1);

	m_pExportToolBarButton = new QPushButton(__tr2qs_ctx("Export ToolBar","editor"),this);
	connect(m_pExportToolBarButton,SIGNAL(clicked()),this,SLOT(exportToolBar()));
	g->addWidget(m_pExportToolBarButton,4,1);

	KviTrashcanLabel * t = new KviTrashcanLabel(this);
	g->addWidget(t,6,1);

	b = new QPushButton(__tr2qs_ctx("Close","editor"),this);
	connect(b,SIGNAL(clicked()),this,SLOT(closeClicked()));
	g->addWidget(b,7,1);

	g->setRowStretch(5,1);
	g->setColumnStretch(0,1);

	m_pDrawer->fill();

	connect(KviActionManager::instance(),SIGNAL(currentToolBarChanged()),this,SLOT(currentToolBarChanged()));
	KviActionManager::instance()->customizeToolBarsDialogCreated();

	currentToolBarChanged();

	if(g_rectToolBarEditorDialogGeometry.y() < 5)
	{
		g_rectToolBarEditorDialogGeometry.setY(5);
	}
	//setGeometry(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry));
	resize(g_rectToolBarEditorDialogGeometry.width(),
		g_rectToolBarEditorDialogGeometry.height());
	move(g_rectToolBarEditorDialogGeometry.x(),
		g_rectToolBarEditorDialogGeometry.y());
}

KviCustomizeToolBarsDialog::~KviCustomizeToolBarsDialog()
{
	g_rectToolBarEditorDialogGeometry = QRect(pos().x(),pos().y(),size().width(),size().height());

	KviActionManager::instance()->customizeToolBarsDialogDestroyed();
	m_pInstance = 0;
}

void KviCustomizeToolBarsDialog::currentToolBarChanged()
{
	m_pDeleteToolBarButton->setEnabled(KviActionManager::instance()->currentToolBar());
	m_pRenameToolBarButton->setEnabled(KviActionManager::instance()->currentToolBar());
}

void KviCustomizeToolBarsDialog::showEvent(QShowEvent * e)
{
	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	move((rect.width() - width())/2,(rect.height() - height())/2);

	QDialog::showEvent(e);
}

void KviCustomizeToolBarsDialog::deleteToolBar()
{
	KviCustomToolBar * t = KviActionManager::currentToolBar();
	if(!t)return;
	if(QMessageBox::question(this,
			__tr2qs_ctx("Confirm ToolBar Deletion","editor"),
			__tr2qs_ctx("Do you really want to delete toolbar \"%1\" ?","editor").arg(t->windowTitle()),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No) != QMessageBox::Yes)return;
	KviCustomToolBarManager::instance()->destroyDescriptor(t->descriptor()->id());
}

void KviCustomizeToolBarsDialog::exportToolBar()
{
	KviCustomToolBar * t = KviActionManager::currentToolBar();
	if(!t)return;

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += t->descriptor()->id();
	szName += ".kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs_ctx("Choose a Filename - KVIrc","editor"),szName,KVI_FILTER_SCRIPT,true,true,true))return;

	QString szCode;

	int ret = QMessageBox::question(this,
			__tr2qs_ctx("ToolBar Export","editor"),
			__tr2qs_ctx("Do you want the associated actions to be exported with the toolbar ?","editor"),
			__tr2qs_ctx("Yes","editor"),
			__tr2qs_ctx("No","editor"),
			__tr2qs_ctx("Cancel","editor"));

	if(ret == 2)return;

	bool bExportActions = ret == 0;

	if(bExportActions)
	{
		KviPointerList<QString> * a = t->descriptor()->actions();
		if(a)
		{
			for(QString * s = a->first();s;s = a->next())
			{
				KviAction * act = KviActionManager::instance()->getAction(*s);
				if(act)
				{
					if(act->isKviUserActionNeverOverrideThis())
					{
						((KviKvsUserAction *)act)->exportToKvs(szCode);
						szCode += "\n\n";
					}
				}
			}
		}
	}

	szCode += "toolbar.create ";
	szCode += t->descriptor()->id();
	szCode += " ";
	szCode += t->descriptor()->labelCode();
	szCode += " ";
	szCode += t->descriptor()->iconId();
	szCode += "\n";

	KviPointerList<QString> * aa = t->descriptor()->actions();
	if(aa)
	{
		for(QString * ss = aa->first();ss;ss = aa->next())
		{
			szCode += "toolbar.additem ";
			szCode += t->descriptor()->id();
			szCode += " ";
			szCode += *ss;
			szCode += "\n";
		}
	}

	szCode += "toolbar.show ";
	szCode += t->descriptor()->id();
	szCode += "\n";

	if(!KviFileUtils::writeFile(szFile,szCode))
	{
		QMessageBox::warning(this,__tr2qs_ctx("Write Failed - KVIrc","editor"),__tr2qs_ctx("Unable to write to the toolbar file.","editor"),__tr2qs_ctx("OK","editor"));
	}
}

void KviCustomizeToolBarsDialog::renameToolBar()
{
	KviCustomToolBar * t = KviActionManager::currentToolBar();
	if(!t)return;

	KviCustomToolBarPropertiesDialog * dlg = new KviCustomToolBarPropertiesDialog(this,
		__tr2qs_ctx("Please specify the properties for the toolbar \"%1\"","editor").arg(t->windowTitle()),
		t->descriptor()->id(),
		t->descriptor()->labelCode(),
		t->descriptor()->iconId());

	dlg->show();
	if(dlg->exec() != QDialog::Accepted)
	{
		delete dlg;
		return;
	}

	QString id = dlg->id();
	QString label = dlg->label();
	QString icon = dlg->iconId();
	delete dlg;

	if((id == t->descriptor()->id()) && (label == t->descriptor()->labelCode()) && (icon == t->descriptor()->iconId()))return;

	KviCustomToolBarManager::instance()->renameDescriptor(t->descriptor()->id(),id,label);
	t->descriptor()->setIconId(icon);
}

void KviCustomizeToolBarsDialog::newToolBar()
{
	KviCustomToolBarPropertiesDialog * dlg = new KviCustomToolBarPropertiesDialog(this,
		__tr2qs_ctx("Please specify the properties for the new toolbar","editor"),
		KviCustomToolBarManager::instance()->idForNewToolBar(__tr2qs_ctx("My ToolBar","editor")),
		__tr2qs_ctx("My ToolBar","editor"));

	dlg->show();
	if(dlg->exec() != QDialog::Accepted)
	{
		delete dlg;
		return;
	}

	QString id = dlg->id();
	QString label = dlg->label();
	QString icon = dlg->iconId();
	delete dlg;

	KviCustomToolBarDescriptor * d = KviCustomToolBarManager::instance()->create(id,label);
	d->setIconId(icon);
	KviCustomToolBar * t = d->createToolBar();
	KviActionManager::instance()->setCurrentToolBar(t);
}

void KviCustomizeToolBarsDialog::closeClicked()
{
	delete this;
}

void KviCustomizeToolBarsDialog::cleanup()
{
	if(!m_pInstance)return;
	delete m_pInstance;
	m_pInstance = 0;
}

void KviCustomizeToolBarsDialog::display()
{
	if(m_pInstance)return;
	m_pInstance = new KviCustomizeToolBarsDialog(g_pFrame);
	m_pInstance->show();
}

void KviCustomizeToolBarsDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	delete this;
}

void KviCustomizeToolBarsDialog::reject()
{
	cleanup();
}
