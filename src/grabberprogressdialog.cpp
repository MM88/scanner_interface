#include "grabberprogressdialog.h"
#include <stdio.h>
#include <QPushButton>
#include <fstream>
#include "cloudsgrabber.h"


GrabberProgressDialog::GrabberProgressDialog(QObject *parent) :
    QObject(parent)
{
    pd = new QProgressDialog();
    pd = new QProgressDialog("Elaborazione in corso", "Annulla", 0, 3);
//    pd->setWindowModality(Qt::WindowModal);
    connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
    pd->show();
}

std::vector<RScloud> GrabberProgressDialog::perform()
{
    steps = 1;
    pd->setValue(steps);

    CloudsGrabber *grabber = CloudsGrabber::Instance();
    grabber->grabClouds();

    boost::this_thread::sleep (boost::posix_time::seconds (1));
    steps++;
    pd->setValue(steps);

    grabber->processClouds();
    boost::this_thread::sleep (boost::posix_time::seconds (1));
    steps++;
    pd->setValue(steps);

    return grabber->getPointcloudvector();

}

void GrabberProgressDialog::cancel()
{
    pd->cancel();
}







