#ifndef FILEKEZELES_H
#define FILEKEZELES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <stdio.h>

#include "megjelenites.h"
#include "vezerlo.h"

void mentes(char *file_nev, Alakzat *eleje, Metszespont *pontok);
void betoltes(Ablak *ablak, char *file_nev, Alakzat **eleje, Metszespont **pontok);

#endif
