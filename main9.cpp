#include <iostream>
#include <vector>
#include "libtcod.hpp"

const int   win_x   =   80; // window width in cells
const int   win_y   =   50; // window height in cells
const int   LIMIT_FPS = 20;

const int MAP_WIDTH = 80;
const int MAP_HEIGHT = 45;

TCODColor color_dark_wall(0, 0, 100);
TCODColor color_dark_ground(50, 50, 150);

TCODConsole *con = new TCODConsole(win_x, win_y);

class Tile {

public:

    int blocked;
    int block_sight;

    Tile() { blocked = 0; block_sight = 0; }

    Tile(int isblocked, int isblock_sight){
        blocked = isblocked;
        block_sight = isblock_sight;
        if (blocked) block_sight = 1;
    }
};

std::vector<Tile> map_array; // declared here because used in Object.move

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
       
        int tempx = 0;
        int tempy = 0;
        tempx = x + dx;
        tempy = y + dy;

        if (!map_array[tempy*MAP_WIDTH+tempx].blocked){
        x += dx;
        y += dy;
        } else std::cout << "Fuck, it's blocked. " << std::endl;

        if (x >= MAP_WIDTH) {x = MAP_WIDTH-1;std::cout << "No, I'm not stepping into the void." << std::endl;}
        if (y >= MAP_HEIGHT) {y = MAP_HEIGHT-1;std::cout << "No, I'm not stepping into the void." << std::endl;}
        if (x <= 0) {x = 0;std::cout << "No, I'm not stepping into the void." << std::endl;}
        if (y <= 0) {y = 0;std::cout << "No, I'm not stepping into the void." << std::endl;}
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

void make_map(){
           map_array.resize(MAP_HEIGHT * MAP_WIDTH);
        
    int row = 0;
    
    for (int i = 0; i < MAP_HEIGHT ;++i){
        
        for (int l = 0; l < MAP_WIDTH ;++l) {
           map_array[row * MAP_WIDTH + l] = Tile(0,0);
           map_array[3597] = Tile(1,1);
        }
        ++row;
    }

map_array[1].blocked = 1;
map_array[1].block_sight = 1;
map_array[4].blocked = 1;
map_array[4].block_sight = 1;
map_array[1789].blocked = 1;
map_array[1789].block_sight = 1;
map_array[1790-MAP_WIDTH].blocked = 1;
map_array[1790-MAP_WIDTH].block_sight = 1;
map_array[1790-MAP_WIDTH*2].blocked = 1;
map_array[1790-MAP_WIDTH*2].block_sight = 1;
map_array[1790].blocked = 1;
map_array[1790].block_sight = 1;
map_array[1791].blocked = 1;
map_array[1791].block_sight = 1;
map_array[1792].blocked = 1;
map_array[1792].block_sight = 1;
map_array[1793].blocked = 1;
map_array[1793].block_sight = 1;
map_array[1810].blocked = 1;
map_array[1810].block_sight = 1;

}

void render_all (std::vector<Object*> &invector){

    bool wall = 0;

    int row = 0;

    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            wall = map_array[row * MAP_WIDTH + l].blocked;
            if (wall) con->setCharBackground(l, i, color_dark_wall, TCOD_BKGND_SET);
            else con->setCharBackground(l, i, color_dark_ground, TCOD_BKGND_SET);
         }
        ++row;
    }

    for (unsigned int i = 0; i<invector.size(); ++i) invector[i]->draw();
    
    TCODConsole::blit(con,0,0,80,50,TCODConsole::root,0,0);
}

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

    myvector.push_back(&player);
    myvector.push_back(&npc);

    make_map();

    TCODConsole::initRoot(win_x, win_y, "windowname", false);
    
    while (! TCODConsole::isWindowClosed()) {

        TCODConsole::root->clear();
        
        render_all(myvector);
        
        TCODConsole::root->setAlignment(TCOD_RIGHT);
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->print(78, 47, "Use arrows to move");
        TCODConsole::root->print(78, 48, "Press 'q' to quit");
       
        TCODConsole::flush(); // this updates the screen

        for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->clear();

        if (handle_keys(player)) break;
    }
    return 0;
}
