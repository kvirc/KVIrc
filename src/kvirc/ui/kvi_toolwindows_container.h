#ifndef _KVI_TOOLWINDOWS_CONTAINER_H_
#define _KVI_TOOLWINDOWS_CONTAINER_H_

#include <qwidgetstack.h> 
#include <qtoolbutton.h> 
#include <qobjectcleanuphandler.h> 
#include <qpushbutton.h> 
#include <qvbox.h> 

#include "kvi_heapobject.h"
#include "kvi_styled_controls.h"

class KviWindowToolWidget;
class KviWindowToolPageButton;

// FIXME: these classes are probably useless now... no ?

class KVIRC_API KviToolWindowsContainer : public QFrame
{
	Q_OBJECT
public:
	KviToolWindowsContainer( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
	~KviToolWindowsContainer();
	
	void registerWidget(KviWindowToolWidget* page);
	void unregisterWidget(KviWindowToolWidget* page);
	
	QWidget* buttonContainer() { return m_pButtonContainer; };
	
protected:
	QWidgetStack		*m_pStack;
	KviWindowToolWidget	*m_pCurrentPage;
	QVBox			*m_pButtonContainer;
	QObjectCleanupHandler    m_ObjectHandler;
protected:
	QWidgetStack* widgetContainer() { return m_pStack; };
};


class KVIRC_API KviWindowToolWidget : public QWidget
{
	Q_OBJECT
public:
	KviWindowToolWidget( QWidget * parent,KviWindowToolPageButton* button, const char * name = 0, WFlags f = 0 );
	~KviWindowToolWidget();
	
	void setAutoDelete(bool b) { m_bAutoDelete=b; };
	bool autoDelete() { return m_bAutoDelete; };
	
	virtual void registerSelf();
	virtual void unregisterSelf();
/*public slots:
	virtual void hide ();
	virtual void show ();*/
protected:
//	KviToolWindowsContainer		*m_pContainer;
	KviWindowToolPageButton		*m_pButton;
	bool 				 m_bAutoDelete;
	bool 				 m_bHidden;
//	QObjectCleanupHandler		 m_ObjectHandler;
	
};

class KVIRC_API KviWindowToolPageButton : public KviStyledToolButton
{
	Q_OBJECT
public:
	KviWindowToolPageButton ( int pixon,int pixoff, const QString & text, QWidget * parent,bool bOn=0,const char * name = 0 );
	~KviWindowToolPageButton();
/*protected:
	virtual void drawButton ( QPainter * painter);*/
};

#endif //_KVI_TOOLWINDOWS_CONTAINER_H_
