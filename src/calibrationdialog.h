#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H
// Point Cloud Library
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <vtkRenderWindow.h>
#include <QDialog>
#include "rscloud.h"
#include "cloudsgrabber.h"

#define pointT pcl::PointXYZRGB // con colori
//#define pointTnormal pcl::PointXYZRGBNormal // con colori
// #define pointT pcl::PointXYZ // senza colori
 #define pointTnormal pcl::Normal // senza colori

namespace Ui {
class CalibrationDialog;
}

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrationDialog(QWidget *parent = 0);
    ~CalibrationDialog();


protected:
    boost::shared_ptr<pcl::visualization::PCLVisualizer> doubleViewer;
    std::vector<RScloud> pointcloudvector;


private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_getCloudsButton_clicked();

    void double_visualzer(std::string sourceCloudName,std::string targetCloudName);
private:

    Ui::CalibrationDialog *ui;
    CloudsGrabber *grabber;
    std::string sourceCloudName;
    std::string targetCloudName;
};

#endif // CALIBRATIONDIALOG_H
