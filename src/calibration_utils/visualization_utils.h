#include <boost/make_shared.hpp>
#include <pcl/visualization/pcl_visualizer.h>

#include "point_catched.h"
#include "transformation_utils.h"
#include "color_ring.h"

using namespace std;

#define POINT_TYPE pcl::PointXYZRGBA // con colori
// #define POINT_TYPE pcl::PointXYZ // senza colori
#define TRANLSATION_Z_SECOND_CLOUD 3.0 // 3 metri lungo la Z (meglio della X o Y perché sempre positiva)

namespace VisualizationUtils {
    //unsigned int click_id = 1; // per il mouse event

    // non riesco a recuperare la view dalle varie callback, così me la salvo globale e ciao
    boost::shared_ptr<pcl::visualization::PCLVisualizer> global_viewer;
    int v1, v2; // conterranno l'id delle due visualizzazioni, necessari per riferirsi a loro successivamente

    pcl::PointCloud<POINT_TYPE>::Ptr clicked_points(new pcl::PointCloud<POINT_TYPE>);
    pcl::PointCloud<POINT_TYPE>::Ptr clicked_points2(new pcl::PointCloud<POINT_TYPE>);

    // per ricordarsi da che parte era l'ultimo punto preso
    bool isSecondCloud;
    // abilita la possibilità di eliminare l'ultimo punto
    bool canIRollBack = false;
    POINT_TYPE previous_points;

    void loop_view(boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer) {
        while (!viewer->wasStopped()) {
            viewer->spinOnce(100);
            boost::this_thread::sleep(boost::posix_time::microseconds(100000));
        }
    }

    // ------------------------- METODI INUTILI DA QUI ------------------------------------
    /*void mouseEvent(const pcl::visualization::MouseEvent &event, void* args) { // 2D mouse click - abbastanza inutile
        // recupero la view - non funziona, uso global_viewer!
        //boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer = *static_cast<boost::shared_ptr<pcl::visualization::PCLVisualizer> *>(args);
        // si guarda se l'evento è un rilascio del click sinistro
        if (event.getButton() == pcl::visualization::MouseEvent::LeftButton && event.getType() == pcl::visualization::MouseEvent::MouseButtonRelease) {
            cout << "Click alla posizione (" << event.getX() << ", " << event.getY() << ")" << endl;
            // mette la bandierina
            char str[512];
            sprintf(str, "click#%03d", click_id++);
            global_viewer->addText(str, event.getX(), event.getY(), str);
        }
    }

    // gestione evento di selezione di un punto della cloud singola
    void pointPickEvent(const pcl::visualization::PointPickingEvent& event, void* args) {
        POINT_TYPE current_point;
        if (event.getPointIndex() == -1) {
            // non so quando possa succedere, ma così era nell'esempio
            cout << "pointIndex = -1" << endl;
            return;
        }
        cout << " -> selezionato il punto numero " << event.getPointIndex();
        // ricavo le coordinate
        event.getPoint(current_point.x, current_point.y, current_point.z);
        cout << " - coordinate (" << current_point.x << ", " << current_point.y << ", " << current_point.z << ")" << endl;
        clicked_points->points.push_back(current_point);
        // colora il punto in rosso
        pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZRGBA> red(clicked_points, 255, 0, 0);
        // aggiorno i punti taggati come clicked_points
        global_viewer->removePointCloud("clicked_points");
        global_viewer->addPointCloud(clicked_points, red, "clicked_points");
        global_viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, "clicked_points");
    }

    // visualizzazione base con input mouse
    void vis_base(pcl::PointCloud<POINT_TYPE>::ConstPtr cloud, string name) {
        boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
        // in teoria dovrebbe evitare di fare shift-click, ma non funfa
        //viewer->getInteractorStyle()->setKeyboardModifier(pcl::visualization::INTERACTOR_KB_MOD_SHIFT);
        viewer->initCameraParameters();
        viewer->setBackgroundColor(0.3, 0.3, 0.3); // grigio
        // --------------------------
        // se le cloud non hanno colori invertire i commenti (commento, commento, decommento)
        pcl::visualization::PointCloudColorHandlerRGBField<POINT_TYPE> rgb(cloud); // handler per i colori
        viewer->addPointCloud<POINT_TYPE>(cloud, rgb, name); // aggiungi la cloud alla visualizzazione
        // viewer->addPointCloud<POINT_TYPE>(cloud, name);
        // --------------------------
        viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, name); // dimensione dei punti della cloud
        viewer->addCoordinateSystem(0.1); // mostra gli assi di riferimento, scalati di 1/10
        viewer->addText(name, 10, 10); // aggiunge il nome nell'angolo in basso a sinistra
        // viewer->registerMouseCallback(mouseEvent, (void*)&viewer);

        global_viewer = viewer;
        viewer->registerPointPickingCallback(pointPickEvent, (void*)&viewer);

        loop_view(viewer);
    }*/
    // --------------------------------- A QUI --------------------------------------------

    // aggiorno i punti selezionati dall'utente
    void updateClickedPoints() {
        string cloudName = (isSecondCloud ? "clicked_right" : "clicked_left");
        int n_view = (isSecondCloud ? v2 : v1);

        // aggiorno la cloud dei punti selezionati
        global_viewer->removePointCloud(cloudName);
        pcl::visualization::PointCloudColorHandlerRGBField<POINT_TYPE> rgb((isSecondCloud ? clicked_points2 : clicked_points));
        global_viewer->addPointCloud((isSecondCloud ? clicked_points2 : clicked_points), rgb, cloudName, n_view);
        global_viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, cloudName, n_view);

