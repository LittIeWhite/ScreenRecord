#include "SingleApplication.h"

SingleApplication::SingleApplication(const QString & appName, int &argc, char **argv) : QApplication(argc, argv)
{
	m_pLocalServer = nullptr;
	m_appIsRunning = false;
	m_appName = appName;

	QLocalSocket socket;
	// 连接到Server
	socket.connectToServer(appName);
	if (socket.waitForConnected(500))
	{
		// 连接上
		m_appIsRunning = true;
	}
	else
	{
		// 连接不上，就创建一个Server
		CreateLocalServer();
	}
}

SingleApplication::~SingleApplication()
{
	if (m_pLocalServer)
	{
		delete m_pLocalServer;
	}
}

bool SingleApplication::IsRunning()
{
	return m_appIsRunning;
}

void SingleApplication::CreateLocalServer()
{
	m_pLocalServer = new QLocalServer(this);
	if (!m_pLocalServer->listen(m_appName))
	{
		// 监听失败，可能是程序崩溃导致进程残留，移除再监听
		if (m_pLocalServer->serverError() == QAbstractSocket::AddressInUseError)
		{
			QLocalServer::removeServer(m_appName);
			m_pLocalServer->listen(m_appName);
		}
	}
}