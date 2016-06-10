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
#include <pcl/io/ply_io.h>

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
    sourceCloudName.clear();
    sourceCloudName = list[0].toStdString();
    targetCloudName = list[1].toStdString();
}


void CalibrationDialog::on_getCloudsButton_clicked()
{
    pointcloudvector.clear();
    grabber->grabClouds();   
    pointcloudvector = grabber->getPointcloudvector();
    for (int i=0;i<pointcloudvector.size();i++){
        std::stringstream fileName;
        fileName<<"./cloud_registrazione/cloud"<<i<<".ply";
        pointcloudvector[i].filter_cloud();
        pcl::io::savePLYFileBinary (fileName.str(), *pointcloudvector[i].getPointcloud());
    }
//    pointcloudvector[0].filter_cloud();
//    doubleViewer->updatePointCloud (pointcloudvector[0].getPointcloud(), "cloud");
//    doubleViewer->resetCamera ();
//    ui->qvtkWidget->update ();
//    ui->getCloudsButton->update();

}

void CalibrationDialog::double_visualzer(std::string sourceCloudName, std::string targetCloudName)
{

}
