#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "megjelenites.h"
#include "vezerlo.h"
#include "szerkesztesek.h"
#include "filekezeles.h"

/** Várakozás, majd visszatérés az esemény-el */
SDL_Event varakozas_esemenyre(){
	SDL_Event esemeny;
	SDL_WaitEvent(&esemeny);
	return esemeny;
}

/** szöveget egésszé vált, a szövegben csak számok lehetnek, és egy negatív jel a legelején */
int string_to_int(char *szoveg){
	/* Negatív szorzó */
	int negativ = 1;
	/* Ha üres a string, legyen 0 a szám */
	if (szoveg[0] == 0)
		return 0;
	/* Szám, index */
	int szam = 0, i = 0;
	/* Ha az első karakter -, akkor a negatív szorzó legyen -1 */
	if (szoveg[0] == '-'){
		negativ = -1;
		i = 1;
	}
	/* Karaktereken végigfutás */
	for (i; i < strlen(szoveg); i++){
		/* Ha nem szám a karakter, akkor azzal ne foglalkozzon */
		if (szoveg[i] < 48 || szoveg[i] > 57)
			SDL_Log("Hibás karakter!");
		/* Új számnál beszorzom 10-el az eredményt, és hozzáadom az új számot */
		szam *= 10;
		szam += szoveg[i] - 48;
	}
	/* Szorzom a negatív szorzóval (ha az elején - van akkor -1-el szoroz, ellenben 1-el szoroz */
	return szam * negativ;
}

/** Gomb struktúrába betölti a megadott adatokat */
void gomb_adat(Gomb *gomb, int x, int y, int w, int h, char *szoveg, bool en){
        gomb->meret.x = x; gomb->meret.y = y; gomb->meret.w = w; gomb->meret.h = h; strcpy(gomb->szoveg, szoveg); gomb->en = en;
}

/* A megadott x y koordinátákból megállapítja, hogy melyik gombon van az (x, y) pont */
int melyik_gomb(Gomb *gombok, int x, int y){
	/* Végig megy a gombokon */
	for (int i = 0; i < 13; i++){
		/* Megnézi, hogy része e az (x, y) a gombnak */
		if (gombok[i].en && x >= gombok[i].meret.x && x <= gombok[i].meret.x + gombok[i].meret.w && y >= gombok[i].meret.y && y <= gombok[i].meret.y + gombok[i].meret.h){
			return i;
		}
	}
	return -1;
}

/** Az alazak gombok alatti 4 gomb engedélyezése, és kirajzolása, kivéve azt az 1-et, ami nem kell. Ha mind a 4 kell, akkor olyan indexűt adok meg, ami nem >= 8 & < 13 */
void gombok_8_13_engedelyezes_megjelenites(Ablak *ablak, Gomb *gombok, int melyiket_ne){
	for (int i = 8; i < 13; i++)
		if (i != melyiket_ne){
			gombok[i].en = true;
			gomb_megjelenit(ablak, &gombok[i], false);
		}
}

/** Négy szövegdoboz beállítása, megadott sztringek belepakolása */
void negy_szovegdoboz(Gomb *dobozok, char *a, char *b, char *c, char *d){
	gomb_adat(&dobozok[0], 15, 190, 15, 24, a, true);
	gomb_adat(&dobozok[1], 85, 190, 15, 24, b, true);
	gomb_adat(&dobozok[2], 15, 220, 15, 24, c, true);
	gomb_adat(&dobozok[3], 85, 220, 15, 24, d, true);
}

/** Az alakzatok beviteléhez szükséges gombok és szövegek törlése */
void elokeszites_torles(Ablak *ablak, Gomb *gombok){
	/* Egy háttér színű négyzettel kitakarom a nem kellő elemeket */
	SDL_Rect torolni_valo;
	torolni_valo.x = 15;
	torolni_valo.y = 190;
	torolni_valo.w = 150;
	torolni_valo.h = 120;
	SDL_SetRenderDrawColor(ablak->renderer, 242, 242, 242, 255);
	SDL_RenderFillRect(ablak->renderer, &torolni_valo);
	/* Gombok engedélyezése hamisra állítása, és a szöveg kitörlése belőlük */
	for (int i = 8; i < 12; i++){
		gombok[i].en = false;
		gombok[i].szoveg[0] = 0;
	}
	gombok[12].en = false;
}

