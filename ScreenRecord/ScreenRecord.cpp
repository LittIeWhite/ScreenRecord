#pragma execution_character_set("utf-8")
#include "ScreenRecord.h"

ScreenRecord::ScreenRecord(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    m_sFilesDir = QCoreApplication::applicationDirPath() + "/Files/";
    QDir dir(m_sFilesDir);
    if (!dir.exists())
    {
        // create files dir
        dir.mkpath(m_sFilesDir);
    }

    // connect slot for timer
    connect(&m_timer2calculateTime, &QTimer::timeout, this, &ScreenRecord::RecordingTime);

    // connect slot for bottons
    connect(ui.pBtn_openFilesDir, &QPushButton::clicked, this, &ScreenRecord::OpenFilesDir);
    connect(ui.pBtn_record, &QPushButton::clicked, this, &ScreenRecord::StartRecording);

    // connect slot for recording thread
    m_pRecordingThread = new RecordingThread();
    connect(this, &ScreenRecord::sig_startRecording, m_pRecordingThread, &RecordingThread::StartRecording);
    connect(this, &ScreenRecord::sig_stopRecording, m_pRecordingThread, &RecordingThread::StopRecording);
    connect(&m_timer2record, &QTimer::timeout, m_pRecordingThread, &RecordingThread::Recording);
}

ScreenRecord::~ScreenRecord()
{
    delete m_pRecordingThread;
}

// open files dir
void ScreenRecord::OpenFilesDir()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_sFilesDir));
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
    if (m_bIsRecording)
    {
        // if is recording, to stop
        StopRecording();
        return;
    }

    // start timer to calculate time
    m_timer2calculateTime.start(1000);

    // start recording
    emit sig_startRecording(m_sFilesDir, m_dFps);
    m_timer2record.start(1000 / m_dFps);

    // change UI botton's text to "Stop"
    ui.pBtn_record->setText("Stop");

    // set recording status to true
    m_bIsRecording = true;
}

// stop recording
void ScreenRecord::StopRecording()
{
    // stop recording
    m_timer2record.stop();
    emit sig_stopRecording();

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

    // set recording status to false
    m_bIsRecording = false;
}
