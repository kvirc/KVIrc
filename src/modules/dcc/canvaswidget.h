#ifndef _CANVAS_WIDGET_H_
#define _CANVAS_WIDGET_H_
//=============================================================================
//
//   File : canvaswidget.h
//   Creation date : Mon Jul 30 07 2001 04:49:49 by Szymon Stefanek
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

#include "DccBroker.h"

#ifdef COMPILE_DCC_CANVAS
#include <QCanvas>
#include <QMenuBar>
#include <QLabel>
#include <QMap>
#include <QVariant>
#include <QTable>
#include <QSplitter>

class DccCanvasWidget;

#define KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE 1
#define KVI_CANVAS_RTTI_CONTROL_TYPE_LINE 2
#define KVI_CANVAS_RTTI_CONTROL_TYPE_POLYGON 4

#define KVI_CANVAS_RTTI_CONTROL_TYPE_MASK 255

#define KVI_CANVAS_RTTI_RECTANGLE (KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE | (1 << 8))
#define KVI_CANVAS_RTTI_RICHTEXT (KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE | (2 << 8))
#define KVI_CANVAS_RTTI_LINE (KVI_CANVAS_RTTI_CONTROL_TYPE_LINE | (3 << 8))
#define KVI_CANVAS_RTTI_ELLIPSE (KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE | (4 << 8))
#define KVI_CANVAS_RTTI_CHORD (KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE | (5 << 8))
#define KVI_CANVAS_RTTI_PIE (KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE | (6 << 8))
#define KVI_CANVAS_RTTI_POLYGON (KVI_CANVAS_RTTI_CONTROL_TYPE_POLYGON | (7 << 8))

#define KVI_CANVAS_RTTI_CONTROL_TYPE(__item) (__item->rtti() & KVI_CANVAS_RTTI_CONTROL_TYPE_MASK)

class KviCanvasPolygon : public QCanvasPolygon
{
public:
	KviCanvasPolygon(QCanvas * c, int x, int y, QPointArray & pnts, double dScaleFactor);
	virtual ~KviCanvasPolygon();

protected:
	QMap<QString, QVariant> m_properties;
	double m_dScaleFactor;
	QPointArray m_points;

public:
	virtual void draw(QPainter & p);
	double scaleFactor() { return m_dScaleFactor; };
	void setScaleFactor(double dScaleFactor);
	void resetPoints();
	const QPointArray & internalPoints() { return m_points; };
	void setInternalPoints(const QPointArray & pnts);
	QMap<QString, QVariant> * properties() { return &m_properties; };
	virtual void setProperty(const QString & property, const QVariant & val);
	virtual int rtti() const;
};

class KviCanvasRectangleItem : public QCanvasRectangle
{
	friend class KviCanvasRectangle;
	friend class KviCanvasRichText;
	friend class KviCanvasEllipticItem;

protected:
	KviCanvasRectangleItem(QCanvas * c, int x, int y, int w, int h);

public:
	virtual ~KviCanvasRectangleItem();

protected:
	QMap<QString, QVariant> m_properties;

public:
	int right() { return ((int)x()) + width(); };
	int bottom() { return ((int)y()) + height(); };
	QMap<QString, QVariant> * properties() { return &m_properties; };
	virtual void setProperty(const QString & property, const QVariant & val);
	//	QVariant property(const QString &name){ return m_properties[name]; };
protected:
	void drawSelection(QPainter & p);
};

class KviCanvasEllipticItem : public KviCanvasRectangleItem
{
	friend class KviCanvasEllipse;
	friend class KviCanvasChord;
	friend class KviCanvasPie;

protected:
	KviCanvasEllipticItem(QCanvas * c, int x, int y, int w, int h);
	~KviCanvasEllipticItem();

public:
	virtual void setProperty(const QString & property, const QVariant & val);
	virtual void draw(QPainter & p);
	virtual void drawContent(QPainter & p);
	virtual int rtti() const;
};

class KviCanvasEllipse : public KviCanvasEllipticItem
{
public:
	KviCanvasEllipse(QCanvas * c, int x, int y, int w, int h);
	~KviCanvasEllipse();

public:
	virtual void drawContent(QPainter & p);
	virtual int rtti() const;
};

class KviCanvasPie : public KviCanvasEllipticItem
{
public:
	KviCanvasPie(QCanvas * c, int x, int y, int w, int h);
	~KviCanvasPie();

public:
	virtual void drawContent(QPainter & p);
	virtual int rtti() const;
};

class KviCanvasChord : public KviCanvasEllipticItem
{
public:
	KviCanvasChord(QCanvas * c, int x, int y, int w, int h);
	~KviCanvasChord();

public:
	virtual void drawContent(QPainter & p);
	virtual int rtti() const;
};

