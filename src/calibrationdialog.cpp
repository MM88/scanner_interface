#include "calibrationdialog.h"
#include "../build/ui_calibrationdialog.h"
#include <stdio.h>
#include <fstream>
#include "rscloud.h"

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>

using namespace cv;
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

    QStringList list = arg1.split("-");

}


void CalibrationDialog::on_getCloudsButton_clicked()
{

}
