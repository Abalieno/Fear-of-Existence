#include <iostream>
#include <vector>
#include "libtcod.hpp"
#include <windows.h> // for Sleep()

const int   win_x   =   80; // window width in cells
const int   win_y   =   50; // window height in cells
const int   LIMIT_FPS = 20;

const int MAP_WIDTH = 80;
const int MAP_HEIGHT = 45;

const int quit = 1;
const int playing = 1;
const int no_turn = 2;

//parameters for dungeon generator
int ROOM_MAX_SIZE = 10;
int ROOM_MIN_SIZE = 6;
int MAX_ROOMS = 32;
int MAX_ROOM_MONSTERS = 3;

TCOD_fov_algorithm_t FOV_ALGO = FOV_BASIC; //default FOV algorithm
bool FOV_LIGHT_WALLS = true;
int TORCH_RADIUS = 10;

TCODColor color_dark_wall(0, 0, 50);
TCODColor color_dark_ground(15, 15, 80);
TCODColor blood(255, 0, 0);
TCODColor blood1(200, 0, 0);
TCODColor blood2(160, 0, 0);
TCODColor blood3(120, 0, 0);
TCODColor blood4(100, 0, 0);

TCODColor color_light_wall(0, 0, 100);
TCODColor color_light_ground(50, 50, 150);
//TCODColor color_light_ground(200, 180, 50);

TCODColor orc(0, 200, 0);
TCODColor troll(0, 255, 0);

bool fov_recompute;

TCODConsole *con = new TCODConsole(MAP_WIDTH, MAP_HEIGHT);
TCODConsole *mesg = new TCODConsole(33, 3);  // message pop-up drawn on top of "con"

TCODMap * fov_map = new TCODMap(MAP_WIDTH,MAP_HEIGHT);

class Tile {

public:

    bool blocked;
    bool block_sight;
    int bloodyt; // amount of blood on Tile
    int explored;

    Tile() { blocked = false; block_sight = false; bloodyt = 0; }

    Tile(int isblocked, int isblock_sight){
        bloodyt = 0;
        blocked = isblocked;
        block_sight = isblock_sight;
        if (blocked) block_sight = true;
        explored = false;
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

    if (x1 < x2){ // to carve tunnel in the right direction, left to right
        for (int l = y * MAP_WIDTH + x1 ; l <= (y * MAP_WIDTH + x2) ;++l){
            map_array[l] = Tile(0,0);
        }
    } else for (int l = y * MAP_WIDTH + x2 ; l <= (y * MAP_WIDTH + x1) ;++l){
        map_array[l] = Tile(0,0);
    }
}

void create_v_tunnel(int y1, int y2, int x){
    if (y1 < y2){
        for (int l = y1 * MAP_WIDTH + x; l <= (y2 * MAP_WIDTH + x) ; l = l + MAP_WIDTH){
            map_array[l] = Tile(0,0);
        }
    } else for (int l = y2 * MAP_WIDTH + x; l <= (y1 * MAP_WIDTH + x) ; l = l + MAP_WIDTH){
        map_array[l] = Tile(0,0);
    }
}


class Object {

public: // public should be moved down, but I keep it here for debug messages

    char name[20];
    int x;
    int y;
    char selfchar;
    TCODColor color;
    TCODColor colorb;
    int h; // temp hit points
    int bloody;
    bool blocks;

    Object(int a, int b, char pchar, TCODColor oc, TCODColor oc2, int health) {
        x = a;
        y = b;
        selfchar = pchar;
        color = oc;
        colorb = oc2;
        h = health;
        bloody = 0;
    }

    void move(int dx, int dy, std::vector<Object> smonvector); 
       
     /* 
        defined below
      */ 

    bool attack(Object &foe){
        foe.h = foe.h - 1;
        if (foe.h == 0) {std::cout << "Killed a foe." << std::endl; return 1;}// killed!
        return 0; // not killed
    }

    void draw(bool uh) {
        con->setDefaultForeground(color);
        if (!uh) colorb = con->getCharBackground(x, y);
        con->setDefaultBackground(colorb);
        if (fov_map->isInFov(x,y))
        con->putChar(x, y, selfchar, TCOD_BKGND_SET);
    }

    void clear() {
        if (fov_map->isInFov(x,y))
        con->putChar(x, y, ' ', TCOD_BKGND_NONE);
    }

