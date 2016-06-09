#include "rscloudgrabber.h"
#include <librealsense/rs.hpp>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <QPushButton>
#include <fstream>

using namespace rs;
using namespace cv;

RScloudgrabber::RScloudgrabber(QObject *parent) :
    QObject(parent)
{
    pd = new QProgressDialog();
    pd = new QProgressDialog("Elaborazione in corso", "Annulla", 0, 4);
    pd->setWindowModality(Qt::WindowModal);
    connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
    pd->show();
}

void RScloudgrabber::perform()
{
    steps = 1;
    pd->setValue(steps);
    //... perform one percent of the operation

    // Turn on logging. We can separately enable logging to console or to file, and use different severity filters for each.
    rs::log_to_console(rs::log_severity::warn);
    //rs::log_to_file(rs::log_severity::debug, "librealsense.log");

    // Create a context object. This object owns the handles to all connected realsense devices.
    rs::context ctx;
    printf("There are %d connected RealSense devices.\n", ctx.get_device_count());
    if(ctx.get_device_count() == 0) return;

    int dev_number = ctx.get_device_count();

    // Enumerate all devices
    std::vector<rs::device *> devices;
    for(int i=0; i<ctx.get_device_count(); ++i)
    {
        devices.push_back(ctx.get_device(i));
    }

    // Configure and start our devices
    for(auto dev : devices)
    {
        std::cout << "Starting " << dev->get_name() << "... ";
        dev->enable_stream(rs::stream::depth, rs::preset::best_quality);
        dev->enable_stream(rs::stream::color, rs::preset::best_quality);
        dev->start();
        dev->set_option((rs::option)12, (double)0); //laser power
        dev->set_option((rs::option)13, (double)1);  //accuracy
        dev->set_option((rs::option)15, (double)5); //filter option
        std::cout << "done." << std::endl;
    }

    // This tutorial will access only a single device, but it is trivial to extend to multiple devices

    const clock_t begin_time = clock(); //to check grabbing time
    std::vector<std::vector<float3rgb>> txtcloudvector;

    for(auto dev : devices)
    {
        dev->set_option((rs::option)12, (double)16);

        // Capture frames to give autoexposure
        for (int i = 0; i < 15; ++i) dev->wait_for_frames();

        dev->wait_for_frames();
        // Retrieve our images
        const uint16_t * depth_image = (const uint16_t *)dev->get_frame_data(rs::stream::depth);
        const uint8_t * color_image = (const uint8_t *)dev->get_frame_data(rs::stream::color_aligned_to_depth); //

        cv::Mat image = cv::Mat::zeros(480,640,CV_8UC3);
        int count=0;
        for (int i = 0; i < 480; i++)
           for (int j = 0; j < 640; j++) {
               image.at<Vec3b>(i,j)[0] = color_image[count];
               count++;
               image.at<Vec3b>(i,j)[1] = color_image[count];
               count++;
               image.at<Vec3b>(i,j)[2] = color_image[count];
               count++;
           }

        // Retrieve camera parameters for mapping between depth and color
        rs::intrinsics depth_intrin = dev->get_stream_intrinsics(rs::stream::depth);
        rs::extrinsics depth_to_color = dev->get_extrinsics(rs::stream::depth, rs::stream::color);
        rs::intrinsics color_intrin = dev->get_stream_intrinsics(rs::stream::color);
        float scale = dev->get_depth_scale();

        std::vector<float3rgb> point_cloud;
        for(int dy=0; dy<depth_intrin.height; dy++)
        {
            for(int dx=0; dx<depth_intrin.width; dx++)
            {
                // Retrieve the 16-bit depth value and map it into a depth in meters
                uint16_t depth_value = depth_image[dy * depth_intrin.width + dx];
                float depth_in_meters = depth_value * scale; //0.1*3.12352;// scale*10000/3.12352;

                // Skip over pixels with a depth value of zero, which is used to indicate no data
                if(depth_value == 0) continue;

                // Map from pixel coordinates in the depth image to pixel coordinates in the color image
                rs::float2 depth_pixel = {(float)dx, (float)dy};
                rs::float3 depth_point = depth_intrin.deproject(depth_pixel, depth_in_meters);
                rs::float3 color_point = depth_to_color.transform(depth_point);
                float3rgb point;

                point.x = color_point.x; point.y = color_point.y; point.z = color_point.z;

                point.r = image.at<Vec3b>(dy,dx)[0];
                point.g = image.at<Vec3b>(dy,dx)[1];
                point.b = image.at<Vec3b>(dy,dx)[2];

                point_cloud.push_back(point);

            }
        }
        txtcloudvector.push_back(point_cloud);
        dev->set_option((rs::option)12, (double)0);
    }
    std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC<<endl;

    boost::this_thread::sleep (boost::posix_time::seconds (1));
    steps++;
    pd->setValue(steps);

    std::cout << "done scanning." << std::endl;
    std::cout << "saving clouds.." << std::endl;
    std::cout << "num of couds " << txtcloudvector.size() << std::endl;

    for (int i=0;i<txtcloudvector.size();i++){
            PointCloud<pointT>::Ptr cloud = txt2pointcloud(txtcloudvector[i]);
            
//            std::ostringstream matrixPath;
//            std::string data;
//            Eigen::Matrix4f transformMatrix = Eigen::Matrix4f::Identity();
//            matrixPath << "./matrix_"<<i<<".txt";
//            std::ifstream textMatrix(matrixPath.str());
//            if (textMatrix.is_open()){
//            getline(textMatrix, data);
//            for (int j=0;j<3;j++) {
//                    std::vector<std::string> x;
//                    boost::split(x,data, boost::is_any_of("\t "));
//                    transformMatrix (j,0) = atof(x[0]);
//                    transformMatrix (j,1) = atof(x[1]);
//                    transformMatrix (j,2) = atof(x[2]);
//                    transformMatrix (j,3) = atof(x[3]);
//                    getline(in, data);
//                }
//            textMatrix.close();
//            }
//            pcl::transformPointCloud (*cloud, cloud, transformMatrix);
            RScloud rscloud;
            rscloud.setPointcloud(cloud);
            pointcloudvector.push_back(rscloud);
        }

  std::cout << "cloud points " << pointcloudvector[0].getPointcloud()->points.size() << std::endl;

    std::cout << "done saving." << std::endl;
    boost::this_thread::sleep (boost::posix_time::seconds (1));
    steps++;
    pd->setValue(steps);


    for (int i=0;i<pointcloudvector.size();i++)
        pointcloudvector[i].processCloud();

    boost::this_thread::sleep (boost::posix_time::seconds (1));
    steps++;
    pd->setValue(steps);

    return;

}

void RScloudgrabber::cancel()
{
    pd->cancel();
}

//RScloudgrabber* RScloudgrabber::instance = 0;
//RScloudgrabber* RScloudgrabber::Instance ()
//{
//    if (instance == 0)
//        instance = new RScloudgrabber;
//    return instance;
//}

PointCloud<pointT>::Ptr RScloudgrabber::txt2pointcloud(std::vector<float3rgb> txtcloud) {

    pcl::PointCloud<pointT>::Ptr cloud (new pcl::PointCloud<pointT>);

    for (int j=0;j<txtcloud.size();j++){
        pointT p;
        p.x = txtcloud[j].x;
        p.y = txtcloud[j].y;
        p.z = txtcloud[j].z;
        p.r = txtcloud[j].r;
        p.g = txtcloud[j].g;
        p.b = txtcloud[j].b;

        cloud->push_back(p);
    }
    return cloud;
}

std::vector<RScloud> RScloudgrabber::getPointcloudvector() const
{
    return pointcloudvector;
}






