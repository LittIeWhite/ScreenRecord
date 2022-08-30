#pragma once

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QBuffer>
#include <QtWin>
#include <Windows.h>

extern "C"
{
#include"avilib.h"
}

class RecordingThread : public QObject
{
	Q_OBJECT

public:
	RecordingThread(QObject* parent = 0);
	~RecordingThread();

	// start recording
	void StartRecording(const QString& fileDir, const double& fps);

	// recording
	void Recording();

	// stop recording
	void StopRecording();

	static QPixmap grabWindow(HWND winId, int x = 0, int y = 0, int w = -1, int h = -1);

private:
	QThread m_thread;
	avi_t* m_pFileOutputPath = NULL;    // avi output path obj
};
