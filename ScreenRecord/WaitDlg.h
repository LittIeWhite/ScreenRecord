#pragma once

#include <QDialog>
#include "ui_WaitDlg.h"
#include <QIcon>

class WaitDlg : public QDialog
{
	Q_OBJECT

public:
	WaitDlg(QWidget *parent = Q_NULLPTR);
	~WaitDlg();

	// set progressBar value
	void SetProgressBarValue(const int& value);

private:
	Ui::WaitDlg ui;
};
