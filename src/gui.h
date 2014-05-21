#ifndef _GUI_H_
#define _GUI_H_

#include "game.h"

static std::vector<UIhook> localhook;

void UI_register(Game &tgame, int ID, int x, int y, int w, int h);

void UI_hook(Game &tgame, int ID);

void UI_unhook(Game &tgame, int ID);

#endif
