#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>

#include "megjelenites.h"
#include "vezerlo.h"
#include "szerkesztesek.h"

/** Két pontból megállapítja az összekötő szakaszra merőleges egyenes meredekségét */
int meroleges_szoge(int x1, int y1, int x2, int y2){
	/* Kiszámolom a szakasz meredekségét, átváltom szögbe, hozzáadok 90°-ot */
	return (int)(atan((double)(y2 - y1) / (x2 - x1)) * 180 / 3.14159 + 90);
}

/** A kapott pont és alakzat alapján meghívja a szakasz rajzolást, úgy hogy az merőleges legyen */
void meroleges_allitas(Ablak *ablak, Alakzat *alakzat, int x, int y){
	/* Körre nem állítok merőlegest */
	if (alakzat->tipus % KOR == 0)
		return;
	else {
		/* Merőleges szögének beállítása */
		int szog;
		if (alakzat->tipus == SZAKASZ)
			szog = meroleges_szoge(alakzat->x1, alakzat->y1, alakzat->x2, alakzat->y2);
		else
			 szog = alakzat->szog + 90;
		/* A merőleges egy egyenes, elmentem alakzatként, majd meghívom rá az egyenes rajzolást */
		Alakzat *uj = (Alakzat*)malloc(sizeof(Alakzat));
		uj->tipus = EGYENES; uj->x1 = x; uj->y1 = y; uj->szog = szog;
		egyenes_rajzolas(ablak, uj);
		free(uj);
	}
}

/** Megkeresi melyik alakzatra kattintottunk, majd abban a pontban merőlegest állít az egyenesre/szakaszra */
void alakzat_keres(Ablak *ablak, Alakzat *eleje, int x, int y){
	/* Végigmegyek az alakzatokot */
	Alakzat *mozgo;
	for (mozgo = eleje; mozgo != NULL; mozgo = mozgo->kov){
		/* Ha szakasz, akkor megnézi, hogy beleesik a pont a szakasz intervallumába, majd a szakasz merőlegesét összeveti az egyik végpont és a kattintás helyét összekötő szakasz merőlegesével */
		if (mozgo->tipus == SZAKASZ){
			/* x és y legyen a két végpont minimuma és maximuma között */
			if ((mozgo->x1 < mozgo->x2) ? (x >= mozgo->x1 && x <= mozgo->x2) : (x >= mozgo->x2 && x <= mozgo->x1) && (mozgo->y1 < mozgo->y2) ? (y >= mozgo->y1 && y <= mozgo->y2) : (y >= mozgo->y2 && y <= mozgo->y1)){
			/* Ha elég kicsi a meredekség különbség -> közel van a pont a szakaszhoz */
			if (abs(meroleges_szoge(mozgo->x1, mozgo->y1, mozgo->x2, mozgo->y2) - meroleges_szoge(mozgo->x1, mozgo->y1, x, y)) % 180 <= 5){
					/* Merőleges állítása, leáll a ciklus, hogy csak egy alakzatra állítson */
					meroleges_allitas(ablak, mozgo, x, y);
					break;
				}
			}
		}
		/* Ha egyenes, akkor csak a merőlegest kiszámítja, és az egyenes szögével összeveti */
		else if (mozgo->tipus == 2)
            SDL_Log("%d", abs(meroleges_szoge(mozgo->x1, mozgo->y1, x, y) - 90));
            /* Meredekség különbség legyen elég kicsi */
            if (abs(meroleges_szoge(mozgo->x1, mozgo->y1, x, y) - 90 - mozgo->szog) % 180 <= 5){
                meroleges_allitas(ablak, mozgo, x, y);
                break;
            }
            else if (abs(meroleges_szoge(mozgo->x1, mozgo->y1, x, y) + 90 - mozgo->szog) % 180 <= 5){
                meroleges_allitas(ablak, mozgo, x, y);
                break;
            }
	}
}

