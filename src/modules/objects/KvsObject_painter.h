#ifndef _CLASS_PAINTER_H_
#define _CLASS_PAINTER_H_
//=============================================================================
//
//   File : KvsObject_painter.h
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "object_macros.h"
#include <QPrinter>

#include <QPainter>
#include <QPaintDevice>
#include "KviCString.h"
#include <QPainterPath>

class KvsObject_painter : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_painter)
protected:
	QPrinter * m_pPrinter;
	bool bDonotdeleteinternalqpainter;
	QPainter * m_pPainter;
	QLinearGradient * m_pGradient;
	QPainterPath * m_pPainterPath;

public:
	void setInternalPainter(QPainter * p);
	bool setFont(KviKvsObjectFunctionCall * c);
	bool setFontSize(KviKvsObjectFunctionCall * c);

	bool fontAscent(KviKvsObjectFunctionCall * c);
	bool fontDescent(KviKvsObjectFunctionCall * c);
	bool fontMetricsWidth(KviKvsObjectFunctionCall * c);
	bool fontMetricsHeight(KviKvsObjectFunctionCall * c);
	bool setBrush(KviKvsObjectFunctionCall * c);
	bool setPen(KviKvsObjectFunctionCall * c);
	bool pen(KviKvsObjectFunctionCall * c);
	bool setBackGroundMode(KviKvsObjectFunctionCall * c);

	bool drawRect(KviKvsObjectFunctionCall * c);
	bool drawWinFocusRect(KviKvsObjectFunctionCall * c);
	bool drawLine(KviKvsObjectFunctionCall * c);
	bool drawRoundRect(KviKvsObjectFunctionCall * c);
	bool drawPoint(KviKvsObjectFunctionCall * c);
	bool drawArc(KviKvsObjectFunctionCall * c);
	bool drawChord(KviKvsObjectFunctionCall * c);
	bool drawPie(KviKvsObjectFunctionCall * c);
	bool drawEllipse(KviKvsObjectFunctionCall * c);

	bool drawText(KviKvsObjectFunctionCall * c);
	bool drawPixmap(KviKvsObjectFunctionCall * c);

	bool begin(KviKvsObjectFunctionCall * c);
	bool end(KviKvsObjectFunctionCall * c);
	bool save(KviKvsObjectFunctionCall * c);
	bool restore(KviKvsObjectFunctionCall * c);

	bool scale(KviKvsObjectFunctionCall * c);
	bool shear(KviKvsObjectFunctionCall * c);
	bool rotate(KviKvsObjectFunctionCall * c);
	bool reset(KviKvsObjectFunctionCall * c);
	bool translate(KviKvsObjectFunctionCall * c);
	bool setPenStyle(KviKvsObjectFunctionCall * c);
	bool setBrushStyle(KviKvsObjectFunctionCall * c);
	bool setCompositionMode(KviKvsObjectFunctionCall * c);

	// QT4
	bool setOpacity(KviKvsObjectFunctionCall * c);
	bool setSmoothPixmapTransform(KviKvsObjectFunctionCall * c);
	bool setTextAntialiasing(KviKvsObjectFunctionCall * c);
	bool setAntialiasing(KviKvsObjectFunctionCall * c);
	bool setGradientStart(KviKvsObjectFunctionCall * c);
	bool setGradientStop(KviKvsObjectFunctionCall * c);
	bool setGradientColor(KviKvsObjectFunctionCall * c);
	bool setGradientAsBrush(KviKvsObjectFunctionCall * c);
	bool clearGradient(KviKvsObjectFunctionCall * c);

	bool drawIcon(KviKvsObjectFunctionCall * c);
	bool drawHtmlText(KviKvsObjectFunctionCall * c);
	bool htmlTextSize(KviKvsObjectFunctionCall * c);
	bool fillRect(KviKvsObjectFunctionCall * c);
	bool setPenWidth(KviKvsObjectFunctionCall * c);
	bool colorNames(KviKvsObjectFunctionCall * c);
	bool pathMoveTo(KviKvsObjectFunctionCall * c);
	bool pathLineTo(KviKvsObjectFunctionCall * c);
	bool drawPath(KviKvsObjectFunctionCall * c);
	bool setPenJoinStyle(KviKvsObjectFunctionCall * c);
	bool setPenCapStyle(KviKvsObjectFunctionCall * c);
	bool pathAddText(KviKvsObjectFunctionCall * c);
	bool resetPath(KviKvsObjectFunctionCall * c);

	bool beginPdf(KviKvsObjectFunctionCall * c);
	bool drawPixmapWithColorEffect(KviKvsObjectFunctionCall * c);
	bool drawPixmapMirrored(KviKvsObjectFunctionCall * c);
	bool painterDeviceWidth(KviKvsObjectFunctionCall * c);
	bool painterDeviceHeight(KviKvsObjectFunctionCall * c);

protected:
	void attachDevice(KviKvsObject * o, QPaintDevice * p);
protected slots:
	void detachDevice();

private:
	KviKvsObject * m_pDeviceObject;
};
#endif // !_CLASS_PAINTER_H_
