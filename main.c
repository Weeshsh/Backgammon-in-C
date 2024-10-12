#define _CRT_SECURE_NO_WARNINGS
#include "conio2.h"
#include <time.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>


#define AUTOR "autor: Mikolaj Wiszniewski 197925"
#define GRACZ_B 1
#define GRACZ_CZ 0
#define PION_B "BB"
#define PION_C "CC"
#define ROZM_PL 24
#define WYS_POLA 8
#define SZER_POLA 7
#define PION_BARIERA '|'
#define POZ_BARIERA '-'
#define LACZENIE '+'
#define SRODEK '='
#define PUSTE_GORA "\\/"
#define PUSTE_DOL "/\\"
#define PUSTE_2 ".."
#define X0 3
#define Y0 3
#define WYS (WYS_POLA * 2 + 4)
#define SZER (SZER_POLA * (ROZM_PL / 2 + 1))
#define KOST1 SZER/2-SZER_POLA + 14, WYS/2+Y0
#define KOST2 SZER/2-SZER_POLA + 18, WYS/2+Y0
#define MENU_X0 X0+SZER/2-SZER_POLA
#define MENU_Y0 Y0+WYS+3
#define BICIE 'B'
#define RUCH 'R'
#define ZDJECIE_Z_BANDY 'Z'
#define ODSTAW_DOM 'O'
#define STRZ_P 0x4D
#define STRZ_L 0x4B
#define STRZ_G 0x48
#define STRZ_D 0x50
#define LIMIT_NAZWY 128
#define MAX_GRACZY 100
#define MAX_CYFR 20

typedef struct {
    int pionki_b, pionki_c;
    int x0, y0;
} pole_t;
typedef struct {
    int oczka1, oczka2;
} rzut_t;
typedef struct {
    int dwor, banda;
    int wynik;
} gracz_t;
typedef struct {
    pole_t plansza[ROZM_PL];
    int czyj_ruch, reset, zapisuj;
    rzut_t rzut[2];
    gracz_t bialy, czarny;
} stan_gry;
typedef struct {
    int wynik;
    char nazwa[LIMIT_NAZWY];
} zawodnik_t;

int suma_oczek(const stan_gry* gra) {
    int suma = 0;
    for (int i = 0; i < 2; i++) {
        suma += gra->rzut[i].oczka1;
        suma += gra->rzut[i].oczka2;
    }
    return suma;
}
void wyzeruj_ruchy(stan_gry* gra) {
    for (int i = 0; i < 2; i++) {
        gra->rzut[i].oczka1 = 0;
        gra->rzut[i].oczka2 = 0;
    }
}

