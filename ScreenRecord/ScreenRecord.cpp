#pragma execution_character_set("utf-8")
#include "ScreenRecord.h"

ScreenRecord::ScreenRecord(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// inti UI
	this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
	QString strVersion = "ScreenRecord V1.4";
	this->setWindowTitle(strVersion);

	// init settings
	InitSettings();

	// connect slot for timer
	connect(&m_timer2calculateTime, &QTimer::timeout, this, &ScreenRecord::RecordingTime);

	// connect slot for bottons
	connect(ui.pBtn_record, &QPushButton::clicked, this, &ScreenRecord::StartRecording);
	connect(ui.pBtn_settings, &QPushButton::clicked, this, &ScreenRecord::ShowSettings);
	connect(ui.pBtn_openDir, &QPushButton::clicked, this, &ScreenRecord::OpenFilesDir);

	// connect slot for merge thread
	connect(this, &ScreenRecord::sig_mergeFinish, this, &ScreenRecord::MergeFinish);

	// connect slot for waitDlg
	connect(this, &ScreenRecord::sig_mergeProgressValue, &m_waitDlg, &WaitDlg::SetProgressBarValue);
}

ScreenRecord::~ScreenRecord()
{

}

// init settings
void ScreenRecord::InitSettings()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());

	QString iniFile = QCoreApplication::applicationDirPath() + "/Settings.ini";
	QSettings settings(iniFile, QSettings::Format::IniFormat);
	m_stSettings.nFps = settings.value("Settings/FPS").toInt();
	m_stSettings.bIsRecordScreen = settings.value("Settings/IsRecordScreen").toBool();
	m_stSettings.bIsRecordMicrophone = settings.value("Settings/IsRecordMicrophone").toBool();
	m_stSettings.bIsRecordSystemSound = settings.value("Settings/IsRecordSystemSound").toBool();

	// clear win list
	ui.comboBox_window->clear();

	// enum window
	ui.comboBox_window->addItem("desktop");
	EnumWindows(ScreenRecord::FindWindowProc, (LPARAM)this);
	m_stSettings.sWindow = ui.comboBox_window->currentText();

	if (m_stSettings.nFps < 1 || m_stSettings.nFps >60)
	{
		m_stSettings.nFps = 25;
	}
	ui.spinBox_fps->setValue(m_stSettings.nFps);

	ui.checkBox_recordScreen->setChecked(m_stSettings.bIsRecordScreen);
	ui.checkBox_recordMicrophone->setChecked(m_stSettings.bIsRecordMicrophone);
	ui.checkBox_recordSystemSound->setChecked(m_stSettings.bIsRecordSystemSound);

	if (m_stSettings.sFilesDir.isEmpty())
	{
		m_stSettings.sFilesDir = "./Files/";
	}
	QDir dir(m_stSettings.sFilesDir);
	if (!dir.exists())
	{
		// create files dir
		dir.mkpath(m_stSettings.sFilesDir);
	}
	ui.lineEdit_filesDir->setText(m_stSettings.sFilesDir);

	ui.widget_settings->hide();

	// regsvr32 dll
	QString dll_path = "audio_sniffer-x64.dll";
	if (QFile::exists(dll_path))
	{
		QProcess process;
		QString command = QString("regsvr32 %1").arg(dll_path);
		process.start(command);
		process.waitForFinished(-1);
		QString dll_path_new = "audio_sniffer-x64_used.dll";
		QFile dll_file(dll_path);
		dll_file.rename(dll_path, dll_path_new);
	}
}

