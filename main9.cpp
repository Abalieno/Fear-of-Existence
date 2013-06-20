#include <iostream>
#include <vector>
#include "libtcod.hpp"
#include <windows.h>

const int   win_x   =   80; // window width in cells
const int   win_y   =   50; // window height in cells
const int   LIMIT_FPS = 20;

const int MAP_WIDTH = 80;
const int MAP_HEIGHT = 45;

//parameters for dungeon generator
int ROOM_MAX_SIZE = 10;
int ROOM_MIN_SIZE = 6;
int MAX_ROOMS = 30;

TCODColor color_dark_wall(0, 0, 100);
TCODColor color_dark_ground(50, 50, 150);
TCODColor blood(255, 0, 0);
TCODColor blood1(200, 0, 0);
TCODColor blood2(160, 0, 0);
TCODColor blood3(120, 0, 0);
TCODColor blood4(100, 0, 0);

TCODConsole *con = new TCODConsole(80, 45);
TCODConsole *mesg = new TCODConsole(33, 3);  // message pop-up drawn on top of "con"

class Tile {

public:

    bool blocked;
    bool block_sight;
    int bloodyt; // amount of blood on tile

    Tile() { blocked = false; block_sight = false; bloodyt = 0; }

    Tile(int isblocked, int isblock_sight){
        bloodyt = 0;
        blocked = isblocked;
        block_sight = isblock_sight;
        if (blocked) block_sight = true;
    }
};

std::vector<Tile> map_array; // declared here because used in Object.moved

class Rect {
public:
    int x1;
    int x2;
    int y1;
    int y2;
    int center_x;
    int center_y;

    Rect(int x, int y, int w, int h){
        x1 = x;
        y1 = y;
        x2 = x + w;
        y2 = y + h;
        center_x = (x1 + x2) / 2;
        center_y = (y1 + y2) / 2;
    }    

    void center(){
        // already set variables into class
    }

    bool intersect(Rect &other){
        // return (self.x1 <= other.x2 and self.x2 >= other.x1 and
        //        self.y1 <= other.y2 and self.y2 >= other.y1)
        std::cout << " " << x1 << " " << other.x2 << " " << x2 << " " << other.x1 << " " << y1 << " " << other.y2 << " "
            << y2 << " " << other.y1 << std::endl;
        return (x1 <= other.x2 && x2 >= other.x1 &&
               y1 <= other.y2 && y2 >= other.y1);
    }
};

void create_room(Rect inroom){
    
    for (int i = inroom.y1 + 1; i < inroom.y2;++i){
        for (int l = inroom.x1 + 1; l < inroom.x2 ;++l) {
            map_array[i * MAP_WIDTH + l] = Tile(0,0);
        }
    }
}

void create_h_tunnel(int x1, int x2, int y){

    for (int l = y * MAP_WIDTH + x1 + 1; l < y * MAP_WIDTH + x2 ;++l) {
        map_array[l] = Tile(0,0);
    }
}

void create_v_tunnel(int y1, int y2, int x){
    for (int l = y1 * MAP_WIDTH + x; l < y2 * MAP_WIDTH + x ; l = l + MAP_WIDTH) {
        map_array[l] = Tile(0,0);
    }
}

class Object {

public: // public should be moved down, but I keep it here for debug messages

    int x;
    int y;
    char selfchar;
    TCODColor color;
    TCODColor colorb;
    int h; // temp hit points
    int bloody;

    Object(int a, int b, char pchar, TCODColor oc, TCODColor oc2, int health) {
        x = a;
        y = b;
        selfchar = pchar;
        color = oc;
        colorb = oc2;
        h = health;
        bloody = 0;
    }