    ~Object(){} // ?
};

Object player(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5);

std::vector<Object*> myvector;
std::vector<Object> monvector;

bool is_blocked(int x, int y, std::vector<Object> smonvector){
    if (map_array[y*MAP_WIDTH+x].blocked) return true;

    for (unsigned int i = 0; i<smonvector.size(); ++i){
        if (smonvector[i].blocks == true && smonvector[i].x == x && smonvector[i].y == y){
            std::cout << "Monster in the way." << std::endl;
            return true;
        }
       // monvector[i].draw(0);
    }
    return false;
}

void Object::move(int dx, int dy, std::vector<Object> smonvector) {
       
        int tempx = 0;
        int tempy = 0;
        tempx = x + dx;
        tempy = y + dy;

        if (!is_blocked(tempx,tempy,smonvector)){
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
    }

void place_objects(Rect room){
    TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

    int num_mosters = wtf->getInt(0, MAX_ROOM_MONSTERS, 0);

    int x, y;
    Object monster(0, 0, 'i', TCODColor::black, TCODColor::black, 0);

    for (int i = 0; i < num_mosters; ++i){
        x = wtf->getInt((room.x1+1), (room.x2-1), 0);
        y = wtf->getInt((room.y1+1), (room.y2-1), 0);
        if (!is_blocked(x,y, monvector)){
        if ( wtf->getInt(0, 100, 0) < 80){
            monster.x = x;
            monster.y = y;
            monster.selfchar= 'o';
            monster.color = orc;
            monster.colorb = TCODColor::black;
            monster.h = 4;
            monster.blocks = true;
            //monster.name[] = "orc";
            strcpy(monster.name, "orc");
            }
        else {
            monster.x = x;
            monster.y = y;
            monster.selfchar= 'T';
            monster.color = troll;
            monster.colorb = TCODColor::black;
            monster.h = 5;  
            monster.blocks = true;
            //monster.name[] = "troll";
            strcpy(monster.name, "troll");
        }  
        monvector.push_back(monster);
    }
    
    std::cout << " Monster array: " << myvector.size() << std::endl;
    }
}

int killall = 0;

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

    for (int r = 0; r < MAX_ROOMS; ++r){

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

        bool failed;
        failed = false;
        
        for (unsigned int i = 0; i<rooms.size(); ++i){
            if ( new_room.intersect(rooms[i]) ){ 
                failed = true;
                break;
            }
        }  

        if (!failed){
            create_room(new_room);
            place_objects(new_room);
            if (num_rooms == 0){
                duh.x = new_room.center_x;
                duh.y = new_room.center_y; // new player coordinates from room 0
            } else {
               // npc.x = new_room.center_x;
               // npc.y = new_room.center_y; // new npc coordinates from whatever room
               
                prev_x = rooms[num_rooms-1].center_x;
                prev_y = rooms[num_rooms-1].center_y;

                if (wtf->getInt(0, 1, 0) == 1){
                    create_h_tunnel(prev_x, new_room.center_x, prev_y);
                    create_v_tunnel(prev_y, new_room.center_y, new_room.center_x);
                } else {
                    create_v_tunnel(prev_y, new_room.center_y, prev_x);
                    create_h_tunnel(prev_x, new_room.center_x, new_room.center_y);
                }
            }
            num_rooms = num_rooms + 1; // add room to counter
            rooms.push_back(new_room); // add room to array
        }
    }

    killall = monvector.size();
}

void set_black(){
    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            con->putChar(l, i, ' ', TCOD_BKGND_SET);
                        con->setCharBackground(l, i, TCODColor::black, TCOD_BKGND_SET);
                        con->setCharForeground(l, i, TCODColor::black);
        }
    } 
} // fill "con" with black

