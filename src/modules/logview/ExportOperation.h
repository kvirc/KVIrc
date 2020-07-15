#ifndef _EXPORTOPERATION_H_
#define _EXPORTOPERATION_H_

#include "LogFile.h"

#include <QObject>

#include <vector>

class ExportOperation : public QObject
{
	Q_OBJECT

	std::vector<LogFile *> m_logs;
	const QString m_szDir;
	const LogFile::ExportType m_iId;

public:
	ExportOperation(std::vector<LogFile *> logs, LogFile::ExportType iId, QString szDir, QObject * parent = nullptr);
	~ExportOperation();
	void start();
};

#endif