/** Egy szakasz megadásához szükséges gombok, szövegek felállítása */
void szakasz_elokeszit(Ablak *ablak, Gomb *gombok){
	/* Terület törlése */
	elokeszites_torles(ablak, gombok);
	/* Gombok engedélyezése legyen igaz, mejelenítés
	 * A szakasz megadásához mind a 4 gombra szükség van (egy olyan nem kell, ami nincs köztük, pl 0) */
	gombok_8_13_engedelyezes_megjelenites(ablak, gombok, 0);
	/* Szövegdobozok létrehozása, megjelenítés */
	Gomb dobozok[4];
	negy_szovegdoboz(dobozok, "x1:", "y1:", "x2:", "y2");
	for (int i = 0; i < 4; i++)
		szoveg_kiir(ablak, &dobozok[i]);
}

/** Egy egyenes megadásához szükséges gombok, szövegek felállítása */
void egyenes_elokeszit(Ablak *ablak, Gomb *gombok){
	/* Terület törlése */
	elokeszites_torles(ablak, gombok);
	/* Gombok engedélyezése legyen igaz, megjelenítés
	 * Az egyenes megadásához a 10-es indexű gombra nincs szükség */
	gombok_8_13_engedelyezes_megjelenites(ablak, gombok, 10);
	/* Szövegdobozok létrehozása, megjelenítés */
	Gomb dobozok[4];
	negy_szovegdoboz(dobozok, "x:", "y:", "mered. szög:", "");
	for (int i = 0; i < 3; i++)
		szoveg_kiir(ablak, &dobozok[i]);
}

/** Egy kör megadásához szükséges gombok, szövegek felállítása */
void kor_elokeszit(Ablak *ablak, Gomb *gombok){
	/* Terület törlése */
	elokeszites_torles(ablak, gombok);
	/* Gombok engedélyezése legye igaz, megjelenítés */
	/* A kör megadásához a 11-es indexű gombra nincs szükség */
	gombok_8_13_engedelyezes_megjelenites(ablak, gombok, 11);
	/* Szövegdobozok létrehozása, megjelenítés */
	Gomb dobozok[4];
	negy_szovegdoboz(dobozok, "x:", "y:", "r:", "");
	for (int i = 0; i < 3; i++)
		szoveg_kiir(ablak, &dobozok[i]);
}

/** A megkapott gomb index-e alapján megállípítja, hogy van e ott input lehetőség
 * A 0. gomb a filnév, a 8-11 gombok az alakzatok adatának beviteli helye
 * Tehát az ezeken kívüli gombokhoz nem kell input */
bool kell_input(int lenyomott_gomb){
	if ((lenyomott_gomb > 0 && lenyomott_gomb < 8) || lenyomott_gomb == 12)
		return false;
	return true;
}

/** Hozzáadja a gomb szövegéhez a lenyomott karaktert, és újra kirajzolja a gombot */
void gepeles(Ablak *ablak, Gomb *gomb, char *bevitel){
	strcat(gomb->szoveg, bevitel);
	gomb_megjelenit(ablak, gomb, true);
}

/** Kirötli az utolsó kerektert a kapott stringből */
void karakter_torles(char *szoveg){
	int meret = strlen(szoveg);
	if (meret > 0)
		szoveg[meret-1] = 0;
}

/** Megmodja, hogy a megadott alakzat típus szerint kirajzolható */
bool kell_e_rajzolni(Alakzat_tipus alakzat){
	/* Ha nulla -> semilyen alakzat, nem kell kirajzolni */
	if (alakzat == 0)
		return false;
	return true;
}

