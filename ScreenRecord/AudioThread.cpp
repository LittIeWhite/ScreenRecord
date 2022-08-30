#pragma execution_character_set("utf-8")
#include "AudioThread.h"

AudioThread::AudioThread(QObject *parent)
	: QObject(parent)
{
	// audio functions move to thread
	this->moveToThread(&m_thread);
	m_thread.start();

	m_sAudioDir = QCoreApplication::applicationDirPath() + "/Files/WAV/";
	QDir dir(m_sAudioDir);
	if (!dir.exists())
	{
		// create files dir
		dir.mkpath(m_sAudioDir);
	}

	// init audio recorder
	QAudioEncoderSettings audioSettings;
	audioSettings.setCodec("audio/amr");
	audioSettings.setQuality(QMultimedia::HighQuality);
	m_audioRecorder.setEncodingSettings(audioSettings);
}

AudioThread::~AudioThread()
{
	// close thread
	m_thread.quit();
	m_thread.wait();
}

// start recording
void AudioThread::StartRecording(const QString& fileName, const double&)
{
	QString filePath = m_sAudioDir + "/" + fileName;

	// set file path
	m_audioRecorder.setOutputLocation(QUrl::fromLocalFile(filePath));

	// start recording
	m_audioRecorder.record();
}

// stop recording
void AudioThread::StopRecording()
{
	// stop recording
	m_audioRecorder.stop();
}
