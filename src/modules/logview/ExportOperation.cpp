#include "ExportOperation.h"

#include <QProgressDialog>
#include <QFutureWatcher>
#include <QVector>
#include <QtConcurrent>

#include "KviRegExp.h"
#include "LogFile.h"
#include "LogViewWindow.h"
#include "KviFileUtils.h"

ExportOperation::ExportOperation(const std::vector<std::shared_ptr<LogFile>> & logs, LogFile::ExportType type, QString szDir, QObject * parent)
    : QObject(parent)
    , m_logs(logs)
    , m_type(type)
    , m_szDir(szDir)
{
}

void ExportOperation::start()
{
	QProgressDialog * pProgressDialog = new QProgressDialog("Exporting logs...", "Cancel", 0, m_logs.size());
	QFutureWatcher<void> * pFutureWatcher = new QFutureWatcher<void>();

	QObject::connect(pFutureWatcher, &QFutureWatcher<void>::finished, pProgressDialog, &QProgressDialog::deleteLater);
	QObject::connect(pFutureWatcher, &QFutureWatcher<void>::finished, pFutureWatcher, &QFutureWatcher<void>::deleteLater);
	QObject::connect(pFutureWatcher, &QFutureWatcher<void>::finished, this, &ExportOperation::deleteLater);

	QObject::connect(pProgressDialog, &QProgressDialog::canceled, pFutureWatcher, &QFutureWatcher<void>::cancel);
	QObject::connect(pFutureWatcher, &QFutureWatcher<void>::progressValueChanged, pProgressDialog, &QProgressDialog::setValue);

	// The directory string and the export type could be captured by value if
	// this function was inlined. However, because the QtConcurrent functions
	// aside from QtConcurrent::run only operate on references the list of
	// pointers might expire, hence the purpose of this class.
	pFutureWatcher->setFuture(QtConcurrent::map(m_logs, [this](const std::shared_ptr<LogFile> & pLog) {
		const QString szDate = pLog->date().toString("yyyy.MM.dd");
		QString filename = QString("%1_%2.%3_%4").arg(pLog->typeString(), pLog->name(), pLog->network(), szDate);
		filename.replace(KviRegExp("[\\\\/:*?\"<>|]"), "_");
		QString szLog = m_szDir + QString(KVI_PATH_SEPARATOR_CHAR) + filename;
		KviFileUtils::adjustFilePath(szLog);
		pLog->createLog(m_type, szLog);
	}));
	pProgressDialog->show();
}