    void move(int dx, int dy) {
       
        int tempx = 0;
        int tempy = 0;
        tempx = x + dx;
        tempy = y + dy;
        
        if (!map_array[tempy*MAP_WIDTH+tempx].blocked){
        x += dx;
        y += dy;
        if (bloody > 0){
            if (bloody >= map_array[y * MAP_WIDTH + x].bloodyt)
            map_array[y * MAP_WIDTH + x].bloodyt = bloody;
        }
        
        } else std::cout << "Fuck, it's blocked. " << std::endl;

        if (x >= MAP_WIDTH) {x = MAP_WIDTH-1;std::cout << "No, I'm not stepping into the void." << std::endl;}
        if (y >= MAP_HEIGHT) {y = MAP_HEIGHT-1;std::cout << "No, I'm not stepping into the void." << std::endl;}
        if (x <= 0) {x = 0;std::cout << "No, I'm not stepping into the void." << std::endl;}
        if (y <= 0) {y = 0;std::cout << "No, I'm not stepping into the void." << std::endl;}

        bloody = (map_array[y * MAP_WIDTH + x].bloodyt);
        if (map_array[y * MAP_WIDTH + x].bloodyt > 1) --map_array[y * MAP_WIDTH + x].bloodyt;
        std::cout <<  map_array[y * MAP_WIDTH + x].bloodyt << std::endl;
    } 

    bool attack(Object &foe){
        foe.h = foe.h - 1;
        if (foe.h == 0) {std::cout << "Killed a foe." << std::endl; return 1;}// killed!
        return 0;
    }

    void draw(bool uh) {
        con->setDefaultForeground(color);
        if (!uh) colorb = con->getCharBackground(x, y);
        con->setDefaultBackground(colorb);
        con->putChar(x, y, selfchar, TCOD_BKGND_SET);
    }

    void clear() {
        con->putChar(x, y, ' ', TCOD_BKGND_NONE);
    }

    ~Object(){} // ?
};

Object npc(25, 26, '%', TCODColor::yellow, TCODColor::black, 3);
std::vector<Object*> myvector;

void make_map(Object &duh){
           
    map_array.resize(MAP_HEIGHT * MAP_WIDTH);
        
    int row = 0;
    
    for (int i = 0; i < MAP_HEIGHT ;++i){
        
        for (int l = 0; l < MAP_WIDTH ;++l) {
           map_array[row * MAP_WIDTH + l] = Tile(1,1);
           map_array[3597] = Tile(1,1);
        }
        ++row;
    } // fills map with walls 

    std::vector<Rect> rooms;
    int num_rooms = 0;

    int prev_x = 0;
    int prev_y = 0;

//Rect new_room(1, 1, 1, 1);
//rooms.push_back(&new_room);

    while (num_rooms <= MAX_ROOMS){
        bool failed;
Sleep(550);
        std::cout << "num_rooms: " << num_rooms << std::endl;
        std::cout << "Rooms array: " << rooms.size() << std::endl;


        TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

        //random width and height
        int w = 0;
         w=    wtf->getInt(ROOM_MIN_SIZE, ROOM_MAX_SIZE, 0);
        int h = 0;
         h=   wtf->getInt(ROOM_MIN_SIZE, ROOM_MAX_SIZE, 0);
        //random position without going out of the boundaries of the map
        int x = 0;
         x=   wtf->getInt( 0, MAP_WIDTH - w - 1,0);
        int y = 0;
         y=   wtf->getInt( 0, MAP_HEIGHT - h - 1,0);

        //Rect *new_room = new Rect(x, y, w, h);
        Rect new_room(x, y, w, h);
std::cout << "new room x y w h: " << x << " " << y << " "  << w << " " << h << std::endl;


        failed = false;
        
        for (unsigned int i = 0; i<rooms.size(); ++i){
std::cout << "i: " << i  << std::endl;

            if ( new_room.intersect(rooms[i]) ){ 
                failed = true;
                
                std::cout << "failed?: " << failed  << std::endl;

                break;}
                    }  
       // Sleep(3550);

        if (!failed){
            create_room(new_room);
            if (num_rooms == 0){
                duh.x = new_room.center_x;
                duh.y = new_room.center_y; // new player coordinates from room 0
            } else {
                npc.x = new_room.center_x;
                npc.y = new_room.center_y; // new npc coordinates from whatever room
                
                prev_x = rooms[num_rooms].center_x;
                prev_y = rooms[num_rooms].center_y;

                if (wtf->getInt(0, 1, 0) == 1){
                    create_h_tunnel(prev_x, rooms[num_rooms].center_x, prev_y);
                    create_v_tunnel(prev_y, rooms[num_rooms].center_y, rooms[num_rooms].center_x);
                } else {
                    create_v_tunnel(prev_y, rooms[num_rooms].center_y, prev_x);
                    create_h_tunnel(prev_x, rooms[num_rooms].center_x, rooms[num_rooms].center_y);
                }
            }
            num_rooms = num_rooms + 1; // add room to counter
            rooms.push_back(new_room); // add room to array
        }
           
    }

    npc.h = npc.h +2;
    npc.bloody = 0;
    myvector.push_back(&npc); // resurrects '%'

  //  Rect room1(20, 13, 10, 15);
  //  Rect room2(50, 18, 10, 15);
  //  create_room(room1);
  //  create_room(room2);

  //  create_h_tunnel(25, 55, 23);
  //  create_v_tunnel(18, 38, 33);

   // duh.x = rooms[0]->center_x;
   // duh.y = rooms[0]->center_y;

   //npc.x = rooms[6]->center_x;
   // npc.y = rooms[6]->center_y;

   //  npc.y = 40;

    //map_array[1789].blocked = 1;
    //map_array[1789].block_sight = 1;
    //map_array[1790-MAP_WIDTH].blocked = 1;
    //map_array[1790-MAP_WIDTH].block_sight = 1;
    //map_array[1790-MAP_WIDTH*2].blocked = 1;
    //map_array[1790-MAP_WIDTH*2].block_sight = 1;
}

