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

using namespace std;
using namespace pcl;


class GrabberProgressDialog : public QObject {
     Q_OBJECT
public:
    explicit GrabberProgressDialog(QObject *parent = 0);
    signals:

public slots:
    std::vector<RScloud>  perform();
    void cancel();

private:
    QProgressDialog *pd;
    CloudsGrabber *grabber;

public:

        void setOutPath(std::string outpath) {
            GrabberProgressDialog::outPath = outpath;
        }

private:
        int steps;
//        int num_border_to_remove = 3;
//        bool filtering = true;
//        bool remove_border = true;
//        bool smoothing = true;
        std::string outPath;

};
#endif // GRABBERPROGRESSDIALOG

