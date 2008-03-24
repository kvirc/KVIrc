//=============================================================================
//
//   File : kvi_tal_listbox.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#define __KVILIB__
#include "kvi_tal_listbox.h"

#ifdef COMPILE_USE_QT4
	#include <qpainter.h>
	#include <qstyle.h>
	#include <qapplication.h>
	
	KviTalListBox::KviTalListBox(QWidget * pParent,Qt::WFlags f)
	: Q3ListBox(pParent,0,f)
	{
		connect(this,SIGNAL(highlighted(Q3ListBoxItem *)),this,SLOT(redirect_highlighted(Q3ListBoxItem *)));
		connect(this,SIGNAL(selected(Q3ListBoxItem *)),this,SLOT(redirect_selected(Q3ListBoxItem *)));
		connect(this,SIGNAL(selectionChanged(Q3ListBoxItem *)),this,SLOT(redirect_selectionChanged(Q3ListBoxItem *)));
		connect(this,SIGNAL(currentChanged(Q3ListBoxItem *)),this,SLOT(redirect_currentChanged(Q3ListBoxItem *)));
		connect(this,SIGNAL(clicked(Q3ListBoxItem *)),this,SLOT(redirect_clicked(Q3ListBoxItem *)));
		connect(this,SIGNAL(clicked(Q3ListBoxItem *,const QPoint &)),this,SLOT(redirect_clicked(Q3ListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(pressed(Q3ListBoxItem *)),this,SLOT(redirect_pressed(Q3ListBoxItem *)));
		connect(this,SIGNAL(pressed(Q3ListBoxItem *,const QPoint &)),this,SLOT(redirect_pressed(Q3ListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(doubleClicked(Q3ListBoxItem *)),this,SLOT(redirect_doubleClicked(Q3ListBoxItem *)));
		connect(this,SIGNAL(returnPressed(Q3ListBoxItem *)),this,SLOT(redirect_returnPressed(Q3ListBoxItem *)));
		connect(this,SIGNAL(rightButtonClicked(Q3ListBoxItem *, const QPoint &)),this,SLOT(redirect_rightButtonClicked(Q3ListBoxItem *, const QPoint &)));
		connect(this,SIGNAL(rightButtonPressed(Q3ListBoxItem *, const QPoint &)),this,SLOT(redirect_rightButtonPressed(Q3ListBoxItem *, const QPoint &)));
		connect(this,SIGNAL(mouseButtonPressed(int,Q3ListBoxItem *,const QPoint &)),this,SLOT(redirect_mouseButtonPressed(int,Q3ListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(mouseButtonClicked(int,Q3ListBoxItem *,const QPoint &)),this,SLOT(redirect_mouseButtonClicked(int,Q3ListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(contextMenuRequested(Q3ListBoxItem *,const QPoint &)),this,SLOT(redirect_contextMenuRequested(Q3ListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(onItem(Q3ListBoxItem *)),this,SLOT(redirect_onItem(Q3ListBoxItem *)));
	}

	void KviTalListBox::redirect_highlighted(Q3ListBoxItem *item)
	{
		emit highlighted((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_selected(Q3ListBoxItem *item)
	{
		emit selected((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_selectionChanged(Q3ListBoxItem * item)
	{
		emit selectionChanged((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_currentChanged(Q3ListBoxItem * item)
	{
		emit currentChanged((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_clicked(Q3ListBoxItem * item)
	{
		emit clicked((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_clicked(Q3ListBoxItem * item,const QPoint & pnt)
	{
		emit clicked((KviTalListBoxItem *)item,pnt);
	}

	void KviTalListBox::redirect_pressed(Q3ListBoxItem * item)
	{
		emit pressed((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_pressed(Q3ListBoxItem * item,const QPoint & pnt)
	{
		emit pressed((KviTalListBoxItem *)item,pnt);
	}

	void KviTalListBox::redirect_doubleClicked(Q3ListBoxItem * item)
	{
		emit doubleClicked((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_returnPressed(Q3ListBoxItem * item)
	{
		emit returnPressed((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_rightButtonClicked(Q3ListBoxItem * item, const QPoint &pnt)
	{
		emit rightButtonClicked((KviTalListBoxItem *)item,pnt);
	}

	void KviTalListBox::redirect_rightButtonPressed(Q3ListBoxItem * item, const QPoint &pnt)
	{
		emit rightButtonPressed((KviTalListBoxItem *)item,pnt);
	}

	void KviTalListBox::redirect_mouseButtonPressed(int button,Q3ListBoxItem * item,const QPoint & pos)
	{
		emit mouseButtonPressed(button,(KviTalListBoxItem *)item,pos);
	}

	void KviTalListBox::redirect_mouseButtonClicked(int button,Q3ListBoxItem * item,const QPoint & pos)
	{
		emit mouseButtonClicked(button,(KviTalListBoxItem *)item,pos);
	}

	void KviTalListBox::redirect_contextMenuRequested(Q3ListBoxItem * item,const QPoint & pos)
	{
		emit contextMenuRequested((KviTalListBoxItem *)item,pos);
	}

	void KviTalListBox::redirect_onItem(Q3ListBoxItem * i)
	{
		emit onItem((KviTalListBoxItem *)i);
	}

	KviTalListBoxText::KviTalListBoxText(KviTalListBox *listbox, const QString &text)
	    :KviTalListBoxItem(listbox)
	{
	    setText(text);
	}

	KviTalListBoxText::KviTalListBoxText(const QString &text)
	    :KviTalListBoxItem()
	{
	    setText(text);
	}
	
	KviTalListBoxText::KviTalListBoxText(KviTalListBox* listbox, const QString &text, KviTalListBoxItem *after)
	    : KviTalListBoxItem(listbox, after)
	{
	    setText(text);
	}
	
	KviTalListBoxText::~KviTalListBoxText()
	{
	}
	
	void KviTalListBoxText::paint(QPainter *painter)
	{
	    int itemHeight = height(listBox());
	    QFontMetrics fm = painter->fontMetrics();
	    int yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();
	    painter->drawText(3, yPos, text());
	}
	
	int KviTalListBoxText::height(const KviTalListBox* lb) const
	{
	    int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
	    return qMax(h, QApplication::globalStrut().height());
	}
	
	int KviTalListBoxText::width(const KviTalListBox* lb) const
	{
	    int w = lb ? lb->fontMetrics().width(text()) + 6 : 0;
	    return qMax(w, QApplication::globalStrut().width());
	}
	
	int KviTalListBoxText::rtti() const
	{
	    return RTTI;
	}
	
	KviTalListBoxPixmap::KviTalListBoxPixmap(KviTalListBox* listbox, const QPixmap &pixmap)
	    : KviTalListBoxItem(listbox)
	{
	    pm = pixmap;
	}
	
	KviTalListBoxPixmap::KviTalListBoxPixmap(const QPixmap &pixmap)
	    : KviTalListBoxItem()
	{
	    pm = pixmap;
	}
	
	KviTalListBoxPixmap::KviTalListBoxPixmap(KviTalListBox* listbox, const QPixmap &pixmap, KviTalListBoxItem *after)
	    : KviTalListBoxItem(listbox, after)
	{
	    pm = pixmap;
	}
	
	KviTalListBoxPixmap::~KviTalListBoxPixmap()
	{
	}

	KviTalListBoxPixmap::KviTalListBoxPixmap(KviTalListBox* listbox, const QPixmap &pix, const QString& text)
	    : KviTalListBoxItem(listbox)
	{
	    pm = pix;
	    setText(text);
	}
	
	KviTalListBoxPixmap::KviTalListBoxPixmap(const QPixmap & pix, const QString& text)
	    : KviTalListBoxItem()
	{
	    pm = pix;
	    setText(text);
	}
	
	KviTalListBoxPixmap::KviTalListBoxPixmap(KviTalListBox* listbox, const QPixmap & pix, const QString& text,
	                                KviTalListBoxItem *after)
	    : KviTalListBoxItem(listbox, after)
	{
	    pm = pix;
	    setText(text);
	}
	
	void KviTalListBoxPixmap::paint(QPainter *painter)
	{
	    int itemHeight = height(listBox());
	    int yPos;
	
	    const QPixmap *pm = pixmap();
	    if (pm && ! pm->isNull()) {
	        yPos = (itemHeight - pm->height()) / 2;
	        painter->drawPixmap(3, yPos, *pm);
	    }
	
	    if (!text().isEmpty()) {
	        QFontMetrics fm = painter->fontMetrics();
	        yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();
	        painter->drawText(pm->width() + 5, yPos, text());
	    }
	}
	
	int KviTalListBoxPixmap::height(const KviTalListBox* lb) const
	{
	    int h;
	    if (text().isEmpty())
	        h = pm.height();
	    else
	        h = qMax(pm.height(), lb->fontMetrics().lineSpacing() + 2);
	    return qMax(h, QApplication::globalStrut().height());
	}
	
	int KviTalListBoxPixmap::width(const KviTalListBox* lb) const
	{
	    if (text().isEmpty())
	        return qMax(pm.width() + 6, QApplication::globalStrut().width());
	    return qMax(pm.width() + lb->fontMetrics().width(text()) + 6,
	            QApplication::globalStrut().width());
	}
	
	int KviTalListBoxPixmap::rtti() const
	{
	    return RTTI;
	}
	

	#include "kvi_tal_listbox_qt4.moc"
#else
	#include <qpainter.h>
	#include <qstyle.h>
	#include <qapplication.h>


	KviTalListBox::KviTalListBox(QWidget * pParent,Qt::WFlags f)
	: QListBox(pParent,0,f)
	{
		connect(this,SIGNAL(highlighted(QListBoxItem *)),this,SLOT(redirect_highlighted(QListBoxItem *)));
		connect(this,SIGNAL(selected(QListBoxItem *)),this,SLOT(redirect_selected(QListBoxItem *)));
		connect(this,SIGNAL(selectionChanged(QListBoxItem *)),this,SLOT(redirect_selectionChanged(QListBoxItem *)));
		connect(this,SIGNAL(currentChanged(QListBoxItem *)),this,SLOT(redirect_currentChanged(QListBoxItem *)));
		connect(this,SIGNAL(clicked(QListBoxItem *)),this,SLOT(redirect_clicked(QListBoxItem *)));
		connect(this,SIGNAL(clicked(QListBoxItem *,const QPoint &)),this,SLOT(redirect_clicked(QListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(pressed(QListBoxItem *)),this,SLOT(redirect_pressed(QListBoxItem *)));
		connect(this,SIGNAL(pressed(QListBoxItem *,const QPoint &)),this,SLOT(redirect_pressed(QListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(doubleClicked(QListBoxItem *)),this,SLOT(redirect_doubleClicked(QListBoxItem *)));
		connect(this,SIGNAL(returnPressed(QListBoxItem *)),this,SLOT(redirect_returnPressed(QListBoxItem *)));
		connect(this,SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),this,SLOT(redirect_rightButtonClicked(QListBoxItem *, const QPoint &)));
		connect(this,SIGNAL(rightButtonPressed(QListBoxItem *, const QPoint &)),this,SLOT(redirect_rightButtonPressed(QListBoxItem *, const QPoint &)));
		connect(this,SIGNAL(mouseButtonPressed(int,QListBoxItem *,const QPoint &)),this,SLOT(redirect_mouseButtonPressed(int,QListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(mouseButtonClicked(int,QListBoxItem *,const QPoint &)),this,SLOT(redirect_mouseButtonClicked(int,QListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(contextMenuRequested(QListBoxItem *,const QPoint &)),this,SLOT(redirect_contextMenuRequested(QListBoxItem *,const QPoint &)));
		connect(this,SIGNAL(onItem(QListBoxItem *)),this,SLOT(redirect_onItem(QListBoxItem *)));
	}

	void KviTalListBox::redirect_highlighted(QListBoxItem *item)
	{
		emit highlighted((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_selected(QListBoxItem *item)
	{
		emit selected((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_selectionChanged(QListBoxItem * item)
	{
		emit selectionChanged((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_currentChanged(QListBoxItem * item)
	{
		emit currentChanged((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_clicked(QListBoxItem * item)
	{
		emit clicked((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_clicked(QListBoxItem * item,const QPoint & pnt)
	{
		emit clicked((KviTalListBoxItem *)item,pnt);
	}

	void KviTalListBox::redirect_pressed(QListBoxItem * item)
	{
		emit pressed((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_pressed(QListBoxItem * item,const QPoint & pnt)
	{
		emit pressed((KviTalListBoxItem *)item,pnt);
	}

	void KviTalListBox::redirect_doubleClicked(QListBoxItem * item)
	{
		emit doubleClicked((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_returnPressed(QListBoxItem * item)
	{
		emit returnPressed((KviTalListBoxItem *)item);
	}

	void KviTalListBox::redirect_rightButtonClicked(QListBoxItem * item, const QPoint &pnt)
	{
		emit rightButtonClicked((KviTalListBoxItem *)item,pnt);
	}

	void KviTalListBox::redirect_rightButtonPressed(QListBoxItem * item, const QPoint &pnt)
	{
		emit rightButtonPressed((KviTalListBoxItem *)item,pnt);
	}

	void KviTalListBox::redirect_mouseButtonPressed(int button,QListBoxItem * item,const QPoint & pos)
	{
		emit mouseButtonPressed(button,(KviTalListBoxItem *)item,pos);
	}

	void KviTalListBox::redirect_mouseButtonClicked(int button,QListBoxItem * item,const QPoint & pos)
	{
		emit mouseButtonClicked(button,(KviTalListBoxItem *)item,pos);
	}

	void KviTalListBox::redirect_contextMenuRequested(QListBoxItem * item,const QPoint & pos)
	{
		emit contextMenuRequested((KviTalListBoxItem *)item,pos);
	}

	void KviTalListBox::redirect_onItem(QListBoxItem * i)
	{
		emit onItem((KviTalListBoxItem *)i);
	}

	KviTalListBoxText::KviTalListBoxText( KviTalListBox *listbox, const QString &text )
	    :KviTalListBoxItem( listbox )
	{
	    setText( text );
	}
	
	KviTalListBoxText::KviTalListBoxText( const QString &text )
	    :KviTalListBoxItem()
	{
	    setText( text );
	}
	
	KviTalListBoxText::KviTalListBoxText( KviTalListBox* listbox, const QString &text, KviTalListBoxItem *after )
	    : KviTalListBoxItem( listbox, after )
	{
	    setText( text );
	}
	
	KviTalListBoxText::~KviTalListBoxText()
	{
	}
	
	void KviTalListBoxText::paint( QPainter *painter )
	{
	    int itemHeight = height( listBox() );
	    QFontMetrics fm = painter->fontMetrics();
	    int yPos = ( ( itemHeight - fm.height() ) / 2 ) + fm.ascent();
	    painter->drawText( 3, yPos, text() );
	}
	
	int KviTalListBoxText::height( const KviTalListBox* lb ) const
	{
	    int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
	    return QMAX( h, QApplication::globalStrut().height() );
	}
	
	int KviTalListBoxText::width( const KviTalListBox* lb ) const
	{
	    int w = lb ? lb->fontMetrics().width( text() ) + 6 : 0;
	    return QMAX( w, QApplication::globalStrut().width() );
	}
	
	int KviTalListBoxText::RTTI = 1;
	
	int KviTalListBoxText::rtti() const
	{
	    return RTTI;
	}

	KviTalListBoxPixmap::KviTalListBoxPixmap( KviTalListBox* listbox, const QPixmap &pixmap )
	    : KviTalListBoxItem( listbox )
	{
	    pm = pixmap;
	}
	
	KviTalListBoxPixmap::KviTalListBoxPixmap( const QPixmap &pixmap )
	    : KviTalListBoxItem()
	{
	    pm = pixmap;
	}
	
	KviTalListBoxPixmap::KviTalListBoxPixmap( KviTalListBox* listbox, const QPixmap &pixmap, KviTalListBoxItem *after )
	    : KviTalListBoxItem( listbox, after )
	{
	    pm = pixmap;
	}
	
	KviTalListBoxPixmap::~KviTalListBoxPixmap()
	{
	}
	
	KviTalListBoxPixmap::KviTalListBoxPixmap( KviTalListBox* listbox, const QPixmap &pix, const QString& text)
	    : KviTalListBoxItem( listbox )
	{
	    pm = pix;
	    setText( text );
	}

	KviTalListBoxPixmap::KviTalListBoxPixmap( const QPixmap & pix, const QString& text)
	    : KviTalListBoxItem()
	{
	    pm = pix;
	    setText( text );
	}
	
	KviTalListBoxPixmap::KviTalListBoxPixmap( KviTalListBox* listbox, const QPixmap & pix, const QString& text,
					KviTalListBoxItem *after )
	    : KviTalListBoxItem( listbox, after )
	{
	    pm = pix;
	    setText( text );
	}
	
	void KviTalListBoxPixmap::paint( QPainter *painter )
	{
	    int itemHeight = height( listBox() );
	    int yPos;
	
	    const QPixmap *pm = pixmap();
	    if ( pm && ! pm->isNull() ) {
		yPos = ( itemHeight - pm->height() ) / 2;
		painter->drawPixmap( 3, yPos, *pm);
	    }
	
	    if ( !text().isEmpty() ) {
		QFontMetrics fm = painter->fontMetrics();
		yPos = ( ( itemHeight - fm.height() ) / 2 ) + fm.ascent();
		painter->drawText( pm->width() + 5, yPos, text() );
	    }
	}
	
	int KviTalListBoxPixmap::height( const KviTalListBox* lb ) const
	{
	    int h;
	    if ( text().isEmpty() )
		h = pm.height();
	    else
		h = QMAX( pm.height(), lb->fontMetrics().lineSpacing() + 2 );
	    return QMAX( h, QApplication::globalStrut().height() );
	}
	
	int KviTalListBoxPixmap::width( const KviTalListBox* lb ) const
	{
	    if ( text().isEmpty() )
		return QMAX( pm.width() + 6, QApplication::globalStrut().width() );
	    return QMAX( pm.width() + lb->fontMetrics().width( text() ) + 6,
		    QApplication::globalStrut().width() );
	}
	
	int KviTalListBoxPixmap::RTTI = 2;
	
	int KviTalListBoxPixmap::rtti() const
	{
	    return RTTI;
	}
	
	#include "kvi_tal_listbox_qt3.moc"
#endif

