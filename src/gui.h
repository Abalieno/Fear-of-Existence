#ifndef _GUI_H_
#define _GUI_H_

#include <memory>
#include <vector>
#include <string>
#include "libtcod.hpp"
#include "game.h"

static std::vector<UIhook> localhook;

void UI_register(Game &tgame, int ID, int x, int y, int w, int h);

void UI_hook(Game &tgame, int ID);

void UI_unhook(Game &tgame, int ID);

const int move_up = 5;
const int move_down = 10;
const int keysel = 100; // used in UI_menu
const int action = 15;
const int outhere = 999;
const TCODColor colorbase(185, 192, 168); // made const for conflict in linking with main?

int menu_key(TCOD_event_t &eve, char &sel);
void buildframe(int totx, int toty, TCODConsole menu);

int UI_menu (unsigned int posx, unsigned int posy, std::vector<std::string> pack, bool type);

#endif
