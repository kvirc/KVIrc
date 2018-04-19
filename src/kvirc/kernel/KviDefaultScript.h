#ifndef _KVI_DEFAULTSCRIPT_H_
#define _KVI_DEFAULTSCRIPT_H_
//=============================================================================
//
//   File : KviDefaultScript.h
//   Creation date : Fri Oct 01 2010 19:14:48 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hell at hellvis69 dot netsons dot org)
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

/**
* \file KviDefaultScript.h
* \author Elvio Basello
* \brief Default script handling
*/

#include "kvi_settings.h"

#include <QDialog>
#include <QObject>

class QCheckBox;
class QGroupBox;
class KviConfigurationFile;
class KviDefaultScriptDialog;

/**
* \class KviDefaultScriptManager
* \brief The manager who handles the restoring of the default script
*/
class KVIRC_API KviDefaultScriptManager : public QObject
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the default script manager
	* \return KviDefaultScriptManager
	*/
	KviDefaultScriptManager();

	/**
	* \brief Destroys the default script manager
	*/
	~KviDefaultScriptManager();

private:
	static KviDefaultScriptManager * m_pSelf;
	static unsigned int m_uCount;
	bool m_bNoNeedToRestore = false;
	bool m_bConfigFileMissing = false;
	KviDefaultScriptDialog * m_pDialog = nullptr;
	QString m_szVersion;
	QString m_szDate;
	QString m_szAction;
	QString m_szAddon;
	QString m_szAlias;
	QString m_szClass;
	QString m_szEvent;
	QString m_szPopup;
	QString m_szRaw;
	QString m_szToolbar;

public:
	/**
	* \brief Initializes the class instance
	* \return void
	*/
	static void init();

	/**
	* \brief Destroys the class instance
	* \return void
	*/
	static void done();

	/**
	* \brief Returns the instance of the class
	* \return KviDefaultScriptManager *
	*/
	static inline KviDefaultScriptManager * instance() { return m_pSelf; }

	/**
	* \brief Returns the number of instances of the class
	* \return unsigned int
	*/
	unsigned int count() const { return m_uCount; }

	/**
	* \brief Checks if the local defscript is up to date
	* \return bool
	*/
	bool isDefscriptUpToDate();

	/**
	* \brief Restores the default script
	*
	* If this function is suggested by KVIrc, the default script will be wiped out and then
	* it will be reloaded. Otherwise, when the user wants to restore it, he can choose which
	* part of the default script have to be restored
	*
	* This is an external helper to the real function
	* \return void
	*/
	void restore();

	/**
	* \brief Loads the configuration of the default script
	*
	* This is an external helper to the real function
	* \param szConfigFile The file where to load
	* \return void
	*/
	void load(const QString & szConfigFile);

	/**
	* \brief Initializes an empty configuration
	*
	* \return void
	*/
	void loadEmptyConfig();

	/**
	* \brief Saves the configuration of the default script
	*
	* This is an external helper to the real function
	* \param szConfigFile The file to save
	* \return void
	*/
	void save(const QString & szConfigFile);

private:
	/**
	* \brief Restores the default script
	*
	* This is the real function
	* \return void
	* \todo This function still misses class clearing due to a missing function in class editor
	*/
	void restoreInternal();

	/**
	* \brief Loads the configuration of the default script
	*
	* This is the real function
	* \param pCfg The configuration file
	* \return void
	*/
	void loadInternal(KviConfigurationFile * pCfg);

	/**
	* \brief Saves the configuration of the default script
	*
	* This is the real function
	* \param pCfg The configuration file
	* \return void
	*/
	void saveInternal(KviConfigurationFile * pCfg);

	/**
	* \brief Compares versions between global and personal data
	* \param szConfig The global configuration file
	* \param pszError The buffer containing errors
	* \return bool
	*/
	bool compareVersions(QString & szConfig, QString * pszError);
};

/**
* \class KviDefaultScriptDialog
* \brief A dialog which allow the user to choose which elements to restore
*/
class KVIRC_API KviDefaultScriptDialog : public QDialog
{
	Q_OBJECT
	friend class KviDefaultScriptManager;

public:
	/**
	* \brief Constructs the default script dialog
	* \return KviDefaultScriptDialog
	*/
	KviDefaultScriptDialog();

	/**
	* \brief Destroys the default script dialog
	*/
	~KviDefaultScriptDialog();

protected:
	QGroupBox * m_pAdvanced;
	QCheckBox * m_pAll;
	QCheckBox * m_pData;
	QCheckBox * m_pAction;
	QCheckBox * m_pAddon;
	QCheckBox * m_pAlias;
	QCheckBox * m_pClass;
	QCheckBox * m_pEvent;
	QCheckBox * m_pPopup;
	QCheckBox * m_pRaw;
	QCheckBox * m_pToolbar;
	QPushButton * m_pAdvancedButton;
protected slots:
	/**
	* \brief Enables or disables all checkboxes
	*
	* Called when the user (de)selects the 'All' one
	* \return void
	*/
	void toggleAll(bool);

	/**
	* \brief Open/close the advanced widget
	*
	* Called when the user clicks on 'Advanced...'
	* \return void
	*/
	void advanced();

	/**
	* \brief Accepts the dialog
	*
	* Called when the user clicks on 'Ok'
	* \return void
	*/
	void accept() override;

	/**
	* \brief Rejects the dialog
	*
	* Called when the user clicks on 'Cancel' or closes the dialog using the window
	* decoration or pressing ESC
	* \return void
	*/
	void reject() override;
};

#endif // _KVI_DEFAULTSCRIPT_H_
