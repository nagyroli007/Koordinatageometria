#ifndef VEZERLO_H
#define VEZERLO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>

#include "megjelenites.h"
#include "szerkesztesek.h"
#include "filekezeles.h"

bool metszespont_elmentve(Metszespont *eleje, double x, double y);

void alakzat_listaba(Alakzat_tipus tipus, int x1, int y1, int x2, int y2, int r, int szog, Alakzat **eleje);
void metszespont_mentes(Metszespont **eleje, double x, double y);
void metszespont_felszabadit(Metszespont **eleje);
void metszespontok_megkeresese(Ablak *ablak, Alakzat *eleje, Metszespont **pontok);
void eger_lenyomva(Ablak *ablak, Gomb *gombok,int x, int y, int *lenyomott_gomb, Alakzat **eleje, Metszespont **pontok, bool *varakozas_kattintasra);
SDL_Event varakozas_esemenyre();
int string_to_int(char *szoveg);
void gomb_adat(Gomb *gomb, int x, int y, int w, int h, char *szoveg, bool en);
int melyik_gomb(Gomb *gombok, int x, int y);
void gombok_8_13_engedelyezes_megjelenites(Ablak *ablak, Gomb *gombok, int melyiket_ne);
void negy_szovegdoboz(Gomb *dobozok, char *a, char *b, char *c, char *d);
void szakasz_elokeszit(Ablak *ablak, Gomb *gomb);
void egyenes_elokeszit(Ablak *ablak, Gomb *gomb);
void kor_elokeszit(Ablak *ablak, Gomb *gomb);
void elokeszites_torles(Ablak *ablak, Gomb *gombok);
bool kell_input(int lenyomott_gomb);
void gepeles(Ablak *ablak, Gomb *gomb, char *bevitel);
void karakter_torles(char *szoveg);
bool kell_e_rajzolni(Alakzat_tipus alakzat);
void alakzat_rajzolas(Ablak *ablak, Gomb *gombok, Alakzat_tipus alakzat, Alakzat **eleje);
void lista_felszabadit(Alakzat **eleje);
void kiterjesztes_torlese(char *szoveg);

#endif
