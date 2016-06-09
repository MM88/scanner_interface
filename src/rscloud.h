#ifndef RSCLOUD
#define RSCLOUD

#include <pcl/point_types.h>
#include <pcl/io/ply_io.h>
#include <pcl/io/vtk_lib_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/features/normal_3d.h>
#include <pcl/surface/gp3.h>
#include <pcl/io/vtk_io.h>
#include <pcl/surface/mls.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/console/parse.h>
#include <pcl/surface/concave_hull.h>
#include <pcl/surface/vtk_smoothing/vtk_mesh_smoothing_laplacian.h>
#include <pcl/surface/vtk_smoothing/vtk_utils.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/surface/vtk_smoothing/vtk_mesh_smoothing_windowed_sinc.h>
#include <pcl/surface/vtk_smoothing/vtk_mesh_subdivision.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/filters/passthrough.h>
#include <pcl/features/boundary.h>
#include <pcl/console/time.h>
#include <pcl/registration/transforms.h>

#define pointT pcl::PointXYZRGB // con colori
//#define pointTnormal pcl::PointXYZRGBNormal // con colori
// #define pointT pcl::PointXYZ // senza colori
 #define pointTnormal pcl::Normal // senza colori

using namespace pcl;

class RScloud{

private:

    pcl::PointCloud<pointT>::Ptr pointcloud;
    pcl::PolygonMesh polygonmesh;
    void filter_cloud();
    void smooth_cloud();
    void delete_boundaries(int num_border_to_remove);
    pcl::PolygonMesh triangulate_cloud();

public:

    virtual ~RScloud() { }
    RScloud() {}
    void processCloud();
    pcl::PointCloud<pointT>::Ptr getPointcloud() const;
    void setPointcloud(const pcl::PointCloud<pointT>::Ptr &value);
    pcl::PolygonMesh getPolygonmesh() const;
    void setPolygonmesh(const pcl::PolygonMesh &value);
};

#endif // RSCLOUD

