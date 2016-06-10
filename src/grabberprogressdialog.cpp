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
    pd = new QProgressDialog("Elaborazione in corso", "Annulla", 0, 3);
    pd->setWindowModality(Qt::WindowModal);
    connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
    pd->show();
}

std::vector<RScloud> GrabberProgressDialog::perform()
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

    std::vector<RScloud> pointcloudvector =  grabber->getPointcloudvector();

    steps = 0;
    return pointcloudvector;

}

void GrabberProgressDialog::cancel()
{
    pd->cancel();
}