/** A kapott adatok alapján létrehoz egy új alakzatot, és elmenti azt a lista elejére */
void alakzat_listaba(Alakzat_tipus tipus, int x1, int y1, int x2, int y2, int r, int szog, Alakzat **eleje){
	/* Hely lefoglalása az új elemnek */
	Alakzat *uj = (Alakzat*) malloc(sizeof(Alakzat));
	/* Adatok eltárolása */
	uj->x1 = x1; uj->y1 = y1; uj->x2 = x2; uj->y2 = y2; uj->r = r; uj->szog = szog; uj->tipus = tipus;
	/* Az új elem az elejére kerül, az eddigi eleje pedig az új elem után jön */
	uj->kov = *eleje;
	*eleje = uj;
}

/** A kapott alakzatot elmenti a listába, majt meghívja, a kirajzoló függvényt hozzá */
void alakzat_rajzolas(Ablak *ablak, Gomb *gombok, Alakzat_tipus alakzat, Alakzat **eleje){
	/* Beolvasom a megfelelő gombokról az adatokat, mentem a listába és kirajzolom */
	switch (alakzat){
		case SZAKASZ:
			alakzat_listaba(SZAKASZ, string_to_int(gombok[8].szoveg), string_to_int(gombok[9].szoveg), string_to_int(gombok[10].szoveg), string_to_int(gombok[11].szoveg), 0, 0, eleje);
			szakasz_rajzolas(ablak, *eleje);
			break;
		case EGYENES:
			alakzat_listaba(EGYENES, string_to_int(gombok[8].szoveg), string_to_int(gombok[9].szoveg), 0, 0, 0, string_to_int(gombok[11].szoveg), eleje);
			egyenes_rajzolas(ablak, *eleje);
            break;
		case KOR:
			alakzat_listaba(KOR, string_to_int(gombok[8].szoveg), string_to_int(gombok[9].szoveg), 0, 0, string_to_int(gombok[10].szoveg), 0, eleje);
			kor_rajzolas(ablak, *eleje);
            break;
		default : break;
	}
}

/** Az alakzat lista felszabadítása */
void lista_felszabadit(Alakzat **eleje){
		/** Végigfut az összes elemen. Elenti a következőt, meghívja a freet, majd tovább megy az elmentett következővel */
Alakzat *mozgo = *eleje;
	while (mozgo != NULL){
		Alakzat *uj = mozgo->kov;
		free(mozgo);
		mozgo = uj;
	}
}

/* kiröli a ".txt"-t a filenév végéről, miután már betöltöttük a file-t*/
void kiterjesztes_torlese(char *szoveg){
    int meret = strlen(szoveg);
    szoveg[meret-4] = 0;
}

/** Felszabadítja a metszéspontokat tároló láncolt listát */
void metszespont_felszabadit(Metszespont **eleje){
	/** Végigfut az összes elemen. Elenti a következőt, meghívja a freet, majd tovább megy az elmentett következővel */
	Metszespont *mozgo = *eleje;
	while (mozgo != NULL){
		Metszespont *kov = mozgo->kov;
		free(mozgo);
		mozgo = kov;
	}
}

/** El lett-e mát mentve a metszéspont a listába */
bool metszespont_elmentve(Metszespont *eleje, double x, double y){
    Metszespont *mozgo = eleje;
    while (mozgo != NULL){
        /* Ha a mozgó koordinátái megeggyeznek a megadott új pont koorinátáival, akkor el lett már mentve (pixel eggyezést vizsgálok) */
        if (fabs(mozgo->x - x) <= 0.1 && fabs(mozgo->y - y) <= 0.1)
            return true;
        mozgo = mozgo->kov;
    }
    return false;
}

/** Elment egy új metszéspontot a láncolt lista elejére */
void metszespont_mentes(Metszespont **eleje, double x, double y){
	/* Új elem lefoglalása */
	Metszespont *uj = (Metszespont*) malloc(sizeof(Metszespont));
	/* Adatok eltárolása */
	uj->x = x; uj->y = y;
	/* Az új elem a lista elejére kerül, az eddigi első elem pedig az új után jön. Csak akkor fűzi hozzá, ha nem lett már elmentve */
	if (!metszespont_elmentve(*eleje, x, y)){
        uj->kov = *eleje;
        *eleje = uj;
	}
}

