#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "megjelenites.h"

/** Átváltja a megadott szöget fokból radiánba */
double szog_radianba(int szog){
	return (double)szog / 360 * 3.14159 * -2;
}

/** SDL inicializálása */
void init(){
	/* Ha nem sikerül, lépjen ki */
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
		SDL_Log("Nem indítható az SDL: %s", SDL_GetError());
		exit(1);
	}
}

/* Kapott pont koordinátáit átváltja az ablak koordinátáiba */
void pont_koordinata_atvaltas(Ablak *ablak, double *x, double *y){
	/* 10-szeres nyújtás, és eltolás annyival, ahol van az y tengely az x-en */
	*x = *x * 10 + (ablak->abra_meret / 2 + ablak->menu_szelesseg);
	/* 10-szeres nyújtás, és eltolás annyival, ahol van az x tengely az y-on */
	*y = ablak->abra_meret / 2 - *y * 10;
}

/** Ablak felállítása méret és fejléc alapján */
SDL_Window *window_letrehoz(int szel, int hossz, char *fejlec){
	SDL_Window *window = SDL_CreateWindow(fejlec, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szel, hossz, 0);
	/* Ha nem sikerült létrehozni, lépjen ki */
	if (window == NULL){
		SDL_Log("Nem hozható létre az ablak: %s", SDL_GetError());
		exit(1);
	}
	return window;
}

/** Megjelenítő létrehozása */
SDL_Renderer *renderer_letrehoz(SDL_Window *window){
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	/* Ha nem siketült létrehozni, lépjen ki */
	if (renderer == NULL){
		SDL_Log("Nem hozható létre a megjelenítő: %s", SDL_GetError());
		exit(1);
	}
	return renderer;
}

/** Háttérszín fehérre állítása */
void hatter_beallit(Ablak *ablak){
	/* Szín beállítása, majd a megjelenítő letörlése a színnel */
	SDL_SetRenderDrawColor(ablak->renderer, 242, 242, 242, 255);
	SDL_RenderClear(ablak->renderer);
	SDL_RenderPresent(ablak->renderer);
}

/** 4 vonal meghúzása: 2 egymáshoz közeli ami elválasztja az ábrát a menütől, a másik 2 pedig az x és y tengely */
void kezdo_vonalak_rajzolas(Ablak *ablak){
	/* Méreteket eltárolom helyben (menu, abra) */
	int menu = ablak->menu_szelesseg, abra = ablak->abra_meret;
	/* Szín */
	SDL_SetRenderDrawColor(ablak->renderer, 0, 0, 0, 255);
	/* Választóvonalak */
	SDL_RenderDrawLine(ablak->renderer, menu, 0, menu, abra);
	SDL_RenderDrawLine(ablak->renderer, menu -2, 0, menu -2, abra);
	/* Tengelyek */
	SDL_RenderDrawLine(ablak->renderer, menu + abra / 2, 0, menu + abra / 2, abra);
	SDL_RenderDrawLine(ablak->renderer, menu, abra / 2, menu + abra, abra / 2);
	/* Kirajzolás */
	SDL_RenderPresent(ablak->renderer);
}

/** A kapott gombon megjeleníti a hozzá tartozó szöveget */
void szoveg_kiir(Ablak *ablak, Gomb *gomb){
	/* ttf beállítása */
	TTF_Init();
	TTF_Font *betutipus = TTF_OpenFont("LiberationSerif-Regular.ttf", 15);
	if (!betutipus){
		SDL_Log("Nem sikerült megnyitni a betűtípust: %s", TTF_GetError());
		exit(1);
	}
	SDL_Color fekete = {0,0,0};
	SDL_Surface *felirat = TTF_RenderUTF8_Solid(betutipus, gomb->szoveg, fekete);
	SDL_Texture *felirat_textura = SDL_CreateTextureFromSurface(ablak->renderer, felirat);
	/* Szövegdoboz méretei, kicsit kisebb mint a gomb */
	SDL_Rect szoveg_doboz;
	szoveg_doboz.x = gomb->meret.x + 2;
	szoveg_doboz.y = gomb->meret.y + 3;
	szoveg_doboz.w = felirat->w;
	szoveg_doboz.h = felirat->h;
	/* Kiírás, felszabadítás, bezárás */
	SDL_RenderCopy(ablak->renderer, felirat_textura, NULL, &szoveg_doboz);
	SDL_FreeSurface(felirat);
	SDL_DestroyTexture(felirat_textura);
	TTF_CloseFont(betutipus);
	SDL_RenderPresent(ablak->renderer);
}

