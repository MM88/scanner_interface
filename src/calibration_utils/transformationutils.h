#ifndef TRANSFORMATIONUTILS_H
#define TRANSFORMATIONUTILS_H

#include <pcl/point_types.h>

#define pointT pcl::PointXYZRGB // con colori
//#define pointTnormal pcl::PointXYZRGBNormal // con colori
// #define pointT pcl::PointXYZ // senza colori
 #define pointTnormal pcl::Normal // senza colori


namespace TransformationUtils {

    // calcola il punto medio tra tutti i selezionati
    Eigen::Vector3f getCentroid(Eigen::MatrixXf M);
    // si elimina la parte di traslazione spostando le origini nei centroidi
    Eigen::MatrixXf centraMatrice(Eigen::MatrixXf M, Eigen::Vector3f c);
    Eigen::Matrix4f trovaT(Eigen::MatrixXf Mx, Eigen::MatrixXf My) ;

}

#endif
