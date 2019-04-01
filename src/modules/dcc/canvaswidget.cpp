//=============================================================================
//
//   File : canvaswidget.cpp
//   Creation date : Mon Jul 30 07 2001 04:50:50 by Szymon Stefanek
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

#include "canvaswidget.h"

#ifdef COMPILE_DCC_CANVAS
#include "KviCString.h"
#include "KviLocale.h"

#include <QCursor>
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QValidator>
#include <QMenu>

#include <math.h>
#include <stdlib.h>

//
// KviCanvasRectangleItem
//

KviCanvasRectangleItem::KviCanvasRectangleItem(QCanvas * c, int x, int y, int w, int h)
    : QCanvasRectangle(x, y, w, h, c)
{
}

KviCanvasRectangleItem::~KviCanvasRectangleItem()
{
}

void KviCanvasRectangleItem::drawSelection(QPainter & p)
{
	p.setRasterOp(NotROP);
	p.fillRect((int)x() + 1, (int)y() + 1, width() - 2, height() - 2, QBrush(Dense6Pattern));
	p.setPen(QPen(DotLine));
	p.drawRect((int)x(), (int)y(), width(), height());
	p.setRasterOp(CopyROP);
}

void KviCanvasRectangleItem::setProperty(const QString & property, const QVariant & val)
{
	if(m_properties[property].isValid())
	{
		m_properties.replace(property, val);
		hide();
		show();
	}
}

//
// KviCanvasPolygon
//

KviCanvasPolygon::KviCanvasPolygon(QCanvas * c, int x, int y, QPointArray & pnts, double dScaleFactor)
    : QCanvasPolygon(c)
{
	m_properties.insert("clrForeground", QVariant(QColor(0, 0, 0)));
	m_properties.insert("uLineWidth", QVariant((unsigned int)0));

	m_properties.insert("clrBackground", QVariant(QColor(0, 0, 0)));
	m_properties.insert("bHasBackground", QVariant(false, 1));

	m_dScaleFactor = dScaleFactor;
	m_points = pnts;

	resetPoints();
	move(x, y);
}

KviCanvasPolygon::~KviCanvasPolygon()
{
}

void KviCanvasPolygon::setScaleFactor(double dScaleFactor)
{
	m_dScaleFactor = dScaleFactor;
	resetPoints();
}

void KviCanvasPolygon::setInternalPoints(const QPointArray & pnts)
{
	m_points = pnts;
	resetPoints();
}

void KviCanvasPolygon::resetPoints()
{
	QPointArray scaled(m_points.size());
	for(unsigned int i = 0; i < m_points.size(); i++)
	{
		int px;
		int py;
		m_points.point(i, &px, &py);
		px = (int)(px * m_dScaleFactor);
		py = (int)(py * m_dScaleFactor);
		scaled.setPoint(i, px, py);
	}
	setPoints(scaled);
}

int KviCanvasPolygon::rtti() const
{
	return KVI_CANVAS_RTTI_POLYGON;
}

void KviCanvasPolygon::setProperty(const QString & property, const QVariant & val)
{
	if(m_properties[property].isValid())
	{
		m_properties.replace(property, val);
		if((property == "clrForeground") || (property == "uLineWidth"))
		{
			setPen(QPen(m_properties["clrForeground"].asColor(), m_properties["uLineWidth"].toInt()));
		}
		else if((property == "clrBackground") || (property == "bHasBackground"))
		{
			if(m_properties["bHasBackground"].asBool())
				setBrush(QBrush(m_properties["clrBackground"].asColor()));
			else
				setBrush(QBrush());
		}
		else
		{
			hide();
			show();
		}
	}
}

void KviCanvasPolygon::draw(QPainter & p)
{
	if(isEnabled())
	{
		p.setBrush(brush());
		p.setPen(pen());
		p.drawPolygon(areaPoints());
	}

	if(isSelected())
	{
		p.setRasterOp(NotROP);
		p.setPen(QPen(DotLine));
		p.drawPolygon(areaPoints());
		p.setBrush(QBrush());
		double dVal = 10;
		p.drawEllipse((int)(x() - dVal), (int)(y() - dVal), (int)(dVal * 2), (int)(dVal * 2));
		p.drawLine((int)(x() - (dVal * 2)), (int)y(), (int)(x() + (dVal * 2)), (int)y());
		p.drawLine((int)x(), (int)(y() - (dVal * 2)), (int)x(), (int)(y() + (dVal * 2)));
		p.setRasterOp(CopyROP);
		canvas()->setChanged(QRect((int)(x() - dVal), (int)(y() - dVal), (int)(dVal * 4), (int)(dVal * 4)));
	}
}

//
// KviCanvasEllipticItem
//

KviCanvasEllipticItem::KviCanvasEllipticItem(QCanvas * c, int x, int y, int w, int h)
    : KviCanvasRectangleItem(c, x, y, w, h)
{
	m_properties.insert("clrForeground", QVariant(QColor(0, 0, 0)));
	m_properties.insert("uLineWidth", QVariant((unsigned int)0));

	m_properties.insert("clrBackground", QVariant(QColor(0, 0, 0)));
	m_properties.insert("bHasBackground", QVariant(false, 1));

	//	m_properties.insert("iStartAngle",QVariant(0));
	//	m_properties.insert("iEndAngle",QVariant(360));
}

KviCanvasEllipticItem::~KviCanvasEllipticItem()
{
}

