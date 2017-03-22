#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "libtcod.hpp"
#include <windows.h> // for Sleep() and not currently used
#include <random>
#include <string>
#include <memory>

#include "loader.h"
//#include "tilevalues.h"
#include "map16.h"
#include "rng.h" // dice(number of rolls, sides)
#include "fileops.h" // mob_enc template struct
#include "game.h"
#include "liventities.h"
#include "debug.h" // debugmsg()
#include "inventory.h"
#include "gui.h"
#include "screens.h"
#include "busywork.h"

#include "chargen.h"

#include "colors.h"
// #include <process.h> //used for threading?

// used in liventities (extern) and here
std::vector<msg_log> msg_log_list;
std::vector<msg_log_c> msg_log_context;

bool combat_mode = false;
bool is_handle_combat = false;

int imageinit = 1;
float whattime = 0;
//TCODImage *pix; // was used to "blank" the root

int mapmode = 0; // changing map routine on regeneration

bool revealdungeon = 0;

extern const int MAP_WIDTH; // used for close_mode in liventities.c
extern const int MAP_HEIGHT;
const int MAP_WIDTH = 220;
const int MAP_HEIGHT = 140;
// 16map 55 35

const int MAP_WIDTH_AREA = 110;
const int MAP_HEIGHT_AREA = 70;
// visible area

// HD = 960 540 120 65
int   win_x   = (MAP_WIDTH_AREA) + 15 +3; // window width in cells 128 1024
int   win_y   = (MAP_HEIGHT_AREA)+ 18 +3; // window height in cells 91 728
int   LIMIT_FPS = 30;

const int quit = 1;
const int quit2 = 99; // combat mode exit game
const int playing = 1;
const int no_turn = 2;
const int dead = 99;

int stance_pos = 1;
int stance_pos2 = 0; // for in sight widget_1

int release_button = 1; // set to check for mouse button release

int insto_rows = 3; // rows in in sight UI


//parameters for dungeon generator
int ROOM_MAX_SIZE = 18;
int ROOM_MIN_SIZE = 4;
int MAX_ROOMS = 30;
int MAX_ROOM_MONSTERS = 4;
unsigned int MAX_TOTAL_MONSTERS = 815;

TCOD_fov_algorithm_t FOV_ALGO = FOV_BASIC; //default FOV algorithm
bool FOV_LIGHT_WALLS = true;
int TORCH_RADIUS = 10;
int MON_RADIUS = 8; // used for monster FoV

TCODColor color_dark_wall(50, 50, 50);

TCODColor blood(255, 0, 0);
TCODColor blood1(200, 0, 0);
TCODColor blood2(160, 0, 0);
TCODColor blood3(120, 0, 0);
TCODColor blood4(100, 0, 0);
TCODColor door_c(222, 136, 0);
TCODColor feature_c(254, 197, 42);

TCODColor demake(0,146,170);

TCODColor demake_main_floor(154,174,182);
TCODColor demake_sub_floor(0,0,0);  
TCODColor demake_main_wall(219,85,97);
TCODColor demake_sub_wall(0,0,0);

TCODColor color_bdark_wall(219, 85, 97);
TCODColor color_adark_wall(0, 0, 0);

TCODColor color_door_main(222, 136, 0);
TCODColor color_door_sub(0, 0, 0);

TCODColor color_light_wall(219, 85, 97);

TCODColor color_light_ground(50, 50, 150);
TCODColor color_light_ground2(154, 174, 182);
TCODColor color_dark_groundF(TCODColor::darkGrey);
TCODColor color_dark_wallF(0, 0, 50);
//TCODColor color_light_ground(200, 180, 50);
//
TCODColor minimap_floor(94,104,166);

TCODColor nullcolor(39,40,34);

TCODColor stdgrey(203, 203, 203);

TCODColor orc(0, 200, 0);
TCODColor troll(0, 255, 0);
TCODColor monsterdead(TCODColor::lightGrey);

TCODConsole *con_mini = new TCODConsole(MAP_WIDTH_AREA+2, MAP_HEIGHT_AREA+2); // both minimaps
//TCODConsole *con_tini = new TCODConsole(MAP_WIDTH_AREA+2, MAP_HEIGHT_AREA+2); // for tinymap

TCODConsole *load = new TCODConsole(win_x, win_y);  // load screen

TCODConsole *widget_top = new TCODConsole(win_x, 1);  // UI topbar
TCODConsole *widget_prompt = new TCODConsole(win_x, 1);  // UI prompt

TCODConsole *widget_popup = new TCODConsole(100, 50);  // UI popup

TCODConsole *widget_help = new TCODConsole(100, 50);  // UI help popup

TCODConsole *widget_1 = new TCODConsole(4, 1);  // UI numbers mapmodes
TCODConsole *widget_2 = new TCODConsole(8, 1);  // UI top widget for objects in sight
TCODConsole *widget_2_p = new TCODConsole(80, MAP_HEIGHT-10); // UI pop up object widget

TCODConsole *panel = new TCODConsole(win_x, (win_y - MAP_HEIGHT_AREA));  // combat UI panel (includes Message Log)
TCODConsole *panel_xtd = new TCODConsole(win_x-32, (win_y - MAP_HEIGHT_AREA)+60);  // Message Log extended
TCODConsole *r_panel = new TCODConsole((win_x - MAP_WIDTH_AREA), MAP_HEIGHT_AREA-3); // panel on right of map 
// 30, 46 message log
int BAR_WIDTH = 20;
int MSG_X = 30; // BAR_WIDTH + 2;
int MSG_WIDTH = 93; // SCREEN_WIDTH - BAR_WIDTH - 2 | was 63!
unsigned int MSG_HEIGHT = 15;// PANEL_HEIGHT - 1 | was 12!
bool MSG_MODE_XTD = false; // flag for extended msg log panel

// UI panel toggles
int wid_top_open = 0; // is top widget open?
int wid_combat_open = 0;
int wid_help = 0;
int wid_rpanel_open = 0;
bool wid_prompt_open = false; // player combat prompt panel
int prompt_selection = 0; // which option is selected
int fetch = 0; // returns player command during combat

int event_sender = -1;

/*
TCODMap * fov_map_mons = new TCODMap(MAP_WIDTH,MAP_HEIGHT);

TCODMap * fov_map_mons_path0 = new TCODMap(MAP_WIDTH,MAP_HEIGHT);
TCODMap * fov_map_mons_path1 = new TCODMap(MAP_WIDTH,MAP_HEIGHT);
*/

std::vector<unsigned int> mon_list;


void map_8x16_font(){

    for (int n = 0; n <= 100; n++){
        int step = 0;
        step = 2*n;
        TCODConsole::mapAsciiCodeToFont(1001+step,1,0+step);
        TCODConsole::mapAsciiCodeToFont(1001+(step+1),1,0+(step+1));
    }
}

void map_c64_font(){

    for (int n = 0; n <= 100; n++){
        int step = 0;
        step = 2*n;
        TCODConsole::mapAsciiCodeToFont(1301+step,3,0+step);
        TCODConsole::mapAsciiCodeToFont(1301+(step+1),3,0+(step+1));
        // second row
        TCODConsole::mapAsciiCodeToFont(1601+step,4,0+step);
        TCODConsole::mapAsciiCodeToFont(1601+(step+1),4,0+(step+1));
    }
}

// linemax set from 0 to last useful + 1
int print_8x16(TCODConsole* &loc, int where_x, int where_y, 
        const char *msg, TCODColor front, TCODColor back, int linemax = 0){

    loc->setDefaultForeground(front);
    loc->setDefaultBackground(back);
    unsigned int i = 0;
    unsigned int inner = 0; 
    bool endline = false;
    int line_n = 0; // lines printed
    if(linemax == 0) linemax = win_x; // use the max line length 
    unsigned int curmax = linemax;

    while(!endline){
        bool skip = false;
        inner = i; // sets the beginning of current line to print below
        while(i <= curmax){
            if(msg[i] == '%' && msg[i+1] == 't'){ // paragraph break
                skip = true;
                curmax = i;
                ++i; // skip character
            } else{
                if(msg[i] == '\0'){ 
                    endline = true; // found EOL
                    curmax = i;
                }    
            }  
            ++i;
            if(endline == true) break;
        }   
        if(!skip){
            if(!endline){
                while(msg[curmax] != ' '){
                    --curmax;
                    if (curmax == 0) break;
                } 
            }
        }    

        int linestart = 0;
        for (unsigned int n = inner; n < curmax; ++n){
            int letter = 0;
            letter = int(msg[n]);
            int step = 0;
            step = 2*(letter - 32);
            loc->putChar(where_x + linestart, where_y + line_n, 1001+step, TCOD_BKGND_SET);
            loc->putChar(where_x + linestart, where_y+1 + line_n, 1001+(step+1), TCOD_BKGND_SET);
            ++linestart;
        }
        i = curmax + 1; // sets on the beginning of next line
        curmax = i + linemax; // sets length for the next line
        if(!endline) line_n += 2;
        if(skip) {line_n += 2; ++i; ++i;}
    }
    return (where_y + 2 + line_n);
}

void print_c64(TCODConsole* &loc, int where_x, int where_y, const char *msg, TCODColor front, TCODColor back){

    loc->setDefaultForeground(front);
    loc->setDefaultBackground(back);
    int msg_length = 0;
    for (int i = 0; msg[i] != '\0'; i++){
        msg_length = i;
    }
    
    for (int n = 0; n <= msg_length; ++n){
        int letter = 0;
        letter = int(msg[n]);
        int step = 0;
        step = 2*(letter - 32);
        // 936 + 65 = 1001
        //step = step +2; 
        loc->putChar((where_x + n)*2, where_y, 1301+step, TCOD_BKGND_SET);
        loc->putChar((where_x + n)*2, where_y + 1, 1301+(step+1), TCOD_BKGND_SET);
        loc->putChar((where_x + n)*2+1, where_y, 1601+step, TCOD_BKGND_SET);
        loc->putChar((where_x + n)*2+1, where_y + 1, 1601+(step+1), TCOD_BKGND_SET);
    }
}

std::vector<Tile> map_array; // declared here because used in Object.moved

class Rect {
public:
    int x1;
    int x2;
    int y1;
    int y2;
    int center_x;
    int center_y;
    bool special;
    bool needcol;
    bool reround; // for BSP tress, make room after BSP

    Rect(int x, int y, int w, int h){
        x1 = x;
        y1 = y;
        x2 = x + w;
        y2 = y + h;
        center_x = (x1 + x2) / 2;
        center_y = (y1 + y2) / 2;
        special = false;
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

void create_room(Rect &inroom){
    
    inroom.reround = false;
    inroom.needcol = false;
    for (int i = inroom.y1 ; i <= inroom.y2; i++){
        for (int l = inroom.x1 ; l <= inroom.x2; l++) {
            map_array[i * MAP_WIDTH + l] = Tile(0,0);
        }
    }
}

void create_round_room2(Rect &inroom){

    inroom.needcol = false; 
    inroom.reround = true;
    
    map_array[inroom.center_y * MAP_WIDTH + inroom.center_x] = Tile(0,0);
    map_array[(inroom.center_y-1) * MAP_WIDTH + (inroom.center_x)] = Tile(0,0);
    map_array[(inroom.center_y+1) * MAP_WIDTH + (inroom.center_x)] = Tile(0,0);
    map_array[(inroom.center_y) * MAP_WIDTH + (inroom.center_x-1)] = Tile(0,0);
    map_array[(inroom.center_y) * MAP_WIDTH + (inroom.center_x+1)] = Tile(0,0);
}

void create_round_room(Rect &inroom){

    inroom.special = true;
    int radius = 0;
    int width = inroom.x2 - inroom.x1;
    int height = inroom.y2 - inroom.y1;

    radius = std::min(width, height) / 2;
    if (radius > 3) inroom.needcol = true;
    else inroom.needcol = false;
    
    for (int i = inroom.y1; i <= inroom.y2; i++){
        for (int l = inroom.x1; l <= inroom.x2; l++) {
            if (sqrt( pow((l - inroom.center_x),2) + pow((i - inroom.center_y),2) ) <= radius){
                map_array[i * MAP_WIDTH + l] = Tile(0,0);
            }    
        }
    }
}

void create_column_room(Rect &inroom){

    inroom.special = true;
    inroom.needcol = false;
    inroom.center_x = inroom.x1 + 6;
    inroom.center_y = inroom.y1 + 3;
    
    for (int i = inroom.y1 ; i < inroom.y1+7; ++i){
        for (int l = inroom.x1 ; l < inroom.x1+12; ++l) {
            map_array[i * MAP_WIDTH + l] = Tile(0,0);
        }
    }

    //for (int col = 0; col < 5; ++col){
        
    //}
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

std::vector<Object_player*> myvector; // player vector object

Fighter fighter_component(30, 2, 5, 8); // hp, defense, power, speed

//Object_player playera(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5, fighter_component);
//Object_player playerb(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5, fighter_component);

Object_player player(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5, fighter_component);
//strcpy(player.name, "Playername");
//strcpy(monster.name, "orc");

std::vector<Object_monster> monvector; // MAIN MONSTER VECTOR

bool is_blocked(int x, int y){
    if (map_array[y*MAP_WIDTH+x].blocked) return true;

    for (unsigned int i = 0; i<monvector.size(); ++i){
        if (monvector[i].blocks == true && monvector[i].x == x && monvector[i].y == y){
            std::cout << "Monster in the way." << std::endl;
            return true;
        }
    }

    // prevent monsters moving atop the player
    if(player.x == x && player.y == y) return true;    

    return false;
}

bool iis_blocked(int x, int y){ // was used by object_monster.move
    if (map_array[y*MAP_WIDTH+x].blocked) return true;

    for (unsigned int i = 0; i<monvector.size(); ++i){
        if (monvector[i].blocks == true && monvector[i].x == x && monvector[i].y == y){
            std::cout << "Monster in the way." << std::endl;
            return true;
        }
    }
         return false;
}

void Object_monster::move(int dx, int dy, bool p_dir) {
     
    // sets facing
    if (dy != 0){
        if (dy == -1) facing = 0;
        else facing = 2;
    }  
    if (dx != 0){
        if (dx == -1) facing = 3;
        else facing = 1;
    }

    //std::cout << "BEFORE" << std::endl;
    
    TCODPath *path;
    if(path_mode == 1) path = path1;
    else path = path0;

    stuck = false;

    if(!path->isEmpty()){
        std::cout << "Move path is: " << path->size() << std::endl;
    } else {
        std::cout << "Move path is Empty." << std::endl;
        stuck = true; 
        return;
    }

    //std::cout << "MID" << std::endl;

    int newx,newy;
    path->get(0,&newx,&newy);

    if (!is_blocked(newx,newy)){
        if (path->walk(&newx, &newy, true)) {
            if( (newx != x) || (newy != y) ){ 
                move_counter++; // increment move counter, even out of combat tho
                if(move_counter == (combat_move_max/2)){
                    std::cout << "PMONSTER " << combat_move_max/2 << std::endl;
                    ++cflag_attacks; // number of attack each turn affect skill rolls
                    move_counter = 0;
                }    
            }    
            x = newx;
            y = newy;
        }    
    } else stuck = true;

        //std::cout << "AFTER" << std::endl;
        
        //if (path->walk(&newx,&newy,true)) {
        //    if (!is_blocked(newx,newy)){
        ////        x = newx;
        //        y = newy;
        //    } else stuck = true;
        //}    
}

void Object_player::move(int dx, int dy, std::vector<Object_monster> smonvector) {
       
        int tempx = 0;
        int tempy = 0;

        // check for actual movement happening
        bool override = false;
        if( (dx == 0) && (dy == 0) ) override = true;

        tempx = x + dx;
        tempy = y + dy;

        if (!is_blocked(tempx,tempy)){
            x += dx;
            y += dy;
            if(!override){
                move_counter++; // only add used movement point if actually moved
                if(move_counter == 4){
                    cflag_attacks++;
                    move_counter = 0;
                }    
            }  
            // if player bloodier than tile, tile get more blood
            if (bloody > 0){
                if (bloody >= map_array[y * MAP_WIDTH + x].bloodyt)
                map_array[y * MAP_WIDTH + x].bloodyt = bloody;
            }
        } else ; //std::cout << "Fuck, it's blocked. " << std::endl;

        if (x >= MAP_WIDTH) {x = MAP_WIDTH-1;std::cout << "No, I'm not stepping into the void." << std::endl;}
        if (y >= MAP_HEIGHT) {y = MAP_HEIGHT-1;std::cout << "No, I'm not stepping into the void." << std::endl;}
        if (x <= 0) {x = 0;std::cout << "No, I'm not stepping into the void." << std::endl;}
        if (y <= 0) {y = 0;std::cout << "No, I'm not stepping into the void." << std::endl;}

        bloody = (map_array[y * MAP_WIDTH + x].bloodyt);
        if(!override){ // only decrease blood on tile if actually moved
            if (map_array[y * MAP_WIDTH + x].bloodyt > 1) --map_array[y * MAP_WIDTH + x].bloodyt;
        }
}

BasicMonster orc_ai;


void place_objects(Rect room, lvl1 myenc, Game &GAME){

    if (monvector.size() >= MAX_TOTAL_MONSTERS) return ;

    // 1 every 4 rooms has monsters
    int skip = rng(0, 3);
    if(skip == 3){

        int rollenc = rng(0,100);
        int bottomenc = 0;
        int num_mosters = 0;
        //std::cout << "myenc.cave1.size: " << myenc.cave1.size() << std::endl;
        for (unsigned u = 0; u < myenc.cave1.size(); u++){
            //std::cout << "u: " << u << std::endl;
            //std::cout << "types: " << myenc.vmob_types.size() << std::endl;
            if(rollenc < myenc.cave1[u].probability && rollenc > bottomenc){
                num_mosters = myenc.cave1[u].enc.size();   
                std::cout << "enc type: " << u << std::endl;

                int x, y;

                // make monster object + figter component
                Fighter fighter_component(0, 0, 0, 0);
                Object_monster monster(0, 0, 'i', TCODColor::white, TCODColor::black, 0, fighter_component);

                for (int i = 0; i < num_mosters; ++i){
                    x = rng((room.x1+1), (room.x2-1));
                    y = rng((room.y1+1), (room.y2-1));
                    int overrider = 0;
                    while( is_blocked(x,y) ){
                        x = rng((room.x1+1), (room.x2-1));
                        y = rng((room.y1+1), (room.y2-1));
                        overrider++;
                        if (overrider > 50) return; // maybe the room too small, so stop trying
                    }    
                    monster.x = x;
                    monster.y = y;
                    Fighter fighter_component(myenc.vmob_types[myenc.cave1[u].enc[i]].s_hp,
                        myenc.vmob_types[myenc.cave1[u].enc[i]].s_defense,
                        myenc.vmob_types[myenc.cave1[u].enc[i]].s_power,
                        myenc.vmob_types[myenc.cave1[u].enc[i]].s_speed); // hp, defense, power, speed
                    monster.stats = fighter_component;
                    monster.myai = &orc_ai;
                    monster.selfchar = myenc.vmob_types[myenc.cave1[u].enc[i]].selfchar;
                    monster.self_8 = myenc.vmob_types[myenc.cave1[u].enc[i]].self_8;
                    monster.self_16 = myenc.vmob_types[myenc.cave1[u].enc[i]].self_16;
                    monster.color = myenc.vmob_types[myenc.cave1[u].enc[i]].color;
                    monster.colorb = TCODColor::black;
                    monster.h = myenc.vmob_types[myenc.cave1[u].enc[i]].h;
                    monster.blocks = true;
                    strcpy(monster.name, myenc.vmob_types[myenc.cave1[u].enc[i]].name);
                    std::cout << "name: " << monster.name << std::endl;
                    monster.alive = true;
                    monster.chasing = false;
                    monster.stuck = 0;
                    monster.wasstuck = 0;
                    monster.bored = 500;
                    monster.boren = false;
                    monster.in_sight = false;
                    monster.path_mode = 0;
                    monster.combat_move = myenc.vmob_types[myenc.cave1[u].enc[i]].combat_move;
                    monster.combat_move_max = monster.combat_move;
                    monster.c_mode = false;
                    monster.speed = myenc.vmob_types[myenc.cave1[u].enc[i]].speed;
                    monster.STR = myenc.vmob_types[myenc.cave1[u].enc[i]].STR;
                    monster.hit = false;

                    monster.facing = rng(0,3); // random facing
                    monster.initML = rng(35, 65);

                    monster.phase = 3; // hardcoded at average

                    monster.overpower_l = 0;

                    monster.stats.wpn1.wpn_AC =     myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_AC;
                    monster.stats.wpn1.wpn_DC =     myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_DC;
                    monster.stats.wpn1.wpn_B =      myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_B;
                    monster.stats.wpn1.wpn_aspect = myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_aspect;
                    monster.stats.wpn1.reach =      myenc.vmob_types[myenc.cave1[u].enc[i]].reach;

                    monster.stats.wpn1.wp_B.first = myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_B;
                    monster.stats.wpn1.wp_B.second = 0;
                    monster.stats.wpn1.wp_E.first = myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_E;
                    monster.stats.wpn1.wp_E.second = 1;
                    monster.stats.wpn1.wp_P.first = myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_P;
                    monster.stats.wpn1.wp_P.second = 2;

                    monster.armor_tot.clear();
                    for(unsigned int n = 0; n < 16; ++n){
                        armorsection armortemp;
                        armortemp.B = myenc.vmob_types[myenc.cave1[u].enc[i]].armor;
                        armortemp.E = myenc.vmob_types[myenc.cave1[u].enc[i]].armor;
                        armortemp.P = myenc.vmob_types[myenc.cave1[u].enc[i]].armor;
                        monster.armor_tot.push_back(armortemp);
                    } 

                    monster.stats.ML = myenc.vmob_types[myenc.cave1[u].enc[i]].ML;

                    Generic_object item;
                    strcpy(item.name, "objectname");
                    item.glyph_8 = 'K';
                    monster.inventory.push_back(item);

                    monster.path0 = new TCODPath(GAME.gstate.fov_map_mons_path0, 0.0f);
                    monster.path1 = new TCODPath(GAME.gstate.fov_map_mons_path1, 0.0f);
                    monvector.push_back(monster);
                }
            }
            bottomenc = myenc.cave1[u].probability;
        }   
    }  
}

int killall = 0; // monster count

int init = 0;

class Miner {

public:
    int x;
    int y;
    int lifetime;

    Miner(int initx, int inity, int initlifetime){
        x = initx;
        y = inity;
        lifetime = initlifetime;
    }

};

class Door {

public:
    int x;
    int y;
    int lifetime;

    Door(int initx, int inity, int initlifetime){
        x = initx;
        y = inity;
        lifetime = initlifetime;
    }

};
std::vector<Door> doors;

void place_objects2(Rect room){

    if (monvector.size() >= MAX_TOTAL_MONSTERS) return ;


}    

void make_map2(Object_player &duh){

    map_array.resize(MAP_HEIGHT * MAP_WIDTH);
    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
           map_array[i * MAP_WIDTH + l] = Tile(1,1);
        }
    } // fills map with walls

    Miner dig1(110, 70, 0);
    std::vector<Miner> diggers;
    diggers.push_back(dig1);

    TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

    int num1 = 0;
    
    map_array[70 * MAP_WIDTH + 110] = Tile(0,0);

    for (int i = 0; i < 2260; i++){
        for (unsigned int l = 0; l < diggers.size(); l++){
            num1 = wtf->getInt(1, 4, 0); 
            switch(num1){
                case 1:
                    diggers[l].y = diggers[l].y - 1;
                    if(diggers[l].y < 2) diggers[l].y = 2;
                    if(diggers[l].y > (MAP_HEIGHT-2)) diggers[l].y = (MAP_HEIGHT-2);
                    map_array[diggers[l].y * MAP_WIDTH + diggers[l].x] = Tile(0,0);
                    break;
                case 2:
                    diggers[l].x = diggers[l].x + 1;
                    if(diggers[l].x < 2) diggers[l].x = 2;
                    if(diggers[l].x > (MAP_WIDTH-2)) diggers[l].x = (MAP_WIDTH-2);
                    map_array[diggers[l].y * MAP_WIDTH + diggers[l].x] = Tile(0,0);
                    break;
                case 3:
                    diggers[l].y = diggers[l].y + 1;
                    if(diggers[l].y < 2) diggers[l].y = 2;
                    if(diggers[l].y > (MAP_HEIGHT-2)) diggers[l].y = (MAP_HEIGHT-2);
                    map_array[diggers[l].y * MAP_WIDTH + diggers[l].x] = Tile(0,0);
                    break;
                case 4:
                    diggers[l].x = diggers[l].x - 1;
                    if(diggers[l].x < 2) diggers[l].x = 2;
                    if(diggers[l].x > (MAP_WIDTH-2)) diggers[l].x = (MAP_WIDTH-2);
                    map_array[diggers[l].y * MAP_WIDTH + diggers[l].x] = Tile(0,0);
                    break;
            }
            int spawn = 0;
            spawn = wtf->getInt(1, 50, 0);
            if(spawn > 20 && diggers.size() < 10){
                Miner dig2(diggers[l].x, diggers[l].y, 0);
                diggers.push_back(dig2);
            }
        }    
    }  

    unsigned int max_mon;
    max_mon = wtf->getInt(8, 16, 0); // sets number of monsters

    while (monvector.size() < max_mon){
        Object_monster monster(0, 0, 'i', TCODColor::black, TCODColor::black, 0, fighter_component);
        int x = 0;
        int y = 0;
        int switche = 0;
        switche = wtf->getInt(1,2,0);
        if (switche > 1){
            x = wtf->getInt(1, 80, 0);
        } else {
            x = wtf->getInt(150, 219, 0);                
        }   
        switche = wtf->getInt(1,2,0);
        if (switche > 1){
            y = wtf->getInt(1, 50, 0);
        } else {
            y = wtf->getInt(90, 139, 0);                
        }
        if(!(is_blocked(x,y))){
            if ( wtf->getInt(0, 100, 0) < 80){
            Fighter fighter_component(10, 0, 3, 2); // hp, defense, power, speed
            monster.x = x;
            monster.y = y;
            monster.selfchar= 'o';
            monster.color = orc;
            monster.colorb = TCODColor::black;
            monster.h = 4;
            monster.blocks = true;
            //monster.name[] = "orc";
            strcpy(monster.name, "Orc");
            monster.stats = fighter_component;
            monster.myai = &orc_ai;
            monster.alive = true;
            monster.chasing = false;
            monster.stuck = 0;
            monster.bored = 500;
            monster.boren = false;
            monster.in_sight = false;
            monster.path_mode = 0;
            monster.combat_move = 6;
            monster.c_mode = false;
            monster.speed = 4; // for initiative
            monster.hit = false;
            monvector.push_back(monster);
            } else {
                Fighter fighter_component(12, 1, 4, 4); // hp, defense, power, speed
            monster.x = x;
            monster.y = y;
            monster.selfchar= 'T';
            monster.color = troll;
            monster.colorb = TCODColor::black;
            monster.h = 5;  
            monster.blocks = true;
            //monster.name[] = "troll";
            strcpy(monster.name, "Troll");
            monster.stats = fighter_component;
            monster.myai = &orc_ai;
            monster.alive = true;
            monster.chasing = false;
            monster.stuck = 0;
            monster.bored = 500;
            monster.boren = false;
            monster.in_sight = false;
            monster.path_mode = 0;
            monster.combat_move = 10;
            monster.c_mode = false;
            monster.speed = 8;
            monster.hit = false;
            monvector.push_back(monster);
            }
        }

    }

    duh.x = 110; // player set position
    duh.y = 70;
    killall = monvector.size();
}

void place_doors(Rect inroom){

    if(!inroom.special){

    for (int i = inroom.y1-1; i <= inroom.y2+1; ++i){ // +1 so it looks to walls too
        for (int l = inroom.x1-1; l <= inroom.x2+1; ++l){

            Door door1(l, i, 0);
            
            if(rng(1, 10) > 5){

            if(!map_array[i * MAP_WIDTH + l].blocked){ // if cell is free
                if(map_array[i * MAP_WIDTH + (l-1)].blocked && map_array[i * MAP_WIDTH + (l+1)].blocked){ // H walls
                    if(!map_array[(i+1) * MAP_WIDTH + l].blocked && // S
                        !map_array[(i+1) * MAP_WIDTH + (l-1)].blocked && // SW
                        !map_array[(i+2) * MAP_WIDTH + l].blocked && // 2S
                        !map_array[(i+2) * MAP_WIDTH + (l-1)].blocked ){ // 2SW 
                        
                        doors.push_back(door1); // creates the door
                        map_array[i * MAP_WIDTH + l] = Tile(0,1); // makes cell block LOS
                    } else if(!map_array[(i+1) * MAP_WIDTH + l].blocked && // S
                        !map_array[(i+1) * MAP_WIDTH + (l+1)].blocked && // SE
                        !map_array[(i+2) * MAP_WIDTH + l].blocked && // 2S
                        !map_array[(i+2) * MAP_WIDTH + (l+1)].blocked){ // 2SE
                        
                        doors.push_back(door1);
                        map_array[i * MAP_WIDTH + l] = Tile(0,1);
                    } else if(!map_array[(i-1) * MAP_WIDTH + l].blocked && // N
                        !map_array[(i-1) * MAP_WIDTH + (l-1)].blocked && // NW
                        !map_array[(i-2) * MAP_WIDTH + l].blocked && // 2N
                        !map_array[(i-2) * MAP_WIDTH + (l-1)].blocked ){ // 2NW 
                        
                        doors.push_back(door1);
                        map_array[i * MAP_WIDTH + l] = Tile(0,1);
                    } else if(!map_array[(i-1) * MAP_WIDTH + l].blocked && // N
                        !map_array[(i-1) * MAP_WIDTH + (l+1)].blocked && // NE
                        !map_array[(i-2) * MAP_WIDTH + l].blocked && // 2N
                        !map_array[(i-2) * MAP_WIDTH + (l+1)].blocked){ // 2NE
                        
                        doors.push_back(door1);
                        map_array[i * MAP_WIDTH + l] = Tile(0,1);
                    }
                } else if(map_array[(i-1) * MAP_WIDTH + l].blocked && map_array[(i+1) * MAP_WIDTH + l].blocked){ // V walls
                    if(!map_array[i * MAP_WIDTH + (l+1)].blocked && // E
                        !map_array[(i+1) * MAP_WIDTH + (l+1)].blocked && // SE
                        !map_array[i * MAP_WIDTH + (l+2)].blocked && // 2S
                        !map_array[(i+1) * MAP_WIDTH + (l+2)].blocked ){ // 2SE 
                        
                        doors.push_back(door1);
                        map_array[i * MAP_WIDTH + l] = Tile(0,1);
                    } else if(!map_array[i * MAP_WIDTH + (l+1)].blocked && // E
                        !map_array[(i-1) * MAP_WIDTH + (l+1)].blocked && // NE
                        !map_array[i * MAP_WIDTH + (l+2)].blocked && // 2E
                        !map_array[(i-1) * MAP_WIDTH + (l+2)].blocked){ // 2NE
                        
                        doors.push_back(door1);
                        map_array[i * MAP_WIDTH + l] = Tile(0,1);
                    } else if(!map_array[i * MAP_WIDTH + (l-1)].blocked && // W
                        !map_array[(i+1) * MAP_WIDTH + (l-1)].blocked && // SW
                        !map_array[i * MAP_WIDTH + (l-2)].blocked && // 2W
                        !map_array[(i+1) * MAP_WIDTH + (l-2)].blocked ){ // 2SW 
                        
                        doors.push_back(door1);
                        map_array[i * MAP_WIDTH + l] = Tile(0,1);
                    } else if(!map_array[i * MAP_WIDTH + (l-1)].blocked && // W
                        !map_array[(i-1) * MAP_WIDTH + (l-1)].blocked && // NW
                        !map_array[i * MAP_WIDTH + (l-2)].blocked && // 2W
                        !map_array[(i-1) * MAP_WIDTH + (l-2)].blocked){ // 2NW
                        
                        doors.push_back(door1);
                        map_array[i * MAP_WIDTH + l] = Tile(0,1);
                    }    
                }
            }
            }
        }
    }
    } // inroom.special

}

void place_columns(Rect inroom){
    map_array[(inroom.y1+1) * MAP_WIDTH + (inroom.x1 + 2)] = Tile(1,1);
    map_array[(inroom.y1+1) * MAP_WIDTH + (inroom.x1 + 4)] = Tile(1,1);
    map_array[(inroom.y1+1) * MAP_WIDTH + (inroom.x1 + 6)] = Tile(1,1);
    map_array[(inroom.y1+1) * MAP_WIDTH + (inroom.x1 + 8)] = Tile(1,1);
    map_array[(inroom.y1+1) * MAP_WIDTH + (inroom.x1 + 10)] = Tile(1,1);

    map_array[(inroom.y1+5) * MAP_WIDTH + (inroom.x1 + 2)] = Tile(1,1);
    map_array[(inroom.y1+5) * MAP_WIDTH + (inroom.x1 + 4)] = Tile(1,1);
    map_array[(inroom.y1+5) * MAP_WIDTH + (inroom.x1 + 6)] = Tile(1,1);
    map_array[(inroom.y1+5) * MAP_WIDTH + (inroom.x1 + 8)] = Tile(1,1);
    map_array[(inroom.y1+5) * MAP_WIDTH + (inroom.x1 + 10)] = Tile(1,1);
}

void place_column(Rect inroom){

    if(inroom.needcol){
    map_array[(inroom.center_y) * MAP_WIDTH + (inroom.center_x)] = Tile(1,1);
    map_array[(inroom.center_y+1) * MAP_WIDTH + (inroom.center_x)] = Tile(1,1);
    map_array[(inroom.center_y-1) * MAP_WIDTH + (inroom.center_x)] = Tile(1,1);
    map_array[(inroom.center_y) * MAP_WIDTH + (inroom.center_x+1)] = Tile(1,1);
    map_array[(inroom.center_y) * MAP_WIDTH + (inroom.center_x-1)] = Tile(1,1);
    }

}

void vline(int x, int y1, int y2) {
	int y=y1;
	int dy=(y1>y2?-1:1);
    map_array[y * MAP_WIDTH + x] = Tile(0,0);
	if ( y1 == y2 ) return;
	do {
		y+=dy;
		map_array[y * MAP_WIDTH + x] = Tile(0,0);
	} while (y!=y2);
}


// draw a vertical line up until we reach an empty space
void vline_up(int x, int y) {
	while (y >= 0 && map_array[y * MAP_WIDTH + x].blocked) {
		map_array[y * MAP_WIDTH + x] = Tile(0,0);
		y--;
	}
}

// draw a vertical line down until we reach an empty space
void vline_down(int x, int y) {
	while (y < MAP_HEIGHT && map_array[y * MAP_WIDTH + x].blocked) {
		map_array[y * MAP_WIDTH + x] = Tile(0,0);
		y++;
	}
}

// draw a horizontal line
void hline(int x1, int y, int x2) {
	int x=x1;
	int dx=(x1>x2?-1:1);
	map_array[y * MAP_WIDTH + x] = Tile(0,0);
	if ( x1 == x2 ) return;
	do {
		x+=dx;
		map_array[y * MAP_WIDTH + x] = Tile(0,0);
	} while (x!=x2);
}

// draw a horizontal line left until we reach an empty space
void hline_left(int x, int y) {
	while (x >= 0 && map_array[y * MAP_WIDTH + x].blocked) {
		map_array[y * MAP_WIDTH + x] = Tile(0,0);
		x--;
	}
}

// draw a horizontal line right until we reach an empty space
void hline_right(int x, int y) {
	while (x < MAP_WIDTH && map_array[y * MAP_WIDTH + x].blocked) {
		map_array[y * MAP_WIDTH + x] = Tile(0,0);
		x++;
	}
}

int roomcount = 0;
std::vector<Rect> BSProoms;

class MyCallback : public ITCODBspCallback {
public :
    bool visitNode(TCODBsp *node, void *userData) {
        

        if (node->isLeaf()){
            
            int minx = node->x+1; // x+1
			int maxx = node->x+node->w-1; // from x to width -1
			int miny = node->y+1;
			int maxy = node->y+node->h-1;

            //if ( minx > 1 ) minx--;
			//if ( miny > 1 ) miny--; 
            if (maxx == MAP_WIDTH-1 ) maxx--;
			if (maxy == MAP_HEIGHT-1 ) maxy--; 
            minx = rng(minx,maxx-ROOM_MIN_SIZE+1);
			miny = rng(miny,maxy-ROOM_MIN_SIZE+1);
            if( (maxx - minx) > ROOM_MAX_SIZE ) maxx = minx + ROOM_MAX_SIZE;
            if( (maxy - miny) > ROOM_MAX_SIZE ) maxy = miny + ROOM_MAX_SIZE;
			maxx = rng(minx+ROOM_MIN_SIZE-1,maxx);
			maxy = rng(miny+ROOM_MIN_SIZE-1,maxy);
            // ratio
            if((maxx-minx)*3 < (maxy-miny)) maxy = miny+(maxx-minx)*3;
            if((maxy-miny)*3 < (maxx-minx)) maxx = minx+(maxy-miny)*3;

            node->x=minx;
			node->y=miny;
			node->w=maxx-minx+1;
			node->h=maxy-miny+1;
            //Rect new_room(minx, miny, node->w, node->h);
            Rect new_room(minx, miny, maxx-minx, maxy-miny);
            /* for (int x=minx; x <= maxx; x++ ) {
				for (int y=miny; y <= maxy; y++ ) {
					map_array[y * MAP_WIDTH + x] = Tile(0,0);
				}
			}*/
            int round = 0;
            round = rng(0, 5);
            int radius = 0;
            radius = std::min(node->w, node->h);
            if (round == 5 && radius > 4){ 
                
                // - 1 ; 2
                node->x=new_room.center_x-1;
                node->y=new_room.center_y-1;
                node->w=2;
			    node->h=2;
                create_round_room2(new_room);
                //printf("node pos x%d y%d ",new_room.center_x, new_room.center_y);
            }  else { 
                create_room(new_room);
            }
            BSProoms.push_back(new_room);

            if (BSProoms.size() == 1){
                player.x = new_room.center_x;
                player.y = new_room.center_y; // new player coordinates from room 0
            }    

            //for (int i = miny ; i < maxy; ++i){
            //    for (int l = minx; l < maxx; ++l) {
            //        map_array[i * MAP_WIDTH + l] = Tile(0,0);
            //    }
            //}
        } else {
            //printf("lvl %d %dx%d %dx%d\n",node->level, node->x,node->y,node->w,node->h);
			// resize the node to fit its sons
			TCODBsp *left=node->getLeft();
			TCODBsp *right=node->getRight();
			node->x=MIN(left->x,right->x);
			node->y=MIN(left->y,right->y);
			node->w=MAX(left->x+left->w,right->x+right->w)-node->x;
			node->h=MAX(left->y+left->h,right->y+right->h)-node->y;
			// create a corridor between the two lower nodes
			if (node->horizontal) {
				// vertical corridor
				if ( left->x+left->w -1 < right->x || right->x+right->w-1 < left->x ) {
					// no overlapping zone. we need a Z shaped corridor
					int x1=rng(left->x,left->x+left->w-1);
					int x2=rng(right->x,right->x+right->w-1);
					int y=rng(left->y+left->h,right->y);
					vline_up(x1,y-1);
					hline(x1,y,x2);
					vline_down(x2,y+1);
				} else {
					// straight vertical corridor
					int minx=MAX(left->x,right->x);
					int maxx=MIN(left->x+left->w-1,right->x+right->w-1);
					int x=rng(minx,maxx);
					vline_down(x,right->y);
					vline_up(x,right->y-1);
				}
			} else {
				// horizontal corridor
				if ( left->y+left->h -1 < right->y || right->y+right->h-1 < left->y ) {
					// no overlapping zone. we need a Z shaped corridor
					int y1=rng(left->y,left->y+left->h-1);
					int y2=rng(right->y,right->y+right->h-1);
					int x=rng(left->x+left->w,right->x);
					hline_left(x-1,y1);
					vline(x,y1,y2);
					hline_right(x+1,y2);
				} else {
					// straight horizontal corridor
					int miny=MAX(left->y,right->y);
					int maxy=MIN(left->y+left->h-1,right->y+right->h-1);
					int y=rng(miny,maxy);
					hline_left(right->x-1,y);
					hline_right(right->x,y);
				}
			}
		}    
        return true;
    }
};


void make_map_BSP(Object_player &duh, Game &GAME){

    map_array.resize(MAP_HEIGHT * MAP_WIDTH);
     
    // fill map with walls [1,1]
    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
           map_array[i * MAP_WIDTH + l] = Tile(1,1);
        }
    } 

    TCODRandom *randtcod = new TCODRandom(130);
    TCODBsp *myBSP = new TCODBsp(30,30, MAP_WIDTH-60, MAP_HEIGHT-60);
    myBSP->splitRecursive(randtcod,6,ROOM_MIN_SIZE+1,ROOM_MIN_SIZE+1,1.5f,1.5f);
    //myBSP->splitRecursive(NULL,17,3,2,1.5f,1.5f);
    delete randtcod;
    
    myBSP->traverseInvertedLevelOrder(new MyCallback(),NULL);

    // load monster values?
    lvl1 myenc; 
    load_stuff(myenc);
    std::cout << "encounters: " << myenc.cave1.size() << std::endl;
    std::cout << "monster types: " << myenc.vmob_types.size() << std::endl;
    std::cout << "monster1 ML: " << myenc.vmob_types[0].ML << std::endl;
    std::cout << "monster1 name: " << myenc.vmob_types[0].name << std::endl;
    std::cout << "monster2 name: " << myenc.vmob_types[1].name << std::endl;

    //LEAK
    
    for (unsigned int i = 0; i<BSProoms.size(); ++i){
        if(BSProoms[i].reround) create_round_room(BSProoms[i]);
        place_doors(BSProoms[i]);
        place_column(BSProoms[i]);
        if(i > 5) place_objects(BSProoms[i], myenc, GAME); // don't place monsters on player start (within 5 rooms)
    }
    
    delete myBSP;

    // trying floor variation
    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            if(!map_array[i * MAP_WIDTH + l].blocked)
                map_array[i * MAP_WIDTH + l].feature = rng(1,4);
        }
    }

    killall = monvector.size(); // how many monsters on map
    while(map_array[player.y * MAP_WIDTH + player.x].blocked){
        player.x++;
        player.y++;
    }    
}  