/** Leellenőrzi a két metszéspontot, hogy rajta van e a két körön, ha nincs, akkor végigmegy a egyik körvonalon, és megnézi hol van sugár távolságban a másik körtől (átállítja a metszéspontokat) */
void metszespont_kor_kor_ellenorzes_javitas(Alakzat *kor1, Alakzat *kor2, double *x1, double *y1, double *x2, double *y2){
    /* Ha a metszéspontok a körökre esnek */
    if (abs(kor1->r - sqrt(pow(kor1->x1 - *x1, 2) + pow(kor1->y1 - *y1, 2))) <= 0.01 &&
        abs(kor2->r - sqrt(pow(kor2->x1 - *x1, 2) + pow(kor2->y1 - *y1, 2))) <= 0.01 &&
        abs(kor1->r - sqrt(pow(kor1->x1 - *x2, 2) + pow(kor1->y1 - *y2, 2))) <= 0.01 &&
        abs(kor2->r - sqrt(pow(kor2->x1 - *x2, 2) + pow(kor2->y1 - *y2, 2))) <= 0.01)
        return;
    else{
        /* Végigmegyek a körön oda vissza, felveszem a pontot a körvonalon, ha a másik körtől annak sugarára van, akkor átállítja a metszéspontot */
        double szog;
        for (szog = 0; szog <= 2 * 3.14159; szog += (double)1/100){
            double x = kor1->x1 + kor1->r * cos(szog), y = kor1->y1 + kor1->r * sin(szog);
            if (fabs((double)kor2->r - sqrt(pow(kor2->x1 - x, 2) + pow(kor2->y1 - y, 2))) <= 0.1){
                *x1 = x; *y1 = y;
                break;
            }
        }
        for (double ujszog = 0; ujszog > -2*3.14159 + szog; ujszog -= (double)1/100){
            double x = kor1->x1 + kor1->r * cos(ujszog), y = kor1->y1 + kor1->r * sin(ujszog);
            if (fabs((double)kor2->r - sqrt(pow(kor2->x1 - x, 2) + pow(kor2->y1 - y, 2))) <= 0.1){
                *x2 = x; *y2 = y;
                break;
            }
        }
    }
}

/** Két kör metszéspontját megkeresi
 *  Képlet forrás: http://2000clicks.com/MathHelp/GeometryConicSectionCircleIntersection.aspx */
void metszespont_kor_kor(Ablak *ablak, Alakzat *kor1, Alakzat *kor2, Metszespont **pontok){
	/* Középpontok közti távolság nyégyzete */
    double d2 = pow(kor1->x1-kor2->x1, 2) + pow(kor1->y1-kor2->y1, 2);
    /* A középpontok és az egyik metszéspontot körbefogó háromszög területe */
	double k = 0.25 * sqrt((double)(pow(kor1->r+kor2->r, 2) - d2)*(double)(d2 - pow(kor1->r - kor2->r, 2)));
	/* A két metszéspont x y koordinátái */
    double x1 = ((double)1/2)*(kor2->x1 + kor1->x1) + ((double)1/2)*(kor2->x2-kor1->x1)*(pow(kor1->r, 2) - pow(kor2->r, 2))/d2 + 2*(kor2->y1-kor1->y1)*k/d2;
    double x2 = ((double)1/2)*(kor2->x1 + kor1->x1) + ((double)1/2)*(kor2->x2-kor1->x1)*(pow(kor1->r, 2) - pow(kor2->r, 2))/d2 - 2*(kor2->y1-kor1->y1)*k/d2;
    double y1 = ((double)1/2)*(kor2->y1 + kor1->y1) + ((double)1/2)*(kor2->y2-kor1->y1)*(pow(kor1->r, 2) - pow(kor2->r, 2))/d2 - 2*(kor2->x1-kor1->x1)*k/d2;
    double y2 = ((double)1/2)*(kor2->y1 + kor1->y1) + ((double)1/2)*(kor2->y2-kor1->y1)*(pow(kor1->r, 2) - pow(kor2->r, 2))/d2 + 2*(kor2->x1-kor1->x1)*k/d2;

	metszespont_kor_kor_ellenorzes_javitas(kor1, kor2, &x1, &y1, &x2, &y2);

	/* Pontok kirajzolása és eltárolása, ha ráférnek az ábrára */
	if (abs(x1) <= 25 && abs(y1) <= 25){
		metszespont_mentes(pontok, x1, y1);
		metszespont_rajzolas(ablak, x1, y1);
	}
	if (abs(x2) <= 25 && abs(y2) <= 25){
		metszespont_mentes(pontok, x2, y2);
		metszespont_rajzolas(ablak, x2, y2);
	}
}