// show settings
void ScreenRecord::ShowSettings()
{
	bool settingsShowing = !ui.widget_settings->isHidden();

	if (!settingsShowing)
	{
		// clear win list
		ui.comboBox_window->clear();

		// enum window
		ui.comboBox_window->addItem("desktop");
		EnumWindows(ScreenRecord::FindWindowProc, (LPARAM)this);
		m_stSettings.sWindow = ui.comboBox_window->currentText();

		ui.widget_settings->show();
		ui.pBtn_settings->setText("Save Set");

		// in set, not allow to record
		ui.pBtn_record->setEnabled(false);
	}
	else
	{
		// to save settings
		m_stSettings.sWindow = ui.comboBox_window->currentText();

		m_stSettings.nFps = ui.spinBox_fps->value();

		m_stSettings.bIsRecordScreen = ui.checkBox_recordScreen->isChecked();
		m_stSettings.bIsRecordMicrophone = ui.checkBox_recordMicrophone->isChecked();
		m_stSettings.bIsRecordSystemSound = ui.checkBox_recordSystemSound->isChecked();

		QString iniFile = QCoreApplication::applicationDirPath() + "/Settings.ini";
		QSettings settings(iniFile, QSettings::Format::IniFormat);
		settings.setValue("Settings/FPS", m_stSettings.nFps);
		settings.setValue("Settings/IsRecordScreen", m_stSettings.bIsRecordScreen);
		settings.setValue("Settings/IsRecordMicrophone", m_stSettings.bIsRecordMicrophone);
		settings.setValue("Settings/IsRecordSystemSound", m_stSettings.bIsRecordSystemSound);

		ui.widget_settings->hide();
		ui.pBtn_settings->setText("Settings");

		// set finish, allow to record
		ui.pBtn_record->setEnabled(true);
	}
}

// open files dir
void ScreenRecord::OpenFilesDir()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(m_stSettings.sFilesDir));
}

// recording time
void ScreenRecord::RecordingTime()
{
	m_stRecordingTime.nSecond++;
	if (m_stRecordingTime.nSecond > 59)
	{
		m_stRecordingTime.nMinute++;
		m_stRecordingTime.nSecond = 0;
	}
	if (m_stRecordingTime.nMinute > 59)
	{
		m_stRecordingTime.nHour++;
		m_stRecordingTime.nMinute = 0;
	}

	// update UI
	QString sHour = QString("%1").arg(m_stRecordingTime.nHour, 2, 10, QLatin1Char('0'));
	QString sMinute = QString("%1").arg(m_stRecordingTime.nMinute, 2, 10, QLatin1Char('0'));
	QString sSecond = QString("%1").arg(m_stRecordingTime.nSecond, 2, 10, QLatin1Char('0'));
	ui.pLabel_time->setText(QString("%1:%2:%3").arg(sHour).arg(sMinute).arg(sSecond));
}

// start recording
void ScreenRecord::StartRecording()
{
	if (!m_stSettings.bIsRecordScreen && !m_stSettings.bIsRecordMicrophone && !m_stSettings.bIsRecordSystemSound)
	{
		// not open record
		return;
	}

	if (m_bIsRecording)
	{
		// if is recording, to stop
		StopRecording();
		return;
	}

	// start timer to calculate time
	m_timer2calculateTime.start(1000);

	m_sFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");

	// record screen
	if (m_stSettings.bIsRecordScreen)
	{
		QString screenDir = m_stSettings.sFilesDir + "/Screen/";
		QDir dir1(screenDir);
		if (!dir1.exists())
		{
			// create files dir
			dir1.mkpath(screenDir);
		}
		QString screenFile = screenDir + m_sFileName + ".mp4";
		QString ffmpeg_path = "ffmpeg.exe";
		int offset_x = 0;
		int offset_y = 0;
		QString command;
		if (m_stSettings.sWindow == "desktop")
		{
			QRect screenRect = QApplication::desktop()->screenGeometry();
			command = QString("%1 -f gdigrab -i %2 -framerate %3 -offset_x %4 -offset_y %5 -video_size %6x%7  -pix_fmt yuv420p -vcodec libx264 -crf 18 -threads 5 -preset ultrafast %8").arg(ffmpeg_path).arg(m_stSettings.sWindow).arg(m_stSettings.nFps).arg(offset_x).arg(offset_y).arg(screenRect.width()).arg(screenRect.height()).arg(screenFile);
		}
		else
		{
			command = QString("%1 -f gdigrab -framerate %2 -i title=\"%3\" -pix_fmt yuv420p -vcodec libx264 -crf 18 -threads 5 -preset ultrafast %4").arg(ffmpeg_path).arg(m_stSettings.nFps).arg(m_stSettings.sWindow).arg(screenFile);
		}
		m_screenProcess.start(command);
	}

	// record system sound
	if (m_stSettings.bIsRecordSystemSound)
	{
		QString systemSoundDir = m_stSettings.sFilesDir + "/SystemSound/";
		QDir dir1(systemSoundDir);
		if (!dir1.exists())
		{
			// create files dir
			dir1.mkpath(systemSoundDir);
		}
		QString systemSound = systemSoundDir + m_sFileName + ".wav";
		QString ffmpeg_path = "ffmpeg.exe";
		QString command = QString("%1 -f dshow -i audio=\"virtual-audio-capturer\" -ab 192k -ac 2 -preset ultrafast %2").arg(ffmpeg_path).arg(systemSound);
		m_systemSoundProcess.start(command);
	}

	// record audio
	if (m_stSettings.bIsRecordMicrophone)
	{
		QString audioDir = QCoreApplication::applicationDirPath() + m_stSettings.sFilesDir + "/Microphone/";
		QDir dir(audioDir);
		if (!dir.exists())
		{
			// create files dir
			dir.mkpath(audioDir);
		}
		QString audioFile = audioDir + m_sFileName;
		QAudioEncoderSettings audioSettings;
		audioSettings.setCodec("audio/pcm");
		audioSettings.setQuality(QMultimedia::HighQuality);
		m_audioRecorder.setEncodingSettings(audioSettings);
		m_audioRecorder.setOutputLocation(QUrl::fromLocalFile(audioFile));
		m_audioRecorder.record();
	}

	// change UI botton's text to "Stop"
	ui.pBtn_record->setText("Stop");

	// set recording status to true
	m_bIsRecording = true;

	// recording not allow to change settings
	ui.pBtn_settings->setEnabled(false);
}

