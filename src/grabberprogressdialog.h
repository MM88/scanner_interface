#ifndef GRABBERPROGRESSDIALOG
#define GRABBERPROGRESSDIALOG

#include <iostream>
#include "grabberprogressdialog.h"
#include <vector>
#include <fstream>
#include "rscloud.h"
#include <QObject>
#include <QProgressDialog>
#include <QTimer>
#include "cloudsgrabber.h"
#include <pcl/point_types.h>

using namespace std;
using namespace pcl;


class GrabberProgressDialog : public QObject {
     Q_OBJECT
public:
    explicit GrabberProgressDialog(QObject *parent = 0);
    signals:

public slots:
    pcl::PolygonMesh perform();
    void cancel();

private:
    QProgressDialog *pd;
    CloudsGrabber *grabber;


private:
        int steps;
//        int num_border_to_remove = 3;
//        bool filtering = true;
//        bool remove_border = true;
//        bool smoothing = true;

};
#endif // GRABBERPROGRESSDIALOG

