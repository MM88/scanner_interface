
#include <cstddef>
#include <pcl/point_types.h>
#include "pointcatched.h"

#define POINT_TYPE pcl::PointXYZRGBA // con colori
// #define POINT_TYPE pcl::PointXYZ // senza colori

// lista per raccogliere i punti premuti


pointCatched::pointCatched() {
    head = NULL;
    num_points = 0;
}

// distruttore che non funfa (e non credo che nemmeno serva)
/*~pointCatched() {
    clear_stack;
}*/

// inserimento in testa
void pointCatched::add(int new_index, pointT new_point) {
    Node *n = new Node;
    n->index = new_index;
    n->x = new_point.x;
    n->y = new_point.y;
    // valore della seconda cloud fixato
    n->z = (new_point.z > 3 ? new_point.z -3 : new_point.z);
    n->next = head;
    head = n;
    num_points++;
}

// rimozione dalla testa - restituisce l'ID del punto eliminato
int pointCatched::remove_last() {
    int id = head->index;
    Node *n = head;
    head = head->next;
    num_points--;
    delete n;
    return id;
}

// svuota la lista
void pointCatched::clear_stack() {
    while (head != NULL) {
        Node *n = head->next;
        delete head;
        head = n;
    }
    num_points = 0;
}

void pointCatched::print_all() {
    int i = 1;
    Node *ptr = head;
    while (ptr != NULL) {
        std::cout << i << ". ID: " << ptr->index << " (" << ptr->x << ", " << ptr->y << ", " << ptr->z << ") " << std::endl;
        i++;
        ptr = ptr->next;
    }
}

// non so perché ogni tanto prende un punto con le stesse coordinate del precedente (anche se clicco molto più in là), lo scarto
bool pointCatched::checkPointError(int new_index, pointT new_point) {
    Node *ptr = head;
    while (ptr != NULL) {
        if (new_index == ptr->index)
            return true; // stesso indice, punto già preso (raro ma ci può stare)
        if (new_point.x == ptr->x && new_point.y == ptr->y && new_point.z == ptr->z)
            return true; // stesse coordinate, non prendo di nuovo
        ptr = ptr->next;
    }
    return false;
}

Eigen::MatrixXf pointCatched::makeMatrix() {
    int i = 0;
    Eigen::MatrixXf M(3, num_points);
    Node *ptr = head;
    while (ptr != NULL) {
        M(0, i) = ptr->x;
        M(1, i) = ptr->y;
        M(2, i) = ptr->z;
        ptr = ptr->next;
        i++;
    }
    return M;
}

