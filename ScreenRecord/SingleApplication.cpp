#include "SingleApplication.h"

SingleApplication::SingleApplication(const QString & appName, int &argc, char **argv) : QApplication(argc, argv)
{
	m_pLocalServer = nullptr;
	m_appIsRunning = false;
	m_appName = appName;

	QLocalSocket socket;
	// ���ӵ�Server
	socket.connectToServer(appName);
	if (socket.waitForConnected(500))
	{
		// ������
		m_appIsRunning = true;
	}
	else
	{
		// ���Ӳ��ϣ��ʹ���һ��Server
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
		// ����ʧ�ܣ������ǳ���������½��̲������Ƴ��ټ���
		if (m_pLocalServer->serverError() == QAbstractSocket::AddressInUseError)
		{
			QLocalServer::removeServer(m_appName);
			m_pLocalServer->listen(m_appName);
		}
	}
}