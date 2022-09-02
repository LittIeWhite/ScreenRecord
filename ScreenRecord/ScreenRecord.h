#pragma once

#include <QtWidgets/QWidget>
#include "ui_ScreenRecord.h"
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>
#include <QJsonDocument>
#include <QAudioRecorder>
#include <QDesktopWidget>

#include <Windows.h>

#include "WaitDlg.h"

class ScreenRecord : public QWidget
{
    Q_OBJECT

public:
    ScreenRecord(QWidget *parent = Q_NULLPTR);
    ~ScreenRecord();

    // init settings
    void InitSettings();

    // show settings
    void ShowSettings();

    // open files dir
    void OpenFilesDir();

    // recording time
    void RecordingTime();

    // start recording
    void StartRecording();

    // stop recording
    void StopRecording();

    // record finish
    void RecordFinish();

    // merge screen and sound
    void MergeScreenAndSound(const QString& fileName);

    // merge screen and sound finish
    void MergeFinish();

    // to find window
    static BOOL FindWindowProc(HWND hwnd, LPARAM param);

signals:
    void sig_mergeFinish();
    void sig_mergeProgressValue(const int& value);

private:
    Ui::ScreenRecordClass ui;
    struct Settings
    {
        QString sWindow;                  // current window title
        int nFps = 30;                    // video file fps
        bool bIsRecordScreen = true;      // record screen or not
        bool bIsRecordMicrophone = true;  // record microphone or not
        QString sFilesDir;                // save files's dir
    }m_stSettings;

    QString m_sFileName;                  // current file's name
    QTimer m_timer2calculateTime;         // timer to calculate time
    bool m_bIsRecording = false;          // is recording or not

    struct RecordingTimeStruct
    {
        int nHour = 0;
        int nMinute = 0;
        int nSecond = 0;
    }m_stRecordingTime;                   // recording time obj

    QAudioRecorder m_audioRecorder;
    QProcess m_screenProcess;
    WaitDlg m_waitDlg;
};
