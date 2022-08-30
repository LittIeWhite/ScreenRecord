#include "RecordingThread.h"

RecordingThread::RecordingThread(QObject *parent)
	: QObject(parent)
{
	// recording functions move to thread
	this->moveToThread(&m_thread);
	m_thread.start();

	// connect slot for timer
	connect(&m_timer2record, &QTimer::timeout, this, &RecordingThread::Recording);

	m_pFileOutputPath = NULL;
}

RecordingThread::~RecordingThread()
{
	// close thread
	m_thread.quit();
	m_thread.wait();
}

// start recording
void RecordingThread::StartRecording(const QString& fileDir)
{
	QString curTime = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
	QString fileName = fileDir + "/" + curTime + ".avi";

	// set file path
	m_pFileOutputPath = AVI_open_output_file(fileName.toLocal8Bit().data());
	if (m_pFileOutputPath == NULL)
	{
		return;
	}

	// set size, fps, file's format
	QRect screenRect = QApplication::desktop()->screenGeometry();
	double fps = 10;
	AVI_set_video(m_pFileOutputPath, screenRect.width(), screenRect.height(), fps, (char*)"MJPG");

	// start timer to recording
	m_timer2record.start(1000/fps);
}

// recording
void RecordingThread::Recording()
{
	// get one frame
	//QPixmap pixmap = QApplication::primaryScreen()->grabWindow(QApplication::desktop()->effectiveWinId());
	QPixmap pixmap = grabWindow((HWND)QApplication::desktop()->winId());

	// convert pixmap to bytearray
	QByteArray ba;
	QBuffer buf(&ba);
	buf.open(QIODevice::WriteOnly);
	pixmap.save(&buf, "jpg");

	// write one frame to avi file
	if (AVI_write_frame(m_pFileOutputPath, ba.data(), ba.length(), 1) < 0)
	{
		//qDebug() << "frame write to avi file error\n";
	}

	buf.close();
}

// stop recording
void RecordingThread::StopRecording()
{
	// stop recording
	m_timer2record.stop();

	// close avi output obj
	AVI_close(m_pFileOutputPath);
}

QPixmap RecordingThread::grabWindow(HWND winId, int x, int y, int w, int h)
{

	RECT r;
	GetClientRect(winId, &r);

	if (w < 0)
	{
		w = r.right - r.left;
	}
	if (h < 0)
	{
		h = r.bottom - r.top;
	}

	HDC display_dc = GetDC(winId);
	HDC bitmap_dc = CreateCompatibleDC(display_dc);
	HBITMAP bitmap = CreateCompatibleBitmap(display_dc, w, h);
	HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);

	BitBlt(bitmap_dc, 0, 0, w, h, display_dc, x, y, SRCCOPY | CAPTUREBLT);

	CURSORINFO ci;
	ci.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&ci);
	if ((ci.ptScreenPos.x > x) && (ci.ptScreenPos.y > y) && (ci.ptScreenPos.x < (x + w)) && (ci.ptScreenPos.y < (y + h)))
	{
		DrawIcon(bitmap_dc, ci.ptScreenPos.x - x, ci.ptScreenPos.y - y, ci.hCursor);
	}

	// clean up all but bitmap
	ReleaseDC(winId, display_dc);
	SelectObject(bitmap_dc, null_bitmap);
	DeleteDC(bitmap_dc);

	QPixmap pixmap = QtWin::fromHBITMAP(bitmap);

	DeleteObject(bitmap);

	return pixmap;
}
