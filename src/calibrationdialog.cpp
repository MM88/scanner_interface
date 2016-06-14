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
#include "calibrationdialog.h"
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/point_cloud_color_handlers.h>

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

//ui->qvtkWidget->update ();
    ui->qvtkWidget->SetRenderWindow (doubleViewer->getRenderWindow ());
    doubleViewer->setupInteractor (ui->qvtkWidget->GetInteractor (), ui->qvtkWidget->GetRenderWindow ());
     doubleViewer->getInteractorStyle ()->setKeyboardModifier (pcl::visualization::INTERACTOR_KB_MOD_SHIFT);
//    doubleViewer->setBackgroundColor(0.3, 0.3, 0.3);
//    pcl::PointCloud<pointT>::Ptr emptyCloud (new pcl::PointCloud<pointT>);
//    doubleViewer->addPointCloud (emptyCloud, "cloud",0);


    doubleViewer->createViewPort(0.0, 0.0, 0.5, 1.0, v1);
    doubleViewer->setCameraPosition(0.0, 0.0, 0.0, 0.0, 0.0, 0.15, 0.0, 1.0, 0.0, v1);
    doubleViewer->setBackgroundColor(0.2, 0.2, 0.2, v1); // background grigio
    pcl::PointCloud<pointT>::Ptr emptyCloud1 (new pcl::PointCloud<pointT>);
    doubleViewer->addPointCloud (emptyCloud1, "1",v1);

    doubleViewer->createViewPort(0.5, 0.0, 1.0, 1.0, v2);
    doubleViewer->setCameraPosition(0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD, 0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD + 0.15, 0.0, 1.0, 0.0, v2);
    doubleViewer->setBackgroundColor(0.3, 0.3, 0.3, v2);
    pcl::PointCloud<pointT>::Ptr emptyCloud2 (new pcl::PointCloud<pointT>);
    doubleViewer->addPointCloud (emptyCloud2, "2",v2);
    doubleViewer->createViewPortCamera(v2);


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


void CalibrationDialog::updateClickedPoints() {
    string cloudName = (isSecondCloud ? "clicked_right" : "clicked_left");
    int n_view = (isSecondCloud ? v2 : v1);

    // aggiorno la cloud dei punti selezionati
    doubleViewer->removePointCloud(cloudName);
    pcl::visualization::PointCloudColorHandlerRGBField<pointT> rgb((isSecondCloud ? clicked_points2 : clicked_points));
    doubleViewer->addPointCloud((isSecondCloud ? clicked_points2 : clicked_points), rgb, cloudName, n_view);
    doubleViewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, cloudName, n_view);

    // aggiorno l'indicazione sul prossimo colore da inserire
    doubleViewer->removeShape((isSecondCloud ? "v2_text" : "v1_text"));
    string colorName;
    if (canIRollBack)
        colorName = (isSecondCloud ? color_right : color_left).getNextColorName();
    else
        colorName = (isSecondCloud ? color_right : color_left).getColorName();
    doubleViewer->addText("Prossimo colore: " + colorName, 10, 10, (isSecondCloud ? "v2_text" : "v1_text"), n_view);
}

void CalibrationDialog::keybordEventWrapper( const pcl::visualization::KeyboardEvent& event, void* )
        {
            if( event.getKeyCode() && event.keyDown() ){
                std::cout << "Key : " << event.getKeyCode() << std::endl;
//                dialog()->keyboardEventOccurred(event);
            }
        }

// premendo "z" da tastiera elimina l'ultimo punto preso
void CalibrationDialog::keyboardEventOccurred(const pcl::visualization::KeyboardEvent& event) {

     std::cout << "Key pressed: " << event.getKeyCode() << "\n";

    if (event.getKeySym() == "z" && event.keyDown() && canIRollBack) {

        cout << "--> Tasto Z premuto, elimino l'ultimo punto preso dalla cloud " << (isSecondCloud ? "destra." : "sinistra.") << endl;
        int id = (isSecondCloud ? points_right : points_left).remove_last();
        (isSecondCloud ? color_right : color_left).resetToLastColor();
        canIRollBack = false;

        // rimuovo anche dalla visualizzazione
        (isSecondCloud ? clicked_points2 : clicked_points)->points.pop_back();
        updateClickedPoints();
    }
}