void wypisz_legende(void) {
    gotoxy(MENU_X0, MENU_Y0 - 1);
    cputs("Strzalka w prawo - nastepny ruch\n");
    gotoxy(MENU_X0, MENU_Y0);
    cputs("Strzalka w lewo - poprzedni ruch\n");
    gotoxy(MENU_X0, MENU_Y0 + 1);
    cputs("Strzalka w dol - poczatek partii\n");
    gotoxy(MENU_X0, MENU_Y0 + 2);
    cputs("Strzalka w gore - koniec partii\n");
    gotoxy(MENU_X0, MENU_Y0 + 3);
    cputs("Q - powrot do menu glownego\n");
}
void wypisz_menu(void) {
    gotoxy(MENU_X0, MENU_Y0);
    cputs("[1] Graj");
    gotoxy(MENU_X0, MENU_Y0+1);
    cputs("[2] Zapisz gre i wyjdz");
}
void wypisz_niepr_ruch(bool wypisz = 0) {
    gotoxy(MENU_X0 - 4 * SZER_POLA, MENU_Y0);
    if (wypisz)
        cputs("[NIEPRAWIDLOWY RUCH]");
    else
        cputs("                    ");
}
void wypisz_zbito_pionek(bool wypisz = 0) {
    gotoxy(MENU_X0 - 4 * SZER_POLA, MENU_Y0 + 1);
    if (wypisz)
        cputs("zbito pionek");
    else
        cputs("            ");
}
void wypisz_bande(const stan_gry* gra) {
    char temp[MAX_CYFR];
    gotoxy(X0 + SZER / 2 - 1, Y0 + WYS_POLA / 2);
    sprintf(temp, "%2d", gra->czarny.banda);
    cputs("C:");
    cputs(temp);
    gotoxy(X0 + SZER / 2 - 1, Y0 + WYS - WYS_POLA / 2);
    sprintf(temp, "%2d", gra->bialy.banda);
    cputs("B:");
    cputs(temp);

}
void wypisz_dwor(const stan_gry* gra) {
    char temp[MAX_CYFR];
    for (int y = 0; y < 3; y++) {
        gotoxy(SZER + X0 + 2, WYS / 2 + Y0 - 1 + y);
        switch (y) {
        case 0:
            cputs("C: ");
            sprintf(temp, "%2d", gra->czarny.dwor);
            cputs(temp);
            break;
        case 1:
            cputs("[DWOR]");
            break;
        case 2:
            cputs("B: ");
            sprintf(temp, "%2d", gra->bialy.dwor);
            cputs(temp);
        }
    }
}
void wypisz_nadmiar(const stan_gry* gra, int indeks) {
    pole_t p_i = gra->plansza[indeks];

    if (indeks < ROZM_PL/2)
        gotoxy(p_i.x0 - 1, Y0 + WYS / 2 - 1);
    else
        gotoxy(p_i.x0 - 1, Y0 + WYS / 2 + 1);

    if (p_i.pionki_b > WYS_POLA - 2 || p_i.pionki_c > WYS_POLA - 2) {
        char temp[MAX_CYFR];
        int nadmiar = 0;
        if (p_i.pionki_b > WYS_POLA - 2)
            nadmiar = p_i.pionki_b - WYS_POLA + 2;
        else if (p_i.pionki_c > WYS_POLA - 2)
            nadmiar = p_i.pionki_c - WYS_POLA + 2;
        sprintf(temp, "[+%d]", nadmiar);
        cputs(temp);
    }
    else {
        cputs("    ");
    }
}
void wypisz_pole(const stan_gry* gra, int indeks) {
    pole_t p_i = gra->plansza[indeks];
    int p_b = p_i.pionki_b, p_c = p_i.pionki_c;
    int x0 = p_i.x0, y0 = p_i.y0;
    for (int y = 0; y < WYS_POLA; y++) {
        indeks < ROZM_PL/2 ? gotoxy(x0, y + y0) : gotoxy(x0, y0 - y);
        if (y == 0) {
            char temp[MAX_CYFR];
            sprintf(temp, "%2d", indeks + 1);
            cputs(temp);
        }
        if (y > 1) {
            if (p_b > 0 && y - 2 < p_b)
                cputs(PION_B);
            else if (p_c > 0 && y - 2 < p_c)
                cputs(PION_C);
            else
                if (indeks < 12)
                    cputs((indeks % 2 == 1) ? PUSTE_GORA : PUSTE_2);
                else
                    cputs((indeks % 2 == 1) ? PUSTE_DOL : PUSTE_2);
        }
    }
    wypisz_nadmiar(gra, indeks);
}
void wypisz_czyj_ruch(const stan_gry* gra) {
    if (gra->czyj_ruch == GRACZ_B) {
        gotoxy(SZER - 8, 1);
        cputs("RUCH BIALEGO ");
    }
    else {
        gotoxy(SZER - 8, 1);
        cputs("RUCH CZARNEGO");
    }
}
void wypisz_rzut(const stan_gry* gra) {
    for (int i = 1; i >= 0; i--) {
        int r1 = gra->rzut[i].oczka1;
        int r2 = gra->rzut[i].oczka2;
        gotoxy(KOST1);
        putch((r1 == 0) ? ('X') : ('0' + r1));

        gotoxy(KOST2);
        putch((r2 == 0) ? ('X') : ('0' + r2));
    }
}
void wypisz_pola(const stan_gry* gra) {
    for (int i = 0; i < ROZM_PL; i++) {
        wypisz_pole(gra, i);
    }
}
void wypisz_elementy_stale(void) {
    gotoxy(1, 1);
    cputs(AUTOR);

    for (int y = 0; y <= WYS; y++) {
        for (int x = 0; x <= SZER; x++) {
            gotoxy(x + X0, y + Y0);
            if (x != 0 && x != SZER) {
                if (y == WYS / 2 - 2 || y == WYS / 2 + 2)
                    putch(SRODEK);

                if (y == WYS / 2 || y == WYS / 2 - 1 || y == WYS / 2 + 1) {
                    if (x == SZER / 2 - SZER_POLA && y == WYS / 2)
                        cputs("wylosowano   i  ");
                    continue;
                }
            }
            if (x % SZER_POLA == 0) {
                if ((y == 1 || y == WYS - 1) && (x != 0 && x != SZER))
                    putch(LACZENIE);
                putch(PION_BARIERA);
            }
            else if (y == 1 || y == WYS - 1) {
                if (x >= (SZER - SZER_POLA) / 2 && x <= (SZER + SZER_POLA) / 2)
                    putch(SRODEK);
                putch(POZ_BARIERA);
            }
        }
    }
}
void wypisz_gre(const stan_gry* gra) {
    clrscr();
    gotoxy(0, 0);
    wypisz_elementy_stale();
    wypisz_pola(gra);
    wypisz_dwor(gra);
    wypisz_bande(gra);
    wypisz_rzut(gra);
    wypisz_czyj_ruch(gra);
}


