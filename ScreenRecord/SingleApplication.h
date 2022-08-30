#pragma once
#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>

class SingleApplication : public QApplication
{
	Q_OBJECT

public:
	SingleApplication(const QString & appName, int &argc, char **argv);
	~SingleApplication();

	// 获取是否有应用实例在运行
	bool IsRunning();

	// 创建本地Server
	void CreateLocalServer();

private:
	bool m_appIsRunning;           // 是否有实例在运行
	QLocalServer * m_pLocalServer; // 本地Server
	QString m_appName;             // 应用标识
};