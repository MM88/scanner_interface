#include "calibrationdialog.h"
#include "../build/ui_calibrationdialog.h"
#include <stdio.h>
#include <fstream>

using namespace std;

CalibrationDialog::CalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrationDialog)
{
    ui->setupUi(this);

    doubleViewer.reset (new pcl::visualization::PCLVisualizer ("calibration viewer", false));
    ui->qvtkWidget->SetRenderWindow (doubleViewer->getRenderWindow ());
    doubleViewer->setupInteractor (ui->qvtkWidget->GetInteractor (), ui->qvtkWidget->GetRenderWindow ());
}

CalibrationDialog::~CalibrationDialog()
{
    delete ui;
}

void CalibrationDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    std::string utf8_text = arg1.toStdString();
    cout<<utf8_text<<endl;
//    boost::split(utf8_text,arg1, boost::is_any_of("\t"));
//    cout<<utf8_text[0]<<" "<<utf8_text[1]<<endl;

}