void load_map(Object_player &duh, Game &GAME){
   

    lvl1_map map1;
    load_level(map1);
    // loader.cpp > 

    map_array.resize(MAP_HEIGHT * MAP_WIDTH);
     
    // fill map with walls [1,1]
    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
           map_array[i * MAP_WIDTH + l] = Tile(1,1);
        }
    }
    
    for (int i = 0; i < map1.max_y ;++i){
        for (int l = 0; l < map1.max_x ;++l) {
            if(map1.map_int[i * map1.max_x + l] == 1)
                map_array[i * MAP_WIDTH + l] = Tile(1,1);
            else map_array[i * MAP_WIDTH + l] = Tile(0,0);
        }
    }

    player.x = 7;
    player.y = 7;

    //Feature thisfeature(8, 11, 0);
    //GAME.gstate.features.push_back(thisfeature);
    load_features(GAME);
}

void make_map(Object_player &duh){

    bool throne = false;
    bool doorescape = false;
    int column = 0;
           
    map_array.resize(MAP_HEIGHT * MAP_WIDTH);
     
    // fill map with walls [1,1]
    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
           map_array[i * MAP_WIDTH + l] = Tile(1,1);
           //map_array[3597] = Tile(1,1);
        }
    } // fills map with walls 

    std::vector<Rect> rooms;
    int num_rooms = 0;

    int prev_x = 0;
    int prev_y = 0;

    int cycle = 0;
    bool secd = false;

    for (int r = 0; r < MAX_ROOMS; ++r){

        /// I think this makes execute animation every 2 frames
        if ( r%2 == 0 ) secd = true; else secd = false;
        
        if (init && secd){
            load->setAlignment(TCOD_CENTER);

            if (cycle == 0){
                load->print(win_x/2, (win_y/2)-1, "%c%c ",TCOD_CHAR_NW,TCOD_CHAR_HLINE);
                load->print(win_x/2, win_y/2,     " %c ", TCOD_CHAR_CHECKBOX_UNSET);
                load->print(win_x/2, (win_y/2)+1, "   ");
                load->print(win_x/2, (win_y/2)-3, "Generating new level");

            } else if (cycle == 1){ 
                load->print(win_x/2, (win_y/2)-1, " %c%c",TCOD_CHAR_HLINE,TCOD_CHAR_NE);
                load->print(win_x/2, win_y/2,     " %c ", TCOD_CHAR_CHECKBOX_UNSET);
                load->print(win_x/2, (win_y/2)+1, "   ");
                load->print(win_x/2, (win_y/2)-3, "Generating new level");
                           
            } else if (cycle == 2){
                load->print(win_x/2, (win_y/2)-1, "  %c",TCOD_CHAR_NE);
                load->print(win_x/2, win_y/2,     " %c%c", TCOD_CHAR_CHECKBOX_UNSET,TCOD_CHAR_VLINE);
                load->print(win_x/2, (win_y/2)+1, "   ");
                load->print(win_x/2, (win_y/2)-3, "Generating new level");
               
            } else if (cycle == 3){
                load->print(win_x/2, (win_y/2)-1, "   ");
                load->print(win_x/2, win_y/2,     " %c%c", TCOD_CHAR_CHECKBOX_UNSET,TCOD_CHAR_VLINE);
                load->print(win_x/2, (win_y/2)+1, "  %c",TCOD_CHAR_SE);
                load->print(win_x/2, (win_y/2)-3, "Generating new level");
                
            } else if (cycle == 4){
                load->print(win_x/2, (win_y/2)-1, "   ");
                load->print(win_x/2, win_y/2,     " %c ", TCOD_CHAR_CHECKBOX_UNSET);
                load->print(win_x/2, (win_y/2)+1, " %c%c",TCOD_CHAR_HLINE,TCOD_CHAR_SE);
                load->print(win_x/2, (win_y/2)-3, "Generating new level");
               
            } else if (cycle == 5){
                load->print(win_x/2, (win_y/2)-1, "   ");
                load->print(win_x/2, win_y/2,     " %c ", TCOD_CHAR_CHECKBOX_UNSET);
                load->print(win_x/2, (win_y/2)+1, "%c%c ",TCOD_CHAR_SW,TCOD_CHAR_HLINE);
                load->print(win_x/2, (win_y/2)-3, "Generating new level");
               
            } else if (cycle == 6){
                load->print(win_x/2, (win_y/2)-1, "   ");
                load->print(win_x/2, win_y/2,     "%c%c ",TCOD_CHAR_VLINE, TCOD_CHAR_CHECKBOX_UNSET);
                load->print(win_x/2, (win_y/2)+1, "%c  ",TCOD_CHAR_SW);
                load->print(win_x/2, (win_y/2)-3, "Generating new level");
              
            } else if (cycle == 7){
                load->print(win_x/2, (win_y/2)-1, "%c  ",TCOD_CHAR_NW,TCOD_CHAR_HLINE);
                load->print(win_x/2, win_y/2,     "%c%c ",TCOD_CHAR_VLINE, TCOD_CHAR_CHECKBOX_UNSET);
                load->print(win_x/2, (win_y/2)+1, "   ");
                load->print(win_x/2, (win_y/2)-3, "Generating new level");
            
            }    
            
        TCODConsole::blit(load,0,0,0,0,TCODConsole::root,0,0);
        TCODConsole::flush(); // this updates the screen
        ++cycle;
        if (cycle >= 7) cycle = 0;
        }

        std::cout << "num_rooms: " << num_rooms << std::endl;
        std::cout << "Rooms array: " << rooms.size() << std::endl;

        TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

        //random width and height
        int w = 0;
        w = wtf->getInt(ROOM_MIN_SIZE, ROOM_MAX_SIZE, 0);
        int h = 0;
        h = wtf->getInt(ROOM_MIN_SIZE, ROOM_MAX_SIZE, 0);

        //random position without going out of the boundaries of the map
        int x = 0;
        x = wtf->getInt( 0, MAP_WIDTH - w - 1, 0);
        int y = 0;
        y = wtf->getInt( 0, MAP_HEIGHT - h - 1, 0);

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
            
            if (!throne && (w > 14 && h > 6)) {
                create_column_room(new_room); 
                throne = true; 
                doorescape = true;
            }
            else {
                doorescape = false;
                int round = 0;
                round = wtf->getInt( 0, 5, 0);
                if (round == 5){ 
                    create_round_room(new_room);
                }    
                else { 
                    create_room(new_room);
                }
            }    
            
            if (num_rooms == 0){
                duh.x = new_room.center_x;
                duh.y = new_room.center_y; // new player coordinates from room 0
            } else {
               // npc.x = new_room.center_x;
               // npc.y = new_room.center_y; // new npc coordinates from whatever room
             
                // TEMP!!!!
                lvl1 myenc; 
                load_stuff(myenc);
                //place_objects(new_room, myenc); // only add monsters if not first room
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
            if(doorescape) column = rooms.size()-1;
        }
    }

    for (unsigned int i = 0; i<rooms.size(); ++i){
        place_doors(rooms[i]);
        place_column(rooms[i]);
    }

    place_columns(rooms[column]);

    killall = monvector.size();
    ++init;
}

void set_black(Game &tgame){
    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            tgame.gstate.con->putChar(l, i, ' ', TCOD_BKGND_SET);
            tgame.gstate.con->setCharBackground(l, i, TCODColor::black, TCOD_BKGND_SET);
            tgame.gstate.con->setCharForeground(l, i, TCODColor::black);
        }
    } 
} // fill "con" with black

// find if there's a monster alive at x,y coordinates, returns -1 if none found
int monster_this(int x, int y, const std::vector<Object_monster> &monsters){
    for (unsigned int i = 0; i<monsters.size(); ++i){ 
        if (monsters[i].x == x && monsters[i].y == y){
            if(monsters[i].alive){ // found a monster in path
                return i;
            }
        }    
    }
    return -1;
}    

void set_black_b(Game &tgame){
    for (int i = 0; i < (MAP_HEIGHT*2) ;++i){
        for (int l = 0; l < (MAP_WIDTH*2) ;++l) {
            tgame.gstate.con->putChar(l, i, ' ', TCOD_BKGND_SET);
            tgame.gstate.con->setCharBackground(l, i, TCODColor::black, TCOD_BKGND_SET);
            tgame.gstate.con->setCharForeground(l, i, TCODColor::black);
        }
    } 
} // fill "con" with black

TCOD_key_t keyz;
TCOD_mouse_t mousez;
TCOD_event_t ev;

void givemescreenxy(int &x, int &y, Game &tgame){
    if (!tgame.gstate.bigg){
        x = x - tgame.gstate.off_xx;
        y = y - tgame.gstate.off_yy;
    } else if (tgame.gstate.bigg){   
        x = (x - tgame.gstate.off_xx - 28)*2;
        y = (y - tgame.gstate.off_yy - 18)*2;
    }
}   

void givememapxy(int &x, int &y, Game &tgame){
    if (!tgame.gstate.bigg){
        x = x + tgame.gstate.off_xx;
        y = y + tgame.gstate.off_yy;
        if(x >= MAP_WIDTH || y >= MAP_HEIGHT) x = -1, y = -1; // off map bounds
    } else if (tgame.gstate.bigg){  
        x = ((x/2) + tgame.gstate.off_xx)+28;
        y = ((y/2) + tgame.gstate.off_yy)+18;
        if(x >= MAP_WIDTH || y >= MAP_HEIGHT) x = -1, y = -1; // off map bounds
    }
}

// mapx, mapy used to print text
void overlay(int who, int mapx, int mapy, int realx, int realy, bool bigg){
    int facing = 0;
    if (who == -1){ // player
        facing = player.facing;
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::setColorControl(TCOD_COLCTRL_3,TCODColor::lighterYellow,TCODColor::black);
        TCODConsole::root->print(0, 71, "Mouse on [Player] at [%c%d%c.%c%d%c]", 
                TCOD_COLCTRL_3, mapx, TCOD_COLCTRL_STOP, TCOD_COLCTRL_3, mapy, TCOD_COLCTRL_STOP);
    } else { // all other monsters alive
        facing = monvector[who].facing;
        char *whatis;
        TCODColor col_obj; // color of object
        whatis = &(monvector[who].name[0]);
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->setAlignment(TCOD_LEFT);
        col_obj = monvector[who].color;
        TCODConsole::setColorControl(TCOD_COLCTRL_3,TCODColor::lighterYellow,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_1,col_obj,TCODColor::black);
        TCODConsole::root->print(0, 71, "Mouse on [%c%s%c] at [%c%d%c.%c%d%c]",
                TCOD_COLCTRL_1, whatis, TCOD_COLCTRL_STOP, 
                TCOD_COLCTRL_3, mapx, TCOD_COLCTRL_STOP, TCOD_COLCTRL_3, mapy, TCOD_COLCTRL_STOP);
    }
    // highlight selection (not facing)
    if(!bigg){
        TCODConsole::root->setCharForeground(realx, realy, TCODColor::TCODColor::darkGrey);
        TCODConsole::root->setCharBackground(realx, realy, TCODColor::white, TCOD_BKGND_SET);
    } else if (bigg){
        if (realx % 2 == 1) realx -= 1;
        if (realy % 2 == 1) realy -= 1;
        TCODConsole::root->setCharForeground(realx, realy, TCODColor::darkGrey);
        TCODConsole::root->setCharBackground(realx, realy, TCODColor::white, TCOD_BKGND_SET);
        TCODConsole::root->setCharForeground(realx+1, realy, TCODColor::darkGrey);
        TCODConsole::root->setCharBackground(realx+1, realy, TCODColor::white, TCOD_BKGND_SET);
        TCODConsole::root->setCharForeground(realx, realy+1, TCODColor::darkGrey);
        TCODConsole::root->setCharBackground(realx, realy+1, TCODColor::white, TCOD_BKGND_SET);
        TCODConsole::root->setCharForeground(realx+1, realy+1, TCODColor::darkGrey);
        TCODConsole::root->setCharBackground(realx+1, realy+1, TCODColor::white, TCOD_BKGND_SET);
    }    
    int casebig = 0;
    if (!bigg) casebig = 1;
    else casebig = 2;
    switch (facing){
        case 0:
            realy -= casebig;
            break;
        case 1:
            realx += casebig;
            break;
        case 2:
            realy += casebig;
            break;
        case 3:
            realx -= casebig;
            break;
        default:
            g_debugmsg("Broken facing: %d", facing);
            break;
    } 
    if (!bigg){
        TCODConsole::root->setDefaultForeground(TCODColor::yellow);
        TCODConsole::root->putChar(realx, realy, 178, TCOD_BKGND_SET);
    } else {        
        if (realx % 2 == 1) realx -= 1;
        if (realy % 2 == 1) realy -= 1;
        TCODConsole::root->setDefaultForeground(TCODColor::yellow);
        TCODConsole::root->putChar(realx, realy, 178, TCOD_BKGND_SET);
        TCODConsole::root->putChar(realx+1, realy, 178, TCOD_BKGND_SET);
        TCODConsole::root->putChar(realx, realy+1, 178, TCOD_BKGND_SET);
        TCODConsole::root->putChar(realx+1, realy+1, 178, TCOD_BKGND_SET);
    }
    TCODConsole::root->setDefaultBackground(TCODColor::black);
}    

