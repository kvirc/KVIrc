#ifndef _KVI_TAL_WIZARD_H_
#define _KVI_TAL_WIZARD_H_
//=============================================================================
//
//   File : KviTalWizard.h
//   Creation date : Tue Feb 06 2007 14:35:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek (pragma at kvirc dot net)
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

class KviTalWizardPageData;
class KviTalWizardPrivate;
class QPushButton;
class QShowEvent;

///
/// \class KviTalWizard
///
/// \brief Provides a wizard-style dialog with steps
///
class KVILIB_API KviTalWizard : public QDialog
{
	Q_OBJECT
public:
	KviTalWizard(QWidget * pParent);
	~KviTalWizard();

protected:
	KviTalWizardPrivate * m_p;

public:
	///
	/// Adds a page to the wizard with the specified title.
	/// The pages are displayed in order they're added.
	/// Adding a page a second time is equivalent to calling
	/// setPageTitle() and enabling the page.
	///
	void addPage(QWidget * pWidget, const QString & szTitle);
	///
	/// Adds a page to the wizard with the specified title
	/// and in the specified position.
	/// Adding a page a second time is equivalent to calling
	/// setPageTitle() and enabling the page.
	///
	void insertPage(QWidget * pWidget, const QString & szTitle, int iIndex);
	///
	/// Enables or disables a page. A disabled page
	/// is skipped when the user presses "Next" in the
	/// previous page or "Back" in the page after.
	/// Disabling the current page has no effect.
	///
	/// Returns true on success or false if the pWidget
	/// does not identify a page that has been added to this wizard.
	///
	bool setPageEnabled(QWidget * pWidget, bool bEnabled);
	///
	/// Changes a page title.
	///
	/// Returns true on success or false if the pWidget
	/// does not identify a page that has been added to this wizard.
	///
	bool setPageTitle(QWidget * pWidget, const QString & szTitle);
	///
	/// Switches the wizard to the specified page.
	/// Please note that this class handles page switching
	/// automatically so you usually don't need to call this function.
	///
	/// Returns true on success or false if the pWidget
	/// does not identify a page that has been added to this wizard.
	///
	bool setCurrentPage(QWidget * pWidget);
	///
	/// Returns a pointer to the current page
	///
	QWidget * currentPage();
	///
	/// Enables or disables the help button for the specified page.
	/// By default the help button is always disabled.
	///
	void setHelpEnabled(QWidget * pWidget, bool bEnabled);
	///
	/// Enables or disables the cancel button for the specified page.
	/// By default the cancel button is always enabled.
	///
	void setCancelEnabled(QWidget * pWidget, bool bEnabled);
	///
	/// Enables or disables the finish button for the specified page.
	/// By default the finish button is always disabled.
	///
	void setFinishEnabled(QWidget * pWidget, bool bEnabled);
	///
	/// Enables or disables the next button for the specified page.
	/// By default the next button is always enabled.
	///
	void setNextEnabled(QWidget * pWidget, bool bEnabled);
	///
	/// Enables or disables the prev button for the specified page.
	/// By default the prev button is always enabled.
	///
	void setBackEnabled(QWidget * pWidget, bool bEnabled);
	///
	/// Returns a pointer to the cancel button displayed in the dialog.
	///
	QPushButton * cancelButton();
	///
	/// Returns a pointer to the help button displayed in the dialog.
	///
	QPushButton * helpButton();
	///
	/// Returns a pointer to the finish button displayed in the dialog.
	///
	QPushButton * finishButton();
	///
	/// Returns a pointer to the next button displayed in the dialog.
	///
	QPushButton * nextButton();
	///
	/// Returns a pointer to the back button displayed in the dialog.
	///
	QPushButton * backButton();
signals:
	///
	/// Emitted when the help button is clicked.
	///
	void helpClicked();
	void pageChanged(QString title);

protected:
	///
	/// Displays the first page if no other page is shown yet.
	///
	void showEvent(QShowEvent * e) override;

	///
	/// Handles redirects the close button to the "cancel" operation.
	///
	void closeEvent(QCloseEvent * e) override;

protected:
	void setCurrentPage(KviTalWizardPageData * pData);
protected slots:
	void backButtonClicked();
	void nextButtonClicked();
	void helpButtonClicked();
	void cancelButtonClicked();
	void finishButtonClicked();
};

#endif // _KVI_TAL_WIZARD_H_
