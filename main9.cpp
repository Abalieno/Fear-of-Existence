#include <iostream>
#include <vector>
#include "libtcod.hpp"

const int   win_x   =   80; // window width in cells
const int   win_y   =   50; // window height in cells
const int   LIMIT_FPS = 20;

TCODConsole *con = new TCODConsole(win_x, win_y);

class Object {

public: // public should be moved down, but I keep it here for debug messages

    int x;
    int y;
    char selfchar;
    TCODColor color;

    Object(int a, int b, char pchar, TCODColor oc) {
        x = a;
        y = b;
        selfchar = pchar;
        color = oc;
    }

    void move(int dx, int dy) {
        x += dx;
        y += dy;

        if (x >= 80) x = 79;
        if (y >= 50) y = 49;
        if (x <= 0) x = 0;
        if (y <= 0) y = 0;
        std::cout << "x in move: " << x << std::endl; 
    }  

    void draw() {
        con->setDefaultForeground(color);
        con->putChar(x, y, selfchar, TCOD_BKGND_NONE);
        std::cout << "x in draw: " << x << std::endl; 
    }

    void clear() {
        con->putChar(x, y, ' ', TCOD_BKGND_NONE);
    }
};    

bool handle_keys(Object &duh) {

    TCOD_key_t key = TCODConsole::waitForKeypress(true); 
    if ( key.c == 'q' )  return true;

    if (TCODConsole::isKeyPressed(TCODK_UP))
        duh.move(0, -1);
    else if (TCODConsole::isKeyPressed(TCODK_DOWN))
        duh.move(0, 1);
    else if (TCODConsole::isKeyPressed(TCODK_LEFT))
        duh.move(-1, 0);
    else if (TCODConsole::isKeyPressed(TCODK_RIGHT))
        duh.move(1, 0);

    return false;
}    

int main() {
        
    TCODConsole::setCustomFont("arial10x10.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
    TCODSystem::setFps(LIMIT_FPS);

    Object player(win_x/2, win_y/2, '@', TCODColor::white);
    Object npc(win_x/2 - 5, win_y/2, '%', TCODColor::yellow);

    std::vector<Object*> myvector;

    std::vector<int> test;
    test.push_back(25);
    test.push_back(50);
    for (unsigned int i = 0; i<test.size(); ++i){
        std::cout << test[i] << std::endl;
    }

    myvector.push_back(&player);
    myvector.push_back(&npc);

    std::cout << myvector.size() << std::endl; // 2
    std::cout << player.selfchar << std::endl; 
    
    TCODConsole::initRoot(win_x, win_y, "windowname", false);
    
    while (! TCODConsole::isWindowClosed()) {

         TCODConsole::root->clear();
       
        for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->draw();
        
        con->setDefaultForeground(TCODColor::white);
        con->print(1, 1, "Use arrows to move.");

        TCODConsole::blit(con,0,0,80,50,TCODConsole::root,0,0);
        TCODConsole::flush(); // this updates the screen

        for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->clear();

        std::cout << "true player: " << player.x << " npc: " << npc.x << std::endl;
        std::cout << "vector: " << myvector[0]->x << std::endl;
        
        if (handle_keys(player)) break;
    }
    return 0;
}