void I_am_moused(Game &tgame){
   
    mousez = TCODMouse::getStatus();
    int x = mousez.cx;
    int y = mousez.cy;

    char *whatis; // monster namestring

    if(!release_button){
        if(!mousez.lbutton){
            release_button = 1;
        }

    } else {    

        // extended msg log
        if(mousez.lbutton && y == 73 && (x == 33 || x == 34)) {
            if(MSG_MODE_XTD){ 
                MSG_MODE_XTD = 0 ; 
                UI_unhook(tgame, 3);
            } else {
                MSG_MODE_XTD = 1; 
                UI_hook(tgame, 3);
            }
            release_button = 0;
        }    
        
        if(mousez.lbutton && y == 0 && (x == 0 || x == 1 || x == 2)) {
            if(wid_top_open){ 
                wid_top_open=0;
                UI_unhook(tgame, 0); 
            } else {
                wid_top_open = 1;
                UI_hook(tgame, 0);
            }
            release_button = 0;
        }
        

        if(mousez.lbutton && y > (win_y-2) && x > (win_x-4) ) {
            if(wid_combat_open){ 
                wid_combat_open=0; 
                UI_unhook(tgame, 2);
            } else {
            wid_combat_open = 1; 
            UI_hook(tgame, 2);
            }
        release_button = 0;
        }

        if( (mousez.lbutton && y == 3 && x == 127) ||
                (mousez.lbutton && y == 3 && x == 126) ||
                (mousez.lbutton && y == 3 && x == 125) ) {
            if(wid_rpanel_open){ 
                wid_rpanel_open=0; 
                UI_unhook(tgame, 1);
            } else {
            wid_rpanel_open = 1;
            UI_hook(tgame, 1);
            }
        release_button = 0;
        } 
    
    // MAP MODES TOPBAR
    if(wid_top_open){
        if(mousez.lbutton && y == 0 && (x >= 92 && x <= 94)) {
            wid_help = 1;
        }
        if(mousez.lbutton_pressed && (y == 0 && x == 27) ) { // set ranged mode or melee
            fetch = switchweapon(tgame, combat_mode); // in inventory.h 
            release_button = 0;
        }
        if(mousez.lbutton && x == 4 && y == 0) {
            if(tgame.gstate.mapmode != 1){
                stance_pos = 1; 
                tgame.gstate.bigg = 0; 
                tgame.gstate.fov_recompute = true;
                tgame.gstate.mapmode = 1;
            }
        }
        if(mousez.lbutton && x == 5 && y == 0) {
            if(tgame.gstate.mapmode != 2){
                stance_pos = 2; 
                tgame.gstate.bigg = 1; 
                tgame.gstate.fov_recompute = true;
                tgame.gstate.mapmode = 2;
            }
        }
        if(x == 6 && y == 0) {
            if(tgame.gstate.mapmode != 3){
                tgame.gstate.bigg2 = 1;
                tgame.gstate.fov_recompute = true;
                tgame.gstate.mapmode = 3;
            }
        } else {
            if(tgame.gstate.mapmode == 3){
                tgame.gstate.bigg2 = 0;
                tgame.gstate.fov_recompute = true;
                if(stance_pos == 1) {tgame.gstate.mapmode = 1; tgame.gstate.bigg = 0;}
                else {tgame.gstate.mapmode = 2; tgame.gstate.bigg = 1;}
            }
            tgame.gstate.bigg2 = 0;
        }
        if(x == 7 && y == 0) {
            if(tgame.gstate.mapmode != 4){
                tgame.gstate.bigg3 = 1;
                tgame.gstate.fov_recompute = true;
                tgame.gstate.mapmode = 4;
            }
        } else {
            if(tgame.gstate.mapmode == 4){
                tgame.gstate.bigg3 = 0;
                tgame.gstate.fov_recompute = true;
                if(stance_pos == 1) {tgame.gstate.mapmode = 1; tgame.gstate.bigg = 0;}
                else {tgame.gstate.mapmode = 2; tgame.gstate.bigg = 1;}
            }
            tgame.gstate.bigg3 = 0;
        }
        if( (x >= 10 && x < 18) && y == 0) {
            stance_pos2 = 1;
        } else {
            stance_pos2 = 0;
        }
    }
    if(wid_help){
        if(mousez.lbutton && y == 10 && (x >= 92 && x <= 95)) {
            wid_help = 0;
        }    
    }    
    } // release button 

    //std::cout << "MOUSE " << x << " " << y << std::endl;

    bool found = false;
    TCODColor col_obj; // color of object

    // find map coordinates
    int mapx = 0;
    int mapy = 0;
    if (!tgame.gstate.bigg){
        mapx = x + tgame.gstate.off_xx;
        mapy = y + tgame.gstate.off_yy;
        if(mapx >= MAP_WIDTH || mapy >= MAP_HEIGHT) mapx = -1, mapy = -1; // off map bounds
    } else if (tgame.gstate.bigg){    
        mapx = ((x/2) + tgame.gstate.off_xx)+28;
        mapy = ((y/2) + tgame.gstate.off_yy)+18;
        if(mapx >= MAP_WIDTH || mapy >= MAP_HEIGHT) mapx = -1, mapy = -1; // off map bounds
    }

    for (auto i : tgame.gstate.UI_hook) {
        if( (x >= i.x && x <= (i.x + i.w)) && (y >= i.y && y <= (i.y + i.h) ) ){
            mapx = -1;
            mapy = -1;
            if (i.ID == 1 && mon_list.size() > 0){ // initiative list
                int list_h = mon_list.size();
                if (y-6 >= 0 && y-6 < list_h){
                    for(int p = 0; p < 17; ++p){
                        TCODColor fore;
                        TCODColor back;
                        fore = TCODConsole::root->getCharBackground(110+p, y);
                        back = TCODConsole::root->getCharForeground(110+p, y);
                        TCODConsole::root->setCharForeground(110+p, y, fore);
                        TCODConsole::root->setCharBackground(110+p, y, back, TCOD_BKGND_SET);
                    }    
                    if (mon_list[y-6] == 255){
                        int rx = player.x;
                        int ry = player.y;
                        givemescreenxy(rx, ry, tgame);
                        overlay(-1, player.x, player.y, rx, ry, tgame.gstate.bigg);
                        
                    } else if (mon_list[y-6] != 256){ 
                        int xx = monvector[mon_list[y-6]].x;
                        int yy = monvector[mon_list[y-6]].y;
                        int rx = xx;
                        int ry = yy;
                        givemescreenxy(rx, ry, tgame);
                        overlay(mon_list[y-6], xx, yy, rx, ry, tgame.gstate.bigg);
                    }    
                    found = true;
                }    
            } else if(i.ID == 0 && (x == 24 && y == 0) && mousez.lbutton){
                char_sheet(tgame); // screens.cpp
            } else if(i.ID == 3){
                if(y == 12 && (x >= 44 && x <= 53) ){
                    if(mousez.lbutton_pressed) tgame.gstate.MLfilter101 = !tgame.gstate.MLfilter101;
                }    
            } else if(i.ID == 4){ // combat prompt
                //int in_x = x - i.x; // sets mouse origin to the panel
                int in_y = y - i.y; 
                if(!tgame.gstate.mode_move && !tgame.gstate.mode_attack){
                    if(in_y == 3) prompt_selection = 1; // to send event to draw menu function
                    else if(in_y == 2) prompt_selection = 4; // pass
                    else if(in_y == 4) prompt_selection = 2;
                    else if(in_y == 5) prompt_selection = 3;
                    else if(in_y == 6) prompt_selection = 5; // aim
                    else if(in_y == 7) prompt_selection = 6; // switch target
                    else prompt_selection = 0;
                    if(mousez.lbutton_pressed){
                        if(in_y == 3 && tgame.gstate.first) fetch = 1;
                        else if(in_y == 2 && tgame.gstate.second) fetch = 4; // pass
                        else if(in_y == 4 && tgame.gstate.second) fetch = 2;
                        else if(in_y == 5 && tgame.gstate.third) fetch = 3;
                        else if(in_y == 6 && tgame.player->rangeweapon){ // aim
                            if(tgame.player->aim != tgame.player->skill.bowML) fetch = 5; // only if not filled
                        } else if(in_y == 7 && tgame.player->rangeweapon && 
                                tgame.player->ranged_target != -2) fetch = 6;  
                                // excluded tgame.player->rangeaim >= 2 
                    }else fetch = 0; 
                } else if(tgame.gstate.mode_move){ // move mode
                    if(in_y == 2) prompt_selection = 1; 
                    else prompt_selection = 0;
                    if(mousez.lbutton_pressed){
                        if(in_y == 2 && tgame.gstate.first) fetch = 1;
                    }else fetch = 0;
                } else if(tgame.gstate.mode_attack){
                    if(in_y == 2) prompt_selection = 1; 
                    else prompt_selection = 0;
                    if(mousez.lbutton_pressed){
                        if(in_y == 2 && tgame.gstate.first) fetch = 1;
                    }else fetch = 0;
                }    
            }    
        }
    }    

    if(!tgame.gstate.modal){ // do overlays only if non modal (ranged aiming)
        if (mapx == player.x && mapy == player.y && !found){ // look for player
            found = true;
            overlay(-1, mapx, mapy, x, y, tgame.gstate.bigg);
        } else { // look for monsters
            for (unsigned int n = 0; n<monvector.size(); ++n){
                if( ((mapx == monvector[n].x && mapy == monvector[n].y && monvector[n].alive) 
                            && tgame.gstate.fov_map->isInFov(monvector[n].x,monvector[n].y)) && !found  ){
                    overlay(n, mapx, mapy, x, y, tgame.gstate.bigg); 
                    found = true;
                }
            }
            if (!found){ // only if no moster alive found
                for (unsigned int n = 0; n<monvector.size(); ++n) {
                    if( ((mapx == monvector[n].x && mapy == monvector[n].y && !(monvector[n].alive) ) 
                                && tgame.gstate.fov_map->isInFov(monvector[n].x,monvector[n].y)) && !found  ){
                        whatis = &(monvector[n].name[0]);
                        TCODConsole::root->setDefaultForeground(TCODColor::white);
                        TCODConsole::root->setAlignment(TCOD_LEFT);
                        col_obj = monvector[n].color;
                        TCODConsole::setColorControl(TCOD_COLCTRL_1,col_obj,TCODColor::black);
                        TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::lighterYellow,TCODColor::black);
                        TCODConsole::root->print(0, 70, "Mouse on [dead %c%s%c] at [%c%d%c.%c%d%c]",
                                TCOD_COLCTRL_1, whatis, TCOD_COLCTRL_STOP, 
                                TCOD_COLCTRL_2, mapx, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, mapy, TCOD_COLCTRL_STOP);
                        TCODConsole::root->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
                        found = true;
                    }
                }    
            }        
        }   
    }
    if (!found){
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::lighterYellow,TCODColor::black);
        TCODConsole::root->print(0, 69, "Mouse on [Nothing] at [%c%d%c.%c%d%c]", 
                TCOD_COLCTRL_2, x, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, y, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(0, 70, "MAP x,y [Nothing] at [%c%d%c.%c%d%c]", 
                TCOD_COLCTRL_2, mapx, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, mapy, TCOD_COLCTRL_STOP);
        TCODConsole::root->setDefaultBackground(TCODColor::black); // sets the screen as black
        found = false;
        tgame.gstate.mapx = mapx;
        tgame.gstate.mapy = mapy;
        tgame.gstate.mx = x;
        tgame.gstate.my = y;
    }        
}


// HP bar during combat
//def render_bar(x, y, total_width, name, value, maximum, bar_color, back_color):
void render_bar(int x, int y, int total_width, const char *name, 
        int value, int maximum, TCODColor bar_color, TCODColor back_color){

    int bar_width = int(float(value) / maximum * total_width);

    // moved first so that the background of text is properly working
    panel->setDefaultForeground(TCODColor::white);
    panel->setAlignment(TCOD_CENTER);
    panel->print((x + total_width) / 2, y, "%s: %d/%d",name , value, maximum);

    // fills bar with darkred
    panel->setDefaultBackground(back_color);
    panel->rect(x, y, total_width, 1, false,TCOD_BKGND_SET);

    // draws the active part in lightred 
    panel->setDefaultBackground(bar_color);
    if (bar_width > 0) 
        panel->rect(x, y, bar_width, 1, false,TCOD_BKGND_SET);
    
    panel->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black

}

void render_bar_s2(int x, int y, int total_width, const char *name, 
        int value, int maximum, int phase, TCODColor bar_color, TCODColor back_color, TCODColor label){

    panel->setDefaultForeground(back_color);

    panel->setDefaultForeground(TCODColor::lighterGrey);
    panel->putChar(x, y, '[');
    int tempmaximum = maximum;
    int index = 0;
    int ivalue = maximum - value;
    int temphase = phase; // used for print lower down
    for(unsigned int drop = tempmaximum; drop > 0; --drop){
        if(ivalue){
            panel->setDefaultForeground(TCODColor::lightRed);
            panel->putChar(x+index+1, y, '*');
            --ivalue;
        } else { 
            panel->setDefaultForeground(TCODColor::lighterGreen);
            panel->putChar(x+index+1, y, '+');
            if(phase == 0){ 
                panel->setDefaultForeground(TCODColor::lighterYellow);
                panel->putChar(x+index+1, y, '=');
            }    
            if(phase > 0) --phase;
        }
        panel->setDefaultForeground(TCODColor::lighterGrey);
        panel->putChar(x+index+2, y, '.');
        index += 2;
    } 
    panel->setDefaultForeground(TCODColor::grey);
    // 9 ap 3 used -> 9 maximum 6 value (left)
    if ((maximum - value) > 0) panel->putChar(x+(maximum-value)*2, y, '|'); // two dividers
    if (temphase > 0) panel->putChar(x+((maximum-value)+temphase)*2, y, '|');
    panel->setDefaultForeground(TCODColor::lighterGrey);
    panel->putChar(x+index, y, ']');
    panel->setDefaultForeground(TCODColor::lighterGreen);
    panel->print(x, y+1, "Phase left: %d", temphase);

    panel->setDefaultForeground(TCODColor::lighterGrey);
    TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::white, TCODColor::black);
    panel->setAlignment(TCOD_LEFT);
    if(value <= 0) TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightRed,TCODColor::black);
    else TCODConsole::setColorControl(TCOD_COLCTRL_1, label,TCODColor::black);
    panel->print(13, y-1, "AP: %c%d%c/%c%d%c", TCOD_COLCTRL_1, value, TCOD_COLCTRL_STOP, 
            TCOD_COLCTRL_2, maximum, TCOD_COLCTRL_STOP);
    panel->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
}

void Message_Log(Game &GAME){

    panel->setAlignment(TCOD_LEFT);

    int panel_offset = 0;
    TCODConsole *whatpanel;
    if(MSG_MODE_XTD){
        MSG_HEIGHT = 75; // 5 times the 15 standard
        whatpanel = panel_xtd;
        panel_offset = 0;

        whatpanel->setBackgroundFlag(TCOD_BKGND_SET);
        whatpanel->setDefaultForeground(TCODColor::lightGrey);
        whatpanel->print(0, 0, "Filters:");
        if(GAME.gstate.MLfilter101){
            whatpanel->setDefaultForeground(TCODColor::black);
            whatpanel->setDefaultBackground(TCODColor::lightYellow);
        } else {
            whatpanel->setDefaultForeground(TCODColor::lightYellow);
            whatpanel->setDefaultBackground(TCODColor::black);
        }
        whatpanel->print(10, 0, "Initiative");
        whatpanel->setDefaultForeground(TCODColor::white);
        whatpanel->setDefaultBackground(TCODColor::black);

        panel->setDefaultForeground(TCODColor::red);
        panel->putChar(33, 1, 25, TCOD_BKGND_SET); // expand button
        panel->setDefaultForeground(TCODColor::white);
    } else {
        MSG_HEIGHT = 15;
        whatpanel = panel;
        panel_offset = 34;
        panel->setDefaultForeground(TCODColor::red);
        panel->putChar(33, 1, 24, TCOD_BKGND_SET); // expand button
        panel->setDefaultForeground(TCODColor::white);
    }    

    if(msg_log_list.size() > 0){
        whatpanel->setDefaultForeground(stdlogc);
        whatpanel->setBackgroundFlag(TCOD_BKGND_SET);
        //whatpanel->print(panel_offset, 2, ">");
        /* 
        while(msg_log_list.size() > 80){
            msg_log_list.erase(msg_log_list.begin(),msg_log_list.begin()+1);
        }
        */
        //int i = msg_log_list.size() + 1
        int a = 2;
        int howmany = 0;
        int bump = 0; // add blank line past first message
        howmany = (msg_log_list.size())- MSG_HEIGHT;
        //std::cout << "Quantity " << howmany << std::endl;
        if(howmany < 0) howmany = 0;
        for(int i = (msg_log_list.size()-1); i >= howmany ; i--){
            TCODColor bck;
            if (!msg_log_list[i].c1) bck = TCODColor::black; else bck = msg_log_list[i].bcolor1;
            TCODConsole::setColorControl(TCOD_COLCTRL_1,msg_log_list[i].color1,bck);
            if (!msg_log_list[i].c2) bck = TCODColor::black; else bck = msg_log_list[i].bcolor2;
            TCODConsole::setColorControl(TCOD_COLCTRL_2,msg_log_list[i].color2,bck);
            if (!msg_log_list[i].c3) bck = TCODColor::black; else bck = msg_log_list[i].bcolor3;
            TCODConsole::setColorControl(TCOD_COLCTRL_3,msg_log_list[i].color3,bck);
            if (!msg_log_list[i].c4) bck = TCODColor::black; else bck = msg_log_list[i].bcolor4;
            TCODConsole::setColorControl(TCOD_COLCTRL_4,msg_log_list[i].color4,bck);
            if (!msg_log_list[i].c5) bck = TCODColor::black; else bck = msg_log_list[i].bcolor5;
            TCODConsole::setColorControl(TCOD_COLCTRL_5,msg_log_list[i].color5,bck);
            if(msg_log_list[i].filter == 101 && !GAME.gstate.MLfilter101){ // initiative
                --howmany; // add line availability, since it was skipped
            } else {
                whatpanel->print(panel_offset+1, a+bump, "%s", msg_log_list[i].message);
                a++;
                bump = 1;
            }    
        }
    } else {
        whatpanel->print(panel_offset, 2, ">Message Log currently empty");
    }
    // just draws frame (not on extended panel)
    for (int n = 0; n < 16; ++n){
        panel->setDefaultForeground(TCODColor::lighterGrey);
        panel->setDefaultBackground(TCODColor::black);
        panel->print(33, n+2, "%c", TCOD_CHAR_VLINE);
    }
}

void render_messagelog(Game &GAME){
    if(wid_combat_open){
            panel->clear();
            panel_xtd->clear();
            Message_Log(GAME);
            panel->setDefaultForeground(TCODColor::white);
            //panel->print(win_x-1, (win_y - MAP_HEIGHT_AREA)-4, "^");
            //panel->print(win_x-1, (win_y - MAP_HEIGHT_AREA)-3, "%c", TCOD_CHAR_SE);
            TCODConsole::setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_5,TCODColor::white,TCODColor::black);
            panel->print(win_x-3, (win_y - MAP_HEIGHT_AREA)-3, "%c[%c%c^%c%c]%c", TCOD_COLCTRL_5, TCOD_COLCTRL_STOP,
                    TCOD_COLCTRL_4, TCOD_COLCTRL_STOP, TCOD_COLCTRL_5, TCOD_COLCTRL_STOP);
            panel->setAlignment(TCOD_LEFT);
            //render_bar(1, 1, BAR_WIDTH, "HP", player.stats.hp, player.stats.max_hp, TCODColor::lightRed, TCODColor::darkerRed);
            if(player.stats.hp <= 5) TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightRed,TCODColor::black);
            else TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);
            panel->setDefaultForeground(TCODColor::lighterGrey);
            TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::white, TCODColor::black);
            panel->print(1, 1, "HP: %c%d%c/%c%d%c", TCOD_COLCTRL_1, player.stats.hp, TCOD_COLCTRL_STOP, 
                    TCOD_COLCTRL_2, player.stats.max_hp, TCOD_COLCTRL_STOP);        
            TCODColor mov_bar;
            if (player.AP < 4)
                mov_bar = TCODColor::red;
            else mov_bar = TCODColor::white;
            render_bar_s2(1, 2, BAR_WIDTH, "Mov", player.AP, player.APm, player.phaseAP,
                    TCODColor::lightPurple, TCODColor::darkerPurple, mov_bar);
            TCODConsole::blit(panel,0,0,0,0,TCODConsole::root,0,MAP_HEIGHT_AREA+2);
            if(MSG_MODE_XTD) TCODConsole::blit(panel_xtd,0,0,0,0,TCODConsole::root,34,(MAP_HEIGHT_AREA+2)-60);
        } else {
            //win_x, (win_y - MAP_HEIGHT_AREA
            //panel->setDefaultForeground(TCODColor::red);
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
            //panel->print(win_x-1, (win_y - MAP_HEIGHT_AREA)-2, "%c*%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            //panel->print(win_x-1, (win_y - MAP_HEIGHT_AREA)-1, "%c*%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            panel->print(win_x-3, (win_y - MAP_HEIGHT_AREA)-3, "%c[%c%c*%c%c]%c", TCOD_COLCTRL_5, TCOD_COLCTRL_STOP,
                    TCOD_COLCTRL_4, TCOD_COLCTRL_STOP, TCOD_COLCTRL_5, TCOD_COLCTRL_STOP);
            TCODConsole::blit(panel,win_x-3, (win_y - MAP_HEIGHT_AREA)-3,3,1,TCODConsole::root, win_x-3,win_y-1);
            //TCODConsole::blit(panel,win_x-2, (win_y - MAP_HEIGHT_AREA)-2,2,2,TCODConsole::root, win_x-2,win_y-2);
            //TCODConsole::blit(widget_top,0,0,1,1,TCODConsole::root,0,0);
            //TCODConsole::root->print(win_x-1, win_y-1, "Q");
        }
}

void render_rpanel(){
    r_panel->setAlignment(TCOD_RIGHT);
    // TCODConsole *r_panel = new TCODConsole((win_x - MAP_WIDTH_AREA), MAP_HEIGHT_AREA);
    if(wid_rpanel_open){
        //r_panel->clear();
        //r_panel->setDefaultForeground(TCODColor::white);
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
        r_panel->print((win_x - MAP_WIDTH_AREA)-1, 0, "[%c<%c]", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::blit(r_panel, 0, 0, 0, 0,TCODConsole::root, MAP_WIDTH_AREA, 3);
    } else {
        //r_panel->clear();
        //r_panel->setDefaultForeground(TCODColor::red);
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
        r_panel->print((win_x - MAP_WIDTH_AREA)-1, 0, "[%c*%c]", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        //r_panel->print(win_x-1, (win_y - MAP_HEIGHT_AREA)-1, "*");
        TCODConsole::blit(r_panel, (win_x - MAP_WIDTH_AREA)-3, 0, 3, 1,TCODConsole::root, win_x-3, 3);
        //TCODConsole::blit(r_panel,0,0,0,0,TCODConsole::root,MAP_WIDTH_AREA, 0);
    }
}

void render_help(){
    widget_help->clear();
    widget_help->print(0, 0, ">HELP<");
    widget_help->print(0, 2, "> Red asteriscs toggle on UI panels");
    widget_help->print(0, 3, "> bla bla bla");
    widget_help->print(19, 8, " OK ");
    TCODConsole::blit(widget_help,0,0,40,10,TCODConsole::root, 73, 2);
}

void render_base(Game &tgame){
    TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->print(1, win_y-3, "Press 'q' to quit");
        
        TCODConsole::root->setAlignment(TCOD_RIGHT);
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+5, "Press 'z' to punch walls");
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+6, "Press 'r' to regenerate layout/revive player");
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+8, "Press 'd' for DEBUG, Press 'w' to switch tile mode");
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::yellow,TCODColor::black);
        if (tgame.gstate.debug) TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+9, "%cMonster count%c: %d",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, killall);
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+10, "Press 'CTRL+V' to toggle-reveal the map");
       
        TCODConsole::root->setAlignment(TCOD_CENTER);
        TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_2,troll,color_light_ground);
        TCODConsole::setColorControl(TCOD_COLCTRL_3,orc,color_light_ground);
        if (killall > 0){
            TCODConsole::root->print(win_x/2,win_y-1,"%cKILL%c all the '%c(T)rolls%c' and '%c(o)rcs%c'",
                TCOD_COLCTRL_1,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_COLCTRL_STOP,TCOD_COLCTRL_3,TCOD_COLCTRL_STOP);
        }
        else TCODConsole::root->print(win_x/2,win_y-1,"%cALL KILLED!%c",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
}

void render_minimaps(Game &tgame){
    int off_x = 0;
    int off_y = 0;

    off_x = player.x - 55; // centered 
    off_y = player.y - 35;
        
    if (off_x < 0) off_x = 0;
    if (off_y < 0) off_y = 0;

    // draw frame
    if ( tgame.gstate.bigg2 || tgame.gstate.bigg3){
        // mapmode 4 cleared here, mapmode 3 cleared before map building
        if (tgame.gstate.bigg3) con_mini->clear();
        con_mini->setDefaultForeground(TCODColor::lighterGrey);
        con_mini->setDefaultBackground(TCODColor::black);
        for (int n = 0; n < MAP_HEIGHT_AREA+1; ++n){
            con_mini->putChar(0, n, TCOD_CHAR_VLINE, TCOD_BKGND_SET);
            con_mini->putChar(MAP_WIDTH_AREA+1, n, TCOD_CHAR_VLINE, TCOD_BKGND_SET);
        }
        for (int n = 0; n < MAP_WIDTH_AREA+1; ++n){
            con_mini->putChar(n, 0, TCOD_CHAR_HLINE, TCOD_BKGND_SET);
            con_mini->putChar(n, MAP_HEIGHT_AREA+1, TCOD_CHAR_HLINE, TCOD_BKGND_SET);
        }
        con_mini->putChar(0, 0,  TCOD_CHAR_NW, TCOD_BKGND_SET);
        con_mini->putChar(MAP_WIDTH_AREA+1, 0, TCOD_CHAR_NE, TCOD_BKGND_SET);
        con_mini->putChar(0, MAP_HEIGHT_AREA+1, TCOD_CHAR_SW, TCOD_BKGND_SET);
        con_mini->putChar(MAP_WIDTH_AREA+1, MAP_HEIGHT_AREA+1, TCOD_CHAR_SE, TCOD_BKGND_SET);
    }

    if(tgame.gstate.bigg2){
        off_x = player.x - 55; // centered 
        off_y = player.y - 35;
        if (off_x < 0) off_x = 0;
        if (off_y < 0) off_y = 0;
        if ((off_x+110) > MAP_WIDTH) off_x = MAP_WIDTH-110;
        if ((off_y+70) > MAP_HEIGHT) off_y = MAP_HEIGHT-70;
        
        TCODConsole::blit(con_mini,0,0,0,0,TCODConsole::root,0,1); // minimap layer
        //tgame.gstate.fov_recompute = true; // (offset prob) needed to show bigg when moving mouse away
    }

    if(tgame.gstate.bigg3){
        // draws tinymap, 2 steps at once, tinyblock goes 0-15 for all subcell
        for (int i = 0; i < MAP_HEIGHT-2; i += 2){ // i = column
            for (int l = 0; l < MAP_WIDTH; l += 2){ // l = row
                int tinyblock = map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH].blocked | 
                    ( map_array[(i * MAP_WIDTH + (l+1))+ MAP_WIDTH].blocked << 1 ) |
                    ( map_array[((i+1) * MAP_WIDTH + l)+ MAP_WIDTH].blocked << 2 ) |
                    ( map_array[((i+1) * MAP_WIDTH + (l+1))+ MAP_WIDTH].blocked << 3 );
                con_mini->putChar((l/2)+1, (i/2)+1, (tinyblock+400), TCOD_BKGND_SET);
                con_mini->setCharForeground((l/2)+1, (i/2)+1, TCODColor::black);
                con_mini->setCharBackground((l/2)+1, (i/2)+1, minimap_floor, TCOD_BKGND_SET);
            }
        }
        int playerblock = 0;
        int mody = -1;
        if(player.x % 2 == 0 && player.y % 2 == 0) playerblock = 4; // ok
        if(player.x % 2 == 0 && player.y % 2 != 0) playerblock = 1;
        if(player.x % 2 != 0 && player.y % 2 == 0) playerblock = 8; // ok
        if(player.x % 2 != 0 && player.y % 2 != 0) playerblock = 2;
        // 1 2 4 8
        if(player.y % 2 != 0) mody = 0;
        int whoami =  con_mini->getChar(((player.x)/2)+1, ((player.y)/2)+mody+1);
        con_mini->print(0, 67, "I'm on [%d]", whoami);
        if(whoami == 405 && playerblock == 2) playerblock = 16;
        if(whoami == 405 && playerblock == 8) playerblock = 17;
        if(whoami == 404 && playerblock == 2) playerblock = 18;
        if(whoami == 404 && playerblock == 8) playerblock = 19;
        if(whoami == 410 && playerblock == 1) playerblock = 20;
        if(whoami == 410 && playerblock == 4) playerblock = 21;
        if(whoami == 403 && playerblock == 4) playerblock = 22;
        if(whoami == 403 && playerblock == 8) playerblock = 23;
        if(whoami == 412 && playerblock == 1) playerblock = 24;
        if(whoami == 412 && playerblock == 2) playerblock = 25;
        if(whoami == 401 && playerblock == 4) playerblock = 26;
        if(whoami == 401 && playerblock == 8) playerblock = 27;
        if(whoami == 408 && playerblock == 1) playerblock = 28;
        if(whoami == 408 && playerblock == 2) playerblock = 29;
        if(whoami == 408 && playerblock == 4) playerblock = 30;
        if(whoami == 402 && playerblock == 1) playerblock = 31;
        if(whoami == 402 && playerblock == 4) playerblock = 32;
        if(whoami == 402 && playerblock == 8) playerblock = 33;
        // black is white on tile(foreground), ground is black on tile
        con_mini->putChar(((player.x)/2)+1, ((player.y)/2)+mody+1, 400+playerblock, TCOD_BKGND_SET);
        
        con_mini->setCharForeground(((player.x)/2)+1, ((player.y)/2)+mody+1, TCODColor::white);
        if(whoami == 400) // middle of room, draw floor as background
            con_mini->setCharBackground(((player.x)/2)+1,((player.y)/2)+mody+1,minimap_floor,TCOD_BKGND_SET);
        else
            con_mini->setCharBackground(((player.x)/2)+1,((player.y)/2)+mody+1,TCODColor::black,TCOD_BKGND_SET);
       // con_mini->setCharBackground(((player.x)/2), ((player.y)/2)+mody, color_light_ground, (TCOD_bkgnd_flag_t)bkFlag);
        //con_mini->putChar((player.x)/2+1, (player.y)/2+1, 400, TCOD_BKGND_SET);
        //con_mini->setCharForeground((player.x)/2+1, (player.y)/2+1, TCODColor::black);
        //con_mini->setCharBackground((player.x)/2+1, (player.y)/2+1, TCODColor::white, TCOD_BKGND_SET);

        TCODConsole::blit(con_mini,0,0,0,0,TCODConsole::root,0,1); // minimap layer
    } // end bigg3 tinimap
}

