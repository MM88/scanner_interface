
#include <cstddef>
#include<string>
#include "colorring.h"


ColorRing::ColorRing() {
    head = NULL;
    // verranno usati al contrario, poiché per praticità faccio inserimento in testa
    add("marrone", 130, 90, 44);
    add("magenta", 216, 0, 115);
    add("giallo", 227, 200, 0);
    add("blu", 0, 0, 255);
    add("verde", 0, 255, 0);
    add("rosso", 255, 0, 0);
    start = head;
    makeRing();
}

void ColorRing::add(std::string name, int r, int g, int b) {
    Color *c = new Color;
    c->name = name;
    c->r = r;
    c->g = g;
    c->b = b;
    c->next = head;
    head = c;
}

// chiude ad anello la lista
void ColorRing::makeRing() {
    Color *ptr = head;
    while (ptr->next != NULL)
        ptr = ptr->next;
    ptr->next = head;
}


// colora il punto passato con il colore attuale
pointT ColorRing::colorize(pointT pt) {
    pt.r = head->r;
    pt.g = head->g;
    pt.b = head->b;
    return pt;
}

// nome del colore attuale (serve solo all'avvio)
std::string ColorRing::getColorName() {
    return head->name;
}

// nome del prossimo colore
std::string ColorRing::getNextColorName() {
    return head->next->name;
}

// passa al colore successivo
void ColorRing::nextColor() {
    prec = head;
    head = head->next;
}

// torna al colore precedente
void ColorRing::resetToLastColor() {
    head = prec;
}

// riparto dal primo colore
void ColorRing::restart() {
    head = start;
}