void inicjalizuj_pionki(stan_gry* gra) {
    for (int i = 0; i < ROZM_PL; i++) {
        gra->plansza[i].pionki_b = 0;
        gra->plansza[i].pionki_c = 0;
    }

    gra->plansza[5].pionki_b = 5;
    gra->plansza[7].pionki_b = 3;
    gra->plansza[12].pionki_b = 5;
    gra->plansza[23].pionki_b = 2;

    gra->plansza[0].pionki_c = 2;
    gra->plansza[11].pionki_c = 5;
    gra->plansza[16].pionki_c = 3;
    gra->plansza[18].pionki_c = 5;
}
void inicjalizuj_wspolrzedne_pol(stan_gry* gra) {
    for (int i = 0; i < ROZM_PL / 2; i++) {
        if (i < 6)
            gra->plansza[i].x0 = SZER - (i * SZER_POLA + 1);
        else
            gra->plansza[i].x0 = SZER - ((i + 1) * SZER_POLA + 1);
        gra->plansza[i].y0 = Y0;
    }
    for (int i = (ROZM_PL / 2); i < ROZM_PL; i++) {
        if (i > 17)
            gra->plansza[i].x0 = X0 + SZER_POLA / 2 + (i - (ROZM_PL / 2) + 1) * SZER_POLA;
        else
            gra->plansza[i].x0 = X0 + SZER_POLA / 2 + (i - (ROZM_PL / 2)) * SZER_POLA;

        gra->plansza[i].y0 = WYS + Y0;
    }
}
void inicjalizuj(stan_gry* gra) {
    inicjalizuj_pionki(gra);
    inicjalizuj_wspolrzedne_pol(gra);
    gra->czyj_ruch = -1;

    gra->bialy.wynik = 0;
    gra->bialy.dwor = 0;
    gra->bialy.banda = 0;

    gra->czarny.dwor = 0;
    gra->czarny.wynik = 0;
    gra->czarny.banda = 0;

    gra->reset = 0;
    gra->zapisuj = 0;
}


