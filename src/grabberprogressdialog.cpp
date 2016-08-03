#include "grabberprogressdialog.h"
#include <stdio.h>
#include <QPushButton>
#include <fstream>
#include "cloudsgrabber.h"


GrabberProgressDialog::GrabberProgressDialog(QObject *parent) :
    QObject(parent)
{
    grabber = CloudsGrabber::Instance();
    pd = new QProgressDialog();
    pd = new QProgressDialog("Elaborazione in corso", "Annulla", 0, 6);
    pd->setWindowModality(Qt::WindowModal);
    connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
    pd->show();
}

pcl::PolygonMesh GrabberProgressDialog::perform()
{
    steps = 1;

    pd->setValue(steps);

    grabber->grabClouds();

    boost::this_thread::sleep (boost::posix_time::seconds (1));
    steps++;
    pd->setValue(steps);

    grabber->processClouds();
    boost::this_thread::sleep (boost::posix_time::seconds (1));

    steps++;
    pd->setValue(steps);

    boost::this_thread::sleep (boost::posix_time::seconds (1));

    grabber->transformClouds();

    std::vector<RScloud> pointcloudvector =  grabber->getPointcloudvector();

    steps++;
    pd->setValue(steps);

    boost::this_thread::sleep (boost::posix_time::seconds (1));

    pcl::PointCloud<pointT>::Ptr cloudSum (new pcl::PointCloud<pointT>);
    for (int i=0; i<pointcloudvector.size();i++){
        if (i == 0)
            pcl::copyPointCloud(*pointcloudvector[i].getPointcloud(), *cloudSum);
        else
            *cloudSum+=*pointcloudvector[i].getPointcloud();
    }
    RScloud rscloud;
    rscloud.setPointcloud(cloudSum);
    rscloud.processCloud();
            std::stringstream file;
            file<<"/home/miky/Scrivania/cloudSum.ply";
            pcl::io::savePLYFileBinary (file.str(), *rscloud.getPointcloud());

    steps++;
    pd->setValue(steps);

    boost::this_thread::sleep (boost::posix_time::seconds (1));

//    rscloud.smooth_cloud();
    pcl::PolygonMesh mesh = rscloud.triangulate_cloud();

    steps++;
    pd->setValue(steps);

    boost::this_thread::sleep (boost::posix_time::seconds (1));


    return mesh;
}

void GrabberProgressDialog::cancel()
{
    pd->cancel();
}