        // aggiorno l'indicazione sul prossimo colore da inserire
        global_viewer->removeShape((isSecondCloud ? "v2_text" : "v1_text"));
        string colorName;
        if (canIRollBack)
            colorName = (isSecondCloud ? color_right : color_left).getNextColorName();
        else
            colorName = (isSecondCloud ? color_right : color_left).getColorName();
        global_viewer->addText("Prossimo colore: " + colorName, 10, 10, (isSecondCloud ? "v2_text" : "v1_text"), n_view);
    }

    // premendo "z" da tastiera elimina l'ultimo punto preso
    void keyboardEventOccurred(const pcl::visualization::KeyboardEvent &event, void* args) {
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

    // gestione evento di selezione di un punto della cloud in visuale doppia
    void pointPickDoubleViewEvent(const pcl::visualization::PointPickingEvent& event, void* args) {
        POINT_TYPE current_point;
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
        if (current_point.z > TRANLSATION_Z_SECOND_CLOUD)
            isSecondCloud = true;

        cout << " - coordinate (" << current_point.x << ", " << current_point.y << ", " << current_point.z << ") ";
        cout << "cloud " << (isSecondCloud ? "destra." : "sinistra.") << endl;

        // coloro il punto
        current_point = (isSecondCloud ? color_right : color_left).colorize(current_point);
        // lo aggiungo alla nuovola dei selezionati
        (isSecondCloud ? clicked_points2 : clicked_points)->points.push_back(current_point);
        updateClickedPoints();
        // salvo i punti presi
        (isSecondCloud ? points_right : points_left).add(event.getPointIndex(), current_point);

        // passo al colore successivo
        (isSecondCloud ? color_right : color_left).nextColor();
    }

    // visualizzazione doppia
    void vis_doppia(pcl::PointCloud<POINT_TYPE>::ConstPtr cloud1, pcl::PointCloud<POINT_TYPE>::ConstPtr cloud2, string name1, string name2) {
        // resetto le variabili globali
        clicked_points->clear();
        clicked_points2->clear();
        points_left.clear_stack();
        points_right.clear_stack();
        color_left.restart();
        color_right.restart();

        // creo la finestra
        boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
        global_viewer = viewer;
        viewer->initCameraParameters();
        viewer->setSize(1200, 650);

        // assegno la prima cloud
        viewer->createViewPort(0.0, 0.0, 0.5, 1.0, v1);
        viewer->setCameraPosition(0.0, 0.0, 0.0, 0.0, 0.0, 0.15, 0.0, 1.0, 0.0, v1);
        viewer->setBackgroundColor(0.3, 0.3, 0.3, v1); // background grigio
        viewer->addText("Prossimo colore: " + color_left.getColorName(), 10, 10, "v1_text", v1);
        pcl::visualization::PointCloudColorHandlerRGBField<POINT_TYPE> rgb(cloud1);
        viewer->addPointCloud<POINT_TYPE>(cloud1, rgb, name1, v1);
        viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, name1);

        // assegno la seconda cloud
        viewer->createViewPort(0.5, 0.0, 1.0, 1.0, v2);
        viewer->setBackgroundColor(0.3, 0.3, 0.3, v2);
        viewer->addText("Prossimo colore: " + color_right.getColorName(), 10, 10, "v2_text", v2);
        pcl::visualization::PointCloudColorHandlerRGBField<POINT_TYPE> rgb2(cloud2);
        // la traslo per poter identificare i suoi punti
        Eigen::Affine3f transform = Eigen::Affine3f::Identity();
        transform.translation() << 0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD;
        //cout << "matrice applicata alla seconda cloud" << endl << transform.matrix() << endl;
        pcl::PointCloud<POINT_TYPE>::Ptr transformed_cloud2(new pcl::PointCloud<POINT_TYPE>);
        pcl::transformPointCloud(*cloud2, *transformed_cloud2, transform);
        viewer->addPointCloud<POINT_TYPE>(transformed_cloud2, rgb2, name2, v2);
        viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, name2);

        // gestione separata della telecamera
        viewer->createViewPortCamera(v2);
        viewer->setCameraPosition(0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD, 0.0, 0.0, TRANLSATION_Z_SECOND_CLOUD + 0.15, 0.0, 1.0, 0.0, v2);

        viewer->registerKeyboardCallback(keyboardEventOccurred, (void*)&viewer);
        viewer->registerPointPickingCallback(pointPickDoubleViewEvent, (void*)&viewer);

        loop_view(viewer);

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
            return;
        }

        /*if (Mx.cols() < 3) {
            cout << "Dai, sforzati di prendere almeno 3 punti..." << endl;
            return;
        }*/

        Eigen::Matrix4f T = TransformationUtils::trovaT(Mx, My);
        ofstream outFile("/home/miky/Scrivania/trasformazione.txt");
        outFile << T;
        cout << " -> matrice di trasformazione salvata nel file trasformazione.txt" << endl;

        pcl::PointCloud<POINT_TYPE>::Ptr result(new pcl::PointCloud<POINT_TYPE>);
        pcl::transformPointCloud(*cloud1, *result, T);
        pcl::io::savePLYFileBinary("/home/miky/Scrivania/Cloud12.ply", *result);
        cout << " -> salvata la cloud traslata con nome Cloud12.ply" << endl;
    };
};