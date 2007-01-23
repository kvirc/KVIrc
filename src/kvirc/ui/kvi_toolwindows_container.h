#ifndef _KVI_TOOLWINDOWS_CONTAINER_H_
#define _KVI_TOOLWINDOWS_CONTAINER_H_

#include "kvi_tal_widgetstack.h"
#include "kvi_tal_vbox.h"

#include <qtoolbutton.h> 
#include <qobjectcleanuphandler.h> 
#include <qpushbutton.h> 

#include "kvi_heapobject.h"
#include "kvi_styled_controls.h"

class KviWindowToolWidget;
class KviWindowToolPageButton;

// FIXME: these classes are probably useless now... no ?

//    Pragma: KviWindowToolPageButton is actually used in kvi_window.h and others (need to fix the name and move
//            it to its own file.



class KVIRC_API KviWindowToolWidget : public QWidget
{
	Q_OBJECT
public:
	KviWindowToolWidget( QWidget * parent,KviWindowToolPageButton* button/*, const char * name = 0, WFlags f = 0 */);
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