void render_all (std::vector<Object*> &invector){

    bool wall = 0;
    int isbloody = 0;

    int row = 0;

    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l){
            wall = map_array[row * MAP_WIDTH + l].blocked;
            isbloody = map_array[row * MAP_WIDTH + l].bloodyt;
            if (wall){             
                con->putChar(l, i, '#', TCOD_BKGND_SET);
                con->setCharBackground(l, i, color_dark_wall, TCOD_BKGND_SET);
                con->setCharForeground(l, i, color_dark_wall);
            }
            else {
                con->putChar(l, i, '.', TCOD_BKGND_SET);
                con->setCharForeground(l, i, TCODColor::white);
                con->setCharBackground(l, i, color_dark_ground, TCOD_BKGND_SET);
            }
            if (isbloody > 0){
                if (isbloody >= 4) blood = blood1;
                if (isbloody == 3) blood = blood2;
                if (isbloody == 2) blood = blood3;
                if (isbloody < 2) blood = blood4;
                con->setCharBackground(l, i, blood, TCOD_BKGND_SET);
            }
        }
        ++row;
    }

    for (unsigned int i = 0; i<invector.size(); ++i) invector[i]->draw(0);
    
    TCODConsole::blit(con,0,0,80,45,TCODConsole::root,0,0);
}

