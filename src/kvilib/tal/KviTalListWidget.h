#ifndef _KVI_TAL_LISTWIDGET_H_
#define _KVI_TAL_LISTWIDGET_H_
//=============================================================================
//
//   File : KviTalListWidget.h
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

#include "kvi_settings.h"
#include "KviTalIconAndRichTextItemDelegate.h"

#include <QListWidget>

class KviTalListWidgetItem;

class KVILIB_API KviTalListWidget : public QListWidget
{
	Q_OBJECT
public:
	KviTalListWidget(QWidget * pParent, QString name, Qt::WindowType f = Qt::Widget);
	KviTalListWidget(QWidget * pParent)
	    : QListWidget(pParent){};
	~KviTalListWidget() = default;

protected:
	bool event(QEvent * e) override;
	bool eventFilter(QObject * o, QEvent * e) override;
signals:
	void tipRequest(QListWidgetItem *, const QPoint &);
};

class KVILIB_API KviTalListWidgetItem : public QListWidgetItem
{
public:
	KviTalListWidgetItem()
	    : QListWidgetItem(){};
	KviTalListWidgetItem(KviTalListWidget * pParent)
	    : QListWidgetItem(pParent){};
	KviTalListWidgetItem(KviTalListWidget * pParent, QString & label)
	    : QListWidgetItem(label, pParent){};
	KviTalListWidget * listWidget() { return (KviTalListWidget *)QListWidgetItem::listWidget(); };
	~KviTalListWidgetItem() = default;
};

class KVILIB_API KviTalListWidgetText : public KviTalListWidgetItem
{
public:
	KviTalListWidgetText(KviTalListWidget * listbox, const QString & text = QString());
	KviTalListWidgetText(const QString & text = QString());
	KviTalListWidgetText(const KviTalListWidgetText &) = delete;
	~KviTalListWidgetText();

	int height(const KviTalListWidget *) const;
	int width(const KviTalListWidget *) const;

	int rtti() const;
	enum
	{
		RTTI = 1
	};

protected:
	virtual void paint(QPainter *);
};

class KVILIB_API KviTalListWidgetPixmap : public KviTalListWidgetItem
{
public:
	KviTalListWidgetPixmap(KviTalListWidget * listbox, const QPixmap &);
	KviTalListWidgetPixmap(const QPixmap &);
	KviTalListWidgetPixmap(KviTalListWidget * listbox, const QPixmap &, const QString &);
	KviTalListWidgetPixmap(const QPixmap &, const QString &);
	KviTalListWidgetPixmap(const KviTalListWidgetPixmap &) = delete;
	~KviTalListWidgetPixmap();

	const QPixmap * pixmap() const { return &pm; }

	int height(const KviTalListWidget *) const;
	int width(const KviTalListWidget *) const;

	int rtti() const;
	enum
	{
		RTTI = 2
	};

protected:
	virtual void paint(QPainter *);

private:
	QPixmap pm;
};

#endif // _KVI_TAL_LISTWIDGET_H_
