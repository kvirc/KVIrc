#include "ExportOperation.h"

#include <QProgressDialog>
#include <QFutureWatcher>
#include <QVector>
#include <QtConcurrent>

#include "LogFile.h"
#include "LogViewWindow.h"
#include "KviFileUtils.h"

ExportOperation::ExportOperation(std::vector<LogFile *> logs, LogFile::ExportType type, QString szDir, QObject * parent)
    : QObject(parent)
    , m_logs(std::move(logs))
    , m_type(type)
    , m_szDir(szDir)
{
}

void ExportOperation::start()
{
	QProgressDialog * pProgressDialog = new QProgressDialog("Exporting Logs...", "Cancel", 0, m_logs.size());
	QFutureWatcher<void> * pFutureWatcher = new QFutureWatcher<void>();

	QObject::connect(pFutureWatcher, &QFutureWatcher<void>::finished, pProgressDialog, &QProgressDialog::deleteLater);
	QObject::connect(pFutureWatcher, &QFutureWatcher<void>::finished, pFutureWatcher, &QFutureWatcher<void>::deleteLater);
	QObject::connect(pFutureWatcher, &QFutureWatcher<void>::finished, this, &ExportOperation::deleteLater);

	QObject::connect(pProgressDialog, &QProgressDialog::canceled, pFutureWatcher, &QFutureWatcher<void>::cancel);
	QObject::connect(pFutureWatcher, &QFutureWatcher<void>::progressValueChanged, pProgressDialog, &QProgressDialog::setValue);

	pFutureWatcher->setFuture(QtConcurrent::map(m_logs, [this](LogFile * pLog) {
		const QString szDate = pLog->date().toString("yyyy.MM.dd");
		const QString szLog = m_szDir + KVI_PATH_SEPARATOR_CHAR + QString("%1_%2.%3_%4").arg(pLog->typeString(), pLog->name(), pLog->network(), szDate);
		pLog->createLog(m_type, szLog);
	}));
	pProgressDialog->exec();
}

ExportOperation::~ExportOperation()
{
	for(unsigned int i = 0; i < m_logs.size(); ++i)
	{
		delete m_logs.at(i);
	}
}