void KviCanvasEllipticItem::draw(QPainter & p)
{
	if(isEnabled())
	{
		QBrush b = p.brush();
		if(m_properties["bHasBackground"].asBool())
			p.setBrush(m_properties["clrBackground"].asColor());
		else
			p.setBrush(QBrush());
		p.setPen(pen());
		drawContent(p);
		p.setBrush(b);
	}

	if(isSelected())
		drawSelection(p);
}

void KviCanvasEllipticItem::drawContent(QPainter & p)
{
}

void KviCanvasEllipticItem::setProperty(const QString & property, const QVariant & val)
{
	if(m_properties[property].isValid())
	{
		m_properties.replace(property, val);
		if((property == "clrForeground") || (property == "uLineWidth"))
		{
			setPen(QPen(m_properties["clrForeground"].asColor(), m_properties["uLineWidth"].toInt()));
		}
		else
		{
			hide();
			show();
		}
	}
}

int KviCanvasEllipticItem::rtti() const
{
	return KVI_CANVAS_RTTI_ELLIPSE;
}

//
// KviCanvasEllipse
//

KviCanvasEllipse::KviCanvasEllipse(QCanvas * c, int x, int y, int w, int h)
    : KviCanvasEllipticItem(c, x, y, w, h)
{
}

KviCanvasEllipse::~KviCanvasEllipse()
{
}

int KviCanvasEllipse::rtti() const
{
	return KVI_CANVAS_RTTI_ELLIPSE;
}

void KviCanvasEllipse::drawContent(QPainter & p)
{
	p.drawEllipse((int)x(), (int)y(), width(), height());
}

//
// KviCanvasPie
//

KviCanvasPie::KviCanvasPie(QCanvas * c, int x, int y, int w, int h)
    : KviCanvasEllipticItem(c, x, y, w, h)
{
	m_properties.insert("iStartAngle", QVariant((int)0));
	m_properties.insert("iExtensionAngle", QVariant((int)360));
}

KviCanvasPie::~KviCanvasPie()
{
}

int KviCanvasPie::rtti() const
{
	return KVI_CANVAS_RTTI_PIE;
}

void KviCanvasPie::drawContent(QPainter & p)
{
	int iStartAngle = m_properties["iStartAngle"].asInt() * 16;
	int iEndAngle = m_properties["iExtensionAngle"].asInt() * 16;
	p.drawPie((int)x(), (int)y(), width(), height(), iStartAngle, iEndAngle);
}

//
// KviCanvasChord
//

KviCanvasChord::KviCanvasChord(QCanvas * c, int x, int y, int w, int h)
    : KviCanvasEllipticItem(c, x, y, w, h)
{
	m_properties.insert("iStartAngle", QVariant((int)0));
	m_properties.insert("iExtensionAngle", QVariant((int)360));
}

KviCanvasChord::~KviCanvasChord()
{
}

int KviCanvasChord::rtti() const
{
	return KVI_CANVAS_RTTI_CHORD;
}

void KviCanvasChord::drawContent(QPainter & p)
{
	int iStartAngle = m_properties["iStartAngle"].asInt() * 16;
	int iEndAngle = m_properties["iExtensionAngle"].asInt() * 16;
	p.drawChord((int)x(), (int)y(), width(), height(), iStartAngle, iEndAngle);
}

//
// KviCanvasRectangle
//

KviCanvasRectangle::KviCanvasRectangle(QCanvas * c, int x, int y, int w, int h)
    : KviCanvasRectangleItem(c, x, y, w, h)
{
	m_properties.insert("clrForeground", QVariant(QColor(0, 0, 0)));
	m_properties.insert("uLineWidth", QVariant((unsigned int)0));

	m_properties.insert("clrBackground", QVariant(QColor(0, 0, 0)));
	m_properties.insert("bHasBackground", QVariant(false, 1));
}

KviCanvasRectangle::~KviCanvasRectangle()
{
}

int KviCanvasRectangle::rtti() const
{
	return KVI_CANVAS_RTTI_RECTANGLE;
}

void KviCanvasRectangle::setProperty(const QString & property, const QVariant & val)
{
	if(m_properties[property].isValid())
	{
		m_properties.replace(property, val);
		if((property == "clrForeground") || (property == "uLineWidth"))
		{
			setPen(QPen(m_properties["clrForeground"].asColor(), m_properties["uLineWidth"].toInt()));
		}
		else
		{
			hide();
			show();
		}
	}
}

void KviCanvasRectangle::draw(QPainter & p)
{
	if(isEnabled())
	{
		if(m_properties["bHasBackground"].asBool())
		{
			p.fillRect((int)x() + 1, (int)y() + 1, width() - 2, height() - 2, m_properties["clrBackground"].asColor());
		}
		p.setPen(pen());
		p.drawRect((int)x(), (int)y(), width(), height());
	}

	if(isSelected())
		drawSelection(p);
}

//
// KviCanvasRichText
//

KviCanvasRichText::KviCanvasRichText(QCanvas * c, int x, int y, int w, int h)
    : KviCanvasRectangleItem(c, x, y, w, h)
{
	QFont f = QFont();
	f.setStyleHint(QFont::SansSerif);
	f.setPointSize(12);
	m_properties.insert("szText", QVariant(QString("Insert here your <font color=\"#FF0000\"><b>RICH TEXT</b></font>")));
	m_properties.insert("fntDefault", QVariant(f));
}

KviCanvasRichText::~KviCanvasRichText()
{
}

int KviCanvasRichText::rtti() const
{
	return KVI_CANVAS_RTTI_RICHTEXT;
}

