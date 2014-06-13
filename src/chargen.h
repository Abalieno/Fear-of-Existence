#ifndef _CHARGEN_H_
#define _CHARGEN_H_

#include <string>
#include "game.h"

const int Human = 0;
const int Sindarin = 1;
const int Khuzdul = 2;

const int Male = 1;
const int Female = 0;

const int Nuzyael = 0;
const int Peonu = 1;
const int Kelen = 2;
const int Nolus = 3;
const int Larane = 4;
const int Agrazhar = 5;
const int Azura = 6;
const int Halane = 7;
const int Savor = 8;
const int Ilvin = 9;
const int Navek = 10;
const int Morgat = 11;

const int Ulandus = 0;
const int Aralius = 1;
const int Feniri = 2;
const int Ahnu = 3;
const int Angberelius = 4;
const int Nadai = 5;
const int Hirin = 6;
const int Tarael = 7;
const int Tai = 8;
const int Skorus = 9;
const int Masara = 10;
const int Lado = 11;

const int Outcast = 0;
const int Unpopular = 1;
const int Average = 2;
const int Popular = 3;
const int Favorite = 4;

const int Righthanded = 0;
const int Lefthanded = 1;
const int Ambidextrous = 2;

const int Scant = 0;
const int Light = 1;
const int Medium = 2;
const int Heavy = 3;
const int Massive = 4;

const int CPallid = 0;
const int CFair = 1;
const int CMedium = 2;
const int CDark = 3;
const int CBlack = 4;

const int HNull = -1;
const int HBlack = 0;
const int HBrown = 1;
const int HRed = 2;
const int HAuburn = 3;
const int HDarkblonde = 4;
const int HBlonde = 5;
const int HGreywhite = 6;

const int EBrown = 0;
const int EHazel = 1;
const int EBlue = 2;
const int EGrey = 3;
const int EGreen = 4;
const int EViolet = 5;

const int Ugly = 0;
const int Plain = 1;
const int BAverage = 2;
const int Attractive = 3;
const int Handsome = 4;

void gen_name(Game &GAME);
void gen_species(Game &GAME);
void gen_sex(Game &GAME);
void gen_birthdate(Game &GAME);
void gen_sunsign(Game &GAME);
void gen_sibrank(Game &GAME);
void gen_strange(Game &GAME);
void gen_hand(Game &GAME);
void gen_height(Game &GAME);
void gen_frame(Game &GAME);
void gen_weight(Game &GAME);
void gen_complexion(Game &GAME);
void gen_hair(Game &GAME);
void gen_eye(Game &GAME);

void draw_frame(const char *title1, const char *title2);

void txt_sex(char* here, Game &GAME);
void txt_species(char* here, Game &GAME);
void txt_birthdate(char* here, Game &GAME);
void txt_sunsign(char* here, Game &GAME);
void txt_complexion(char* here, Game &GAME);

int chargen(Game &GAME);



#endif
