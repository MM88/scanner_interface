#ifndef RSCLOUDGRABBER
#define RSCLOUDGRABBER

#include <iostream>
#include "rscloudgrabber.h"
#include <vector>
#include <fstream>
#include "rscloud.h"
#include <pcl/point_types.h>
#include <pcl/io/ply_io.h>
#include <pcl/console/parse.h>
#include <pcl/console/time.h>
#include <pcl/point_types.h>


#include <QObject>
#include <QProgressDialog>
#include <QTimer>

using namespace std;
using namespace pcl;

#define pointT pcl::PointXYZRGB // con colori
//#define pointTnormal pcl::PointXYZRGBNormal // con colori
// #define pointT pcl::PointXYZ // senza colori
 #define pointTnormal pcl::Normal // senza colori

class RScloudgrabber : public QObject {
     Q_OBJECT
public:
    explicit RScloudgrabber(QObject *parent = 0);
    signals:

public slots:
    void perform();
    void cancel();

private:
    QProgressDialog *pd;

public:
////        static RScloudgrabber* Instance();
////        virtual ~RScloudgrabber() { }
        void setOutPath(std::string outpath) {
            RScloudgrabber::outPath = outpath;
        }

        std::vector<RScloud> getPointcloudvector() const;

//protected:

////        RScloudgrabber(){};

private:
////        static RScloudgrabber* instance;
        struct float3rgb { float x,y,z; uint8_t r,g,b; };
        int steps;
        pcl::PointCloud<pointT>::Ptr txt2pointcloud(std::vector<float3rgb>);
        std::vector<RScloud> pointcloudvector;
        int num_border_to_remove = 3;
        bool filtering = true;
        bool remove_border = true;
        bool smoothing = true;
        std::string outPath;

};
#endif // RSCLOUDGRABBER