//// gestione evento di selezione di un punto della cloud in visuale doppia
void CalibrationDialog::pointPickDoubleViewEvent(const pcl::visualization::PointPickingEvent& event, void* args) {
//    pointT current_point;
//    isSecondCloud = false;

    cout << " -> selezionato il punto numero " << event.getPointIndex();
//    event.getPoint(current_point.x, current_point.y, current_point.z);

//    // verifico la validità del punto
//    if ((current_point.z > TRANLSATION_Z_SECOND_CLOUD ? points_right : points_left).checkPointError(event.getPointIndex(), current_point)) {
//        cout << " -> punto non valido!" << endl;
//        return;
//    }
//    // se è un punto valido consento l'annullamento e procedo ad inserirlo
//    canIRollBack = true;

//    // identifico prima o seconda cloud
//    if (current_point.z > TRANLSATION_Z_SECOND_CLOUD)
//        isSecondCloud = true;

//    cout << " - coordinate (" << current_point.x << ", " << current_point.y << ", " << current_point.z << ") ";
//    cout << "cloud " << (isSecondCloud ? "destra." : "sinistra.") << endl;

//    // coloro il punto
//    current_point = (isSecondCloud ? color_right : color_left).colorize(current_point); pcl::PointCloud<pointT>::Ptr emptyCloud1 (new pcl::PointCloud<pointT>);
//    doubleViewer->addPointCloud (emptyCloud1, "1",v1);
//    // lo aggiungo alla nuovola dei selezionati
//    (isSecondCloud ? clicked_points2 : clicked_points)->points.push_back(current_point);
//    updateClickedPoints();
//    // salvo i punti presi
//    (isSecondCloud ? points_right : points_left).add(event.getPointIndex(), current_point);

//    // passo al colore successivo
//    (isSecondCloud ? color_right : color_left).nextColor();
}
// visualizzazione doppia
void CalibrationDialog::doubleVisualization( string name1, string name2) {

    std::stringstream firstCloud;
    firstCloud<<"./cloud_registrazione/cloud"<<name1<<".ply";
    std::stringstream secondCloud;
    secondCloud<<"./cloud_registrazione/cloud"<<name2<<".ply";
    cout<<firstCloud.str()<<endl;
    cout<<secondCloud.str()<<endl;

    pcl::PointCloud<pointT>::Ptr cloud1 (new pcl::PointCloud<pointT>);
    pcl::PointCloud<pointT>::Ptr cloud2 (new pcl::PointCloud<pointT>);
    pcl::io::loadPLYFile(firstCloud.str().c_str(),*cloud1);
    pcl::io::loadPLYFile(secondCloud.str().c_str(),*cloud2);

    pcl::PointCloud<pointT>::Ptr clicked_points_app(new pcl::PointCloud<pointT>);
    pcl::PointCloud<pointT>::Ptr clicked_points2_app(new pcl::PointCloud<pointT>);
    // resetto le variabili globali
    clicked_points_app->clear();
    clicked_points2_app->clear();

    points_left.clear_stack();
    points_right.clear_stack();

    color_left.restart();
    color_right.restart();

    clicked_points = clicked_points2_app;
    clicked_points2 = clicked_points2_app;

    // creo la finestra
//    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
//    doubleViewer = viewer;
//        viewer->initCameraParameters();
//        viewer->setSize(1200, 650);


    // assegno la prima cloud

    doubleViewer->addText("Prossimo colore: " + color_left.getColorName(), 10, 10, "v1_text", v1);
    pcl::visualization::PointCloudColorHandlerRGBField<pointT> rgb(cloud1);
    doubleViewer->addPointCloud<pointT>(cloud1, rgb, "1", v1);
    doubleViewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "1");
    doubleViewer->setCameraPosition(0.0, 0.0, 0.0, 0.0, 0.0, 0.15, 0.0, 1.0, 0.0, v1);

    // assegno la seconda cloud

    doubleViewer->addText("Prossimo colore: " + color_right.getColorName(), 10, 10, "v2_text", v2);
    pcl::visualization::PointCloudColorHandlerRGBField<pointT> rgb2(cloud2);
    // la traslo per poter identificare i suoi punti
    Eigen::Affine3f transform = Eigen::Affine3f::Identity();
    transform.translation() << 0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD;
    //cout << "matrice applicata alla seconda cloud" << endl << transform.matrix() << endl;
    pcl::PointCloud<pointT>::Ptr transformed_cloud2(new pcl::PointCloud<pointT>);
    pcl::transformPointCloud(*cloud2, *transformed_cloud2, transform);
    doubleViewer->addPointCloud<pointT>(transformed_cloud2, rgb2, "2", v2);
    doubleViewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "2");

    doubleViewer->setCameraPosition(0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD, 0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD + 0.15, 0.0, 1.0, 0.0, v2);
    // gestione separata della telecamera
