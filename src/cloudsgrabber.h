#ifndef GRABBER
#define GRABBER
#include <fstream>
#include "rscloud.h"
#include <pcl/point_types.h>
#include <pcl/io/ply_io.h>
#include <pcl/console/parse.h>
#include <pcl/console/time.h>
#include <pcl/point_types.h>


using namespace std;
using namespace pcl;

#define pointT pcl::PointXYZRGB // con colori
//#define pointTnormal pcl::PointXYZRGBNormal // con colori
// #define pointT pcl::PointXYZ // senza colori
 #define pointTnormal pcl::Normal // senza colori

class CloudsGrabber {
public:

    void grabClouds();
    void processClouds();

public slots:

private:

public:
        static CloudsGrabber* Instance();
        virtual ~CloudsGrabber() { }

        std::vector<RScloud> getPointcloudvector() const;

        CloudsGrabber(){};

private:
        static CloudsGrabber* instance;
        struct float3rgb { float x,y,z; uint8_t r,g,b; };

        pcl::PointCloud<pointT>::Ptr txt2pointcloud(std::vector<float3rgb>);
        std::vector<RScloud> pointcloudvector;

};
#endif // GRABBER

