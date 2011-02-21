//=============================================================================
//
//   File : KviOptionsWidget.h
//   Creation date : Mon Jun 10 2000 17:47:33 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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



#include "KviOptionsWidget.h"

#define _WANT_OPTION_FLAGS_
#include "KviOptions.h"

#include "KviLocale.h"
#include "KviApplication.h"
#include "KviTalToolTip.h"

#define KVI_OPTIONSWIDGET_GRIDLAYOUT_BORDER 4
#define KVI_OPTIONSWIDGET_GRIDLAYOUT_SPACE 6

QString KviOptionsWidget::m_szBasicTipStart; // empty decl
QString KviOptionsWidget::m_szBasicTipEnd;

KviOptionsWidget::KviOptionsWidget(QWidget * parent,const char * name,bool)
: QFrame(parent), KviSelectorInterface()
{
	if(name)
		setObjectName(name);

	if(m_szBasicTipStart.isEmpty())
	{
		m_szBasicTipStart = "<center><font color=\"#a0a0a0\">";
		m_szBasicTipStart += __tr2qs("This option is also available as");
		m_szBasicTipStart += "<br><nobr>/option <b> ";
		m_szBasicTipEnd = "</b></nobr></font></center>";

	}
	// FIXME: The bSunken value is ignored!
	//if(bSunken)setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	//setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_pLayout = 0;
	m_pTabWidget = 0;
	m_iResetFlags = 0;
	m_iSelectors = 0;
	m_pSelectorInterfaceList = new KviPointerList<KviSelectorInterface>();
	m_pSelectorInterfaceList->setAutoDelete(false);
}

KviOptionsWidget::~KviOptionsWidget()
{
	delete m_pSelectorInterfaceList;
}

void KviOptionsWidget::mergeTip(QWidget * w,const QString &tip)
{
	static QString begin = "<table width=\"100%\" align=\"center\"><tr><td bgcolor=\"#fefef0\"><font color=\"#000000\">";
	static QString mid = "</font></td></tr><tr><td>";
	static QString end = "</td></tr></table>";

	QString s = w->toolTip();
	if(s.isEmpty())KviTalToolTip::add(w,tip);
	else {
		QString tmp = begin;
		tmp += tip;
		tmp += mid;
		tmp += s;
		tmp += end;
		KviTalToolTip::add(w,tmp);
	}
}

void KviOptionsWidget::createLayout()
{
	if(m_pLayout)
		delete m_pLayout;
	m_pLayout = new QGridLayout(this);//,rows,cols,KVI_OPTIONSWIDGET_GRIDLAYOUT_BORDER,KVI_OPTIONSWIDGET_GRIDLAYOUT_SPACE);
	m_pLayout->setMargin(3);
	m_pLayout->setSpacing(2);
}

void KviOptionsWidget::createTabbedPage()
{
	if(m_pTabWidget)
		return; // already created (may happen with reparenting)
	createLayout();
	layout()->setMargin(3);
	layout()->setSpacing(2);
	m_pTabWidget = new QTabWidget(this);
	m_pTabWidget->installEventFilter(this);
	addWidgetToLayout(m_pTabWidget,0,0,0,0);
}


bool KviOptionsWidget::eventFilter(QObject * watched,QEvent * e)
{
	// this is the tab widget

	if((watched == m_pTabWidget) && (e->type() == QEvent::ChildRemoved))
	{
		QChildEvent * ev = static_cast<QChildEvent *>(e);
		if(ev->child()->inherits("KviOptionsWidget"))
		{
			KviOptionsWidget * pWidget = static_cast<KviOptionsWidget *>(ev->child());
			Q_ASSERT(pWidget);
			QObject::disconnect(pWidget,SIGNAL(destroyed()),this,SLOT(childOptionsWidgetDestroyed()));
			m_pSelectorInterfaceList->removeRef(pWidget);
			return false; // continue processing
		}
	}

	return QFrame::eventFilter(watched,e);
}