/** Megmondja, hogy benne van e az adott pont a szakasz értelmezési tartományában és értékkészletében */
bool szakasz_eleme(Alakzat *szakasz, double x, double y){
	int minx, maxx, miny, maxy;
	minx = (szakasz->x1 <= szakasz->x2) ? szakasz->x1 : szakasz->x2;
	maxx = (szakasz->x1 >= szakasz->x2) ? szakasz->x1 : szakasz->x2;
	miny = (szakasz->y1 <= szakasz->y2) ? szakasz->y1 : szakasz->y2;
	maxy = (szakasz->y1 >= szakasz->y2) ? szakasz->y1 : szakasz->y2;
	/* Ha a minimun és a maximum közé esik az x és y, akkor az eleme */
	if (x >= minx && x <= maxx && y >= miny && y <= maxy)
		return true;
	return false;
}

/* Egyenesből felvesz 2 pontot szögfüggvények segítségével az ábra két szélén */
void egyenesbol_ket_pont(Alakzat *egyenes, double *p1x, double *p1y, double *p2x, double *p2y){
	double tav = 70, pi = 3.14159;
	*p1x = egyenes->x1 + tav * cos(szog_radianba(egyenes->szog));
	*p1y = (egyenes->y1 + tav * sin(szog_radianba(egyenes->szog)));
	tav = -70;
	*p2x = egyenes->x1 + tav * cos(szog_radianba(egyenes->szog) + pi);
	*p2y = (egyenes->y1 + tav * sin(szog_radianba(egyenes->szog) + pi));
	/*double szog_rad = szog_radianba((egyenes->szog));
	tav = (double)(25 - egyenes->x1) / cos(szog_rad);
    /* Pont koordinátái szögfüggvényekkel */
    /**p1x = egyenes->x1 + tav * cos(szog_rad); *p1y = egyenes->y1 + tav * sin(szog_rad);
    /* Bal szélétől a távolság */
    //tav = (double)(-25 - egyenes->x1) / cos(szog_rad + pi);
    /* Pont koordinátái szögfüggvényekkel */
    /**p2x = egyenes->x1 + tav * cos(szog_rad + pi); *p2y = egyenes->y1 + tav * sin(szog_rad + pi);*/
}

/** Egy kör és egy szakasz/egyenes metszéspontját megkeresi
 *  Egyenlet forrás: http://csharphelper.com/blog/2014/09/determine-where-a-line-intersects-a-circle-in-c/ */