void gra_wykorzystaj_ruch(stan_gry* gra, int ruch) {
    for (int i = 0; i < 2; i++) {
        if (ruch == gra->rzut[i].oczka1) {
            gra->rzut[i].oczka1 = 0;
            break;
        }
        else if (ruch == gra->rzut[i].oczka2) {
            gra->rzut[i].oczka2 = 0;
            break;
        }
    }
}
void gra_zdejmij_z_planszy_bialy(stan_gry* gra, int **rzuty, int i) {
    for (int pole = 5; pole >= 0; pole--) {
        int pole_z = *rzuty[i];
        if (!pole_z)
            continue;
        if (pole <= pole_z - 1 && gra->plansza[pole].pionki_b > 0) {
            gra->plansza[pole].pionki_b--;
            gra->bialy.dwor++;
            gra_wykorzystaj_ruch(gra, *rzuty[i]);
            wypisz_pole(gra, pole);
            Sleep(200);
            wypisz_dwor(gra);
            return;
        }
    }
}
void gra_zdejmij_z_planszy_czarny(stan_gry* gra, int** rzuty, int i) {
    for (int pole = 18; pole < ROZM_PL; pole++) {
        int pole_z = ROZM_PL - *rzuty[i];
        if (!(*rzuty[i]))
            continue;
        if (pole >= pole_z && gra->plansza[pole].pionki_c > 0) {
            gra->plansza[pole].pionki_c--;
            gra->czarny.dwor++;
            gra_wykorzystaj_ruch(gra, *rzuty[i]);
            wypisz_pole(gra, pole);
            Sleep(200);
            wypisz_dwor(gra);
            return;
        }
    }
}
void gra_zdejmij_z_planszy(stan_gry* gra) {
    int* rzuty[4] = { &gra->rzut[0].oczka1, &gra->rzut[0].oczka2, &gra->rzut[1].oczka1, &gra->rzut[1].oczka2 };
    for (int i = 0; i < 4; i++) {
        switch (gra->czyj_ruch) {
        case GRACZ_B:
            gra_zdejmij_z_planszy_bialy(gra, rzuty, i);
            break;
        case GRACZ_CZ:
            gra_zdejmij_z_planszy_czarny(gra, rzuty, i);
            break;
        }
    }
}
void gra_rzut_kostkami(stan_gry* gra) {
    srand((unsigned int)time(NULL));
    int r_1 = rand() % 6 + 1;
    int r_2 = rand() % 6 + 1;
    gra->rzut[0] = { r_1, r_2 };
    gra->rzut[1] = { 0, 0 };
    if (r_1 == r_2)
        gra->rzut[1] = { r_1, r_1 };
    wypisz_rzut(gra);
}
void gra_zbij_pionek(stan_gry* gra, int pole_z, int pole_cel) {
    switch (gra->czyj_ruch) {
    case GRACZ_B:
        gra->plansza[pole_z].pionki_b--;
        gra->plansza[pole_cel].pionki_c--;
        Sleep(500);
        gra->plansza[pole_cel].pionki_b++;
        gra->czarny.banda++;
        break;
    case GRACZ_CZ:
        gra->plansza[pole_z].pionki_c--;
        gra->plansza[pole_cel].pionki_b--;
        Sleep(500);
        gra->plansza[pole_cel].pionki_c++;
        gra->bialy.banda++;
        break;
    }
    wypisz_pole(gra, pole_z);
    wypisz_pole(gra, pole_cel);
    wypisz_bande(gra);
}
void lista_ruchow(stan_gry* gra, int* ruchy[4]) {
    rzut_t* r1 = &gra->rzut[0], * r2 = &gra->rzut[1];
    ruchy[0] = &r1->oczka1;
    ruchy[1] = &r1->oczka2;
    ruchy[2] = &r2->oczka1;
    ruchy[3] = &r2->oczka2;
    if (*ruchy[0] > *ruchy[1]) {
        ruchy[0] = &r1->oczka1;
        ruchy[1] = &r1->oczka2;
    }
}
int  gra_szukaj_bicia(stan_gry* gra) {
    int* ruchy[4];
    lista_ruchow(gra, ruchy);
    int r=0, p_akt, p_pc, p_c, p_t;
    for (int i = 0; i < 4; i++) {
        r = *(ruchy[i]);
        if (r) {
            for (int p = 0; p < ROZM_PL; p++) {
                switch (gra->czyj_ruch) {
                case GRACZ_CZ:
                    p_t = p;
                    p_c = p + r;
                    p_akt = gra->plansza[p_t].pionki_c;
                    p_pc = gra->plansza[p_c].pionki_b;
                    break;
                case GRACZ_B:
                    p_t = ROZM_PL - p - 1;
                    p_c = p_t - r;
                    p_akt = gra->plansza[p_t].pionki_b;
                    p_pc = gra->plansza[p_c].pionki_c;
                }
                if (p_c >= 0 && p_c < ROZM_PL)
                    if (p_pc == 1 && p_akt > 0) {
                        gra_zbij_pionek(gra, p_t, p_c);
                        wypisz_zbito_pionek(1);
                        gra_wykorzystaj_ruch(gra, r);
                        return 1;
                    }
            }
        }
    }
    return 0;
}
void gra_rusz_pionek(stan_gry* gra, int pole, int ile) {
    gra_wykorzystaj_ruch(gra, ile);
    int pole_cel = (gra->czyj_ruch == GRACZ_CZ) ? (pole + ile) : (pole - ile);
    switch (gra->czyj_ruch) {
    case GRACZ_B:
        gra->plansza[pole].pionki_b--;
        Sleep(200);
        gra->plansza[pole_cel].pionki_b++;
        break;
    case GRACZ_CZ:
        gra->plansza[pole].pionki_c--;
        Sleep(200);
        gra->plansza[pole_cel].pionki_c++;
        break;
    }
    wypisz_pole(gra, pole_cel);
    wypisz_pole(gra, pole);
}
void gra_zdejmij_z_bandy(stan_gry* gra, int gdzie) {
    pole_t* p_c = &gra->plansza[gdzie - 1];
    switch (gra->czyj_ruch) {
    case GRACZ_B:
        gra_wykorzystaj_ruch(gra, ROZM_PL - gdzie + 1);
        gra->bialy.banda--;
        Sleep(200);
        if (p_c->pionki_c == 1) {
            p_c->pionki_c--;
            Sleep(200);
            wypisz_zbito_pionek(1);
            gra->czarny.banda++;
        }
        p_c->pionki_b++;
        break;
    case GRACZ_CZ:
        gra_wykorzystaj_ruch(gra, gdzie);
        gra->czarny.banda--;
        Sleep(200);
        if (p_c->pionki_b == 1) {
            p_c->pionki_b--;
            Sleep(200);
            wypisz_zbito_pionek(1);
            gra->bialy.banda++;
        }
        p_c->pionki_c++;
        break;
    }
    wypisz_bande(gra);
    wypisz_pole(gra, gdzie - 1);
}