void render_all (std::vector<Object*> &invector){

    bool wall = false;
    bool visible = false;
    int isbloody = 0;

    if (fov_recompute){
        fov_map->computeFov(player.x,player.y, TORCH_RADIUS, FOV_LIGHT_WALLS,FOV_ALGO);

        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                visible = fov_map->isInFov(l,i);
                wall = map_array[i * MAP_WIDTH + l].blocked;
                isbloody = map_array[i * MAP_WIDTH + l].bloodyt;
                con->putChar(l, i, ' ', TCOD_BKGND_SET);
                        con->setCharBackground(l, i, TCODColor::black, TCOD_BKGND_SET);
                        con->setCharForeground(l, i, TCODColor::black);

                if (!visible){
                    if (map_array[i * MAP_WIDTH + l].explored){
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
                                        }
                } else {
                    if (wall){
                        con->putChar(l, i, '#', TCOD_BKGND_SET);
                        con->setCharBackground(l, i, color_light_wall, TCOD_BKGND_SET);
                        con->setCharForeground(l, i, color_light_wall);
                    }
                    else {
                        con->putChar(l, i, '.', TCOD_BKGND_SET);
                        con->setCharForeground(l, i, TCODColor::white);
                        con->setCharBackground(l, i, color_light_ground, TCOD_BKGND_SET);
                    }
                    if (isbloody > 0){
                if (isbloody >= 4) blood = blood1;
                if (isbloody == 3) blood = blood2;
                if (isbloody == 2) blood = blood3;
                if (isbloody < 2) blood = blood4;
                con->setCharBackground(l, i, blood, TCOD_BKGND_SET);
            }
                    map_array[i * MAP_WIDTH + l].explored = true;
                    
                }
                //fov_map->setProperties(l, i, map_array[i * MAP_WIDTH + l].block_sight, map_array[i * MAP_WIDTH + l].blocked);
                
            }
        }

    fov_recompute = false;
    }

    for (unsigned int i = 0; i<invector.size(); ++i) invector[i]->draw(0);
    for (unsigned int i = 0; i<monvector.size(); ++i) monvector[i].draw(0);
    
    TCODConsole::blit(con,0,0,win_x,win_y,TCODConsole::root,0,0);
}

int blx = 0;
int bly = 0;

int fly_blood_fly_d(int asin, int bsin, int origin, int y, int x){
    
    int tile_1 = origin + ((asin*1) * MAP_WIDTH) + (bsin*1);
    int tile_2 = origin + ((asin*2) * MAP_WIDTH) + (bsin*1);
    int tile_4 = origin + ((asin*1) * MAP_WIDTH) + (bsin*2);
    int tile_5 = origin + ((asin*2) * MAP_WIDTH) + (bsin*2);
    int tile_X = origin + ((asin*y) * MAP_WIDTH) + (bsin*x);

    if (map_array[tile_1].blocked || (y == 1 && x == 1)) return tile_1;
    else if (y < 3 && x < 3) return tile_X;
    else if ((y == 3 && x == 1) || (y == 3 && x == 2)) 
        if (map_array[tile_2].blocked) return tile_2;
        else return tile_X;
    else if ((y == 1 && x == 3) || (y == 2 && x == 3))
        if (map_array[tile_4].blocked) return tile_4;
        else return tile_X;
    else if (map_array[tile_5].blocked) return tile_5;     
    else return tile_X;
}

int fly_blood_fly_h(int asin, int bsin, int origin, int where){

    int tile_B = origin + ((asin*1) * MAP_WIDTH + (bsin*1));
    int tile_1 = origin + ((asin*2) * MAP_WIDTH + (bsin*2));
    int tile_2 = origin + ((asin*3) * MAP_WIDTH + (bsin*3));
    int tile_3 = origin + ((asin*4) * MAP_WIDTH + (bsin*4));

    if (map_array[tile_B].blocked) return tile_B;
    else if (map_array[tile_1].blocked || where == 1) return tile_1;
    else if (map_array[tile_2].blocked || where == 2) return tile_2;
    else return tile_3;
}

