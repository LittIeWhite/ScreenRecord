#pragma once

#include <QObject>
#include <QThread>
#include <QAudioRecorder>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QDateTime>

class AudioThread : public QObject
{
	Q_OBJECT

public:
	AudioThread(QObject *parent = 0);
	~AudioThread();

	// start recording
	void StartRecording(const QString& fileName, const double&);

	// stop recording
	void StopRecording();

private:
	QThread m_thread;
	QAudioRecorder m_audioRecorder;
	QString m_sAudioDir;                // save audio's dir
};
