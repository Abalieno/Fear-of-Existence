#include "busywork.h"

void press_key_combat(TCODConsole *thisone){
    thisone->setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
    thisone->setColorControl(TCOD_COLCTRL_2,TCODColor::black,TCODColor::white);
    thisone->setBackgroundFlag(TCOD_BKGND_SET);
    thisone->printf(1, 1,"%c START COMBAT TURN, Press any key %c",
            TCOD_COLCTRL_2,TCOD_COLCTRL_STOP);
    thisone->setDefaultForeground(TCODColor::black);
    thisone->setDefaultBackground(TCODColor::white);
    for (int n = 0; n < 3; ++n){
        thisone->putChar(0, n, TCOD_CHAR_VLINE, TCOD_BKGND_SET);
        thisone->putChar(35, n, TCOD_CHAR_VLINE, TCOD_BKGND_SET);
    }
    for (int n = 0; n < 35; ++n){
        thisone->putChar(n, 0, TCOD_CHAR_HLINE, TCOD_BKGND_SET);
        thisone->putChar(n, 2, TCOD_CHAR_HLINE, TCOD_BKGND_SET);
    }
    thisone->putChar(0, 0,  TCOD_CHAR_NW, TCOD_BKGND_SET);
    thisone->putChar(35, 0, TCOD_CHAR_NE, TCOD_BKGND_SET);
    thisone->putChar(0, 2, TCOD_CHAR_SW, TCOD_BKGND_SET);
    thisone->putChar(35, 2, TCOD_CHAR_SE, TCOD_BKGND_SET);
    return;
}
