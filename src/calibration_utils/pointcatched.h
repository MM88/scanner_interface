#ifndef POINTCATCHED_H
#define POINTCATCHED_H

#include <cstddef>
#include <pcl/point_types.h>

#define pointT pcl::PointXYZRGB // con colori
//#define pointTnormal pcl::PointXYZRGBNormal // con colori
// #define pointT pcl::PointXYZ // senza colori
 #define pointTnormal pcl::Normal // senza colori

// lista per raccogliere i punti premuti
class pointCatched {
    struct Node {
        int index;
        float x;
        float y;
        float z;
        Node *next;
    };

    Node *head;
    int num_points;

public:

    pointCatched();

    // distruttore che non funfa (e non credo che nemmeno serva)
    /*~pointCatched() {
        clear_stack;
    }*/

    // inserimento in testa
    void add(int new_index, pointT new_point);

    // rimozione dalla testa - restituisce l'ID del punto eliminato
    int remove_last() ;
    // svuota la lista
    void clear_stack();

    void print_all() ;
    // non so perché ogni tanto prende un punto con le stesse coordinate del precedente (anche se clicco molto più in là), lo scarto
    bool checkPointError(int new_index, pointT new_point) ;
    Eigen::MatrixXf makeMatrix() ;

};

#endif