// stop recording
void ScreenRecord::StopRecording()
{
	// stop recording audio
	if (m_stSettings.bIsRecordMicrophone)
	{
		m_audioRecorder.stop();
	}

	// stop recording system sound
	if (m_systemSoundProcess.isOpen())
	{
		m_systemSoundProcess.write("q");
		m_systemSoundProcess.waitForFinished(-1);
	}

	// stop recording screen
	if (m_screenProcess.isOpen())
	{
		m_screenProcess.write("q");
		m_screenProcess.waitForFinished(-1);
	}

	// stop timer to calculate time
	m_timer2calculateTime.stop();

	// reset recording time
	m_stRecordingTime.nHour = 0;
	m_stRecordingTime.nMinute = 0;
	m_stRecordingTime.nSecond = 0;
	QString sHour = QString("%1").arg(m_stRecordingTime.nHour, 2, 10, QLatin1Char('0'));
	QString sMinute = QString("%1").arg(m_stRecordingTime.nMinute, 2, 10, QLatin1Char('0'));
	QString sSecond = QString("%1").arg(m_stRecordingTime.nSecond, 2, 10, QLatin1Char('0'));
	ui.pLabel_time->setText(QString("%1:%2:%3").arg(sHour).arg(sMinute).arg(sSecond));

	// change UI botton's text to "Start"
	ui.pBtn_record->setText("Start");
	ui.pBtn_record->setEnabled(false);

	// set recording status to false
	m_bIsRecording = false;

	RecordFinish();
}

// record finish
void ScreenRecord::RecordFinish()
{
	// update wait ProgressBar
	emit sig_mergeProgressValue(50);
	m_waitDlg.show();

	// merge screen and sound
	QtConcurrent::run(this, &ScreenRecord::MergeScreenAndSound, m_sFileName);
}