//    doubleViewer->createViewPortCamera(v2);
    QWidget * dialog = this;
//    boost::function<void (const pcl::visualization::KeyboardEvent &)> f2( boost::bind( CalibrationDialog::keyboardEventOccurred, &doubleViewer ) );
//    boost::function<void( const pcl::visualization::KeyboardEvent & ) > callback = boost::bind( &CalibrationDialog::keyboardEventOccurred, this, _1 );
//    doubleViewer->registerKeyboardCallback( f2 );

    doubleViewer->registerKeyboardCallback(CalibrationDialog::keybordEventWrapper,(void*)&doubleViewer);
    doubleViewer->registerPointPickingCallback(CalibrationDialog::pointPickDoubleViewEvent, (void*)&doubleViewer);

    while (!doubleViewer->wasStopped()) {
        doubleViewer->spinOnce(100);
        boost::this_thread::sleep(boost::posix_time::microseconds(100000));
    }

    // chiusa la view, stampo i punti catturati
    cout << "Punti selezionati:" << endl << " -> cloud sinistra:" << endl;
    //cout << endl << points_left.makeMatrix() << endl;
    points_left.print_all();
    cout << " -> cloud destra:" << endl;
    //cout << endl << points_right.makeMatrix() << endl;
    points_right.print_all();

    Eigen::MatrixXf Mx = points_left.makeMatrix();
    Eigen::MatrixXf My = points_right.makeMatrix();

    if (Mx.cols() != My.cols()) {
        cout << "Errore... Hai preso un numero diverso di punti tra destra e sinistra?" << endl;
        return ;
    }

    /*if (Mx.cols() < 3) {
        cout << "Dai, sforzati di prendere almeno 3 punti..." << endl;
        return;
    }*/

    Eigen::Matrix4f T = TransformationUtils::trovaT(Mx, My);
    ofstream outFile("/home/miky/Scrivania/trasformazione.txt");
    outFile << T;
    cout << " -> matrice di trasformazione salvata nel file trasformazione.txt" << endl;

    pcl::PointCloud<pointT>::Ptr result(new pcl::PointCloud<pointT>);
    pcl::transformPointCloud(*cloud1, *result, T);
    pcl::io::savePLYFileBinary("/home/miky/Scrivania/Cloud12.ply", *result);
    cout << " -> salvata la cloud traslata con nome Cloud12.ply" << endl;

    return ;
}

boost::shared_ptr<pcl::visualization::PCLVisualizer> CalibrationDialog::getDoubleViewer() const
{
    return doubleViewer;
}

void CalibrationDialog::on_calibButton_clicked()
{
    doubleViewer->removePointCloud("1",v1);
    doubleViewer->removePointCloud("2",v2);
    doubleVisualization(sourceCloudName,targetCloudName);
}