void render_context(Game &tgame){
    widget_prompt->setDefaultBackground(TCODColor::darkerGrey);
    widget_prompt->clear();
    TCODConsole::blit(widget_prompt,0,0,0,0,TCODConsole::root,0,71);
}    

void render_top(Game &tgame){
    if(wid_top_open){
        widget_top->clear();
    // map modes
        switch(stance_pos){
            case 1:
                TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::red);
                TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::white,TCODColor::black);
                TCODConsole::setColorControl(TCOD_COLCTRL_3,TCODColor::white,TCODColor::black);
                //std::cout << std::endl << stance_pos << std::endl;
                break;
            case 2:
                TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
                TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::white,TCODColor::red);
                TCODConsole::setColorControl(TCOD_COLCTRL_3,TCODColor::white,TCODColor::black);
                break;
            case 3:   
                TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
                TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::white,TCODColor::black);
                TCODConsole::setColorControl(TCOD_COLCTRL_3,TCODColor::white,TCODColor::red);
                break;
        }
        TCODConsole::setColorControl(TCOD_COLCTRL_5,TCODColor::white,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::black);

        widget_top->setBackgroundFlag(TCOD_BKGND_SET);
        widget_top->print(0, 0, "%c[%c%c>%c%c]%c %c1%c%c2%c%c34%c", TCOD_COLCTRL_5, TCOD_COLCTRL_STOP, TCOD_COLCTRL_4, TCOD_COLCTRL_STOP, TCOD_COLCTRL_5, TCOD_COLCTRL_STOP,
            TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,
            TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
        TCODConsole::blit(widget_top,0,0,0,0,TCODConsole::root,0,0);

        // object-list pop up window
        if(stance_pos2 == 1){
            widget_2_p->clear();
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::red);
            widget_2_p->setDefaultForeground(TCODColor::white);
            int obj_line;
            obj_line = 0;
            for (unsigned int i = 0; i<monvector.size(); ++i) {
                if (tgame.gstate.fov_map->isInFov(monvector[i].x,monvector[i].y)){
                    obj_line += 1;
                    TCODConsole::setColorControl(TCOD_COLCTRL_2,monvector[i].color,TCODColor::black);     
                    widget_2_p->print(0, obj_line, "%c%c%c> This is one %c%s%c.", 
                        TCOD_COLCTRL_2, monvector[i].selfchar, TCOD_COLCTRL_STOP, 
                        TCOD_COLCTRL_2, monvector[i].name, TCOD_COLCTRL_STOP);
                }    
            //if (monvector[i].selfchar == '%')
            //monvector[i].draw(0); // first draws dead bodies
            } 
            if (obj_line == 0){  
                widget_2_p->print(0, 1, "No object in range.");
                obj_line = 1;
            }    
            TCODConsole::blit(widget_2_p,0,0,50,obj_line+2,TCODConsole::root,6,1);  
        } else {
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
        }  

        //widget_2->setBackgroundFlag(TCOD_BKGND_SET);
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::red,TCODColor::black);
        widget_top->print(10, 0, "%cOn Sight%c",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

        // player widget
        widget_top->print(22, 0, "%c@-%c",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::lighterYellow);
        widget_top->print(24, 0, "%cCi%c",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);

        if(tgame.player->rangeweapon) TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
        else TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);    
        widget_top->print(27, 0, "%cR%c",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        
        // fps count
        int fpscount = TCODSystem::getFps();
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
        //TCODConsole::root->print(100, 0, "FPS: %d", fpscount);
        widget_top->print(100, 0, "%cFPS: %d%c", TCOD_COLCTRL_1, fpscount, TCOD_COLCTRL_STOP);
        widget_top->print(92, 0, "%c>?<%c", TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);

        if(!combat_mode)
        widget_top->print(win_x-6, 0, "%c%c%c%c%c%c%c%cMode-N%c", TCOD_COLCTRL_FORE_RGB,255,255,255,
                TCOD_COLCTRL_BACK_RGB,0,0,0,TCOD_COLCTRL_STOP);
        else widget_top->print(win_x-6, 0, "%c%c%c%c%c%c%c%cMode-C%c", TCOD_COLCTRL_FORE_RGB,255,255,255,
                TCOD_COLCTRL_BACK_RGB,0,0,0,TCOD_COLCTRL_STOP);

        TCODConsole::blit(widget_top,0,0,0,0,TCODConsole::root,0,0);

        } else {
        // BLIT widget_top as closed
        //widget_top->setDefaultForeground(TCODColor::red);
        TCODConsole::setColorControl(TCOD_COLCTRL_5,TCODColor::white,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::black);
        widget_top->print(0, 0, "%c[%c%c*%c%c]%c", TCOD_COLCTRL_5, TCOD_COLCTRL_STOP, TCOD_COLCTRL_4,
                TCOD_COLCTRL_STOP, TCOD_COLCTRL_5, TCOD_COLCTRL_STOP);
        TCODConsole::blit(widget_top,0,0,3,1,TCODConsole::root,0,0);
        }
}