void bloodsplat(Object &cobj){
    TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

    map_array[cobj.y  * MAP_WIDTH + cobj.x].bloodyt = 6; // center

    int notone = wtf->getInt(1, 4, 0);
    if (notone != 1) map_array[(cobj.y-1)  * MAP_WIDTH + cobj.x].bloodyt = 3;
    if (notone != 2) map_array[(cobj.y+1)  * MAP_WIDTH + cobj.x].bloodyt = 3;
    if (notone != 3) map_array[cobj.y  * MAP_WIDTH + (cobj.x+1)].bloodyt = 3;
    if (notone != 4) map_array[cobj.y  * MAP_WIDTH + (cobj.x-1)].bloodyt = 3; // cross

    short int fly = 0;
    fly = wtf->getInt(1, 5, 0);
    std::cout << "splats number(1-5): " << fly << std::endl;

    while (fly > 0){
        int dir = wtf->getInt(1, 8, 0);
        std::cout << "splat dir(1-8): " << dir << std::endl;
        int sdir = 0;
        int xdir = 0;
        int ydir = 0;
        switch (dir){ // flying blood spatter
            case 1:
                sdir = wtf->getInt(1, 2, 0);
                std::cout << "splat sdir(1-2): " << sdir << std::endl;
                if (sdir == 1) map_array[(cobj.y-2)  * MAP_WIDTH + cobj.x].bloodyt = 2;
                else map_array[(cobj.y-3)  * MAP_WIDTH + cobj.x].bloodyt = 2;    
                break;
            case 2:
                xdir = wtf->getInt(1, 3, 0);
                ydir = wtf->getInt(1, 3, 0);
                std::cout << "splat xdir(1-3): " << xdir << " splat ydir(1-3): " << ydir << std::endl;
                map_array[(cobj.y-ydir)  * MAP_WIDTH + (cobj.x+xdir)].bloodyt = 2;
                break;
            case 3:
                sdir = wtf->getInt(1, 2, 0);
                std::cout << "splat sdir(1-2): " << sdir << std::endl;
                if (sdir == 1) map_array[cobj.y  * MAP_WIDTH + (cobj.x+2)].bloodyt = 2;
                else map_array[cobj.y  * MAP_WIDTH + (cobj.x+3)].bloodyt = 2;
                break;
            case 4: 
                xdir = wtf->getInt(1, 3, 0);
                ydir = wtf->getInt(1, 3, 0);
                std::cout << "splat xdir(1-3): " << xdir << " splat ydir(1-3): " << ydir << std::endl;
                map_array[(cobj.y+ydir)  * MAP_WIDTH + (cobj.x+xdir)].bloodyt = 2;
                break;
            case 5:
                sdir = wtf->getInt(1, 2, 0);
                std::cout << "splat sdir(1-2): " << sdir << std::endl;
                if (sdir == 1) map_array[(cobj.y+2)  * MAP_WIDTH + cobj.x].bloodyt = 2;
                else map_array[(cobj.y+3)  * MAP_WIDTH + cobj.x].bloodyt = 2;
                break;
            case 6:
                xdir = wtf->getInt(1, 3, 0);
                ydir = wtf->getInt(1, 3, 0);
                std::cout << "splat xdir(1-3): " << xdir << " splat ydir(1-3): " << ydir << std::endl;
                map_array[(cobj.y+ydir)  * MAP_WIDTH + (cobj.x-xdir)].bloodyt = 2;
                break;
            case 7:
                sdir = wtf->getInt(1, 2, 0);
                std::cout << "splat sdir(1-2): " << sdir << std::endl;
                if (sdir == 1) map_array[cobj.y  * MAP_WIDTH + (cobj.x-2)].bloodyt = 2;
                else map_array[cobj.y  * MAP_WIDTH + (cobj.x-3)].bloodyt = 2;
                break;
            case 8:
                xdir = wtf->getInt(1, 3, 0);
                ydir = wtf->getInt(1, 3, 0);
                std::cout << "splat xdir(1-3): " << xdir << " splat ydir(1-3): " << ydir << std::endl;
                map_array[(cobj.y-ydir)  * MAP_WIDTH + (cobj.x-xdir)].bloodyt = 2;
                break;
            default:
                break;
                std::cout << "splat sdir(none): " << dir << std::endl;
        }
        --fly;
    }
       

}

short int bloodycount = 0;


