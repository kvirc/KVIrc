#ifndef _KVI_TAL_TABDIALOG_H_
#define _KVI_TAL_TABDIALOG_H_
//=============================================================================
//
//   File : KviTalTabDialog.h
//   Creation date : Tue Feb 06 2007 14:35:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include <QDialog>

class QGridLayout;
class QTabWidget;

class KVILIB_API KviTalTabDialog : public QDialog
{
	Q_OBJECT
public:
	KviTalTabDialog(QWidget * pParent = nullptr, const char * name = nullptr, bool bModal = false);
	~KviTalTabDialog();

protected:
	void setOkButton(const QString &);
	void setCancelButton(const QString &);
	void addTab(QWidget *, const QString &);
protected slots:
	void okPressed();
	void cancelPressed();

private:
	QGridLayout * m_pLayout;
	QTabWidget * m_pTabWidget;
signals:
	void applyButtonPressed();
	void cancelButtonPressed();
};

#endif // _KVI_TAL_TABDIALOG_H_
