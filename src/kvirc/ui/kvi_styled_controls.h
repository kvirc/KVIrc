#ifndef _KVI_STYLED_CONTROLS_H_
#define _KVI_STYLED_CONTROLS_H_

///////////////////////////////////////////////////////////////////////////////
//
//   File : kvi_styled_controls.h
//   Creation date : 19  Jan 2006 GMT by Alexey Uzhva
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2006 Alexey Uzhva
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
///////////////////////////////////////////////////////////////////////////////

#include "kvi_settings.h"
#include "kvi_heapobject.h"

#include <QCheckBox>
#include <QEvent>
#include <QTimer>
#include <QToolButton>

#ifdef COMPILE_ON_MINGW
        #define KviStyledCheckBox QCheckBox
        #define KviStyledToolButton QToolButton
#else

class KviStyledControl;
class KviTalToolBar;

#define KVI_STYLE_NUM_STEPS  20
#define KVI_STYLE_COLOR_DIFF  60
#define KVI_STYLE_TIMER_STEP 18

// FIXME: THIS STUFF SHOULD BE REMOVED.
//        HARDCODING STYLES IS A BAD GUI PRACTICE.

class KVIRC_API KviStyledControlInternal : public QObject
{
	Q_OBJECT
public:
	KviStyledControlInternal( KviStyledControl* control );
	virtual ~KviStyledControlInternal();
public slots:
	virtual void paintTimerShot();
protected:
	bool eventFilter( QObject *obj, QEvent *ev );
protected:
	KviStyledControl * m_pControl;
};

class KVIRC_API KviStyledControl
{
	friend class KviStyledControlInternal;
public:
	KviStyledControl(QWidget*);
	virtual ~KviStyledControl();
protected:
	virtual void enterEvent ( QEvent * );
	virtual void leaveEvent ( QEvent * );
	
	int                        m_bMouseEnter;
	int                        m_iStepNumber;
	QTimer                   * m_pTimer;
	KviStyledControlInternal * m_pInternal;
	QWidget                  * m_pWidget;
};

class KVIRC_API KviStyledCheckBox : public QCheckBox, public KviStyledControl
{
	Q_OBJECT
public:
	KviStyledCheckBox ( QWidget * parent, const char * name = 0 );
	KviStyledCheckBox ( const QString & text, QWidget * parent, const char * name = 0 );
	~KviStyledCheckBox();
protected:
	virtual void paintEvent ( QPaintEvent * );
};

class KVIRC_API KviStyledToolButton : public QToolButton, public KviStyledControl
{
	Q_OBJECT
private:
	bool bShowSubmenuIndicator;
public:
	KviStyledToolButton ( QWidget * parent, const char * name = 0 );
	KviStyledToolButton ( const QIcon & iconSet, const QString & textLabel, const QString & grouptext, QObject * receiver, const char * slot, KviTalToolBar * parent, const QString &name=QString() );
	~KviStyledToolButton();
	
	void setShowSubmenuIndicator(bool bShow);
protected:
	virtual void paintEvent ( QPaintEvent * );
	virtual void resizeEvent ( QResizeEvent * );
};

#endif //! COMPILE_ON_MINGW

#endif
