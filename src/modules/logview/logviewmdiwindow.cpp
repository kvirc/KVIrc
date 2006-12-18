//
//   File : logviewmdiwindow.cpp
//   Creation date : Tue Apr 23 2002 18:08:22 by Juanjo �varez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "logviewmdiwindow.h"
#include "logviewwidget.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_module.h"
#include "kvi_options.h"
#include "kvi_frame.h"
#include "kvi_ircview.h"
#include "kvi_qstring.h"
#include "kvi_app.h"
#include "kvi_fileutils.h"

#include <qpixmap.h>
#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qlistview.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qpopupmenu.h>
#include <qcursor.h>

#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif
#include <qtextcodec.h>

extern KviLogViewMDIWindow * g_pLogViewWindow;

KviLogViewMDIWindow::KviLogViewMDIWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_LOGVIEW,lpFrm,"logview"), KviModuleExtension(d)
{
	g_pLogViewWindow = this;
//	m_pLogViewWidget = new KviLogViewWidget(this);
	m_pSplitter = new QSplitter(QSplitter::Horizontal,this,"main_splitter");
	
	m_pTabWidget = new QTabWidget(m_pSplitter);
	
	m_pIndexTab  = new QVBox(m_pTabWidget);
	m_pTabWidget->insertTab(m_pIndexTab,__tr2qs_ctx("Index","logview"));
	m_pListView = new QListView(m_pIndexTab);
	m_pListView->addColumn(__tr2qs_ctx("Log File","logview"),135);
	m_pListView->setColumnWidthMode(0,QListView::Maximum);
	m_pListView->setAllColumnsShowFocus(true);
	m_pListView->setMultiSelection(false);
	m_pListView->setShowSortIndicator(true);
	m_pListView->setRootIsDecorated(true);
	connect(m_pListView,SIGNAL(selectionChanged(QListViewItem *)),this,SLOT(itemSelected(QListViewItem *)));
	connect(m_pListView,SIGNAL(rightButtonClicked ( QListViewItem * , const QPoint &, int )),this,SLOT(rightButtonClicked ( QListViewItem * , const QPoint &, int )));
	
	//m_pSearchTab  = new QVBox(m_pTabWidget);
	//m_pTabWidget->insertTab(m_pSearchTab,__tr2qs_ctx("Search","logview"));

	m_pIrcView = new KviIrcView(m_pSplitter,g_pFrame,this);
	m_pIrcView->setFocusPolicy(QWidget::ClickFocus);
	
	QValueList<int> li;
	li.append(110);
	li.append(width()-110);
	m_pSplitter->setSizes(li);
	
	g_pApp->getLocalKvircDirectory(m_szLogDirectory,KviApp::Log);
	KviQString::ensureLastCharIs(m_szLogDirectory,'/'); // Does this work on Windows?
	
	oneTimeSetup();
}



KviLogViewMDIWindow::~KviLogViewMDIWindow()
{
    g_pLogViewWindow = 0;
}

QPixmap * KviLogViewMDIWindow::myIconPtr()
{
    return g_pIconManager->getSmallIcon(KVI_SMALLICON_LOG);
}


void KviLogViewMDIWindow::resizeEvent(QResizeEvent *e)
{
	m_pSplitter->setGeometry(0,0,width(),height());
}

void KviLogViewMDIWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Log Viewer","logview");

	m_szHtmlActiveCaption = QString("<nobr><font color=\"%1\"><b>").arg(KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name().ascii());
	m_szHtmlActiveCaption.append(m_szPlainTextCaption);
	m_szHtmlActiveCaption.append("</b></font></nobr>");

	m_szHtmlInactiveCaption = QString("<nobr><font color=\"%1\"><b>").arg(KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name().ascii());
	m_szHtmlInactiveCaption.append(m_szPlainTextCaption);
	m_szHtmlInactiveCaption.append("</b></font></nobr>");
}

void KviLogViewMDIWindow::die()
{
	close();
}

QSize KviLogViewMDIWindow::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(),m_pIrcView->sizeHint().height());
	return ret;
}

