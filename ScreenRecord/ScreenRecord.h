#pragma once

#include <QtWidgets/QWidget>
#include "ui_ScreenRecord.h"
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>

#include "RecordingThread.h"
#include "AudioThread.h"

class ScreenRecord : public QWidget
{
    Q_OBJECT

public:
    ScreenRecord(QWidget *parent = Q_NULLPTR);
    ~ScreenRecord();

    // open files dir
    void OpenFilesDir();

    // recording time
    void RecordingTime();

    // start recording
    void StartRecording();

    // stop recording
    void StopRecording();

    // merge avi and wav
    void MergeAviWav(const QString& fileName);

signals:
    void sig_startRecording(const QString& fileName, const double& fps);
    void sig_stopRecording();

private:
    Ui::ScreenRecordClass ui;
    QString m_sFilesDir;             // save files's dir
    QString m_sFileName;             // current file's name
    QTimer m_timer2calculateTime;    // timer to calculate time
    QTimer m_timer2record;           // timer to recording
    bool m_bIsRecording = false;     // is recording or not
    struct RecordingTimeStruct
    {
        int nHour = 0;
        int nMinute = 0;
        int nSecond = 0;
    }m_stRecordingTime;              // recording time obj
    RecordingThread* m_pRecordingThread;
    AudioThread* m_pAudioThread;
    double m_dFps = 10;              // avi file fps
};