void KviOptionsWidget::childEvent(QChildEvent * e)
{
	// handle reparents of child widgets
	if(e->type() == QEvent::ChildRemoved)
	{
		if(e->child()->inherits("KviOptionsWidget"))
		{
			KviOptionsWidget * pWidget = static_cast<KviOptionsWidget *>(e->child());
			Q_ASSERT(pWidget);
			QObject::disconnect(pWidget,SIGNAL(destroyed()),this,SLOT(childOptionsWidgetDestroyed()));
			m_pSelectorInterfaceList->removeRef(pWidget);
		}
	}

	QFrame::childEvent(e);
}

void KviOptionsWidget::removeSelector(KviSelectorInterface * pInterface)
{
	KVI_ASSERT(pInterface);
	m_pSelectorInterfaceList->removeRef(pInterface);
}


void KviOptionsWidget::childOptionsWidgetDestroyed()
{
	// dynamic_cast<KviOptionsWidget> doesn't work here since the signal is emitted from QObject...
	KviOptionsWidget * pWidget = static_cast<KviOptionsWidget *>(sender());
	Q_ASSERT(pWidget);
	m_pSelectorInterfaceList->removeRef(pWidget);
}

void KviOptionsWidget::addOptionsWidget(const QString &szText,const QIcon &iconSet,KviOptionsWidget * pWidget)
{
	if(m_pSelectorInterfaceList->findRef(pWidget) >= 0)
		return; // already there ?

	if(pWidget->layout())
		pWidget->layout()->setMargin(3);

	m_pTabWidget->addTab(pWidget,iconSet,szText);

	// add the widget to the selector interface *after* adding its widget to m_pTabWidget,
	// or it will we removed from m_pSelectorInterfaceList because of the addTab() call:
	// QTabWidget::addTab => QTabWidget::insertTab => KviOptionsWidget::eventFilter(m_pTabWidget, QEvent::ChildRemoved);
	m_pSelectorInterfaceList->append(pWidget);
	QObject::connect(pWidget,SIGNAL(destroyed()),this,SLOT(childOptionsWidgetDestroyed()));

}


void KviOptionsWidget::addWidgetToLayout(QWidget * w,int x1,int y1,int x2,int y2)
{
	if((x1 == x2) && (y1 == y2))layout()->addWidget(w,y1,x1);
	else layout()->addWidget(w, y1, x1, (y2 < 0) ? -1 : y2 - y1 + 1, (x2 < 0) ? -1 : x2 - x1 + 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

KviDirectorySelector * KviOptionsWidget::addDirectorySelector(int x1,int y1,int x2,int y2,const QString & txt,QString *pOption,bool bEnabled)
{
	KviDirectorySelector * d = new KviDirectorySelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(d,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviDirectorySelector * KviOptionsWidget::addDirectorySelector(QWidget * pParent,const QString & txt,QString *pOption,bool bEnabled)
{
	KviDirectorySelector * d = new KviDirectorySelector(pParent,txt,pOption,bEnabled);
	//addWidgetToLayout(d,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviDirectorySelector * KviOptionsWidget::addDirectorySelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviDirectorySelector * d = addDirectorySelector(x1,y1,x2,y2,txt,&(KVI_OPTION_STRING(optId)),bEnabled);

	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);

	return d;
}

KviDirectorySelector * KviOptionsWidget::addDirectorySelector(QWidget * pParent,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviDirectorySelector * d = addDirectorySelector(pParent,txt,&(KVI_OPTION_STRING(optId)),bEnabled);

	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);

	return d;
}


///////////////////////////////////////////////////////////////////////////////////////////////////


KviFileSelector * KviOptionsWidget::addFileSelector(int x1,int y1,int x2,int y2,const QString & txt,QString *pOption,bool bEnabled)
{
	KviFileSelector * d = new KviFileSelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(d,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviFileSelector * KviOptionsWidget::addFileSelector(QWidget * pParent,const QString & txt,QString *pOption,bool bEnabled)
{
	KviFileSelector * d = new KviFileSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviFileSelector * KviOptionsWidget::addFileSelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviFileSelector * d = addFileSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_STRING(optId)),bEnabled);

	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);

	return d;
}

KviFileSelector * KviOptionsWidget::addFileSelector(QWidget * pParent,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviFileSelector * d =  addFileSelector(pParent,txt,&(KVI_OPTION_STRING(optId)),bEnabled);

	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);

	return d;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


KviSoundSelector * KviOptionsWidget::addSoundSelector(int x1,int y1,int x2,int y2,const QString & txt,QString *pOption,bool bEnabled)
{
	KviSoundSelector * d = new KviSoundSelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(d,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviSoundSelector * KviOptionsWidget::addSoundSelector(QWidget * pParent,const QString & txt,QString *pOption,bool bEnabled)
{
	KviSoundSelector * d = new KviSoundSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviSoundSelector * KviOptionsWidget::addSoundSelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviSoundSelector * d = addSoundSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_STRING(optId)),bEnabled);

	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);

	return d;
}

