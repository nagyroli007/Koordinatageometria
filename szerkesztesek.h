#ifndef SZERKESZTESEK_H
#define SZERKESZTESEK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>

#include "megjelenites.h"
#include "vezerlo.h"

int meroleges_szoge(int x1, int y1, int x2, int y2);
void meroleges_allitas(Ablak *ablak, Alakzat *alakzat, int x, int y);
void alakzat_keres(Ablak *ablak, Alakzat *eleje, int x, int y);
void metszespont_kor_kor(Ablak *ablak, Alakzat *kor1, Alakzat *kor2, Metszespont **pontok);
bool szakasz_eleme(Alakzat *szakasz, double x, double y);
void egyenesbol_ket_pont(Alakzat *egyenes, double *p1x, double *p1y, double *p2x, double *p2y);
void metszespont_kor_egyenes(Ablak *ablak, Alakzat *kor, Alakzat *egyenes, Metszespont **pontok);
void metszespont_egyenes_egyenes(Ablak *ablak, Alakzat *e1, Alakzat *e2, Metszespont **pontok);

#endif