void sprawdz_zdjecie_z_bandy(const stan_gry* gra, int* mozliwe) {
    if (gra->bialy.banda || gra->czarny.banda) {
        int ruchy[] = { gra->rzut[0].oczka1, gra->rzut[0].oczka2, gra->rzut[1].oczka1, gra->rzut[1].oczka2 };
        for (int i = 0; i < 4; i++) {
            if (!ruchy[i])
                continue;
            int pole_w = (gra->czyj_ruch == GRACZ_CZ) ? (ruchy[i] - 1) : (ROZM_PL - ruchy[i]);
            const pole_t* p_w = &gra->plansza[pole_w];
            int przec_pionki = (gra->czyj_ruch == GRACZ_CZ) ? (p_w->pionki_b) : (p_w->pionki_c);
            if (przec_pionki < 2)
                mozliwe[(mozliwe[0] == 0) ? 0 : 1] = pole_w + 1;
        }
    }
}
bool sprawdz_ruch(const stan_gry* gra, int pole, int ile) {
    int pole_cel = (gra->czyj_ruch == GRACZ_CZ) ? (pole + ile) : (pole - ile);
    if (ile > 0 && pole < ROZM_PL && pole >= 0 && pole_cel >= 0 && pole_cel < ROZM_PL) {
        pole_t p = gra->plansza[pole], p_c = gra->plansza[pole_cel];
        if (ile == gra->rzut[0].oczka1 || ile == gra->rzut[0].oczka2 || ile == gra->rzut[1].oczka1 || ile == gra->rzut[1].oczka2) {
            if (gra->czyj_ruch == GRACZ_B && p.pionki_b > 0 && p_c.pionki_c == 0)
                return 1;
            if (gra->czyj_ruch == GRACZ_CZ && p.pionki_c > 0 && p_c.pionki_b == 0)
                return 1;
        }
    }
    return 0;
}
int czy_zdejm_z_planszy(const stan_gry* gra) {
    int count = (gra->czyj_ruch == GRACZ_CZ) ? (gra->czarny.dwor) : (gra->bialy.dwor);
    for (int i = 0; i < ROZM_PL; i++) {
        if (i <= 5 && gra->czyj_ruch == GRACZ_B)
            count += gra->plansza[i].pionki_b;
        else if (i >= 18 && gra->czyj_ruch == GRACZ_CZ)
            count += gra->plansza[i].pionki_c;
    }
    return count;
}