void KviLogViewMDIWindow::oneTimeSetup()
{
    m_pListView->clear();
    KviStr tmp, lastTypeToken, lastNameToken;
    QString szFname;
    KviStr typeToken, nameToken, dateToken;
    lastTypeToken = "";
    lastNameToken = "";

    KviLogListViewItemType * typeNode;
    KviLogListViewItemType * lastTypeNode = 0;
    KviLogListViewItem * nameNode;
    KviLogListViewItem * lastNameNode = 0;
    KviLogListViewLog * logNode;

    QStringList m_pFileNames = getFileNames();
    m_pFileNames.sort();

    for(QStringList::Iterator it = m_pFileNames.begin(); it != m_pFileNames.end(); ++it)
    {
        szFname=(*it);
        QFileInfo fi(szFname);
        if(fi.extension(false)=="tmp")
        	continue;
#ifdef COMPILE_ZLIB_SUPPORT
		if(fi.extension(false)=="gz")
        	tmp = fi.baseName(true);
        else
#endif
        	tmp = szFname;
        typeToken = tmp.getToken('_');
        // Ignore non-logs files, this includes '.' and '..'
        if( (!kvi_strEqualCI(typeToken.ptr(),"channel")) && (!kvi_strEqualCI(typeToken.ptr(),"console")) && (!kvi_strEqualCI(typeToken.ptr(),"dccchat")) && (!kvi_strEqualCI(typeToken.ptr(),"query")) )
            continue;

		// <type>_<name>.<network>_<date>.log

		KviStr undecoded = tmp.getToken('.');
		nameToken.hexDecode(undecoded.ptr());
		
		int idx = tmp.findFirstIdx("_2");
		if(idx != -1)
		{
			undecoded = tmp.left(idx);
			tmp.cutLeft(idx + 1);
		} else 
			undecoded = tmp.getToken('_');

		KviStr network;
		network.hexDecode(undecoded.ptr());
		nameToken.append("@");
		nameToken.append(network);
        dateToken.hexDecode(tmp.ptr());
        dateToken.cutRight(4); // '.log'

        if(typeToken == "dccchat") // 'ip.add.r_ip.add.r_'
        {
            dateToken.getToken('_');dateToken.getToken('_');
        }
        if(typeToken == lastTypeToken)
        {
            if(nameToken == lastNameToken)
                logNode = new KviLogListViewLog(lastNameNode, dateToken.ptr(),(*it).ascii(),lastNameToken.ptr());
            else // New name node (channel-#kvirc, query-Kane,...)
            {
                lastNameToken = nameToken;
                nameNode = new KviLogListViewItem(lastTypeNode,typeToken.ptr(),nameToken.ptr());
                lastNameNode = nameNode;

                logNode = new KviLogListViewLog(lastNameNode, dateToken.ptr(),(*it).ascii(),lastNameToken.ptr());
            }
        }else // New type node (channel, query...)
        {
            lastTypeToken = typeToken;
            typeNode = new KviLogListViewItemType(m_pListView,typeToken.ptr());
            lastTypeNode = typeNode;

            lastNameToken = nameToken;
            nameNode = new KviLogListViewItem(lastTypeNode,typeToken.ptr(),nameToken.ptr());
            lastNameNode = nameNode;

            logNode = new KviLogListViewLog(lastNameNode,dateToken.ptr(),(*it).ascii(),lastNameToken.ptr());
        }
    }
}

void KviLogViewMDIWindow::itemSelected(QListViewItem * it)
{
	bool bCompressed=0;
	//A parent node
	m_pIrcView->clearBuffer();
	if(!it || !it->parent() || (((KviLogListViewItem *)it)->fileName(0).isEmpty()))
	{
		return;
	}
	
	if(((KviLogListViewItem *)it)->fileName(0).isEmpty())return;
	
	KviStr logName = m_szLogDirectory;
	QFile logFile;
	QString text;
	logName.append(((KviLogListViewLog *)it)->fileName(0));
	QFileInfo fi(QString(logName.ptr()));
#ifdef COMPILE_ZLIB_SUPPORT
	if(fi.extension(false)=="gz")
	{
		gzFile file=gzopen(QTextCodec::codecForLocale()->fromUnicode(logName.ptr()).data(),"rb");
		if(file)
		{
			char buff[1025];
			int len;
			QCString data;
			len=gzread(file,buff,1024);
			while(len>0)
			{
				buff[len]=0;
				data.append(buff);
				len=gzread(file,buff,1024);
			}
			gzclose(file);
			text = QString::fromUtf8(data);
		} else {
			debug("Cannot open compressed file %s",logName.ptr());
		}
	} else {
#endif
		logFile.setName(QString::fromUtf8(logName.ptr()));
		
		if(!logFile.open(IO_ReadOnly))
		return;
	
		QByteArray bytes;
		bytes=logFile.readAll();
		text = QString::fromUtf8(bytes.data(), bytes.size());
		logFile.close();
#ifdef COMPILE_ZLIB_SUPPORT
	}
#endif
	QStringList lines=QStringList::split('\n',text);
	bool bOk;
	int iMsgType;
	for ( QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) {
		QString num=(*it).section(' ',0,0);
		iMsgType=num.toInt(&bOk);
		if(bOk)
			outputNoFmt(iMsgType,(*it).section(' ',1),KviIrcView::NoRepaint | KviIrcView::NoTimestamp);
		else
			outputNoFmt(0,*it,KviIrcView::NoRepaint | KviIrcView::NoTimestamp);
	}
	m_pIrcView->repaint(false);
}

QStringList KviLogViewMDIWindow::getFileNames()
{
    QString logPath;
    g_pApp->getLocalKvircDirectory(logPath,KviApp::Log);
    QString qPath(logPath);
    QDir logDir(qPath);
    return logDir.entryList();
}

void KviLogViewMDIWindow::rightButtonClicked ( QListViewItem * it, const QPoint &, int )
{
	if(!it) return;
	if(((KviLogListViewItem *)it)->fileName(0).isEmpty()) return;
	QPopupMenu* popup = new QPopupMenu(this);
	popup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),__tr2qs_ctx("Remove file","logview"),this,SLOT(deleteCurrent()));
	popup->exec( QCursor::pos() );
}

void KviLogViewMDIWindow::deleteCurrent()
{
	KviLogListViewItem* pItem = (KviLogListViewItem *)(m_pListView->currentItem());
	if(pItem)
	{
		if(!pItem->fileName(0).isNull())
		{
			QString szFname;
			g_pApp->getLocalKvircDirectory(szFname,KviApp::Log,pItem->fileName(0));
			KviFileUtils::removeFile(szFname);
			delete pItem;
			m_pIrcView->clearBuffer();
		}
	}
}

#include "logviewmdiwindow.moc"
