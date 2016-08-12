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
#include <pcl/common/io.h>
#include <pcl/registration/icp.h>
#include <pcl/registration/icp_nl.h>
#include <QMessageBox>
#include <Eigen/Core>
#include <Eigen/SVD>

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
//    doubleViewer->getInteractorStyle ()->setKeyboardModifier (pcl::visualization::INTERACTOR_KB_MOD_SHIFT);

    doubleViewer->createViewPort(0.0, 0.0, 0.5, 1.0, v1);
    doubleViewer->setCameraPosition(0.0, 0.0, 0.0, 0.0, 0.0, 0.15, 0.0, 1.0, 0.0, v1);
    doubleViewer->setBackgroundColor(0.3, 0.3, 0.3, v1);
    pcl::PointCloud<pointT>::Ptr emptyCloud1 (new pcl::PointCloud<pointT>);
    doubleViewer->addPointCloud (emptyCloud1, "1",v1);

    doubleViewer->createViewPort(0.5, 0.0, 1.0, 1.0, v2);
    doubleViewer->setCameraPosition(0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD, 0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD + 0.15, 0.0, 1.0, 0.0, v2);
    doubleViewer->setBackgroundColor(0.4, 0.4, 0.4, v2);
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
    if (arg1.contains("coppia di camere")){
        doubleViewer->removePointCloud("clicked_right");
        doubleViewer->removePointCloud("clicked_left");
        doubleViewer->removePointCloud("1",v1);
        doubleViewer->removePointCloud("2",v2);
        points_left.clear_stack();
        points_right.clear_stack();
        color_left.restart();
        color_right.restart();
        ui->qvtkWidget->update ();
    }else{
        doubleViewer->removePointCloud("clicked_right");
        doubleViewer->removePointCloud("clicked_left");
        QStringList list = arg1.split("-");
        sourceCloudName.clear();
        targetCloudName = list[0].toStdString();
        sourceCloudName = list[1].toStdString();
        doubleViewer->removePointCloud("1",v1);
        doubleViewer->removePointCloud("2",v2);
        doubleVisualization(sourceCloudName,targetCloudName);
    }
}


void CalibrationDialog::on_getCloudsButton_clicked()
{
    pointcloudvector.clear();
    grabber->grabClouds();   
    pointcloudvector = grabber->getPointcloudvector();
    for (int i=0;i<pointcloudvector.size();i++){
        std::stringstream fileName;
        fileName<<"./../registrazione/cloud"<<i<<".ply";
        pointcloudvector[i].filter_cloud();
//        pointcloudvector[i].smooth_cloud();
//        pointcloudvector[i].delete_boundaries(2);
//        std::stringstream file;
//        file<<"/home/miky/Scrivania/cloud"<<i<<".ply";
//        pcl::io::savePLYFileBinary (file.str(), *pointcloudvector[i].getPointcloud());
        pcl::io::savePLYFileBinary (fileName.str(), *pointcloudvector[i].getPointcloud());
    }
    ui->comboBox->setCurrentIndex(0);
}

