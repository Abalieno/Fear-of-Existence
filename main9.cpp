#include <iostream>
#include "libtcod.hpp"

const int   win_x   =   80; // window width in cells
const int   win_y   =   50; // window height in cells
const int   LIMIT_FPS = 20;

int player_x = win_x / 2;
int player_y = win_y / 2;

bool handle_keys() {

    TCOD_key_t key = TCODConsole::waitForKeypress(true); 
    if ( key.c == 'q' )  return true;

    if (TCODConsole::isKeyPressed(TCODK_UP))
        --player_y;
    else if (TCODConsole::isKeyPressed(TCODK_DOWN))
        ++player_y;
    else if (TCODConsole::isKeyPressed(TCODK_LEFT))
        --player_x;
    else if (TCODConsole::isKeyPressed(TCODK_RIGHT))
        ++player_x;
    else return false;

    if (player_x >= 80) player_x = 79;
    if (player_y >= 50) player_y = 49;
    if (player_x <= 0) player_x = 0;
    if (player_y <= 0) player_y = 0;

    return false;

}    

int main() {
        
    TCODConsole::setCustomFont("arial10x10.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
    TCODSystem::setFps(LIMIT_FPS);
    
    TCODConsole::initRoot(win_x, win_y, "windowname", false);

    while (! TCODConsole::isWindowClosed()) {
        TCODConsole::root->clear();
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->print(1, 1, "Use arrows to move.");
        TCODConsole::root->setAlignment(TCOD_RIGHT);
        TCODConsole::root->print(78, 1, "Press 'q' to quit.");
        TCODConsole::root->print(78, 48, "I'm sorry, can't exit this screen.");
        TCODConsole::root->setAlignment(TCOD_LEFT);

        TCODConsole::root->setDefaultForeground(TCODColor::yellow);
        TCODConsole::root->print(player_x, player_y, "@");
        TCODConsole::flush(); // this updates the screen
        TCODConsole::root->print(player_x, player_y, " ");

        if (handle_keys()) break;
    }
    return 0;
}
