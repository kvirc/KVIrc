//=============================================================================
//
//   File : KviTalListWidget.cpp
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KviTalListWidget.h"
#include "KviTalIconAndRichTextItemDelegate.h"

#include <QEvent>
#include <QPainter>
#include <QHelpEvent>
#include <QApplication>

KviTalListWidget::KviTalListWidget(QWidget * pParent, QString name, Qt::WindowType f)
    : QListWidget(pParent)
{
	setObjectName(name);
	setWindowFlags(f);

	//setUniformItemSizes(true);
	//setResizeMode(QListWidget::Adjust);
	//setWordWrap(true);
	viewport()->installEventFilter(this);
}

bool KviTalListWidget::eventFilter(QObject * o, QEvent * e)
{
	if(e->type() == QEvent::Resize)
	{
		// A veeeery ugly hack.
		// This is the only reliable way I have found to perform a full
		// relayout on the items.
		// The Qt ItemView framework is so complicated that it's almost unusable.
		// We had the same functionality in Qt3 with 1/10 of the code.
		if(model())
		{
			// And this does not even work when the items should shrink...
			// ...but well, that's the best we can do (but only under Qt5, as with Qt4 this is even protected).
			emit model()->layoutChanged();
			// The model is hidden so we can't make these public.
			//model()->beginResetModel();
			//model()->endResetModel();
		}
	}
	return QListWidget::eventFilter(o, e);
}

bool KviTalListWidget::event(QEvent * e)
{
	if(e->type() == QEvent::ToolTip)
	{
		QHelpEvent * helpEvent = static_cast<QHelpEvent *>(e);
		const QPoint & p = helpEvent->pos();
		QListWidgetItem * item = itemAt(p);
		if(item)
		{
			emit tipRequest(item, p);
		}
	}
	return QListWidget::event(e);
}

KviTalListWidgetText::KviTalListWidgetText(KviTalListWidget * listbox, const QString & text)
    : KviTalListWidgetItem(listbox)
{
	setText(text);
}

KviTalListWidgetText::KviTalListWidgetText(const QString & text)
    : KviTalListWidgetItem()
{
	setText(text);
}

KviTalListWidgetText::~KviTalListWidgetText()
    = default;

void KviTalListWidgetText::paint(QPainter * painter)
{
	int itemHeight = height(listWidget());
	QFontMetrics fm = painter->fontMetrics();
	int yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();
	painter->drawText(3, yPos, text());
}

int KviTalListWidgetText::height(const KviTalListWidget * lb) const
{
	int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
	return qMax(h, QApplication::globalStrut().height());
}

int KviTalListWidgetText::width(const KviTalListWidget * lb) const
{
	int w = lb ? lb->fontMetrics().width(text()) + 6 : 0;
	return qMax(w, QApplication::globalStrut().width());
}

int KviTalListWidgetText::rtti() const
{
	return RTTI;
}

KviTalListWidgetPixmap::KviTalListWidgetPixmap(KviTalListWidget * listbox, const QPixmap & pixmap)
    : KviTalListWidgetItem(listbox)
{
	pm = pixmap;
}

KviTalListWidgetPixmap::KviTalListWidgetPixmap(const QPixmap & pixmap)
    : KviTalListWidgetItem()
{
	pm = pixmap;
}

KviTalListWidgetPixmap::~KviTalListWidgetPixmap()
    = default;

KviTalListWidgetPixmap::KviTalListWidgetPixmap(KviTalListWidget * listbox, const QPixmap & pix, const QString & text)
    : KviTalListWidgetItem(listbox)
{
	pm = pix;
	setText(text);
	setIcon(QIcon(pix));
}

KviTalListWidgetPixmap::KviTalListWidgetPixmap(const QPixmap & pix, const QString & text)
    : KviTalListWidgetItem()
{
	pm = pix;
	setText(text);
}

void KviTalListWidgetPixmap::paint(QPainter * painter)
{
	int itemHeight = height(listWidget());
	int yPos;

	const QPixmap * pm = pixmap();
	if(pm && !pm->isNull())
	{
		yPos = (itemHeight - pm->height()) / 2;
		painter->drawPixmap(3, yPos, *pm);
	}

	if(!text().isEmpty())
	{
		QFontMetrics fm = painter->fontMetrics();
		yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();
		painter->drawText(pm->width() + 5, yPos, text());
	}
}

int KviTalListWidgetPixmap::height(const KviTalListWidget * lb) const
{
	int h;
	if(text().isEmpty())
		h = pm.height();
	else
		h = qMax(pm.height(), lb->fontMetrics().lineSpacing() + 2);
	return qMax(h, QApplication::globalStrut().height());
}

int KviTalListWidgetPixmap::width(const KviTalListWidget * lb) const
{
	if(text().isEmpty())
		return qMax(pm.width() + 6, QApplication::globalStrut().width());
	return qMax(pm.width() + lb->fontMetrics().width(text()) + 6,
	    QApplication::globalStrut().width());
}

int KviTalListWidgetPixmap::rtti() const
{
	return RTTI;
}
