#ifndef PCLVIEWER_H
#define PCLVIEWER_H

#include <iostream>
// Qt
#include <QMainWindow>
#include <QGroupBox>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressDialog>
#include <QProgressBar>
#include <QLayout>
// Point Cloud Library
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <vtkRenderWindow.h>
//local
#include "calibrationdialog.h"
#include "grabberprogressdialog.h"


using namespace pcl;

namespace Ui
{
  class PCLViewer;
}

class PCLViewer : public QMainWindow
{
  Q_OBJECT

public:
  explicit PCLViewer (QWidget *parent = 0);
  ~PCLViewer ();

protected:
  boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer;
  pcl::PolygonMesh mesh ;

private slots:
   void on_saveButton_clicked();

   void on_scanButton_clicked();

   void on_calibButton_clicked();

private:

    Ui::PCLViewer *ui;
    GrabberProgressDialog *rsgrabber;
    CalibrationDialog *calibDialog;

};

#endif // PCLVIEWER_H