void bloodsplat(Object &cobj){

    int blood = 2;

    TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

    map_array[cobj.y  * MAP_WIDTH + cobj.x].bloodyt = 6; // center

    int notone = wtf->getInt(1, 4, 0);
    if (notone != 1) map_array[(cobj.y-1)  * MAP_WIDTH + cobj.x].bloodyt += 3;
    if (notone != 2) map_array[(cobj.y+1)  * MAP_WIDTH + cobj.x].bloodyt += 3;
    if (notone != 3) map_array[cobj.y  * MAP_WIDTH + (cobj.x+1)].bloodyt += 3;
    if (notone != 4) map_array[cobj.y  * MAP_WIDTH + (cobj.x-1)].bloodyt += 3; // cross

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
                sdir = wtf->getInt(1, 3, 0);
                std::cout << "splat sdir(1-3): " << sdir << std::endl;
                map_array[fly_blood_fly_h(-1, 0, (cobj.y*MAP_WIDTH+cobj.x), sdir)]. bloodyt += blood;
                break;
            case 2:
                xdir = wtf->getInt(1, 3, 0);
                ydir = wtf->getInt(1, 3, 0);
                std::cout << "splat xdir(1-3): " << xdir << " splat ydir(1-3): " << ydir << std::endl;
                map_array[fly_blood_fly_d(-1, 1, (cobj.y*MAP_WIDTH+cobj.x), ydir, xdir)]. bloodyt += blood;
                break;
            case 3:
                sdir = wtf->getInt(1, 3, 0);
                std::cout << "splat sdir(1-3): " << sdir << std::endl;
                map_array[fly_blood_fly_h(0, 1, (cobj.y*MAP_WIDTH+cobj.x), sdir)]. bloodyt += blood;
                break;
            case 4: 
                xdir = wtf->getInt(1, 3, 0);
                ydir = wtf->getInt(1, 3, 0);
                std::cout << "splat xdir(1-3): " << xdir << " splat ydir(1-3): " << ydir << std::endl;
                map_array[fly_blood_fly_d(1, 1, (cobj.y*MAP_WIDTH+cobj.x), ydir, xdir)]. bloodyt += blood;
                break;
            case 5:
                sdir = wtf->getInt(1, 3, 0);
                std::cout << "splat sdir(1-3): " << sdir << std::endl;
                map_array[fly_blood_fly_h(1, 0, (cobj.y*MAP_WIDTH+cobj.x), sdir)]. bloodyt += blood;
                break;
            case 6:
                xdir = wtf->getInt(1, 3, 0);
                ydir = wtf->getInt(1, 3, 0);
                std::cout << "splat xdir(1-3): " << xdir << " splat ydir(1-3): " << ydir << std::endl;
                map_array[fly_blood_fly_d(1, -1, (cobj.y*MAP_WIDTH+cobj.x), ydir, xdir)]. bloodyt += blood;
                break;
            case 7:
                sdir = wtf->getInt(1, 3, 0);
                std::cout << "splat sdir(1-3): " << sdir << std::endl;
                map_array[fly_blood_fly_h(0, -1, (cobj.y*MAP_WIDTH+cobj.x), sdir)]. bloodyt += blood;
                break;
            case 8:
                xdir = wtf->getInt(1, 3, 0);
                ydir = wtf->getInt(1, 3, 0);
                std::cout << "splat xdir(1-3): " << xdir << " splat ydir(1-3): " << ydir << std::endl;
                map_array[fly_blood_fly_d(-1, -1, (cobj.y*MAP_WIDTH+cobj.x), ydir, xdir)]. bloodyt += blood;
                break;
            default:
                std::cout << "splat sdir(none): " << dir << std::endl;
                break;
        }
        --fly;
    }
}

short int bloodycount = 0;
int game_state = 0;
int player_action = 0;

void player_move_attack(int dx, int dy){
    
    int x = player.x + dx;
    int y = player.y + dy;

    //Object *target;
     unsigned   int target = 0;
     bool is_it = false;
    
    for (unsigned int i = 0; i<monvector.size(); ++i){
        if (monvector[i].x == x && monvector[i].y == y){
            //*target = &monvector[i];
            target = i;
            is_it = true;
        }
    }

    if (is_it){
        if (player.attack(monvector[target])){
                //monvector.erase (target); 
                --killall;
                bloodsplat(monvector[target]);
                bloodycount = 5;
                monvector[target].x=-365;
                monvector[target].y=-365; // teleport out of map   
                player.move(0, 0, monvector); // updates player position so feet get bloody
                fov_recompute = true;
                render_all(myvector);
                TCODConsole::flush();
            }
            else {
                TCODConsole::root->clear();
                mesg->setAlignment(TCOD_LEFT);
                mesg->setDefaultForeground(TCODColor::yellow);
                mesg->setDefaultBackground(TCODColor::black);
                monvector[target].colorb = TCODColor::red;
                monvector[target].draw(1);
                mesg->print(1, 1, "Hit!");
                if(!(player.y > MAP_HEIGHT-8 )) TCODConsole::blit(mesg,0,0,33,3,con,1,MAP_HEIGHT-4);
                else TCODConsole::blit(mesg,0,0,33,3,con,MAP_WIDTH-37,1);
                TCODConsole::blit(con,0,0,win_x,win_y,TCODConsole::root,0,0);
                TCODConsole::flush();
                Sleep(200); // shitty way for attack "animation", uses windows.h
                monvector[target].colorb = color_dark_ground;
                monvector[target].draw(0);
                con->clear();
                fov_recompute = true;
                render_all(myvector);
                TCODConsole::flush();
            }
        std::cout << "The " << monvector[target].name << " laughs!" << std::endl;
    } else { 
        //std::cout << "Tdsfsdsadfsdfsdfhe "  << std::endl;
        player.move(dx, dy, monvector);
        fov_recompute = true;
    }    
}

