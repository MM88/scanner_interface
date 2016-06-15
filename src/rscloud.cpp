//
// Created by miky on 23/05/16.
//

#include "rscloud.h"

void RScloud::smooth_cloud() {

    //MLS SMOOTHING
    MovingLeastSquares<pointT, pointT> mls;
    mls.setInputCloud (pointcloud);
    mls.setSearchRadius (0.004);
    mls.setPolynomialFit (true);
    mls.setPolynomialOrder (2);
    PointCloud<pointT>::Ptr cloud_smoothed (new PointCloud<pointT> ());
    mls.process (*cloud_smoothed);
    //remove nans
    for(int i=0;i<cloud_smoothed->points.size();i++){
        if (!(pcl::isFinite(cloud_smoothed->at(i)))){
            cloud_smoothed->at(i).x = 0.0;
            cloud_smoothed->at(i).y = 0.0;
            cloud_smoothed->at(i).z = 0.0;
        }
    }
    pointcloud = cloud_smoothed;
}

pcl::PointCloud<pointT>::Ptr RScloud::getPointcloud() const
{
    return pointcloud;
}

void RScloud::setPointcloud(const pcl::PointCloud<pointT>::Ptr &value)
{
    pointcloud = value;
}

pcl::PolygonMesh RScloud::getPolygonmesh() const
{
    return polygonmesh;
}

void RScloud::setPolygonmesh(const pcl::PolygonMesh &value)
{
    polygonmesh = value;
}

void RScloud::filter_cloud() {

    pcl::PassThrough<pointT> pass ;
    pass.setInputCloud(pointcloud) ;

    pass.setFilterFieldName("z" ) ;
    pass.setFilterLimits(0.12,0.35);
    pass.filter(*pointcloud);

    pass.setInputCloud(pointcloud) ;
    pass.setFilterFieldName("x" ) ;
    pass.setFilterLimits(-0.17, 0.12);
    pass.filter(*pointcloud);

    pass.setInputCloud(pointcloud) ;
    pass.setFilterFieldName("y" ) ;
    pass.setFilterLimits(-0.09, 0.2);
    pass.filter(*pointcloud);

    pcl::StatisticalOutlierRemoval<pointT> sor;
    sor.setInputCloud (pointcloud);
    sor.setMeanK (20);
    sor.setStddevMulThresh (0.2);
    sor.filter (*pointcloud);

}


void RScloud::delete_boundaries(int num_border_to_remove) {

    for (int i=0;i<num_border_to_remove;i++){

        pcl::PointCloud<pointTnormal>::Ptr normals (new pcl::PointCloud<pointTnormal>);
        pcl::NormalEstimation<pointT, pointTnormal> ne;
        ne.setInputCloud (pointcloud);
        pcl::search::KdTree<pointT>::Ptr tree (new pcl::search::KdTree<pointT> ());
        ne.setSearchMethod (tree);
        ne.setRadiusSearch (0.002);
        ne.compute (*normals);
        pcl::PointCloud<pcl::Boundary> boundaries;
        pcl::BoundaryEstimation<pointT, pointTnormal, pcl::Boundary> est;
        est.setInputCloud (pointcloud);
        est.setInputNormals (normals);
        est.setRadiusSearch (0.002);
        est.setSearchMethod (pcl::search::KdTree<pointT>::Ptr (new pcl::search::KdTree<pointT>));
        est.compute (boundaries);

        for(int i = 0; i < pointcloud->points.size(); i++)
        {
            if(boundaries[i].boundary_point == 1)
            {
                pointcloud->at(i).z = 0;
                pointcloud->at(i).x = 0;
                pointcloud->at(i).y = 0;
            }
        }
    }
}

pcl::PolygonMesh RScloud::triangulate_cloud() {

    pcl::PointCloud<pointT>::Ptr cloudfiltered(new pcl::PointCloud<pointT>);
    pcl::StatisticalOutlierRemoval<pointT> sor;
    sor.setInputCloud (pointcloud);
    sor.setMeanK (50);
    sor.setStddevMulThresh (2.5);
    sor.filter (*cloudfiltered);
    // Normal estimation*
    pcl::NormalEstimation<pointT, pointTnormal> n;
    pcl::PointCloud<pointTnormal>::Ptr normals (new pcl::PointCloud<pointTnormal>);
    pcl::search::KdTree<pointT>::Ptr tree (new pcl::search::KdTree<pointT>);
    tree->setInputCloud (cloudfiltered);
    n.setInputCloud (cloudfiltered);
    n.setSearchMethod (tree);
    n.setKSearch (50);
    n.compute (*normals);
    // Concatenate the XYZ and normal fields*
    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud_with_normals (new pcl::PointCloud<pcl::PointXYZRGBNormal>);
    pcl::concatenateFields (*cloudfiltered, *normals, *cloud_with_normals);
    pcl::search::KdTree<pcl::PointXYZRGBNormal>::Ptr tree2 (new pcl::search::KdTree<pcl::PointXYZRGBNormal>);
    tree2->setInputCloud (cloud_with_normals);
    // Initialize objects
    pcl::GreedyProjectionTriangulation<pcl::PointXYZRGBNormal> gp3;
    pcl::PolygonMesh triangles;
    // Set the maximum distance between connected points (maximum edge length)
    gp3.setSearchRadius (0.025);
    // Set typical values for the parameters
    gp3.setMu (2.5);
    gp3.setMaximumNearestNeighbors (100);
    gp3.setMaximumSurfaceAngle(M_PI/4);
    gp3.setMinimumAngle(M_PI/18); // 10 degrees
    gp3.setMaximumAngle(2*M_PI/3); // 120 degrees
    gp3.setNormalConsistency(false);
    gp3.setConsistentVertexOrdering(true);
    // Get result
    gp3.setInputCloud (cloud_with_normals);
    gp3.setSearchMethod (tree2);
    gp3.reconstruct (triangles);
    // Additional vertex information
    std::vector<int> parts = gp3.getPartIDs();
    std::vector<int> states = gp3.getPointStates();

    return triangles;

}

void RScloud::processCloud() {

    //point_cloud_maker();
    filter_cloud();
    delete_boundaries(2);
    smooth_cloud();
//    polygonmesh = triangulate_cloud();
//    pcl::io::savePLYFileBinary("/home/miky/Scrivania/ciccia.ply", polygonmesh);

}