void plik_zapisz_stan(const stan_gry* gra) {
    FILE* f = fopen("zapisgry.txt", "w");

    fwrite(&gra->plansza, sizeof(gra->plansza), 1, f);
    fwrite(&gra->rzut, sizeof(gra->rzut), 1, f);

    fwrite(&gra->bialy.banda, sizeof(int), 1, f);
    fwrite(&gra->bialy.dwor, sizeof(int), 1, f);
    fwrite(&gra->bialy.wynik, sizeof(int), 1, f);

    fwrite(&gra->czarny.banda, sizeof(int), 1, f);
    fwrite(&gra->czarny.dwor, sizeof(int), 1, f);
    fwrite(&gra->czarny.wynik, sizeof(int), 1, f);

    fwrite(&gra->czyj_ruch, sizeof(int), 1, f);

    fwrite(&gra->reset, sizeof(int), 1, f);

    fwrite(&gra->zapisuj, sizeof(int), 1, f);

    fclose(f);
}
void plik_wczytaj_stan(stan_gry* gra) {
    FILE* f = fopen("zapisgry.txt", "r");

    if (f == NULL) {
        perror("Error");
        Sleep(200);
        return;
    }

    fread(&gra->plansza, sizeof(gra->plansza), 1, f);
    fread(&gra->rzut, sizeof(gra->rzut), 1, f);

    fread(&gra->bialy.banda, sizeof(int), 1, f);
    fread(&gra->bialy.dwor, sizeof(int), 1, f);
    fread(&gra->bialy.wynik, sizeof(int), 1, f);

    fread(&gra->czarny.banda, sizeof(int), 1, f);
    fread(&gra->czarny.dwor, sizeof(int), 1, f);
    fread(&gra->czarny.wynik, sizeof(int), 1, f);

    fread(&gra->czyj_ruch, sizeof(int), 1, f);

    fread(&gra->reset, sizeof(int), 1, f);

    fread(&gra->zapisuj, sizeof(int), 1, f);

    fclose(f);
}
void plik_zapisz_ruch(const stan_gry* gra) {
    FILE* f = fopen("zapisruchow.txt", "a");

    for (int i = 0; i < ROZM_PL; i++) {
        fprintf(f, "%2d ", gra->plansza[i].pionki_b);
        fprintf(f, "%2d ", gra->plansza[i].pionki_c);
    }
    for (int i = 0; i < 2; i++) {
        fprintf(f, "%d ", gra->rzut[i].oczka1);
        fprintf(f, "%d ", gra->rzut[i].oczka2);
    }
    fprintf(f, "%2d %2d %2d ", gra->bialy.banda, gra->bialy.dwor, gra->bialy.wynik);
    fprintf(f, "%2d %2d %2d ", gra->czarny.banda, gra->czarny.dwor, gra->czarny.wynik);
    fprintf(f, "%d ", gra->czyj_ruch);
    fprintf(f, "%d ", gra->reset);
    fprintf(f, "%d\n", gra->zapisuj);

    fclose(f);
}
void plik_wczytaj_ruch(FILE* f, stan_gry* gra, int miejsce) {
    fseek(f, miejsce, SEEK_SET);

    for (int i = 0; i < ROZM_PL; i++) {
        fscanf(f, "%2d", &gra->plansza[i].pionki_b);
        fscanf(f, "%2d", &gra->plansza[i].pionki_c);
    }

    for (int i = 0; i < 2; i++) {
        fscanf(f, "%d", &gra->rzut[i].oczka1);
        fscanf(f, "%d", &gra->rzut[i].oczka2);
    }

    fscanf(f, "%2d %2d %2d", &gra->bialy.banda, &gra->bialy.dwor, &gra->bialy.wynik);
    fscanf(f, "%2d %2d %2d", &gra->czarny.banda, &gra->czarny.dwor, &gra->czarny.wynik);
    fscanf(f, "%d", &gra->czyj_ruch);
    fscanf(f, "%d", &gra->reset);
    fscanf(f, "%d", &gra->zapisuj);


}
int plik_rozmiar(FILE* f) {
    fseek(f, 0, SEEK_END);
    int wynik = ftell(f);
    fseek(f, 0, SEEK_SET);
    return wynik;
}
int plik_dl_rzedu(FILE* f) {
    char c = 'a';
    int dl = 1;
    while (c != '\n') {
        c = fgetc(f);
        dl++;
    }
    fseek(f, 0, SEEK_SET);
    return dl;
}


void inicjalizuj_ranking(zawodnik_t* ranking) {
    for (int i = 0; i < MAX_GRACZY; i++) {
        ranking[i].wynik = 0;
        strcpy(ranking[i].nazwa, "");
    }
}
void uzupelnij_plik_rankingowy(zawodnik_t* ranking,zawodnik_t z1, zawodnik_t z2) {
    int i = 0, count1 = 0, count2 = 0;
    FILE* f = fopen("ranking.txt", "r+");

    while (fscanf(f, "%s %d", &ranking[i].nazwa, &ranking[i].wynik) == 2)
        i++;

    for (int j = 0; j < i; j++)
        if (strcmp(z1.nazwa, ranking[j].nazwa) == 0) {
            ranking[j].wynik += z1.wynik;
            count1++;
        }
        else if (strcmp(z2.nazwa, ranking[j].nazwa) == 0) {
            ranking[j].wynik += z2.wynik;
            count2++;
        }

    if (count1 == 0) {
        ranking[i] = z1;
        i++;
    }
    if (count2 == 0) {
        ranking[i] = z2;
        i++;
    }
    fseek(f, 0, SEEK_SET);

    for (int j = 0; j < i; j++)
        fprintf(f, "%s %3d\n", ranking[j].nazwa, ranking[j].wynik);

    fclose(f);

}
void uzupelnij_ranking(const stan_gry* gra) {
    clrscr();
    gotoxy(1, 1);
    zawodnik_t* ranking = (zawodnik_t*)malloc(MAX_GRACZY * sizeof(zawodnik_t));

    inicjalizuj_ranking(ranking);

    zawodnik_t nowyZaw1, nowyZaw2;
    cputs("Wpisz nazwe gracza bialego i czarnego: ");
    scanf("%s %s", &nowyZaw1.nazwa, &nowyZaw2.nazwa);
    printf("%s %s", nowyZaw1.nazwa, nowyZaw2.nazwa);
    nowyZaw1.wynik = gra->bialy.wynik;
    nowyZaw2.wynik = gra->czarny.wynik;

    uzupelnij_plik_rankingowy(ranking, nowyZaw1, nowyZaw2);
}
int porownaj_wyniki(const void* a, const void* b) {
    const zawodnik_t* zawodnikA = (const zawodnik_t*)a;
    const zawodnik_t* zawodnikB = (const zawodnik_t*)b;
    return zawodnikB->wynik - zawodnikA->wynik;
}
void wypisz_ranking(void) {
    zawodnik_t* ranking = (zawodnik_t*)malloc(MAX_GRACZY * sizeof(zawodnik_t));
    int i = 0;
    FILE* f = fopen("ranking.txt", "r");
    while (fscanf(f, "%s %d", &ranking[i].nazwa, &ranking[i].wynik) == 2)
        i++;
    fclose(f);
    qsort(ranking, i, sizeof(zawodnik_t), porownaj_wyniki);

    for (int j = 0; j < i; j++)
        printf("%s %d\n", ranking[j].nazwa, ranking[j].wynik);
}