bool handle_keys(Object &duh) {

    bool mycase_p;
    mycase_p = 0;
    
    TCOD_key_t key = TCODConsole::waitForKeypress(true); 

    //if (bloodycount == 0) duh.bloody = false;
    if (bloodycount < 0) bloodycount = 0; // if ... change color 
  
    if ( key.c == 'q' )  return true;

    if ( key.c == 'r' ){
        make_map(duh);
        duh.bloody = 0;


    }

    if ( key.c == 'p' ){
        TCODConsole::root->clear();
        mesg->setAlignment(TCOD_LEFT);
        mesg->setDefaultForeground(TCODColor::white);
        mesg->setDefaultBackground(TCODColor::black);
        mesg->print(1, 1, "Give a direction to dig dungeon");
        myvector[1]->draw(0);
       
        if(!(duh.y > 37 )) TCODConsole::blit(mesg,0,0,33,3,con,1,41);
        else TCODConsole::blit(mesg,0,0,33,3,con,46,1);

        TCODConsole::blit(con,0,0,80,45,TCODConsole::root,0,0);

        TCODConsole::flush();
        while (!mycase_p){
            TCODConsole::waitForKeypress(true);
            if (TCODConsole::isKeyPressed(TCODK_UP)){ 
                std::cout << "Dig up." << std::endl; mycase_p = 1;
                map_array[(duh.y - 1)*MAP_WIDTH +duh.x].blocked = 0;
                map_array[(duh.y - 1)*MAP_WIDTH +duh.x].block_sight = 0;
                mesg->clear();
                return false;
            }
            if (TCODConsole::isKeyPressed(TCODK_DOWN)){ 
                std::cout << "Dig down." << std::endl;mycase_p = 1; 
                map_array[(duh.y + 1)*MAP_WIDTH +duh.x].blocked = 0;
                map_array[(duh.y + 1)*MAP_WIDTH +duh.x].block_sight = 0;
                mesg->clear();
                return false; 
            }
            if (TCODConsole::isKeyPressed(TCODK_LEFT)){
                std::cout << "Dig left." << std::endl;mycase_p = 1;
                map_array[duh.y*MAP_WIDTH +(duh.x -1)].blocked = 0;
                map_array[duh.y*MAP_WIDTH +(duh.x -1)].block_sight = 0;
                mesg->clear();
                return false;
            }
            if (TCODConsole::isKeyPressed(TCODK_RIGHT)){ 
                std::cout << "Dig right." << std::endl;mycase_p = 1;
                map_array[duh.y*MAP_WIDTH +(duh.x + 1)].blocked = 0;
                map_array[duh.y*MAP_WIDTH +(duh.x + 1)].block_sight = 0;
                mesg->clear();
                return false;
            }
        }
    }

    // end of KEY DIG cycle

    if (TCODConsole::isKeyPressed(TCODK_UP)){
        if(npc.x == duh.x && npc.y == duh.y - 1) { // checks existence of particular object
            if (duh.attack(npc)){
                myvector.erase (myvector.begin()+1); 
                bloodsplat(npc);
                bloodycount = 5;
                npc.x=-365;
                npc.y=-365; // teleport out of map   
                duh.move(0, 0); // updates player position so feet get bloody
            }
            else {
                TCODConsole::root->clear();
                mesg->setAlignment(TCOD_LEFT);
                mesg->setDefaultForeground(TCODColor::yellow);
                mesg->setDefaultBackground(TCODColor::black);
                npc.colorb = TCODColor::red;
                npc.draw(1);
                mesg->print(1, 1, "Hit!");
                if(!(duh.y > 37 )) TCODConsole::blit(mesg,0,0,33,3,con,1,41);
                else TCODConsole::blit(mesg,0,0,33,3,con,46,1);
                TCODConsole::blit(con,0,0,80,45,TCODConsole::root,0,0);
                TCODConsole::flush();
                Sleep(200); // shitty way for attack "animation", uses windows.h
                npc.colorb = color_dark_ground;
                con->clear();
            }
        }
    else  {
        --bloodycount;
        --duh.bloody;  
        duh.move(0, -1);
    }
    }

    // end KEY UP cycle

    else if (TCODConsole::isKeyPressed(TCODK_DOWN)){
        if(npc.x == duh.x && npc.y == duh.y + 1){
            if (duh.attack(npc)){ 
                myvector.erase (myvector.begin()+1); 
                bloodsplat(npc); 
                bloodycount = 5;
                npc.x=-365;
                npc.y=-365;
                duh.move(0, 0);
            }
            else {
                TCODConsole::root->clear();
                mesg->setAlignment(TCOD_LEFT);
                mesg->setDefaultForeground(TCODColor::yellow);
                mesg->setDefaultBackground(TCODColor::black);
                npc.colorb = TCODColor::red;
                npc.draw(1);
                mesg->print(1, 1, "Hit!");
                if(!(duh.y > 37 )) TCODConsole::blit(mesg,0,0,33,3,con,1,41);
                else TCODConsole::blit(mesg,0,0,33,3,con,46,1);
                TCODConsole::blit(con,0,0,80,45,TCODConsole::root,0,0);
                TCODConsole::flush();
                Sleep(200);
                npc.colorb = color_dark_ground;
                con->clear();
            }
        }
        else { 
            --bloodycount;
            --duh.bloody; 
            duh.move(0, 1);
        }
    }

    // end KEY DOWN cycle

    else if (TCODConsole::isKeyPressed(TCODK_LEFT)){
        if(npc.x == duh.x - 1 && npc.y == duh.y ){
            if (duh.attack(npc)){
                myvector.erase (myvector.begin()+1); 
                bloodsplat(npc);
                bloodycount = 5;
                npc.x=-365;
                npc.y=-365;
                duh.move(0, 0);
            }
            else {
                TCODConsole::root->clear();
                mesg->setAlignment(TCOD_LEFT);
                mesg->setDefaultForeground(TCODColor::yellow);
                mesg->setDefaultBackground(TCODColor::black);
                npc.colorb = TCODColor::red;
                npc.draw(1);
                mesg->print(1, 1, "Hit!");
                if(!(duh.y > 37 )) TCODConsole::blit(mesg,0,0,33,3,con,1,41);
                else TCODConsole::blit(mesg,0,0,33,3,con,46,1);
                TCODConsole::blit(con,0,0,80,45,TCODConsole::root,0,0);
                TCODConsole::flush();
                Sleep(200);
                npc.colorb = color_dark_ground;
                con->clear();
            }
        }
        else {
            --bloodycount;
            --duh.bloody;   
            duh.move(-1, 0);
        }
    }

    // end KEY LEFT cycle

    else if (TCODConsole::isKeyPressed(TCODK_RIGHT)){
        if(npc.x == duh.x + 1 && npc.y == duh.y ) {
            if (duh.attack(npc)){
                myvector.erase (myvector.begin()+1); 
                bloodsplat(npc);
                bloodycount = 5;
                npc.x=-365;
                npc.y=-365;
                duh.move(0, 0);
            }
            else {
                TCODConsole::root->clear();
                mesg->setAlignment(TCOD_LEFT);
                mesg->setDefaultForeground(TCODColor::yellow);
                mesg->setDefaultBackground(TCODColor::black);
                npc.colorb = TCODColor::red;
                npc.draw(1);
                mesg->print(1, 1, "Hit!");
                if(!(duh.y > 37 )) TCODConsole::blit(mesg,0,0,33,3,con,1,41);
                else TCODConsole::blit(mesg,0,0,33,3,con,46,1);
                TCODConsole::blit(con,0,0,80,45,TCODConsole::root,0,0);
                TCODConsole::flush();
                Sleep(200);
                npc.colorb = color_dark_ground;
                con->clear();
           }
        }
        else  {
            --bloodycount; 
            --duh.bloody; 
            duh.move(1, 0);
        }
    }

    // end KEY RIGHT cycle

    std::cout << "player.x: " << duh.x << " player.y: " << duh.y << std::endl; 
    return false;
}    

