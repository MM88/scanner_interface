#ifndef COLORRING_H
#define COLORRING_H

#include <cstddef>
#include<string>
#include <pcl/point_types.h>

using namespace pcl;

#define pointT pcl::PointXYZRGB // con colori
//#define pointTnormal pcl::PointXYZRGBNormal // con colori
// #define pointT pcl::PointXYZ // senza colori
 #define pointTnormal pcl::Normal // senza colori

class ColorRing {

    struct Color {
        std::string name;
        int r;
        int g;
        int b;
        Color *next;
    };

    Color *head;
    Color *prec;
    Color *start;

    void add(std::string name, int r, int g, int b);

    // chiude ad anello la lista
    void makeRing() ;
public:
    // genero una lista ad anello con 6 colori
    ColorRing();

    // colora il punto passato con il colore attuale
    pointT colorize(pointT pt) ;
    // nome del colore attuale (serve solo all'avvio)
    std::string getColorName();
    // nome del prossimo colore
    std::string getNextColorName() ;

    // passa al colore successivo
    void nextColor() ;

    // torna al colore precedente
    void resetToLastColor();

    // riparto dal primo colore
    void restart();

};

#endif