void render_prompt(Game &GAME){
    int index = 0;
    if(wid_prompt_open && !GAME.gstate.bigg2 && !GAME.gstate.bigg3){
        for(unsigned int i = 0; i<GAME.gstate.UI_hook.size(); ++i){ 
            if(GAME.gstate.UI_hook[i].ID == 4) index = i;
        }   
        int loc_x = GAME.gstate.UI_hook[index].x;
        int loc_y = GAME.gstate.UI_hook[index].y;
        int w = GAME.gstate.UI_hook[index].w;
        TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
        TCODConsole::root->setDefaultBackground(TCODColor::black);
        for(int x = 0; x < w; ++x){ // black & frame drawing
            TCODConsole::root->putChar(x+loc_x, loc_y+9, '-', TCOD_BKGND_SET);
            TCODConsole::root->putChar(x+loc_x, loc_y+8, ' ', TCOD_BKGND_SET);
            TCODConsole::root->putChar(x+loc_x, loc_y+1, ' ', TCOD_BKGND_SET);
            TCODConsole::root->putChar(x+loc_x, loc_y+2, ' ', TCOD_BKGND_SET);
            TCODConsole::root->putChar(x+loc_x, loc_y+3, ' ', TCOD_BKGND_SET);
            TCODConsole::root->putChar(x+loc_x, loc_y+4, ' ', TCOD_BKGND_SET);
            TCODConsole::root->putChar(x+loc_x, loc_y+5, ' ', TCOD_BKGND_SET);
            TCODConsole::root->putChar(x+loc_x, loc_y+6, ' ', TCOD_BKGND_SET);
            TCODConsole::root->putChar(x+loc_x, loc_y+7, ' ', TCOD_BKGND_SET);
        }
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->print(loc_x, loc_y, "Combat Prompt");
        TCODConsole::root->setColorControl(TCOD_COLCTRL_1,colorbase,TCODColor::black);
        TCODConsole::root->setColorControl(TCOD_COLCTRL_2,colorbase,TCODColor::black);
        TCODConsole::root->setColorControl(TCOD_COLCTRL_3,colorbase,TCODColor::black);
        TCODConsole::root->setColorControl(TCOD_COLCTRL_4,colorbase,TCODColor::red);
        if(!GAME.gstate.mode_move && !GAME.gstate.mode_attack){
            if(prompt_selection == 4 && GAME.gstate.fourth){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::white);
                TCODConsole::root->print(loc_x, loc_y+2, "%cP%c%cASS%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
            } else if(GAME.gstate.fourth){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,colorbase,TCODColor::black);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,colorbase,TCODColor::red);
                TCODConsole::root->print(loc_x, loc_y+2, "%cP%c%cASS%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
            }
            if(prompt_selection == 1 && GAME.gstate.first){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::white);
                TCODConsole::root->print(loc_x, loc_y+3, "%cH%c%cOLD%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
            } else if(GAME.gstate.first){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,colorbase,TCODColor::black);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,colorbase,TCODColor::red);
                TCODConsole::root->print(loc_x, loc_y+3, "%cH%c%cOLD%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
            }
            if(prompt_selection == 2 && GAME.gstate.second){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_2,TCODColor::black,TCODColor::white);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::white);
                TCODConsole::root->print(loc_x, loc_y+4, "%cM%c%cOVE%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_COLCTRL_STOP);
            } else if(GAME.gstate.second){  
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,colorbase,TCODColor::black);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,colorbase,TCODColor::red);
                TCODConsole::root->print(loc_x, loc_y+4, "%cM%c%cOVE%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_COLCTRL_STOP);
            }
            if(GAME.player->rangeweapon){
                if(prompt_selection == 3 && GAME.gstate.third){
                    TCODConsole::root->setColorControl(TCOD_COLCTRL_3,TCODColor::black,TCODColor::white);
                    TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::white);
                    TCODConsole::root->print(loc_x, loc_y+5, "%cF%c%cIRE%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_3,TCOD_COLCTRL_STOP);
                } else if(GAME.gstate.third){
                    TCODConsole::root->setColorControl(TCOD_COLCTRL_1,colorbase,TCODColor::black);
                    TCODConsole::root->setColorControl(TCOD_COLCTRL_4,colorbase,TCODColor::red);
                    TCODConsole::root->print(loc_x, loc_y+5, "%cF%c%cIRE%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_3,TCOD_COLCTRL_STOP);
                }
                TCODConsole::root->setColorControl(TCOD_COLCTRL_3,TCODColor::lighterGrey,TCODColor::black);
                TCODConsole::root->print(loc_x+4, loc_y+5, "%c> (%d AP)%c", TCOD_COLCTRL_3, GAME.player->attAP, 
                        TCOD_COLCTRL_STOP);
            } else {    
                if(prompt_selection == 3 && GAME.gstate.third){
                    TCODConsole::root->setColorControl(TCOD_COLCTRL_3,TCODColor::black,TCODColor::white);
                    TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::white);
                    TCODConsole::root->print(loc_x, loc_y+5, "%cA%c%cTTACK%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_3,TCOD_COLCTRL_STOP);
                } else if(GAME.gstate.third){
                    TCODConsole::root->setColorControl(TCOD_COLCTRL_1,colorbase,TCODColor::black);
                    TCODConsole::root->setColorControl(TCOD_COLCTRL_4,colorbase,TCODColor::red);
                    TCODConsole::root->print(loc_x, loc_y+5, "%cA%c%cTTACK%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_3,TCOD_COLCTRL_STOP);
                }
                TCODConsole::root->setColorControl(TCOD_COLCTRL_3,TCODColor::lighterGrey,TCODColor::black);
                TCODConsole::root->print(loc_x+6, loc_y+5, "%c> (%d AP)%c", TCOD_COLCTRL_3, GAME.player->attAP, 
                        TCOD_COLCTRL_STOP);
            }

            int ranged_offx = 0;

            if(prompt_selection == 5 && GAME.player->rangeweapon){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::white);
                switch(GAME.player->rangeaim){
                    case 0:
                        TCODConsole::root->print(loc_x, loc_y+6, "%cN%c%cOCK%c",
                                TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                        ranged_offx = 5;
                        break;
                    case 1:
                        TCODConsole::root->print(loc_x, loc_y+6, "%cD%c%cRAW%c",
                                TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                        ranged_offx = 5;
                        break; 
                    case 2:
                        TCODConsole::root->print(loc_x, loc_y+6, "%cT%c%cARGET%c",
                                TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                        ranged_offx = 7;
                        break;    
                } 
                if(GAME.player->rangeaim >= 2 && GAME.player->ranged_target != -2){
                    TCODConsole::root->print(loc_x, loc_y+6, "%cA%c%cIM%c",
                                TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                    ranged_offx = 4;
                }
            } else if(GAME.player->rangeweapon){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,colorbase,TCODColor::black);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,colorbase,TCODColor::red);
                switch(GAME.player->rangeaim){
                    case 0:
                        TCODConsole::root->print(loc_x, loc_y+6, "%cN%c%cOCK%c",
                                TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                        ranged_offx = 5;
                        break;
                    case 1:
                        TCODConsole::root->print(loc_x, loc_y+6, "%cD%c%cRAW%c",
                                TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                        ranged_offx = 5;
                        break;
                    case 2:
                        TCODConsole::root->print(loc_x, loc_y+6, "%cT%c%cARGET%c",
                                TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                        ranged_offx = 7;
                        break;    
                }
                if(GAME.player->rangeaim >= 2 && GAME.player->ranged_target != -2){
                    TCODConsole::root->print(loc_x, loc_y+6, "%cA%c%cIM%c",
                            TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                    ranged_offx = 4;
                }  
            }
            if(GAME.player->rangeweapon){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_3,TCODColor::lighterGrey,TCODColor::black);
                switch(GAME.player->rangeaim){
                    case 0:
                        TCODConsole::root->print(ranged_offx, loc_y+6, "%c> Bow ready%c", 
                                TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
                        break;
                    case 1:
                        TCODConsole::root->print(ranged_offx, loc_y+6, "%c> Arrow nocked%c", 
                                TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
                        break;
                    case 2:
                        TCODConsole::root->print(ranged_offx, loc_y+6, "%c> Drawing%c", TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
                        break;
                } 
                if(GAME.player->rangeaim >= 3){
                    TCODConsole::root->setColorControl(TCOD_COLCTRL_5,TCODColor::lighterGreen,TCODColor::black);
                    if(GAME.player->aim >= GAME.player->skill.bowML)
                        TCODConsole::root->print(ranged_offx, loc_y+6, "%c> Max aim(%d): %c%d%%%c%c", TCOD_COLCTRL_3, GAME.player->rangeaim-2,
                            TCOD_COLCTRL_5, GAME.player->aim, TCOD_COLCTRL_STOP, TCOD_COLCTRL_STOP);
                    else TCODConsole::root->print(ranged_offx, loc_y+6, "%c> Aiming(%d): %c%d%%%c%c", TCOD_COLCTRL_3, GAME.player->rangeaim-2,
                            TCOD_COLCTRL_5, GAME.player->aim, TCOD_COLCTRL_STOP, TCOD_COLCTRL_STOP);
                }    
            }   
            if(prompt_selection == 6 && GAME.player->rangeweapon && 
                    GAME.player->ranged_target != -2 && GAME.player->rangeaim >= 2){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::white);
                TCODConsole::root->print(loc_x, loc_y+7, "%cN%c%cEW TARGET%c",
                                TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
            } else if(GAME.player->rangeweapon &&
                    GAME.player->ranged_target != -2 && GAME.player->rangeaim >= 2){
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,colorbase,TCODColor::black);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,colorbase,TCODColor::red);
                TCODConsole::root->print(loc_x, loc_y+7, "%cN%c%cEW TARGET%c",
                                TCOD_COLCTRL_4,TCOD_COLCTRL_STOP,TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
            }    
            if(!GAME.gstate.third){
                int phaseswitch = player.att_phase;
                if(phaseswitch > 5) phaseswitch = phaseswitch - 5; // restart from 1
                switch(phaseswitch){ // just colors message
                    case 1:
                        TCODConsole::root->setColorControl(TCOD_COLCTRL_5,TCODColor::lighterGreen,TCODColor::black);
                        break;
                    case 2:
                        TCODConsole::root->setColorControl(TCOD_COLCTRL_5,TCODColor::lighterBlue,TCODColor::black);
                        break;
                    case 3:
                        TCODConsole::root->setColorControl(TCOD_COLCTRL_5,TCODColor::lighterYellow,TCODColor::black);
                        break;
                    case 4:
                        TCODConsole::root->setColorControl(TCOD_COLCTRL_5,TCODColor::lightOrange,TCODColor::black);
                        break;
                    case 5:
                        TCODConsole::root->setColorControl(TCOD_COLCTRL_5,TCODColor::lightRed,TCODColor::black);
                        break;
                }    
                TCODConsole::root->setColorControl(TCOD_COLCTRL_3,TCODColor::darkGrey,TCODColor::black);
                int aimoffset = 4;
                if(!GAME.player->rangeweapon) aimoffset += 2; // if not ranged, then melee 
                if(GAME.player->phaseAP < GAME.player->attAP) 
                    TCODConsole::root->print(loc_x+aimoffset, loc_y+5, "> Not enough AP (%d)", GAME.player->attAP);
                else TCODConsole::root->print(loc_x+aimoffset, loc_y+5, "> Attack phase: %c%d%c",
                        TCOD_COLCTRL_5, player.att_phase, TCOD_COLCTRL_STOP);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::darkGrey,TCODColor::black);
                if(GAME.player->rangeweapon)
                    TCODConsole::root->print(loc_x, loc_y+5, "%cFIRE%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP);
                else    
                    TCODConsole::root->print(loc_x, loc_y+5, "%cATTACK%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP);
            }    
            if(!GAME.gstate.second){ 
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::darkGrey,TCODColor::black);
                TCODConsole::root->print(loc_x, loc_y+4, "%cMOVE%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP);
            }    
            if(!GAME.gstate.first){ 
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::darkGrey,TCODColor::black);
                TCODConsole::root->print(loc_x, loc_y+3, "%cHOLD%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP);
            } 
            if(!GAME.gstate.fourth){ 
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::darkGrey,TCODColor::black);
                TCODConsole::root->print(loc_x, loc_y+2, "%cPASS%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP);
            }
            if(GAME.player->aim >= GAME.player->skill.bowML){ // can't improve aim, max reached 
                TCODConsole::root->setColorControl(TCOD_COLCTRL_4,TCODColor::darkGrey,TCODColor::black);
                TCODConsole::root->print(loc_x, loc_y+6, "%cAIM%c",TCOD_COLCTRL_4,TCOD_COLCTRL_STOP);
            }
        }else if(GAME.gstate.mode_move){
            TCODConsole::root->setColorControl(TCOD_COLCTRL_1,TCODColor::lightRed,TCODColor::black);
            TCODConsole::root->setColorControl(TCOD_COLCTRL_2,TCODColor::lightRed,TCODColor::lighterYellow);
            if(prompt_selection == 1){ 
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::lightRed);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_2,TCODColor::lighterYellow,TCODColor::lightRed);
            }    
            TCODConsole::root->print(loc_x, loc_y+2, "%cE%c%cND MOVEMENT%c", TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                    TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        } else if(GAME.gstate.mode_attack){
            TCODConsole::root->setColorControl(TCOD_COLCTRL_1,TCODColor::lightRed,TCODColor::black);
            TCODConsole::root->setColorControl(TCOD_COLCTRL_2,TCODColor::lightRed,TCODColor::lighterYellow);
            if(prompt_selection == 1){ 
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::lightRed);
                TCODConsole::root->setColorControl(TCOD_COLCTRL_2,TCODColor::lighterYellow,TCODColor::lightRed);
            }    
            TCODConsole::root->print(loc_x, loc_y+2, "%cE%c%cND ATTACK%c", TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                    TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            if(GAME.gstate.combat_aiming){ 
                TCODConsole::root->setColorControl(TCOD_COLCTRL_1,TCODColor::lighterGrey,TCODColor::black);
                TCODConsole::root->print(loc_x, loc_y+3, "%cESC or Right Mouse Button%c", 
                        TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                TCODConsole::root->print(loc_x, loc_y+4, "%cto cancel%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP); 
            }    
        }  
        TCODConsole::root->print(loc_x, loc_y+10, "aimP(%d) target(%d)",
                GAME.player->rangeaim, GAME.player->ranged_target);
    }    
}    

void render_all (Game &tgame){

    int off_x = 0;
    int off_y = 0;
    off_x = player.x - 55; // centered (110 70 is map area space) 
    off_y = player.y - 35;
    if (off_x < 0 && !tgame.gstate.bigg) off_x = 0; // doesn't go negative, blit 0 0 con to 0 0 screen
    if (off_y < 0 && !tgame.gstate.bigg) off_y = 0;
    if (off_x < -28 && tgame.gstate.bigg) off_x = -28; // doesn't go negative, BIGG
    if (off_y < -18 && tgame.gstate.bigg) off_y = -18;
    if (off_x < 0 && tgame.gstate.bigg2) off_x = 0; 
    if (off_y < 0 && tgame.gstate.bigg2) off_y = 0; 

    // overridden by off_xx at the bottom of this function?
    tgame.gstate.off_xx = off_x; // used in monster attack cycle
    tgame.gstate.off_yy = off_y;
    if(tgame.gstate.off_xx >= MAP_WIDTH_AREA) tgame.gstate.off_xx = MAP_WIDTH_AREA; // stop end of map
    if(tgame.gstate.off_yy >= MAP_HEIGHT_AREA) tgame.gstate.off_yy = MAP_HEIGHT_AREA;

    // upper limit of window to draw, offset + window size, or max map
    int maxmap_y = off_y + win_y;
    int maxmap_x = off_x + win_x;
    if(maxmap_y > MAP_HEIGHT) maxmap_y = MAP_HEIGHT;
    if(maxmap_x > MAP_WIDTH) maxmap_x = MAP_WIDTH;
    
    int drawmap_off_x = off_x; // needed so the map drawing stops on upper limit
    int drawmap_off_y = off_y;
    if ((off_x+110) > MAP_WIDTH) drawmap_off_x = MAP_WIDTH-110;
    if ((off_y+70) > MAP_HEIGHT) drawmap_off_y = MAP_HEIGHT-70;

    bool wall = false;
    bool visible = false;
    int isbloody = 0;


    int t_i = 0; // true map coordinates
    int t_l = 0;

    t_i = drawmap_off_y; // true coordinates to start drawing
    t_l = drawmap_off_x;
    
    int topx = 0; // end of drawn box, false coordinates
    int topy = 0;
    topx = maxmap_x - off_x;
    topy = maxmap_y - off_y;
    if (topx < MAP_WIDTH_AREA) topx = MAP_WIDTH_AREA;
    if (topy < MAP_HEIGHT_AREA) topy = MAP_HEIGHT_AREA;

    // disable map building if in popup mapmodes
    if (tgame.gstate.fov_recompute){
        con_mini->clear(); // used for mapmode 3
        tgame.gstate.fov_map->computeFov(player.x, player.y, TORCH_RADIUS, FOV_LIGHT_WALLS, FOV_ALGO);

        tgame.gstate.con->setDefaultBackground(nullcolor);
        tgame.gstate.con->clear(); 
        tgame.gstate.con->setDefaultBackground(TCODColor::black);

        for (int i = 0; i < topy; ++i){ // i = column
            for (int l = 0; l < topx; ++l) { // l = row

                t_i = drawmap_off_y + i; // actual coordinates for map logic
                t_l = drawmap_off_x + l;

                visible = tgame.gstate.fov_map->isInFov(t_l,t_i);
                wall = map_array[t_i * MAP_WIDTH + t_l].blocked;
                isbloody = map_array[t_i * MAP_WIDTH + t_l].bloodyt;

                if (tgame.gstate.bigg && l > 27 && i > 17 ){
                    l -= 28;
                    i -= 18;
                    tgame.gstate.con->putChar((l*2), (i*2), 503, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((l*2)+1, (i*2), 603, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((l*2), (i*2)+1, 703, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((l*2)+1, (i*2)+1, 803, TCOD_BKGND_SET);
               
                    tgame.gstate.con->setCharBackground((l*2), (i*2), TCODColor::black, TCOD_BKGND_SET);
                    tgame.gstate.con->setCharBackground((l*2)+1, (i*2), TCODColor::black, TCOD_BKGND_SET);
                    tgame.gstate.con->setCharBackground((l*2), (i*2)+1, TCODColor::black, TCOD_BKGND_SET);
                    tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, TCODColor::black, TCOD_BKGND_SET);

                    tgame.gstate.con->setCharForeground((l*2), (i*2), TCODColor::black);
                    tgame.gstate.con->setCharForeground((l*2)+1, (i*2), TCODColor::black);
                    tgame.gstate.con->setCharForeground((l*2), (i*2)+1, TCODColor::black);
                    tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, TCODColor::black);
                    l += 28;
                    i += 18;

                } else if (!tgame.gstate.bigg){
                    tgame.gstate.con->putChar(l, i, ' ', TCOD_BKGND_SET);
                    tgame.gstate.con->setCharBackground(l, i, TCODColor::black, TCOD_BKGND_SET);
                    tgame.gstate.con->setCharForeground(l, i, TCODColor::black);
                }
                if (tgame.gstate.bigg2){
                    con_mini->putChar(l+1, i+1, ' ', TCOD_BKGND_SET);
                    con_mini->setCharBackground(l+1, i+1, TCODColor::black, TCOD_BKGND_SET);
                    con_mini->setCharForeground(l+1, i+1, TCODColor::black);
                }

                if (!visible){ // if NOT visible
                    if (map_array[t_i * MAP_WIDTH + t_l].explored || revealdungeon){
                        if (wall){
                            if(tgame.gstate.bigg && l > 27 && i > 17 ){
                                l -= 28;
                                i -= 18;
                                tgame.gstate.con->putChar((l*2), (i*2), 503, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1, (i*2), 603, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2), (i*2)+1, 703, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1, (i*2)+1, 803, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_dark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_dark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_dark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_dark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_dark_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_dark_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_dark_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_dark_wall);
                                l += 28;
                                i += 18;
                            } else if (!tgame.gstate.bigg){    
                                tgame.gstate.con->putChar(l, i, '#', TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground(l, i, color_dark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, color_dark_wall);
                            }
                            if (tgame.gstate.bigg2){
                                con_mini->putChar(l+1, i+1, '#', TCOD_BKGND_SET);
                                con_mini->setCharBackground(l+1, i+1, color_dark_wall, TCOD_BKGND_SET);
                                con_mini->setCharForeground(l+1, i+1, color_dark_wall);
                            }
                        } else { // if floor
                            if(tgame.gstate.bigg && l > 27 && i > 17 ){
                                l -= 28;
                                i -= 18;
                                tgame.gstate.con->putChar(l*2,i*2, 509, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, 609, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, 709, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, 809, TCOD_BKGND_SET); 
                        
                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_dark_groundF);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_dark_groundF);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_dark_groundF);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_dark_groundF);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), tgame.gstate.color_dark_ground, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), tgame.gstate.color_dark_ground, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, tgame.gstate.color_dark_ground, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, tgame.gstate.color_dark_ground, TCOD_BKGND_SET);
                                l += 28;
                                i += 18;
                            } else if (!tgame.gstate.bigg){
                                tgame.gstate.con->putChar(l, i, '.', TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, color_dark_groundF);
                                tgame.gstate.con->setCharBackground(l, i, tgame.gstate.color_dark_ground, TCOD_BKGND_SET);
                            }
                            if (tgame.gstate.bigg2 ){
                                con_mini->putChar(l+1, i+1, '.', TCOD_BKGND_SET);
                                con_mini->setCharForeground(l+1, i+1, color_dark_groundF);
                                con_mini->setCharBackground(l+1, i+1, tgame.gstate.color_dark_ground, TCOD_BKGND_SET);
                            }
                        }
                    }
                } else { // if visible
                    if (wall){
                        if(tgame.gstate.bigg && l > 27 && i > 17 ){
                            l -= 28;
                            i -= 18;
                            
                            // make doors as wall to correctly draw
                            for (unsigned int n = 0; n<doors.size(); ++n){
                                map_array[doors[n].y * MAP_WIDTH + doors[n].x] = Tile(1,1);
                            }
                            
                            // adark = not hidden in 2*8 mode
                            if( ( (map_array[(t_i * MAP_WIDTH + t_l)-1].blocked) && 
                                        (map_array[(t_i * MAP_WIDTH + t_l)+1].blocked) ) &&
                                    ( tgame.gstate.fov_map->isInFov(t_l+1,t_i) && tgame.gstate.fov_map->isInFov(t_l-1,t_i) ) ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_wall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_wall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_wall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(t_i * MAP_WIDTH + t_l)+1].blocked) && 
                                        tgame.gstate.fov_map->isInFov(t_l+1,t_i) ) &&
                                    ( (map_array[(t_i * MAP_WIDTH + t_l)+MAP_WIDTH].blocked) &&
                                      tgame.gstate.fov_map->isInFov(t_l,t_i+1) ) ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_trwall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_trwall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_trwall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_trwall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(t_i * MAP_WIDTH + t_l)-1].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l-1,t_i) ) &&
                                    ( (map_array[(t_i * MAP_WIDTH + t_l)+MAP_WIDTH].blocked) &&
                                    tgame.gstate.fov_map->isInFov(t_l,t_i+1) ) ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_tlwall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_tlwall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_tlwall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_tlwall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(t_i * MAP_WIDTH + t_l)+1].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l+1,t_i) ) &&
                                    ( (map_array[(t_i * MAP_WIDTH + t_l)-MAP_WIDTH].blocked) &&
                                    tgame.gstate.fov_map->isInFov(t_l,t_i-1) ) ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_bwall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_bwall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_bwall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_bwall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(t_i * MAP_WIDTH + t_l)-MAP_WIDTH].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l,t_i-1) ) &&
                                    ( (map_array[(t_i * MAP_WIDTH + t_l)-1].blocked) &&
                                    tgame.gstate.fov_map->isInFov(t_l-1,t_i) ) ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_ibwall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_ibwall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_ibwall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_ibwall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(t_i * MAP_WIDTH + t_l)+MAP_WIDTH].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l,t_i+1) ) ||
                                    ( (map_array[(t_i * MAP_WIDTH + t_l)-MAP_WIDTH].blocked) &&
                                    tgame.gstate.fov_map->isInFov(t_l,t_i-1) ) ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_vwall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_vwall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_vwall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_vwall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( !(map_array[(t_i * MAP_WIDTH + t_l)-MAP_WIDTH].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l,t_i-1) ) &&
                                    ( (map_array[(t_i * MAP_WIDTH + t_l)+MAP_WIDTH].blocked) &&
                                    tgame.gstate.fov_map->isInFov(t_l,t_i+1) ) &&
                                    ( (map_array[(t_i * MAP_WIDTH + t_l)-MAP_WIDTH-1].blocked) &&
                                    tgame.gstate.fov_map->isInFov(t_l-1,t_i-1) ) ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_tlwall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_tlwall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_tlwall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_tlwall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(t_i * MAP_WIDTH + t_l)-1].blocked) && 
                                    (map_array[(t_i * MAP_WIDTH + t_l)+1].blocked) ) ||
                                    ( tgame.gstate.fov_map->isInFov(t_l+1,t_i) && tgame.gstate.fov_map->isInFov(t_l-1,t_i) ) ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_wall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_wall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_wall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(t_i * MAP_WIDTH + t_l)].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l,t_i) ) &&
                                    ( !(map_array[(t_i * MAP_WIDTH + t_l)-MAP_WIDTH].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l,t_i-1) ) &&
                                    ( !(map_array[(t_i * MAP_WIDTH + t_l)+MAP_WIDTH].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l,t_i+1) )
                                     ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_vwall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_vwall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_vwall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_vwall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(t_i * MAP_WIDTH + t_l)-1].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l-1,t_i) ) ||
                                    ( (map_array[(t_i * MAP_WIDTH + t_l)+1].blocked) && 
                                    tgame.gstate.fov_map->isInFov(t_l+1,t_i)  )
                                     ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_wall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_wall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_wall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if(abs(t_i-player.x) > abs(t_l-player.y)
                                     ){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_wall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_wall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_wall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else {
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_vwall, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_vwall+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_vwall+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_vwall+300, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            }

                            
                            // reset door hack
                            for (unsigned int n = 0; n<doors.size(); ++n){
                                map_array[doors[n].y * MAP_WIDTH + doors[n].x] = Tile(0,1);
                            }
                            l += 28;
                            i += 18;
                                   
                        } else if (!tgame.gstate.bigg){ // if map standard

                            // make doors as wall to correctly draw
                            for (unsigned int n = 0; n<doors.size(); ++n){
                                map_array[doors[n].y * MAP_WIDTH + doors[n].x] = Tile(1,1);
                            }

                            if(!(map_array[(t_i * MAP_WIDTH + t_l)-1].blocked) && 
                                    !(map_array[(t_i * MAP_WIDTH + t_l)+ MAP_WIDTH].blocked)){ // L NE
                                //con->putChar(l, i, 668, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l, i, tgame.tileval.u8_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground(l, i, tgame.tileval.u8_wallCa, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, tgame.tileval.u8_wallCb);
                                //con->setCharBackground(l, i, demake_sub_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, demake_main_wall);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall); 
                            } else if( (map_array[(t_i * MAP_WIDTH + t_l)-1].blocked) && 
                                    (map_array[(t_i * MAP_WIDTH + t_l)+ MAP_WIDTH].blocked) && 
                                    !(map_array[(t_i * MAP_WIDTH + t_l)+ MAP_WIDTH-1].blocked) ){ // upR corner
                                //con->putChar(l, i, 669, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l, i, tgame.tileval.u8_trwall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground(l, i, tgame.tileval.u8_wallCb, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, tgame.tileval.u8_wallCa);
                                //con->setCharBackground(l, i, demake_main_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, demake_main_wall);
                                //con->setCharBackground(l, i, color_light_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_dark_wallF);

                            } else if(!(map_array[(t_i * MAP_WIDTH + t_l)+ MAP_WIDTH].blocked)){ // h wall top
                                //con->putChar(l, i, 666, TCOD_BKGND_SET);
                                //con->putChar(l, i, 446, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l, i, tgame.tileval.u8_hwall, TCOD_BKGND_SET);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall); 
                                tgame.gstate.con->setCharBackground(l, i, tgame.tileval.u8_wallCa, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, tgame.tileval.u8_wallCb);
                            } else if(!(map_array[(t_i * MAP_WIDTH + t_l)-1].blocked)){ // v wall (right)
                                //con->putChar(l, i, 667, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l, i, tgame.tileval.u8_vwall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground(l, i, tgame.tileval.u8_wallCc, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, tgame.tileval.u8_wallCd);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                            } else if(!(map_array[(t_i * MAP_WIDTH + t_l)+1].blocked)){ // v wall left
                                //con->putChar(l, i, 667, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l, i, ' ', TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground(l, i, tgame.tileval.u8_wallCb, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, tgame.tileval.u8_wallCa);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                            } else if(!tgame.gstate.fov_map->isInFov(t_l,t_i+1)){ // h wall bottom
                                //con->putChar(l, i, 667, TCOD_BKGND_SET);
                                //con->putChar(l, i, 446, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l, i, tgame.tileval.u8_bwall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground(l, i, tgame.tileval.u8_wallCd, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, tgame.tileval.u8_wallCc);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                            } else if( !(map_array[(t_i * MAP_WIDTH + t_l)+MAP_WIDTH+1].blocked) ){ // upL corner
                                //con->putChar(l, i, 667, TCOD_BKGND_SET);
                                //con->putChar(l, i, 446, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l, i, tgame.tileval.u8_tlwall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharBackground(l, i, tgame.tileval.u8_wallCb, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, tgame.tileval.u8_wallCa);
                                //con->setCharBackground(l, i, demake_main_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, demake_main_wall);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                            } else {    
                                tgame.gstate.con->putChar(l, i, '#', TCOD_BKGND_SET);
                                //con->setCharBackground(l, i, color_light_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                                tgame.gstate.con->setCharBackground(l, i, demake_sub_wall, TCOD_BKGND_SET);
                                tgame.gstate.con->setCharForeground(l, i, demake_main_wall);
                            }
                            for (unsigned int n = 0; n<doors.size(); ++n){
                                map_array[doors[n].y * MAP_WIDTH + doors[n].x] = Tile(0,1);
                            }
                        }
                        if (tgame.gstate.bigg2){
                            con_mini->putChar(l+1, i+1, '#', TCOD_BKGND_SET);
                            con_mini->setCharBackground(l+1, i+1, color_light_wall, TCOD_BKGND_SET);
                            con_mini->setCharForeground(l+1, i+1, color_light_wall);
                        }
                    }
                    else { // if floor
                        if(tgame.gstate.bigg && l > 27 && i > 17 ){
                            l -= 28;
                            i -= 18;
                            if(map_array[t_i * MAP_WIDTH + t_l].feature > 2){
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_floor2, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_floor2+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_floor2+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_floor2+300, TCOD_BKGND_SET);
                            } else {
                                tgame.gstate.con->putChar(l*2,i*2, tgame.tileval.u16_floor1, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,i*2, tgame.tileval.u16_floor1+100, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar(l*2,(i*2)+1, tgame.tileval.u16_floor1+200, TCOD_BKGND_SET);
                                tgame.gstate.con->putChar((l*2)+1,(i*2)+1, tgame.tileval.u16_floor1+300, TCOD_BKGND_SET);
                            }

                            tgame.gstate.con->setCharBackground((l*2), (i*2), TCODColor::black, TCOD_BKGND_SET);
                            tgame.gstate.con->setCharBackground((l*2)+1, (i*2), TCODColor::black, TCOD_BKGND_SET);
                            tgame.gstate.con->setCharBackground((l*2), (i*2)+1, TCODColor::black, TCOD_BKGND_SET);
                            tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, TCODColor::black, TCOD_BKGND_SET);

                            tgame.gstate.con->setCharForeground((l*2), (i*2), color_light_ground2);
                            tgame.gstate.con->setCharForeground((l*2)+1, (i*2), color_light_ground2);
                            tgame.gstate.con->setCharForeground((l*2), (i*2)+1, color_light_ground2);
                            tgame.gstate.con->setCharForeground((l*2)+1, (i*2)+1, color_light_ground2); 
                            l += 28;
                            i += 18;

                        } else if (!tgame.gstate.bigg){
                            /*
                            if(t_l%2==1){
                                if(t_l%3==1 || t_i%3==1){
                                    if(t_l%3==1)tgame.gstate.con->putChar(l, i, tgame.tileval.u8_floor1, TCOD_BKGND_SET);
                                    if(t_i%3==1)tgame.gstate.con->putChar(l, i, tgame.tileval.u8_floor2, TCOD_BKGND_SET);
                                } else tgame.gstate.con->putChar(l, i, tgame.tileval.u8_floor1, TCOD_BKGND_SET);
                            } 
                            else if(t_l%2==1)tgame.gstate.con->putChar(l, i, tgame.tileval.u8_floor2, TCOD_BKGND_SET); 
                            else tgame.gstate.con->putChar(l, i, tgame.tileval.u8_floor2, TCOD_BKGND_SET);
                            */
                            if(map_array[t_i * MAP_WIDTH + t_l].feature == 4) 
                                tgame.gstate.con->putChar(l, i, tgame.tileval.u8_floor4, TCOD_BKGND_SET);
                            else if(map_array[t_i * MAP_WIDTH + t_l].feature == 3) 
                                tgame.gstate.con->putChar(l, i, tgame.tileval.u8_floor3, TCOD_BKGND_SET);
                            else if(map_array[t_i * MAP_WIDTH + t_l].feature == 2) 
                                tgame.gstate.con->putChar(l, i, tgame.tileval.u8_floor2, TCOD_BKGND_SET);
                            else tgame.gstate.con->putChar(l, i, tgame.tileval.u8_floor1, TCOD_BKGND_SET);
                            //con->putChar(l, i, '.', TCOD_BKGND_SET);
                            //con->setCharForeground(l, i, TCODColor::white);
                            //con->setCharForeground(l, i, demake_main_floor);
                            //con->setCharBackground(l, i, color_light_ground, TCOD_BKGND_SET);
                            //con->setCharBackground(l, i, demake_sub_floor, TCOD_BKGND_SET);
                            tgame.gstate.con->setCharForeground(l, i, tgame.tileval.u8_floorCa);
                            tgame.gstate.con->setCharBackground(l, i, tgame.tileval.u8_floorCb, TCOD_BKGND_SET);
                        }
                        if (tgame.gstate.bigg2){
                            con_mini->putChar(l+1, i+1, '.', TCOD_BKGND_SET);
                            con_mini->setCharForeground(l+1, i+1, TCODColor::white);
                            con_mini->setCharBackground(l+1, i+1, color_light_ground, TCOD_BKGND_SET);
                        }
                        
                        
                    }

                    // set blood on tiles
                    if (isbloody > 0){
                        if (isbloody >= 4) blood = blood1;
                        if (isbloody == 3) blood = blood2;
                        if (isbloody == 2) blood = blood3;
                        if (isbloody < 2) blood = blood4; // sets color
                        if(tgame.gstate.bigg && l > 27 && i > 17 ){
                            l -= 28;
                            i -= 18;
                            tgame.gstate.con->setCharBackground((l*2), (i*2), blood, TCOD_BKGND_SET);
                            tgame.gstate.con->setCharBackground((l*2)+1, (i*2), blood, TCOD_BKGND_SET);
                            tgame.gstate.con->setCharBackground((l*2), (i*2)+1, blood, TCOD_BKGND_SET);
                            tgame.gstate.con->setCharBackground((l*2)+1, (i*2)+1, blood, TCOD_BKGND_SET);
                            l += 28;
                            i += 18;
                        } else if (!tgame.gstate.bigg){   
                            tgame.gstate.con->setCharBackground(l, i, blood, TCOD_BKGND_SET);
                        }
                    }

                    map_array[t_i * MAP_WIDTH + t_l].explored = true;
                    
                }
                //fov_map->setProperties(l, i, map_array[i * MAP_WIDTH + l].block_sight, map_array[i * MAP_WIDTH + l].blocked);
                
            }
        }

    tgame.gstate.fov_recompute = false;

    }

    for (unsigned int i = 0; i<tgame.gstate.features.size(); ++i){
        int shortx = tgame.gstate.features[i].x; 
        int shorty = tgame.gstate.features[i].y;
        if(shortx == player.x && shorty == player.y){ 
            if(!tgame.gstate.features[i].observed){ 
                //event_description(tgame);
                event_sender = i;
                tgame.gstate.features[i].observed = true; // visited 
            }    
        }    
        if(tgame.gstate.fov_map->isInFov(shortx, shorty)){
            if(tgame.gstate.bigg){
            } else {
                tgame.gstate.con->putChar((shortx)-tgame.gstate.off_xx, 
                        (shorty)-tgame.gstate.off_yy, '?', TCOD_BKGND_SET);
                tgame.gstate.con->setCharBackground((shortx)-tgame.gstate.off_xx, (shorty)-tgame.gstate.off_yy, TCODColor::black, TCOD_BKGND_SET);
                if(tgame.gstate.features[i].observed)
                    tgame.gstate.con->setCharForeground((shortx)-tgame.gstate.off_xx, (shorty)-tgame.gstate.off_yy, TCODColor::grey);
                else
                    tgame.gstate.con->setCharForeground((shortx)-tgame.gstate.off_xx, (shorty)-tgame.gstate.off_yy, feature_c);
            }  
        }    
    }    

    // draw doors
    for (unsigned int i = 0; i<doors.size(); ++i){
        if(tgame.gstate.fov_map->isInFov(doors[i].x,doors[i].y)){
            if(tgame.gstate.bigg){
                if((map_array[(doors[i].y * MAP_WIDTH + doors[i].x)-1].blocked)){
                    doors[i].x -= 28;
                    doors[i].y -= 18;
                    doors[i].x -= tgame.gstate.off_xx;
                    doors[i].y -= tgame.gstate.off_yy;
                    tgame.gstate.con->putChar(doors[i].x*2,doors[i].y*2, tgame.tileval.u16_door, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((doors[i].x*2)+1,doors[i].y*2, tgame.tileval.u16_door+100, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar(doors[i].x*2,(doors[i].y*2)+1, tgame.tileval.u16_door+200, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((doors[i].x*2)+1,(doors[i].y*2)+1, tgame.tileval.u16_door+300, TCOD_BKGND_SET);
                } else {
                    doors[i].x -= 28;
                    doors[i].y -= 18;
                    doors[i].x -= tgame.gstate.off_xx;
                    doors[i].y -= tgame.gstate.off_yy;
                    tgame.gstate.con->putChar(doors[i].x*2,doors[i].y*2, tgame.tileval.u16_doorv, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((doors[i].x*2)+1,doors[i].y*2, tgame.tileval.u16_doorv+100, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar(doors[i].x*2,(doors[i].y*2)+1, tgame.tileval.u16_doorv+200, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((doors[i].x*2)+1,(doors[i].y*2)+1, tgame.tileval.u16_doorv+300, TCOD_BKGND_SET); 
                }
                    
                tgame.gstate.con->setCharBackground((doors[i].x*2), (doors[i].y*2), color_door_sub, TCOD_BKGND_SET);
                tgame.gstate.con->setCharBackground((doors[i].x*2)+1, (doors[i].y*2), color_door_sub, TCOD_BKGND_SET);
                tgame.gstate.con->setCharBackground((doors[i].x*2), (doors[i].y*2)+1, color_door_sub, TCOD_BKGND_SET);
                tgame.gstate.con->setCharBackground((doors[i].x*2)+1, (doors[i].y*2)+1, color_door_sub, TCOD_BKGND_SET);
                
                // black for demake, brown for 28
                tgame.gstate.con->setCharForeground((doors[i].x*2), (doors[i].y*2), color_door_main);
                tgame.gstate.con->setCharForeground((doors[i].x*2)+1, (doors[i].y*2), color_door_main);
                tgame.gstate.con->setCharForeground((doors[i].x*2), (doors[i].y*2)+1, color_door_main);
                tgame.gstate.con->setCharForeground((doors[i].x*2)+1, (doors[i].y*2)+1, color_door_main);
                doors[i].x += 28;
                doors[i].y += 18;
                doors[i].x += tgame.gstate.off_xx;
                doors[i].y += tgame.gstate.off_yy;
            } else {    
                //con->putChar(doors[i].x, doors[i].y, TCOD_CHAR_CROSS, TCOD_BKGND_SET);
                //con->putChar(doors[i].x, doors[i].y, 444, TCOD_BKGND_SET);
                tgame.gstate.con->putChar((doors[i].x)-tgame.gstate.off_xx, (doors[i].y)-tgame.gstate.off_yy, tgame.tileval.u8_door, TCOD_BKGND_SET);
                // door color swwitches wrong between 8tile and 8ASCII
                tgame.gstate.con->setCharBackground((doors[i].x)-tgame.gstate.off_xx, (doors[i].y)-tgame.gstate.off_yy, TCODColor::black, TCOD_BKGND_SET);
                tgame.gstate.con->setCharForeground((doors[i].x)-tgame.gstate.off_xx, (doors[i].y)-tgame.gstate.off_yy, door_c);
            }    
        }
    }
   
    for (unsigned int i = 0; i<monvector.size(); ++i) {
        if (monvector[i].selfchar == '%')
        monvector[i].draw(0, tgame); // first draws dead bodies
    }

    // DROPPED ITEMS
    for (auto i : tgame.gstate.wd_object) {
        tgame.gstate.con->putChar(i.posx(), i.posy(), i.glyph_8, TCOD_BKGND_SET);
    }    

    for (unsigned int i = 0; i<monvector.size(); ++i) {
        if (monvector[i].selfchar != '%')
        monvector[i].draw(0, tgame); // then draws monsters still alive
    }

    for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->draw(0, tgame); // player vector

    // blacks the root, to almost black
    // TCODConsole::blit(border,0,0,0,0,TCODConsole::root,0,0); // offmap background
    // TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SCREEN);
    TCODConsole::root->setDefaultBackground(nullcolor);
    TCODConsole::root->clear();
    TCODConsole::root->setDefaultBackground(TCODColor::black);

    // BLIT MAP TO ROOT (both default and bigg map)
    if(!tgame.gstate.bigg){
        int smalloff_x = off_x;
        int smalloff_y = off_y;
        // - used for origin blit
        // starting point + screenmap area > entire map, 
        if((smalloff_x+MAP_WIDTH_AREA) > MAP_WIDTH) smalloff_x = MAP_WIDTH-MAP_WIDTH_AREA;
        if((smalloff_y+MAP_HEIGHT_AREA) > MAP_HEIGHT) smalloff_y = MAP_HEIGHT-MAP_HEIGHT_AREA;
        // - used for blit width height
        // starting point + winx > entire map
        // then reduce win_x by the mount it exceed the map size
        // source, source_x, source_y, howmuch_x, howmuch_y, target,
        //std::cout << "SMALLOFF: " << smalloff_x << std::endl;
        if(!tgame.gstate.bigg3 && !tgame.gstate.bigg2) 
            TCODConsole::blit(tgame.gstate.con,0,0,topx,topy,TCODConsole::root,0,0);
        //TCODConsole::blit(tgame.gstate.con,0,0, fullx, fully,TCODConsole::root,0,0);
       
        tgame.gstate.off_xx = smalloff_x; // stores offset for external use
        tgame.gstate.off_yy = smalloff_y;
        
        //std::cout << "SMALLOFF: " << off_xx << " " << off_yy << std::endl;
    } else {
        int bigoff_x = (player.x*2)-55;
        int bigoff_y = (player.y*2)-35;
        if(bigoff_x < 0) bigoff_x = 0;
        if(bigoff_y < 0) bigoff_y = 0;
        // bigg: *2 to pick the correct source point of the square to copy
        if(!tgame.gstate.bigg3 && !tgame.gstate.bigg2) 
            //TCODConsole::blit(tgame.gstate.con, bigoff_x, bigoff_y, win_x, win_y,TCODConsole::root,0,0);
            TCODConsole::blit(tgame.gstate.con, 0, 0, 0, 0, TCODConsole::root,0,0);
        //std::cout << " " << bigoff_x;
        tgame.gstate.offbig_x = bigoff_x;
        tgame.gstate.offbig_y = bigoff_y; // sets offset to bring during monster attacks
    }

    render_top(tgame);
    render_context(tgame);

    if(!combat_mode && !wid_combat_open) render_base(tgame);
    render_messagelog(tgame);
    render_rpanel();
    render_minimaps(tgame);

    if(wid_help){
        render_help();
    }

    if(combat_mode){
        render_prompt(tgame);
    } 
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

void bloodsplat(Object_monster &cobj){

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

int m_x = 0;
int m_y = 0;
bool combat_null = false; // set if waiting instead of moving, in combat

void player_move_attack(int dx, int dy, Game &tgame, int overpowering){

    int x = player.x + dx; // coordinates of the movement cell
    int y = player.y + dy;

    // this block just updates facing in the 4 directions
    if (dy != 0){
        if (dy == -1) player.facing = 0;
        else player.facing = 2;
    }  
    if (dx != 0){
        if (dx == -1) player.facing = 3;
        else player.facing = 1;
    }

    unsigned int target = 0;
    bool is_it = false;
  
    // checks if monster is in next cell
    for (unsigned int i = 0; i<monvector.size(); ++i){ 
        if (monvector[i].x == x && monvector[i].y == y){
            if (monvector[i].alive == true){
                target = i;
                is_it = true; // used to enable attack block below
            }
        }
    }

    if(is_it && monvector[target].alive && overpowering) player.AP += 4; // gives movement points to add attacks
    if( (is_it && monvector[target].alive && player.AP >= 4 && tgame.gstate.mode_attack && player.phase_attack == 0) ||
        (is_it && monvector[target].alive && player.AP >= 4 && tgame.gstate.mode_attack && overpowering) ){

        player.stats.attack(player, monvector[target], 0, player.overpower_l);
        ++player.phase_attack; // used to stop attacking again after 1st

        if(!tgame.gstate.no_combat)player.AP -= 4; // decrease combat movement only if in combat mode
        if (monvector[target].stats.hp < 1){
                //monvector.erase (target); 
                --killall;
                bloodsplat(monvector[target]);
                bloodycount = 5;


                monvector[target].selfchar = '%';
                monvector[target].color = monsterdead;
                monvector[target].blocks = false;
                monvector[target].alive = false;

                monvector[target].self_16 = 526;
                monvector[target].self_8 = 451; // was 506, why?
 
                player.move(0, 0, monvector); // updates player position so feet get bloody
                tgame.gstate.fov_recompute = true;
                render_all(tgame);
                TCODConsole::flush();
                m_x = 0;
                m_y = 0;
            }
            else {
                if(!tgame.gstate.bigg2){
                    char tchar = monvector[target].selfchar;
                    //TCODConsole::root->clear();
                    monvector[target].colorb = TCODColor::red;
                    monvector[target].hit = true;
                    player.colorb = TCODColor::black;
                    if (!tgame.gstate.bigg) monvector[target].selfchar = '-';
                    player.draw(1, tgame);
                    monvector[target].draw(1, tgame);

                    if (tgame.gstate.bigg){  
                        TCODConsole::blit(tgame.gstate.con, 0, 0, 0, 0, TCODConsole::root,0,0);
                        //TCODConsole::blit(tgame.gstate.con,tgame.gstate.offbig_x,tgame.gstate.offbig_y+2,110,68,TCODConsole::root,0,2);
                    } else {    
                    //    TCODConsole::blit(con,0,0,110,70,TCODConsole::root,0,0);
                    //} else {
                        TCODConsole::blit(tgame.gstate.con, 0, 0, 0, 0, TCODConsole::root,0,0);
                        //TCODConsole::blit(tgame.gstate.con,tgame.gstate.off_xx,tgame.gstate.off_yy+1,110,69,TCODConsole::root,0,1);
                    }

                    //TCODConsole::blit(con,0,0,0,0,TCODConsole::root,0,0);
                    TCODConsole::flush();
                    Sleep(250); // shitty way for attack "animation", uses windows.h
                    monvector[target].colorb = tgame.gstate.color_dark_ground;
                    player.colorb = tgame.gstate.color_dark_ground;
                    monvector[target].hit = false;
                    if (!tgame.gstate.bigg) monvector[target].selfchar = tchar;
               
                    player.draw(0, tgame);
                    monvector[target].draw(0, tgame);
                    monvector[target].drop(tgame.gstate.wd_object); // drop monster inventory
                    tgame.gstate.con->clear();
                    tgame.gstate.fov_recompute = true;
                    render_all(tgame);
                    TCODConsole::flush();
                }

                tgame.gstate.mesg->setAlignment(TCOD_LEFT);
                tgame.gstate.mesg->setDefaultForeground(TCODColor::yellow);
                tgame.gstate.mesg->setDefaultBackground(TCODColor::black);
                tgame.gstate.mesg->print(1, 1, "Hit!");
         
               
                std::cout << "monster target hp: " << monvector[target].stats.hp << std::endl;
                m_x = 0;
                m_y = 0;
            }
    } else { 
        //std::cout << "TIMES INTO move loop"  << std::endl;
        //player.move(dx, dy, monvector);
        //fov_recompute = true;
        if(overpowering && player.overpower_l > 1){ // if trying to overpower but enemy dead
            m_x = dx; // MOMENTUM
            m_y = dy;
            player.overpower_l = 0;
            return;
            
        }else if(overpowering && player.overpower_l == 1){
            m_x = 0; // don't move
            m_y = 0;
            player.overpower_l = 0;
            return;
        }    
        player.overpower_l = 0;
        m_x = dx;
        m_y = dy;
    }    
}

int alreadydead = 0;

bool ranged_target(Game &GAME){

    GAME.gstate.modal = true; // disables overlays
    bool range_td = false;
    TCOD_mouse_t mouse;
    TCOD_key_t key;
    TCOD_event_t eve;
    bool second = false;
    uint32 millisecond = 0;
    mouse.lbutton = 0; // trying to reset

    int targetx = -1;
    int targety = -1;
    int stepdistance = 0; // distance
    int rangepenalty = 0;
    GAME.gstate.combat_aiming = true; // state to draw the correct command prompt

    do{
        uint32 millicounter = TCODSystem::getElapsedMilli();
        if(millicounter > millisecond){ 
            millisecond = millicounter + 500;
            second = !second;
        }    
        render_all(GAME);
        I_am_moused(GAME);
        if(GAME.gstate.mapx != -1 && GAME.gstate.mapy != -1){
            TCODLine::init(GAME.player->x,GAME.player->y,GAME.gstate.mapx,GAME.gstate.mapy);

            int x = GAME.player->x;
            int y = GAME.player->y;
            int nonx = 0;
            int nony = 0;
            stepdistance = 0; // reset distance on redraw
            while (!TCODLine::step(&x,&y)) {
                
                if (!GAME.gstate.bigg){
                    nonx = x - GAME.gstate.off_xx;
                    nony = y - GAME.gstate.off_yy;
                } else if (GAME.gstate.bigg){    
                    nonx = ((x*2) - GAME.gstate.off_xx)-28;
                    nony = ((y*2) - GAME.gstate.off_yy)-18;
                }
                if( (GAME.gstate.fov_map->isInFov(x,y) || map_array[y * MAP_WIDTH + x].explored) 
                        && map_array[y * MAP_WIDTH + x].blocked) break;
                if(x == GAME.gstate.mapx && y == GAME.gstate.mapy) break; // origin
                TCODConsole::root->setDefaultForeground(TCODColor::yellow);
                TCODConsole::root->putChar(nonx, nony, 178, TCOD_BKGND_SET);
            }
            if((x != GAME.player->x || y != GAME.player->y) && second){ // blinking *
                TCODConsole::root->setDefaultForeground(TCODColor::red);
                TCODConsole::root->putChar(nonx, nony, '*', TCOD_BKGND_SET);
            }
            stepdistance = (int)sqrt(pow(x-GAME.player->x,2)+pow(y-GAME.player->y,2));
            if(stepdistance <= GAME.player->rangedD1) rangepenalty = 10;
            else if(stepdistance > GAME.player->rangedD1 && stepdistance <= GAME.player->rangedD2) rangepenalty = 5;
            else if(stepdistance > GAME.player->rangedD2 && stepdistance <= GAME.player->rangedD3) rangepenalty = 0;
            else if(stepdistance > GAME.player->rangedD3 && stepdistance <= GAME.player->rangedD4) rangepenalty = -20;
            else if(stepdistance > GAME.player->rangedD4 && stepdistance <= GAME.player->rangedD5) rangepenalty = -40; 
            else if(stepdistance > GAME.player->rangedD5 && stepdistance <= GAME.player->rangedD6) rangepenalty = -80;
            // previous two ifs should be disabled, due to dungeon ceiling
            if(stepdistance <= GAME.player->rangedD4 && stepdistance > 0){ 
                if((GAME.player->skill.bowML + rangepenalty) > 0)
                        TCODConsole::root->setDefaultForeground(TCODColor::lightGreen);
                else TCODConsole::root->setDefaultForeground(TCODColor::red);  
                TCODConsole::root->print(nonx+2, nony, "%d%%", GAME.player->skill.bowML + rangepenalty); 
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                int stepx = 0; int stepy = 0;
                if( (y-GAME.player->y) >= 0 ){stepx = GAME.player->x - 4; stepy = GAME.player->y - 2;}
                else {stepx = GAME.player->x - 4; stepy = GAME.player->y + 2;}
                if (!GAME.gstate.bigg){
                    stepx = stepx - GAME.gstate.off_xx;
                    stepy = stepy - GAME.gstate.off_yy;
                } else if (GAME.gstate.bigg){    
                    stepx = ((stepx*2) - GAME.gstate.off_xx)-28;
                    stepy = ((stepy*2) - GAME.gstate.off_yy)-18;
                }
                TCODConsole::root->print(stepx, stepy, "Dist: %d", stepdistance); // distance UI
            }  
            if(stepdistance > GAME.player->rangedD4){ // into 5-6 distance phases, so ceiling
                TCODConsole::root->setDefaultForeground(TCODColor::red);
                TCODConsole::root->print(nonx+2, nony, "Ceiling!");
            }    
            targetx = nonx;
            targety = nony;
            if(mouse.lbutton || (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_ENTER)) range_td = true;
        }
        //std::cout << "rangevar: " << range_td << " mouse: " << mouse.lbutton << std::endl;
        eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
        TCODConsole::flush(); // this updates the screen
    } while ( (eve != TCOD_EVENT_KEY_PRESS || 
                (key.vk != TCODK_ESCAPE && key.c != 'e')) && !range_td && !mouse.rbutton);

    GAME.gstate.combat_aiming = false;
    GAME.gstate.modal = false; // re-enables overlays
    std::cout << "distance: " << stepdistance << std::endl;

    msg_log msgd;
    if(!range_td){ // if aborted targeting 
        fetch = 1; // abort firing
        msgd.color1 = TCODColor::lighterBlue;
        sprintf(msgd.message, "%c>%cPlayer is looking for targets.",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        msg_log_list.push_back(msgd);
    }    
    else{ // if vaild target
        GAME.player->tlocx = targetx;
        GAME.player->tlocy = targety;
        std::cout << "targetx: " << GAME.player->tlocx << " targety: " << GAME.player->tlocy << std::endl;
        givememapxy(GAME.player->tlocx, GAME.player->tlocy, GAME);
        std::cout << "targetx: " << GAME.player->tlocx << " targety: " << GAME.player->tlocy << std::endl;
        GAME.player->ranged_target = -1; // point blank default
        // checks if a monster is the target
        for (unsigned int i = 0; i<monvector.size(); ++i){ 
            if (monvector[i].x == GAME.player->tlocx && monvector[i].y == GAME.player->tlocy){
                if(monvector[i].alive)
                    GAME.player->ranged_target = i; // target is the monster index 
            }
        }
    }    

    //TCODLine::init(GAME.player->x,GAME.player->y,GAME.gstate.mapx,GAME.gstate.mapy); // ?????
    return true;
}    

int handle_keys(Object_player &duh, Game &tgame) {

    bool mycase_p;
    mycase_p = 0;
   
    TCOD_key_t keyr;
    TCOD_mouse_t mouser;
    //TCOD_key_t key = TCODConsole::waitForKeypress(true);
    TCOD_event_t eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&keyr,&mouser);

    if (bloodycount < 0) bloodycount = 0; // if ... change color 

    if (keyr.vk==TCODK_F1) {
        // switch renderers with F1,F2,F3 
        TCODSystem::setRenderer(TCOD_RENDERER_GLSL);
    } else if (keyr.vk==TCODK_F2) {
        TCODSystem::setRenderer(TCOD_RENDERER_OPENGL);
    } else if (keyr.vk==TCODK_F3) {
        TCODSystem::setRenderer(TCOD_RENDERER_SDL);
    }

    // COMBAT PROMPT
    if(!tgame.gstate.mode_move && !tgame.gstate.mode_attack){ 
        if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'p' && wid_prompt_open){
            if(tgame.gstate.fourth) fetch = 4;
        }
        if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'h' && wid_prompt_open){
            if(tgame.gstate.first) fetch = 1;
        } 
        if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'm' && wid_prompt_open){
            if(tgame.gstate.second) fetch = 2;
        }
        if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'a' && wid_prompt_open){
            if(tgame.gstate.third) fetch = 3;
        }
        if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'f' && wid_prompt_open){
            if(tgame.gstate.third) fetch = 5;
        }
    }
    if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'e' && wid_prompt_open){
        if(tgame.gstate.mode_move || tgame.gstate.mode_attack) fetch = 1;
    }    
 
    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == '`' ){ 
        if(LIMIT_FPS == 30) LIMIT_FPS = 3000;
        else LIMIT_FPS = 30;
        TCODSystem::setFps(LIMIT_FPS);
    }   

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'q' ) return quit;

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'x' ) return quit2; // quit combat

    //if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'y' ) combat_mode = true;

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'v' ){ if (revealdungeon) revealdungeon = false; else revealdungeon = true; Sleep (100); tgame.gstate.fov_recompute = true;}

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'd' ){ if (tgame.gstate.debug) tgame.gstate.debug = false; else tgame.gstate.debug = true;
        tgame.gstate.fov_recompute = true; Sleep (100);}

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'w' ){

        tgame.gstate.fov_recompute = true;

        if(tgame.gstate.bigg){
            if(tgame.tileval.u16_door == tgame.tileval.t28_door){
                color_bdark_wall.r = 0;
                color_bdark_wall.g = 0;
                color_bdark_wall.b = 0;
                color_door_sub.r = color_light_wall.r;
                color_door_sub.g = color_light_wall.g;
                color_door_sub.b = color_light_wall.b;
                color_door_main.r = 0;
                color_door_main.g = 0;
                color_door_main.b = 0;
                tgame.tileval.u16_door = tgame.tileval.t16_door;
                tgame.tileval.u16_doorv = tgame.tileval.t16_doorv;
                //tgame.tileval.u16_player = tgame.tileval.t16_player; 
                tgame.tileval.u16_wall = tgame.tileval.t16_wall;
                tgame.tileval.u16_vwall = tgame.tileval.t16_vwall;
                tgame.tileval.u16_trwall = tgame.tileval.t16_trwall;
                tgame.tileval.u16_tlwall = tgame.tileval.t16_tlwall;
                tgame.tileval.u16_bwall = tgame.tileval.t16_bwall;
                tgame.tileval.u16_ibwall = tgame.tileval.t16_ibwall;
                tgame.tileval.u16_floor1 = tgame.tileval.t16_floor1; 
                tgame.tileval.u16_floor2 = tgame.tileval.t16_floor2; 
                //tgame.tileval.u16_orc = tgame.tileval.t16_orc; 
                //tgame.tileval.u16_troll = tgame.tileval.t16_troll; 
                //tgame.tileval.u16_corpse = tgame.tileval.t16_corpse; 
            } else {
                color_bdark_wall.r = color_light_wall.r;
                color_bdark_wall.g = color_light_wall.g;
                color_bdark_wall.b = color_light_wall.b;
                color_door_sub.r = 0;
                color_door_sub.g = 0;
                color_door_sub.b = 0;
                color_door_main.r = 222;
                color_door_main.g = 136;
                color_door_main.b = 0;
                tgame.tileval.u16_door = tgame.tileval.t28_door;
                tgame.tileval.u16_doorv = tgame.tileval.t28_door;
                tgame.tileval.u16_player = tgame.tileval.t28_player; 
                tgame.tileval.u16_wall = tgame.tileval.t28_wall; 
                tgame.tileval.u16_vwall = tgame.tileval.t28_nwall;
                tgame.tileval.u16_trwall = tgame.tileval.t28_nwall;
                tgame.tileval.u16_tlwall = tgame.tileval.t28_nwall;
                tgame.tileval.u16_bwall = tgame.tileval.t28_wall;
                tgame.tileval.u16_ibwall = tgame.tileval.t28_wall;
                tgame.tileval.u16_floor1 = tgame.tileval.t28_floor1; 
                tgame.tileval.u16_floor2 = tgame.tileval.t28_floor2; 
                tgame.tileval.u16_orc = tgame.tileval.t28_orc; 
                tgame.tileval.u16_troll = tgame.tileval.t28_troll; 
                tgame.tileval.u16_corpse = tgame.tileval.t28_corpse;
            }    
        } else { // block to swap basic 8 mode NOT BIGG
            if(tgame.tileval.u8_door == TCOD_CHAR_CROSS){
                tgame.tileval.U8 = true; // sprite
                tgame.tileval.u8_door = 444;
                tgame.tileval.u8_player = 445;
                tgame.tileval.u8_floor1 = 447;
                tgame.tileval.u8_floor2 = 447;  
                tgame.tileval.u8_floor3 = 448;
                tgame.tileval.u8_floor4 = 448;
                tgame.tileval.u8_wall = 446;
                tgame.tileval.u8_hwall = 446;
                tgame.tileval.u8_trwall = ' ';
                tgame.tileval.u8_tlwall = ' ';
                tgame.tileval.u8_vwall = ' ';
                tgame.tileval.u8_bwall = 446;
                tgame.tileval.u8_orc = 449; 
                tgame.tileval.u8_troll = 450; 
                tgame.tileval.u8_corpse = 451;
                tgame.tileval.u8_wallCa = demake_sub_wall;
                tgame.tileval.u8_wallCb = demake_main_wall;
                tgame.tileval.u8_wallCd = demake_sub_wall;
                tgame.tileval.u8_wallCc = demake_main_wall;
                tgame.tileval.u8_floorCa = demake_main_floor;
                tgame.tileval.u8_floorCb = demake_sub_floor;
            } else {
                tgame.tileval.u8_door = TCOD_CHAR_CROSS;
                tgame.tileval.U8 = false; // ASCII
                tgame.tileval.u8_player = '@';
                tgame.tileval.u8_floor1 = '.';
                tgame.tileval.u8_floor2 = ',';
                tgame.tileval.u8_floor3 = '\'';
                tgame.tileval.u8_floor4 = '`';
                tgame.tileval.u8_wall = 668;
                tgame.tileval.u8_hwall = 666;
                tgame.tileval.u8_trwall = 669;
                tgame.tileval.u8_tlwall = ' ';
                tgame.tileval.u8_vwall = 667;
                tgame.tileval.u8_bwall = ' ';
                tgame.tileval.u8_orc = 'o'; 
                tgame.tileval.u8_troll = 'T'; 
                tgame.tileval.u8_corpse = '%';

                tgame.tileval.u8_wallCa = color_dark_wallF;
                tgame.tileval.u8_wallCb = color_light_wall;
                tgame.tileval.u8_wallCc = color_dark_wallF;
                tgame.tileval.u8_wallCd = color_light_wall;
                tgame.tileval.u8_floorCa = TCODColor::white;
                tgame.tileval.u8_floorCb = color_light_ground;
            }    
        }    

    }  

    // read descriptions manually
    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'i' ){
        for (unsigned int i = 0; i<tgame.gstate.features.size(); ++i){
            int shortx = tgame.gstate.features[i].x; 
            int shorty = tgame.gstate.features[i].y;
            if(shortx == player.x && shorty == player.y){ 
                event_description(tgame, i); // screens.cpp
            }
        }
    }    

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'r' ){

        m_x = 0;
        m_y = 0;

        std::cout << " Monster array: " << monvector.size() << std::endl;
        for (auto &i : monvector){
            delete i.path0;
            delete i.path1;
        }    
        monvector.clear();
        doors.clear();

        if(color_light_wall.r == 1 || color_light_wall.r == 219){ // organge
            color_light_wall.r = 255;
            color_light_wall.g = 90;
            color_light_wall.b = 16;
            color_dark_wallF.r = 160;
            color_dark_wallF.g = 70;
            color_dark_wallF.b = 30;
            color_light_ground.r = 25;
            color_light_ground.g = 86;
            color_light_ground.b = 20; 
            color_light_ground2.r = 55;
            color_light_ground2.g = 86;
            color_light_ground2.b = 20;
            if(tgame.tileval.u16_wall == tgame.tileval.t16_wall){
                color_bdark_wall.r = 0;
                color_bdark_wall.g = 0;
                color_bdark_wall.b = 0;
                color_door_sub.r = color_light_wall.r;
                color_door_sub.g = color_light_wall.g;
                color_door_sub.b = color_light_wall.b;
                color_door_main.r = 0;
                color_door_main.g = 0;
                color_door_main.b = 0;
            }    
            if(tgame.tileval.u16_wall == tgame.tileval.t28_wall){
                color_bdark_wall.r = color_light_wall.r;
                color_bdark_wall.g = color_light_wall.g;
                color_bdark_wall.b = color_light_wall.b;
            }    
        } else if(color_light_wall.r == 255){ // pink
            color_light_wall.r = 250;
            color_light_wall.g = 0;
            color_light_wall.b = 130;
            color_dark_wallF.r = 160;
            color_dark_wallF.g = 42;
            color_dark_wallF.b = 103;
            color_light_ground.r = 136;
            color_light_ground.g = 16;
            color_light_ground.b = 158;
            color_light_ground2.r = 186;
            color_light_ground2.g = 16;
            color_light_ground2.b = 158;
            if(tgame.tileval.u16_wall == tgame.tileval.t16_wall){
                color_bdark_wall.r = 0;
                color_bdark_wall.g = 0;
                color_bdark_wall.b = 0;
                color_door_sub.r = color_light_wall.r;
                color_door_sub.g = color_light_wall.g;
                color_door_sub.b = color_light_wall.b;
                color_door_main.r = 0;
                color_door_main.g = 0;
                color_door_main.b = 0;
            }
            if(tgame.tileval.u16_wall == tgame.tileval.t28_wall){
                color_bdark_wall.r = color_light_wall.r;
                color_bdark_wall.g = color_light_wall.g;
                color_bdark_wall.b = color_light_wall.b;
            }
        } else if(color_light_wall.r == 250){ // blue
            color_light_wall.r = 0;
            color_light_wall.g = 0;
            color_light_wall.b = 100;
            color_dark_wallF.r = 0;
            color_dark_wallF.g = 0;
            color_dark_wallF.b = 50;
            color_light_ground.r = 50;
            color_light_ground.g = 50;
            color_light_ground.b = 150;
            color_light_ground2.r = 90;
            color_light_ground2.g = 50;
            color_light_ground2.b = 150;
            if(tgame.tileval.u16_wall == tgame.tileval.t16_wall){
                color_bdark_wall.r = 0;
                color_bdark_wall.g = 0;
                color_bdark_wall.b = 0;
                color_door_sub.r = color_light_wall.r;
                color_door_sub.g = color_light_wall.g;
                color_door_sub.b = color_light_wall.b;
                color_door_main.r = 0;
                color_door_main.g = 0;
                color_door_main.b = 0;
            }
            if(tgame.tileval.u16_wall == tgame.tileval.t28_wall){
                color_bdark_wall.r = color_light_wall.r;
                color_bdark_wall.g = color_light_wall.g;
                color_bdark_wall.b = color_light_wall.b;
            }
        } else if(color_light_wall.r == 0){ // demake
            color_light_wall.r = 1;
            color_light_wall.g = 146;
            color_light_wall.b = 170;
            color_dark_wallF.r = 0;
            color_dark_wallF.g = 0;
            color_dark_wallF.b = 50;
            color_light_ground.r = 50;
            color_light_ground.g = 50;
            color_light_ground.b = 150;
            color_light_ground2.r = 1;
            color_light_ground2.g = 146;
            color_light_ground2.b = 170;
            if(tgame.tileval.u16_wall == tgame.tileval.t16_wall){
                color_bdark_wall.r = 0;
                color_bdark_wall.g = 0;
                color_bdark_wall.b = 0;
                color_door_sub.r = color_light_wall.r;
                color_door_sub.g = color_light_wall.g;
                color_door_sub.b = color_light_wall.b;
                color_door_main.r = 0;
                color_door_main.g = 0;
                color_door_main.b = 0;
            }
            if(tgame.tileval.u16_wall == tgame.tileval.t28_wall){
                color_bdark_wall.r = color_light_wall.r;
                color_bdark_wall.g = color_light_wall.g;
                color_bdark_wall.b = color_light_wall.b;
            }
        }  

        // set colors for 8 mode
        //if(!bigg){
            if(tgame.tileval.u8_door != TCOD_CHAR_CROSS){ // demake (is this needed if colors don't change?)
                tgame.tileval.u8_wallCa = demake_sub_wall;
                tgame.tileval.u8_wallCb = demake_main_wall;
                tgame.tileval.u8_wallCd = demake_sub_wall;
                tgame.tileval.u8_wallCc = demake_main_wall;
                tgame.tileval.u8_floorCa = demake_main_floor;
                tgame.tileval.u8_floorCb = demake_sub_floor;
            } else { // ASCII   
                tgame.tileval.u8_wallCa = color_dark_wallF;
                tgame.tileval.u8_wallCb = color_light_wall;
                tgame.tileval.u8_wallCc = color_dark_wallF;
                tgame.tileval.u8_wallCd = color_light_wall;
                tgame.tileval.u8_floorCa = TCODColor::white;
                tgame.tileval.u8_floorCb = color_light_ground;
            }   
        //}    

        //Sleep(4000);
        if (mapmode == 1){
            
            //color_light_wall(0, 0, 100); 
            BSProoms.clear();
            make_map_BSP(duh, tgame);
            //make_map(duh);
            mapmode = 0;
        } else {
            BSProoms.clear();
            
            make_map_BSP(duh, tgame);
            //make_map2(duh);
            mapmode = 1;
        }    
        duh.bloody = 0;
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                tgame.gstate.fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                        !(map_array[i * MAP_WIDTH + l].blocked));
                //map_array[row * MAP_WIDTH + l] = Tile(1,1);
            }
        }
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                tgame.gstate.fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                        !(map_array[i * MAP_WIDTH + l].blocked));
            }
        }
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                tgame.gstate.fov_map_mons_path0->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                        !(map_array[i * MAP_WIDTH + l].blocked));
            }
        }
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                tgame.gstate.fov_map_mons_path1->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                        !(map_array[i * MAP_WIDTH + l].blocked));
            }
        }
        tgame.gstate.fov_recompute = true;
        player.stats.hp = player.stats.max_hp;
        player.selfchar = '@';
        game_state = playing;
        player.AP = player.APm;
        set_black(tgame);
        alreadydead = 0;
        //    fov_recompute = true;
        render_all(tgame);
        TCODConsole::flush(); // this updates the screen
        if(is_handle_combat) return quit2; // regenerates everything and quits combat
    }

    if (game_state == playing) {

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'z' ){
        m_x = 0;
        m_y = 0;
        TCODConsole::root->clear();
        tgame.gstate.mesg->setAlignment(TCOD_LEFT);
        tgame.gstate.mesg->setDefaultForeground(TCODColor::white);
        tgame.gstate.mesg->setDefaultBackground(TCODColor::black);
        tgame.gstate.mesg->print(1, 1, "Give a direction to dig dungeon");
       
        // CRASH , why?
        //myvector[1]->draw(0);
       
        if(!(duh.y > MAP_HEIGHT-8 )) TCODConsole::blit(tgame.gstate.mesg,0,0,33,3,tgame.gstate.con,1,MAP_HEIGHT-4);
        else TCODConsole::blit(tgame.gstate.mesg,0,0,33,3,tgame.gstate.con,MAP_WIDTH-37,1);

        TCODConsole::blit(tgame.gstate.con,0,0,win_x,win_y,TCODConsole::root,0,0);

        TCODConsole::flush();
        while (!mycase_p){
            TCODConsole::waitForKeypress(true);
            if (TCODConsole::isKeyPressed(TCODK_UP)){ 
                std::cout << "Dig up." << std::endl; mycase_p = 1;
                map_array[(duh.y - 1)*MAP_WIDTH +duh.x].blocked = 0;
                map_array[(duh.y - 1)*MAP_WIDTH +duh.x].block_sight = 0;
                tgame.gstate.mesg->clear();

                tgame.gstate.fov_map->setProperties(duh.x, duh.y-1, 1, 1);
                tgame.gstate.fov_map_mons->setProperties(duh.x, duh.y-1, 1, 1);
                tgame.gstate.fov_map_mons_path0->setProperties(duh.x, duh.y-1, 1, 1);
                tgame.gstate.fov_map_mons_path1->setProperties(duh.x, duh.y-1, 1, 1);
               
                tgame.gstate.fov_recompute = true;
                return false;
            }
            if (TCODConsole::isKeyPressed(TCODK_DOWN)){ 
                std::cout << "Dig down." << std::endl;mycase_p = 1; 
                map_array[(duh.y + 1)*MAP_WIDTH +duh.x].blocked = 0;
                map_array[(duh.y + 1)*MAP_WIDTH +duh.x].block_sight = 0;
                tgame.gstate.mesg->clear();

                tgame.gstate.fov_map->setProperties(duh.x, duh.y+1, 1, 1);
                tgame.gstate.fov_map_mons->setProperties(duh.x, duh.y+1, 1, 1);
                tgame.gstate.fov_map_mons_path0->setProperties(duh.x, duh.y+1, 1, 1);
                tgame.gstate.fov_map_mons_path1->setProperties(duh.x, duh.y+1, 1, 1);

                tgame.gstate.fov_recompute = true;
                return false; 
            }
            if (TCODConsole::isKeyPressed(TCODK_LEFT)){
                std::cout << "Dig left." << std::endl;mycase_p = 1;
                map_array[duh.y*MAP_WIDTH +(duh.x -1)].blocked = 0;
                map_array[duh.y*MAP_WIDTH +(duh.x -1)].block_sight = 0;
                tgame.gstate.mesg->clear();

                tgame.gstate.fov_map->setProperties(duh.x-1, duh.y, 1, 1);
                tgame.gstate.fov_map_mons->setProperties(duh.x-1, duh.y, 1, 1);
                tgame.gstate.fov_map_mons_path0->setProperties(duh.x-1, duh.y, 1, 1);
                tgame.gstate.fov_map_mons_path1->setProperties(duh.x-1, duh.y, 1, 1);

                tgame.gstate.fov_recompute = true;
                return false;
            }
            if (TCODConsole::isKeyPressed(TCODK_RIGHT)){ 
                std::cout << "Dig right." << std::endl;mycase_p = 1;
                map_array[duh.y*MAP_WIDTH +(duh.x + 1)].blocked = 0;
                map_array[duh.y*MAP_WIDTH +(duh.x + 1)].block_sight = 0;
                tgame.gstate.mesg->clear();

                tgame.gstate.fov_map->setProperties(duh.x+1, duh.y, 1, 1);
                tgame.gstate.fov_map_mons->setProperties(duh.x+1, duh.y, 1, 1);
                tgame.gstate.fov_map_mons_path0->setProperties(duh.x+1, duh.y, 1, 1);
                tgame.gstate.fov_map_mons_path1->setProperties(duh.x+1, duh.y, 1, 1);
 
                tgame.gstate.fov_recompute = true;
                return false;
            }
        }
    }

    // end of KEY DIG cycle

    if (eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_UP ){
        --bloodycount;
        --duh.bloody; 
        do player_move_attack(0, -1, tgame, player.overpower_l);
        while (player.overpower_l > 0);
        //std::cout << " Monster array: " << myvector.size() << std::endl;
    }

    // end KEY UP cycle

    else if (eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_DOWN){
        --bloodycount;
        --duh.bloody; 
        do player_move_attack(0, 1, tgame, player.overpower_l);
        while (player.overpower_l > 0);
    }

    // end KEY DOWN cycle

    else if (eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_LEFT){
        --bloodycount;
        --duh.bloody;   
        do player_move_attack(-1, 0, tgame, player.overpower_l);
        while (player.overpower_l > 0);
    }

    // end KEY LEFT cycle

    else if ( eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_RIGHT){
        --bloodycount; 
        --duh.bloody; 
        do player_move_attack(1, 0, tgame, player.overpower_l);
        while (player.overpower_l > 0);
    }

    // end KEY RIGHT cycle

    else if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == '.'){
        // wasted one mov point without moving
        player_move_attack(0, 0, tgame, 0);
        combat_null = true;
    }

    else {
        m_x = 0;
        m_y = 0;
        if(player.AP != player.APm) return 0; // make monster execute while player is frozen
        else return no_turn;
    }
    
    
    }
    //std::cout << "player.x: " << duh.x << " player.y: " << duh.y << std::endl; 
    return 0;
}    