void CalibrationDialog::updateClickedPoints()
{
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

void CalibrationDialog::keyboardEventOccurred(const pcl::visualization::KeyboardEvent& event,void*) {

    if( event.getKeyCode() && event.keyDown() ){
        std::cout << "Key : " << event.getKeyCode() << std::endl;
    }
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

void CalibrationDialog::pointPickDoubleViewEvent(const pcl::visualization::PointPickingEvent& event, void* )
{
    pointT current_point;
    isSecondCloud = false;

    cout << " -> selezionato il punto numero " << event.getPointIndex();
    event.getPoint(current_point.x, current_point.y, current_point.z);

    // verifico la validità del punto
    if ((current_point.z > TRANLSATION_Z_SECOND_CLOUD ? points_right : points_left).checkPointError(event.getPointIndex(), current_point)) {
        cout << " -> punto non valido!" << endl;
        return;
    }
    // se è un punto valido consento l'annullamento e procedo ad inserirlo
    canIRollBack = true;
    // identifico prima o seconda cloud
    if (current_point.z > TRANLSATION_Z_SECOND_CLOUD){
        isSecondCloud = true;
        pcl::PointXYZ selected_point;

        selected_point.x = current_point.x;
        selected_point.y = current_point.y;
        selected_point.z = current_point.z-TRANLSATION_Z_SECOND_CLOUD;
         dst_pc_.points.push_back(selected_point);
    }else{
        pcl::PointXYZ selected_point;

        selected_point.x = current_point.x;
        selected_point.y = current_point.y;
        selected_point.z = current_point.z;
        src_pc_.points.push_back(selected_point);
    }


    cout << " - coordinate (" << current_point.x << ", " << current_point.y << ", " << current_point.z << ") ";
    cout << "cloud " << (isSecondCloud ? "destra." : "sinistra.") << endl;
    // coloro il punto
    current_point = (isSecondCloud ? color_right : color_left).colorize(current_point); pcl::PointCloud<pointT>::Ptr emptyCloud1 (new pcl::PointCloud<pointT>);
    doubleViewer->addPointCloud (emptyCloud1, "1",v1);
    // lo aggiungo alla nuovola dei selezionati
    (isSecondCloud ? clicked_points2 : clicked_points)->points.push_back(current_point);
    updateClickedPoints();
    // salvo i punti presi
    (isSecondCloud ? points_right : points_left).add(event.getPointIndex(), current_point);
    // passo al colore successivo
    (isSecondCloud ? color_right : color_left).nextColor();
}

void CalibrationDialog::doubleVisualization( string name1, string name2)
{
    std::stringstream firstCloud;
    firstCloud<<"./../registrazione/cloud"<<name1<<".ply";
    std::stringstream secondCloud;
    secondCloud<<"./../registrazione/cloud"<<name2<<".ply";
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

    src_pc_.points.clear();
    dst_pc_.points.clear();
//    src_pc_.height = 1; src_pc_.width = 0;
//    dst_pc_.height = 1; dst_pc_.width = 0;


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
    doubleViewer->registerKeyboardCallback(&CalibrationDialog::keyboardEventOccurred, *this);
    doubleViewer->registerPointPickingCallback(&CalibrationDialog::pointPickDoubleViewEvent, *this);

    while (!doubleViewer->wasStopped()) {
        doubleViewer->spinOnce(100);
        boost::this_thread::sleep(boost::posix_time::microseconds(100000));
    }
    return ;
}


void CalibrationDialog::on_calibButton_clicked()
{
    cout << "Punti selezionati:" << endl << " -> cloud sinistra:" << endl;
    points_left.print_all();
    cout << " -> cloud destra:" << endl;
    points_right.print_all();
    Eigen::MatrixXf Mx = points_left.makeMatrix();
    Eigen::MatrixXf My = points_right.makeMatrix();
    if (Mx.cols() != My.cols()) {
        int ret = QMessageBox::warning(this, tr("Calibrazione camere"),
                                       tr("Errore... Hai preso un numero diverso di punti tra destra e sinistra?"),
                                      QMessageBox::Ok);
        return ;
    }
    if (Mx.cols() < 3) {
        int ret = QMessageBox::warning(this, tr("Calibrazione camere"),
                                       tr("Selezionare almeno 3 coppie di punti"),
                                      QMessageBox::Ok);
        return;
    }

//    Eigen::Matrix4f T = TransformationUtils::trovaT(Mx, My);

    Eigen::Matrix4f T ;
    pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ> tfe;
    tfe.estimateRigidTransformation(src_pc_, dst_pc_, T);

    std::cout << "Transform : " << std::endl << T << std::endl;
    Eigen::Matrix4f T_icp = icp(T);

    std::stringstream outFileName;
    cout<<"il source è: "<<sourceCloudName<<endl;
    outFileName<<"./../registrazione/matrix_"<<sourceCloudName<<".txt";

    std::ofstream output(outFileName.str().c_str());
    for (int k=0; k<T_icp.rows(); k++)
    {
        for (int l=0; l<T_icp.cols(); l++)
        {
            output << T_icp(k,l) << "\t";
        }
        output << "\n";
    }
    output.close();

    doubleViewer->removePointCloud("clicked_right");
    doubleViewer->removePointCloud("clicked_left");
    doubleViewer->removePointCloud("1",v1);
    doubleViewer->removePointCloud("2",v2);
    ui->qvtkWidget->update ();
    ui->calibButton->update();
}

Eigen::Matrix4f CalibrationDialog::icp( Eigen::Matrix4f T){

    std::stringstream firstCloud;
    firstCloud<<"./../registrazione/cloud"<<sourceCloudName<<".ply";
    std::stringstream secondCloud;
    secondCloud<<"./../registrazione/cloud"<<targetCloudName<<".ply";

    pcl::PointCloud<pointT>::Ptr cloud1 (new pcl::PointCloud<pointT>);
    pcl::PointCloud<pointT>::Ptr cloud2 (new pcl::PointCloud<pointT>);
    pcl::PointCloud<pointT>::Ptr aligned (new pcl::PointCloud<pointT>);

    pcl::io::loadPLYFile(firstCloud.str().c_str(),*cloud1);
    pcl::io::loadPLYFile(secondCloud.str().c_str(),*cloud2);

    pcl::IterativeClosestPoint<pointT, pointT> icp;
    icp.setInputSource(cloud1);
    icp.setInputTarget(cloud2);
    icp.setMaxCorrespondenceDistance(0.002);
    // Set the maximum number of iterations (criterion 1)
    icp.setMaximumIterations(10000);
    // Set the transformation epsilon (criterion 2)
    icp.setTransformationEpsilon(1e-8);
    // Set the euclidean distance difference epsilon (criterion 3)
    icp.setEuclideanFitnessEpsilon(1);
    // do the align
    icp.align(*aligned,T);

    cout << endl << "Analisi conclusa: " << (icp.hasConverged() ? "terminata per convergenza." : "*boh*") << endl;
    // eulero: sum of squared distances from the source to the target
    cout << "Punteggio di Euclide: " << icp.getFitnessScore() << endl;
    cout << "Matrice di trasformazione identificata:" << endl << icp.getFinalTransformation() << endl << endl;
    Eigen::Matrix4f T_new = icp.getFinalTransformation();
    cout << "Cloud salvata." << endl;
    return T_new;
}