class KviCanvasLine : public QCanvasLine
{
public:
	KviCanvasLine(QCanvas * c, int x1, int y1, int x2, int y2);
	virtual ~KviCanvasLine();

protected:
	QMap<QString, QVariant> m_properties;

public:
	QMap<QString, QVariant> * properties() { return &m_properties; };
	virtual void setProperty(const QString & property, const QVariant & val);
	//	QVariant property(const QString &name){ return m_properties[name]; };
	virtual void draw(QPainter & p);
	virtual int rtti() const;
};

class KviCanvasRectangle : public KviCanvasRectangleItem
{
public:
	KviCanvasRectangle(QCanvas * c, int x, int y, int w, int h);
	~KviCanvasRectangle();

public:
	virtual void draw(QPainter & p);
	virtual void setProperty(const QString & property, const QVariant & val);
	virtual int rtti() const;
};

class KviCanvasRichText : public KviCanvasRectangleItem
{
public:
	KviCanvasRichText(QCanvas * c, int x, int y, int w, int h);
	~KviCanvasRichText();

public:
	virtual void draw(QPainter & p);
	virtual int rtti() const;
};

class KviCanvasView : public QCanvasView
{
	Q_OBJECT
public:
	KviCanvasView(QCanvas * c, DccCanvasWidget * cw, QWidget * par);
	~KviCanvasView();

public:
	enum State
	{
		Idle,
		SelectOrigin
	};
	enum ObjectType
	{
		Rectangle,
		RichText,
		Line,
		Ellipse,
		Chord,
		Pie,
		PolygonTriangle,
		PolygonRectangle,
		PolygonPentagon,
		PolygonHexagon
	};
	enum DragMode
	{
		None,
		All,
		Left,
		Right,
		Top,
		Bottom,
		LeftTop,
		RightTop,
		LeftBottom,
		RightBottom,
		Scale,
		SinglePoint,
		Rotate
	};

protected:
	DccCanvasWidget * m_pCanvasWidget;

	// Insertion of objects
	State m_state;
	ObjectType m_objectToInsert;

	// Selected item
	QCanvasItem * m_pSelectedItem;

	DragMode m_dragMode;
	QPoint m_dragBegin;
	double m_dragScaleFactor;
	unsigned int m_dragPointIndex;
	QPointArray m_dragPointArray;

protected:
	void beginDragRectangle(KviCanvasRectangleItem * it, const QPoint & p, bool bInitial = false);
	void dragRectangle(KviCanvasRectangleItem * it, const QPoint & p);

	void beginDragLine(KviCanvasLine * it, const QPoint & p, bool bInitial = false);
	void dragLine(KviCanvasLine * it, const QPoint & p);

	void beginDragPolygon(KviCanvasPolygon * it, const QPoint & p, bool bShift = false, bool bCtrl = false);
	void dragPolygon(KviCanvasPolygon * it, const QPoint & p);

	void setItemSelected(QCanvasItem * it);
	void clearSelection();
	void insertObjectAt(const QPoint & pnt, ObjectType o);
	void contentsMousePressEvent(QMouseEvent * e) override;
	void contentsMouseMoveEvent(QMouseEvent * e) override;
	void contentsMouseReleaseEvent(QMouseEvent * e) override;
public slots:
	void insertRectangle();
	void insertRichText();
	void insertLine();
	void insertPie();
	void insertChord();
	void insertEllipse();
	void insertPolygonTriangle();
	void insertPolygonRectangle();
	void insertPolygonPentagon();
	void insertPolygonHexagon();
	void propertyChanged(const QString & s, const QVariant & v);
};

class KviVariantTableItem : public QTableItem
{
public:
	KviVariantTableItem(QTable * t, const QVariant & property);
	~KviVariantTableItem();

private:
	QVariant m_property;

public:
	virtual QWidget * createEditor() const;
	virtual void setContentFromEditor(QWidget * w);
	QVariant & property() { return m_property; };
	virtual void paint(QPainter * p, const QColorGroup & cg, const QRect & cr, bool selected);
};

class KviCanvasItemPropertiesWidget : public QTable
{
	Q_OBJECT
public:
	KviCanvasItemPropertiesWidget(QWidget * par);
	~KviCanvasItemPropertiesWidget();

public:
	void editItem(QCanvasItem * it);
protected slots:
	void cellEdited(int row, int col);
signals:
	void propertyChanged(const QString & s, const QVariant & v);
};

class DccCanvasWidget : public QWidget
{
	friend class KviCanvasView;
	Q_OBJECT
public:
	DccCanvasWidget(QWidget * par);
	~DccCanvasWidget();

protected:
	QSplitter * m_pSplitter;
	QCanvas * m_pCanvas;
	QMenuBar * m_pMenuBar;
	KviCanvasView * m_pCanvasView;
	QLabel * m_pStatusLabel;
	KviCanvasItemPropertiesWidget * m_pPropertiesWidget;

protected:
	void resizeEvent(QResizeEvent *) override;
};
#endif

#endif //_CANVAS_WIDGET_H_
