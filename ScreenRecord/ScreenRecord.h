#pragma once

#include <QtWidgets/QWidget>
#include "ui_ScreenRecord.h"
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>

#include "RecordingThread.h"

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

signals:
    void sig_startRecording(const QString& fileDir);
    void sig_stopRecording();

private:
    Ui::ScreenRecordClass ui;
    QString m_sFilesDir;         // save files's dir
    QTimer m_timer2recording;    // timer to recording time
    bool m_bIsRecording = false; // is recording or not
    struct RecordingTimeStruct
    {
        int nHour = 0;
        int nMinute = 0;
        int nSecond = 0;
    }m_stRecordingTime;          // recording time obj
    RecordingThread* m_pRecordingThread;
};