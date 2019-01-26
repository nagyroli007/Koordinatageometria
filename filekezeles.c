#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "filekezeles.h"
#include "megjelenites.h"
#include "vezerlo.h"

/** Elmenti a megadott file-ba az alakzatokat és a metszéspontokat */
void mentes(char *file_nev, Alakzat *eleje, Metszespont *pontok){
	/* Ha üres a file név, visszatér */
	if (file_nev[0] == 0){
		SDL_Log("Hibás file név");
		return;
	}
	/* Betöltöm a file-t úgy, hogy a megadott névhez hozzáadok egy .txt kiterjeszést
	 * Ha betöltötte, egyből eltüntetem a kiterjesztést a stringből */
	FILE *f;
	f = fopen(strcat(file_nev, ".txt"), "w");
	kiterjesztes_torlese(file_nev);
	/* Verzió kiírádsa */
	fprintf(f, "koordinatageometria_v0.1\n");
	/* Végigfutok mindkét listán, és kiírom az adatokat a file-ba */
	Alakzat *mozgo;
	for (mozgo = eleje; mozgo != NULL; mozgo = mozgo->kov)
		fprintf(f, "%d %d %d %d %d %d %d\n", mozgo->tipus, mozgo->x1, mozgo->y1, mozgo->x2, mozgo->y2, mozgo->r, mozgo->szog);

	fprintf(f, "alakzatok vége\n");
	Metszespont *pmozgo;
	for (pmozgo = pontok; pmozgo != NULL; pmozgo = pmozgo->kov)
		fprintf(f, "%lf %lf\n", pmozgo->x, pmozgo->y);
	/* File bezárása */
	fclose(f);
}

/** Betölti a megadott file-ból az alakzatokat és a metszéspontokat  */
void betoltes(Ablak *ablak, char *file_nev, Alakzat **eleje, Metszespont **pontok){
	/* Ha üres a filenév, visszatér */
	if (file_nev[0] == 0){
		SDL_Log("Hibás file név");
		return;
	}
	/* Betöltöm a file-t úgy, hogy a megadott névhez hozzáadok egy .txt kiterjesztést
	 * Ha betöltötte, egyből eltüntetem a kiterjesztést a stringből */
	FILE *f;
	f = fopen(strcat(file_nev, ".txt"), "r+");
	kiterjesztes_torlese(file_nev);
	/* Ha nem létezik a megadott file, visszatér */
	if (f == NULL){
		SDL_Log("Nem létezik a megadott file");
		return;
	}
	/* létrehozok egy stringet, beolvasom az első sort (ami a program verziót tartalmazza) */
	char c[50];
	fscanf(f, "%s", &c);
	/* Ideiglenes változók, ebbe olvasom az adatokat */
	int tipus, x1, y1, x2, y2, r, szog;
	double px, py;

	/* Új listák lértehozása, elemek belepakolása
	 * Az alakzat beolvasó ciklus addig fut, ameddig sikeresen be tud olvasni a következő sorból
	 * Az alakzatok végét egy olyan sor jelzi, ami egy nem számokat tartalmaz, hanem szoveget */
	Alakzat *uj_eleje = NULL;
	while (fscanf(f, "%d %d %d %d %d %d %d\n", &tipus, &x1, &y1, &x2, &y2, &r, &szog) > 0)
		alakzat_listaba(tipus, x1, y1, x2, y2, r, szog, &uj_eleje);
	/* Egy sort végigolvasok, mert ebben a sorban csak az van, hogy alakzatok vége */
   	char a = 'a';
   	while(fscanf(f, "%c", &a) > 0 && a != '\n');
	Metszespont *uj_pontok = NULL;
	while (fscanf(f, "%lf %lf", &px, &py) > 0){
		metszespont_mentes(&uj_pontok, px, py);
    }

	/* Végigjutás az új alakzatokon, hogy kirajzoljam őket */
	Alakzat *mozgo = uj_eleje;
	while (mozgo != NULL){
		switch (mozgo->tipus){
			case (1):
				szakasz_rajzolas(ablak, mozgo);
				break;
			case (2):
				egyenes_rajzolas(ablak, mozgo);
				break;
			case (3):
				kor_rajzolas(ablak, mozgo);
				break;
			default:
				break;
		}
		mozgo = mozgo->kov;
	}
	/* Végigfutok az új pontokon, és megjelenítem őket */
	Metszespont *pmozgo = uj_pontok;
	while (pmozgo != NULL){
		metszespont_rajzolas(ablak, pmozgo->x, pmozgo->y);
		pmozgo = pmozgo->kov;
	}
	/* Régi listák felszabadítása, új listák eltárolása a régiben, file bezárás */
	lista_felszabadit(eleje);
	*eleje = uj_eleje;
	metszespont_felszabadit(pontok);
	*pontok = uj_pontok;
	fclose(f);
}
