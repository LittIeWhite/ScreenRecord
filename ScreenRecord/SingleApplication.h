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

	// ��ȡ�Ƿ���Ӧ��ʵ��������
	bool IsRunning();

	// ��������Server
	void CreateLocalServer();

private:
	bool m_appIsRunning;           // �Ƿ���ʵ��������
	QLocalServer * m_pLocalServer; // ����Server
	QString m_appName;             // Ӧ�ñ�ʶ
};