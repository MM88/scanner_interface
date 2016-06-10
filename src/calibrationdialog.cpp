#include "calibrationdialog.h"
#include "../build/ui_calibrationdialog.h"
#include <stdio.h>
#include <fstream>
#include "rscloud.h"
#include "cloudsgrabber.h"
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <pcl/point_types.h>

using namespace pcl;
using namespace cv;
using namespace std;

CalibrationDialog::CalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrationDialog)
{
    ui->setupUi(this);

    grabber = CloudsGrabber::Instance();

    doubleViewer.reset (new pcl::visualization::PCLVisualizer ("calibration viewer", false));
    ui->qvtkWidget->SetRenderWindow (doubleViewer->getRenderWindow ());
    doubleViewer->setupInteractor (ui->qvtkWidget->GetInteractor (), ui->qvtkWidget->GetRenderWindow ());
    doubleViewer->setBackgroundColor(158,158,158);
    pcl::PointCloud<pointT>::Ptr emptyCloud (new pcl::PointCloud<pointT>);
    doubleViewer->addPointCloud (emptyCloud, "cloud",0);
    ui->qvtkWidget->update ();
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

    pointcloudvector.clear();
    grabber->grabClouds();   
    pointcloudvector = grabber->getPointcloudvector();
    pointcloudvector[0].filter_cloud();
    doubleViewer->updatePointCloud (pointcloudvector[0].getPointcloud(), "cloud");
    doubleViewer->resetCamera ();
    ui->qvtkWidget->update ();
}