void player_death(Game &GAME){
    Fighter fighter_component(0, 0, 0, 0);
    Object_monster monster(player.x, player.y, 'i', TCODColor::black, TCODColor::black, 0, fighter_component);
    game_state = dead;
    player.selfchar = 'X';
    GAME.tileval.u8_player = 'X';

    bloodsplat(monster);
    bloodsplat(monster);
    bloodsplat(monster);
    bloodsplat(monster);
      
    TCODConsole::root->setAlignment(TCOD_CENTER);
    TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
    TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
    TCODConsole::root->print(win_x/2,win_y-5,"%cYour DEAD!!1ONE!%c",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
}

struct Unit { unsigned int mon_index;  };

struct Monster { int *initiative; int *speed; };

bool compare(Monster a, Monster b) {if (*(a.initiative) != *(b.initiative)) return (*(a.initiative) > *(b.initiative)); else return
    (*(a.speed) > *(b.speed));}

int init_UI(TCODConsole *r_panel, Game &GAME, const std::vector<Unit> &Phase, const std::vector<Monster> &monsters, const std::vector<Object_monster> &monvector, int init_ln, int phase, int turnseq){
    int line_temp = init_ln;
    if(phase == 1 || phase == 6) r_panel->setColorControl(TCOD_COLCTRL_1, TCODColor::black, TCODColor::lighterGreen);
    if(phase == 2 || phase == 7) r_panel->setColorControl(TCOD_COLCTRL_1, TCODColor::black, TCODColor::lighterBlue);
    if(phase == 3 || phase == 8) r_panel->setColorControl(TCOD_COLCTRL_1, TCODColor::black, TCODColor::lighterYellow);
    if(phase == 4 || phase == 9) r_panel->setColorControl(TCOD_COLCTRL_1, TCODColor::black, TCODColor::lightOrange);
    if(phase == 5 || phase == 10) r_panel->setColorControl(TCOD_COLCTRL_1, TCODColor::black, TCODColor::lightRed);
    r_panel->setColorControl(TCOD_COLCTRL_2, TCODColor::lightAmber, TCODColor::black);
    r_panel->setColorControl(TCOD_COLCTRL_3, TCODColor::lightYellow, TCODColor::black);
    r_panel->setColorControl(TCOD_COLCTRL_4, TCODColor::lightRed, TCODColor::black);
    r_panel->setColorControl(TCOD_COLCTRL_5, TCODColor::darkRed, TCODColor::black); // turn passed
    if(Phase.size() > 0){
        for (unsigned int i = 0; i<monsters.size(); ++i) { // cycle initiative
            if (i == (GAME.player->player_own)){ // was -1 - if player
                for(unsigned int n = 0; n<Phase.size(); ++n){ // cycle phase
                    if(Phase[n].mon_index == 666){ // is player in phase?
                        ++init_ln;
                        mon_list.push_back(255); // for mouse lookup on list
                        if(init_ln-2 < turnseq){ // turnseq == 2 if player 2nd phase
                            r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "%c[%d] Player%c %c %c", TCOD_COLCTRL_5, 
                                player.temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                        } else if(init_ln-2 == turnseq){
                            r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "[%c%d%c] Player < %c %c", TCOD_COLCTRL_2, 
                                GAME.player->temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                        }else{
                            r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "[%c%d%c] Player %c %c", TCOD_COLCTRL_2, 
                                GAME.player->temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                        }  
                    }    
                } 
            } else { // if initiative has monster   
                for (unsigned int b = 0; b<monvector.size(); ++b){
                    unsigned int monster_ini = monvector[b].initiative;
                    if(monvector[b].in_sight){
                        if ((i+1) == monster_ini ){ // find the monster by matching initiative
                            for(unsigned int n = 0; n<Phase.size(); ++n){ // cycle phase
                                if(Phase[n].mon_index == b){ // is monster in phase?
                                    mon_list.push_back(b); // for mouse lookup on list
                                    ++init_ln;
                                    if(init_ln-2 < turnseq){
                                        r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "%c[%d] %s%c %c %c",
                                            TCOD_COLCTRL_5, monvector[b].temp_init, monvector[b].name,
                                            TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                                    } else if(init_ln-2 == turnseq){
                                        r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "[%c%d%c] %c%s%c < %c %c", 
                                            TCOD_COLCTRL_2, monvector[b].temp_init, TCOD_COLCTRL_STOP,
                                            TCOD_COLCTRL_3, monvector[b].name, TCOD_COLCTRL_STOP, 
                                            TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                                    }else{
                                        r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "[%c%d%c] %c%s%c %c %c", 
                                            TCOD_COLCTRL_2, monvector[b].temp_init, TCOD_COLCTRL_STOP,
                                            TCOD_COLCTRL_3, monvector[b].name, TCOD_COLCTRL_STOP, 
                                            TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                                    }
                                } 
                            }
                        }    
                    } else {
                        if ((i+1) == monster_ini ){
                            for(unsigned int n = 0; n<Phase.size(); ++n){
                                if(Phase[n].mon_index == b){
                                    mon_list.push_back(256); // for mouse lookup on list
                                    ++init_ln;
                                    if(init_ln-2 < turnseq){
                                        r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "%c[%d] %s%cX%c %c", TCOD_COLCTRL_5,
                                            monvector[b].temp_init, monvector[b].name, TCOD_COLCTRL_STOP,
                                            TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                                    } else if(init_ln-2 == turnseq){    
                                        r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "[%c%d%c] %c%s%c%cX%c <%c %c", TCOD_COLCTRL_2,
                                            monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_3, 
                                            monvector[b].name, TCOD_COLCTRL_STOP, TCOD_COLCTRL_4, TCOD_COLCTRL_STOP, 
                                            TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                                    } else{
                                        r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "[%c%d%c] %c%s%c%cX%c%c %c", TCOD_COLCTRL_2,
                                            monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_3, 
                                            monvector[b].name, TCOD_COLCTRL_STOP, TCOD_COLCTRL_4, TCOD_COLCTRL_STOP, 
                                            TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                                    }    
                                } 
                            }
                        }    
                    }    
                }
            }
        }    
    } else {
        mon_list.push_back(256); // for mouse lookup on list
        ++init_ln;
        r_panel->print((win_x-MAP_WIDTH_AREA)-1, init_ln, "%c %c", TCOD_COLCTRL_1, 
                TCOD_COLCTRL_STOP);
    }    

    return init_ln - line_temp;
} 

// blink UI, not the action animation
// blink effect is within player_turn
void rangedraw(Game &GAME, const std::vector<Object_monster> &monsters){
    int nonx = 0; int nony = 0;
    int x = GAME.player->x;
    int y = GAME.player->y;

    if(GAME.player->ranged_target >= 0){ // if targeting a monster
        if(!monsters[GAME.player->ranged_target].alive){ // if target has been killed
            GAME.player->ranged_target = -1; // convert to point blank
        } else {
            GAME.player->tlocx = monsters[GAME.player->ranged_target].x; // update target pos
            GAME.player->tlocy = monsters[GAME.player->ranged_target].y;
        }  
        if(!GAME.gstate.fov_map->isInFov(monvector[GAME.player->ranged_target].x,monvector[GAME.player->ranged_target].y)){
            GAME.player->ranged_target = -2; // reset if gone out of fov
            GAME.player->aim = 0; // resets aim
            if(GAME.player->rangeaim > 2) GAME.player->rangeaim = 2; // resets rangephase, but not unlock target
            msg_log msgd;
            sprintf(msgd.message, "Target not visible."); 
            msg_log_list.push_back(msgd);
        }
    }

    TCODLine::init(GAME.player->x,GAME.player->y,GAME.player->tlocx,GAME.player->tlocy);
    while (!TCODLine::step(&x,&y)) {

        if (!GAME.gstate.bigg){
            nonx = x - GAME.gstate.off_xx;
            nony = y - GAME.gstate.off_yy;
        } else if (GAME.gstate.bigg){    
            nonx = ((x*2) - GAME.gstate.off_xx)-28;
            nony = ((y*2) - GAME.gstate.off_yy)-18;
        }

        TCODConsole::root->setDefaultForeground(TCODColor::yellow);
        //if(GAME.gstate.fov_map->isInFov(x,y))
            TCODConsole::root->putChar(nonx, nony, 178, TCOD_BKGND_SET);
        //else break;
    }  
    if(x != GAME.player->x || y != GAME.player->y){ 
        TCODConsole::root->setDefaultForeground(TCODColor::red);
        TCODConsole::root->putChar(nonx, nony, '*', TCOD_BKGND_SET);
    }
    return;
}    

void rangeanimation(Game &GAME, const std::vector<Object_monster> &monvector, const int whohit){

    TCOD_mouse_t mouse;
    TCOD_key_t key;
    uint32 millisecond = 0;
    int second = -2;

    int x = GAME.player->x;
    int y = GAME.player->y;
    int nonx = 0; int nony = 0;
    bool done = false;
    int step = 0;
    do{
        uint32 millicounter = TCODSystem::getElapsedMilli();
        if(millicounter > millisecond){ 
            millisecond = millicounter + 1000;
            ++second;
        }
        render_all(GAME);
        //I_am_moused(GAME);

        if(GAME.player->deflected) 
            TCODLine::init(GAME.player->x,GAME.player->y,GAME.player->deflx,GAME.player->defly);
        else TCODLine::init(GAME.player->x,GAME.player->y,GAME.player->tlocx,GAME.player->tlocy);
        while (!TCODLine::step(&x,&y)) {

            if (!GAME.gstate.bigg){
                nonx = x - GAME.gstate.off_xx;
                nony = y - GAME.gstate.off_yy;
            } else if (GAME.gstate.bigg){    
                nonx = ((x*2) - GAME.gstate.off_xx)-28;
                nony = ((y*2) - GAME.gstate.off_yy)-18;
            }

            TCODConsole::root->setDefaultForeground(TCODColor::yellow);
            if(GAME.gstate.fov_map->isInFov(x,y))
                TCODConsole::root->putChar(nonx, nony, 178, TCOD_BKGND_SET);
            TCODConsole::root->setDefaultForeground(TCODColor::grey);
            if(step == second){
                int *tgtx; int *tgty;
                if(GAME.player->deflected){ 
                    tgtx = &GAME.player->deflx; 
                    tgty = &GAME.player->defly;
                } else {
                    tgtx = &GAME.player->tlocx; 
                    tgty = &GAME.player->tlocy;
                }    
                if(GAME.gstate.fov_map->isInFov(x,y))
                    TCODConsole::root->putChar(nonx, nony, 'x', TCOD_BKGND_SET);
                else {done = true; break;} // stop when out of fov
                if(x == *tgtx && y == *tgty){ 
                    TCODConsole::root->setDefaultForeground(TCODColor::lightRed);
                    TCODConsole::root->putChar(nonx, nony, 'X', TCOD_BKGND_SET);
                }
            }    
            ++step;
        }
        if(second > step-1) done = true;
        step = 0;

        if(whohit >= 0);

        TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
        TCODConsole::flush(); // this updates the screen

    } while (!done);
    TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
    GAME.player->deflected = false;
    return;
}    

void rangekill(Game GAME, int whohit){
    if (monvector[whohit].stats.hp < 1){
        --killall;
        bloodsplat(monvector[whohit]);
        bloodycount = 23;

        monvector[whohit].selfchar = '%';
        monvector[whohit].color = monsterdead;
        monvector[whohit].blocks = false;
        monvector[whohit].alive = false;

        monvector[whohit].self_16 = 526;
        monvector[whohit].self_8 = 451; // was 506, why?

        player.move(0, 0, monvector); // updates player position so feet get bloody
        if(GAME.player->ranged_target != -1) // keeps point blank as valid
            GAME.player->ranged_target = -2; // resets target
    }
    return;
}    

int player_turn(Game &GAME, const std::vector<Monster> &monsters, std::vector<Unit> AllPhases[10], int turnseq, int phase){
    bool in_sight = false;
    GAME.gstate.con->clear();
    TCODConsole::root->clear();
    fetch = 0; // resets fetch to avoid skipping turn when swapping weapon (fetch assigned in mouse function)

    GAME.gstate.fov_recompute = true;
    render_all(GAME);

    GAME.gstate.first = false; // HOLD
    GAME.gstate.second = false; // MOVE
    GAME.gstate.third = false; // ATTACK 
    GAME.gstate.fourth = true; // PASS
    GAME.gstate.mode_move = false;
    GAME.gstate.mode_attack = false;
    GAME.gstate.mode_pass = false;

    if(phase < 9) GAME.gstate.first = true; // cannot hold at last phase
    if(player.phase <= phase+1) GAME.gstate.second = true;

    if (player.AP > 0 && !wid_prompt_open){
        UI_hook(GAME, 4); // hooks the prompt
        wid_prompt_open = true;
    }  

    // PLAYER BLOCK
    // PLAYER BLOCK
    bool actionflag = false; // used to skip mouse when command is already sent 
    int action = 0; // option selected in combat prompt 
    int phasemove = 0; // used to count movements done during phase
    bool exitcycle = false;

    bool blink = false;
    uint32 millisecond = 0;

    while (player.AP >= 1 && !exitcycle){

        // phaseAP = points left in phase
        player.phaseAP = 4 - phasemove;
        if(player.phaseAP > player.AP) player.phaseAP = player.AP; // phase AP can't exceed pool left

        if(player.att_phase > phase+1) GAME.gstate.third = false; // weapon speed, cannot attack
        else if(player.phaseAP >= player.attAP) GAME.gstate.third = true; // enough AP this phase, can attack
        else GAME.gstate.third = false; // retrigger false is phaseAP were modified

        if(player.phase_attack) phasemove = 4; // if attack happened, phase is spent (might break overpower?)

        bool didmove = false; // flag if player moved in this loop

        player_action = handle_keys(player, GAME);

        if(fetch != 0){ // fetch is global in main, only assigned when key is pressed 
            action = fetch; // return combat prompt selection from handle_key
            fetch = 0; // fetch is global
        } 

        if(action == 1){ // HOLD
            for(unsigned int n = 0; n<AllPhases[phase].size(); ++n){ // cycle this phase
                if(AllPhases[phase][n].mon_index == 666){ // is player in phase
                    //AllPhases[phase].erase(AllPhases[phase].begin()+n); // no need, i stays in history
                    Unit tempunit;
                    tempunit.mon_index = 666; // player flag
                    if(phase == 9) {player.AP = 0; break;} // if exausted move steps but phases maxed
                    AllPhases[phase+1].push_back(tempunit);
                    int init_ln = 2;
                    mon_list.clear(); // mouse lookup
                    for(unsigned int i = 0; i<10; ++i){
                        init_ln += init_UI(r_panel, GAME, AllPhases[i], 
                                monsters, monvector, init_ln, i+1, turnseq);
                    }
                    exitcycle = true;
                    break;
                }
            }    
        } 

        // needs exitcycle since HOLD executed by assinging that bool, then looping
        if(player.APburn > 0 && !exitcycle){ // after HOLD, since HOLD is called after four of APburn
            ++phasemove;
            --player.APburn;
            --player.AP;

            // reassigning to update the values going in the AP bar UI
            player.phaseAP = 4 - phasemove;
            if(player.phaseAP > player.AP) player.phaseAP = player.AP; // phase AP can't exceed pool left

            if(player.APburn == 0){ // execute armor swap
                switchweapon_ex(GAME);
            }  
            if(phasemove == 4){ // skipping phase
                msg_log msgd;
                sprintf(msgd.message, "Player skipping combat phase. Action in progress.");
                msg_log_list.push_back(msgd);
            }
            std::cout << "phasemove: " << phasemove << " " << player.APburn << std::endl;
        }

        if(action == 5){ // AIMING
            if(GAME.player->rangeaim < 2 || GAME.player->ranged_target != -2) // nock, draw, or improve aim 
                player_aim(GAME, phasemove, monvector);
            else{ // need to acquire target
                GAME.gstate.mode_attack = true; // for aim menu UI, right?
                ranged_target(GAME); // acquire target function
                if(GAME.player->ranged_target != -2){ // do only if now target exists 
                    player_aim(GAME, phasemove, monvector); // messages + takes 1 aim automatically
                    fetch = 1; // end phase, might need to be outside {} ?
                }    
            }    
            action = 0;
        }   

        if(action == 6){ // NEW TARGET AIMING
            if(GAME.player->rangeaim >= 2 && GAME.player->ranged_target != -2){ // if further phase and already targerting
                GAME.player->ranged_target = -2; // reset target
                GAME.player->rangeaim = 2; // reset phase back to drawing
                GAME.player->aim = 0; // resets aim
                GAME.gstate.mode_attack = true;
                ranged_target(GAME); // acquire target
                if(GAME.player->ranged_target != -2){ // do only if now target exists
                    player_aim(GAME, phasemove, monvector); // messages + takes 1 aim automatically
                    fetch = 1; // end phase, might need to be outside {} ?
                }    
            }    
            action = 0;
        }

        if(action == 4){ // PASS
            while(player.AP > 0){
                --player.AP;
            } 
            action = 1; // goes to "hold" to jump phase
        }

        if(action == 2){ // MOVE
            GAME.gstate.mode_move = true;
        }  

        if(action == 3 && GAME.gstate.third){ // ATTACK
            if(!GAME.player->rangeweapon) GAME.gstate.mode_attack = true; // if melee
            else if(GAME.player->aim == 0){ // if ranged and aim is 0
                msg_log msgd;
                sprintf(msgd.message, "Cannot fire bow, not enough aim.");
                msg_log_list.push_back(msgd);
                action = 0;
            } else { 
                //GAME.gstate.mode_attack = true;
                if(GAME.player->rangeaim < 3){ // shouldn't happen
                    msg_log msgd;
                    sprintf(msgd.message, "This is the wrong rangeaim phase.");
                    msg_log_list.push_back(msgd);
                } else {
                    int whohit = -1;
                    if (fire_target(GAME, phasemove, monvector, whohit)){
                        rangeanimation(GAME, monvector, whohit);
                        if (whohit >= 0) rangekill(GAME, whohit);
                    }    
                }    
                fetch = 1; // end phase
                actionflag = true; // skip mouse to not override command
            }
        }

        if (player_action == quit2){ // quit combat
            Sleep(100);
            combat_mode = false;
            GAME.gstate.iscombat = false;
            r_panel->clear();
            return 10;
        } // exits combat? 
        if (player_action == quit || TCODConsole::isWindowClosed()){
            return 666;
        } // exits program
     
        // moves only after MOVE command is selected
        if ((m_x != 0 || m_y != 0 || combat_null) && player.AP > 0 && action == 2 && phasemove < 4){
            player.move(m_x, m_y, monvector);
            ++phasemove;
            --player.AP;
            GAME.gstate.fov_recompute = true;
            didmove = true;
            combat_null = false; // reset waiting action
            if(GAME.player->rangeweapon && GAME.player->rangeaim >= 1){ 
                GAME.player->rangeaim = 1; // resets aim phase if moved
                GAME.player->aim = 0; // resets aim if moved
                GAME.player->ranged_target = -2; // resets target lock
            }    
        } else didmove = false; // player action

        // trying to handle combat/overpower without skipping phase automatically
        if(phasemove == 4 && !GAME.gstate.mode_attack) action = 1; // half movement exhausted, execute HOLD

        if(didmove){
            // add monster blocks 
            for (unsigned int u = 0; u<monvector.size(); ++u){
                GAME.gstate.fov_map_mons_path1->setProperties(monvector[u].x, monvector[u].y, 1, 0);
            }
            // monsters chasing and not in FoV do not ger updated 
            for (unsigned int n = 0; n<monvector.size(); ++n) {
                // player sees monster
                in_sight = GAME.gstate.fov_map->isInFov(monvector[n].x,monvector[n].y);

                if(in_sight && monvector[n].alive == true){
                    monvector[n].c_mode = true;
                    monvector[n].pl_x = player.x; // if player in sight, store player pos
                    monvector[n].pl_y = player.y;
                    monvector[n].chasing = true;
                    monvector[n].bored = 400;
                    monvector[n].boren = false;
                    monvector[n].stuck = false;

                    if(monvector[n].path_mode == 1)
                        monvector[n].path1->compute(monvector[n].x,monvector[n].y,monvector[n].pl_x,monvector[n].pl_y);
                    else monvector[n].path0->compute(monvector[n].x,monvector[n].y,monvector[n].pl_x,monvector[n].pl_y);
                }
            }
            // remove monster blocks
            for (unsigned int u = 0; u<monvector.size(); ++u){
                GAME.gstate.fov_map_mons_path1->setProperties(monvector[u].x, monvector[u].y, 1, 1);
            }
        }

        // SIDEBAR UI
        r_panel->clear();
        TCODConsole::root->setAlignment(TCOD_RIGHT);

        r_panel->print((win_x-MAP_WIDTH_AREA)-1, 1, "Initiative list");
        //TCODConsole::root->print(win_x-1, 3, "Initiative list");

        // Initiative UI list in player turn
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightAmber,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::darkRed, TCODColor::black);

        int init_ln = 2;
        mon_list.clear(); // mouse lookup
        for(unsigned int i = 0; i<10; ++i){
            init_ln += init_UI(r_panel, GAME, AllPhases[i], monsters, monvector, init_ln, i+1, turnseq);
        }

        // COMBAT UI
        panel->clear();

        if(msg_log_list.size() > 0){
            Message_Log(GAME);
        }
        render_all(GAME);

        // used to blink rangedraw()
        uint32 millicounter = TCODSystem::getElapsedMilli();
        if(millicounter > millisecond){ 
            millisecond = millicounter + 500;
            blink = !blink;
        }
        if(GAME.player->rangeweapon && GAME.player->ranged_target != -2 && blink && !MSG_MODE_XTD) 
            rangedraw(GAME, monvector);

        if(!actionflag) I_am_moused(GAME); // skip mouse if command is in queue
        TCODConsole::flush(); // this updates the screen
    }
    // PLAYER BLOCK
    // PLAYER BLOCK

    UI_unhook(GAME, 4); // unhooks the prompt
    wid_prompt_open = false;

    player.phase_attack = 0; // used in player_move_attack to stop consequent attacks
    GAME.gstate.fov_recompute = true;
    render_all(GAME);
    TCODConsole::flush(); // this updates the screen

    GAME.player->forcedswap = false; // used when forcing swap weapon
    
    return 0;
}  

