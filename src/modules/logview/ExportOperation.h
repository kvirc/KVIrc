#ifndef _EXPORTOPERATION_H_
#define _EXPORTOPERATION_H_

#include "LogFile.h"

#include <QObject>

#include <vector>
#include <memory>

class ExportOperation : public QObject
{
	Q_OBJECT

	std::vector<std::shared_ptr<LogFile>> m_logs;
	const QString m_szDir;
	const LogFile::ExportType m_type;

public:
	ExportOperation(const std::vector<std::shared_ptr<LogFile>> & logs, LogFile::ExportType type, QString szDir, QObject * parent = nullptr);
	void start();
};

#endif