int handle_keys(Object &duh) {

    bool mycase_p;
    mycase_p = 0;
    
    TCOD_key_t key = TCODConsole::waitForKeypress(true); 

    if (bloodycount < 0) bloodycount = 0; // if ... change color 
  
    if ( key.c == 'q' ) return quit;

    if ( key.c == 'r' ){

        std::cout << " Monster array: " << monvector.size() << std::endl;
        unsigned int b = monvector.size();
        for (unsigned int i = 0; i < b; ++i) monvector.erase (monvector.begin()+i); // erase monster vector on map regen
        std::cout << " Monster array: " << monvector.size() << std::endl; // 0

        //Sleep(4000);
        make_map(duh);
        duh.bloody = 0;
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
                //map_array[row * MAP_WIDTH + l] = Tile(1,1);
            }
        }
        fov_recompute = true;
        set_black();
    }

    if (game_state == playing) {

    if ( key.c == 'p' ){
        TCODConsole::root->clear();
        mesg->setAlignment(TCOD_LEFT);
        mesg->setDefaultForeground(TCODColor::white);
        mesg->setDefaultBackground(TCODColor::black);
        mesg->print(1, 1, "Give a direction to dig dungeon");
        myvector[1]->draw(0);
       
        if(!(duh.y > MAP_HEIGHT-8 )) TCODConsole::blit(mesg,0,0,33,3,con,1,MAP_HEIGHT-4);
        else TCODConsole::blit(mesg,0,0,33,3,con,MAP_WIDTH-37,1);

        TCODConsole::blit(con,0,0,win_x,win_y,TCODConsole::root,0,0);

        TCODConsole::flush();
        while (!mycase_p){
            TCODConsole::waitForKeypress(true);
            if (TCODConsole::isKeyPressed(TCODK_UP)){ 
                std::cout << "Dig up." << std::endl; mycase_p = 1;
                map_array[(duh.y - 1)*MAP_WIDTH +duh.x].blocked = 0;
                map_array[(duh.y - 1)*MAP_WIDTH +duh.x].block_sight = 0;
                mesg->clear();
                for (int i = 0; i < MAP_HEIGHT ;++i){
                    for (int l = 0; l < MAP_WIDTH ;++l) {
                            fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                            l].blocked));
                            //map_array[row * MAP_WIDTH + l] = Tile(1,1);
                    }
                }
                fov_recompute = true;
                return false;
            }
            if (TCODConsole::isKeyPressed(TCODK_DOWN)){ 
                std::cout << "Dig down." << std::endl;mycase_p = 1; 
                map_array[(duh.y + 1)*MAP_WIDTH +duh.x].blocked = 0;
                map_array[(duh.y + 1)*MAP_WIDTH +duh.x].block_sight = 0;
                mesg->clear();
                for (int i = 0; i < MAP_HEIGHT ;++i){
                    for (int l = 0; l < MAP_WIDTH ;++l) {
                        fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
                        //map_array[row * MAP_WIDTH + l] = Tile(1,1);
                    }
                }
                fov_recompute = true;
                return false; 
            }
            if (TCODConsole::isKeyPressed(TCODK_LEFT)){
                std::cout << "Dig left." << std::endl;mycase_p = 1;
                map_array[duh.y*MAP_WIDTH +(duh.x -1)].blocked = 0;
                map_array[duh.y*MAP_WIDTH +(duh.x -1)].block_sight = 0;
                mesg->clear();
                for (int i = 0; i < MAP_HEIGHT ;++i){
                    for (int l = 0; l < MAP_WIDTH ;++l) {
                        fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
                        //map_array[row * MAP_WIDTH + l] = Tile(1,1);
                    }
                }
                fov_recompute = true;
                return false;
            }
            if (TCODConsole::isKeyPressed(TCODK_RIGHT)){ 
                std::cout << "Dig right." << std::endl;mycase_p = 1;
                map_array[duh.y*MAP_WIDTH +(duh.x + 1)].blocked = 0;
                map_array[duh.y*MAP_WIDTH +(duh.x + 1)].block_sight = 0;
                mesg->clear();
                for (int i = 0; i < MAP_HEIGHT ;++i){
                    for (int l = 0; l < MAP_WIDTH ;++l) {
                        fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
                        //map_array[row * MAP_WIDTH + l] = Tile(1,1);
                    }
                }
                fov_recompute = true;
                return false;
            }
        }
    }

    // end of KEY DIG cycle

    if (TCODConsole::isKeyPressed(TCODK_UP)){
        --bloodycount;
        --duh.bloody;  
        player_move_attack(0, -1);
        std::cout << " Monster array: " << myvector.size() << std::endl;
    }

    // end KEY UP cycle

    else if (TCODConsole::isKeyPressed(TCODK_DOWN)){
        --bloodycount;
        --duh.bloody; 
        player_move_attack(0, 1);
    }

    // end KEY DOWN cycle

    else if (TCODConsole::isKeyPressed(TCODK_LEFT)){
        --bloodycount;
        --duh.bloody;   
        player_move_attack(-1, 0);
    }

    // end KEY LEFT cycle

    else if (TCODConsole::isKeyPressed(TCODK_RIGHT)){
        --bloodycount; 
        --duh.bloody; 
        player_move_attack(1, 0);
    }

    else return no_turn;
    
    // end KEY RIGHT cycle
    }
    std::cout << "player.x: " << duh.x << " player.y: " << duh.y << std::endl; 
    return 0;
}    