/** Gomb megjelenítése, meghívódik lenyomásnál és felengedésnél, hogy a szín változtatásával legyen egy kis animáció */
void gomb_megjelenit(Ablak *ablak, Gomb *gomb, bool lenyomva){
	/* Ha nincs engedélyezés a gombhoz, térjen vissza */
	if (!gomb->en)
		return;
	/* Megkülönböztetek 2 állapotot: le van nyomva, nincs legyomva
	 * Ha le van nyomva, akkor világosabb lesz a gomb */
	if (lenyomva)
		SDL_SetRenderDrawColor(ablak->renderer, 240, 240, 255, 255);
	else
		SDL_SetRenderDrawColor(ablak->renderer, 210, 210, 255, 255);
	/* Négyzet kitöltése és keret megrajzolása */
	SDL_RenderFillRect(ablak->renderer, &gomb->meret);
	SDL_SetRenderDrawColor(ablak->renderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(ablak->renderer, &gomb->meret);
	/* Ha a gombon levő szöveg nem üres akkor írja ki */
	if (gomb->szoveg[0] != 0){
		szoveg_kiir(ablak, gomb);
	}
	SDL_RenderPresent(ablak->renderer);
}

/** A kapott két pontot átválja az ablak koordinátáiba, mejd összeköti őket */
void szakasz_rajzolas(Ablak *ablak, Alakzat *szakasz){
	/* Adatok helyben tárolása, átváltások */
	double x1 = szakasz->x1, y1 = szakasz->y1, x2 = szakasz->x2, y2 = szakasz->y2;
	pont_koordinata_atvaltas(ablak, &x1, &y1);
	pont_koordinata_atvaltas(ablak, &x2, &y2);
	SDL_SetRenderDrawColor(ablak->renderer, 0, 0, 0, 255);
	/* Két pont összekötése, kirajzolás */
	SDL_RenderDrawLine(ablak->renderer, x1, y1, x2, y2);
	SDL_RenderPresent(ablak->renderer);
}

/** A megadott pont és a szög alapján létrehoz két pontot az ábra jobb és bal szélén, majd szakaszként rajzolja/tárolja az egyenest */
void egyenes_rajzolas(Ablak *ablak, Alakzat *egyenes){
	/* Adatok helyben tárolása, átváltások */
	double x = egyenes->x1, y = egyenes->y1, szog = egyenes->szog;
	int x1, y1, x2, y2; /* Két összekötendő pont koordinátái */
	double pi = 3.14159, tav;
	double szog_rad = szog_radianba(szog);
	pont_koordinata_atvaltas(ablak, &x, &y);
	/* Ha +- 90 az egyenes szöge, akkor az x1 x2 = x, és y = az ábra teteje és alja */
	if (((int)szog + 90) % 180 == 0){
        x1 = x; y1 = 0;
        x2 = x; y2 = ablak->abra_meret;
	}
	/* A megadott pont és az ábra jobb szélén levő pont távolsága */
	else{
        tav = ((ablak->menu_szelesseg + ablak->abra_meret) - x) / cos(szog_rad);
        /* Pont koordinátái szögfüggvényekkel */
        x1 = x + tav * cos(szog_rad); y1 = y + tav * sin(szog_rad);
        /* Bal szélétől a távolság */
        tav = (ablak->menu_szelesseg - x) / cos(szog_rad + pi);
        /* Pont koordinátái szögfüggvényekkel */
        x2 = x + tav * cos(szog_rad + pi); y2 = y + tav * sin(szog_rad + pi);
    }
    /** Nem itt rajzolom ki, mert nem pontos a rajz. Átadom a szakasz_rajzolas()-nak */
	/* A két pont összekötése, kirajzolás */
	/*SDL_SetRenderDrawColor(ablak->renderer, 0, 0, 0, 255);
	SDL_RenderDrawLine(ablak->renderer, x1, y1, x2, y2);
	SDL_RenderPresent(ablak->renderer);*/

	/* A kirajzoláshoz használt két pontot fogom elmenteni az alakzatba, és szakaszként használom, mert az egyenessel sok hiba volt amit nem tudtam javítani (de szakasszal működik) */
    int x_eltolas = ablak->abra_meret/2 + ablak->menu_szelesseg, y_eltolas = ablak->abra_meret/2;
    egyenes->x1 = (x1-x_eltolas)/10; egyenes->y1 = -(y1-y_eltolas)/10;
    egyenes->x2 = (x2-x_eltolas)/10; egyenes->y2 = -(y2-y_eltolas)/10;
    egyenes->tipus = SZAKASZ;

    szakasz_rajzolas(ablak, egyenes);
}

/** Kör kirajzolása. Sugár távolságban helyez el pontokat a középpont körül */
void kor_rajzolas(Ablak *ablak, Alakzat *kor){
	/* Adatok helyben tárolása, átváltások */
    double x = kor->x1, y = kor->y1, r = kor->r;
	double pi = 3.14159;
	r *= 10;
	pont_koordinata_atvaltas(ablak, &x, &y);
	SDL_SetRenderDrawColor(ablak->renderer, 0, 0, 0, 255);
	/* A szög 0-tól 2pi-ig halad 1/10r lépésekben */
	for (double szog = 0; szog < 2 * pi; szog += (double)1/(10*r)){
		/* Körvonal pontjának koordinátái */
		int tempx, tempy;
		tempx = x + r * cos(szog);
		tempy = y + r * sin(szog);
		/* Ha nem esik a menübe a rajzolandó pont, akkor rajzolja ki */
		if (tempx > ablak->menu_szelesseg)
			SDL_RenderDrawPoint(ablak->renderer, tempx, tempy);
	}
	/* Változstatások kirajzolása */
	SDL_RenderPresent(ablak->renderer);
}

/** Metszéspont kirajzolása, a kapott x y koordináták alapján */
void metszespont_rajzolas(Ablak *ablak, double x, double y){
	/* Koordináták átváltása, szín beállítása*/
	pont_koordinata_atvaltas(ablak, &x, &y);
	SDL_SetRenderDrawColor(ablak->renderer, 0, 0, 0, 255);
	/* A pont köré rajzol 2 kört, 1 és 2 pixel hosszú sugárral */
	for (int r = 1; r < 3; r++){
		/* A szög 0-tól 2pi-ig halad 1/10r lépésekben */
		for (double szog = 0; szog < 2 * 3.14159; szog += (double)1/(10*r)){
			/* Körvonal pontjának koordinátái */
			int tempx, tempy;
			tempx = x + r * cos(szog);
			tempy = y + r * sin(szog);
			/* Ha nem esik a menübe a rajzolandó pont, akkor rajzolja ki */
			if (tempx > ablak->menu_szelesseg)
				SDL_RenderDrawPoint(ablak->renderer, tempx, tempy);
		}
	}
	/* Változtatások kirajzolása */
	SDL_RenderPresent(ablak->renderer);
}

/** Ablak adatainak beállítása, megjelenítés */
void ablak_felallit(Ablak *ablak){
	/** Inicialicálás */
	init();
	/** Méretek beállítása */
	ablak->abra_meret = 500; ablak->menu_szelesseg = 200;
	/** Ablak és megjelenítő felállítása */
	ablak->window = window_letrehoz(ablak->abra_meret + ablak->menu_szelesseg, ablak->abra_meret , "Koordinátageometria");
	ablak->renderer = renderer_letrehoz(ablak->window);
	/** Háttér, kezdővonalak felállítása */
	hatter_beallit(ablak);
	kezdo_vonalak_rajzolas(ablak);
	/** Kirajzolás */
	SDL_RenderPresent(ablak->renderer);
}