void sprawdz_koniec(stan_gry* gra) {
    if (gra->bialy.dwor == 15) {
        if (gra->czarny.banda)
            gra->bialy.wynik += 2;
        if (gra->czarny.dwor == 0)
            gra->bialy.wynik++;
        gra->bialy.wynik++;
        gra->reset = 1;
    }
    if (gra->czarny.dwor == 15) {
        if (gra->bialy.banda)
            gra->czarny.wynik += 2;
        if (gra->bialy.dwor == 0)
            gra->czarny.wynik++;
        gra->czarny.wynik++;
        gra->reset = 1;
    }
    if(gra->reset)
        uzupelnij_ranking(gra);
}
void wizualizuj_przegieg_rozgrywki(stan_gry* gra) {
    inicjalizuj(gra);
    FILE* f = fopen("zapisruchow.txt", "r+");

    if (f == NULL) {
        perror("Error");
        Sleep(200);
        return;
    }
    int dl_rzedu = plik_dl_rzedu(f), c = 0, miejsce = 0;
    int rozmiar = plik_rozmiar(f);

    plik_wczytaj_ruch(f, gra, miejsce);
    wypisz_gre(gra);
    while (c != 'Q') {
        wypisz_legende();
        c = getch();
        if (c == 0) {
            c = getch();
            if (c == STRZ_L)
                (miejsce == 0) ? (miejsce = 0) : (miejsce -= dl_rzedu);

            if (c == STRZ_G)
                miejsce = rozmiar - dl_rzedu - 2;

            if (c == STRZ_D)
                miejsce = 0;

            if (c == STRZ_P)
                miejsce += dl_rzedu;
            plik_wczytaj_ruch(f, gra, miejsce);
            wypisz_gre(gra);
        }
    }

    fclose(f);
}
void zmien_kolej(stan_gry* gra) {
    if (!(gra->rzut[0].oczka1 || gra->rzut[0].oczka2 || gra->rzut[1].oczka1 || gra->rzut[1].oczka2)) {
        gra->czyj_ruch = !gra->czyj_ruch;
        wypisz_czyj_ruch(gra);
        gra_rzut_kostkami(gra);
    }
}
int tura_zdejmij_z_bandy(stan_gry* gra) {
    int pole = 0;
    if (((gra->czyj_ruch == GRACZ_B && gra->bialy.banda) || (gra->czyj_ruch == GRACZ_CZ && gra->czarny.banda))) {
        int mozliwe[2] = { 0, 0 };
        sprawdz_zdjecie_z_bandy(gra, mozliwe);
        if (mozliwe[0]) {
            gotoxy(MENU_X0 + 1, MENU_Y0 + 3);
            clreol();
            cputs("na jakie pole zdjac pionka z bandy: ");
            scanf("%i", &pole);
            printf("%d", pole);
            if (((gra->bialy.banda && pole > 18) || (gra->czarny.banda && pole < 7)) && (mozliwe[0] == pole || mozliwe[1] == pole)) {
                gra_zdejmij_z_bandy(gra, pole);
                return 1;
            }
            else
                wypisz_niepr_ruch(1);
        }
        else
            wyzeruj_ruchy(gra);
        return 1;
    }
    return 0;
}
int legalne_ruchy(const stan_gry* gra) {
    int wynik = 0;
    for (int pole = 0; pole < ROZM_PL; pole++)
        for (int i = 0; i < 2; i++)
            wynik += sprawdz_ruch(gra, pole, gra->rzut[i].oczka1) + sprawdz_ruch(gra, pole, gra->rzut[i].oczka2);
    return wynik;
}
void koniec_tury(stan_gry* gra) {
    if (gra->zapisuj == 1)
        plik_zapisz_ruch(gra);

    zmien_kolej(gra);
    sprawdz_koniec(gra);
}
void tura(stan_gry* gra) {
    wypisz_niepr_ruch();
    wypisz_zbito_pionek();
    int pole = 0, ile = 0, flaga = 1;
    if (!tura_zdejmij_z_bandy(gra)) {
        if (czy_zdejm_z_planszy(gra) == 15) {
            gra_zdejmij_z_planszy(gra);
            flaga = (legalne_ruchy(gra)) ? 1 : 0;
        }

        if (!gra_szukaj_bicia(gra) && flaga) {
            if (legalne_ruchy(gra) != 0) {
                gotoxy(MENU_X0 + 1, MENU_Y0 + 3);
                clreol();
                cputs("z ktorego pola i o ile chcesz sie ruszyc: ");
                scanf("%d %d", &pole, &ile);
                printf("%d %d", pole, ile);
                if (sprawdz_ruch(gra, pole - 1, ile))
                    gra_rusz_pionek(gra, pole - 1, ile);
                else
                    wypisz_niepr_ruch(1);
            }
            else
                wyzeruj_ruchy(gra);
        }
    }

    koniec_tury(gra);
}