int main() {
        
    TCODConsole::setCustomFont("arial10x10.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
    TCODSystem::setFps(LIMIT_FPS);

    Object player(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5);
    
    
    myvector.push_back(&player);
    myvector.push_back(&npc);
   
    make_map(player);

    player.colorb = con->getCharBackground(player.x, player.y);
    npc.colorb = con->getCharBackground(npc.x, npc.y);

    player.colorb = color_dark_ground;
    npc.colorb = color_dark_ground;

    TCODConsole::initRoot(win_x, win_y, "windowname", false);
    
    while (! TCODConsole::isWindowClosed()) {

        TCODConsole::root->clear();
        
        render_all(myvector);
        
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->setDefaultBackground(TCODColor::black);
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->print(1, 46, "Use arrows to move");
        TCODConsole::root->print(1, 47, "Press 'q' to quit");
        TCODConsole::root->setAlignment(TCOD_RIGHT);
        TCODConsole::root->print(78, 46, "Press 'p' to punch walls");
        TCODConsole::root->print(78, 47, "Press 'r' to regenerate layout");
        
        TCODConsole::root->setAlignment(TCOD_CENTER);
        TCODConsole::root->print(40,49,"%c%c%c%c%c%c%c%cKILL%c the '%'",
        TCOD_COLCTRL_FORE_RGB,255,1,1,TCOD_COLCTRL_BACK_RGB,1,1,255,TCOD_COLCTRL_STOP);
       
        TCODConsole::flush(); // this updates the screen

        for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->clear();

        if (handle_keys(player)) break;
    }
    return 0;
}