int main() {
        
    TCODConsole::setCustomFont("arial10x10.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE); 
    TCODSystem::setFps(LIMIT_FPS);
        
    myvector.push_back(&player);
    //myvector.push_back(&npc);
   
    make_map(player);

    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
        }
    } 

    player.colorb = con->getCharBackground(player.x, player.y);
    //npc.colorb = con->getCharBackground(npc.x, npc.y);

    player.colorb = color_dark_ground;
    //npc.colorb = color_dark_ground;

    fov_recompute = true;

    TCODConsole::initRoot(win_x, win_y, "windowname", false);

    game_state = playing;
    
    while (! TCODConsole::isWindowClosed()) {

        int player_action = 0;

        TCODConsole::root->clear();
        
        render_all(myvector);
        
        TCODConsole::root->setAlignment(TCOD_LEFT);
        //TCODConsole::root->setDefaultBackground(TCODColor::black);
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->print(1, win_y-4, "Use arrows to move");
        TCODConsole::root->print(1, win_y-3, "Press 'q' to quit");
        TCODConsole::root->setAlignment(TCOD_RIGHT);
        TCODConsole::root->print(win_x-2, win_y-4, "Press 'p' to punch walls");
        TCODConsole::root->print(win_x-2, win_y-3, "Press 'r' to regenerate layout");
        
        TCODConsole::root->setAlignment(TCOD_CENTER);
        TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_2,troll,color_light_ground);
        TCODConsole::setColorControl(TCOD_COLCTRL_3,orc,color_light_ground);
        if (killall > 0){
            TCODConsole::root->print(win_x/2,win_y-1,"%cKILL%c all the '%ctrolls%c' and '%corcs%c'",
                TCOD_COLCTRL_1,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_COLCTRL_STOP,TCOD_COLCTRL_3,TCOD_COLCTRL_STOP);
        }
        else TCODConsole::root->print(win_x/2,win_y-1,"%cALL KILLED!%c",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
       
        TCODConsole::flush(); // this updates the screen

        for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->clear();

        player_action = handle_keys(player);

        if (player_action == quit) break;
        if (game_state == playing && player_action != no_turn){
            for (unsigned int i = 0; i<monvector.size(); ++i); //std::cout << "The " << monvector[i].name << " growls!" << std::endl; 
        }
    }
    return 0;
}
