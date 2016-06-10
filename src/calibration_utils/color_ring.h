#include <cstddef>
#include<string>

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

    void add(std::string name, int r, int g, int b) {
        Color *c = new Color;
        c->name = name;
        c->r = r;
        c->g = g;
        c->b = b;
        c->next = head;
        head = c;
    }

    // chiude ad anello la lista
    void makeRing() {
        Color *ptr = head;
        while (ptr->next != NULL)
            ptr = ptr->next;
        ptr->next = head;
    }

public:
    // genero una lista ad anello con 6 colori
    ColorRing() {
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

    // colora il punto passato con il colore attuale
    POINT_TYPE colorize(POINT_TYPE pt) {
        pt.r = head->r;
        pt.g = head->g;
        pt.b = head->b;
        return pt;
    }

    // nome del colore attuale (serve solo all'avvio)
    std::string getColorName() {
        return head->name;
    }

    // nome del prossimo colore
    std::string getNextColorName() {
        return head->next->name;
    }

    // passa al colore successivo
    void nextColor() {
        prec = head;
        head = head->next;
    }

    // torna al colore precedente
    void resetToLastColor() {
        head = prec;
    }

    // riparto dal primo colore
    void restart() {
        head = start;
    }
};

ColorRing color_left;
ColorRing color_right;