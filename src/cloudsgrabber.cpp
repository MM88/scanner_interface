#include <librealsense/rs.hpp>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <QPushButton>
#include <fstream>
#include <iostream>
#include <string>
#include "cloudsgrabber.h"
#include "rscloud.h"
#include <boost/algorithm/string.hpp>

using namespace rs;
using namespace cv;

CloudsGrabber* CloudsGrabber::instance = 0;
CloudsGrabber* CloudsGrabber::Instance ()
{
    if (instance == 0)
        instance = new CloudsGrabber;
    return instance;
}

void CloudsGrabber::grabClouds()
{
    pointcloudvector.clear();
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
//        dev->set_option((rs::option)14, (double)0); //motion range
        std::cout << "done." << std::endl;
    }

    // This tutorial will access only a single device, but it is trivial to extend to multiple devices

    const clock_t begin_time = clock(); //to check grabbing time
    std::vector<std::vector<float3rgb>> txtcloudvector;
//    float time = 0;
//    while (time < 300){
//         time = float(clock () - begin_time)/  CLOCKS_PER_SEC;
//    }
    for(auto dev : devices)
    {
        dev->set_option((rs::option)12, (double)16);

        // Capture frames to give autoexposure
        for (int i = 0; i < 15; ++i) dev->wait_for_frames();

        dev->wait_for_frames();
        // Retrieve our images
        const uint16_t * depth_image = (const uint16_t *)dev->get_frame_data(rs::stream::depth_aligned_to_color);
        const uint8_t * color_image = (const uint8_t *)dev->get_frame_data(rs::stream::color);

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
        rs::intrinsics depth_intrin = dev->get_stream_intrinsics(rs::stream::depth_aligned_to_color);
        rs::extrinsics depth_to_color = dev->get_extrinsics(rs::stream::depth_aligned_to_color, rs::stream::color);
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


    std::cout << "done scanning." << std::endl;
    std::cout << "saving clouds.." << std::endl;
    std::cout << "num of couds " << txtcloudvector.size() << std::endl;

    for (int i=0;i<txtcloudvector.size();i++){
            PointCloud<pointT>::Ptr cloud = txt2pointcloud(txtcloudvector[i]);
            RScloud rscloud;
            rscloud.setPointcloud(cloud);
            pointcloudvector.push_back(rscloud);
        }
    std::cout << "cloud points " << pointcloudvector[0].getPointcloud()->points.size() << std::endl;
    std::cout << "done saving." << std::endl;

    return;
}

void CloudsGrabber::transformClouds()
{
    for (int i=0;i<pointcloudvector.size();i++){

            if(i==3){
                std::ostringstream matrixPath1;
                std::ostringstream matrixPath2;
                std::string data1;
                std::string data2;
                Eigen::Matrix4f transformMatrix1 = Eigen::Matrix4f::Identity();
                Eigen::Matrix4f transformMatrix2 = Eigen::Matrix4f::Identity();
                matrixPath1 << "./../registrazione/matrix_"<<i<<".txt";
                matrixPath2 << "./../registrazione/matrix_1.txt";

                std::ifstream textMatrix1(matrixPath1.str());
                if (textMatrix1.is_open()){
                getline(textMatrix1, data1);
                for (int j=0;j<4;j++) {
                    std::vector<std::string> x1;
                    boost::split(x1,data1, boost::is_any_of("\t "));
                    std::stringstream ss1;
                    ss1 << x1[0];
                    ss1 >> transformMatrix1(j,0);
                    ss1.clear();
                    ss1 << x1[1];
                    ss1 >> transformMatrix1(j,1);
                    ss1.clear();
                    ss1 << x1[2];
                    ss1 >> transformMatrix1(j,2);
                    ss1.clear();
                    ss1 << x1[3];
                    ss1 >> transformMatrix1(j,3);
                    ss1.clear();
                    getline(textMatrix1, data1);
                 }
                textMatrix1.close();
                }
                pcl::PointCloud<pointT>::Ptr appCloud1 (new pcl::PointCloud<pointT>);
                pcl::transformPointCloud (*pointcloudvector[i].getPointcloud(), *appCloud1, transformMatrix1);
                pointcloudvector[i].setPointcloud(appCloud);

                std::ifstream textMatrix2(matrixPath2.str());
                if (textMatrix2.is_open()){
                getline(textMatrix2, data2);
                for (int j=0;j<4;j++) {
                    std::vector<std::string> x2;
                    boost::split(x2,data2, boost::is_any_of("\t "));
                    std::stringstream ss2;
                    ss2 << x2[0];
                    ss2 >> transformMatrix2(j,0);
                    ss2.clear();
                    ss2 << x2[1];
                    ss2 >> transformMatrix2(j,1);
                    ss2.clear();
                    ss2 << x2[2];
                    ss2 >> transformMatrix2(j,2);
                    ss2.clear();
                    ss2 << x2[3];
                    ss2 >> transformMatrix2(j,3);
                    ss2.clear();
                    getline(textMatrix2, data2);
                 }
                textMatrix2.close();
                }
                pcl::PointCloud<pointT>::Ptr appCloud2 (new pcl::PointCloud<pointT>);
                pcl::transformPointCloud (*pointcloudvector[i].getPointcloud(), *appCloud2, transformMatrix2);

                pointcloudvector[i].setPointcloud(appCloud);
            }
            std::ostringstream matrixPath;
            std::string data;
            Eigen::Matrix4f transformMatrix = Eigen::Matrix4f::Identity();
            matrixPath << "./../registrazione/matrix_"<<i<<".txt";
            std::ifstream textMatrix(matrixPath.str());
            if (textMatrix.is_open()){
            getline(textMatrix, data);
            for (int j=0;j<4;j++) {
                std::vector<std::string> x;
                boost::split(x,data, boost::is_any_of("\t "));
                std::stringstream ss;
                ss << x[0];
                ss >> transformMatrix(j,0);
                ss.clear();
                ss << x[1];
                ss >> transformMatrix(j,1);
                ss.clear();
                ss << x[2];
                ss >> transformMatrix(j,2);
                ss.clear();
                ss << x[3];
                ss >> transformMatrix(j,3);
                ss.clear();
                getline(textMatrix, data);
             }
            textMatrix.close();
            }
            pcl::PointCloud<pointT>::Ptr appCloud (new pcl::PointCloud<pointT>);
            pcl::transformPointCloud (*pointcloudvector[i].getPointcloud(), *appCloud, transformMatrix);
            pointcloudvector[i].setPointcloud(appCloud);
        }
}

PointCloud<pointT>::Ptr CloudsGrabber::txt2pointcloud(std::vector<float3rgb> txtcloud)
{
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

void CloudsGrabber::processClouds()
{
    for (int i=0;i<pointcloudvector.size();i++)
        pointcloudvector[i].processCloud();
}

std::vector<RScloud> CloudsGrabber::getPointcloudvector() const
{
    return pointcloudvector;
}
