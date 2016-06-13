#include <pcl/common/time.h>
#include <pcl/common/angles.h>
#include <pcl/io/openni2_grabber.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/image_viewer.h>
#include <pcl/console/parse.h>
#include <pcl/console/time.h>
#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>

#include <pcl/filters/passthrough.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/surface/mls.h>

#include <boost/make_shared.hpp>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/point_representation.h>

#include <pcl/io/pcd_io.h>

#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/filter.h>

#include <pcl/features/normal_3d.h>

#include <pcl/registration/icp.h>
#include <pcl/registration/icp_nl.h>
#include <pcl/registration/transforms.h>

#include <pcl/visualization/pcl_visualizer.h>

#include "visualizationutils.cpp"

using namespace std;

#define POINT_TYPE pcl::PointXYZRGBA // con colori
// #define POINT_TYPE pcl::PointXYZ // senza colori

void allinea_cloud() {

    pcl::PointCloud<POINT_TYPE>::Ptr cloud1(new pcl::PointCloud<POINT_TYPE>);
    pcl::PointCloud<POINT_TYPE>::Ptr cloud2(new pcl::PointCloud<POINT_TYPE>);
    pcl::PointCloud<POINT_TYPE>::Ptr source(new pcl::PointCloud<POINT_TYPE>);
    pcl::PointCloud<POINT_TYPE>::Ptr target(new pcl::PointCloud<POINT_TYPE>);
    pcl::PointCloud<POINT_TYPE>::Ptr aligned(new pcl::PointCloud<POINT_TYPE>);
//    int idMesh;
    std::stringstream sourcePath, destPath;
    destPath << "/home/miky/Scrivania/cloud1.ply";

    cout << endl;
    cout << "Allineamento della cloud 1 sulla 2..." << endl;
    for (int i = 1; i <= 2; i++) {
        sourcePath.str("");
        cout << " -> Indice della mesh " << i << ": ";
//        cin >> idMesh;
        sourcePath << "/home/miky/Scrivania/cloud2.ply";
//        destPath << idMesh << (i == 1 ? "_" : "");
        pcl::io::loadPLYFile<POINT_TYPE>(sourcePath.str(), i == 1 ? *source : *target);
    }
//    destPath << ".ply";

    //parametri da ottimizzare quando si sanno le posizioni relative delle camere

    // use iterative closet point
    pcl::IterativeClosestPoint<POINT_TYPE, POINT_TYPE> icp;
    icp.setInputSource(source);
    icp.setInputTarget(target);
    // Set the max correspondence distance to 3cm (0.03)(correspondences with higher distances will be ignored)
    float max_distance;
    cout << endl << " -> Distanza massima di corrispondenza (cm): ";
    cin >> max_distance;
    icp.setMaxCorrespondenceDistance(max_distance/100);
    // Set the maximum number of iterations (criterion 1)
    icp.setMaximumIterations(100);
    // Set the transformation epsilon (criterion 2)
    icp.setTransformationEpsilon(1e-6);
    // Set the euclidean distance difference epsilon (criterion 3)
    icp.setEuclideanFitnessEpsilon(1);
    // do the align
    icp.align(*aligned);

    cout << endl << "Analisi conclusa: " << (icp.hasConverged() ? "terminata per convergenza." : "*boh*") << endl;
    // eulero: sum of squared distances from the source to the target
    cout << "Punteggio di Euclide: " << icp.getFitnessScore() << endl;
    cout << "Matrice di trasformazione identificata:" << endl << icp.getFinalTransformation() << endl << endl;

    cout << "Salvataggio cloud allineata:" << endl;
    cout << " -> numero punti: " << aligned->points.size() << endl;
    cout << " -> destinazione: " << destPath.str() << endl;
    std::string out_final = "/home/miky/Scrivania/cloud1_icp.ply";
    pcl::io::savePLYFileBinary(out_final, *aligned);
    cout << "Cloud salvata." << endl;
};

// riduce i punti della cloud ad uno ogni cubetto, di lato dimLeaf
pcl::PointCloud<POINT_TYPE>::Ptr downsample(pcl::PointCloud<POINT_TYPE>::Ptr &loadedCloud, float dimLeaf) {
    pcl::VoxelGrid<POINT_TYPE> sor;
    sor.setInputCloud(loadedCloud);
    sor.setLeafSize(dimLeaf, dimLeaf, dimLeaf);
    pcl::PointCloud<POINT_TYPE>::Ptr cloud_filtered(new pcl::PointCloud<POINT_TYPE>);
    sor.filter(*cloud_filtered);
    return cloud_filtered;
}

int main() {
    int c, meanK, idMesh, idMesh2;
    float stDev;
    std::stringstream filePath;
    pcl::PointCloud<POINT_TYPE>::Ptr loadedCloud(new pcl::PointCloud<POINT_TYPE>);
    pcl::PointCloud<POINT_TYPE>::Ptr loadedCloud2(new pcl::PointCloud<POINT_TYPE>);
    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer;

    do {
        filePath.str("");
        cout << endl << endl;
        cout << "-------------------- Menu --------------------" << endl;
        cout << "1. acquisisci e filtra automaticamente" << endl;
        cout << "2. acquisisci e filtra con valori di outlier personalizzati" << endl;
        cout << endl;
        cout << "3. visualizza due cloud e selezionane i punti d'iteresse" << endl;
        cout << endl;
        cout << "4. unisci due mesh tramite i punti selezionati" << endl;
        cout << endl;
        cout << "9. esci" << endl;
        cout << "<1-4> ";
        cin >> c;
        cout << endl;
        switch (c) {
//            case 1:
//                kinect.save_cloud(50, 1);
//                break;
//            case 2:
//                cout << " -> dimensione dell'intorno di ogni punto da considerare (default: 50): ";
//                cin >> meanK;
//                cout << " -> moltiplicatore della deviazione standard (default: 1): ";
//                cin >> stDev;
//                kinect.save_cloud(meanK, stDev);
//                break;
//                /*case 3: // vecchia visualizzazione singola
//                    cout << " -> indice del file da aprire: ";
//                    cin >> idMesh;
//                    filePath << "D:/Google Drive/Università/tesi/body_scanner_proj/captures/Cloud" << idMesh << ".ply";
//                    pcl::io::loadPLYFile<POINT_TYPE>(filePath.str(), *loadedCloud);
//                    VisualizationUtils::vis_base(loadedCloud, "Cloud" + idMesh);
//                    break;*/
            case 3:
                cout << " -> indice del primo file da aprire: ";
//                cin >> idMesh;
                filePath << "/home/miky/Scrivania/cloud1.ply";
                pcl::io::loadPLYFile<POINT_TYPE>(filePath.str(), *loadedCloud);
                //loadedCloud = downsample(loadedCloud, 0.005f); // riduce ad un punto ogni mezzo centimetro

                filePath.str("");
                cout << " -> indice del secondo file da aprire: ";
//                cin >> idMesh2;
                filePath << "/home/miky/Scrivania/cloud2.ply";
                pcl::io::loadPLYFile<POINT_TYPE>(filePath.str(), *loadedCloud2);
                //loadedCloud2 = downsample(loadedCloud2, 0.005f);

                VisualizationUtils::vis_doppia(loadedCloud, loadedCloud2, "Cloud 1", "Cloud 2" );
                break;
            case 4:
                allinea_cloud();
                break;
            case 9:
//                kinect.deactivate_kinect();
                break;
        }
    } while (c != 9);
    cout << "Programma terminato.";
    cin.get();
    return (0);
}