int monster_turn(Game &GAME, const std::vector<Monster> &monsters, unsigned int i, std::vector<Unit> AllPhases[10], int turnseq, int phase, int phaseplace){
    for (unsigned int b = 0; b<monvector.size(); ++b) {
        unsigned int monster_ini = monvector[b].initiative;

        // MONSTER BLOCK
        // MONSTER BLOCK
        if ((i+1) == monster_ini && monvector[b].alive){ // this monster found & alive

            TCODConsole::root->setAlignment(TCOD_CENTER);
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::yellow);

            TCODConsole::root->clear();
            GAME.gstate.fov_recompute = true;
            render_all(GAME);
            r_panel->clear();
            widget_popup->clear();
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
            widget_popup->print(0, 0, "%cMONSTER TURNS%c",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
            TCODConsole::blit(widget_popup,0,0,13,1,TCODConsole::root, (MAP_WIDTH_AREA/2)-6, 66);

            // Initiative UI list in monster turn
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightAmber,TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::darkRed, TCODColor::black);

            int init_ln = 2;
            for(unsigned int i = 0; i<10; ++i){
                init_ln += init_UI(r_panel, GAME, AllPhases[i], monsters, monvector, init_ln, i+1, turnseq);
            }
        
            TCODConsole::flush(); // this block to update player rendering before monster TURN

            bool seehere = false;
            seehere = GAME.gstate.fov_map->isInFov(monvector[b].x,monvector[b].y);
            int ctl = 0;
            while (monvector[b].combat_move > 0 && !monvector[b].pass){
                // take turn (monster)
                if (monvector[b].myai->take_turn(monvector[b], player, monvector[b].pl_x,
                            monvector[b].pl_y,seehere, GAME) );// render_all();

                if (player.stats.hp < 1 && !alreadydead ){
                    player_death(GAME);
                    alreadydead = 1;
                    return 5;
                } 

                if (ctl > 1){
                    GAME.gstate.con->clear();
                    GAME.gstate.fov_recompute = true;
                    ctl = 0;
                }

                // jump to next phase if movement points (enough for attack) are left
                if(monvector[b].pass && monvector[b].combat_move >= 1 && phase < 9){
                    Unit tempunit;
                    tempunit.mon_index = b;
                    AllPhases[phase+1].push_back(tempunit);
                    int init_ln = 2;
                    mon_list.clear(); // mouse lookup
                    for(unsigned int z = 0; z<10; ++z){
                        init_ln += init_UI(r_panel, GAME, AllPhases[z], 
                                monsters, monvector, init_ln, z+1, turnseq);
                    }
                }

                widget_popup->clear();
                TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
                widget_popup->print(0, 0, "%cMONSTER TURNS%c", TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                render_all(GAME);
                TCODConsole::blit(widget_popup,0,0,13,1,TCODConsole::root, (MAP_WIDTH_AREA/2)-6, 66);
                TCODConsole::flush();
                Sleep(100);
                ++ctl; // for trail animation on monster movement
            }
            monvector[b].hasmoved = false; // resets movement flag for next phase
            monvector[b].pass = false; // reset
            monvector[b].phase_attack = 0;
            monvector[b].step = 0;
            // couldn't move this turn ?
            if(monvector[b].stuck == true) ++monvector[b].wasstuck;
            else monvector[b].wasstuck = 0;
        } // if
        // MONSTER BLOCK
        // MONSTER BLOCK

    } // for
    return 0;
} 

