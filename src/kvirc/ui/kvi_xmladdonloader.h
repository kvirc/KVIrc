#ifndef _KVI_XML_ADDON_LOADER_H_
#define _KVI_XML_ADDON_LOADER_H_

///////////////////////////////////////////////////////////////////////////////
//
//   File : kvi_xmladdonloader.h
//   Creation date : 25  Jul 2006 GMT by Alexey Uzhva
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

#include "kvi_heapobject.h"
#include <qdialog.h>
#include <qlistview.h> 
#include <qdom.h>

class KVIRC_API KviXmlAddonLoaderItem: public QCheckListItem {

public:
	KviXmlAddonLoaderItem ( QDomElement *pElement,QCheckListItem * parent );
	~KviXmlAddonLoaderItem();
	
	QDomElement * element() { return m_pElement; };
protected:
	QDomElement *m_pElement;
};


class KVIRC_API KviXmlAddonLoader: public QDialog {
	Q_OBJECT
public:
	KviXmlAddonLoader ( QString szAddonFilename,QWidget * parent = 0, const char * name = 0, bool modal = FALSE, WFlags f = 0 );
	~KviXmlAddonLoader();
protected:
	QString		 m_szFilename;
	QListView	*m_pListView;
	QDomDocument    *m_pDocument;
protected slots:
	virtual void accept();
	virtual void reject();
protected:
	void fillListView();
};

#endif //!_KVI_XML_ADDON_LOADER_H_
