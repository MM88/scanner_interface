#include "mainwindow.h"
#include "../build/ui_mainwindow.h"
#include "grabberprogressdialog.h"
#include <QtMsgHandler>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QString>
#include <QHBoxLayout>
#include <pcl/io/ply_io.h>


PCLViewer::PCLViewer (QWidget *parent) :
  QMainWindow (parent),
  ui (new Ui::PCLViewer)
{
  ui->setupUi (this);
  this->setWindowTitle ("PCL viewer");

  // Set up the QVTK window
  viewer.reset (new pcl::visualization::PCLVisualizer ("viewer", false));
  ui->qvtkWidget->SetRenderWindow (viewer->getRenderWindow ());
  viewer->setupInteractor (ui->qvtkWidget->GetInteractor (), ui->qvtkWidget->GetRenderWindow ());
  viewer->setBackgroundColor(158,158,158);
  pcl::PolygonMesh::Ptr emptyMesh (new pcl::PolygonMesh);
  viewer->addPolygonMesh(*emptyMesh,"cloud",0);
  ui->qvtkWidget->update ();
  ui->saveButton->setVisible(false);

}

PCLViewer::~PCLViewer ()
{
  delete ui;
}


void PCLViewer::on_saveButton_clicked()
{
     QString filename = QFileDialog::getSaveFileName(this, tr ("Salva mesh"), "/home/miky/Scrivania/", tr ("Point cloud data (*.ply)"));

     PCL_INFO("File chosen: %s\n", filename.toStdString ().c_str ());

     if (filename.isEmpty ())
       return;

    int return_status;

    if (filename.endsWith (".ply", Qt::CaseInsensitive))
       return_status = pcl::io::savePLYFileBinary (filename.toStdString (), mesh);
     else
     {
       filename.append(".ply");
       return_status = pcl::io::savePLYFileBinary (filename.toStdString (), mesh);
     }

     if (return_status != 0)
     {
       PCL_ERROR("Errore durante la scrittura %s\n", filename.toStdString ().c_str ());
       return;
     }
}

void PCLViewer::on_scanButton_clicked()
{
    rsgrabber = new GrabberProgressDialog;
    mesh = rsgrabber->perform()[0].getPolygonmesh();
    viewer->removePolygonMesh("cloud",0);
    viewer->addPolygonMesh (mesh, "cloud",0);
    viewer->resetCamera ();
    ui->qvtkWidget->update ();
    ui->saveButton->setVisible(true);
    ui->scanButton->update();
}


void PCLViewer::on_calibButton_clicked()
{
    calibDialog = new CalibrationDialog(this);
    calibDialog->show();
}