KviSoundSelector * KviOptionsWidget::addSoundSelector(QWidget * pParent,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviSoundSelector * d =  addSoundSelector(pParent,txt,&(KVI_OPTION_STRING(optId)),bEnabled);

	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);

	return d;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

KviBoolSelector * KviOptionsWidget::addBoolSelector(int x1,int y1,int x2,int y2,const QString & txt,bool *pOption,bool bEnabled)
{
	KviBoolSelector * b = new KviBoolSelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(b,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(b);
	return b;
}

KviBoolSelector * KviOptionsWidget::addBoolSelector(QWidget * pParent,const QString & txt,bool *pOption,bool bEnabled)
{
	KviBoolSelector * b = new KviBoolSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(b);
	return b;
}

KviBoolSelector * KviOptionsWidget::addBoolSelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_boolOptionsTable[optId].flags & KviOption_resetMask);
	KviBoolSelector * d = addBoolSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_BOOL(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_boolOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

KviBoolSelector * KviOptionsWidget::addBoolSelector(QWidget * pParent,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_boolOptionsTable[optId].flags & KviOption_resetMask);
	KviBoolSelector * d = addBoolSelector(pParent,txt,&(KVI_OPTION_BOOL(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_boolOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


KviPixmapSelector * KviOptionsWidget::addPixmapSelector(int x1,int y1,int x2,int y2,const QString & txt,KviPixmap *pOption,bool bEnabled)
{
	KviPixmapSelector * b = new KviPixmapSelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(b,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(b);
	return b;
}

KviPixmapSelector * KviOptionsWidget::addPixmapSelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_pixmapOptionsTable[optId].flags & KviOption_resetMask);
	KviPixmapSelector * d = addPixmapSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_PIXMAP(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_pixmapOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


KviUIntSelector * KviOptionsWidget::addUIntSelector(int x1,int y1,int x2,int y2,const QString & txt,
	unsigned int * pOption,unsigned int uLowBound,
	unsigned int uHighBound,unsigned int uDefault,bool bEnabled)
{
	KviUIntSelector * u = new KviUIntSelector(this,txt,pOption,uLowBound,uHighBound,uDefault,bEnabled,false);
	addWidgetToLayout(u,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(u);
	return u;
}

KviUIntSelector * KviOptionsWidget::addUIntSelector(QWidget * pParent,const QString & txt,
	unsigned int * pOption,unsigned int uLowBound,
	unsigned int uHighBound,unsigned int uDefault,bool bEnabled)
{
	KviUIntSelector * u = new KviUIntSelector(pParent,txt,pOption,uLowBound,uHighBound,uDefault,bEnabled,false);
	m_pSelectorInterfaceList->append(u);
	return u;
}

KviUIntSelector * KviOptionsWidget::addUIntSelector(int x1,int y1,int x2,int y2,const QString & txt,
	int optId,unsigned int uLowBound,
	unsigned int uHighBound,unsigned int uDefault,bool bEnabled)
{
	m_iResetFlags |= (g_uintOptionsTable[optId].flags & KviOption_resetMask);
	KviUIntSelector * d = addUIntSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_UINT(optId)),uLowBound,uHighBound,uDefault,bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_uintOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

KviUIntSelector * KviOptionsWidget::addUIntSelector(QWidget * pParent,const QString & txt,
	int optId,unsigned int uLowBound,
	unsigned int uHighBound,unsigned int uDefault,bool bEnabled)
{
	m_iResetFlags |= (g_uintOptionsTable[optId].flags & KviOption_resetMask);
	KviUIntSelector * d = addUIntSelector(pParent,txt,&(KVI_OPTION_UINT(optId)),uLowBound,uHighBound,uDefault,bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_uintOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

////////////////////////////////////////////////////////////////////////////////////////////////

KviUIntSelector * KviOptionsWidget::addUShortIntSelector(int x1,int y1,int x2,int y2,const QString & txt,
	unsigned short int * pOption,unsigned int uLowBound,
	unsigned int uHighBound,unsigned int uDefault,bool bEnabled)
{
	KviUIntSelector * u = new KviUIntSelector(this,txt,(unsigned int *)pOption,uLowBound,uHighBound,uDefault,bEnabled,true);
	addWidgetToLayout(u,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(u);
	return u;
}

////////////////////////////////////////////////////////////////////////////////////////////////


KviStringSelector * KviOptionsWidget::addStringSelector(int x1,int y1,int x2,int y2,const QString & txt,QString *pOption,bool bEnabled)
{
	KviStringSelector * d = new KviStringSelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(d,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviStringSelector * KviOptionsWidget::addStringSelector(QWidget * pParent,const QString & txt,QString *pOption,bool bEnabled)
{
	KviStringSelector * d = new KviStringSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviStringSelector * KviOptionsWidget::addStringSelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviStringSelector * d = addStringSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_STRING(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

KviStringSelector * KviOptionsWidget::addStringSelector(QWidget * pParent,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviStringSelector * d = addStringSelector(pParent,txt,&(KVI_OPTION_STRING(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

////////////////////////////////////////////////////////////////////////////////////////////////


KviPasswordSelector * KviOptionsWidget::addPasswordSelector(int x1,int y1,int x2,int y2,const QString & txt,QString *pOption,bool bEnabled)
{
	KviPasswordSelector * d = new KviPasswordSelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(d,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviPasswordSelector * KviOptionsWidget::addPasswordSelector(QWidget * pParent,const QString & txt,QString *pOption,bool bEnabled)
{
	KviPasswordSelector * d = new KviPasswordSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviPasswordSelector * KviOptionsWidget::addPasswordSelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviPasswordSelector * d = addPasswordSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_STRING(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

KviPasswordSelector * KviOptionsWidget::addPasswordSelector(QWidget * pParent,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringOptionsTable[optId].flags & KviOption_resetMask);
	KviPasswordSelector * d = addPasswordSelector(pParent,txt,&(KVI_OPTION_STRING(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_stringOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

////////////////////////////////////////////////////////////////////////////////////////////////


KviStringListSelector * KviOptionsWidget::addStringListSelector(int x1,int y1,int x2,int y2,const QString & txt,QStringList * pOption,bool bEnabled)
{
	KviStringListSelector * d = new KviStringListSelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(d,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(d);
	return d;
}

KviStringListSelector * KviOptionsWidget::addStringListSelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_stringlistOptionsTable[optId].flags & KviOption_resetMask);
	KviStringListSelector * d = addStringListSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_STRINGLIST(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_stringlistOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

////////////////////////////////////////////////////////////////////////////////////////////////


KviColorSelector * KviOptionsWidget::addColorSelector(int x1,int y1,int x2,int y2,const QString & txt,QColor * pOption,bool bEnabled)
{
	KviColorSelector * s = new KviColorSelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(s,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(s);
	return s;
}

KviColorSelector * KviOptionsWidget::addColorSelector(QWidget * pParent,const QString & txt,QColor * pOption,bool bEnabled)
{
	KviColorSelector * s = new KviColorSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(s);
	return s;
}

KviColorSelector * KviOptionsWidget::addColorSelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_colorOptionsTable[optId].flags & KviOption_resetMask);
	KviColorSelector * d = addColorSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_COLOR(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_colorOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

KviColorSelector * KviOptionsWidget::addColorSelector(QWidget * pParent,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_colorOptionsTable[optId].flags & KviOption_resetMask);
	KviColorSelector * d = addColorSelector(pParent,txt,&(KVI_OPTION_COLOR(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_colorOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

/////////////////////////////////////////////////////////////////////////////////////////////////


KviMircTextColorSelector * KviOptionsWidget::addMircTextColorSelector(int x1,int y1,int x2,int y2,const QString & txt,unsigned int *uFore,unsigned int *uBack,bool bEnabled)
{
	KviMircTextColorSelector * s = new KviMircTextColorSelector(this,txt,uFore,uBack,bEnabled);
	addWidgetToLayout(s,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(s);
	return s;
}

KviMircTextColorSelector * KviOptionsWidget::addMircTextColorSelector(QWidget * pParent,const QString & txt,unsigned int *uFore,unsigned int *uBack,bool bEnabled)
{
	KviMircTextColorSelector * s = new KviMircTextColorSelector(pParent,txt,uFore,uBack,bEnabled);
	m_pSelectorInterfaceList->append(s);
	return s;
}

KviMircTextColorSelector * KviOptionsWidget::addMircTextColorSelector(int x1,int y1,int x2,int y2,const QString & txt,int optForeId,int optBackId,bool bEnabled)
{
	m_iResetFlags |= (g_uintOptionsTable[optForeId].flags & KviOption_resetMask);
	m_iResetFlags |= (g_uintOptionsTable[optBackId].flags & KviOption_resetMask);
	KviMircTextColorSelector * d = addMircTextColorSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_UINT(optForeId)),&(KVI_OPTION_UINT(optBackId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_uintOptionsTable[optForeId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

KviMircTextColorSelector * KviOptionsWidget::addMircTextColorSelector(QWidget * pParent,const QString & txt,int optForeId,int optBackId,bool bEnabled)
{
	m_iResetFlags |= (g_uintOptionsTable[optForeId].flags & KviOption_resetMask);
	m_iResetFlags |= (g_uintOptionsTable[optBackId].flags & KviOption_resetMask);
	KviMircTextColorSelector * d = addMircTextColorSelector(pParent,txt,&(KVI_OPTION_UINT(optForeId)),&(KVI_OPTION_UINT(optBackId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_uintOptionsTable[optForeId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}


////////////////////////////////////////////////////////////////////////////////////////////////


KviFontSelector * KviOptionsWidget::addFontSelector(int x1,int y1,int x2,int y2,const QString & txt,QFont * pOption,bool bEnabled)
{
	KviFontSelector * f = new KviFontSelector(this,txt,pOption,bEnabled);
	addWidgetToLayout(f,x1,y1,x2,y2);
	m_pSelectorInterfaceList->append(f);
	return f;
}

KviFontSelector * KviOptionsWidget::addFontSelector(QWidget * pParent,const QString & txt,QFont * pOption,bool bEnabled)
{
	KviFontSelector * f = new KviFontSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(f);
	return f;
}

KviFontSelector * KviOptionsWidget::addFontSelector(int x1,int y1,int x2,int y2,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_fontOptionsTable[optId].flags & KviOption_resetMask);
	KviFontSelector * d = addFontSelector(x1,y1,x2,y2,txt,&(KVI_OPTION_FONT(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_fontOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

KviFontSelector * KviOptionsWidget::addFontSelector(QWidget * pParent,const QString & txt,int optId,bool bEnabled)
{
	m_iResetFlags |= (g_fontOptionsTable[optId].flags & KviOption_resetMask);
	KviFontSelector * d = addFontSelector(pParent,txt,&(KVI_OPTION_FONT(optId)),bEnabled);
	QString tmp = m_szBasicTipStart;
	tmp += g_fontOptionsTable[optId].name;
	tmp += m_szBasicTipEnd;
	KviTalToolTip::add(d,tmp);
	return d;
}

//#define DECLARE_COMMIT_FUNCTION(__fnc,__class,__classString)
//	void KviOptionsWidget::__fnc()
//	{
//		QObjectList * l = queryList(__classString);
//		if(!l)return;
//		QObjectListIt it(*l);
//		while(it.current())
//		{
//			((__class *)it.current())->commit();
//			++it;
//		}
//		delete l;
//	}

//DECLARE_COMMIT_FUNCTION(commitBoolSelectors,KviBoolSelector,"KviBoolSelector")
//DECLARE_COMMIT_FUNCTION(commitUIntSelectors,KviUIntSelector,"KviUIntSelector")
//DECLARE_COMMIT_FUNCTION(commitStringSelectors,KviStringSelector,"KviStringSelector")
//DECLARE_COMMIT_FUNCTION(commitPixmapSelectors,KviPixmapSelector,"KviPixmapSelector")

void KviOptionsWidget::addRowSpacer(int x1,int y1,int x2,int y2)
{
	QWidget * w = new QWidget(this);
	addWidgetToLayout(w,x1,y1,x2,y2);
	layout()->setRowStretch(y1,1);
}

QLabel * KviOptionsWidget::addLabel(int x1,int y1,int x2,int y2,const QString & text,bool bEnabled)
{
	QLabel * l = new QLabel(text,this);
	l->setWordWrap(true);
	l->setEnabled(bEnabled);
	addWidgetToLayout(l,x1,y1,x2,y2);
	return l;
}

QLabel * KviOptionsWidget::addLabel(QWidget * pParent,const QString & text,bool bEnabled)
{
	QLabel * l = new QLabel(text,pParent);
	l->setWordWrap(true);
	l->setEnabled(bEnabled);
	return l;
}

QLineEdit * KviOptionsWidget::addLineEdit(int x1,int y1,int x2,int y2,bool bEnabled)
{
	QLineEdit * l = new QLineEdit(this);
	l->setEnabled(bEnabled);
	addWidgetToLayout(l,x1,y1,x2,y2);
	return l;
}

QFrame * KviOptionsWidget::addSeparator(int x1,int y1,int x2,int y2)
{
	QFrame * f = new QFrame(this);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	addWidgetToLayout(f,x1,y1,x2,y2);
	return f;
}

KviTalGroupBox * KviOptionsWidget::addGroupBox(int x1,int y1,int x2,int y2,Qt::Orientation o,const QString &txt,bool bEnabled)
{
	KviTalGroupBox * g = new KviTalGroupBox(o,txt,this);
	g->setEnabled(bEnabled);
	addWidgetToLayout(g,x1,y1,x2,y2);
	return g;
}

void KviOptionsWidget::addAdvancedButton(int x1,int y1,int x2,int y2)
{
	QWidget * w = topLevelWidget();
	if(!w)return;
	if(!w->inherits("KviGeneralOptionsDialog"))return;

	QPushButton * b = new QPushButton(__tr2qs("Advanced..."),this);
	connect(b,SIGNAL(clicked()),this,SLOT(switchToAdvancedPage()));
	addWidgetToLayout(b,x1,y1,x2,y2);
}


void KviOptionsWidget::commitSelectors()
{
//	if(m_iSelectors & KVI_OPTIONSELECTOR_TYPE_BOOL)commitBoolSelectors();
//	if(m_iSelectors & KVI_OPTIONSELECTOR_TYPE_UINT)commitUIntSelectors();
//	if(m_iSelectors & KVI_OPTIONSELECTOR_TYPE_STRING)commitStringSelectors();
//	if(m_iSelectors & KVI_OPTIONSELECTOR_TYPE_PIXMAP)commitPixmapSelectors();
//	m_iSelectors = 0;
	for(KviSelectorInterface * i = m_pSelectorInterfaceList->first();i;i = m_pSelectorInterfaceList->next())
	{
		i->commit();
	}
}

void KviOptionsWidget::commitOptionsReset()
{
	if(m_iResetFlags)g_pApp->optionResetUpdate(m_iResetFlags);
	//m_iResetFlags = 0;
}

void KviOptionsWidget::commit()
{
	commitSelectors();
	commitOptionsReset();
}

void KviOptionsWidget::switchToAdvancedPage()
{
	// FIXME: what happens if we're toplevel ????
	// (so nobody listens to this signal....)

	emit wantToSwitchToAdvancedPage(this);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviOptionsWidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
