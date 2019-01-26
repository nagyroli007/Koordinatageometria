#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <stdbool.h>
#include <string.h>

#include "megjelenites.h"
#include "szerkesztesek.h"
#include "vezerlo.h"

void gombok_adatai(Gomb *gombok);
void melyik_alakzat(Alakzat_tipus *alakzat, int lenyomott_gomb);
bool alakzat_gomb(int lenyomott_gomb);

int main(int argc, char *argv[]){

	/* Ablak, gombok létrehozás, felállítás */
	Ablak ablak;
	Gomb gombok[13];
	gombok_adatai(gombok);
	ablak_felallit(&ablak);
	for (int i = 0; i < 13; i++)
		gomb_megjelenit(&ablak, &gombok[i], false);
	/* Segédváltozók */
	bool gepelhet = false;
	bool quit = false;
	int lenyomott_gomb = -1;
	char *bevitel = "";
	bool var_kattintast = false;
	Alakzat_tipus kattintott_tipus = SEMILYEN;
	/* Alakzat és metszéspont lista elejei */
	Alakzat *eleje = NULL;
	Metszespont *pontok = NULL;
	/* Amig nem kell kilépni, itt kezelem az eseményeket */
	while (!quit){
		/* Eseményre várakozás és elmentése */
		SDL_Event esemeny = varakozas_esemenyre();
		/* Esemény vizsgálat */
		switch (esemeny.type){
			/* Kattintás */
			case SDL_MOUSEBUTTONDOWN:
				/* Ha inputos gombra kattintott előzőleg, akkor szünjön meg az input bevitele */
				if (gepelhet){
					/* Gomb színe legyen alapértelemezett */
					gomb_megjelenit(&ablak, &gombok[lenyomott_gomb], false);
					gepelhet = false;
				}
				/* Kattintás kezelése a vezérlőben */
				eger_lenyomva(&ablak, gombok, esemeny.button.x, esemeny.button.y, &lenyomott_gomb, &eleje, &pontok, &var_kattintast);
				/* Ha alakzat gombjára kattintottunk, mentse el milyen alakzat kell éppen */
				if (alakzat_gomb(lenyomott_gomb))
					melyik_alakzat(&kattintott_tipus, lenyomott_gomb);
				/* Ha a rajzra kattintottak */
				if (lenyomott_gomb == 12)
					alakzat_rajzolas(&ablak, gombok, kattintott_tipus, &eleje);
				break;
			/* Egér felengedésnél */
			case SDL_MOUSEBUTTONUP:
				/* Ha volt lenyomott gomb */
				if (lenyomott_gomb != -1){
					/* Ha inputos gombra kattintott, akkor lehet gépelni */
					if (kell_input(lenyomott_gomb))
						gepelhet = true;				
					/* Ha másmilyen gombra, akkor állítsa vissza a színét, felejtse el a legyomott gombot */
					else{
						gomb_megjelenit(&ablak, &gombok[lenyomott_gomb], false);
						lenyomott_gomb = -1;
					}
				}
				break;
			/* Billenytű lenyomásánál, csak a backspace-t vizsgálom itt */	 
			case SDL_KEYDOWN:	
				/* Ha lehet gépelni */
				if (gepelhet){
					/* Ha törölni kell, mert backspace lett leütve */
					if (esemeny.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){	
						/* Töröl belőle, és újra megjeleníti, hogy látszódjon a változás */
						karakter_torles(gombok[lenyomott_gomb].szoveg);
						gomb_megjelenit(&ablak, &gombok[lenyomott_gomb], false);
					}
				}
				break;
			/* Szöveg bevitelnél */
			case SDL_TEXTINPUT:
				/* Ha lehet gépelni, akkor adja hozzá a gombhoz a bevitt karaktert */
				if (gepelhet)
					gepeles(&ablak, &gombok[lenyomott_gomb], esemeny.text.text);
				break;
			/* Ha kilépés gomb meg lett nyomva, akkor a kilépés legyen igaz */
			case SDL_QUIT:
				quit = true;
				break;
		}
	}
	/* Kilépés az SDL-ből */
	SDL_Quit();
	/* Listák felszabadítása */
	lista_felszabadit(&eleje);
	metszespont_felszabadit(&pontok);

	return 0;
}

/** Gombok adatainak beállítása */
void gombok_adatai(Gomb *gombok){
        gomb_adat(&gombok[0], 15, 15, 100, 24, "", true);
		gomb_adat(&gombok[1], 15, 45, 60, 24, "Mentés", true);
		gomb_adat(&gombok[2], 80, 45, 60, 24, "Betöltés", true);
        gomb_adat(&gombok[3], 15, 90, 100, 24, "Metszéspontok", true);
        gomb_adat(&gombok[4], 15, 120, 100, 24, "Merőleges", true);
        gomb_adat(&gombok[5], 15, 160, 55, 24, "Szakasz", true);
        gomb_adat(&gombok[6], 75, 160, 55, 24, "Egyenes", true);
        gomb_adat(&gombok[7], 135, 160, 35, 24, "Kör", true);
        gomb_adat(&gombok[8], 43, 190, 30, 24, "", false);
        gomb_adat(&gombok[9], 110, 190, 30, 24, "", false);
        gomb_adat(&gombok[10], 43, 220, 30, 24, "", false);
        gomb_adat(&gombok[11], 110, 220, 30, 24, "", false);
        gomb_adat(&gombok[12], 15, 250, 50, 24, "Rajz", false);
}

/** A lenyomott gomb alapján visszatér, hogy melyik alakzat gombja az */
void melyik_alakzat(Alakzat_tipus *tipus, int lenyomott_gomb){
	/* Az 5, 6, 7 indexű gombok az alakzatok. Kivonok az indexből 4-et, és kijön az 1-szakasz, 2-egyenes, 3-kör */
	if (lenyomott_gomb < 5 || lenyomott_gomb > 7){
		*tipus = SEMILYEN;
		return;
	}
	*tipus = lenyomott_gomb - 4;
}

/** Alakzat-e a lenyomott gomb (kör, szakasz vagy egyenes gomb lett lenyomva) */
bool alakzat_gomb(int lenyomott_gomb){
	return lenyomott_gomb >= 5 && lenyomott_gomb <= 7;
}