void start(stan_gry* gra) {
    wypisz_gre(gra);
    srand((unsigned int)time(NULL));
    rzut_t rzut = { rand() % 6 + 1, rand() % 6 + 1 };
    gotoxy(KOST1);
    putch('0' + rzut.oczka1);
    gotoxy(KOST2);
    putch('0' + rzut.oczka2);
    while (rzut.oczka1 == rzut.oczka2) {
        rzut = { rand() % 6 + 1, rand() % 6 + 1 };
        gotoxy(KOST1);
        putch('0' + rzut.oczka1);
        gotoxy(KOST2);
        putch('0' + rzut.oczka2);
    }
    (rzut.oczka1 > rzut.oczka2) ? (gra->czyj_ruch = GRACZ_B) : (gra->czyj_ruch = GRACZ_CZ);
    wypisz_czyj_ruch(gra);
    Sleep(500);
    gra_rzut_kostkami(gra);
}
void graj(stan_gry* gra) {
    wypisz_gre(gra);
    unsigned char c = '1';
    int temp = 0;
    while (c != '2') {
        if (gra->reset) {
            gra->reset = 0;
            return;
        }
        wypisz_menu();
        gotoxy(MENU_X0 + 1, MENU_Y0 + 2);
        c = getch();
        putch(c);
        if (c == '1') tura(gra);
        if (c == '2') {
            plik_zapisz_stan(gra);
        }
    }
}
void nowa_gra(stan_gry* gra) {
    char ch = 'a';
    inicjalizuj(gra);
    clrscr();
    gotoxy(1, 1);
    cputs("Czy chcesz zapisywac przebieg tej gry?\n[1] Tak\n[2] Nie\n");
    ch = getchar();
    if (ch == '1') {
        FILE* f = fopen("zapisruchow.txt", "w");
        fclose(f);
        gra->zapisuj = 1;
    }
    start(gra);
    graj(gra);
}


void menu_startowe(stan_gry* gra) {
    char c = 'a';
    while (c != 'Q') {
        clrscr();
        gotoxy(1, 1);
        cputs("[1] Rozpocznij gre\n[2] Wczytaj gre z pliku\n[3] Wizualizuj zapisana gre\n[4] Wypisz ranking\n[Q] Zakoncz program\n");
        c = getchar();
        switch (c) {
        case '1':
            nowa_gra(gra);
            break;
        case '2':
            plik_wczytaj_stan(gra);
            wypisz_gre(gra);
            graj(gra);
            break;
        case '3':
            wizualizuj_przegieg_rozgrywki(gra);
            break;
        case '4':
            wypisz_ranking();
            getch();
            break;
        }
    }
}


int main() {
    stan_gry gra;

    menu_startowe(&gra);

    gotoxy(SZER + X0, WYS + Y0 + 6);
    return 0;
}
