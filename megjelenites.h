#ifndef MEGJELENITES_H
#define MEGJELENITES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct Ablak {
	SDL_Window *window;
	SDL_Renderer *renderer;
	int abra_meret, menu_szelesseg;
} Ablak;

typedef struct Gomb {
	SDL_Rect meret;
	char szoveg[20];
	bool en;
} Gomb;

typedef enum Alakzat_tipus { SEMILYEN, SZAKASZ, EGYENES, KOR } Alakzat_tipus;

typedef struct Alakzat {
	Alakzat_tipus tipus;
	int x1, y1, x2, y2;
	int r;
	int szog;
	struct Alakzat *kov;
} Alakzat;

typedef struct Metszespont {
	double x, y;
	struct Metszespont *kov;
} Metszespont;

double szog_radianba(int szog);
void init();
void pont_koordinata_atvaltas(Ablak *ablak, double *x, double *y);
SDL_Window *window_letrehoz(int szel, int hossz, char *fejlec);
SDL_Renderer *renderer_letrehoz(SDL_Window *window);
void hatter_beallit(Ablak *ablak);
void kezdo_vonalak_rajzolas(Ablak *ablak);
void szoveg_kiir(Ablak *ablak, Gomb *gomb);
void gomb_megjelenit(Ablak *ablak, Gomb *gomb, bool lenyomva);
void szakasz_rajzolas(Ablak *ablak, Alakzat *szakasz);
void egyenes_rajzolas(Ablak *ablak, Alakzat *egyenes);
void kor_rajzolas(Ablak *ablak, Alakzat *kor);
void metszespont_rajzolas(Ablak *ablak, double x, double y);
void ablak_felallit(Ablak *ablak);

#endif
