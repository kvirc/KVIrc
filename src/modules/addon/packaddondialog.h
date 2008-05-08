#ifndef _PACKADDONDIALOG_H_
#define _PACKADDONDIALOG_H_
//=============================================================================
//
//   File : packaddondialog.h
//   Created on Sat 03 May 2008 01:40:44 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
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

#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "kvi_tal_wizard.h"

class QString;
class QLineEdit;
class KviTalTextEdit;
class KviFileSelector;

class KviPackAddonDialog : public KviTalWizard
{
	Q_OBJECT
public:
	KviPackAddonDialog(QWidget * pParent);
	virtual ~KviPackAddonDialog();
protected:
	QString           m_szPackagePath;
	KviFileSelector * m_pPathSelector;
	QLineEdit       * m_pPackagerNameEdit;
	QLineEdit       * m_pPackageNameEdit;
	QLineEdit       * m_pPackageVersionEdit;
	KviTalTextEdit  * m_pPackageDescriptionEdit;
protected:
	virtual void accept();
	bool packAddon();
};

#endif //!_PACKADDONDIALOG_H_
