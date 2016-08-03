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
#include "calibration_utils/visualizationutils.h"
#include "calibration_utils/colorring.h"
#include "calibration_utils/pointcatched.h"
#include "calibration_utils/transformationutils.h"

#define pointT pcl::PointXYZRGB // con colori
//#define pointTnormal pcl::PointXYZRGBNormal // con colori
//#define pointT pcl::PointXYZ // senza colori
 #define pointTnormal pcl::Normal // senza colori
#define TRANLSATION_Z_SECOND_CLOUD 3.0 // 3 metri lungo la Z (meglio della X o Y perché sempre positiva)


namespace Ui {

class CalibrationDialog;
}

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:

    explicit CalibrationDialog(QWidget *parent = 0);
    ~CalibrationDialog();
    void updateClickedPoints();
    void keyboardEventOccurred(const pcl::visualization::KeyboardEvent& event, void*);
    void pointPickDoubleViewEvent(const pcl::visualization::PointPickingEvent& event, void*) ;

    void doubleVisualization(std::string name1, std::string name2);
    Eigen::Matrix4f icp( Eigen::Matrix4f T);

    boost::shared_ptr<pcl::visualization::PCLVisualizer> doubleViewer;

protected:
    std::vector<RScloud> pointcloudvector;


private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_getCloudsButton_clicked();

    void on_calibButton_clicked();

private:


    Ui::CalibrationDialog *ui;
    CloudsGrabber *grabber;
    std::string sourceCloudName;
    std::string targetCloudName;

    ColorRing color_left;
    ColorRing color_right;
    pointCatched points_left;
    pointCatched points_right;

    //attributi registrazione pcl
//    pcl::PointXYZ                     src_point_;
//    pcl::PointXYZ                     dst_point_;
    pcl::PointCloud<pcl::PointXYZ>    src_pc_;
    pcl::PointCloud<pcl::PointXYZ>    dst_pc_;

//    unsigned int click_id = 1; // per il mouse event

    int v1, v2;

    pcl::PointCloud<pointT>::Ptr clicked_points;
    pcl::PointCloud<pointT>::Ptr clicked_points2;

    // per ricordarsi da che parte era l'ultimo punto preso
    bool isSecondCloud;
    // abilita la possibilità di eliminare l'ultimo punto
    bool canIRollBack = false;
    pointT previous_points;

};

#endif // CALIBRATIONDIALOG_H