void KviCanvasRichText::draw(QPainter & p)
{
	if(isEnabled())
	{
		QString szText = m_properties["szText"].asString();
		QSimpleRichText text(szText, m_properties["fntDefault"].asFont());
		text.setWidth(width());
		text.draw(&p, (int)x() + 1, (int)y() + 1, QRegion(QRect((int)x() + 1, (int)y() + 1, width(), height())), QColorGroup());
	}

	if(isSelected())
		drawSelection(p);
}

//
// KviCanvasLine
//

KviCanvasLine::KviCanvasLine(QCanvas * c, int x1, int y1, int x2, int y2)
    : QCanvasLine(c)
{
	setPoints(x1, y1, x2, y2);
	m_properties.insert("uLineWidth", QVariant((unsigned int)0));
	m_properties.insert("clrForeground", QVariant(QColor()));
}

KviCanvasLine::~KviCanvasLine()
{
}

void KviCanvasLine::setProperty(const QString & property, const QVariant & val)
{
	m_properties.replace(property, val);
	if((property == "uLineWidth") || (property == "clrForeground"))
	{
		setPen(QPen(m_properties["clrForeground"].asColor(), m_properties["uLineWidth"].toInt()));
	}
}

int KviCanvasLine::rtti() const
{
	return KVI_CANVAS_RTTI_LINE;
}

void KviCanvasLine::draw(QPainter & p)
{
	if(isEnabled())
	{
		p.setPen(pen());
		p.drawLine(startPoint(), endPoint());
	}

	if(isSelected())
	{
		p.setRasterOp(NotROP);
		p.setPen(QPen(DotLine));
		p.drawLine(startPoint(), endPoint());
		p.setRasterOp(CopyROP);
	}
}

//
// KviCanvasView
//

KviCanvasView::KviCanvasView(QCanvas * c, DccCanvasWidget * cw, QWidget * par)
    : QCanvasView(c, par)
{
	m_pCanvasWidget = cw;
	m_state = Idle;
	m_dragMode = None;
	m_pSelectedItem = nullptr;
	viewport()->setMouseTracking(true);
}

KviCanvasView::~KviCanvasView()
{
}

void KviCanvasView::insertRectangle()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = Rectangle;
}

void KviCanvasView::insertRichText()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = RichText;
}

void KviCanvasView::insertLine()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = Line;
}

void KviCanvasView::insertEllipse()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = Ellipse;
}

void KviCanvasView::insertPie()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = Pie;
}

void KviCanvasView::insertChord()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = Chord;
}

void KviCanvasView::insertPolygonTriangle()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = PolygonTriangle;
}

void KviCanvasView::insertPolygonRectangle()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = PolygonRectangle;
}

void KviCanvasView::insertPolygonPentagon()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = PolygonPentagon;
}

