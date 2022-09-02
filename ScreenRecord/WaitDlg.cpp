#pragma execution_character_set("utf-8")
#include "WaitDlg.h"

WaitDlg::WaitDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint);
}

WaitDlg::~WaitDlg()
{
    
}

void WaitDlg::SetProgressBarValue(const int& value)
{
	ui.progressBar->setValue(value);
}