int main() {

    // color test
    int colr = 0;
    std::string color = "#FFFFFF";
    colr = std::stoi(color.substr(1,2), nullptr, 16);
    std::cout << colr << std::endl;    

    // path test
    TCODMap *myMap = new TCODMap(50,50);
    myMap->clear(1,1);
    TCODPath *path = new TCODPath(myMap); // allocate the path
    path->compute(5,5,25,25);
    int pathj;
    int pathr;
    path->getOrigin(&pathj, &pathr);
    std::cout << "Path origin: " << pathj << " " << pathr << std::endl;
    path->getDestination(&pathj, &pathr);
    std::cout << "Path destination: " << pathj << " " << pathr << std::endl;
    std::cout << "Path size: " << path->size() << std::endl;
    std::cout << "Is path empty: " << path->isEmpty() << std::endl;

    Game GAME;
    GAME.setup();

    GAME.gstate.fov_map_mons_path0 = new TCODMap(220,140);

    // ID, x, y, w, h
    UI_register(GAME, 0, 0, 0, 127, 0); // top panel
    UI_register(GAME, 1, 110, 3, 17, 66); // right panel
    UI_register(GAME, 2, 0, 72, 127, 18); // combat bottom
    UI_register(GAME, 3, 34, 12, 93, 78); // extended message log
    UI_register(GAME, 4, 1, 78, 12, 7); // player combat prompt
    //UI_register(GAME, 101, 34, 12, 93, 0); // message log filter bar (extended)
    GAME.gstate.MLfilter101 = true; // initiative messages enabled

    GAME.gstate.mapmode = 1;

    //TCODConsole::setCustomFont("arial10x10.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
    //TCODConsole::setCustomFont("terminal.png",TCOD_FONT_LAYOUT_ASCII_INCOL,16,256);
    TCODConsole::setCustomFont("terminal.png",TCOD_FONT_LAYOUT_ASCII_INCOL,16,256);
    //TCODConsole::setCustomFont("terminal2.png",TCOD_FONT_LAYOUT_ASCII_INCOL,16,256);
    //TCODConsole::setCustomFont("sample_full_unicode.png",TCOD_FONT_LAYOUT_ASCII_INROW,32,2048);
    TCODConsole::initRoot(win_x, win_y, "FoE", false, TCOD_RENDERER_SDL);
    TCODSystem::setFps(LIMIT_FPS);

        
    myvector.push_back(&player);
   
    player.colorb = GAME.gstate.con->getCharBackground(player.x, player.y);
    player.colorb = GAME.gstate.color_dark_ground;

    strcpy(player.name, "Playername");
    GAME.player = &player; // link player in GAME object, to local object

    GAME.gstate.fov_recompute = true;
    game_state = playing;

    // SETS MORE PLAYER ATTRIBUTES
    player.combat_move = 8; // movement points
    player.speed = 6;
    // weapon setup
    player.stats.wpn1.wpn_AC = 3;
    player.stats.wpn1.wpn_DC = 2;
    player.stats.wpn1.wpn_B = 3;
    player.stats.wpn1.wpn_E = 5;
    player.stats.wpn1.wpn_P = 3;
    player.stats.wpn1.wp_B.first = 3;
    player.stats.wpn1.wp_B.second = 0;
    player.stats.wpn1.wp_E.first = 5;
    player.stats.wpn1.wp_E.second = 1;
    player.stats.wpn1.wp_P.first = 3;
    player.stats.wpn1.wp_P.second = 2;
    player.stats.wpn1.wpn_aspect = 2;
    player.stats.wpn1.reach = 4;

    player.skill.rangedAML = 65;

    player.stats.ML = 60;

    // unit size = fast
    player.phase = 2; // 1veryfast 2fast 3average 4slow 5veryslow 
    player.att_phase = 3; // boardsword is average
    player.wpn_phase = 3; // boardsword is average

    player.rangeweapon = 0; // set melee as default
    player.eRangedDW = 80; // Ranged Weapon default draw weight
    player.rangedD1 = 5; // weapon Close range in cells
    player.rangedD2 = 15; // weapon Very Short range in cells
    player.rangedD3 = 25; // weapon Short range in cells
    player.rangedD4 = 50; // weapon Medium range in cells
    player.rangedD5 = 100; // weapon Long range in cells
    player.rangedD6 = 200; // weapon Extreme range in cells

    player.forcedswap = false; // used to burn AP on weapon swap
    player.APm = 8; // new movement points
    player.AP = player.APm; // movement points
    player.phaseAP = 0;
    player.APburn = 0; // used APs
    player.rangeaim = 0; // the aim phase
    player.aim = 0; // reset aim %
    player.attAP = 4;
    player.ranged_target = -2; // -2 no target, -1 point blank, 0+ monster index

    build_armor(GAME);

    bool quit_now = false;

    TCODConsole::mapAsciiCodeToFont(666,0,223);
    TCODConsole::mapAsciiCodeToFont(667,0,231);
    TCODConsole::mapAsciiCodeToFont(668,0,227);
    TCODConsole::mapAsciiCodeToFont(669,0,232);

    map_8x16_font(); // DOS font for readable text, not used
    map_16x16_tile(); // mapping bigg tiles, in map16.cpp
    map_c64_font();

    TCODConsole::mapAsciiCodeToFont(400,2,16);
    TCODConsole::mapAsciiCodeToFont(401,2,17);
    TCODConsole::mapAsciiCodeToFont(402,2,18);
    TCODConsole::mapAsciiCodeToFont(403,2,19);
    TCODConsole::mapAsciiCodeToFont(404,2,20);
    TCODConsole::mapAsciiCodeToFont(405,2,21);
    TCODConsole::mapAsciiCodeToFont(406,2,22);
    TCODConsole::mapAsciiCodeToFont(407,2,23);
    TCODConsole::mapAsciiCodeToFont(408,2,24);
    TCODConsole::mapAsciiCodeToFont(409,2,25);
    TCODConsole::mapAsciiCodeToFont(410,2,26);
    TCODConsole::mapAsciiCodeToFont(411,2,27);
    TCODConsole::mapAsciiCodeToFont(412,2,28);
    TCODConsole::mapAsciiCodeToFont(413,2,29);
    TCODConsole::mapAsciiCodeToFont(414,2,30);
    TCODConsole::mapAsciiCodeToFont(415,2,31);
    TCODConsole::mapAsciiCodeToFont(416,2,32); // subcells custom for player on minimap
    TCODConsole::mapAsciiCodeToFont(417,2,33);
    TCODConsole::mapAsciiCodeToFont(418,2,34);
    TCODConsole::mapAsciiCodeToFont(419,2,35);
    TCODConsole::mapAsciiCodeToFont(420,2,36);
    TCODConsole::mapAsciiCodeToFont(421,2,37);
    TCODConsole::mapAsciiCodeToFont(422,2,38);
    TCODConsole::mapAsciiCodeToFont(423,2,39);
    TCODConsole::mapAsciiCodeToFont(424,2,40);
    TCODConsole::mapAsciiCodeToFont(425,2,41);
    TCODConsole::mapAsciiCodeToFont(426,2,42);
    TCODConsole::mapAsciiCodeToFont(427,2,43);
    TCODConsole::mapAsciiCodeToFont(428,2,44);
    TCODConsole::mapAsciiCodeToFont(429,2,45);
    TCODConsole::mapAsciiCodeToFont(430,2,46);
    TCODConsole::mapAsciiCodeToFont(431,2,47);
    TCODConsole::mapAsciiCodeToFont(432,2,48);
    TCODConsole::mapAsciiCodeToFont(433,2,49);

    TCODConsole::mapAsciiCodeToFont(444,9,16); // door 8x8
    TCODConsole::mapAsciiCodeToFont(445,9,17); // player 8x8
    GAME.tileval.t8_player = 445;
    TCODConsole::mapAsciiCodeToFont(446,9,18); // wall 8x8
    TCODConsole::mapAsciiCodeToFont(447,9,19); // floor 8x8
    TCODConsole::mapAsciiCodeToFont(448,9,20); // floor.b 8x8
    TCODConsole::mapAsciiCodeToFont(449,9,21); // orc 8x8
    TCODConsole::mapAsciiCodeToFont(450,9,22); // troll 8x8
    TCODConsole::mapAsciiCodeToFont(451,9,23); // corpse 8x8

    GAME.tileval.u16_door = GAME.tileval.t28_door;
    GAME.tileval.u16_doorv = GAME.tileval.t28_door; // same door 
    GAME.tileval.u16_player = GAME.tileval.t28_player; 
    GAME.tileval.u16_wall = GAME.tileval.t28_wall; 
    GAME.tileval.u16_vwall = GAME.tileval.t28_nwall;
    GAME.tileval.u16_trwall = GAME.tileval.t28_nwall;
    GAME.tileval.u16_tlwall = GAME.tileval.t28_nwall;
    GAME.tileval.u16_bwall = GAME.tileval.t28_wall;
    GAME.tileval.u16_ibwall = GAME.tileval.t28_wall;
    GAME.tileval.u16_floor1 = GAME.tileval.t28_floor1; 
    GAME.tileval.u16_floor2 = GAME.tileval.t28_floor2; 
    GAME.tileval.u16_orc = GAME.tileval.t28_orc; 
    GAME.tileval.u16_troll = GAME.tileval.t28_troll; 
    GAME.tileval.u16_corpse = GAME.tileval.t28_corpse;

    //standard 8mode: ASCII 
    GAME.tileval.u8_door = TCOD_CHAR_CROSS;
    GAME.tileval.u8_doorv = TCOD_CHAR_CROSS; // same door 
    GAME.tileval.u8_player = '@'; // 445 
    GAME.tileval.u8_wall = 668; 
    GAME.tileval.u8_vwall = 667;
    GAME.tileval.u8_trwall = 669;
    GAME.tileval.u8_tlwall = ' ';
    GAME.tileval.u8_bwall = ' ';
    GAME.tileval.u8_ibwall = GAME.tileval.t28_wall;
    GAME.tileval.u8_floor1 = '.'; 
    GAME.tileval.u8_floor2 = ',';
    GAME.tileval.u8_floor3 = '\'';
    GAME.tileval.u8_floor4 = '`';
    GAME.tileval.u8_orc = 'o'; 
    GAME.tileval.u8_troll = 'T'; 
    GAME.tileval.u8_corpse = '%';
    GAME.tileval.u8_hwall = 666;

    GAME.tileval.u8_wallCa = color_dark_wallF;
    GAME.tileval.u8_wallCb = color_light_wall;
    GAME.tileval.u8_wallCc = color_dark_wallF;
    GAME.tileval.u8_wallCd = color_light_wall;
    GAME.tileval.u8_floorCa = TCODColor::white;
    GAME.tileval.u8_floorCb = TCODColor::grey;
    color_light_ground = TCODColor::grey;
    
    TCODColor tempcol1 = TCODColor::lightGrey;

    
    TCODConsole::root->clear();
    print_8x16(TCODConsole::root, 10, 5, "In eternity, where there is no time,", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 11, 7, "nothing can grow.", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 12, 9, "Nothing can become.", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 13, 11, "Nothing changes.", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 14, 13, "So Death created time", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 15, 15, "to grow the things that it would kill...", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 16, 17, "and you are reborn", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 17, 19, "but into the same life", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 18, 21, "that you've always been born into.", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 19, 23, "I mean, how many times have we", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 20, 25, "had this conversation?", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 21, 27, "Well, who knows?", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 22, 29, "When you can't remember your lives,", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 23, 31, "you can't change your lives,", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 24, 33, "and that is the terrible and the secret fate of all life.", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 25, 35, "You're trapped...", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 26, 37, "by that nightmare you keep waking up into.", tempcol1, TCODColor::black);
    print_8x16(TCODConsole::root, 10, 41, "-Rustin \"Rust\" Cohle, True Detective", TCODColor::white, TCODColor::black);
    TCODConsole::flush();
    
    TCOD_event_t eve;
    TCOD_key_t key;
    TCOD_mouse_t mouse;
    do {
        TCODConsole::flush(); // idles CPU to match FPS rate
        eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
    }  while ( eve != TCOD_EVENT_KEY_PRESS && eve != TCOD_EVENT_MOUSE_RELEASE );

    // prepares a background to blit when offmap (within render_all)
    //TCODImage *pix = new TCODImage("bg.bmp");
    //pix->blit2x(border, 0, 0, -1, -1);

    TCODConsole::root->clear();
    std::vector<std::string> vecstr;
    std::string st1 = "Generate &New Character";
    std::string st2 = "&Random Character";
    std::string st3 = "&QUIT";
    vecstr.push_back(st1);
    vecstr.push_back(st2);
    vecstr.push_back(st3);

    print_c64(TCODConsole::root, 5, 10, ">", TCODColor::lightGrey, TCODColor::black);
    eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
    TCODConsole::flush();
    eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
    int fade = 0;
    int colfr = 0;
    int colfg = 0;
    int colfb = 0;
    while(fade < 256 &&  (eve != TCOD_EVENT_KEY_PRESS && eve != TCOD_EVENT_MOUSE_RELEASE) ){
        TCODColor fadec(colfr, colfg, colfb);
        print_c64(TCODConsole::root, 6, 10, "FEAR OF EXISTENCE.", fadec, TCODColor::black);
        TCODConsole::flush();
        eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
        colfr = colfr + 5;
        colfg = colfg + 5;
        colfb = colfb + 5;
        fade = fade + 5;
    }
    print_c64(TCODConsole::root, 6, 10, "FEAR OF EXISTENCE.", TCODColor::white, TCODColor::black);

    /*
    std::vector<int> buckets( 100 );
    for( int i = 0; i < 100000; ++i ) { ++buckets.at(rng(0,99 ) ); }
    for( int i = 0; i < 100; ++i ) { std::cout << buckets[ i ] << "\t"; }
    */

    int menu_index = 1;

    while(menu_index == 1){
        menu_index = UI_menu(19, 29, vecstr, 0);
        if(menu_index == -1 || menu_index == 3) return 0;
        if(menu_index == 2) break;
        TCODConsole::root->clear();
        if(menu_index == 1) menu_index = chargen(GAME);
        if(menu_index == -1) return 0;
        else if(menu_index == 0) break;
        TCODConsole::root->clear();
        print_c64(TCODConsole::root, 6, 10, "FEAR OF EXISTENCE.", TCODColor::white, TCODColor::black);
    } 

    // test custom
    if(menu_index == 2){
        // defaults for quick play
        player.skill.lswdAML = 90;
        player.skill.lswdDML = 85;
        player.stats.hp = 120;
        player.stats.max_hp = 120;
        GAME.player->skill.initML = 80;
        player.skill.bowML = 50;

        player.STR = 15;
        player.AGI = 6;
        GAME.player->APm = 12;
        GAME.player->AP = 12;

        for(unsigned int n = 0; n < 16; ++n){ // fixes crash for non generated char
            armorsection armortemp;
            armortemp.B = 5;
            armortemp.E = 6;
            armortemp.P = 3;
            GAME.player->armor_tot.push_back(armortemp);
        }
    }    

    TCODConsole::root->clear();
    vecstr.clear();
    st1 = "Ramdom BSP map";
    st2 = "Load csv map";
    vecstr.push_back(st1);
    vecstr.push_back(st2);
    menu_index = 1;
    while(menu_index == 1){
        menu_index = UI_menu(19, 29, vecstr, 0);
        if(menu_index == 2) {load_map(player, GAME); break;}
        if(menu_index == 1) {make_map_BSP(player, GAME); break;}
        if(menu_index == -1) return 0;
        else if(menu_index == 0) break;
        TCODConsole::root->clear();
    }
    TCODConsole::root->clear();
   
    // creates FOVs/paths
    for (int i = 0; i < MAP_HEIGHT; ++i){
        for (int l = 0; l < MAP_WIDTH; ++l) {
            GAME.gstate.fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
        }
    }

    for (int i = 0; i < MAP_HEIGHT; ++i){
        for (int l = 0; l < MAP_WIDTH; ++l) {
            GAME.gstate.fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
        }
    }

    for (int i = 0; i < MAP_HEIGHT; ++i){
        for (int l = 0; l < MAP_WIDTH; ++l) {
            GAME.gstate.fov_map_mons_path0->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
        }
    }

    for (int i = 0; i < MAP_HEIGHT; ++i){
        for (int l = 0; l < MAP_WIDTH; ++l) {
            GAME.gstate.fov_map_mons_path1->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
        }
    }
    

    wid_top_open = 1; // default on for top bar
    UI_hook(GAME, 0); // hook in the panel

    //uint32 timin1 = 0;
    //uint32 timin2 = 0; 

    std::vector<Unit> AllPhases[10];

    //DEBUG DEBUG DEBUG DEBUG
    //player.skill.bowML = 90;

    while (! TCODConsole::isWindowClosed()) {
        
        // FPS debug
        /* 
        ++fpscount;
        if(fpscount > LIMIT_FPS) fpscount = 0;
        std::cout << fpscount << " " << "pl.x " << player.x;
        */

        jump:

        int player_action = 0;

        render_all(GAME); 
        
        I_am_moused(GAME);

        TCODConsole::flush(); // this updates the screen

        if(event_sender >= 0) event_description(GAME, event_sender); // open windows event
        event_sender = -1; // reset
        
        bool in_sight;

        combat_mode = false;
        GAME.gstate.iscombat = false;

        if (!GAME.gstate.no_combat){ // debug flag

            for (unsigned int i = 0; i<monvector.size(); ++i) {
                in_sight = GAME.gstate.fov_map->isInFov(monvector[i].x,monvector[i].y);
                if(in_sight && monvector[i].alive == true){ 
                    combat_mode = true; // trigger combat mode, if monster is sighted
                    GAME.gstate.iscombat = true; // stre flag into GAME object when in combat loop
                    player.rangeaim = 0; // reset aim status
                    player.aim = 0; // reset aim %

                    if(!wid_combat_open){ 
                        wid_combat_open = 1; // default combat panels on
                        UI_hook(GAME, 2);
                    }    
                    if(!wid_rpanel_open){
                        wid_rpanel_open = 1;
                        UI_hook(GAME, 1);
                    }    

                    break;
                }    
            } // activates combat mode as soon a monster is in sight, deactivates on subsequent loops

        int turn = 1;  
        std::vector<Monster> monsters; // initiative - moved here to keep it valid between turns
        while (combat_mode){

            is_handle_combat = true;
            
            // intial and cycle combat resets
            for (unsigned int i = 0; i<monvector.size(); ++i) { 
                if(turn == 1){ 
                    monvector[i].initiative = -1; // on turn 1 resets initiative for all monsters
                    monvector[i].temp_init = -1;
                    monvector[i].distance = monvector[i].stats.wpn1.reach;
                }
                monvector[i].cflag_attacks = 0; // resets attacks received
                monvector[i].move_counter = 0; // resets number of movements during turn
                monvector[i].hasmoved = false; // reset flag for monster phases
                monvector[i].phase_attack = 0; // reset attack count for monster phases
                monvector[i].pass = false; // reset bool used for phases
                monvector[i].step = 0; // movements each phase
            }
            player.cflag_attacks = 0; // resets number of attacks received
            player.move_counter = 0; // resets number of movements during turn

            if (alreadydead) break;

            // INITIATIVE PURGE
            monsters.clear(); 

            msg_log msg2;
            sprintf(msg2.message, "%cROUND START%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            msg2.filter = 100; // FULL filter
            msg2.color1 = TCODColor::magenta;
            msg_log_list.push_back(msg2);
         
            GAME.gstate.con->clear();
            TCODConsole::root->clear(); 
            GAME.gstate.fov_recompute = true;
            render_all(GAME);
            TCODConsole::flush(); // this updates the screen
                
            bool break_combat = true;

            // updates monster map so that pathing includes monsters position
            for (unsigned int i = 0; i<monvector.size(); ++i){
                if(monvector[i].alive){ // dead monsters don't block pathing
                    // sight - walk, 1 = pass
                    GAME.gstate.fov_map_mons_path1->setProperties(monvector[i].x, monvector[i].y, 1, 0);
                }    
            }

            // clear all phases
            for(unsigned int i = 0; i<10; ++i){
                AllPhases[i].clear();
            }

            // populate turn phases with everything visible
            for (unsigned int i = 0; i<monvector.size(); ++i){
                if(GAME.gstate.fov_map->isInFov(monvector[i].x,monvector[i].y) || monvector[i].chasing){
                    if(monvector[i].phase == 3){
                        Unit tempunit;
                        tempunit.mon_index = i;
                        AllPhases[2].push_back(tempunit); // hardcoded to "average"?
                    }            
                }    
            }  
            // player added to phase
            Unit tempunit;
            tempunit.mon_index = 666; // player flag
            AllPhases[player.phase-1].push_back(tempunit);

            // phase output
            for(unsigned int n = 0; n<10; ++n){
                std::cout << "Phase " << n+1 << ": ";
                for(unsigned int i = 0; i<AllPhases[n].size(); ++i){
                    std::cout << AllPhases[n][i].mon_index << " ";
                }  
                std::cout << std::endl;
            }

            for (unsigned int i = 0; i<monvector.size(); ++i){
                in_sight = GAME.gstate.fov_map->isInFov(monvector[i].x,monvector[i].y);
                if(!in_sight) monvector[i].in_sight = false; 
                else monvector[i].in_sight = true;
                if( (in_sight && monvector[i].alive) || (monvector[i].chasing && monvector[i].alive) ){
                    monvector[i].c_mode = true;
                    monvector[i].pl_x = player.x; // if player in sight, store player pos
                    monvector[i].pl_y = player.y;
                    monvector[i].chasing = true;
                    monvector[i].bored = 400;
                    monvector[i].boren = false;
                    monvector[i].stuck = false;

                    if(monvector[i].wasstuck >= 3){ 
                        monvector[i].path_mode = 1; // switch to monster block path
                        monvector[i].wasstuck = 0;
                        monvector[i].path1->compute(monvector[i].x,monvector[i].y,monvector[i].pl_x,monvector[i].pl_y);
                    }    
                    monvector[i].path0->compute(monvector[i].x,monvector[i].y,monvector[i].pl_x,monvector[i].pl_y);
                    
                    std::cout << "Player: " << monvector[i].pl_x << " " << monvector[i].pl_y << std::endl;
                    std::cout << "Monster: " << monvector[i].x << " " << monvector[i].y << std::endl;
                    std::cout << "Monster path size: " << monvector[i].path0->size() << std::endl;

                    // trying to exit combat if no monster in sight EVEN while letting monster out of FoV chasing
                    if( in_sight && monvector[i].alive ){
                        break_combat = false; // set flag, so that if this cycle never entered, combat is interrupted
                    }    

                    // initiative monster block
                    bool wasincombat = false;
                    if(monvector[i].initiative != -1) wasincombat = true; // alredy joined
                    int roll = 0;
                    roll = rng(1, 20);
                    monvector[i].initiative = monvector[i].initML + (roll - 10);
                    monvector[i].temp_init = monvector[i].initiative; // temp_init keeps original roll

                    Monster tempm;
                    tempm.initiative = &monvector[i].initiative;
                    tempm.speed = &monvector[i].initML;
                    monsters.push_back(tempm); // pushed to initiative vector

                    if(turn > 1 && !wasincombat){
                        msg_log msg2;
                        sprintf(msg2.message, "%c!%cJoining the fray: %s", 
                                TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, monvector[i].name);
                        msg2.color1 = dicec;
                        msg2.filter = 102; // general filter
                        msg_log_list.push_back(msg2);
                    }

                    msg_log msg1;
                    if(monvector[i].in_sight)
                        sprintf(msg1.message, "%c>%c%s initiative: Skill(%d%%) %c1d20%c(%d) - 10 Total: %d",
                                TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, 
                                monvector[i].name,*tempm.speed, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, roll, 
                                monvector[i].initiative); 
                    else 
                        sprintf(msg1.message, "%c>***%c%s initiative: Skill(%d%%) %c1d20%c(%d) - 10 Total: %d",
                                TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,
                                monvector[i].name,*tempm.speed, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, roll, 
                                monvector[i].initiative);
                    msg1.color1 = dicec;
                    msg1.filter = 101; // initiative filter
                    msg_log_list.push_back(msg1);
                }
            }

            // shoouldn't happen, the check for monster alive already happens above
            if(turn > 1){
                for(unsigned int b = 0; b<monvector.size(); ++b){ // cycle all monsters
                    for(unsigned int i = 0; i<monsters.size(); ++i){ // cycle initiative
                        unsigned int monster_ini = monvector[b].initiative;
                        if ((i+1) == monster_ini){ // find the monster by matching initiative
                            if(!monvector[b].alive){ 
                                monvector[b].initiative = -1;
                                monsters.erase(monsters.begin()+i);
                                break; // breaks initiative for
                            }    
                        }
                    }
                    //monvector[b].initiative = monvector[b].temp_init; // resets to original values
                }
                // if this part executes then initiative values are replaced
                // so when the thing is re-sorted in the player block, it resorts the wrong values
                /*
                player.initiative = player.temp_init; // reset player initiative to original values too  
                std::sort(monsters.begin(), monsters.end(), compare); // re-sort for new monsters
                // sorting initiative after dead monsters removed
                for (unsigned int i = 0; i<monsters.size(); ++i) {
                    *(monsters[i].initiative) = i+1;
                }
                */
            }

            // clear monster positions before they move
            for (unsigned int i = 0; i<monvector.size(); ++i){
                GAME.gstate.fov_map_mons_path1->setProperties(monvector[i].x, monvector[i].y, 1, 1);
            }
             
            if (break_combat) {
                r_panel->clear(); 
                break;
            } // break combat mode if monsters all dead or out of FoV

            GAME.gstate.con->clear();
            r_panel->clear();
            TCODConsole::root->clear();

            // roll initiative (player)
            player.distance = player.stats.wpn1.reach; // initial reach
            player.phase_attack = 0;
            int myroll = 0;
            myroll = rng(1, 20);
            player.initiative = GAME.player->skill.initML + (myroll - 10);
            player.temp_init = player.initiative;


            //if initiative = xx, then
                for(unsigned int p = 0; p<10; ++p){
                    for(unsigned int i = 0; i<AllPhases[p].size(); ++i){
                        if(AllPhases[p][i].mon_index == 666){
                            AllPhases[p].erase(AllPhases[p].begin() + i); // erase
                        }
                    }
                }
            AllPhases[player.phase-1].push_back(tempunit); //re-add 

            Monster tempm; // player counts as monster for initiative
            tempm.initiative = &player.initiative;
            tempm.speed = &GAME.player->skill.initML;
            monsters.push_back(tempm);

            msg_log msg1;
            sprintf(msg1.message, "%c>%c%c>%cPlayer initiative: Skill(%d%%) %c1d20%c(%d) - 10 Total: %d",
                    TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                    *tempm.speed, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, myroll, player.initiative);
            msg1.color1 = dicec;
            msg1.color2 = TCODColor::lighterBlue;
            msg1.filter = 101; // initiative filter
            msg_log_list.push_back(msg1);

            // SORTING INITIATIVE
            std::sort(monsters.begin(), monsters.end(), compare);
            for (unsigned int i = 0; i<monsters.size(); ++i) {
                *(monsters[i].initiative) = i+1;
            }

            r_panel->setAlignment(TCOD_RIGHT);
            widget_top->print(win_x-6, 0, "%c%c%c%c%c%c%c%cMode-C%c", TCOD_COLCTRL_FORE_RGB,255,255,255,
                    TCOD_COLCTRL_BACK_RGB,0,0,0,TCOD_COLCTRL_STOP);
            r_panel->print((win_x-MAP_WIDTH_AREA)-1, 1, "Initiative list");

            //unsigned int player_own = player.initiative;
            player.player_own = player.initiative - 1;

            // Create initiative UI list
            r_panel->setBackgroundFlag(TCOD_BKGND_SET);
            r_panel->setDefaultForeground(TCODColor::white);
            int init_ln = 2;
            for(unsigned int i = 0; i<10; ++i){
                init_ln += init_UI(r_panel, GAME, AllPhases[i], monsters, monvector, init_ln, i+1, 0);
            } 

            // press a key to begin combat
            press_key_combat(widget_popup);
            
            GAME.gstate.fov_recompute = true;
            render_all(GAME);
            TCODConsole::blit(widget_popup,0,0,36,3,TCODConsole::root, 40, 65);
            TCODConsole::flush(); // this updates the screen

            TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,0,0,true);

            mon_list.clear(); // mouse lookup

            // TURN SEQUENCE
            int turnseq = 0; // raw line number on the initiative list 
            for(unsigned int p = 0; p<10; ++p){ // cycle phases
                if(AllPhases[p].size() > 0){ // is this phase empty?
                    for(unsigned int i = 0; i<monsters.size(); ++i){ // cycle initiative
                        if (i == (GAME.player->player_own)){ // was -1
                            for(unsigned int n = 0; n<AllPhases[p].size(); ++n){ // cycle this phase
                                if(AllPhases[p][n].mon_index == 666){ // is player in phase
                                    ++turnseq;
                                    int jumping = player_turn(GAME, monsters, AllPhases, turnseq, p);
                                    if(jumping == 666){ // exit everything
                                        quit_now = true;
                                        goto end;
                                    } else if(jumping == 10) goto jump;
                                }
                            }
                        } // if initiative has a monster
                        else for(unsigned int b = 0; b<monvector.size(); ++b){ // cycle all monsters
                            unsigned int monster_ini = monvector[b].initiative;
                            if ((i+1) == monster_ini){ // find the monster by matching initiative
                                for(unsigned int n = 0; n<AllPhases[p].size(); ++n){ // cycle phase
                                    if(AllPhases[p][n].mon_index == b){ // is monster in phase?
                                        ++turnseq;
                                        monster_turn(GAME, monsters, i, AllPhases, turnseq, p, n);
                                    } 
                                }
                            } 
                        }    
                    }    
                } else ++turnseq; 
            }    
            std::cout << "END COMBAT ROUND" << std::endl << std::endl;
            ++turn; // number of turns, starting at 1
            mon_list.clear(); // mouse lookup

            // end turn, so resets movement points
            if(player.AP > 0){ 
                player.AP = player.APm;
                player.APburn = 0;
            } else {
                //player.APburn = abs(player.AP);
                //player.AP += player.APm; // resets player's movement points  
                std::cout << "APburned: " << player.APburn << std::endl;
            } 
            player.AP = player.APm;

            for(unsigned int b = 0; b<monvector.size(); ++b){
                if(monvector[b].color == orc) monvector[b].combat_move = 6;
                else monvector[b].combat_move = 10;
            }

        } // while combat_move

        } else { // no combat
            player.AP = player.APm; // for attack when debug disables combat
        } // reset list if combat didn't happen


        end:
        if (quit_now) break;
       
        is_handle_combat = false;

        // maybe needed for death
        player.move(0, 0, monvector);

        for (unsigned int i = 0; i<monvector.size(); ++i) { 
            if (monvector[i].alive){
                monvector[i].c_mode = false;
            }
        } // deactivates combat mode for all monsters, so they are properly re-flagged on next loop 
        
        player_action = handle_keys(player, GAME);
        bool moved = false;

        if (player_action == quit) break; // break main while loop to exit program


        if (game_state == playing && player_action != no_turn){
            
            for (unsigned int i = 0; i<monvector.size(); ++i) { 
                
                if (monvector[i].alive){ // take turn for every monster alive

                    for (unsigned int u = 0; u<monvector.size(); ++u){
                        // check if alive or dead monsters block path
                        if(monvector[i].alive){
                            //1 = you can, see, walk
                            GAME.gstate.fov_map_mons_path1->setProperties(monvector[u].x, monvector[u].y, 1, 0);
                        }    
                    }

                    bool in_sight;
                    GAME.gstate.fov_recompute = true;

                    in_sight = GAME.gstate.fov_map->isInFov(monvector[i].x,monvector[i].y);
                    // take turn for monster in sight OR monster chasing and not bored
                    if (in_sight || (monvector[i].chasing && !monvector[i].boren)){

                        // compute mons-fov to awake other monsters in view
                        GAME.gstate.fov_map_mons->computeFov(monvector[i].x,monvector[i].y, MON_RADIUS, FOV_LIGHT_WALLS,FOV_ALGO);
                        //fov_map_mons_path->computeFov(monvector[i].x,monvector[i].y, MON_RADIUS, FOV_LIGHT_WALLS,FOV_ALGO);

                        // if at destination OR stuck -> pick a random destination
                        if ( (monvector[i].pl_x == monvector[i].x && monvector[i].pl_y == monvector[i].y) ||
                            monvector[i].stuck){


                            // randomly pick a destination that is valid on the map
                            int vagab_x = 0;
                            int vagab_y = 0;
                            bool retry = true;
                            while(retry){ 
                                vagab_x = rng(monvector[i].x - 30, monvector[i].x + 30);
                                if (vagab_x < 0) vagab_x = 0;
                                if (vagab_x > MAP_WIDTH) vagab_x = MAP_WIDTH;
                                vagab_y = rng(monvector[i].y - 30, monvector[i].y + 30);
                                if (vagab_y < 0) vagab_y = 0;
                                if (vagab_y > MAP_HEIGHT) vagab_y = MAP_HEIGHT;
                                if(!map_array[(vagab_y) * MAP_WIDTH + (vagab_x)].blocked) retry = false;
                            }

                            monvector[i].pl_x = vagab_x; // pick a random spot in FoV
                            monvector[i].pl_y = vagab_y;
                            monvector[i].path0->compute(monvector[i].x,monvector[i].y,vagab_x,vagab_y);
                            std::cout << "Monster path size: " << monvector[i].path0->size() << std::endl;
                        } 

                        --monvector[i].bored;

                        // aims for player if player is seen
                        if (in_sight){
                            monvector[i].pl_x = player.x; // if player in sight, store player pos
                            monvector[i].pl_y = player.y;
                            monvector[i].chasing = true;
                            monvector[i].boren = false;
                            monvector[i].bored = 400;
                            monvector[i].stuck = false;
                            monvector[i].path0->compute(monvector[i].x,monvector[i].y,monvector[i].pl_x,monvector[i].pl_y);
                            std::cout << "Monster path size: " << monvector[i].path0->size() << std::endl;
                        }

                        for (unsigned int u = 0; u<monvector.size(); ++u){
                            GAME.gstate.fov_map_mons_path1->setProperties(monvector[u].x, monvector[u].y, 1, 1);
                        }
                     
                        if (monvector[i].myai->take_turn(monvector[i], player, monvector[i].pl_x,
                                monvector[i].pl_y,in_sight, GAME)); //render_all();
                   
                        // awake monsters seen
                        for (unsigned int l = 0; l<monvector.size(); ++l) {
                            if(GAME.gstate.fov_map_mons->isInFov(monvector[l].x,monvector[l].y) && !monvector[l].chasing) {
                                monvector[l].pl_x = monvector[i].pl_x;
                                monvector[l].pl_y = monvector[i].pl_y;
                                monvector[l].chasing = true;
                            }
                        }
                   
                } // alive 

                    if (monvector[i].bored <= 0){
                        monvector[i].boren = true; // flag for boringness recuperation
                    }
                    if (monvector[i].boren == true){
                        monvector[i].bored += 2;
                        if (monvector[i].bored >= 400){
                            monvector[i].boren = false;
                            monvector[i].chasing = false;
                        }    
                    } // when bored reaches 100, monster is bored, will start to recuperate till it hits 100 again

                } // vector end cycle
            } // game state cycle



        //std::cout << "END MONSTER TURN" << std::endl;

        for (unsigned int i = 0; i<monvector.size(); ++i) {
                monvector[i].in_sight = false;
            } // resets monster flag at the end of loop

       
        if (player.stats.hp < 1 && !alreadydead ){
            player_death(GAME);
            alreadydead = 1;
        } else {
            player.move(m_x, m_y, monvector);
            if( ( m_x != 0) || (m_y != 0) ) moved = true;
            GAME.gstate.fov_recompute = true;
        } // this updates the player movement after the monster turn

        } // game state

        

        // recuperates turns off combat
        for (unsigned int i = 0; i<monvector.size(); ++i) {
            if (monvector[i].alive){
                if(monvector[i].combat_move < monvector[i].combat_move_max) ++monvector[i].combat_move;
            }    
        } 

        while(player.APburn > 0) {
            --player.AP; 
            --player.APburn;
            if(player.APburn == 0) switchweapon_ex(GAME);
        }

        if(player.AP < player.APm) ++player.AP; // one point for every move 
        if(moved) player.forcedswap = false;
        player.rangeaim = 0; // reset aim status
        player.aim = 0; // reset aim %
        player.ranged_target = -2; // reset target
        player.phaseAP = 0;

    } // main while cycle END
    return 0;
}

/*
timin1 = TCODSystem::getElapsedMilli();
timin2 = TCODSystem::getElapsedMilli();
std::cout << "UI block: " << timin2 - timin1 << std::endl;
*/