void KviCanvasView::insertPolygonHexagon()
{
	m_state = SelectOrigin;
	setCursor(crossCursor);
	m_objectToInsert = PolygonPentagon;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void calcPolygonPoints(QPointArray & pnts, unsigned int nVertices)
{
	double dDelta = (2 * M_PI) / nVertices;
	for(unsigned int i = 0; i < nVertices; i++)
	{
		double dAng = dDelta * i;
		double theX = 300 * sin(dAng);
		double theY = 300 * cos(dAng);
		pnts.setPoint(i, (int)theX, (int)theY);
	}
}

void KviCanvasView::insertObjectAt(const QPoint & pnt, ObjectType o)
{
	QCanvasItem * r = nullptr;

	switch(o)
	{
		case Rectangle:
			r = new KviCanvasRectangle(canvas(), pnt.x(), pnt.y(), 0, 0);
			break;
		case RichText:
			r = new KviCanvasRichText(canvas(), pnt.x(), pnt.y(), 0, 0);
			break;
		case Line:
			r = new KviCanvasLine(canvas(), pnt.x(), pnt.y(), pnt.x(), pnt.y());
			break;
		case Ellipse:
			r = new KviCanvasEllipse(canvas(), pnt.x(), pnt.y(), 0, 0);
			break;
		case Pie:
			r = new KviCanvasPie(canvas(), pnt.x(), pnt.y(), 0, 0);
			break;
		case Chord:
			r = new KviCanvasChord(canvas(), pnt.x(), pnt.y(), 0, 0);
			break;
		case PolygonTriangle:
		{
			QPointArray pa(3);
			pa.setPoint(0, 0, -500);
			pa.setPoint(1, -450, 220);
			pa.setPoint(2, 450, 220);
			r = new KviCanvasPolygon(canvas(), pnt.x(), pnt.y(), pa, 0.1);
		}
		break;
		case PolygonRectangle:
		{
			QPointArray pa(4);
			pa.setPoint(0, -350, -350);
			pa.setPoint(1, 350, -350);
			pa.setPoint(2, 350, 350);
			pa.setPoint(3, -350, 350);
			r = new KviCanvasPolygon(canvas(), pnt.x(), pnt.y(), pa, 0.1);
		}
		break;
		case PolygonPentagon:
		{
			QPointArray pa(5);
			calcPolygonPoints(pa, 5);
			r = new KviCanvasPolygon(canvas(), pnt.x(), pnt.y(), pa, 0.1);
		}
		break;
		case PolygonHexagon:
		{
			QPointArray pa(6);
			calcPolygonPoints(pa, 6);
			r = new KviCanvasPolygon(canvas(), pnt.x(), pnt.y(), pa, 0.1);
		}
		break;
	}

	if(r)
	{
		setItemSelected(r);
		r->setEnabled(true);
		r->show();
	}

	switch(KVI_CANVAS_RTTI_CONTROL_TYPE(r))
	{
		case KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE:
			beginDragRectangle((KviCanvasRectangleItem *)r, pnt, true);
			break;
		case KVI_CANVAS_RTTI_CONTROL_TYPE_LINE:
			beginDragLine((KviCanvasLine *)r, pnt, true);
			break;
			//		case KVI_CANVAS_RTTI_CONTROL_TYPE_POLYGON:
			//			beginDragPolygon((KviCanvasPolygon *)r,pnt,true);
			//		break;
	}

	//	canvas()->update();
}

void KviCanvasView::setItemSelected(QCanvasItem * it)
{
	clearSelection();
	it->setSelected(true);
	m_pSelectedItem = it;
	m_pCanvasWidget->m_pPropertiesWidget->editItem(it);
}

void KviCanvasView::clearSelection()
{
	if(!m_pSelectedItem)
		return;
	m_pSelectedItem->setSelected(false);
	m_pSelectedItem = nullptr;
	m_pCanvasWidget->m_pPropertiesWidget->editItem(0);
}

void KviCanvasView::beginDragLine(KviCanvasLine * it, const QPoint & p, bool bInitial)
{
	QPoint sp = it->startPoint();

	m_dragBegin = p - sp;

	if(bInitial)
	{
		m_dragMode = Bottom;
		setCursor(sizeAllCursor);
		return;
	}

	if((abs(p.x() - sp.x()) < 3) && (abs(p.y() - sp.y()) < 3))
	{
		m_dragMode = Top;
		setCursor(sizeAllCursor);
		return;
	}

	sp = it->endPoint();
	if((abs(p.x() - sp.x()) < 3) && (abs(p.y() - sp.y()) < 3))
	{
		m_dragMode = Bottom;
		setCursor(sizeAllCursor);
		return;
	}

	m_dragMode = All;
	setCursor(pointingHandCursor);
}

void KviCanvasView::dragLine(KviCanvasLine * it, const QPoint & p)
{
	switch(m_dragMode)
	{
		case Bottom:
		{
			QPoint sp = it->startPoint();
			it->setPoints(sp.x(), sp.y(), p.x(), p.y());
		}
		break;
		case Top:
		{
			QPoint ep = it->endPoint();
			it->setPoints(p.x(), p.y(), ep.x(), ep.y());
		}
		break;
		case All:
		{
			QPoint sp = p - m_dragBegin;
			QPoint ep = sp + (it->endPoint() - it->startPoint());
			it->setPoints(sp.x(), sp.y(), ep.x(), ep.y());
		}
		break;
		default: /* make gcc happy */
			break;
	}
	canvas()->update();
}

static double ssm_2d_rotationAngleFromXAxis(double dx, double dy)
{
	//
	//      v1 . v2           dx * 1 + dy * 0              dx
	//acos(---------) = acos(-----------------) = acos(---------)
	//     |v1||v2|            |(dx,dy)| * 1           |(dx,dy)|
	//

	//double dVal = hypot(dx,dy);
	double dVal = sqrt((dx * dx) + (dy * dy));

	if(dVal == 0.0)
		return 0; // ???

	dVal = acos(dx / dVal);

	return (dy > 0.0) ? dVal : -dVal;
}

static double ssm_2d_rotationAngle(double drefx, double drefy, double drotatedx, double drotatedy)
{
	double dRefAngle = ssm_2d_rotationAngleFromXAxis(drefx, drefy);
	double dRotAngle = ssm_2d_rotationAngleFromXAxis(drotatedx, drotatedy);
	return dRotAngle - dRefAngle;
}

static void ssm_2d_rotate(double & dx, double & dy, double dAngle)
{
	// Rotation matrix:
	//
	// | cos(x)    sin(x) |
	// |                  |
	// | -sin(x)   cos(x) |

	double s = sin(dAngle);
	double c = cos(dAngle);

	double tmpX = (dx * c) - (dy * s);
	double tmpY = (dx * s) + (dy * c);

	dx = tmpX;
	dy = tmpY;
}

static double ssm_hypot(double a, double b)
{
	return sqrt((a * a) + (b * b));
}

void KviCanvasView::beginDragPolygon(KviCanvasPolygon * it, const QPoint & p, bool bShift, bool bCtrl)
{
	m_dragBegin = QPoint((int)(p.x() - it->x()), (int)(p.y() - it->y()));

	QPointArray pa = it->areaPoints();

	for(unsigned int i = 0; i < pa.size(); i++)
	{
		QPoint pnt = pa.point(i);
		double dX = pnt.x() - p.x();
		double dY = pnt.y() - p.y();
		double dHypot = sqrt((dX * dX) + (dY * dY));
		if(dHypot < 3.0)
		{
			// We're dragging a point
			m_dragMode = SinglePoint;
			m_dragPointIndex = i;
			setCursor(crossCursor);
			return;
		}
	}

	if(bShift)
	{
		m_dragMode = Scale;
		m_dragScaleFactor = it->scaleFactor();
		setCursor(sizeAllCursor);
		return;
	}

	if(bCtrl)
	{
		m_dragMode = Rotate;
		m_dragPointArray = it->internalPoints();
		//		qDebug("Here");
		setCursor(sizeHorCursor);
		return;
	}

	m_dragMode = All;
	setCursor(pointingHandCursor);
}

void KviCanvasView::dragPolygon(KviCanvasPolygon * it, const QPoint & p)
{
	switch(m_dragMode)
	{
		case All:
			it->move(p.x() - m_dragBegin.x(), p.y() - m_dragBegin.y());
			break;
		case SinglePoint:
		{
			QPointArray pnt = it->internalPoints();
			pnt.setPoint(m_dragPointIndex, (int)((p.x() - it->x()) / it->scaleFactor()), (int)((p.y() - it->y()) / it->scaleFactor()));
			it->setInternalPoints(pnt);
		}
		break;
		case Scale:
		{
			double dDistance = ssm_hypot(p.x() - it->x(), p.y() - it->y());
			double dOriginal = ssm_hypot(m_dragBegin.x(), m_dragBegin.y());
			if(dOriginal < 1)
				dOriginal = 1;
			if(dDistance < 0.1)
				dDistance = 0.1;
			it->setScaleFactor(m_dragScaleFactor * dDistance / dOriginal);
		}
		break;
		case Rotate:
		{
			QPoint act((int)(p.x() - it->x()), (int)(p.y() - it->y()));
			double dAngle = ssm_2d_rotationAngle(m_dragBegin.x(), m_dragBegin.y(), act.x(), act.y());
			//			qDebug("%d,%d %d,%d %f",m_dragBegin.x(),m_dragBegin.y(),act.x(),act.y(),dAngle);
			QPointArray thePoints = m_dragPointArray.copy();
			for(unsigned int i = 0; i < thePoints.size(); i++)
			{
				QPoint tmp = thePoints.point(i);
				double dx = tmp.x();
				double dy = tmp.y();
				ssm_2d_rotate(dx, dy, dAngle);
				thePoints.setPoint(i, (int)dx, (int)dy);
			}
			it->setInternalPoints(thePoints);
		}
		break;
		default:
			break;
	}
	canvas()->update();
}

void KviCanvasView::beginDragRectangle(KviCanvasRectangleItem * it, const QPoint & p, bool bInitial)
{
	m_dragBegin = QPoint((int)(p.x() - it->x()), (int)(p.y() - it->y()));

	if(bInitial)
	{
		// Right bottom
		m_dragMode = RightBottom;
		setCursor(sizeFDiagCursor);
		return;
	}

	if(p.x() < (((int)it->x()) + 2))
	{
		// Left edge
		if(p.y() < (((int)it->y()) + 2))
		{
			// Left top
			m_dragMode = LeftTop;
			setCursor(sizeFDiagCursor);
			return;
		}
		if(p.y() > (it->bottom() - 2))
		{
			// Left bottom
			m_dragMode = LeftBottom;
			setCursor(sizeBDiagCursor);
			return;
		}
		m_dragMode = Left;
		setCursor(sizeHorCursor);
		return;
	}

	if(p.x() > (it->right() - 2))
	{
		// Right edge
		if(p.y() < (((int)it->y()) + 2))
		{
			// Right top
			m_dragMode = RightTop;
			setCursor(sizeBDiagCursor);
			return;
		}
		if(p.y() > (it->bottom() - 2))
		{
			// Right bottom
			m_dragMode = RightBottom;
			setCursor(sizeFDiagCursor);
			return;
		}
		m_dragMode = Right;
		setCursor(sizeHorCursor);
		return;
	}

	// Somewhere in the middle
	if(p.y() < (((int)it->y()) + 2))
	{
		// Top
		m_dragMode = Top;
		setCursor(sizeVerCursor);
		return;
	}
	if(p.y() > (it->bottom() - 2))
	{
		// Bottom
		m_dragMode = Bottom;
		setCursor(sizeVerCursor);
		return;
	}

	m_dragMode = All;
	setCursor(pointingHandCursor);
}

void KviCanvasView::dragRectangle(KviCanvasRectangleItem * it, const QPoint & p)
{

	int aux1, aux2, aux3, aux4;

	switch(m_dragMode)
	{
		case All:
			it->move(p.x() - m_dragBegin.x(), p.y() - m_dragBegin.y());
			break;
		case Left:
			aux1 = it->width() + (int)(it->x() - p.x());
			aux2 = p.x();
			if(aux1 < 1)
			{
				aux2 += (aux1 - 1);
				aux1 = 1;
			}
			it->move(aux2, it->y());
			it->setSize(aux1, it->height());
			break;
		case Right:
			aux1 = it->width() + (p.x() - it->right());
			if(aux1 < 1)
				aux1 = 1;
			it->setSize(aux1, it->height());
			break;
		case Top:
			aux1 = it->height() + (int)(it->y() - p.y());
			aux2 = p.y();
			if(aux1 < 1)
			{
				aux2 += (aux1 - 1);
				aux1 = 1;
			}
			it->move(it->x(), aux2);
			it->setSize(it->width(), aux1);
			break;
		case Bottom:
			aux1 = (int)it->height() + (p.y() - it->bottom());
			if(aux1 < 1)
				aux1 = 1;
			it->setSize(it->width(), aux1);
			break;
		case LeftTop:
			aux1 = it->width() + (int)(it->x() - p.x());
			aux3 = p.x();
			if(aux1 < 1)
			{
				aux3 += (aux1 - 1);
				aux1 = 1;
			}
			aux2 = it->height() + (int)(it->y() - p.y());
			aux4 = p.y();
			if(aux2 < 1)
			{
				aux4 += (aux2 - 1);
				aux2 = 1;
			}
			it->setSize(aux1, aux2);
			it->move(aux3, aux4);
			break;
		case RightTop:
			aux1 = it->width() + (int)(p.x() - it->right());
			if(aux1 < 1)
				aux1 = 1;
			aux2 = it->height() + (int)(it->y() - p.y());
			aux4 = p.y();
			if(aux2 < 1)
			{
				aux4 += (aux2 - 1);
				aux2 = 1;
			}
			it->setSize(aux1, aux2);
			it->move(it->x(), aux4);
			break;
		case LeftBottom:
			aux1 = it->width() + (int)(it->x() - p.x());
			aux3 = p.x();
			if(aux1 < 1)
			{
				aux3 += (aux1 - 1);
				aux1 = 1;
			}
			aux2 = it->height() + (int)(p.y() - it->bottom());
			if(aux2 < 1)
				aux2 = 1;
			it->setSize(aux1, aux2);
			it->move(aux3, it->y());
			break;
		case RightBottom:
			aux1 = it->width() + (int)(p.x() - it->right());
			if(aux1 < 1)
				aux1 = 1;
			aux2 = it->height() + (int)(p.y() - it->bottom());
			if(aux2 < 1)
				aux2 = 1;
			it->setSize(aux1, aux2);
			break;
		default:
			break;
	}

	canvas()->update();
}

void KviCanvasView::contentsMouseMoveEvent(QMouseEvent * e)
{
	//	QPoint p = inverseWorldMatrix().map(e->pos());
	QPoint p = e->pos();
	if(e->state() & Qt::LeftButton)
	{
		if((m_dragMode != None) && (m_pSelectedItem))
		{
			if(m_pSelectedItem->isEnabled())
				m_pSelectedItem->setEnabled(false);
			switch(KVI_CANVAS_RTTI_CONTROL_TYPE(m_pSelectedItem))
			{
				case KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE:
					dragRectangle((KviCanvasRectangleItem *)m_pSelectedItem, p);
					break;
				case KVI_CANVAS_RTTI_CONTROL_TYPE_LINE:
					dragLine((KviCanvasLine *)m_pSelectedItem, p);
					break;
				case KVI_CANVAS_RTTI_CONTROL_TYPE_POLYGON:
					dragPolygon((KviCanvasPolygon *)m_pSelectedItem, p);
					break;
			}
		}
	}
	else
	{
		// Without buttons
		if(m_state == Idle)
		{
			QCanvasItemList l = canvas()->collisions(p);
			QCanvasItemList::Iterator it = l.begin();

			if(it != l.end())
			{
				// Got an item
				QCanvasItem * hit = (QCanvasItem *)*it;
				// Now find the point on that we have clicked it
				if(hit == m_pSelectedItem)
				{
					switch(KVI_CANVAS_RTTI_CONTROL_TYPE(m_pSelectedItem))
					{
						case KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE:
							beginDragRectangle((KviCanvasRectangleItem *)m_pSelectedItem, p);
							break;
						case KVI_CANVAS_RTTI_CONTROL_TYPE_LINE:
							beginDragLine((KviCanvasLine *)m_pSelectedItem, p);
							break;
						case KVI_CANVAS_RTTI_CONTROL_TYPE_POLYGON:
							beginDragPolygon((KviCanvasPolygon *)m_pSelectedItem, p);
							break;
					}
				}
				else if(m_dragMode != None)
					setCursor(arrowCursor);
			}
			else
			{
				if(m_dragMode != None)
					setCursor(arrowCursor);
			}
		}
	}
}

void KviCanvasView::contentsMouseReleaseEvent(QMouseEvent * e)
{
	if(m_dragMode != None)
	{
		// Was just dragging a rectangle
		m_dragMode = None;
		setCursor(arrowCursor);
		if(m_pSelectedItem)
		{
			m_pSelectedItem->setEnabled(true);
			canvas()->update();
		}
	}
}

void KviCanvasView::contentsMousePressEvent(QMouseEvent * e)
{
	if(e->button() & Qt::LeftButton)
	{
		//	    QPoint p = inverseWorldMatrix().map(e->pos());
		QPoint p = e->pos();

		switch(m_state)
		{
			case SelectOrigin:
				clearSelection();
				setCursor(arrowCursor);
				m_state = Idle;
				insertObjectAt(p, m_objectToInsert);
				canvas()->update();
				break;

			case Idle:
			{
				QCanvasItemList l = canvas()->collisions(p);
				QCanvasItemList::Iterator it = l.begin();

				if(it != l.end())
				{
					// Got an item
					QCanvasItem * hit = *it;
					if(hit != m_pSelectedItem)
					{
						// Was not selected yet
						setItemSelected(hit);
						canvas()->update();
					}
					// Now find the point on that we have clicked it
					switch(KVI_CANVAS_RTTI_CONTROL_TYPE(hit))
					{
						case KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE:
							beginDragRectangle(((KviCanvasRectangleItem *)hit), p);
							break;
						case KVI_CANVAS_RTTI_CONTROL_TYPE_LINE:
							beginDragLine(((KviCanvasLine *)hit), p);
							break;
						case KVI_CANVAS_RTTI_CONTROL_TYPE_POLYGON:
							beginDragPolygon(((KviCanvasPolygon *)hit), p, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
							break;
					}
				}
				else
				{
					// No item
					clearSelection();
					canvas()->update();
				}
			}
			break;
		}
	}
}

void KviCanvasView::propertyChanged(const QString & s, const QVariant & v)
{
	if(!m_pSelectedItem)
		return;

	switch(KVI_CANVAS_RTTI_CONTROL_TYPE(m_pSelectedItem))
	{
		case KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE:
			((KviCanvasRectangleItem *)m_pSelectedItem)->setProperty(s, v);
			break;
		case KVI_CANVAS_RTTI_CONTROL_TYPE_LINE:
			((KviCanvasLine *)m_pSelectedItem)->setProperty(s, v);
			break;
		case KVI_CANVAS_RTTI_CONTROL_TYPE_POLYGON:
			((KviCanvasPolygon *)m_pSelectedItem)->setProperty(s, v);
			break;
	}

	canvas()->update();
}

//
// KviVariantTableItem
//

KviVariantTableItem::KviVariantTableItem(QTable * t, const QVariant & property)
    : QTableItem(t, QTableItem::WhenCurrent, QString())
{
	m_property = property;
}

KviVariantTableItem::~KviVariantTableItem()
{
}

QWidget * KviVariantTableItem::createEditor() const
{
	switch(m_property.type())
	{
		case QVariant::String:
		{
			QLineEdit * e = new QLineEdit(table()->viewport());
			e->setText(m_property.toString());
			return e;
		}
		break;
		case QVariant::Int:
		{
			QLineEdit * e = new QLineEdit(table()->viewport());
			QString sz;
			sz.setNum(m_property.toInt());
			e->setText(sz);
			e->setValidator(new QIntValidator(e));
			return e;
		}
		break;
		case QVariant::UInt:
		{
			QLineEdit * e = new QLineEdit(table()->viewport());
			QString sz;
			sz.setNum(m_property.toInt());
			e->setText(sz);
			QIntValidator * i = new QIntValidator(e);
			i->setBottom(0);
			e->setValidator(i);
			return e;
		}
		break;
		case QVariant::Bool:
		{
			QComboBox * b = new QComboBox(false, table()->viewport());
			b->addAction("false");
			b->addAction("true");
			b->setCurrentItem(m_property.toBool() ? 1 : 0);
			return b;
		}
		break;
		case QVariant::Color:
		{
			QLineEdit * e = new QLineEdit(table()->viewport());
			e->setText(m_property.toColor().name());
			return e;
		}
		break;
		case QVariant::Font:
		{
			QComboBox * b = new QComboBox(true, table()->viewport());
			QString tmp;
			QString tmpDefault;
			QFont f = QFont();
			f.setStyleHint(QFont::SansSerif);
			tmpDefault = f.family();
			f.setStyleHint(QFont::TypeWriter);
			tmp.setNum(m_property.toFont().pointSize());
			tmp.prepend(", ");
			tmp.prepend(m_property.toFont().family());
			b->addAction(tmp);
			b->addAction(tmpDefault + ", 8");
			b->addAction(tmpDefault + ", 10");
			b->addAction(tmpDefault + ", 12");
			b->addAction(tmpDefault + ", 14");
			b->addAction(tmpDefault + ", 16");
			b->addAction(tmpDefault + ", 18");
			b->addAction(tmpDefault + ", 20");
			b->addAction(tmpDefault + ", 24");
			b->addAction(tmpDefault + ", 28");
			b->addAction(tmpDefault + ", 32");
			b->addAction(tmpDefault + ", 40");
			b->addAction(tmpDefault + ", 48");
			b->addAction(f.family() + ", 12");
			b->setCurrentItem(0);
			b->setCurrentItem(m_property.toBool() ? 1 : 0);
			return b;
		}
		break;
		default:
			break;
	}
	return nullptr;
}

void KviVariantTableItem::setContentFromEditor(QWidget * w)
{
	switch(m_property.type())
	{
		case QVariant::String:
			m_property = QVariant(((QLineEdit *)w)->text());
			break;
		case QVariant::Int:
			m_property = QVariant(((QLineEdit *)w)->text().toInt());
			break;
		case QVariant::UInt:
			m_property = QVariant(((QLineEdit *)w)->text().toUInt());
			break;
		case QVariant::Bool:
			m_property = QVariant(((QComboBox *)w)->currentItem(), 1);
			break;
		case QVariant::Color:
			m_property.asColor().setNamedColor(((QLineEdit *)w)->text());
			break;
		case QVariant::Font:
		{
			KviCString txt = ((QComboBox *)w)->currentText();
			if(txt.hasData())
			{
				KviCString fam = txt;
				fam.cutFromFirst(',', true);
				fam.trimmed();
				KviCString psz = txt;
				psz.cutToFirst(',', true);
				psz.trimmed();
				bool bOk;
				unsigned int uSize = psz.toUInt(&bOk);
				if(!bOk)
					uSize = 12;
				m_property = QVariant(QFont(fam.ptr(), uSize));
			}
		}
		break;
		default:
			break;
	}
}

void KviVariantTableItem::paint(QPainter * p, const QColorGroup & cg, const QRect & cr, bool)
{
	p->fillRect(0, 0, cr.width(), cr.height(), cg.base());

	if(m_property.type() == QVariant::Color)
	{
		p->fillRect(3, 3, cr.width() - 6, cr.height() - 6, m_property.asColor());
	}
	else
	{
		QString sz;
		switch(m_property.type())
		{
			case QVariant::String:
				sz = m_property.toString();
				break;
			case QVariant::Bool:
				sz = m_property.toBool() ? "true" : "false";
				break;
			case QVariant::Font:
				sz.setNum(m_property.toFont().pointSize());
				sz.prepend(", ");
				sz.prepend(m_property.toFont().family());
				break;
			case QVariant::Int:
				sz.setNum(m_property.toInt());
				break;
			case QVariant::UInt:
				sz.setNum(m_property.toUInt());
				break;
			default:
				break;
		}
		p->setPen(cg.text());
		p->drawText(0, 0, cr.width(), cr.height(), Qt::AlignLeft | Qt::AlignTop, sz);
	}
}

//
// KviCanvasItemPropertiesWidget
//

KviCanvasItemPropertiesWidget::KviCanvasItemPropertiesWidget(QWidget * par)
    : QTable(par)
{
	setSelectionMode(QTable::NoSelection);
	setColumnMovingEnabled(false);
	setRowMovingEnabled(false);
	setShowGrid(true);
	setNumCols(2);
	setSorting(false);
	setLeftMargin(0);
	verticalHeader()->hide();
	connect(this, SIGNAL(valueChanged(int, int)), this, SLOT(cellEdited(int, int)));
}

KviCanvasItemPropertiesWidget::~KviCanvasItemPropertiesWidget()
{
}

void KviCanvasItemPropertiesWidget::cellEdited(int row, int)
{
	QTableItem * it = item(row, 0);
	if(!it)
		return;
	QString szName = it->text();
	it = item(row, 1);
	if(!it)
		return;
	emit propertyChanged(szName, ((KviVariantTableItem *)it)->property());
}

void KviCanvasItemPropertiesWidget::editItem(QCanvasItem * it)
{
	if(!it)
	{
		for(int i = 0; i < numRows(); i++)
		{
			clearCell(i, 0);
			clearCell(i, 1);
			clearCellWidget(i, 1);
		}
		setNumRows(0);
		return;
	}

	QMap<QString, QVariant> * m = nullptr;

	switch(KVI_CANVAS_RTTI_CONTROL_TYPE(it))
	{
		case KVI_CANVAS_RTTI_CONTROL_TYPE_RECTANGLE:
			m = ((KviCanvasRectangleItem *)it)->properties();
			break;
		case KVI_CANVAS_RTTI_CONTROL_TYPE_LINE:
			m = ((KviCanvasLine *)it)->properties();
			break;
		case KVI_CANVAS_RTTI_CONTROL_TYPE_POLYGON:
			m = ((KviCanvasPolygon *)it)->properties();
			break;
	}

	if(!m)
	{
		editItem(0);
		return;
	}

	for(int i = 0; i < numRows(); i++)
	{
		clearCell(i, 0);
		clearCell(i, 1);
		clearCellWidget(i, 1);
	}

	setNumRows(m->count());

	QTableItem * item;

	int idx = 0;

	for(QMap<QString, QVariant>::ConstIterator iter = m->begin(); iter != m->end(); ++iter)
	{
		item = new QTableItem(this, QTableItem::Never, iter.key().toUtf8().data());
		setItem(idx, 0, item);
		item = new KviVariantTableItem(this, iter.data());
		setItem(idx, 1, item);
		idx++;
	}
}

//
// DccCanvasWidget
//

DccCanvasWidget::DccCanvasWidget(QWidget * par)
    : QWidget(par, "canvas_widget")
{
	m_pCanvas = new QCanvas(this);
	//#warning "Make this size as parameter of Dcc ?"
	m_pCanvas->resize(648, 480);
	m_pMenuBar = new QMenuBar(this);
	m_pSplitter = new QSplitter(QSplitter::Horizontal, this);
	m_pCanvasView = new KviCanvasView(m_pCanvas, this, m_pSplitter);
	m_pStatusLabel = new QLabel(this);
	m_pPropertiesWidget = new KviCanvasItemPropertiesWidget(m_pSplitter);
	QValueList<int> l;
	l.append(80);
	l.append(20);
	m_pSplitter->setSizes(l);

	connect(m_pPropertiesWidget, SIGNAL(propertyChanged(const QString &, const QVariant &)), m_pCanvasView, SLOT(propertyChanged(const QString &, const QVariant &)));

	QMenu * add = new QMenu(m_pMenuBar);
	QMenu * shapes = new QMenu(add);
	QMenu * polygons = new QMenu(add);
	QMenu * items = new QMenu(add);
	shapes->addAction(__tr2qs_ctx("&Line", "dcc"), m_pCanvasView, SLOT(insertLine()));
	shapes->addAction(__tr2qs_ctx("&Rectangle", "dcc"), m_pCanvasView, SLOT(insertRectangle()));
	shapes->addAction(__tr2qs_ctx("&Ellipse", "dcc"), m_pCanvasView, SLOT(insertEllipse()));
	shapes->addAction(__tr2qs_ctx("&Pie", "dcc"), m_pCanvasView, SLOT(insertPie()));
	shapes->addAction(__tr2qs_ctx("&Chord", "dcc"), m_pCanvasView, SLOT(insertChord()));

	items->addAction(__tr2qs_ctx("&Rich text (html)", "dcc"), m_pCanvasView, SLOT(insertRichText()));

	polygons->addAction(__tr2qs_ctx("&Triangle", "dcc"), m_pCanvasView, SLOT(insertPolygonTriangle()));
	polygons->addAction(__tr2qs_ctx("&Rectangle", "dcc"), m_pCanvasView, SLOT(insertPolygonRectangle()));
	polygons->addAction(__tr2qs_ctx("&Pentagon", "dcc"), m_pCanvasView, SLOT(insertPolygonPentagon()));
	polygons->addAction(__tr2qs_ctx("&Hexagon", "dcc"), m_pCanvasView, SLOT(insertPolygonHexagon()));

	add->addAction(__tr2qs_ctx("&Shape", "dcc"), shapes);
	add->addAction(__tr2qs_ctx("&Item", "dcc"), items);
	add->addAction(__tr2qs_ctx("&Polygons", "dcc"), polygons);

	m_pMenuBar->addAction(__tr2qs_ctx("&Insert", "dcc"), add);
}

DccCanvasWidget::~DccCanvasWidget()
{
}

void DccCanvasWidget::resizeEvent(QResizeEvent *)
{
	int h = m_pMenuBar->sizeHint().height();
	m_pMenuBar->setGeometry(0, 0, width(), h);
	int h2 = m_pStatusLabel->sizeHint().height();
	m_pStatusLabel->setGeometry(0, height() - h2, width(), h2);
	m_pSplitter->setGeometry(0, h, width(), height() - (h + h2));
}

#endif