/** Összehasonlítja az összes lehetséges alakzat párt, hogy van e metszéspont */
void metszespontok_megkeresese(Ablak *ablak, Alakzat *eleje, Metszespont **pontok){
	/* Kettő mozgó alakzat, az 1-es az elsőtől az utolsó előttig megy végig, a 2-es pedig az összes 1-es utánin megy végig */
	Alakzat *mozgo1, *mozgo2;
	for (mozgo1 = eleje; mozgo1->kov != NULL; mozgo1 = mozgo1->kov){
		for (mozgo2 = mozgo1->kov; mozgo2 != NULL; mozgo2 = mozgo2->kov){
			/* ha mindkettő kör */
			if (mozgo1->tipus == KOR && mozgo2->tipus == KOR)
				metszespont_kor_kor(ablak, mozgo1, mozgo2, pontok);
			/* Ha az első kör */
			else if (mozgo1->tipus == KOR)
				metszespont_kor_egyenes(ablak, mozgo1, mozgo2, pontok);
			/* Ha a második kör */
			else if (mozgo2->tipus == KOR)
				metszespont_kor_egyenes(ablak, mozgo2, mozgo1, pontok);
			/* Ha nincs kör a kettő között */
			else
				metszespont_egyenes_egyenes(ablak, mozgo1, mozgo2, pontok);
		}
	}
}

/** Kattintás kezelése, megkeresi a lenyomott gombot, meghívja a gombokhoz tartozó funkciókat */
void eger_lenyomva(Ablak *ablak, Gomb *gombok, int x, int y, int *lenyomott_gomb, Alakzat **eleje, Metszespont **pontok, bool *varakozas_kattintasra){
	/* Megkeresi a lenyomott gombot */
	*lenyomott_gomb = melyik_gomb(gombok, x, y);
	/* Ha talált gombot, akkor jelenítse meg (kattintás animáció miatt) */
	if (*lenyomott_gomb != -1)
		gomb_megjelenit(ablak, &gombok[*lenyomott_gomb], true);
	/* Ha várakoztunk kattintásra, akkor merőlegest kell állítani.
	 * Előbb megkeresem azt az alakzatot amire kattintott a felhasználó */
	if (*varakozas_kattintasra){
		alakzat_keres(ablak, *eleje, (x-450) / 10, -(y-250)/10);
	}
	*varakozas_kattintasra = false;
	/* Az inputot nem itt kezelem */
	if (kell_input(*lenyomott_gomb))
		return;
	/* A lenyomott gomb alapján függvények meghívása */
	switch (*lenyomott_gomb){
		/* Metnés */
		case 1:
			mentes(gombok[0].szoveg, *eleje, *pontok);
			break;
		/* Betöltés */
		case 2:
			/* Visszaállítás alaphelyzetbe */
			hatter_beallit(ablak);
			kezdo_vonalak_rajzolas(ablak);
			elokeszites_torles(ablak, gombok);
			/* Gombok kirajzolása */
			for (int i = 0; i <  13; i++)
				gomb_megjelenit(ablak, &gombok[i], false);
			/* Alakzatok betöltése, kirajzolása */
			betoltes(ablak, gombok[0].szoveg, eleje, pontok);
			break;
		/* Metszéspontok */
		case 3:
			metszespontok_megkeresese(ablak, *eleje, pontok);
			break;
		/* Merőleges */
		case 4:
			/* Elkezdünk várakozni kattintásra */
			*varakozas_kattintasra = true;
			break;
		/* Szakasz */
		case 5:
			szakasz_elokeszit(ablak, gombok);
			break;
		/* Egyenes */
		case 6:
			egyenes_elokeszit(ablak, gombok);
			break;
		/* Kör */
		case 7:
			kor_elokeszit(ablak, gombok);
			break;
	}
}
