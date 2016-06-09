#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H
// Point Cloud Library
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <vtkRenderWindow.h>
#include <QDialog>

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


private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_getCloudsButton_clicked();

private:

    Ui::CalibrationDialog *ui;
};

#endif // CALIBRATIONDIALOG_H
