#ifndef _CLASS_PAINTER_H_
#define _CLASS_PAINTER_H_
//=============================================================================
//
//   File : class_painter.h
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include <QPrinter>

#include <qpainter.h>
#include <qpaintdevice.h>
#include "kvi_string.h"
#include <QPainterPath>
#include "object_macros.h"


class KviKvsObject_painter : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_painter)
protected:
	QPrinter * m_pPrinter;
	QPainter * m_pPainter;
	QMatrix  m_pMatrix;
	QLinearGradient *m_pGradient;
	QPainterPath *m_pPainterPath;
public:
	bool functionSetFont(KviKvsObjectFunctionCall *c);
	bool functionSetFontSize(KviKvsObjectFunctionCall *c);
	
	bool functionfontAscent(KviKvsObjectFunctionCall *c);
	bool functionfontDescent(KviKvsObjectFunctionCall *c);
	bool functionfontMetricsWidth(KviKvsObjectFunctionCall *c);
	bool functionfontMetricsHeight(KviKvsObjectFunctionCall *c);
	bool functionsetBrush(KviKvsObjectFunctionCall *c);
	bool functionsetPen(KviKvsObjectFunctionCall *c);
	bool functionsetBackGroundMode(KviKvsObjectFunctionCall *c);

	bool functiondrawRect(KviKvsObjectFunctionCall *c);
	bool functiondrawWinFocusRect(KviKvsObjectFunctionCall *c);
	bool functiondrawLine(KviKvsObjectFunctionCall *c);
	bool functiondrawRoundRect(KviKvsObjectFunctionCall *c);
	bool functiondrawPoint(KviKvsObjectFunctionCall *c);
	bool functiondrawArc(KviKvsObjectFunctionCall *c);
	bool functiondrawChord(KviKvsObjectFunctionCall *c);
	bool functiondrawPie(KviKvsObjectFunctionCall *c);
	bool functiondrawEllipse(KviKvsObjectFunctionCall *c);

	bool functiondrawText(KviKvsObjectFunctionCall *c);
	bool functiondrawPixmap(KviKvsObjectFunctionCall *c);
	

	bool functionbegin(KviKvsObjectFunctionCall *c);
	bool functionend(KviKvsObjectFunctionCall *c);
	bool functionsave(KviKvsObjectFunctionCall *c);
	bool functionrestore(KviKvsObjectFunctionCall *c);


	bool functionscaleMatrix(KviKvsObjectFunctionCall *c);
	bool functionshearMatrix(KviKvsObjectFunctionCall *c);
	bool functionrotateMatrix(KviKvsObjectFunctionCall *c);
	bool functionresetMatrix(KviKvsObjectFunctionCall *c);
	bool functiontranslateMatrix(KviKvsObjectFunctionCall *c);
	bool functionsetPenStyle(KviKvsObjectFunctionCall *c);
	bool functionsetBrushStyle(KviKvsObjectFunctionCall *c);

// QT4
	bool functionsetOpacity(KviKvsObjectFunctionCall *c);
	bool functionsetSmoothPixmapTransform(KviKvsObjectFunctionCall *c);
	bool functionsetTextAntialiasing(KviKvsObjectFunctionCall *c);
	bool functionsetAntialiasing(KviKvsObjectFunctionCall *c);
	bool functionsetGradientStart(KviKvsObjectFunctionCall *c);
	bool functionsetGradientStop(KviKvsObjectFunctionCall *c);
	bool functionsetGradientColor(KviKvsObjectFunctionCall *c);
	bool functionsetGradientAsBrush(KviKvsObjectFunctionCall *c);
	bool functionclearGradient(KviKvsObjectFunctionCall *c);

	bool functiondrawIcon(KviKvsObjectFunctionCall *c);
	bool functiondrawHtmlText(KviKvsObjectFunctionCall *c);
	bool functionhtmlTextSize(KviKvsObjectFunctionCall *c);
	bool functionfillRect(KviKvsObjectFunctionCall *c);
	bool functionsetPenWidth(KviKvsObjectFunctionCall *c);
	bool functioncolorNames(KviKvsObjectFunctionCall *c);
	bool functionpathMoveTo(KviKvsObjectFunctionCall *c);
	bool functionpathLineTo(KviKvsObjectFunctionCall *c);
	bool functiondrawPath(KviKvsObjectFunctionCall *c);
	bool functionsetPenJoinStyle(KviKvsObjectFunctionCall *c);
	bool functionsetPenCapStyle(KviKvsObjectFunctionCall *c);
	bool functionpathAddText(KviKvsObjectFunctionCall *c);
	bool functionresetPath(KviKvsObjectFunctionCall *c);

	bool functionBeginPdf(KviKvsObjectFunctionCall *c);
	
	bool functionwidth(KviKvsObjectFunctionCall *c);
	bool functionheight(KviKvsObjectFunctionCall *c);


protected:
	void attachDevice(KviKvsObject * o,QPaintDevice * p);
protected slots:
	void detachDevice();
private:
	KviKvsObject * m_pDeviceObject;
};
#endif	// !_CLASS_PAINTER_H_



