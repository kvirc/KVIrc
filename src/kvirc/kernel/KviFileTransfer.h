#ifndef _KVI_FILETRANSFER_H_
#define _KVI_FILETRANSFER_H_
//=============================================================================
//
//   File : KviFileTransfer.h
//   Creation date : Mon Apr 21 22:14:31 2003 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviTalTableWidget.h"

#include <QObject>
#include <vector>

class KviFileTransfer;
class KviWindow;
class QMenu;
class QPainter;

// This is for the external extension
#define KVI_FILE_TRANSFER_WINDOW_EXTENSION_NAME "File transfer extension"

class KVIRC_API KviFileTransferManager : public QObject
{
	friend class KviApplication;
	friend class KviFileTransfer;
	friend class FileTransferWindow; // FIXME: This is in a module!
	Q_OBJECT
public:
	KviFileTransferManager();
	~KviFileTransferManager();

protected:
	std::vector<KviFileTransfer *> m_pTransferList;
	KviWindow * m_pTransferWindow = nullptr;

protected:
	static void cleanup();
	void registerTransfer(KviFileTransfer * t);
	void unregisterTransfer(KviFileTransfer * t);
	void setTransferWindow(KviWindow * wnd) { m_pTransferWindow = wnd; }
public:
	// might be zero!
	KviWindow * transferWindow() const { return m_pTransferWindow; }
	static KviFileTransferManager * instance();
	std::vector<KviFileTransfer *> transferList() const { return m_pTransferList; }
	void invokeTransferWindow(bool bCreateMinimized = false, bool bNoRaise = false);
	void killAllTransfers();
	void killTerminatedTransfers();
signals:
	void transferRegistered(KviFileTransfer * t);
	void transferUnregistering(KviFileTransfer * t);
};

#define COLUMN_TRANSFERTYPE 0
#define COLUMN_FILEINFO 1
#define COLUMN_PROGRESS 2

class KVIRC_API KviFileTransfer : public QObject
{
	Q_OBJECT
public:
	KviFileTransfer();
	~KviFileTransfer();

protected:
	int m_iId;
	KviTalTableWidgetItemEx * m_pDisplayItem = nullptr;

public:
	// This is called by KviFileTransferItem at any time
	void setDisplayItem(KviTalTableWidgetItemEx * i) { m_pDisplayItem = i; }
	int id() const { return m_iId; }
	// this is just a convenience function : it's equivalent to !active()
	bool terminated();
	// This may be used to invoke the transfer window
	void invokeTransferWindow(bool bCreateMinimized = false, bool bNoRaise = false);
	KviFileTransferManager * manager() const { return KviFileTransferManager::instance(); }
	// this returns the pointer to the transfer window : may be 0!
	KviWindow * transferWindow() const { return manager()->transferWindow(); }
	// this returns transferWindow() if not 0, otherwise the application's active window
	KviWindow * outputWindow();

	// this may be called by the transfers to update the display when the state changes
	void displayUpdate();

	// this must be implemented by the transfer
	virtual bool active() = 0;
	virtual void displayPaint(QPainter * p, int column, QRect rect) = 0;
	virtual QString tipText();
	virtual QString localFileName();
	virtual QString retryCommand();
	virtual int displayHeight(int iLineSpacing);
	virtual void fillContextPopup(QMenu * m) = 0;
	virtual void die();
};

#endif //! _KVI_FILETRANSFER_H_
