#include <iostream>
#include <ncurses.h>

#define SZEROKOSC 40
#define WYSOKOSC 20

using std::cout;
using std::endl;

typedef struct punkt_t {
    int x, y;
} punkt_t;

typedef struct gra {
    char plansza[WYSOKOSC][SZEROKOSC];
} gra;

void wypisz_plansze() {
    for (int i = 0; i < WYSOKOSC; ++i) {
        for (int j = 0; j < SZEROKOSC; ++j) {
            if (i==0) {
                cout <<
            }
            cout << "*";
        }
        cout << endl;
    }
}

int main() {
    wypisz_plansze();
    return 0;
}