void metszespont_kor_egyenes(Ablak *ablak, Alakzat *kor, Alakzat *egyenes, Metszespont **pontok){
	/* Négy pont koordinátája */
	double p1x, p1y, p2x, p2y;
	/* Segédváltozók */
	double dx, dy, A, B, C, det, t;
	/* Ha az alakzat egyenes, akkor felveszek belőle egy szakasz */
	if (egyenes->tipus == EGYENES){
		egyenesbol_ket_pont(egyenes, &p1x, &p1y, &p2x, &p2y);
	}else{
		/* Szakasz pontjainak felvétele */
		p1x = egyenes->x1; p1y = egyenes->y1; p2x = egyenes->x2; p2y = egyenes->y2;
	}
	/* A két pont közötti távolság */
	dx = p2x - p1x;
	dy = p2y - p1y;
	/* Másodfokú egyenlet a távolságra */
	A = pow(dx, 2) + pow(dy, 2);
	B = 2 * (dx * (p1x - kor->x1) + dy * (p1y - kor->y1));
	C = pow(p1x - kor->x1, 2) + pow(p1y - kor->y1, 2) - pow(kor->r, 2);
	det = pow(B, 2) - 4 * A * C;
	/* Ha nincs valós  megoldás */
	if ((A <= 0.000001) || (det < 0))
		return;
	/* Ha egy megoldás van */
	else if (det == 0){
		/* Távolság az egyenlet befejezésével, metszéspont kiszámítása */
		t = -B / (double)(2 * A);
		double mx = p1x + t * dx, my = p1y + t * dy;
		/* Megnézem, hogy biztos része-e a metszéspont az alakzatnak */
		if (egyenes->tipus == EGYENES || szakasz_eleme(egyenes, mx, my)){
			metszespont_rajzolas(ablak, mx, my);
			metszespont_mentes(pontok, mx, my);
		}
	}
	/* Ha 2 megoldás van */
	else {
		/* Hasonló az előzőhöz, 2 megoldást külön-külön kiszámolom */
		t = (double)((-B + sqrt(det)) / (2 * A));
		double mx = p1x + t * dx, my = p1y + t * dy;
		if (egyenes->tipus == EGYENES || szakasz_eleme(egyenes, mx, my)){
			metszespont_rajzolas(ablak, mx, my);
			metszespont_mentes(pontok, mx, my);
		}
		t = (double)((-B - sqrt(det)) / (2 * A));
		mx = p1x + t * dx; my = p1y + t * dy;
		if (egyenes->tipus == EGYENES || szakasz_eleme(egyenes, mx, my)){
			metszespont_rajzolas(ablak, mx, my);
			metszespont_mentes(pontok, mx, my);
		}
	}
}

/** Kiszámolja két egyenes metszéspontját
 *  Képlet forrás: https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection */
void metszespont_egyenes_egyenes(Ablak *ablak, Alakzat *e1, Alakzat *e2, Metszespont **pontok){
	/* p1 p2 az első egyenes végpontjai (e1), a p3 p4 pedig a második egyenes végpontjai (e2) */
	double p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y;
	/* Ha egyenes az alakzat, felveszek rajta 2 pontot, ha nem csak eltárolom a szakasz pontjait */
	if (e1->tipus == EGYENES){
		egyenesbol_ket_pont(e1, &p1x, &p1y, &p2x, &p2y);
		//SDL_Log("%lf %lf %lf %lf", p1x, p1y, p2x, p2y);
	}
	else{
		p1x = e1->x1; p1y = e1->y1; p2x = e1->x2; p2y = e1->y2;
	}
	if (e2->tipus == EGYENES){
		egyenesbol_ket_pont(e2, &p3x, &p3y, &p4x, &p4y);
		//SDL_Log("%lf %lf %lf %lf", p3x, p3y, p4x, p4y);
	}
	else{
		p3x = e2->x1; p3y = e2->y1; p4x = e2->x2; p4y = e2->y2;
	}
	/* Képlet alkalmazása */
	double mx, my;
	mx = (double)((p1x*p2y-p1y*p2x)*(p3x-p4x)-(p1x-p2x)*(p3x*p4y-p3y*p4x))/((p1x-p2x)*(p3y-p4y)-(p1y-p2y)*(p3x-p4x));
	my = (double)((p1x*p2y-p1y*p2x)*(p3y-p4y)-(p1y-p2y)*(p3x*p4y-p3y*p4x))/((p1x-p2x)*(p3y-p4y)-(p1y-p2y)*(p3x-p4x));
    /* Ha a metszéspont nem része a szakasznak, akkor térjen vissza, ne mentse/rajzolja a pontot */
	if (e1->tipus == SZAKASZ && !szakasz_eleme(e1, mx, my))
        return;
    if (e2->tipus == SZAKASZ && !szakasz_eleme(e2, mx,my))
        return;

	metszespont_rajzolas(ablak, mx, my);
	metszespont_mentes(pontok, mx, my);
}
