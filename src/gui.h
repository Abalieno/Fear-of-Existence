#ifndef _GUI_H_
#define _GUI_H_

#include "game.h"

static std::vector<UIhook> localhook;

bool UI_register(Game &tgame, int ID, int x, int y, int w, int h);

bool UI_hook(Game &tgame, int ID);

bool UI_unhook(Game &tgame, int ID);

#endif