// merge screen and video
void ScreenRecord::MergeScreenAndSound(const QString& fileName)
{
	// update wait ProgressBar
	emit sig_mergeProgressValue(60);

	QString videoPath = m_stSettings.sFilesDir + "/Screen/" + fileName + ".mp4";
	QString microphonePath = m_stSettings.sFilesDir + "/Microphone/" + fileName + ".wav";
	QString systemSoundPath = m_stSettings.sFilesDir + "/SystemSound/" + fileName + ".wav";

	QString finalPath = m_stSettings.sFilesDir + fileName + ".mp4";
	QString finalPath_wav = m_stSettings.sFilesDir + fileName + ".wav";

	QString ffmpeg_path = "ffmpeg.exe";

	QProcess process;
	QString command;

	// update wait ProgressBar
	emit sig_mergeProgressValue(70);

	QFile videoFile(videoPath);
	QFile microphoneFile(microphonePath);
	QFile systemSoundFile(systemSoundPath);

	// merge vieo and audio
	if (videoFile.exists() && microphoneFile.exists() && systemSoundFile.exists())
	{
		// merge screen, microphone and system sound
		command = QString("%1 -i %2 -i %3 -i %4 -vcodec copy -filter_complex amix=inputs=2:duration=first:dropout_transition=2 -b:a 192k -ac 2 -threads 5 -preset ultrafast %5").arg(ffmpeg_path).arg(videoPath).arg(microphonePath).arg(systemSoundPath).arg(finalPath);
		process.start(command);
		process.waitForFinished(-1);
	}
	else if (videoFile.exists() && !microphoneFile.exists() && !systemSoundFile.exists())
	{
		// only video
		videoFile.rename(videoPath, finalPath);
	}
	else if (videoFile.exists() && microphoneFile.exists() && !systemSoundFile.exists())
	{
		// merge screen, microphone
		command = QString("%1 -i %2 -i %3 -vcodec copy -b:a 192k -ac 2 -threads 5 -preset ultrafast %4").arg(ffmpeg_path).arg(videoPath).arg(microphonePath).arg(finalPath);
		process.start(command);
		process.waitForFinished(-1);
	}
	else if (videoFile.exists() && !microphoneFile.exists() && systemSoundFile.exists())
	{
		// merge screen, system sound
		command = QString("%1 -i %2 -i %3 -vcodec copy -b:a 192k -ac 2 -threads 5 -preset ultrafast %4").arg(ffmpeg_path).arg(videoPath).arg(systemSoundPath).arg(finalPath);
		process.start(command);
		process.waitForFinished(-1);
	}
	else if (!videoFile.exists() && microphoneFile.exists() && systemSoundFile.exists())
	{
		// merge microphone and system sound
		command = QString("%1 -i %2 -i %3 -b:a 192k -ac 2 -threads 5 -preset ultrafast %4").arg(ffmpeg_path).arg(microphonePath).arg(systemSoundPath).arg(finalPath_wav);
		process.start(command);
		process.waitForFinished(-1);
	}
	else if (!videoFile.exists() && !microphoneFile.exists() && systemSoundFile.exists())
	{
		// only system sound
		systemSoundFile.rename(systemSoundPath, finalPath_wav);
	}
	else if (!videoFile.exists() && microphoneFile.exists() && !systemSoundFile.exists())
	{
		// only microphone
		microphoneFile.rename(microphonePath, finalPath_wav);
	}
	else if (!videoFile.exists() && !microphoneFile.exists() && !systemSoundFile.exists())
	{
		// no item to merge
	}

	// update wait ProgressBar
	emit sig_mergeProgressValue(90);

	// remove screen and sound
	videoFile.remove();
	microphoneFile.remove();
	systemSoundFile.remove();

	// update wait ProgressBar
	emit sig_mergeProgressValue(100);

	// emit merge finish signal
	emit sig_mergeFinish();
}

// merge screen and sound finish
void ScreenRecord::MergeFinish()
{
	m_waitDlg.close();
	emit sig_mergeProgressValue(0);

	// stop recording, allow to change settings
	ui.pBtn_settings->setEnabled(true);
	ui.pBtn_record->setEnabled(true);
}

// to find window
BOOL ScreenRecord::FindWindowProc(HWND hwnd, LPARAM param)
{
	LPWSTR lpString = (LPWSTR)malloc(sizeof(WCHAR) * MAX_PATH);
	if (IsWindow(hwnd) && IsWindowEnabled(hwnd) && IsWindowVisible(hwnd))
	{
		if (GetWindowTextW(hwnd, lpString, MAX_PATH) > 0)
		{
			QString winTitle = QString::fromStdWString(lpString);

			// this pointer
			ScreenRecord* pThis = (ScreenRecord*)param;

			// update ui
			pThis->ui.comboBox_window->addItem(winTitle);
		}
	}
	free(lpString);
	return TRUE;
}
