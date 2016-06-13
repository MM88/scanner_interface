#include <boost/make_shared.hpp>
#include <pcl/visualization/pcl_visualizer.h>
#include "colorring.h"
#include "pointcatched.h"
#include "transformationutils.h"


using namespace std;
using namespace pcl;

#define POINT_TYPE pcl::PointXYZRGBA // con colori
// #define POINT_TYPE pcl::PointXYZ // senza colori
#define TRANLSATION_Z_SECOND_CLOUD 3.0 // 3 metri lungo la Z (meglio della X o Y perché sempre positiva)

class VisualizationUtils {

private:

    ColorRing color_left;
    ColorRing color_right;
    pointCatched points_left;
    pointCatched points_right;

    //unsigned int click_id = 1; // per il mouse event

    // non riesco a recuperare la view dalle varie callback, così me la salvo globale e ciao
    boost::shared_ptr<pcl::visualization::PCLVisualizer> global_viewer;
    int v1, v2; // conterranno l'id delle due visualizzazioni, necessari per riferirsi a loro successivamente

    pcl::PointCloud<POINT_TYPE>::Ptr clicked_points;
    pcl::PointCloud<POINT_TYPE>::Ptr clicked_points2;

    // per ricordarsi da che parte era l'ultimo punto preso
    bool isSecondCloud;
    // abilita la possibilità di eliminare l'ultimo punto
    bool canIRollBack = false;
    POINT_TYPE previous_points;
public:
    VisualizationUtils();
    void loop_view(boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer) ;
    void updateClickedPoints();
    void keyboardEventOccurred(const pcl::visualization::KeyboardEvent &event, void* args);
    void pointPickDoubleViewEvent(const pcl::visualization::PointPickingEvent& event, void* args) ;
    void vis_doppia(std::string name1, std::string name2);

    boost::shared_ptr<pcl::visualization::PCLVisualizer> getGlobal_viewer() const;
};
