#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "libtcod.hpp"
#include <windows.h> // for Sleep() and not currently used
#include <random>

#include "loader.h"
#include "tilevalues.h"
#include "map16.h"
#include "rng.h" // dice(number of rolls, sides)
#include "fileops.h" // mob_enc template struct

// #include <process.h> //used for threading?

bool combat_mode = false;
bool is_handle_combat = false;

int imageinit = 1;
float whattime = 0;
//TCODImage *pix;

int mapmode = 0; // changing map routine on regeneration

bool revealdungeon = 0;

const int MAP_WIDTH = 220;
const int MAP_HEIGHT = 140;
// 16map 55 35

const int MAP_WIDTH_AREA = 110;
const int MAP_HEIGHT_AREA = 70;
// visible area

const int   win_x   = (MAP_WIDTH_AREA) + 15 +3; // window width in cells 128 1024
const int   win_y   = (MAP_HEIGHT_AREA)+ 18 +3; // window height in cells 91 728
const int   LIMIT_FPS = 20;

const int quit = 1;
const int move_up = 5;
const int move_down = 10;
const int action = 15;
const int quit2 = 99; // combat mode exit game
const int playing = 1;
const int no_turn = 2;
const int dead = 99;

int stance_pos = 1;
int stance_pos2 = 0; // for in sight widget_1

int release_button = 1; // set to check for mouse button release

int insto_rows = 3; // rows in in sight UI


int bigg = 0; // sets 16x16 font
int bigg2 = 0; // is in minimap
int bigg3 = 0; // is in tinymap

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
TCODColor color_dark_ground(15, 15, 15);
TCODColor blood(255, 0, 0);
TCODColor blood1(200, 0, 0);
TCODColor blood2(160, 0, 0);
TCODColor blood3(120, 0, 0);
TCODColor blood4(100, 0, 0);
TCODColor door_c(222, 136, 0);

int off_xx = 0; // offsets to blit during attacks
int off_yy = 0;
int offbig_x = 0;
int offbig_y = 0;

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

TCODColor orc(0, 200, 0);
TCODColor troll(0, 255, 0);
TCODColor monsterdead(TCODColor::lightGrey);

bool fov_recompute;

bool debug = false; // shows all monsters on map, when set
bool no_combat = false; // disable combat mode

TCODConsole *con = new TCODConsole(440, 280);
TCODConsole *con_mini = new TCODConsole(MAP_WIDTH+2, MAP_HEIGHT+2);

TCODConsole *con_tini = new TCODConsole(MAP_WIDTH_AREA+2, MAP_HEIGHT_AREA+2); // for tinymap
TCODConsole *mesg = new TCODConsole(33, 3);  // message pop-up drawn on top of "con"
TCODConsole *load = new TCODConsole(win_x, win_y);  // load screen

TCODConsole *widget_top = new TCODConsole(win_x, 1);  // UI topbar

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
int wid_top_open = 1; // is top widget open?
int wid_combat_open = 0;
int wid_help = 0;
int wid_rpanel_open = 0;

TCODMap * fov_map = new TCODMap(MAP_WIDTH,MAP_HEIGHT);
TCODMap * fov_map_mons = new TCODMap(MAP_WIDTH,MAP_HEIGHT);

TCODMap * fov_map_mons_path0 = new TCODMap(MAP_WIDTH,MAP_HEIGHT);
TCODMap * fov_map_mons_path1 = new TCODMap(MAP_WIDTH,MAP_HEIGHT);

// bool used for background color presence
struct msg_log { char message [94]; TCODColor color1; TCODColor color2; TCODColor color3; TCODColor color4; TCODColor
    color5; TCODColor color6; TCODColor color7; TCODColor color8; TCODColor color9; TCODColor color10; bool c1;
    bool c2; bool c3; bool c4; bool c5; bool c6; bool c7; bool c8; bool c9; bool c10;
    TCODColor bcolor1; TCODColor bcolor2; TCODColor bcolor3; TCODColor bcolor4; TCODColor bcolor5; TCODColor bcolor6;
    TCODColor bcolor7; TCODColor bcolor8; TCODColor bcolor9; TCODColor bcolor10;};

struct msg_log_c { bool is_context; int ltype;}; 

std::vector<msg_log> msg_log_list;
std::vector<msg_log_c> msg_log_context;


void map_8x16_font(){

    for (int n = 0; n <= 100; n++){
        int step = 0;
        step = 2*n;
        TCODConsole::mapAsciiCodeToFont(1001+step,1,0+step);
        TCODConsole::mapAsciiCodeToFont(1001+(step+1),1,0+(step+1));
    }
}

void print_8x16(int where_x, int where_y, const char *msg, TCODColor front, TCODColor back){

    con->setDefaultForeground(front);
    con->setDefaultBackground(back);
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
        con->putChar(where_x + n, where_y, 1001+step, TCOD_BKGND_SET);
        con->putChar(where_x + n, where_y+1, 1001+(step+1), TCOD_BKGND_SET);
        //con->putChar((where_x + (m + 1))-2, where_y+1, 936+(step+1), TCOD_BKGND_SET);

    }

    //con->putChar(3, 3, 1001+step, TCOD_BKGND_SET);
    //con->putChar(3, 4, 1001+(step+1), TCOD_BKGND_SET);

    //std::cout << "char 1: " << first << std::endl;
}

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

class AI;

class Object_monster;


class Object_player;

class Weapon_use {
public:
    char wpn_type[15];

    int wpn_AC;
    int wpn_DC;

    int wpn_B; // blunt
    int wpn_E;
    int wpn_P;

    int wpn_aspect;

    Weapon_use(){    
    }

};    

// used both on monsters & players
class Fighter {
public:
    int speed; // used for initiative SPD
    int max_hp;
    int hp;
    int defense;
    int power;

    int ML;

    Weapon_use wpn1;

    Fighter(int inithp, int defval, int powval, int speedval){
        max_hp = inithp;
        hp = inithp;
        defense = defval;
        power = powval;
        speed = speedval;
    }

    void attack(Object_player &player, Object_monster &monster, bool who); 

    void take_damage(int damage){
        if (damage > 0) hp -= damage;
    }
};



class Object_monster {

public: // public should be moved down, but I keep it here for debug messages

    char name[20];
    int x;
    int y;
    char selfchar;
    int self_8; // int of the corresponding tile 
    int self_16; // int of the first tile of 4
    TCODColor color;
    TCODColor colorb;
    int h; // temp hit points
    int bloody;
    bool blocks;
    Fighter stats;
    AI * myai;
    bool alive;
    bool chasing; // used to move monster after player for a short while
    int active; // used for wandering mode

    int pl_x; // these are for monster current target/destination
    int pl_y;
    TCODPath *path0;
    TCODPath *path1;
    int path_mode; // 0 = standard 1 = path with monster positions

    bool stuck;
    int bored;
    bool boren; // 100 bored, if to 0, boren true, start recuperating
    bool in_sight;

    bool hit;

    int combat_move;
    int combat_move_max;
    int move_counter; // how many steps moved in the turn
    bool c_mode; // flag monsters for active combat mode
    int speed;
    int initiative;
    int temp_init; // total initiative value for messages/list
    unsigned int cflag_attacks; // combat-flag, attacks during turn received

    Object_monster(int a, int b, char pchar, TCODColor oc, TCODColor oc2, int health, Fighter loc_fighter) : stats(loc_fighter) {
        x = a;
        y = b;
        selfchar = pchar;
        color = oc;
        colorb = oc2;
        h = health;
        bloody = 0;
         
    }

    void move(int dx, int dy, bool p_dir); 
       
    
    void draw(bool uh) {
        //con->setDefaultForeground(color);
        
        // executes only outside combat
        if (!uh){ // if 0
            if(bigg){
                colorb = con->getCharBackground((x*2), (y*2));
            } else {  
                if (selfchar == '%') colorb = con->getCharBackground(x, y);
                else colorb = TCODColor::black;
                //colorb = con->getCharBackground(x, y);
                
            }
        }

        con->setDefaultBackground(colorb);

        if (fov_map->isInFov(x,y) || debug == 1){ 
            if(bigg){ // if 16x16  
               
                // sets color only if sprite isn't colored (lame check)
                con->setDefaultForeground(color);
                
                    con->putChar((x*2), (y*2), self_16, TCOD_BKGND_SET);
                    con->putChar((x*2)+1, (y*2), self_16+100, TCOD_BKGND_SET);
                    con->putChar((x*2), (y*2)+1, self_16+200, TCOD_BKGND_SET);
                    con->putChar((x*2)+1, (y*2)+1, self_16+300, TCOD_BKGND_SET);
             
            } else {
                con->setDefaultForeground(color);
                if(hit) colorb = TCODColor::red;
                else colorb = con->getCharBackground(x, y);

                con->setCharBackground(x, y, colorb, TCOD_BKGND_SET);

                if (U8 == true){
                    con->putChar(x, y, self_8, TCOD_BKGND_SET);
                    //con->setCharBackground(x, y, con->getCharBackground(x, y), TCOD_BKGND_SET);
                    //con->putChar(x, y, u8_orc, TCOD_BKGND_SET);
                } else {
                    con->putChar(x, y, selfchar, TCOD_BKGND_SET);
                }    
            }
        }    
        con->setDefaultBackground(TCODColor::black); // reset background for smaller map
    }

    void move_towards(int target_x, int target_y){

        bool pdir = false;
        int dx = target_x - x;
        int dy = target_y - y;
        float distance = sqrt (pow(dx, 2) + pow(dy, 2));
        
        dx = (int)(round(dx / distance));
        dy = (int)(round(dy / distance));

        if (dx == 0 && target_x > x) pdir = true;
        if (dy == 0 && target_y > y) pdir = true;

        if ( (dx < 2 && dx > -2) && (dy < 2 && dy > -2) )
            move(dx, dy, pdir);
        //std::cout << "move: " << dx << dy << std::endl;
    }

    float distance_to(int other_x, int other_y){
        int dx = other_x - x;
        int dy = other_y - y;
        return sqrt (pow(dx, 2) + pow(dy, 2));
    }

    void clear() {
        if (fov_map->isInFov(x,y)){
            if(bigg){
                con->putChar((x*2), (y*2), 503, TCOD_BKGND_NONE);
                con->putChar((x*2)+1, (y*2), 603, TCOD_BKGND_NONE);
                con->putChar((x*2), (y*2)+1, 703, TCOD_BKGND_NONE);
                con->putChar((x*2)+1, (y*2)+1, 803, TCOD_BKGND_NONE);
            } else {
                con->putChar(x, y, ' ', TCOD_BKGND_NONE);
            }
        }
    }

    ~Object_monster(){} // ?
};

class AI {
public:
     virtual bool take_turn(Object_monster &monster, Object_player &player, int p_x, int p_y, bool myfov) {return false;} 

};

 

class Statistics {

public:

    int M;
    int P;
    int S;

    int MM;
    int MR;
    int PM;
    int PN;
    int SM;
    int SP;

    int MMC;
    int MMP;
    int MMS;

    int MRC;
    int MRP;
    int MRS;

    int PMC;
    int PMP;
    int PMS;

    int PNC;
    int PNP;
    int PNS;

    int SMC;
    int SMP;
    int SMS;

    int SPC;
    int SPP;
    int SPS;

    Statistics(int a){ // player characteristics
        M = 0;
        P = 0;
        S = 0;

        MM = 0;
        MR = 0;
        PM = 0;
        PN = 0;
        SM = 0;
        SP = 0;

        MMC = 0;
        MMP = 0;
        MMS = 0;

        MRC = 0;
        MRP = 0;
        MRS = 0;

        PMC = 0;
        PMP = 0;
        PMS = 0;

        PNC = 0;
        PNP = 0;
        PNS = 0;

        SMC = 0;
        SMP = 0;
        SMS = 0;

        SPC = 0;
        SPP = 0;
        SPS = 0;
    }

};

class Object_player {

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
    Fighter stats;
    Statistics sts;

    int combat_move;
    int combat_move_max;
    int move_counter; // how many steps moved in the turn
    int speed;
    int initiative;
    int temp_init; // total initiative value for messages/list
    unsigned int cflag_attacks; // combat-flag - number of attacks received in current turn

    Object_player(int a, int b, char pchar, TCODColor oc, TCODColor oc2, int health, Fighter loc_fighter, 
            Statistics loc_sts) : stats(loc_fighter), sts(loc_sts) {
            
        x = a;
        y = b;
        selfchar = pchar;
        color = oc;
        colorb = oc2;
        h = health;
        bloody = 0;
        combat_move_max = 8;
    }

    void move(int dx, int dy, std::vector<Object_monster> smonvector); 
      /* 
        defined below
      */ 

    bool attack(Object_monster &foe){
        /*foe.stats.hp = foe.stats.hp - 1;
        if (foe.stats.hp == 0) {std::cout << "Killed a foe." << std::endl; return 1;}// killed!
        return 0; // not killed
        */
        return false;
    }

    void draw(bool uh) {
        con->setDefaultForeground(color);
        // uh is for attack animation. if uh is 0 then the background is taken from cell
        // if uh is 1 then look at colorb 
        if (!uh){  
            if(bigg){
                colorb = con->getCharBackground((x*2), (y*2));
            } else {    
                //colorb = con->getCharBackground(x, y);
                colorb = TCODColor::black; // for sprite
            }  
        }    
        con->setDefaultBackground(colorb);
        if (fov_map->isInFov(x,y)){
            if(bigg){
                con->putChar((x*2), (y*2), u16_player, TCOD_BKGND_SET);
                con->putChar((x*2)+1, (y*2), u16_player+100, TCOD_BKGND_SET);
                con->putChar((x*2), (y*2)+1, u16_player+200, TCOD_BKGND_SET);
                con->putChar((x*2)+1, (y*2)+1, u16_player+300, TCOD_BKGND_SET);
                //con->putChar((x*2), (y*2), 502, TCOD_BKGND_SET);
                //con->putChar((x*2)+1, (y*2), 602, TCOD_BKGND_SET);
                //con->putChar((x*2), (y*2)+1, 702, TCOD_BKGND_SET);
                //con->putChar((x*2)+1, (y*2)+1, 802, TCOD_BKGND_SET);
            } else {
                //con->putChar(x, y, 445, TCOD_BKGND_SET);
                con->putChar(x, y, u8_player, TCOD_BKGND_SET);
            }
        }
        con->setDefaultBackground(TCODColor::black); // reset background for smaller map
      
    }

    void clear() {
        if (fov_map->isInFov(x,y)){
        if(bigg){
        con->putChar((x*2), (y*2), 503, TCOD_BKGND_NONE);
        con->putChar((x*2)+1, (y*2), 603, TCOD_BKGND_NONE);
        con->putChar((x*2), (y*2)+1, 703, TCOD_BKGND_NONE);
        con->putChar((x*2)+1, (y*2)+1, 803, TCOD_BKGND_NONE);
        } else {

        con->putChar(x, y, ' ', TCOD_BKGND_NONE);
        }
        }
    }

    ~Object_player(){} // ?
};




std::vector<Object_player*> myvector; // player vector object

class BasicMonster : public AI {
public:

    BasicMonster(){

    }

    virtual bool take_turn(Object_monster &monster, Object_player &player, int p_x, int p_y, bool myfov){
            //float dist = 0;
            //std::cout << monster.combat_move << std::endl; 
        
            std::cout << "The " << monster.name << " is active! " << std::endl;
            //dist = monster.distance_to(p_x, p_y);

            // 1.1 so the monster attacks, but still moves closer instead of stopping diagonally
            if ( (monster.distance_to(p_x, p_y) >= 1.1) || (monster.chasing && !myfov)){

                if (no_combat || monster.combat_move >= 1){ // move up to and including player pos    
                    monster.move_towards(p_x, p_y);
                    if(!no_combat){
                        monster.combat_move -= 1;
                    }    
                    std::cout << "The " << monster.name << " is moving." << std::endl;
                    return false;
                }

            } else if (myfov){ 
                if (no_combat || (monster.combat_move >= 4)) {
                //TCODConsole::root->clear();
                mesg->setAlignment(TCOD_LEFT);
                mesg->setDefaultForeground(TCODColor::yellow);
                mesg->setDefaultBackground(TCODColor::black);
                player.colorb = TCODColor::red; // otherwise colorb is set in draw(), by looking at floor
                monster.colorb = TCODColor::black;
                player.selfchar = '/';
                player.draw(1);
                monster.draw(1);
                mesg->print(1, 1, "Hit!");
                //if(!(player.y > MAP_HEIGHT-8 )) TCODConsole::blit(mesg,0,0,33,3,con,1,MAP_HEIGHT-4);
                //else TCODConsole::blit(mesg,0,0,33,3,con,MAP_WIDTH-37,1);

                if(bigg){
                    TCODConsole::blit(con,offbig_x,offbig_y+2,110,68,TCODConsole::root,0,2);
                } else {    
                    TCODConsole::blit(con,off_xx,off_yy+1,110,69,TCODConsole::root,0,1);
                }
                //std::cout << "SMALLOFF: " << off_xx << " " << off_yy << std::endl;

                TCODConsole::flush();

                Sleep(250); // shitty way for attack "animation", uses windows.h
                player.colorb = color_dark_ground;
                monster.colorb = color_dark_ground;
                player.selfchar = '@';
                player.draw(0);
                monster.draw(0);
                con->clear();
                fov_recompute = true;
                //render_all();
                
                monster.stats.attack(player, monster, 1); // calls attack function for monsters
                TCODConsole::flush();
                if(!no_combat)monster.combat_move -= 4; // decrease the movement points for attack
                return true;
            } else if (myfov && !no_combat) monster.combat_move = 0; // movement points to 0 if couldn't make the attack
            }
        
        //else std::cout << "The " << monster.name << " lurks in the dark! " ;
        return false; 
    } 

};


void Fighter::attack(Object_player &player, Object_monster &monster, bool who){


    int damage = 0;

    if (who){       

        // calculate AML
        int a_AML = monster.stats.ML; // basic skill
        a_AML += monster.stats.wpn1.wpn_AC; // adding weapon Attack Class
        // should check for walls here
        int d_DML = player.stats.ML; // basic monster skill
        d_DML += player.stats.wpn1.wpn_AC; // adding weapon Attack Class

        // for every attack the player defends from or does, a -10 penality is applied
        if(player.cflag_attacks >= 1){
            d_DML += (player.cflag_attacks * 10) * -1;
            std::cout << "playerD: BASIC " << player.stats.ML << " WEAPON " << player.stats.wpn1.wpn_AC
                << " TOTAL " << d_DML << std:: endl;
        }  
        player.cflag_attacks++; // increment counter (reset at beginning of combat turn)

        // for every attack the monster defends from or does, a -10 penality is applied
        if(monster.cflag_attacks >= 1){
            a_AML += (monster.cflag_attacks * 10) * -1;
            std::cout << "monsterD: BASIC " << monster.stats.ML << " WEAPON " << monster.stats.wpn1.wpn_AC
                << " TOTAL " << a_AML << std:: endl;
        }  
        monster.cflag_attacks++; // increment counter (reset at beginning of combat turn)

        // roll two 1d100, one for player, one for monster
        TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why
        short int a_d100 = wtf->getInt(1, 100, 0);
        short int d_d100 = wtf->getInt(1, 100, 0);

        short int a_success_level = 0;
        short int crit_val = a_d100 % 10;
        if (a_d100 <= a_AML){
            if ( crit_val == 0 || crit_val == 5 ){
                a_success_level = 0; // CS Critical Success
            } else {    
                a_success_level = 1; // MS Marginal Success
            }    
        } else if (a_d100 > a_AML){
            if ( crit_val == 0 || crit_val == 5){
                a_success_level = 3; // CF Critical Failure
            } else {
                a_success_level = 2; // MF Marginal Failure
            }
        }

        // this is player
        short int d_success_level = 0;
        crit_val = d_d100 % 10;
        if (d_d100 <= d_DML){
            if ( crit_val == 0 || crit_val == 5 ){
                d_success_level = 0; // CS Critical Success
            } else {    
                d_success_level = 1; // MS Marginal Success
            }    
        } else if (d_d100 > d_DML){
            if ( crit_val == 0 || crit_val == 5){
                d_success_level = 3; // CF Critical Failure
            } else {
                d_success_level = 2; // MF Marginal Failure
            }
        }

        const int melee_res[4][4] = 
        {
            { 4, 5, 6, 7, },
            { 4, 4, 5, 6, },
            { 2, 4, 4, 3, },
            { 2, 2, 2, 1, }
        };

        std::cout << "aML: " << monster.stats.ML << " a_AML: " << a_AML << " a_d100: " << a_d100 << std::endl;
        std::cout << "dML: " << player.stats.ML << " d_DML: " << d_DML << " d_d100: " << d_d100 << std::endl;
        std::cout << "A Success Level: " << a_success_level << std::endl;
        std::cout << "D Success Level: " << d_success_level << std::endl;
        std::cout << "Melee Result: " << melee_res[a_success_level][d_success_level] << std::endl;

        msg_log msgd;
        sprintf(msgd.message, "%c*%cMonster's skill(%c%d%c) %c1d100%c(%c%d%c) VS Player's defense(%c%d%c) %c1d100%c(%c%d%c)",
                TCOD_COLCTRL_5, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, a_AML, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_3, a_d100, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, d_DML, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_4, d_d100, TCOD_COLCTRL_STOP);
        msgd.color1 = TCODColor::cyan;
        msgd.color2 = TCODColor::yellow;
        msgd.color5 = TCODColor::red;
        switch(a_success_level){
            case 0:
                msgd.c3 = 1;
                msgd.color3 = TCODColor::white;
                msgd.bcolor3 = TCODColor::blue;
                break;
            case 1:
                msgd.color3 = TCODColor::lighterBlue;
                break;
            case 2:
                msgd.color3 = TCODColor::red;
                break;
            case 3:
                msgd.c3 = 1;
                msgd.color3 = TCODColor::white;
                msgd.bcolor3 = TCODColor::red;
                break;
        }        
        switch(d_success_level){
            case 0:
                msgd.c4 = 1;
                msgd.color4 = TCODColor::white;
                msgd.bcolor4 = TCODColor::blue;
                break;
            case 1:
                msgd.color4 = TCODColor::lighterBlue;
                break;
            case 2:
                msgd.color4 = TCODColor::red;
                break;
            case 3:
                msgd.c4 = 1;
                msgd.color4 = TCODColor::white;
                msgd.bcolor4 = TCODColor::red;
                break;
        }
        msg_log_list.push_back(msgd);        

        msg_log msg1;
        switch(melee_res[a_success_level][d_success_level]){
            case 1:
                monster.stats.hp -= 1;
                player.stats.hp -= 1;
                std::cout << "[CF] Both you and your enemy fumble! -1 to HP for both." << std::endl;
                sprintf(msg1.message, "%c*%cBoth your enemy and you fumble! %c-1%c to HP for both.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 2:
                monster.stats.hp -= 1;
                std::cout << "[CF] You fumble the attack! -1 to HP." <<std::endl;
                sprintf(msg1.message, "%c*%cThe monster fumbles the attack! %c-1%c to HP.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 3:
                player.stats.hp -= 1;
                std::cout << "[MF] You fail the attack, but the monster fumbles and hits himself! -1 to monster HP." 
                    << std::endl;
                sprintf(msg1.message, "%c*%cThe monster misses, but you fumble the parry and hurt yourself! %c-1%c to HP.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 4:
                std::cout << "You try to attack, but the monster defends itself!" << std::endl;
                sprintf(msg1.message, "%c*%cThe monster swings his weapon, but you parry.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 5:
                player.stats.hp -= monster.stats.power;
                std::cout << "Your attack does standard damage." << std::endl;
                sprintf(msg1.message, "%c!%cThe monster hits you for %c%d%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, monster.stats.power, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 6:
                player.stats.hp -= monster.stats.power * 2;
                std::cout << "You attack does double damage!" << std::endl;
                sprintf(msg1.message, "%c!%cCritial attack! %c%d%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, (monster.stats.power * 2), TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 7:
                player.stats.hp -= monster.stats.power * 3;
                std::cout << "Your attack does triple damage!" << std::endl;
                sprintf(msg1.message, "%c!%cTriple damage! %c%d%c HP.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, (monster.stats.power * 3), TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
        }
        msg_log_list.push_back(msg1);

        /* old combat 
        damage = monster.stats.power - player.stats.defense;

        if (damage > 0){
            std::cout << monster.name << " attacks " << player.name << " for " << damage << " hit points." << std::endl;
            player.stats.take_damage(damage);

            msg_log msg1;
                sprintf(msg1.message, "The %c%s%c hits you for %c%d%c damage.",
                        TCOD_COLCTRL_1, monster.name, TCOD_COLCTRL_STOP,
                        TCOD_COLCTRL_2, damage, TCOD_COLCTRL_STOP); 
                msg1.color1 = monster.color;
                msg1.color2 = TCODColor::red;
                msg_log_list.push_back(msg1);

        } else std::cout << monster.name << " attacks " << player.name << " but it has no effect!" << std::endl;
        */
        
    } else {
        damage = player.stats.power - monster.stats.defense;

        if (damage > 0){
            std::cout << player.name << " attacks " << monster.name << " for " << damage << " hit points." << std::endl;
            monster.stats.take_damage(damage);
        } else std::cout << player.name << " attacks " << monster.name << " but it has no effect!" << std::endl;       
    }

}


Fighter fighter_component(30, 2, 5, 8); // hp, defense, power
Statistics stati(0);
//Object_player playera(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5, fighter_component);
//Object_player playerb(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5, fighter_component);

Object_player player(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5, fighter_component, stati);
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
      

        //std::cout << "BEFORE" << std::endl;
        //
        TCODPath *path;
        path = path0;
        
        stuck = false;

        if(!path->isEmpty()){
            std::cout << "Path is: " << path->size() << std::endl;
        } else {
            stuck = true; 
            return;
        }

        //std::cout << "MID" << std::endl;


        int newx,newy;
        path->get(0,&newx,&newy);
        if (!is_blocked(newx,newy)){
            if (path->walk(&newx,&newy,true)) {
                if( (newx != x) || (newy != y) ){ 
                    move_counter++; // increment move counter, even out of combat tho
                    if(move_counter == (combat_move_max/2)){
                        std::cout << "PMONSTER " << combat_move_max/2 << std::endl;
                        cflag_attacks++;
                        move_counter = 0;
                    }    
                }    
                x = newx;
                y = newy;
            }    
        } else stuck = true;  

        //std::cout << "AFTER" << std::endl;
        


        /*
        if (path->walk(&newx,&newy,true)) {
            if (!is_blocked(newx,newy)){
                x = newx;
                y = newy;
            } else stuck = true;
        }    
        */

    
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
        if (map_array[y * MAP_WIDTH + x].bloodyt > 1) --map_array[y * MAP_WIDTH + x].bloodyt;
}

BasicMonster orc_ai;


void place_objects(Rect room, lvl1 myenc){

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
                        if (overrider > 50) break; // maybe the room too small, so stop trying
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
                    monster.bored = 500;
                    monster.boren = false;
                    monster.in_sight = false;
                    monster.path_mode = 0;
                    monster.combat_move = myenc.vmob_types[myenc.cave1[u].enc[i]].combat_move;
                    monster.combat_move_max = monster.combat_move;
                    monster.c_mode = false;
                    monster.speed = myenc.vmob_types[myenc.cave1[u].enc[i]].speed;
                    monster.hit = false;

                    monster.stats.wpn1.wpn_AC =     myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_AC;
                    monster.stats.wpn1.wpn_DC =     myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_DC;
                    monster.stats.wpn1.wpn_B =      myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_B;
                    monster.stats.wpn1.wpn_aspect = myenc.vmob_types[myenc.cave1[u].enc[i]].wpn_aspect;

                    monster.stats.ML = myenc.vmob_types[myenc.cave1[u].enc[i]].ML;

                    monster.path0 = new TCODPath(fov_map_mons_path0, 0.0f);
                    monster.path1 = new TCODPath(fov_map_mons_path1, 0.0f);
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

    TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

    for (int i = inroom.y1-1; i <= inroom.y2+1; ++i){ // +1 so it looks to walls too
        for (int l = inroom.x1-1; l <= inroom.x2+1; ++l){

            Door door1(l, i, 0);
            
            if(wtf->getInt(1, 10, 0) > 5){

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
            minx = TCODRandom::getInstance()->getInt(minx,maxx-ROOM_MIN_SIZE+1);
			miny = TCODRandom::getInstance()->getInt(miny,maxy-ROOM_MIN_SIZE+1);
            if( (maxx - minx) > ROOM_MAX_SIZE ) maxx = minx + ROOM_MAX_SIZE;
            if( (maxy - miny) > ROOM_MAX_SIZE ) maxy = miny + ROOM_MAX_SIZE;
			maxx = TCODRandom::getInstance()->getInt(minx+ROOM_MIN_SIZE-1,maxx);
			maxy = TCODRandom::getInstance()->getInt(miny+ROOM_MIN_SIZE-1,maxy);
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
            round = TCODRandom::getInstance()->getInt( 0, 5, 0);
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
					int x1=TCODRandom::getInstance()->getInt(left->x,left->x+left->w-1);
					int x2=TCODRandom::getInstance()->getInt(right->x,right->x+right->w-1);
					int y=TCODRandom::getInstance()->getInt(left->y+left->h,right->y);
					vline_up(x1,y-1);
					hline(x1,y,x2);
					vline_down(x2,y+1);
				} else {
					// straight vertical corridor
					int minx=MAX(left->x,right->x);
					int maxx=MIN(left->x+left->w-1,right->x+right->w-1);
					int x=TCODRandom::getInstance()->getInt(minx,maxx);
					vline_down(x,right->y);
					vline_up(x,right->y-1);
				}
			} else {
				// horizontal corridor
				if ( left->y+left->h -1 < right->y || right->y+right->h-1 < left->y ) {
					// no overlapping zone. we need a Z shaped corridor
					int y1=TCODRandom::getInstance()->getInt(left->y,left->y+left->h-1);
					int y2=TCODRandom::getInstance()->getInt(right->y,right->y+right->h-1);
					int x=TCODRandom::getInstance()->getInt(left->x+left->w,right->x);
					hline_left(x-1,y1);
					vline(x,y1,y2);
					hline_right(x+1,y2);
				} else {
					// straight horizontal corridor
					int miny=MAX(left->y,right->y);
					int maxy=MIN(left->y+left->h-1,right->y+right->h-1);
					int y=TCODRandom::getInstance()->getInt(miny,maxy);
					hline_left(right->x-1,y);
					hline_right(right->x,y);
				}
			}
		}    
        return true;
    }
};


void make_map_BSP(Object_player &duh){

    map_array.resize(MAP_HEIGHT * MAP_WIDTH);
     
    // fill map with walls [1,1]
    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
           map_array[i * MAP_WIDTH + l] = Tile(1,1);
        }
    } 

    TCODBsp *myBSP = new TCODBsp(30,30, MAP_WIDTH-60, MAP_HEIGHT-60);
    myBSP->splitRecursive(NULL,6,ROOM_MIN_SIZE+1,ROOM_MIN_SIZE+1,1.5f,1.5f);
    //myBSP->splitRecursive(NULL,17,3,2,1.5f,1.5f);
    
    myBSP->traverseInvertedLevelOrder(new MyCallback(),NULL);

    // load monster values?
    lvl1 myenc; 
    load_stuff(myenc);
    std::cout << "encounters: " << myenc.cave1.size() << std::endl;
    std::cout << "monster types: " << myenc.vmob_types.size() << std::endl;
    std::cout << "monster1 ML: " << myenc.vmob_types[0].ML << std::endl;
    std::cout << "monster1 name: " << myenc.vmob_types[0].name << std::endl;
    std::cout << "monster2 name: " << myenc.vmob_types[1].name << std::endl;
  
    for (unsigned int i = 0; i<BSProoms.size(); ++i){
        if(BSProoms[i].reround) create_round_room(BSProoms[i]);
        place_doors(BSProoms[i]);
        place_column(BSProoms[i]);
        if(i > 5) place_objects(BSProoms[i], myenc); // don't place monsters on player start (within 5 rooms)
    }
    delete myBSP;

    killall = monvector.size(); // how many monsters on map
    while(map_array[player.y * MAP_WIDTH + player.x].blocked){
        player.x++;
        player.y++;
    }    
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
                place_objects(new_room, myenc); // only add monsters if not first room
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

void set_black(){
    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            con->putChar(l, i, ' ', TCOD_BKGND_SET);
                        con->setCharBackground(l, i, TCODColor::black, TCOD_BKGND_SET);
                        con->setCharForeground(l, i, TCODColor::black);
        }
    } 
} // fill "con" with black

void set_black_b(){
    for (int i = 0; i < (MAP_HEIGHT*2) ;++i){
        for (int l = 0; l < (MAP_WIDTH*2) ;++l) {
            con->putChar(l, i, ' ', TCOD_BKGND_SET);
                        con->setCharBackground(l, i, TCODColor::black, TCOD_BKGND_SET);
                        con->setCharForeground(l, i, TCODColor::black);
        }
    } 
} // fill "con" with black

TCOD_key_t keyz;
    TCOD_mouse_t mousez;
    TCOD_event_t ev;

// never used    
void threadm( void* pParams )
    {

        std::cout << "MOUSE ";
        int l = 0;
        for (;;) {
            int n = l;
            int i = TCODSystem::getElapsedMilli();
            l = i - n;
            if ( l > 50){
                //ev = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&keyz,&mousez);
            } else {
                TCODSystem::sleepMilli(1);
            }
        }
       // int     rinon;
       // for (;;) {
       //     if (al_poll_duh(dp) || closed)
		//	break;

        //    rinon = rand()%(101);
         //   if (rinon < 80)
         //       {
          //      YIELD();
          //      }
      //  }
    }

// (was only) in combat
void I_am_moused(){
    //TCOD_key_t key;
    //TCOD_mouse_t mouse;
    //TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
    mousez = TCODMouse::getStatus();
    int x = mousez.cx;
    int y = mousez.cy;

    char *whatis;

    if(!release_button){
        if(!mousez.lbutton){
            release_button = 1;
        }

    } else {    

        // extended msg log
        if(mousez.lbutton && mousez.cy == 73 && (mousez.cx == 33 || mousez.cx == 34)) {
            if(MSG_MODE_XTD){ 
                MSG_MODE_XTD=0; 
            } else {
                MSG_MODE_XTD = 1; 
            }
            release_button = 0;
        }    

        if(mousez.lbutton && mousez.cy == 0 && (mousez.cx == 0 || mousez.cx == 1 || mousez.cx == 2)) {
            if(wid_top_open){ 
                wid_top_open=0; 
            } else {
                wid_top_open = 1; 
            }
            release_button = 0;
        }

        if(mousez.lbutton && mousez.cy > (win_y-2) && mousez.cx > (win_x-4) ) {
            if(wid_combat_open){ 
                wid_combat_open=0; 
            } else {
            wid_combat_open = 1; 
            }
        release_button = 0;
        }

        if( (mousez.lbutton && mousez.cy == 3 && mousez.cx == 127) ||
                (mousez.lbutton && mousez.cy == 3 && mousez.cx == 126) ||
                (mousez.lbutton && mousez.cy == 3 && mousez.cx == 125) ) {
            if(wid_rpanel_open){ 
                wid_rpanel_open=0; 
            } else {
            wid_rpanel_open = 1; 
            }
        release_button = 0;
        } 
    

    if(wid_top_open){
        if(mousez.lbutton && mousez.cy == 0 && (mousez.cx >= 92 && mousez.cx <= 94)) {
            wid_help = 1;
        }
        if(mousez.lbutton && mousez.cx == 4 && mousez.cy == 0) {stance_pos = 1; bigg = 0; fov_recompute = true;}
        if(mousez.lbutton && mousez.cx == 5 && mousez.cy == 0) {stance_pos = 2; bigg = 1; fov_recompute = true;}
        //if(mousez.lbutton && mousez.cx == 2 && mousez.cy == 0) stance_pos = 3;
        if(mousez.cx == 6 && mousez.cy == 0) {
            bigg2 = 1;
            fov_recompute = true;
        } else {
            bigg2 = 0;
        }
        if(mousez.cx == 7 && mousez.cy == 0) {
            bigg3 = 1;
            fov_recompute = true;
        } else {
            bigg3 = 0;
        }
        if( (mousez.cx >= 10 && mousez.cx < 18) && mousez.cy == 0) {
            stance_pos2 = 1;
        } else {
            stance_pos2 = 0;
        }
    }
    if(wid_help){
        if(mousez.lbutton && mousez.cy == 10 && (mousez.cx >= 92 && mousez.cx <= 95)) {
            wid_help = 0;
        }    
    }    
    } // release button 

    //std::cout << "MOUSE " << x << " " << y << std::endl;

    bool found = false;
    TCODColor col_obj; // color of object

    if(!bigg){

    if ((!bigg && x == player.x && y == player.y) || (bigg && (x/2) == player.x && (y/2) == player.y)){
        panel->setDefaultForeground(TCODColor::white);
        panel->setAlignment(TCOD_LEFT);
        panel->print(1, 5, "Mouse on [Player] at [%d.%d]", x, y);
        panel->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
    } else {
        for (unsigned int n = 0; n<monvector.size(); ++n) {
            if( (!bigg && (x == monvector[n].x && y == monvector[n].y && monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y)) ||
                (bigg && ((x/2) == monvector[n].x && (y/2) == monvector[n].y && monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y)) ){
                whatis = &(monvector[n].name[0]);
                panel->setDefaultForeground(TCODColor::white);
                panel->setAlignment(TCOD_LEFT);
                col_obj = monvector[n].color;
                TCODConsole::setColorControl(TCOD_COLCTRL_1,col_obj,TCODColor::black);
                panel->print(1, 5, "Mouse on [%c%s%c] at [%d.%d]",TCOD_COLCTRL_1, whatis, TCOD_COLCTRL_STOP, x, y);
                panel->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
                found = true;
                  }
        }

        if (!found){ // only if no moster alive found        
        for (unsigned int n = 0; n<monvector.size(); ++n) {
            if( (!bigg && (x == monvector[n].x && y == monvector[n].y) && !(monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y)) ||
                (bigg && ((x/2) == monvector[n].x && (y/2) == monvector[n].y) && !(monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y))    ){
                whatis = &(monvector[n].name[0]);
                panel->setDefaultForeground(TCODColor::white);
                panel->setAlignment(TCOD_LEFT);
                col_obj = monvector[n].color;
                TCODConsole::setColorControl(TCOD_COLCTRL_1,col_obj,TCODColor::black);
                panel->print(1, 5, "Mouse on [dead %c%s%c] at [%d.%d]",TCOD_COLCTRL_1, whatis, TCOD_COLCTRL_STOP, x, y); 
                panel->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
                found = true;
            }
        }
        }
          
        if (!found){
            panel->setDefaultForeground(TCODColor::white);
            panel->setAlignment(TCOD_LEFT);
            panel->print(1, 5, "Mouse on [Nothing] at [%d.%d]", x, y);
            panel->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
            found = false;
        }
    }

    }

    int asciiwhat = 0;
    if(bigg){
        panel->setDefaultForeground(TCODColor::white);
        panel->setAlignment(TCOD_LEFT);
        asciiwhat= TCODConsole::root->getChar(x, y);
        if (asciiwhat == 502 || asciiwhat == 602 || asciiwhat == 702 || asciiwhat == 802){
            panel->print(1, 5, "Mouse on [Player] at [%d.%d]", x, y);
        } else if(asciiwhat == 501 || asciiwhat == 601 || asciiwhat == 701 || asciiwhat == 801){
            panel->print(1, 5, "Mouse on [Floor] at [%d.%d]", x, y);
        } else if(asciiwhat == 504 || asciiwhat == 604 || asciiwhat == 704 || asciiwhat == 804){
            panel->print(1, 5, "Mouse on [Troll] at [%d.%d]", x, y);
        } else if(asciiwhat == 505 || asciiwhat == 605 || asciiwhat == 705 || asciiwhat == 805){
            panel->print(1, 5, "Mouse on [Orc] at [%d.%d]", x, y);
        } else if(asciiwhat == 506 || asciiwhat == 606 || asciiwhat == 706 || asciiwhat == 806){
            panel->print(1, 5, "Mouse on [Remains] at [%d.%d]", x, y);
        }
        panel->setDefaultBackground(TCODColor::black);
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
        int value, int maximum, TCODColor bar_color, TCODColor back_color, TCODColor label){

    int bar_width = int(float(value) / maximum * total_width);

    panel->setDefaultForeground(back_color);

    //panel->print(x, y, "====================");
    for (int h = 0; h < total_width; h++){ 
            panel->putChar((x+h), y, 205);
    }

    
    panel->rect(x, y, total_width, 1, false,TCOD_BKGND_SET);

    //std::cout << "BAR " << bar_width << std::endl;
    //std::cout << "value " << value << std::endl;
    //std::cout << "maximum " << maximum << std::endl;
    //std::cout << "total_width " << total_width << std::endl;
    panel->setDefaultForeground(bar_color);
    if (bar_width > 0){ 
        for (int g = 0; g < bar_width; g++){ 
            panel->putChar((x+g), y, 205);
        }
    }
 
    panel->setDefaultForeground(TCODColor::white);
    panel->setAlignment(TCOD_LEFT);
    TCODConsole::setColorControl(TCOD_COLCTRL_1, label,TCODColor::black);
    panel->print((x + total_width +1), y, "%s[%c%d%c/%d]",name ,TCOD_COLCTRL_1, value,TCOD_COLCTRL_STOP, maximum);
    panel->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black

}



void Message_Log(){

    int panel_offset = 0;
    TCODConsole *whatpanel;
    if(MSG_MODE_XTD){
        MSG_HEIGHT = 75; // 5 times the 15 standard
        whatpanel = panel_xtd;
        panel_offset = 0;
    } else {
        MSG_HEIGHT = 15;
        whatpanel = panel;
        panel_offset = 34;
    }    

    panel->print(33, 1, ">");

    if(msg_log_list.size() > 0){
        whatpanel->setDefaultForeground(TCODColor::white);
        whatpanel->setBackgroundFlag(TCOD_BKGND_SET);
        whatpanel->print(panel_offset, 2, ">");
        /* 
        while(msg_log_list.size() > 80){
            msg_log_list.erase(msg_log_list.begin(),msg_log_list.begin()+1);
        }
        */
        //int i = msg_log_list.size() + 1
        int a = 2;
        int first = 1;
        int howmany = 0;
        howmany = (msg_log_list.size())- MSG_HEIGHT;
        //std::cout << "Quantity " << howmany << std::endl;
        if(howmany < 0) howmany = 0;
        for(int i = (msg_log_list.size()-1); i >= howmany ; i--){
            if (first){
                if (!msg_log_list[i].c1)
                    TCODConsole::setColorControl(TCOD_COLCTRL_1,msg_log_list[i].color1,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_1,msg_log_list[i].color1,msg_log_list[i].bcolor1);
                if (!msg_log_list[i].c2)
                    TCODConsole::setColorControl(TCOD_COLCTRL_2,msg_log_list[i].color2,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_2,msg_log_list[i].color2,msg_log_list[i].bcolor2);
                if (!msg_log_list[i].c3)
                    TCODConsole::setColorControl(TCOD_COLCTRL_3,msg_log_list[i].color3,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_3,msg_log_list[i].color3,msg_log_list[i].bcolor3);
                if (!msg_log_list[i].c4)
                    TCODConsole::setColorControl(TCOD_COLCTRL_4,msg_log_list[i].color4,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_4,msg_log_list[i].color4,msg_log_list[i].bcolor4);
                if (!msg_log_list[i].c5)
                    TCODConsole::setColorControl(TCOD_COLCTRL_5,msg_log_list[i].color5,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_5,msg_log_list[i].color5,msg_log_list[i].bcolor5);
                whatpanel->print(panel_offset+1, a, msg_log_list[i].message,TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, TCOD_COLCTRL_STOP,TCOD_COLCTRL_4,
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_5, TCOD_COLCTRL_STOP );
                a++;
                first = 0;
            } else{
            if (!msg_log_list[i].c1)
                    TCODConsole::setColorControl(TCOD_COLCTRL_1,msg_log_list[i].color1,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_1,msg_log_list[i].color1,msg_log_list[i].bcolor1);
                if (!msg_log_list[i].c2)
                    TCODConsole::setColorControl(TCOD_COLCTRL_2,msg_log_list[i].color2,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_2,msg_log_list[i].color2,msg_log_list[i].bcolor2);
                if (!msg_log_list[i].c3)
                    TCODConsole::setColorControl(TCOD_COLCTRL_3,msg_log_list[i].color3,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_3,msg_log_list[i].color3,msg_log_list[i].bcolor3);
                if (!msg_log_list[i].c4)
                    TCODConsole::setColorControl(TCOD_COLCTRL_4,msg_log_list[i].color4,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_4,msg_log_list[i].color4,msg_log_list[i].bcolor4);
                if (!msg_log_list[i].c5)
                    TCODConsole::setColorControl(TCOD_COLCTRL_5,msg_log_list[i].color5,TCODColor::black);
                else TCODConsole::setColorControl(TCOD_COLCTRL_5,msg_log_list[i].color5,msg_log_list[i].bcolor5);
                whatpanel->print(panel_offset+1, a+1, msg_log_list[i].message,TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, TCOD_COLCTRL_STOP,TCOD_COLCTRL_4,
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_5, TCOD_COLCTRL_STOP );
                a++;
            }
        }

        // just draws frame (not on extended panel)
        for (int n = 0; n < 7; ++n){
            panel->setDefaultForeground(TCODColor::lighterGrey);
            panel->setDefaultBackground(TCODColor::black);
            panel->print(32, n+1, "%c", TCOD_CHAR_VLINE);
        }
        for (int n = 0; n < 20; ++n){
            panel->setDefaultForeground(TCODColor::lighterGrey);
            panel->setDefaultBackground(TCODColor::black);
            panel->print(n+33, 0, "%c", TCOD_CHAR_HLINE);
        } 
        panel->print(32, 0, "%c", TCOD_CHAR_NW);

        /*
        panel->setDefaultForeground(TCODColor::grey);
        panel->print(32, 0, "o");
        panel->print(90, 0, "o");
        panel->print(32, 16, "o");
        panel->print(90, 16, "o");
        for (int n = 1; n < (90-32); n++) panel->print((32+n), 0, "%c",TCOD_CHAR_HLINE);
        for (int n = 1; n < (90-32); n++) panel->print((32+n), 16, "%c",TCOD_CHAR_HLINE);
        for (int n = 1; n < (16); n++) panel->print(32, n, "%c",TCOD_CHAR_VLINE);
        for (int n = 1; n < (16); n++) panel->print(90, n, "%c",TCOD_CHAR_VLINE);
        */
    } else {
        whatpanel->print(panel_offset, 2, ">Message Log currently empty");
    }

}

void render_messagelog(){
    if(wid_combat_open){
            panel->clear();
            panel_xtd->clear();
            Message_Log();
            panel->setDefaultForeground(TCODColor::white);
            //panel->print(win_x-1, (win_y - MAP_HEIGHT_AREA)-4, "^");
            //panel->print(win_x-1, (win_y - MAP_HEIGHT_AREA)-3, "%c", TCOD_CHAR_SE);
            TCODConsole::setColorControl(TCOD_COLCTRL_4,TCODColor::red,TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_5,TCODColor::white,TCODColor::black);
            panel->print(win_x-3, (win_y - MAP_HEIGHT_AREA)-3, "%c[%c%c^%c%c]%c", TCOD_COLCTRL_5, TCOD_COLCTRL_STOP,
                    TCOD_COLCTRL_4, TCOD_COLCTRL_STOP, TCOD_COLCTRL_5, TCOD_COLCTRL_STOP);
            panel->setAlignment(TCOD_LEFT);
            render_bar(1, 1, BAR_WIDTH, "HP", player.stats.hp, player.stats.max_hp,
            TCODColor::lightRed, TCODColor::darkerRed);
            TCODColor mov_bar;
            if (player.combat_move < 4)
                mov_bar = TCODColor::red;
                else mov_bar = TCODColor::white;
            render_bar_s2(1, 2, BAR_WIDTH, "Mov", player.combat_move, 8,
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

void render_base(){
    TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->print(1, MAP_HEIGHT_AREA+3, "Use arrows to move");
        TCODConsole::root->print(1, win_y-3, "Press 'q' to quit");
        
        TCODConsole::root->setAlignment(TCOD_RIGHT);
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+5, "Press 'p' to punch walls");
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+6, "Press 'r' to regenerate layout/revive player");
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+8, "Press 'd' for DEBUG, Press 'w' to switch tile mode");
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::yellow,TCODColor::black);
        if (debug) TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+9, "%cMonster count%c: %d",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, killall);
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

void render_minimaps(){
    int off_x = 0;
    int off_y = 0;

    off_x = player.x - 55; // centered 
    off_y = player.y - 35;
        
    if (off_x < 0) off_x = 0;
    if (off_y < 0) off_y = 0;


    // draws frame of map small
    if(bigg2){
        con_mini->setDefaultForeground(TCODColor::lighterGrey);
        off_x = player.x - 55; // centered 
        off_y = player.y - 35;
        if (off_x < 0) off_x = 0;
        if (off_y < 0) off_y = 0;
        if ((off_x+110) > MAP_WIDTH) off_x = MAP_WIDTH-110;
        if ((off_y+70) > MAP_HEIGHT) off_y = MAP_HEIGHT-70;
        for (int n = 0; n < (MAP_HEIGHT_AREA+1); ++n){ // vertical
            //con_mini->setDefaultBackground(TCODColor::black);
            con_mini->setCharForeground(0+off_x, n+off_y, TCODColor::lighterGrey);
            con_mini->setCharBackground(0+off_x, n+off_y, TCODColor::black, TCOD_BKGND_SET);
            con_mini->setCharForeground((MAP_WIDTH_AREA+1)+off_x, n+off_y, TCODColor::lighterGrey);
            con_mini->setCharBackground((MAP_WIDTH_AREA+1)+off_x, n+off_y, TCODColor::black, TCOD_BKGND_SET);
            con_mini->print(0+off_x, n+off_y, "%c", TCOD_CHAR_VLINE);
            con_mini->print((MAP_WIDTH_AREA+1)+off_x, n+off_y, "%c", TCOD_CHAR_VLINE);
        }
        for (int n = 0; n < MAP_WIDTH_AREA+1; ++n){
            //con_mini->setDefaultBackground(TCODColor::black);
            con_mini->setCharForeground(n+off_x, 0+off_y, TCODColor::lighterGrey);
            con_mini->setCharBackground(n+off_x, 0+off_y, TCODColor::black, TCOD_BKGND_SET);
            con_mini->setCharForeground(n+off_x, (MAP_HEIGHT_AREA+1)+off_y, TCODColor::lighterGrey);
            con_mini->setCharBackground(n+off_x, (MAP_HEIGHT_AREA+1)+off_y, TCODColor::black, TCOD_BKGND_SET);
            con_mini->print(n+off_x, 0+off_y, "%c", TCOD_CHAR_HLINE);
            con_mini->print(n+off_x, (MAP_HEIGHT_AREA+1)+off_y, "%c", TCOD_CHAR_HLINE);
        }
        con_mini->print(0+off_x, 0+off_y, "%c", TCOD_CHAR_NW);
        con_mini->print((MAP_WIDTH_AREA+1)+off_x, 0+off_y, "%c", TCOD_CHAR_NE);
        con_mini->print(0+off_x, (MAP_HEIGHT_AREA+1)+off_y, "%c", TCOD_CHAR_SW);
        con_mini->print((MAP_WIDTH_AREA+1)+off_x, (MAP_HEIGHT_AREA+1)+off_y, "%c", TCOD_CHAR_SE);
        //TCODConsole::blit(con,0,0,111,71,TCODConsole::root,0,0); // minimap layer
        TCODConsole::blit(con_mini,off_x,off_y,112,72,TCODConsole::root,0,1); // minimap layer
        //con_mini->clear();
    }

  
    // draws frame of map tiny    
    if(bigg3){
        con_tini->setDefaultForeground(TCODColor::lighterGrey);
        con_tini->setDefaultBackground(TCODColor::black);
        for (int n = 0; n < MAP_HEIGHT_AREA+1; ++n){
            con_tini->print(0, n, "%c", TCOD_CHAR_VLINE);
            con_tini->print(MAP_WIDTH_AREA+1, n, "%c", TCOD_CHAR_VLINE);
        }
        for (int n = 0; n < MAP_WIDTH_AREA+1; ++n){
            con_tini->print(n, 0, "%c", TCOD_CHAR_HLINE);
            con_tini->print(n, MAP_HEIGHT_AREA+1, "%c", TCOD_CHAR_HLINE);
        }
        con_tini->print(0, 0, "%c", TCOD_CHAR_NW);
        con_tini->print(MAP_WIDTH_AREA+1, 0, "%c", TCOD_CHAR_NE);
        con_tini->print(0, MAP_HEIGHT_AREA+1, "%c", TCOD_CHAR_SW);
        con_tini->print(MAP_WIDTH_AREA+1, MAP_HEIGHT_AREA+1, "%c", TCOD_CHAR_SE);

        // draws tinymap, 2 steps at once, tinyblock goes 0-15 for all subcell
        for (int i = 0; i < (MAP_HEIGHT-2) ;i += 2){ // i = column
            for (int l = 0; l < MAP_WIDTH ;l += 2) { // l = row
                int tinyblock = map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH].blocked | 
                    ( map_array[(i * MAP_WIDTH + (l+1))+ MAP_WIDTH].blocked << 1 ) |
                    ( map_array[((i+1) * MAP_WIDTH + l)+ MAP_WIDTH].blocked << 2 ) |
                    ( map_array[((i+1) * MAP_WIDTH + (l+1))+ MAP_WIDTH].blocked << 3 );
                //std::cout << tinyblock << " ";
                con_tini->putChar((l/2)+1, (i/2)+1, (tinyblock+400), TCOD_BKGND_SET);
                con_tini->setCharForeground((l/2)+1, (i/2)+1, TCODColor::black);
                con_tini->setCharBackground((l/2)+1, (i/2)+1, color_light_ground, TCOD_BKGND_SET);
            }
        }  
        con_tini->putChar((player.x)/2+1, (player.y)/2+1, 400, TCOD_BKGND_SET);
        con_tini->setCharForeground((player.x)/2+1, (player.y)/2+1, TCODColor::black);
        con_tini->setCharBackground((player.x)/2+1, (player.y)/2+1, TCODColor::white, TCOD_BKGND_SET);

        TCODConsole::blit(con_tini,0,0,0,0,TCODConsole::root,0,1); // minimap layer
    } // end bigg3 tinimap
}

void render_top(){
    if(wid_top_open){
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
                if (fov_map->isInFov(monvector[i].x,monvector[i].y)){
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
        widget_top->print(10, 0, "%cIn Sight%c",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        
        // fps count
        int fpscount = TCODSystem::getFps();
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

void render_all (){

    //std::cout << fov_recompute << std::endl;

    int off_x = 0;
    int off_y = 0;

    //if (bigg){
    //    off_x = (player.x * 2) - 55; // centered 
    //    off_y = (player.y * 2) - 35;
    //} else {
        off_x = player.x - 55; // centered 
        off_y = player.y - 35;
    //}    
    if (off_x < 0) off_x = 0;
    if (off_y < 0) off_y = 0;

    off_xx = off_x; // used in monster attack cycle
    off_yy = off_y;

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

    if (fov_recompute){
        fov_map->computeFov(player.x, player.y, TORCH_RADIUS, FOV_LIGHT_WALLS, FOV_ALGO);

        //if(bigg){
        //    maxmap_x = maxmap_x/2;
        //    maxmap_y = maxmap_y/2;
            //drawmap_off_y = drawmap_off_y/2;
            //drawmap_off_x = drawmap_off_x/2;
        //}

        for (int i = drawmap_off_y; i < maxmap_y ;++i){ // i = column
            for (int l = drawmap_off_x; l < maxmap_x ;++l) { // l = row
                visible = fov_map->isInFov(l,i);
                wall = map_array[i * MAP_WIDTH + l].blocked;
                isbloody = map_array[i * MAP_WIDTH + l].bloodyt;

                if (bigg){
                    con->putChar((l*2), (i*2), 503, TCOD_BKGND_SET);
                    con->putChar((l*2)+1, (i*2), 603, TCOD_BKGND_SET);
                    con->putChar((l*2), (i*2)+1, 703, TCOD_BKGND_SET);
                    con->putChar((l*2)+1, (i*2)+1, 803, TCOD_BKGND_SET);
               
                    con->setCharBackground((l*2), (i*2), TCODColor::black, TCOD_BKGND_SET);
                    con->setCharBackground((l*2)+1, (i*2), TCODColor::black, TCOD_BKGND_SET);
                    con->setCharBackground((l*2), (i*2)+1, TCODColor::black, TCOD_BKGND_SET);
                    con->setCharBackground((l*2)+1, (i*2)+1, TCODColor::black, TCOD_BKGND_SET);

                    con->setCharForeground((l*2), (i*2), TCODColor::black);
                    con->setCharForeground((l*2)+1, (i*2), TCODColor::black);
                    con->setCharForeground((l*2), (i*2)+1, TCODColor::black);
                    con->setCharForeground((l*2)+1, (i*2)+1, TCODColor::black);

                } else {
                    con->putChar(l, i, ' ', TCOD_BKGND_SET);
                    con->setCharBackground(l, i, TCODColor::black, TCOD_BKGND_SET);
                    con->setCharForeground(l, i, TCODColor::black);
                }
                if (bigg2){
                    con_mini->putChar(l+1, i+1, ' ', TCOD_BKGND_SET);
                    con_mini->setCharBackground(l+1, i+1, TCODColor::black, TCOD_BKGND_SET);
                    con_mini->setCharForeground(l+1, i+1, TCODColor::black);
                }

                if (!visible){ // if NOT visible
                    if (map_array[i * MAP_WIDTH + l].explored || revealdungeon){
                        if (wall){
                            if(bigg){
                                con->putChar((l*2), (i*2), 503, TCOD_BKGND_SET);
                                con->putChar((l*2)+1, (i*2), 603, TCOD_BKGND_SET);
                                con->putChar((l*2), (i*2)+1, 703, TCOD_BKGND_SET);
                                con->putChar((l*2)+1, (i*2)+1, 803, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_dark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_dark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_dark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_dark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_dark_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_dark_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_dark_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_dark_wall);
                            } else {    
                                con->putChar(l, i, '#', TCOD_BKGND_SET);
                                con->setCharBackground(l, i, color_dark_wall, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, color_dark_wall);
                            }
                            if (bigg2){
                                con_mini->putChar(l+1, i+1, '#', TCOD_BKGND_SET);
                                con_mini->setCharBackground(l+1, i+1, color_dark_wall, TCOD_BKGND_SET);
                                con_mini->setCharForeground(l+1, i+1, color_dark_wall);
                            }
                        } else { // if floor
                            if(bigg){
                                con->putChar(l*2,i*2, 509, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, 609, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, 709, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, 809, TCOD_BKGND_SET); 
                        
                                con->setCharForeground((l*2), (i*2), color_dark_groundF);
                                con->setCharForeground((l*2)+1, (i*2), color_dark_groundF);
                                con->setCharForeground((l*2), (i*2)+1, color_dark_groundF);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_dark_groundF);

                                con->setCharBackground((l*2), (i*2), color_dark_ground, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_dark_ground, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_dark_ground, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_dark_ground, TCOD_BKGND_SET);
                            } else {
                                con->putChar(l, i, '.', TCOD_BKGND_SET);
                                con->setCharForeground(l, i, color_dark_groundF);
                                con->setCharBackground(l, i, color_dark_ground, TCOD_BKGND_SET);
                            }
                            if (bigg2 ){
                                con_mini->putChar(l+1, i+1, '.', TCOD_BKGND_SET);
                                con_mini->setCharForeground(l+1, i+1, color_dark_groundF);
                                con_mini->setCharBackground(l+1, i+1, color_dark_ground, TCOD_BKGND_SET);
                            }
                        }
                    }
                } else { // if visible
                    if (wall){
                        if(bigg){

                            
                            // make doors as wall to correctly draw
                            for (unsigned int n = 0; n<doors.size(); ++n){
                                map_array[doors[n].y * MAP_WIDTH + doors[n].x] = Tile(1,1);
                            }
                            
                            // adark = not hidden in 2*8 mode
                            if( ( (map_array[(i * MAP_WIDTH + l)-1].blocked) && 
                                        (map_array[(i * MAP_WIDTH + l)+1].blocked) ) &&
                                    ( fov_map->isInFov(l+1,i) && fov_map->isInFov(l-1,i) ) ){
                                con->putChar(l*2,i*2, u16_wall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_wall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_wall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_wall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(i * MAP_WIDTH + l)+1].blocked) && 
                                        fov_map->isInFov(l+1,i) ) &&
                                    ( (map_array[(i * MAP_WIDTH + l)+MAP_WIDTH].blocked) &&
                                      fov_map->isInFov(l,i+1) ) ){
                                con->putChar(l*2,i*2, u16_trwall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_trwall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_trwall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_trwall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(i * MAP_WIDTH + l)-1].blocked) && 
                                    fov_map->isInFov(l-1,i) ) &&
                                    ( (map_array[(i * MAP_WIDTH + l)+MAP_WIDTH].blocked) &&
                                    fov_map->isInFov(l,i+1) ) ){
                                con->putChar(l*2,i*2, u16_tlwall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_tlwall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_tlwall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_tlwall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(i * MAP_WIDTH + l)+1].blocked) && 
                                    fov_map->isInFov(l+1,i) ) &&
                                    ( (map_array[(i * MAP_WIDTH + l)-MAP_WIDTH].blocked) &&
                                    fov_map->isInFov(l,i-1) ) ){
                                con->putChar(l*2,i*2, u16_bwall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_bwall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_bwall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_bwall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(i * MAP_WIDTH + l)-MAP_WIDTH].blocked) && 
                                    fov_map->isInFov(l,i-1) ) &&
                                    ( (map_array[(i * MAP_WIDTH + l)-1].blocked) &&
                                    fov_map->isInFov(l-1,i) ) ){
                                con->putChar(l*2,i*2, u16_ibwall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_ibwall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_ibwall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_ibwall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(i * MAP_WIDTH + l)+MAP_WIDTH].blocked) && 
                                    fov_map->isInFov(l,i+1) ) ||
                                    ( (map_array[(i * MAP_WIDTH + l)-MAP_WIDTH].blocked) &&
                                    fov_map->isInFov(l,i-1) ) ){
                                con->putChar(l*2,i*2, u16_vwall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_vwall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_vwall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_vwall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( !(map_array[(i * MAP_WIDTH + l)-MAP_WIDTH].blocked) && 
                                    fov_map->isInFov(l,i-1) ) &&
                                    ( (map_array[(i * MAP_WIDTH + l)+MAP_WIDTH].blocked) &&
                                    fov_map->isInFov(l,i+1) ) &&
                                    ( (map_array[(i * MAP_WIDTH + l)-MAP_WIDTH-1].blocked) &&
                                    fov_map->isInFov(l-1,i-1) ) ){
                                con->putChar(l*2,i*2, u16_tlwall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_tlwall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_tlwall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_tlwall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_bdark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(i * MAP_WIDTH + l)-1].blocked) && 
                                    (map_array[(i * MAP_WIDTH + l)+1].blocked) ) ||
                                    ( fov_map->isInFov(l+1,i) && fov_map->isInFov(l-1,i) ) ){
                                con->putChar(l*2,i*2, u16_wall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_wall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_wall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_wall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(i * MAP_WIDTH + l)].blocked) && 
                                    fov_map->isInFov(l,i) ) &&
                                    ( !(map_array[(i * MAP_WIDTH + l)-MAP_WIDTH].blocked) && 
                                    fov_map->isInFov(l,i-1) ) &&
                                    ( !(map_array[(i * MAP_WIDTH + l)+MAP_WIDTH].blocked) && 
                                    fov_map->isInFov(l,i+1) )
                                     ){
                                con->putChar(l*2,i*2, u16_vwall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_vwall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_vwall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_vwall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if( ( (map_array[(i * MAP_WIDTH + l)-1].blocked) && 
                                    fov_map->isInFov(l-1,i) ) ||
                                    ( (map_array[(i * MAP_WIDTH + l)+1].blocked) && 
                                    fov_map->isInFov(l+1,i)  )
                                     ){
                                con->putChar(l*2,i*2, u16_wall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_wall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_wall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_wall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else if(abs(i-player.x) > abs(l-player.y)
                                     ){
                                con->putChar(l*2,i*2, u16_wall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_wall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_wall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_wall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            } else {
                                con->putChar(l*2,i*2, u16_vwall, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_vwall+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_vwall+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_vwall+300, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_adark_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_adark_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall); 
                            }

                            
                            // reset door hack
                            for (unsigned int n = 0; n<doors.size(); ++n){
                                map_array[doors[n].y * MAP_WIDTH + doors[n].x] = Tile(0,1);
                            }
                                   
                        } else { // if map standard

                            // make doors as wall to correctly draw
                            for (unsigned int n = 0; n<doors.size(); ++n){
                                map_array[doors[n].y * MAP_WIDTH + doors[n].x] = Tile(1,1);
                            }

                            if(!(map_array[(i * MAP_WIDTH + l)-1].blocked) && 
                                    !(map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH].blocked)){ // L NE
                                //con->putChar(l, i, 668, TCOD_BKGND_SET);
                                con->putChar(l, i, u8_wall, TCOD_BKGND_SET);
                                con->setCharBackground(l, i, u8_wallCa, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, u8_wallCb);
                                //con->setCharBackground(l, i, demake_sub_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, demake_main_wall);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall); 
                            } else if( (map_array[(i * MAP_WIDTH + l)-1].blocked) && 
                                    (map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH].blocked) && 
                                    !(map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH-1].blocked) ){ // upR corner
                                //con->putChar(l, i, 669, TCOD_BKGND_SET);
                                con->putChar(l, i, u8_trwall, TCOD_BKGND_SET);
                                con->setCharBackground(l, i, u8_wallCb, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, u8_wallCa);
                                //con->setCharBackground(l, i, demake_main_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, demake_main_wall);
                                //con->setCharBackground(l, i, color_light_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_dark_wallF);

                            } else if(!(map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH].blocked)){ // h wall top
                                //con->putChar(l, i, 666, TCOD_BKGND_SET);
                                //con->putChar(l, i, 446, TCOD_BKGND_SET);
                                con->putChar(l, i, u8_hwall, TCOD_BKGND_SET);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall); 
                                con->setCharBackground(l, i, u8_wallCa, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, u8_wallCb);
                            } else if(!(map_array[(i * MAP_WIDTH + l)-1].blocked)){ // v wall (right)
                                //con->putChar(l, i, 667, TCOD_BKGND_SET);
                                con->putChar(l, i, u8_vwall, TCOD_BKGND_SET);
                                con->setCharBackground(l, i, u8_wallCc, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, u8_wallCd);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                            } else if(!(map_array[(i * MAP_WIDTH + l)+1].blocked)){ // v wall left
                                //con->putChar(l, i, 667, TCOD_BKGND_SET);
                                con->putChar(l, i, ' ', TCOD_BKGND_SET);
                                con->setCharBackground(l, i, u8_wallCb, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, u8_wallCa);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                            } else if(!fov_map->isInFov(l,i+1)){ // h wall bottom
                                //con->putChar(l, i, 667, TCOD_BKGND_SET);
                                //con->putChar(l, i, 446, TCOD_BKGND_SET);
                                con->putChar(l, i, u8_bwall, TCOD_BKGND_SET);
                                con->setCharBackground(l, i, u8_wallCd, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, u8_wallCc);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                            } else if( !(map_array[(i * MAP_WIDTH + l)+MAP_WIDTH+1].blocked) ){ // upL corner
                                //con->putChar(l, i, 667, TCOD_BKGND_SET);
                                //con->putChar(l, i, 446, TCOD_BKGND_SET);
                                con->putChar(l, i, u8_tlwall, TCOD_BKGND_SET);
                                con->setCharBackground(l, i, u8_wallCb, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, u8_wallCa);
                                //con->setCharBackground(l, i, demake_main_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, demake_main_wall);
                                //con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                            } else {    
                                con->putChar(l, i, '#', TCOD_BKGND_SET);
                                //con->setCharBackground(l, i, color_light_wall, TCOD_BKGND_SET);
                                //con->setCharForeground(l, i, color_light_wall);
                                con->setCharBackground(l, i, demake_sub_wall, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, demake_main_wall);
                            }
                            for (unsigned int n = 0; n<doors.size(); ++n){
                                map_array[doors[n].y * MAP_WIDTH + doors[n].x] = Tile(0,1);
                            }
                        }
                        if (bigg2){
                            con_mini->putChar(l+1, i+1, '#', TCOD_BKGND_SET);
                            con_mini->setCharBackground(l+1, i+1, color_light_wall, TCOD_BKGND_SET);
                            con_mini->setCharForeground(l+1, i+1, color_light_wall);
                        }
                    }
                    else { // if floor
                        if(bigg){
                            if(l%2==1){
                                con->putChar(l*2,i*2, u16_floor2, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_floor2+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_floor2+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_floor2+300, TCOD_BKGND_SET);
                            } else {
                                con->putChar(l*2,i*2, u16_floor1, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, u16_floor1+100, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, u16_floor1+200, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, u16_floor1+300, TCOD_BKGND_SET);
                            }

                            con->setCharBackground((l*2), (i*2), TCODColor::black, TCOD_BKGND_SET);
                            con->setCharBackground((l*2)+1, (i*2), TCODColor::black, TCOD_BKGND_SET);
                            con->setCharBackground((l*2), (i*2)+1, TCODColor::black, TCOD_BKGND_SET);
                            con->setCharBackground((l*2)+1, (i*2)+1, TCODColor::black, TCOD_BKGND_SET);

                            con->setCharForeground((l*2), (i*2), color_light_ground2);
                            con->setCharForeground((l*2)+1, (i*2), color_light_ground2);
                            con->setCharForeground((l*2), (i*2)+1, color_light_ground2);
                            con->setCharForeground((l*2)+1, (i*2)+1, color_light_ground2); 

                        } else {
                            //con->putChar(l, i, '.', TCOD_BKGND_SET);
                            if(l%2==1){
                            if(l%3==1 || i%3==1){
                                if(l%3==1)con->putChar(l, i, u8_floor1, TCOD_BKGND_SET);
                                if(i%3==1)con->putChar(l, i, u8_floor2, TCOD_BKGND_SET);
                            }else con->putChar(l, i, u8_floor1, TCOD_BKGND_SET);} 
                            else if(l%2==1)con->putChar(l, i, u8_floor2, TCOD_BKGND_SET); 
                            else con->putChar(l, i, u8_floor2, TCOD_BKGND_SET);
                            //con->setCharForeground(l, i, TCODColor::white);
                            //con->setCharForeground(l, i, demake_main_floor);
                            //con->setCharBackground(l, i, color_light_ground, TCOD_BKGND_SET);
                            //con->setCharBackground(l, i, demake_sub_floor, TCOD_BKGND_SET);
                            con->setCharForeground(l, i, u8_floorCa);
                            con->setCharBackground(l, i, u8_floorCb, TCOD_BKGND_SET);
                        }
                        if (bigg2){
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
                        if(bigg){
                            con->setCharBackground((l*2), (i*2), blood, TCOD_BKGND_SET);
                            con->setCharBackground((l*2)+1, (i*2), blood, TCOD_BKGND_SET);
                            con->setCharBackground((l*2), (i*2)+1, blood, TCOD_BKGND_SET);
                            con->setCharBackground((l*2)+1, (i*2)+1, blood, TCOD_BKGND_SET);
                        } else {   
                            con->setCharBackground(l, i, blood, TCOD_BKGND_SET);
                        }
                    }

                    map_array[i * MAP_WIDTH + l].explored = true;
                    
                }
                //fov_map->setProperties(l, i, map_array[i * MAP_WIDTH + l].block_sight, map_array[i * MAP_WIDTH + l].blocked);
                
            }
        }

    fov_recompute = false;

    }

    // draw doors
    for (unsigned int i = 0; i<doors.size(); ++i){
        if(fov_map->isInFov(doors[i].x,doors[i].y)){
            if(bigg){
                if((map_array[(doors[i].y * MAP_WIDTH + doors[i].x)-1].blocked)){
                    con->putChar(doors[i].x*2,doors[i].y*2, u16_door, TCOD_BKGND_SET);
                    con->putChar((doors[i].x*2)+1,doors[i].y*2, u16_door+100, TCOD_BKGND_SET);
                    con->putChar(doors[i].x*2,(doors[i].y*2)+1, u16_door+200, TCOD_BKGND_SET);
                    con->putChar((doors[i].x*2)+1,(doors[i].y*2)+1, u16_door+300, TCOD_BKGND_SET);
                } else {
                    con->putChar(doors[i].x*2,doors[i].y*2, u16_doorv, TCOD_BKGND_SET);
                    con->putChar((doors[i].x*2)+1,doors[i].y*2, u16_doorv+100, TCOD_BKGND_SET);
                    con->putChar(doors[i].x*2,(doors[i].y*2)+1, u16_doorv+200, TCOD_BKGND_SET);
                    con->putChar((doors[i].x*2)+1,(doors[i].y*2)+1, u16_doorv+300, TCOD_BKGND_SET); 
                }
                    
                con->setCharBackground((doors[i].x*2), (doors[i].y*2), color_door_sub, TCOD_BKGND_SET);
                con->setCharBackground((doors[i].x*2)+1, (doors[i].y*2), color_door_sub, TCOD_BKGND_SET);
                con->setCharBackground((doors[i].x*2), (doors[i].y*2)+1, color_door_sub, TCOD_BKGND_SET);
                con->setCharBackground((doors[i].x*2)+1, (doors[i].y*2)+1, color_door_sub, TCOD_BKGND_SET);
                
                // black for demake, brown for 28
                con->setCharForeground((doors[i].x*2), (doors[i].y*2), color_door_main);
                con->setCharForeground((doors[i].x*2)+1, (doors[i].y*2), color_door_main);
                con->setCharForeground((doors[i].x*2), (doors[i].y*2)+1, color_door_main);
                con->setCharForeground((doors[i].x*2)+1, (doors[i].y*2)+1, color_door_main);
            } else {    
                //con->putChar(doors[i].x, doors[i].y, TCOD_CHAR_CROSS, TCOD_BKGND_SET);
                //con->putChar(doors[i].x, doors[i].y, 444, TCOD_BKGND_SET);
                con->putChar(doors[i].x, doors[i].y, u8_door, TCOD_BKGND_SET);
                // door color swwitches wrong between 8tile and 8ASCII
                con->setCharBackground(doors[i].x, doors[i].y, TCODColor::black, TCOD_BKGND_SET);
                con->setCharForeground(doors[i].x, doors[i].y, door_c);
            }    
        }
    }
    
    for (unsigned int i = 0; i<monvector.size(); ++i) {
        if (monvector[i].selfchar == '%')
        monvector[i].draw(0); // first draws dead bodies
    }

    for (unsigned int i = 0; i<monvector.size(); ++i) {
        if (monvector[i].selfchar != '%')
        monvector[i].draw(0); // then draws monsters still alive
    }


    for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->draw(0); // player vector


    // BLIT MAP TO ROOT (both default and bigg map)
    if(!bigg){
        int smalloff_x = off_x;
        int smalloff_y = off_y;
        if((smalloff_y+MAP_HEIGHT_AREA) > MAP_HEIGHT) smalloff_y = MAP_HEIGHT-MAP_HEIGHT_AREA;
        if((smalloff_x+MAP_WIDTH_AREA) > MAP_WIDTH) smalloff_x = MAP_WIDTH-MAP_WIDTH_AREA;
        // source, source_x, source_y, howmuch_x, howmuch_y, target,
        //std::cout << "SMALLOFF: " << smalloff_x << std::endl;
        TCODConsole::blit(con,smalloff_x,smalloff_y, win_x, win_y,TCODConsole::root,0,0);
        off_xx = smalloff_x;
        off_yy = smalloff_y;
        //std::cout << "SMALLOFF: " << off_xx << " " << off_yy << std::endl;
    } else {
        int bigoff_x = (player.x*2)-55;
        int bigoff_y = (player.y*2)-35;
        if(bigoff_x < 0) bigoff_x = 0;
        if(bigoff_y < 0) bigoff_y = 0;
        // bigg: *2 to pick the correct source point of the square to copy
        TCODConsole::blit(con, bigoff_x, bigoff_y, win_x, win_y,TCODConsole::root,0,0);
        //std::cout << " " << bigoff_x;
        offbig_x = bigoff_x;
        offbig_y = bigoff_y; // sets offset to bring during monster attacks
    }
   
    
        render_top();
    

        if(!combat_mode && !wid_combat_open) render_base();
        render_messagelog();
        render_rpanel();
        render_minimaps();

        if(wid_help){
            render_help();
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

void player_move_attack(int dx, int dy){

    // add movement penalty if player moved
    //int temp_cmove = 0; 
    //if(player.combat_move == 4) {player.cflag_attacks++; temp_cmove++;}
    //if(player.combat_move == 1) {player.cflag_attacks++; temp_cmove++;}
    
    int x = player.x + dx;
    int y = player.y + dy;

    //Object *target;
     unsigned int target = 0;
     bool is_it = false;
    
    for (unsigned int i = 0; i<monvector.size(); ++i){ // checks if monster is in next cell
        if (monvector[i].x == x && monvector[i].y == y){
            //*target = &monvector[i];
            if (monvector[i].alive == true){
                target = i;
                is_it = true;
            }
        }
    }

    if (is_it && monvector[target].alive && player.combat_move >= 4){


        // calculate AML
        int p_AML = player.stats.ML; // basic skill
        p_AML += player.stats.wpn1.wpn_AC; // adding weapon Attack class
        // should check for walls here
        int m_DML = monvector[target].stats.ML; // basic monster skill
        m_DML += monvector[target].stats.wpn1.wpn_AC; // adding weapon Attack class

        // for every attack the player defends from or does, a -10 penality is applied
        if(player.cflag_attacks >= 1){
            p_AML += (player.cflag_attacks * 10) * -1;
            std::cout << "playerA: BASIC " << player.stats.ML << " WEAPON " << player.stats.wpn1.wpn_AC
                << " TOTAL " << p_AML << std:: endl;
        }  
        player.cflag_attacks++; // increment counter (reset at beginning of combat turn)

        // for every attack the monster defends from or does, a -10 penality is applied
        if(monvector[target].cflag_attacks >= 1){
            m_DML += (monvector[target].cflag_attacks * 10) * -1;
            std::cout << "monsterD: BASIC " << monvector[target].stats.ML << " WEAPON " << monvector[target].stats.wpn1.wpn_AC
                << " TOTAL " << m_DML << std:: endl;
        }  
        monvector[target].cflag_attacks++; // increment counter (reset at beginning of combat turn)

        TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why
        short int p_d100 = wtf->getInt(1, 100, 0);
        short int m_d100 = wtf->getInt(1, 100, 0);


        // player (attack)
        short int p_success_level = 0;
        short int crit_val = p_d100 % 10;
        if (p_d100 <= p_AML){
            if ( crit_val == 0 || crit_val == 5 ){
                p_success_level = 0; // CS Critical Success
            } else {    
                p_success_level = 1; // MS Marginal Success
            }    
        } else if (p_d100 > p_AML){
            if ( crit_val == 0 || crit_val == 5){
                p_success_level = 3; // CF Critical Failure
            } else {
                p_success_level = 2; // MF Marginal Failure
            }
        }

        short int m_success_level = 0;
        crit_val = m_d100 % 10;
        if (m_d100 <= m_DML){
            if ( crit_val == 0 || crit_val == 5 ){
                m_success_level = 0; // CS Critical Success
            } else {    
                m_success_level = 1; // MS Marginal Success
            }    
        } else if (m_d100 > m_DML){
            if ( crit_val == 0 || crit_val == 5){
                m_success_level = 3; // CF Critical Failure
            } else {
                m_success_level = 2; // MF Marginal Failure
            }
        }

        const int melee_res[4][4] = 
        {
            { 4, 5, 6, 7, },
            { 4, 4, 5, 6, },
            { 2, 4, 4, 3, },
            { 2, 2, 2, 1, }
        };

        std::cout << "pML: " << player.stats.ML << " p_AML: " << p_AML << " p_d100: " << p_d100 << std::endl;
        std::cout << "mML: " << monvector[target].stats.ML << " m_DML: " << m_DML << " m_d100: " << m_d100 << std::endl;
        std::cout << "P Success Level: " << p_success_level << std::endl;
        std::cout << "M Success Level: " << m_success_level << std::endl;
        std::cout << "Melee Result: " << melee_res[p_success_level][m_success_level] << std::endl;

        msg_log msgd;
        sprintf(msgd.message, "Player's skill(%c%d%c) %c1d100%c(%c%d%c) VS Enemy's defense(%c%d%c) %c1d100%c(%c%d%c)",
                TCOD_COLCTRL_1, p_AML, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_3, p_d100, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, m_DML, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_4, m_d100, TCOD_COLCTRL_STOP);
        msgd.color1 = TCODColor::cyan;
        msgd.color2 = TCODColor::yellow;
        switch(p_success_level){
            case 0:
                msgd.c3 = 1;
                msgd.color3 = TCODColor::white;
                msgd.bcolor3 = TCODColor::blue;
                break;
            case 1:
                msgd.color3 = TCODColor::lighterBlue;
                break;
            case 2:
                msgd.color3 = TCODColor::red;
                break;
            case 3:
                msgd.c3 = 1;
                msgd.color3 = TCODColor::white;
                msgd.bcolor3 = TCODColor::red;
                break;
        }        
        switch(m_success_level){
            case 0:
                msgd.c4 = 1;
                msgd.color4 = TCODColor::white;
                msgd.bcolor4 = TCODColor::blue;
                break;
            case 1:
                msgd.color4 = TCODColor::lighterBlue;
                break;
            case 2:
                msgd.color4 = TCODColor::red;
                break;
            case 3:
                msgd.c4 = 1;
                msgd.color4 = TCODColor::white;
                msgd.bcolor4 = TCODColor::red;
                break;
        }
        msg_log_list.push_back(msgd);        

        msg_log msg1;
        switch(melee_res[p_success_level][m_success_level]){
            case 1:
                player.stats.hp -= 1;
                monvector[target].stats.hp -= 1;
                std::cout << "[CF] Both you and your enemy fumble! -1 to HP for both." << std::endl;
                sprintf(msg1.message, "Both you and your enemy fumble! %c-1%c to HP for both.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 2:
                player.stats.hp -= 1;
                std::cout << "[CF] You fumble the attack! -1 to HP." <<std::endl;
                sprintf(msg1.message, "You fumble the attack! %c-1%c to HP.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 3:
                monvector[target].stats.hp -= 1;
                std::cout << "[MF] You fail the attack, but the monster fumbles and hits himself! -1 to monster HP." 
                    << std::endl;
                sprintf(msg1.message, "You miss, but your enemy fumbles and hurts himself! %c-1%c to HP.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 4:
                std::cout << "You try to attack, but the monster defends itself!" << std::endl;
                sprintf(msg1.message, "You swing your weapon, but the enemy parries.");
                break;
            case 5:
                monvector[target].stats.hp -= 5;
                std::cout << "Your attack does standard damage." << std::endl;
                sprintf(msg1.message, "You hit your enemy for %c5%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 6:
                monvector[target].stats.hp -= 10;
                std::cout << "You attack does double damage!" << std::endl;
                sprintf(msg1.message, "Critial attack! %c10%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 7:
                monvector[target].stats.hp -= 15;
                std::cout << "Your attack does triple damage!" << std::endl;
                sprintf(msg1.message, "Triple damage! %c-15%c HP.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
        }
        msg_log_list.push_back(msg1);

        //player.stats.attack(player, monvector[target], 0); // attack

        if(!no_combat)player.combat_move -= 4; // decrease combat movement only if in combat mode
        if (monvector[target].stats.hp < 1){
                //monvector.erase (target); 
                --killall;
                bloodsplat(monvector[target]);
                bloodycount = 5;

                /* 
                msg_log msg1;
                if (monvector[target].name[0] == 'O'){
                    sprintf(msg1.message, "You've %ckilled%c an %c%s%c!", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,
                        TCOD_COLCTRL_2, monvector[target].name, TCOD_COLCTRL_STOP);
                } else {
                    sprintf(msg1.message, "You've %ckilled%c a %c%s%c!", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,
                        TCOD_COLCTRL_2, monvector[target].name, TCOD_COLCTRL_STOP);
                }
                msg1.color1 = TCODColor::red;
                msg1.color2 = monvector[target].color;
                msg_log_list.push_back(msg1);
                */

                monvector[target].selfchar = '%';
                monvector[target].color = monsterdead;
                monvector[target].blocks = false;
                monvector[target].alive = false;

                monvector[target].self_16 = 526;
                monvector[target].self_8 = 506;

                //monvector[target].x=-365;
                //monvector[target].y=-365; // teleport out of map   
                player.move(0, 0, monvector); // updates player position so feet get bloody
                fov_recompute = true;
                render_all();
                TCODConsole::flush();
                m_x = 0;
                m_y = 0;
            }
            else {
                if(!bigg2){
                    char tchar = monvector[target].selfchar;
                    //TCODConsole::root->clear();
                    monvector[target].colorb = TCODColor::red;
                    monvector[target].hit = true;
                    player.colorb = TCODColor::black;
                    if (!bigg) monvector[target].selfchar = '-';
                    player.draw(1);
                    monvector[target].draw(1);

                     
                    if (bigg){   
                        TCODConsole::blit(con,offbig_x,offbig_y+2,110,68,TCODConsole::root,0,2);
                    } else {    
                    //    TCODConsole::blit(con,0,0,110,70,TCODConsole::root,0,0);
                    //} else {
                        TCODConsole::blit(con,off_xx,off_yy+1,110,69,TCODConsole::root,0,1);
                    }

                    //TCODConsole::blit(con,0,0,0,0,TCODConsole::root,0,0);
                    TCODConsole::flush();
                    Sleep(250); // shitty way for attack "animation", uses windows.h
                    monvector[target].colorb = color_dark_ground;
                    player.colorb = color_dark_ground;
                    monvector[target].hit = false;
                    if (!bigg) monvector[target].selfchar = tchar;
               
                    player.draw(0);
                    monvector[target].draw(0);
                    con->clear();
                    fov_recompute = true;
                    render_all();
                    TCODConsole::flush();
                }

                mesg->setAlignment(TCOD_LEFT);
                mesg->setDefaultForeground(TCODColor::yellow);
                mesg->setDefaultBackground(TCODColor::black);
                mesg->print(1, 1, "Hit!");
                /* 
                msg_log msg1;
                sprintf(msg1.message, "You've hit a %c%s%c!",TCOD_COLCTRL_1, monvector[target].name, TCOD_COLCTRL_STOP);
                msg1.color1 = monvector[target].color;
                //if(msg_log_list.size() > 0) msg_log_list.pop_back();
                msg_log_list.push_back(msg1);
                */
                //if(!(player.y > MAP_HEIGHT-8 )) TCODConsole::blit(mesg,0,0,33,3,con,1,MAP_HEIGHT-4);
                //else TCODConsole::blit(mesg,0,0,33,3,con,MAP_WIDTH-37,1);
               
                std::cout << "monster target hp: " << monvector[target].stats.hp << std::endl;
                m_x = 0;
                m_y = 0;
            }
    } else { 
        //std::cout << "TIMES INTO move loop"  << std::endl;
        //player.move(dx, dy, monvector);
        //fov_recompute = true;
        m_x = dx;
        m_y = dy;
    }    
}

int alreadydead = 0;

int handle_keys(Object_player &duh) {

    bool mycase_p;
    mycase_p = 0;
   
    TCOD_key_t keyr;
    TCOD_mouse_t mouser;
    //TCOD_key_t key = TCODConsole::waitForKeypress(true);
    TCOD_event_t eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&keyr,&mouser);

    if (bloodycount < 0) bloodycount = 0; // if ... change color 
  
    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'q' ) return quit;

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'x' ) return quit2; // quit combat

    //if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'y' ) combat_mode = true;

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'v' ){ if (revealdungeon) revealdungeon = false; else revealdungeon = true; Sleep (100); fov_recompute = true;}

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'd' ){ if (debug) debug = false; else debug = true;
        fov_recompute = true; Sleep (100);}

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'w' ){

        fov_recompute = true;

        if(bigg){
            if(u16_door == t28_door){
                color_bdark_wall.r = 0;
                color_bdark_wall.g = 0;
                color_bdark_wall.b = 0;
                color_door_sub.r = color_light_wall.r;
                color_door_sub.g = color_light_wall.g;
                color_door_sub.b = color_light_wall.b;
                color_door_main.r = 0;
                color_door_main.g = 0;
                color_door_main.b = 0;
                u16_door = t16_door;
                u16_doorv = t16_doorv;
                //u16_player = t16_player; 
                u16_wall = t16_wall;
                u16_vwall = t16_vwall;
                u16_trwall = t16_trwall;
                u16_tlwall = t16_tlwall;
                u16_bwall = t16_bwall;
                u16_ibwall = t16_ibwall;
                u16_floor1 = t16_floor1; 
                u16_floor2 = t16_floor2; 
                //u16_orc = t16_orc; 
                //u16_troll = t16_troll; 
                //u16_corpse = t16_corpse; 
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
                u16_door = t28_door;
                u16_doorv = t28_door;
                u16_player = t28_player; 
                u16_wall = t28_wall; 
                u16_vwall = t28_nwall;
                u16_trwall = t28_nwall;
                u16_tlwall = t28_nwall;
                u16_bwall = t28_wall;
                u16_ibwall = t28_wall;
                u16_floor1 = t28_floor1; 
                u16_floor2 = t28_floor2; 
                u16_orc = t28_orc; 
                u16_troll = t28_troll; 
                u16_corpse = t28_corpse;
            }    
        } else { // block to swap basic 8 mode
            if(u8_door == TCOD_CHAR_CROSS){
                U8 = true; // sprite
                u8_door = 444;
                u8_player = 445;
                u8_floor1 = 447;
                u8_floor2 = 448;   
                u8_wall = 446;
                u8_hwall = 446;
                u8_trwall = ' ';
                u8_tlwall = ' ';
                u8_vwall = ' ';
                u8_bwall = 446;
                u8_orc = 449; 
                u8_troll = 450; 
                u8_corpse = 451;
                u8_wallCa = demake_sub_wall;
                u8_wallCb = demake_main_wall;
                u8_wallCd = demake_sub_wall;
                u8_wallCc = demake_main_wall;
                u8_floorCa = demake_main_floor;
                u8_floorCb = demake_sub_floor;
            } else {
                u8_door = TCOD_CHAR_CROSS;
                U8 = false; // ASCII
                u8_player = '@';
                u8_floor1 = '.';
                u8_floor2 = '.';
                u8_wall = 668;
                u8_hwall = 666;
                u8_trwall = 669;
                u8_tlwall = ' ';
                u8_vwall = 667;
                u8_bwall = ' ';
                u8_orc = 'o'; 
                u8_troll = 'T'; 
                u8_corpse = '%';

                u8_wallCa = color_dark_wallF;
                u8_wallCb = color_light_wall;
                u8_wallCc = color_dark_wallF;
                u8_wallCd = color_light_wall;
                u8_floorCa = TCODColor::white;
                u8_floorCb = color_light_ground;
            }    
        }    

    }    

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'r' ){

        m_x = 0;
        m_y = 0;

        std::cout << " Monster array: " << monvector.size() << std::endl;
        for (unsigned int i = 0; i < monvector.size(); ++i){
            delete monvector[i].path0;
            delete monvector[i].path1;
            monvector.erase (monvector.begin()+i); // erase monster vector on map regen
             
        }    
        monvector.clear();
        std::cout << " Monster array: " << monvector.size() << std::endl; // 0

        for (unsigned int i = 0; i < doors.size(); ++i) doors.erase (doors.begin()+i); // erase monster vector on map regen
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
            if(u16_wall == t16_wall){
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
            if(u16_wall == t28_wall){
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
            if(u16_wall == t16_wall){
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
            if(u16_wall == t28_wall){
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
            if(u16_wall == t16_wall){
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
            if(u16_wall == t28_wall){
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
            if(u16_wall == t16_wall){
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
            if(u16_wall == t28_wall){
                color_bdark_wall.r = color_light_wall.r;
                color_bdark_wall.g = color_light_wall.g;
                color_bdark_wall.b = color_light_wall.b;
            }
        }  

        // set colors for 8 mode
        //if(!bigg){
            if(u8_door != TCOD_CHAR_CROSS){ // demake (is this needed if colors don't change?)
                u8_wallCa = demake_sub_wall;
                u8_wallCb = demake_main_wall;
                u8_wallCd = demake_sub_wall;
                u8_wallCc = demake_main_wall;
                u8_floorCa = demake_main_floor;
                u8_floorCb = demake_sub_floor;
            } else { // ASCII   
                u8_wallCa = color_dark_wallF;
                u8_wallCb = color_light_wall;
                u8_wallCc = color_dark_wallF;
                u8_wallCd = color_light_wall;
                u8_floorCa = TCODColor::white;
                u8_floorCb = color_light_ground;
            }   
        //}    

        //Sleep(4000);
        if (mapmode == 1){
            
            //color_light_wall(0, 0, 100); 
            BSProoms.clear();
            make_map_BSP(duh);
            //make_map(duh);
            mapmode = 0;
        } else {
            BSProoms.clear();
            
            make_map_BSP(duh);
            //make_map2(duh);
            mapmode = 1;
        }    
        duh.bloody = 0;
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                        !(map_array[i * MAP_WIDTH + l].blocked));
                //map_array[row * MAP_WIDTH + l] = Tile(1,1);
            }
        }
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                        !(map_array[i * MAP_WIDTH + l].blocked));
            }
        }
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                fov_map_mons_path0->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                        !(map_array[i * MAP_WIDTH + l].blocked));
            }
        }
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                fov_map_mons_path1->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                        !(map_array[i * MAP_WIDTH + l].blocked));
            }
        }
        fov_recompute = true;
        player.stats.hp = 30;
        player.selfchar = '@';
        game_state = playing;
        player.combat_move = 8;
        set_black();
        alreadydead = 0;
        //    fov_recompute = true;
        render_all();
        TCODConsole::flush(); // this updates the screen
        if(is_handle_combat) return quit2; // regenerates everything and quits combat
    }

    if (game_state == playing) {

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'p' ){
        m_x = 0;
        m_y = 0;
        TCODConsole::root->clear();
        mesg->setAlignment(TCOD_LEFT);
        mesg->setDefaultForeground(TCODColor::white);
        mesg->setDefaultBackground(TCODColor::black);
        mesg->print(1, 1, "Give a direction to dig dungeon");
       
        // CRASH , why?
        //myvector[1]->draw(0);
       
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
                fov_map->setProperties(duh.x, duh.y-1, 1, 1);
                /*
                for (int i = 0; i < MAP_HEIGHT ;++i){
                    for (int l = 0; l < MAP_WIDTH ;++l) {
                            fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                            fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                            fov_map_mons_path->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                           
                            //map_array[row * MAP_WIDTH + l] = Tile(1,1);
                    }
                }
                */
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
                        fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        fov_map_mons_path0->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        fov_map_mons_path1->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
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
                        fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        fov_map_mons_path0->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        fov_map_mons_path1->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
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
                        fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        fov_map_mons_path0->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        fov_map_mons_path1->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), 
                                !(map_array[i * MAP_WIDTH + l].blocked));
                        //map_array[row * MAP_WIDTH + l] = Tile(1,1);
                    }
                }
                fov_recompute = true;
                return false;
            }
        }
    }

    // end of KEY DIG cycle

    if (eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_UP ){
        --bloodycount;
        --duh.bloody;  
        player_move_attack(0, -1);
        //std::cout << " Monster array: " << myvector.size() << std::endl;
    }

    // end KEY UP cycle

    else if (eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_DOWN){
        --bloodycount;
        --duh.bloody; 
        player_move_attack(0, 1);
    }

    // end KEY DOWN cycle

    else if (eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_LEFT){
        --bloodycount;
        --duh.bloody;   
        player_move_attack(-1, 0);
    }

    // end KEY LEFT cycle

    else if ( eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_RIGHT){
        --bloodycount; 
        --duh.bloody; 
        player_move_attack(1, 0);
    }

    // end KEY RIGHT cycle

    else if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == '.'){
        // wasted one mov point without moving
        player_move_attack(0, 0);
        combat_null = true;
    }

    else {
        m_x = 0;
        m_y = 0;
        return no_turn;
    }
    
    
    }
    //std::cout << "player.x: " << duh.x << " player.y: " << duh.y << std::endl; 
    return 0;
}    

void player_death(){
    Fighter fighter_component(0, 0, 0, 0);
    Object_monster monster(player.x, player.y, 'i', TCODColor::black, TCODColor::black, 0, fighter_component);
    game_state = dead;
    player.selfchar = 'X';

    bloodsplat(monster);
    bloodsplat(monster);
    bloodsplat(monster);
    bloodsplat(monster);
      
    TCODConsole::root->setAlignment(TCOD_CENTER);
    TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
    TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
    TCODConsole::root->print(win_x/2,win_y-5,"%cYour DEAD!!1ONE!%c",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
}


struct Monster { int *initiative; int *speed; };

bool compare(Monster a, Monster b) {if (*(a.initiative) != *(b.initiative)) return (*(a.initiative) > *(b.initiative)); else return
    (*(a.speed) > *(b.speed));}

void draw_menu_1(int state){
    TCODConsole::root->setAlignment(TCOD_LEFT);
    TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
    switch(state){
        case 1:
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            TCODConsole::root->print(20, 30, "Generate new character");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(20, 31, "Skip generation");
            TCODConsole::root->print(20, 32, "Quit");
            break;
            case 2:
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(20, 30, "Generate new character");
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            TCODConsole::root->print(20, 31, "Skip generation");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(20, 32, "Quit");
            break;
            case 3:
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(20, 30, "Generate new character");
            TCODConsole::root->print(20, 31, "Skip generation");
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            TCODConsole::root->print(20, 32, "Quit");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            break;
    }
}

void rollstat(int &cap, int &pow, int &spd){
    TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why
    TCODRandom * ftw = TCODRandom::getInstance(); // initializer for random, no idea why
    int added = 6; // 2d6 + 6

    int rolla = 0;
    int rollb = 0;
    int rollt = 0;
    //Sleep(200);
    rolla = wtf->getInt(1, 6, 0);
    rollb = ftw->getInt(1, 6, 0);
    rollt = (rolla + rollb) + added;
    int capt = 0;
    capt = rollt;
            int powt = 100;
            while(powt > capt){
            rolla = wtf->getInt(1, 6, 0);
            rollb = ftw->getInt(1, 6, 0);
            powt = (rolla + rollb) + added;
            }
            int spdt = 100;
            while(spdt > capt){
            rolla = wtf->getInt(1, 6, 0);
            rollb = ftw->getInt(1, 6, 0);
            spdt = (rolla + rollb) + added;
            }  
            cap = capt;
            pow = powt;
            spd = spdt;
}

void draw_frame(const char *title1, const char *title2){
    for (int n = 0; n < win_y; ++n){
        TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
        TCODConsole::root->setDefaultBackground(TCODColor::black);
        TCODConsole::root->print(0, n, "%c", TCOD_CHAR_VLINE);
        TCODConsole::root->print(win_x-1, n, "%c", TCOD_CHAR_VLINE);
    }
    for (int n = 0; n < win_x; ++n){
        TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
        TCODConsole::root->setDefaultBackground(TCODColor::black);
        TCODConsole::root->print(n, 0, "%c", TCOD_CHAR_HLINE);
        TCODConsole::root->print(n, win_y-1, "%c", TCOD_CHAR_HLINE);
    }
    TCODConsole::root->print(0, 0, "%c", TCOD_CHAR_NW);
    TCODConsole::root->print(win_x-1, 0, "%c", TCOD_CHAR_NE);
    TCODConsole::root->print(0, win_y-1, "%c", TCOD_CHAR_SW);
    TCODConsole::root->print(win_x-1, win_y-1, "%c", TCOD_CHAR_SE);

    TCODConsole::root->setAlignment(TCOD_CENTER);
    TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);
    TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::white, TCODColor::black);
    TCODConsole::root->print(win_x/2, 0, "[ %c%s%c - %c%s%c ]", TCOD_COLCTRL_1, title1, TCOD_COLCTRL_STOP,
            TCOD_COLCTRL_2, title2, TCOD_COLCTRL_STOP);

}

// just adds dots to the statistics UI
void dotme(std::vector<int> wheredot){
    TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
    for(unsigned int f = 0; f < wheredot.size(); ++f){
        for(int n = 0; n < 10; ++n){
            TCODConsole::root->print(13+n, wheredot[f], ".");
        }
        for(int n = 0; n < 10; ++n){
            TCODConsole::root->print(34+n, wheredot[f], ".");
        }
        for(int n = 0; n < 10; ++n){
            TCODConsole::root->print(55+n, wheredot[f], ".");
        }
    }
}    

struct tempnumst { int total; int a; int b; int c; };

std::vector<tempnumst> tempnumbers;

// state-rollmenu pickone-numberselected sel-whichnumber pick-bindphase
void draw_menu_2(int state, int pickone, int sel, int rolled, int pick, Statistics &rollst, bool bind, int rolled1ce){
    TCODConsole::root->setAlignment(TCOD_LEFT);
    TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
    TCODConsole::root->setDefaultForeground(TCODColor::white);
    TCODConsole::root->setDefaultBackground(TCODColor::black);
   
    //this block simply flags the lines that need to be dotted
    std::vector<int> dotlines;
    int d1 = 5; 
    dotlines.push_back(d1);
    d1 = 9; 
    dotlines.push_back(d1);
    d1 = 10; 
    dotlines.push_back(d1);
    d1 = 14; 
    dotlines.push_back(d1);
    d1 = 15; 
    dotlines.push_back(d1);
    d1 = 16; 
    dotlines.push_back(d1);
    d1 = 18; 
    dotlines.push_back(d1);
    d1 = 19; 
    dotlines.push_back(d1);
    d1 = 20; 
    dotlines.push_back(d1);
    dotme(dotlines);

    if(pick == 1){
        switch (sel){
            case 1:
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(22, 9, ">");
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                break;
            case 2:
                
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(22, 10, ">");
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                break;
            case 3:
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(43, 9, ">");
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                break;
            case 4:
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(43, 10, ">");
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                break;
            case 5:
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(64, 9, ">");
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                break;
            case 6:
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(64, 10, ">");
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                break;
        }        
        switch(pickone){
            case 1:
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(5, 26, "%d", tempnumbers[0].total);
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                for(unsigned int n = 1; n < tempnumbers.size(); n++){
                    TCODConsole::root->print(5+(n*5), 26, "%d", tempnumbers[n].total);
                }
                /* TCODConsole::root->print(10, 26, "%d", rollst.MR);
                TCODConsole::root->print(15, 26, "%d", rollst.PM);
                TCODConsole::root->print(20, 26, "%d", rollst.PN);
                TCODConsole::root->print(25, 26, "%d", rollst.SM);
                TCODConsole::root->print(30, 26, "%d", rollst.SP);
                */
                break;
            case 2:
                
                TCODConsole::root->print(5, 26, "%d", tempnumbers[0].total); 
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(10, 26, "%d", tempnumbers[1].total);
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                for(unsigned int n = 2; n < tempnumbers.size(); n++){
                    TCODConsole::root->print(5+(n*5), 26, "%d", tempnumbers[n].total);
                }
                break;
            case 3:
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                TCODConsole::root->print(5, 26, "%d", tempnumbers[0].total); 
                TCODConsole::root->print(10, 26, "%d", tempnumbers[1].total);
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(15, 26, "%d", tempnumbers[2].total);
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                for(unsigned int n = 3; n < tempnumbers.size(); n++){
                    TCODConsole::root->print(5+(n*5), 26, "%d", tempnumbers[n].total);
                }
                break;
            case 4:
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                TCODConsole::root->print(5, 26, "%d", tempnumbers[0].total); 
                TCODConsole::root->print(10, 26, "%d", tempnumbers[1].total);
                TCODConsole::root->print(15, 26, "%d", tempnumbers[2].total);
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(20, 26, "%d", tempnumbers[3].total);
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                for(unsigned int n = 4; n < tempnumbers.size(); n++){
                    TCODConsole::root->print(5+(n*5), 26, "%d", tempnumbers[n].total);
                }
                break;
            case 5:
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                TCODConsole::root->print(5, 26, "%d", tempnumbers[0].total); 
                TCODConsole::root->print(10, 26, "%d", tempnumbers[1].total);
                TCODConsole::root->print(15, 26, "%d", tempnumbers[2].total);
                TCODConsole::root->print(20, 26, "%d", tempnumbers[3].total);
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(25, 26, "%d", tempnumbers[4].total);
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                for(unsigned int n = 5; n < tempnumbers.size(); n++){
                    TCODConsole::root->print(5+(n*5), 26, "%d", tempnumbers[n].total);
                }
                break;
            case 6:
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                TCODConsole::root->print(5, 26, "%d", tempnumbers[0].total); 
                TCODConsole::root->print(10, 26, "%d", tempnumbers[1].total);
                TCODConsole::root->print(15, 26, "%d", tempnumbers[2].total);
                TCODConsole::root->print(20, 26, "%d", tempnumbers[3].total);
                TCODConsole::root->print(25, 26, "%d", tempnumbers[4].total);
                TCODConsole::root->setDefaultForeground(TCODColor::black);
                TCODConsole::root->setDefaultBackground(TCODColor::white);
                TCODConsole::root->print(30, 26, "%d", tempnumbers[5].total);
                TCODConsole::root->setDefaultForeground(TCODColor::white);
                TCODConsole::root->setDefaultBackground(TCODColor::black);
                break;    
        }    
        
    } else if (pick !=2){
    switch(state){
        case 1:
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            TCODConsole::root->print(3, 30, "Roll");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(3, 31, "Accept");
            TCODConsole::root->print(3, 32, "Skip generation");
            TCODConsole::root->print(3, 33, "Quit");
            break;
        case 2:
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black); 
            TCODConsole::root->print(3, 30, "Roll");
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            TCODConsole::root->print(3, 31, "Accept");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(3, 32, "Skip generation");
            TCODConsole::root->print(3, 33, "Quit");
            break;
            case 3:
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(3, 30, "Roll");
            TCODConsole::root->print(3, 31, "Accept");
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            TCODConsole::root->print(3, 32, "Skip generation");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(3, 33, "Quit");
            break;
            case 4:
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(3, 30, "Roll");
            TCODConsole::root->print(3, 31, "Accept");
            TCODConsole::root->print(3, 32, "Skip generation");
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            TCODConsole::root->print(3, 33, "Quit");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            break;
    }
    } else {
        switch(state){
        case 1:
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            TCODConsole::root->print(3, 30, "Restart");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(3, 31, "Confirm");
            TCODConsole::root->print(3, 32, "Quit");
            break;
        case 2:
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black); 
            TCODConsole::root->print(3, 30, "Restart");
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            
            TCODConsole::root->print(3, 31, "Confirm");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(3, 32, "Quit");
            break;
            case 3:
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            TCODConsole::root->print(3, 30, "Restart");
            TCODConsole::root->print(3, 31, "Confirm");
            TCODConsole::root->setDefaultForeground(TCODColor::black);
            TCODConsole::root->setDefaultBackground(TCODColor::white);
            TCODConsole::root->print(3, 32, "Quit");
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setDefaultBackground(TCODColor::black);
            break;
        }
    }

    if(bind){ // enter pressed, make the binding
        switch(sel){ // which stat
            case 1:
                //switch(pickone){ // which number
                   // case 1:
                        player.sts.MM = tempnumbers[pickone-1].total;
                        player.sts.MMC = tempnumbers[pickone-1].a;
                        player.sts.MMP = tempnumbers[pickone-1].b;
                        player.sts.MMS = tempnumbers[pickone-1].c;
                      //  break;
                    /* case 2:
                        player.sts.MM = tempnumbers[pickone-1].total;
                        player.sts.MMC = tempnumbers[pickone-1].a;
                        player.sts.MMP = tempnumbers[pickone-1].b;
                        player.sts.MMS = tempnumbers[pickone-1].c;
                        break;
                    case 3:
                        player.sts.MM = tempnumbers[pickone-1].total;
                        player.sts.MMC = tempnumbers[pickone-1].a;
                        player.sts.MMP = tempnumbers[pickone-1].b;
                        player.sts.MMS = tempnumbers[pickone-1].c;
                        break;
                    case 4:
                        player.sts.MM = tempnumbers[pickone-1].total;
                        player.sts.MMC = tempnumbers[pickone-1].a;
                        player.sts.MMP = tempnumbers[pickone-1].b;
                        player.sts.MMS = tempnumbers[pickone-1].c;
                        break;
                    case 5:
                        player.sts.MM = tempnumbers[pickone-1].total;
                        player.sts.MMC = tempnumbers[pickone-1].a;
                        player.sts.MMP = tempnumbers[pickone-1].b;
                        player.sts.MMS = tempnumbers[pickone-1].c;
                        break;
                    case 6:
                        player.sts.MM = tempnumbers[pickone-1].total;
                        player.sts.MMC = tempnumbers[pickone-1].a;
                        player.sts.MMP = tempnumbers[pickone-1].b;
                        player.sts.MMS = tempnumbers[pickone-1].c;
                        break;
                        */
                //}
                tempnumbers.erase(tempnumbers.begin()+(pickone-1));
                break;
            case 2:
                //switch(pickone){
                   // case 1:
                        player.sts.MR = tempnumbers[pickone-1].total;
                        player.sts.MRC = tempnumbers[pickone-1].a;
                        player.sts.MRP = tempnumbers[pickone-1].b;
                        player.sts.MRS = tempnumbers[pickone-1].c;
                       // break;
                        /* 
                    case 2:
                        player.sts.MR = tempnumbers[pickone-1].total;
                        break;
                    case 3:
                        player.sts.MR = tempnumbers[pickone-1].total;
                        break;
                    case 4:
                        player.sts.MR = tempnumbers[pickone-1].total;
                        break;
                    case 5:
                        player.sts.MR = tempnumbers[pickone-1].total;
                        break;
                    case 6:
                        player.sts.MR = tempnumbers[pickone-1].total;
                        break;
                }
                */
                tempnumbers.erase(tempnumbers.begin()+(pickone-1));
                break;
            case 3:
                //switch(pickone){
                  //  case 1:
                        player.sts.PM = tempnumbers[pickone-1].total;
                        player.sts.PMC = tempnumbers[pickone-1].a;
                        player.sts.PMP = tempnumbers[pickone-1].b;
                        player.sts.PMS = tempnumbers[pickone-1].c;
                        /* 
                        break;
                    case 2:
                        player.sts.PM = tempnumbers[pickone-1].total;
                        break;
                    case 3:
                        player.sts.PM = tempnumbers[pickone-1].total;
                        break;
                    case 4:
                        player.sts.PM = tempnumbers[pickone-1].total;
                        break;
                    case 5:
                        player.sts.PM = tempnumbers[pickone-1].total;
                        break;
                    case 6:
                        player.sts.PM = tempnumbers[pickone-1].total;
                        break;
                } 
        */
                tempnumbers.erase(tempnumbers.begin()+(pickone-1));
                break;   
            case 4:
                //switch(pickone){
                    //case 1:
                        player.sts.PN = tempnumbers[pickone-1].total;
                        player.sts.PNC = tempnumbers[pickone-1].a;
                        player.sts.PNP = tempnumbers[pickone-1].b;
                        player.sts.PNS = tempnumbers[pickone-1].c;
                        /* 
                        break;
                    case 2:
                        player.sts.PN = tempnumbers[pickone-1].total;
                        break;
                    case 3:
                        player.sts.PN = tempnumbers[pickone-1].total;
                        break;
                    case 4:
                        player.sts.PN = tempnumbers[pickone-1].total;
                        break;
                    case 5:
                        player.sts.PN = tempnumbers[pickone-1].total;
                        break;
                    case 6:
                        player.sts.PN = tempnumbers[pickone-1].total;
                        break;
                } 
                */ 
                tempnumbers.erase(tempnumbers.begin()+(pickone-1));
                break;
            case 5:
                //switch(pickone){
                   // case 1:
                        player.sts.SM = tempnumbers[pickone-1].total;
                        player.sts.SMC = tempnumbers[pickone-1].a;
                        player.sts.SMP = tempnumbers[pickone-1].b;
                        player.sts.SMS = tempnumbers[pickone-1].c;
                        /* 
                        break;
                    case 2:
                        player.sts.SM = tempnumbers[pickone-1].total;
                        break;
                    case 3:
                        player.sts.SM = tempnumbers[pickone-1].total;
                        break;
                    case 4:
                        player.sts.SM = tempnumbers[pickone-1].total;
                        break;
                    case 5:
                        player.sts.SM = tempnumbers[pickone-1].total;
                        break;
                    case 6:
                        player.sts.SM = tempnumbers[pickone-1].total;
                        break;
                } 
                */
                tempnumbers.erase(tempnumbers.begin()+(pickone-1));
                break;
            case 6:
                //switch(pickone){
                   // case 1:
                        player.sts.SP = tempnumbers[pickone-1].total;
                        player.sts.SPC = tempnumbers[pickone-1].a;
                        player.sts.SPP = tempnumbers[pickone-1].b;
                        player.sts.SPS = tempnumbers[pickone-1].c;
                        /* 
                        break;
                    case 2:
                        player.sts.SP = tempnumbers[pickone-1].total;
                        break;
                    case 3:
                        player.sts.SP = tempnumbers[pickone-1].total;
                        break;
                    case 4:
                        player.sts.SP = tempnumbers[pickone-1].total;
                        break;
                    case 5:
                        player.sts.SP = tempnumbers[pickone-1].total;
                        break;
                    case 6:
                        player.sts.SP = tempnumbers[pickone-1].total;
                        break;
                } 
                */
                tempnumbers.erase(tempnumbers.begin()+(pickone-1));
                break;    
        }
    }

    if(rolled){
    int total = 300;

    while(total > 255){

        tempnumbers.clear();
        for (int n = 0; n < 6; ++n){
        tempnumst tempn;
        rollstat(tempn.a, tempn.b, tempn.c);
        tempn.total = tempn.a + tempn.b + tempn.c;
        tempnumbers.push_back(tempn);
        }
        total = tempnumbers[0].total + tempnumbers[1].total + tempnumbers[2].total + tempnumbers[3].total +
            tempnumbers[4].total + tempnumbers[5].total;


    rollstat(rollst.MMC, rollst.MMP, rollst.MMS);
    rollstat(rollst.MRC, rollst.MRP, rollst.MRS);
    rollstat(rollst.PMC, rollst.PMP, rollst.PMS);
    rollstat(rollst.PNC, rollst.PNP, rollst.PNS);
    rollstat(rollst.SMC, rollst.SMP, rollst.SMS);
    rollstat(rollst.SPC, rollst.SPP, rollst.SPS);
    /*total = rollst.MMC + rollst.MMP + rollst.MMS + rollst.MRC + rollst.MRP + rollst.MRS +
        rollst.PMC + rollst.PMP + rollst.PMS + rollst.PNC + rollst.PNP + rollst.PNS +
        rollst.SMC + rollst.SMP + rollst.SMS + rollst.SPC + rollst.SPP + rollst.SPS;
        */
    }

    rollst.MM = rollst.MMC + rollst.MMP + rollst.MMS;
    rollst.MR = rollst.MRC + rollst.MRP + rollst.MRS;
    rollst.PM = rollst.PMC + rollst.PMP + rollst.PMS;
    rollst.PN = rollst.PNC + rollst.PNP + rollst.PNS;
    rollst.SM = rollst.SMC + rollst.SMP + rollst.SMS;
    rollst.SP = rollst.SPC + rollst.SPP + rollst.SPS;

    
    
    /* 
    tempnumbers.push_back(rollst.MM);
    tempnumbers.push_back(rollst.MR);
    tempnumbers.push_back(rollst.PM);
    tempnumbers.push_back(rollst.PN);
    tempnumbers.push_back(rollst.SM);
    tempnumbers.push_back(rollst.SP);
    */

    player.sts.MM = player.sts.MMC + player.sts.MMP + player.sts.MMS;
    player.sts.MR = player.sts.MRC + player.sts.MRP + player.sts.MRS;
    player.sts.PM = player.sts.PMC + player.sts.PMP + player.sts.PMS;
    player.sts.PN = player.sts.PNC + player.sts.PNP + player.sts.PNS;
    player.sts.SM = player.sts.SMC + player.sts.SMP + player.sts.SMS;
    player.sts.SP = player.sts.SPC + player.sts.SPP + player.sts.SPS;

    }
    player.sts.M = player.sts.MM + player.sts.MR;
    player.sts.P = player.sts.PM + player.sts.PN;
    player.sts.S = player.sts.SM + player.sts.SP;
    

    TCODConsole::root->setDefaultForeground(TCODColor::white);
    if (pick !=2){
        TCODConsole::root->print(3, 24, "ROLLED TOTALS");
    } else {
        TCODConsole::root->print(3, 24, "Character creation complete");
    }
    if(!pick && rolled1ce){
        TCODConsole::root->setDefaultForeground(TCODColor::lightGreen);
    TCODConsole::root->print(5, 26, "%d", tempnumbers[0].total);
    TCODConsole::root->print(10, 26, "%d", tempnumbers[1].total);
    TCODConsole::root->print(15, 26, "%d", tempnumbers[2].total);
    TCODConsole::root->print(20, 26, "%d", tempnumbers[3].total);
    TCODConsole::root->print(25, 26, "%d", tempnumbers[4].total);
    TCODConsole::root->print(30, 26, "%d", tempnumbers[5].total);
    }
   
    if (pick == 2){
        TCODConsole::root->print(0, 0, "DONE");
    }

 

    TCODConsole::root->setDefaultForeground(TCODColor::white);
    TCODConsole::root->print(3, 3, "TRAITS");
    TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
    TCODConsole::root->print(5, 5, "(M)Mental");
    TCODConsole::root->print(26, 5, "(P)Physical");
    TCODConsole::root->print(47, 5, "(S)Spiritual");

    TCODConsole::root->setDefaultForeground(TCODColor::white);
       TCODConsole::root->print(3, 7, "CATEGORIES"); 
       TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
       TCODConsole::root->print(5, 9, "(MM)Mnemonic");
       TCODConsole::root->print(5, 10, "(MR)Reasoning");
       TCODConsole::root->print(26, 9, "(PM)Muscular");
       TCODConsole::root->print(26, 10, "(PN)Neural");
       TCODConsole::root->print(47, 9, "(SM)Metaphysical");
       TCODConsole::root->print(47, 10, "(SP)Psychic");

       TCODConsole::root->setDefaultForeground(TCODColor::white);
       TCODConsole::root->print(3, 12, "ATTRIBUTES");
       TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
       TCODConsole::root->print(5, 14, "(MMC)Capacity");
       TCODConsole::root->print(5, 15, "(MMP)Power");
       TCODConsole::root->print(5, 16, "(MMS)Speed");

       TCODConsole::root->print(5, 18, "(MRC)Capacity");
       TCODConsole::root->print(5, 19, "(MRP)Power");
       TCODConsole::root->print(5, 20, "(MRS)Speed");

       TCODConsole::root->print(26, 14, "(PMC)Capacity");
       TCODConsole::root->print(26, 15, "(PMP)Power");
       TCODConsole::root->print(26, 16, "(PMS)Speed");

       TCODConsole::root->print(26, 18, "(PNC)Capacity");
       TCODConsole::root->print(26, 19, "(PNP)Power");
       TCODConsole::root->print(26, 20, "(PNS)Speed");

       TCODConsole::root->print(47, 14, "(SMC)Capacity");
       TCODConsole::root->print(47, 15, "(SMP)Power");
       TCODConsole::root->print(47, 16, "(SMS)Speed");

       TCODConsole::root->print(47, 18, "(SPC)Capacity");
       TCODConsole::root->print(47, 19, "(SPP)Power");
       TCODConsole::root->print(47, 20, "(SPS)Speed");

       TCODConsole::root->setAlignment(TCOD_RIGHT);
       TCODConsole::root->setDefaultForeground(TCODColor::yellow);

       TCODConsole::root->print(23, 5, "%d", (player.sts.M));
       TCODConsole::root->print(44, 5, "%d", (player.sts.P));
       TCODConsole::root->print(65, 5, "%d", (player.sts.S));

       TCODConsole::root->setDefaultForeground(TCODColor::lighterCyan);
       TCODConsole::root->print(23, 9, "%d", (player.sts.MM));
       TCODConsole::root->print(23, 10, "%d", (player.sts.MR));
       TCODConsole::root->print(44, 9, "%d", (player.sts.PM));
       TCODConsole::root->print(44, 10, "%d", (player.sts.PN));
       TCODConsole::root->print(65, 9, "%d", (player.sts.SM));
       TCODConsole::root->print(65, 10, "%d", (player.sts.SP));

       TCODConsole::root->setDefaultForeground(TCODColor::yellow);
       TCODConsole::root->print(23, 14, "%d", (player.sts.MMC));
       TCODConsole::root->print(23, 15, "%d", (player.sts.MMP));
       TCODConsole::root->print(23, 16, "%d", (player.sts.MMS));

       TCODConsole::root->print(23, 18, "%d", (player.sts.MRC));
       TCODConsole::root->print(23, 19, "%d", (player.sts.MRP));
       TCODConsole::root->print(23, 20, "%d", (player.sts.MRS));

       TCODConsole::root->print(44, 14, "%d", (player.sts.PMC));
       TCODConsole::root->print(44, 15, "%d", (player.sts.PMP));
       TCODConsole::root->print(44, 16, "%d", (player.sts.PMS));

       TCODConsole::root->print(44, 18, "%d", (player.sts.PNC));
       TCODConsole::root->print(44, 19, "%d", (player.sts.PNP));
       TCODConsole::root->print(44, 20, "%d", (player.sts.PNS));

       TCODConsole::root->print(65, 14, "%d", (player.sts.SMC));
       TCODConsole::root->print(65, 15, "%d", (player.sts.SMP));
       TCODConsole::root->print(65, 16, "%d", (player.sts.SMS));

       TCODConsole::root->print(65, 18, "%d", (player.sts.SPC));
       TCODConsole::root->print(65, 19, "%d", (player.sts.SPP));
       TCODConsole::root->print(65, 20, "%d", (player.sts.SPS));
            
        draw_frame("Character Creation", "Choose Attributes");

        TCODConsole::root->setAlignment(TCOD_LEFT);

        for (int n = 0; n < 15; ++n){
        TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
        TCODConsole::root->setDefaultBackground(TCODColor::black);
        TCODConsole::root->print(3, n+36, "%c", TCOD_CHAR_VLINE);
        //TCODConsole::root->print(win_x-1, n, "%c", TCOD_CHAR_VLINE);
    }
    for (int n = 0; n < 30; ++n){
        TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
        TCODConsole::root->setDefaultBackground(TCODColor::black);
        TCODConsole::root->print(n+3, 36, "%c", TCOD_CHAR_HLINE);
        //TCODConsole::root->print(n, win_y-1, "%c", TCOD_CHAR_HLINE);
    }
    TCODConsole::root->print(3, 36, "%c", TCOD_CHAR_NW);

    TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);
    TCODConsole::root->print(4, 37, "Note:");
    TCODConsole::root->printRect(4, 38, 60, 20, "Six numbers will be generated (%c2d6+6%c, for each attribute), when accepted, you can then bind each of the six Categories to one of the numbers. Note that each level corresponds to the sum of the one below. For example: MMC + MMP + MMS = MM, MM + MR = M. \"Capacity\" also defines the maximum \"Power\" and \"Speed\" can reach.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
}

int menu_1(){
    TCOD_key_t key;
    TCOD_mouse_t mouse;
    TCOD_event_t eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);

    if (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_UP ){
        return move_up;
    }    
    if (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_LEFT ){
        return move_up;
    }
    if (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_DOWN ){
        return move_down;
    }    
    if (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_RIGHT ){
        return move_down;
    }    
    if ((eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_ENTER) || 
            (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_SPACE) ){
        return action;
    }  
    return 0;
}



int main() {

    
    //TCODConsole::setCustomFont("arial10x10.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
    TCODConsole::setCustomFont("terminal.png",TCOD_FONT_LAYOUT_ASCII_INCOL,16,256);
    //TCODConsole::setCustomFont("sample_full_unicode.png",TCOD_FONT_LAYOUT_ASCII_INROW,32,2048);
    TCODSystem::setFps(LIMIT_FPS);
        
    myvector.push_back(&player);
    //myvector.push_back(&npc);
   
    make_map_BSP(player);

    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
        }
    }

    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
        }
    }

    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            fov_map_mons_path0->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
        }
    }

    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l) {
            fov_map_mons_path1->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
        }
    }

    player.colorb = con->getCharBackground(player.x, player.y);
    //npc.colorb = con->getCharBackground(npc.x, npc.y);

    player.colorb = color_dark_ground;
    //npc.colorb = color_dark_ground;

    strcpy(player.name, "Playername");

    fov_recompute = true;

    TCODConsole::initRoot(win_x, win_y, "FoE", false);

    game_state = playing;

    // SETS MORE PLAYER ATTRIBUTES
    player.combat_move = 8; // movement points
    player.speed = 6;
    // wrapon setup
    player.stats.wpn1.wpn_AC = 15;
    player.stats.wpn1.wpn_DC = 10;
    player.stats.wpn1.wpn_B = 3;
    player.stats.wpn1.wpn_E = 5;
    player.stats.wpn1.wpn_P = 3;
    player.stats.wpn1.wpn_aspect = 2;

    player.stats.ML = 60;

    bool quit_now = false;

    //bool loped = false; // used for threading?

    //TCODConsole::mapAsciiCodeToFont('s',4,0);
    //TCODConsole::mapAsciiCodeToFont(TCOD_CHAR_HLINE,4,0);

    // x = 1 y = 16 start of new font a = 1,16 + 1,17 / b = 1,18 + 1,19
    //TCODConsole::mapAsciiCodeToFont(666,1,16);
    //TCODConsole::mapAsciiCodeToFont(667,2,16);
    //TCODConsole::mapAsciiCodeToFont(668,1,17);
    //TCODConsole::mapAsciiCodeToFont(669,2,17);

    TCODConsole::mapAsciiCodeToFont(666,0,223);
    TCODConsole::mapAsciiCodeToFont(667,0,231);
    TCODConsole::mapAsciiCodeToFont(668,0,227);
    TCODConsole::mapAsciiCodeToFont(669,0,232);

    map_8x16_font();
    map_16x16_tile();

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

    TCODConsole::mapAsciiCodeToFont(444,9,16); // door 8x8
    TCODConsole::mapAsciiCodeToFont(445,9,17); // player 8x8
    t8_player = 445;
    TCODConsole::mapAsciiCodeToFont(446,9,18); // wall 8x8
    TCODConsole::mapAsciiCodeToFont(447,9,19); // floor 8x8
    TCODConsole::mapAsciiCodeToFont(448,9,20); // floor.b 8x8
    TCODConsole::mapAsciiCodeToFont(449,9,21); // orc 8x8
    TCODConsole::mapAsciiCodeToFont(450,9,22); // troll 8x8
    TCODConsole::mapAsciiCodeToFont(451,9,23); // corpse 8x8

    u16_door = t28_door;
    u16_doorv = t28_door; // same door 
    u16_player = t28_player; 
    u16_wall = t28_wall; 
    u16_vwall = t28_nwall;
    u16_trwall = t28_nwall;
    u16_tlwall = t28_nwall;
    u16_bwall = t28_wall;
    u16_ibwall = t28_wall;
    u16_floor1 = t28_floor1; 
    u16_floor2 = t28_floor2; 
    u16_orc = t28_orc; 
    u16_troll = t28_troll; 
    u16_corpse = t28_corpse;

    //standard 8mode: ASCII 
    u8_door = TCOD_CHAR_CROSS;
    u8_doorv = TCOD_CHAR_CROSS; // same door 
    u8_player = '@'; // 445 
    u8_wall = 668; 
    u8_vwall = 667;
    u8_trwall = 669;
    u8_tlwall = ' ';
    u8_bwall = ' ';
    u8_ibwall = t28_wall;
    u8_floor1 = '.'; 
    u8_floor2 = '.'; 
    u8_orc = 'o'; 
    u8_troll = 'T'; 
    u8_corpse = '%';
    u8_hwall = 666;

    u8_wallCa = color_dark_wallF;
    u8_wallCb = color_light_wall;
    u8_wallCc = color_dark_wallF;
    u8_wallCd = color_light_wall;
    u8_floorCa = TCODColor::white;
    u8_floorCb = TCODColor::grey;
    color_light_ground = TCODColor::grey;
    

    /* 
    msg_log msg1;
    TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
    sprintf(msg1.message, "this is the %c%d%c try!",TCOD_COLCTRL_1, numbr, TCOD_COLCTRL_STOP);
    msg_log_list.push_back(msg1);
    TCODConsole::root->print(1, win_y-4,msg_log_list[0].message); //msg1.message);
    */

    //TCODConsole::disableKeyboardRepeat();

    // MENU CYCLE
    int loopme = 1;
    int menu_index = 1;
    int what_menu = 0;
    while (loopme){
        TCODConsole::root->clear();
      
        draw_menu_1(menu_index);
        what_menu = menu_1();

        if (what_menu == move_up){
            if(menu_index == 1){ 
                menu_index = 3;
            } else --menu_index;
        }    
        
        if (what_menu == move_down){
            if(menu_index == 3){ 
                menu_index = 1;
            } else ++menu_index; 
        }    

        if (what_menu == action && menu_index == 1) loopme = 0;  
        if (what_menu == action && menu_index == 2) loopme = 0;
        if (what_menu == action && menu_index == 3) return 0;

        if (TCODConsole::isWindowClosed()) return 0;

        TCODConsole::flush(); // this updates the screen
    }

    if(menu_index == 1){
        loopme = 1;
        menu_index = 1;
        what_menu = 0;
        int roll = 0; // roll values
        int ch_roll = 0; // accept value
        int rolledonce = 0;
        int pick_index = 1;
        Statistics rollstat(0); // temp for storing rolled values
        int selection = 1; // point to stat to binding
        int selection2 = 6; // total of numbers
        //int selection3 = 4;
        bool doingso = false;
    while (loopme){
        TCODConsole::root->clear();
      
        draw_menu_2(menu_index, pick_index, selection, roll, ch_roll, rollstat, doingso, rolledonce);
        if (roll == 1) rolledonce = 1;
        roll = 0;
        if (doingso) {++selection;--selection2;}
        if (doingso && (pick_index>1)){ --pick_index;}
        if (doingso && (selection > 6)) {ch_roll = 2; menu_index = 1;} // char done
        doingso = false;
       
        what_menu = menu_1();

        if (ch_roll == 1){ // binding numbers
            if (what_menu == move_up){
            if(pick_index == 1){ 
                pick_index = selection2;
            } else --pick_index;
        }    
        
        if (what_menu == move_down){
            if(pick_index == selection2){ 
                pick_index = 1;
            } else ++pick_index; 
        }

        if (what_menu == action){  doingso = true;  }
           //--pick_index;}

        } else if(ch_roll == 2) {
           if (what_menu == move_up){
            if(menu_index == 1){ 
                menu_index = 3;
            } else --menu_index;
        }    
        
        if (what_menu == move_down){
            if(menu_index == 3){ 
                menu_index = 1;
            } else ++menu_index; 
        }  
        if (what_menu == action && menu_index == 1){ ch_roll = 0; selection = 1; selection2 = 6; pick_index = 1;
            what_menu = 0; roll = 0; rolledonce = 0; menu_index = 1;}
        if (what_menu == action && menu_index == 2) loopme = 0;
        if (what_menu == action && menu_index == 3) return 0;

        } else {if (what_menu == move_up){
            if(menu_index == 1){ 
                menu_index = 4;
            } else --menu_index;
        }    
        
        if (what_menu == move_down){
            if(menu_index == 4){ 
                menu_index = 1;
            } else ++menu_index; 
        }  
        if (what_menu == action && menu_index == 1) roll = 1; 
        if ((what_menu == action && menu_index == 2) && rolledonce) ch_roll = 1;
        if (what_menu == action && menu_index == 3) loopme = 0;
        if (what_menu == action && menu_index == 4) return 0;
        }

        

        if (TCODConsole::isWindowClosed()) return 0;

        //std::cout << "Player " << player.sts.MMC << " " << player.sts.MMP << " " << player.sts.MMS << std::endl;

        TCODConsole::flush(); // this updates the screen

        /*
        std::cout << "numbers: ";
        for(int n = 0; n < tempnumbers.size(); ++n){
            std::cout << tempnumbers[n].total << " ";
        } std::cout << " ch " << ch_roll << std::endl;
        */
         
    }
    }

    //uint32 timin1 = 0;
    //uint32 timin2 = 0; 

    //int fpscount = 0;
    //TCODConsole::setKeyboardRepeat(1, 1);

    while (! TCODConsole::isWindowClosed()) {

        
        // FPS debug
        /* 
        ++fpscount;
        if(fpscount > LIMIT_FPS) fpscount = 0;
        std::cout << fpscount << " " << "pl.x " << player.x;
        */


        jump:
        //std::cout << "MAIN LOOP" << std::endl;

        int player_action = 0;
        //TCODConsole::root->putChar( 10,10, 0x2500 );

        if (!alreadydead){
        if(fov_recompute) con->clear();
        TCODConsole::root->clear();
        }
        //fov_recompute = true; // if disabled the screen goes dark

        
        render_all(); 
      
        
        I_am_moused();

        //TCODConsole::root->putChar( 10,10, 0x2500 );
        
        TCODConsole::flush(); // this updates the screen

        
        for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->clear(); // player array, clear previous

        //int pla_x = player.x;
        //int pla_y = player.y;


        bool in_sight;

        combat_mode = false; 

        if (!no_combat){ // debug flag

        for (unsigned int i = 0; i<monvector.size(); ++i) {
            in_sight = fov_map->isInFov(monvector[i].x,monvector[i].y);
            if(in_sight && monvector[i].alive == true){ 
                combat_mode = true; // trigger combat mode, if monster is sighted
                
                wid_combat_open = 1; // default combat panels on
                wid_rpanel_open = 1;
            }    
        } // activates combat mode as soon a monster is in sight, deactivates on subsequent loops

        

        //player.combat_move = 8; // 1 cost for movement, 4 for attack
        while (combat_mode){

            is_handle_combat = true;
            
            // resets initiative & attack counts on all monsters
            for (unsigned int i = 0; i<monvector.size(); ++i) { 
                monvector[i].initiative = -1;
                monvector[i].cflag_attacks = 0; // resets attacks received
                monvector[i].move_counter = 0; // resets number of movements during turn
            } 

            player.cflag_attacks = 0; // resets number of attacks received
            player.move_counter = 0; // resets number of movements during turn

            if (alreadydead) break;
         
            std::vector<Monster> monsters; // vector used for initiative juggling
  
            con->clear();
            TCODConsole::root->clear(); 

            fov_recompute = true;
            render_all();
            TCODConsole::flush(); // this updates the screen
                
            bool break_combat = true;
            
            TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

            
            // updates monster map so that pathing includes monsters position
            for (unsigned int i = 0; i<monvector.size(); ++i){
                if(monvector[i].alive){ // dead monsters don't block pathing
                    fov_map_mons_path1->setProperties(monvector[i].x, monvector[i].y, 1, 0);
                }    
            }  
            
            

            for (unsigned int i = 0; i<monvector.size(); ++i){
                in_sight = fov_map->isInFov(monvector[i].x,monvector[i].y);
                if(!in_sight) monvector[i].in_sight = false; else monvector[i].in_sight = true;
                if( (in_sight && monvector[i].alive) || (monvector[i].chasing && monvector[i].alive) ){
                    monvector[i].c_mode = true;
                    monvector[i].pl_x = player.x; // if player in sight, store player pos
                    monvector[i].pl_y = player.y;
                    monvector[i].chasing = true;
                    monvector[i].bored = 400;
                    monvector[i].boren = false;
                    monvector[i].stuck = false;

                    monvector[i].path0->compute(monvector[i].x,monvector[i].y,monvector[i].pl_x,monvector[i].pl_y);
                    
                    /*
                    int path_x, path_y;
                    monvector[i].path->get(0,&path_x,&path_y);
                    if( is_blocked(path_x, path_y) ){
                        for (unsigned int u = 0; u<monvector.size(); ++u){
                            fov_map_mons_path->setProperties(monvector[u].x, monvector[u].y, 1, 0);
                        }
                        monvector[i].path->compute(monvector[i].x,monvector[i].y,monvector[i].pl_x,monvector[i].pl_y);
                        for (unsigned int u = 0; u<monvector.size(); ++u){
                            fov_map_mons_path->setProperties(monvector[u].x, monvector[u].y, 1, 1);
                        }
                    }   
                    */


                    //std::cout << "Monster coord: " << monvector[i].x << " " << monvector[i].y 
                    //    << " path step: " << path_x << " " << path_y << std::endl; 
                        
                   
                    std::cout << "Monster path size: " << monvector[i].path0->size() << std::endl;

                    // trying to exit combat if no monster in sight EVEN while letting monster out of FoV chasing
                    if( in_sight && monvector[i].alive ){
                        break_combat = false; // set flag, so that if this cycle never entered, combat is interrupted
                    }    

                    int roll = 0;
                    roll = wtf->getInt(1, 10, 0);
                    monvector[i].initiative = monvector[i].speed + roll;
                    monvector[i].temp_init = monvector[i].initiative;

                    Monster tempm;
                    tempm.initiative = &monvector[i].initiative;
                    tempm.speed = &monvector[i].speed;
                    monsters.push_back(tempm);

                    msg_log msg1;
                    if(monvector[i].in_sight)
                        sprintf(msg1.message, "%c>%c%s initiative: %c1d10%c(%d) + SPD(%d) Total: %d.",
                            TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,
                            monvector[i].name, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, roll, 
                            *tempm.speed, monvector[i].initiative); 
                    else 
                        sprintf(msg1.message, "%c>***%c%s initiative: %c1d10%c(%d) + SPD(%d) Total: %d.",
                            TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,
                            monvector[i].name, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, roll, 
                            *tempm.speed, monvector[i].initiative);
                    msg1.color1 = TCODColor::yellow;
                    msg_log_list.push_back(msg1);
                }
            }

            for (unsigned int i = 0; i<monvector.size(); ++i){
                fov_map_mons_path1->setProperties(monvector[i].x, monvector[i].y, 1, 1);
            }
            
            if (break_combat) {wid_combat_open = 0; 
                r_panel->clear(); 
                break;} // break combat mode if monsters all dead or out of FoV

            con->clear();
            r_panel->clear();
            TCODConsole::root->clear();

            int myroll = 0;
            myroll = wtf->getInt(1, 10, 0);
            player.initiative = player.speed + myroll;
            player.temp_init = player.initiative;

            Monster tempm; // player counts as monster for initiative
            tempm.initiative = &player.initiative;
            tempm.speed = &player.speed;
            monsters.push_back(tempm);

            msg_log msg1;
            sprintf(msg1.message, "%c>%cPlayer initiative: %c1d10%c(%d) + SPD(%d) Total: %d.",
                    TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, myroll, 
                player.speed, player.initiative);
            msg1.color1 = TCODColor::yellow;
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

            unsigned int player_own = player.initiative;

            // Create initiative UI list
            r_panel->setDefaultForeground(TCODColor::white);
            TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightAmber,TCODColor::black);
            for (unsigned int i = 0; i<monsters.size(); ++i) {
                if (i == (player_own - 1)){
                    r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(i), "[%c%d%c] Player <", TCOD_COLCTRL_1, 
                            player.temp_init, TCOD_COLCTRL_STOP);
                } else {
                    for (unsigned int b = 0; b<monvector.size(); ++b) {
                        unsigned int monster_ini =  monvector[b].initiative;
                        if(monvector[b].in_sight){
                            if ((i+1) == monster_ini){
                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(i), "[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                    monvector[b].name, TCOD_COLCTRL_STOP);
                            }    
                        } else {
                            if ((i+1) == monster_ini){
                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(i), "***[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                    monvector[b].name, TCOD_COLCTRL_STOP);
                            }
                        }    
                    }
                }
            }  

            //widget_popup->setAlignment(TCOD_CENTER);
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::black,TCODColor::white);
            widget_popup->setBackgroundFlag(TCOD_BKGND_SET);
            widget_popup->print(0, 0,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",TCOD_COLCTRL_2,TCOD_CHAR_NW,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_NE,TCOD_COLCTRL_STOP);
            widget_popup->print(0, 1,"%c%c %c%cSTART COMBAT TURN, Press any key%c%c %c%c"
                    ,TCOD_COLCTRL_2,TCOD_CHAR_VLINE,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_CHAR_VLINE,TCOD_COLCTRL_STOP);
            widget_popup->print(0, 2,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",TCOD_COLCTRL_2,TCOD_CHAR_SW,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_SE,TCOD_COLCTRL_STOP);
            
            fov_recompute = true;
            render_all();
            //TCODConsole::blit(r_panel,0,0,0,0,TCODConsole::root,MAP_WIDTH_AREA, 0);
            TCODConsole::blit(widget_popup,0,0,36,3,TCODConsole::root, 40, 65);
            //TCODConsole::blit(panel,0,0,0,0,TCODConsole::root,0,MAP_HEIGHT_AREA+2);
            //render_messagelog();
            //render_minimaps();
            //render_rpanel();
            TCODConsole::flush(); // this updates the screen
            //TCOD_key_t key2 = 
            TCODConsole::waitForKeypress(true); // to start combat

            // TURN SEQUENCE 
            for (unsigned int i = 0; i<monsters.size(); ++i) {
                if (i == (player_own - 1)){ // -1 because vector starts at 0
                    //std::cout << "Player initiative position: " << player.initiative << std::endl;
                    con->clear();
                    TCODConsole::root->clear();

                    fov_recompute = true;
                    render_all();
                    //render_messagelog();
                    //render_minimaps();

                    // PLAYER BLOCK
                    // PLAYER BLOCK
                    while (player.combat_move >= 1){

                        bool didmove = false; // flag if player moved in this loop

                        player_action = handle_keys(player);

                        if (player_action == quit2){ // quit combat
                            Sleep(100);
                            combat_mode = false;
                            r_panel->clear();
                            goto jump;
                            break;
                        } // exits combat? 
                        if (player_action == quit || TCODConsole::isWindowClosed()){
                            quit_now = true;
                            goto end;
                        } // exits program

                        if ((m_x != 0 || m_y != 0 || combat_null) && player.combat_move > 0){
                            player.move(m_x, m_y, monvector);
                            --player.combat_move;
                            fov_recompute = true;
                            didmove = true;
                            combat_null = false; // reset waiting action
                        } else didmove = false; // player action

                        //con->clear();
                        //TCODConsole::root->clear();
                        //render_all();

                        if(didmove){

                            /*
                            for (unsigned int u = 0; u<monvector.size(); ++u){
                                fov_map_mons_path->setProperties(monvector[u].x, monvector[u].y, 1, 0);
                            }
                            */

                        // monsters chasing and not in FoV do not ger updated    
                        for (unsigned int n = 0; n<monvector.size(); ++n) {
                            in_sight = fov_map->isInFov(monvector[n].x,monvector[n].y);
                            if(in_sight && monvector[n].alive == true){
                                monvector[n].c_mode = true;
                                monvector[n].pl_x = player.x; // if player in sight, store player pos
                                monvector[n].pl_y = player.y;
                                monvector[n].chasing = true;
                                monvector[n].bored = 400;
                                monvector[n].boren = false;
                                monvector[n].stuck = false;
                                monvector[n].path0->compute(monvector[n].x,monvector[n].y,monvector[n].pl_x,monvector[n].pl_y);
                                
                                int path_x, path_y;
                                monvector[n].path0->get(0,&path_x,&path_y);
                                if( is_blocked(path_x, path_y) ){
                                    for (unsigned int u = 0; u<monvector.size(); ++u){
                                        fov_map_mons_path1->setProperties(monvector[u].x, monvector[u].y, 1, 0);
                                    }
                                    monvector[n].path0->compute(monvector[n].x,monvector[n].y,monvector[n].pl_x,monvector[n].pl_y);
                                    for (unsigned int u = 0; u<monvector.size(); ++u){
                                        fov_map_mons_path1->setProperties(monvector[u].x, monvector[u].y, 1, 1);
                                    }
                                }

                                std::cout << "Monster path size: " << monvector[n].path0->size() << std::endl;
                            }
                        }
                            for (unsigned int u = 0; u<monvector.size(); ++u){
                                fov_map_mons_path1->setProperties(monvector[u].x, monvector[u].y, 1, 1);
                            }
                        }

                        // SIDEBAR UI
                        r_panel->clear();
                        TCODConsole::root->setAlignment(TCOD_RIGHT);
                        
                        //r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(i)
                        //TCODConsole::root->print(win_x-1, 0, "Mode-C");

                        r_panel->print((win_x-MAP_WIDTH_AREA)-1, 1, "Initiative list");
                        //TCODConsole::root->print(win_x-1, 3, "Initiative list");

                        // Initiative UI list in player turn
                        TCODConsole::root->setDefaultForeground(TCODColor::white);
                        TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
                        TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightAmber,TCODColor::black);
                        TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::darkRed, TCODColor::black);
                        for (unsigned int n = 0; n<monsters.size(); ++n) {
                            if (n == (player_own - 1)){
                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "[%c%d%c] Player <", TCOD_COLCTRL_1, 
                                    player.temp_init, TCOD_COLCTRL_STOP);
                            } else {
                                for (unsigned int b = 0; b<monvector.size(); ++b) {
                                    unsigned int monster_ini = monvector[b].initiative;
                                    if ((n+1) == monster_ini){
                                        if(monvector[b].in_sight){
                                            if(monster_ini < player_own){
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "%c[%d] %s%c", TCOD_COLCTRL_3,
                                                monvector[b].temp_init, monvector[b].name, TCOD_COLCTRL_STOP);
                                            
                                            } else {  
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                                    monvector[b].name, TCOD_COLCTRL_STOP);
                                            }
                                        } else {
                                            if(monster_ini < player_own){
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "***%c[%d] %s%c", TCOD_COLCTRL_3,
                                                monvector[b].temp_init, monvector[b].name, TCOD_COLCTRL_STOP);
                                            
                                            } else {  
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "***[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                                    monvector[b].name, TCOD_COLCTRL_STOP);
                                            }
                                        }    
                                    }
                                }
                            }
                        }

                        // COMBAT UI
                        panel->clear();
                        panel->setAlignment(TCOD_LEFT);
                        render_bar(1, 1, BAR_WIDTH, "HP", player.stats.hp, player.stats.max_hp,
                            TCODColor::lightRed, TCODColor::darkerRed);
                        TCODColor mov_bar;
                        if (player.combat_move < 4)
                            mov_bar = TCODColor::red;
                        else mov_bar = TCODColor::white;
                        render_bar_s2(1, 2, BAR_WIDTH, "Mov", player.combat_move, 8,
                            TCODColor::lightPurple, TCODColor::darkerPurple, mov_bar);
                        I_am_moused();
                        if(msg_log_list.size() > 0){
                            Message_Log();
                        }
                        //TCODConsole::blit(panel,0,0,0,0,TCODConsole::root,0,MAP_HEIGHT_AREA+2);

                        //render_messagelog();
                        //render_minimaps();
                        //render_rpanel();
                        render_all();
                        TCODConsole::flush(); // this updates the screen
                    }
                    // PLAYER BLOCK
                    // PLAYER BLOCK

                    //con->clear();
                    //TCODConsole::root->clear(); 

             
            //panel->setAlignment(TCOD_CENTER);
            widget_popup->clear();
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
            widget_popup->print(0, 0, "%cpress any key to END Player's turn%c",
                    TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                    
              
            
            fov_recompute = true;
            render_all();
            //TCODConsole::blit(panel,0,0,0,0,TCODConsole::root,0,MAP_HEIGHT_AREA+2);
            TCODConsole::blit(widget_popup,0,0,34,1,TCODConsole::root, 40, 66);
            //render_messagelog();
            //render_minimaps();
            //render_rpanel();
            TCODConsole::flush(); // this updates the screen
                
            TCODConsole::waitForKeypress(true);

                    player.combat_move = 8; // player turn ends, so resets the movement points
                } else {
                    std::cout << "Monster position: " << *(monsters[i].initiative) << std::endl;
                    for (unsigned int b = 0; b<monvector.size(); ++b) {
                        unsigned int monster_ini =  monvector[b].initiative;

                        // MONSTER BLOCK
                        // MONSTER BLOCK
                        if ((i+1) == monster_ini && monvector[b].alive){

                            TCODConsole::root->setAlignment(TCOD_CENTER);
                            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::yellow);
                            //TCODConsole::root->print(win_x/2,win_y-4,"%cMONSTER TURN%c",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);

                            TCODConsole::root->clear();
                            fov_recompute = true;
                            render_all();
                            r_panel->clear();
                            widget_popup->clear();
                            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
                            widget_popup->print(0, 0, "%cMONSTER TURNS%c",
                                TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                                //render_all();
                            TCODConsole::blit(widget_popup,0,0,13,1,TCODConsole::root, (MAP_WIDTH_AREA/2)-6, 66);
                            

                        // Initiative UI list in monster turn
                        TCODConsole::root->setDefaultForeground(TCODColor::white);
                        TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
                        TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightAmber,TCODColor::black);
                        TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::darkRed, TCODColor::black);
                        for (unsigned int n = 0; n<monsters.size(); ++n) {
                            if (n == (player_own - 1)){
                                if(player_own < (i+1)){
                                    r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "%c[%d] Player%c", TCOD_COLCTRL_3, 
                                        player.temp_init, TCOD_COLCTRL_STOP);
                                } else {
                                    r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "[%c%d%c] Player", TCOD_COLCTRL_1, 
                                        player.temp_init, TCOD_COLCTRL_STOP);
                                }
                            } else {
                                for (unsigned int b = 0; b<monvector.size(); ++b) {
                                    unsigned int monster_ini = monvector[b].initiative;
                                    if ((n+1) == monster_ini){
                                        if(monvector[b].in_sight){
                                            if(monster_ini < (i+1)){
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "%c[%d] %s%c", TCOD_COLCTRL_3,
                                                    monvector[b].temp_init, monvector[b].name, TCOD_COLCTRL_STOP);
                                            
                                            } else if(monster_ini == (i+1)){
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "[%c%d%c] %c%s%c <", TCOD_COLCTRL_1,
                                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                                    monvector[b].name, TCOD_COLCTRL_STOP);
                                            } else {   
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                                    monvector[b].name, TCOD_COLCTRL_STOP);
                                            }
                                        } else {
                                            if(monster_ini < (i+1)){
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "***%c[%d] %s%c", TCOD_COLCTRL_3,
                                                    monvector[b].temp_init, monvector[b].name, TCOD_COLCTRL_STOP);
                                            
                                            } else if(monster_ini == (i+1)){
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "***[%c%d%c] %c%s%c <", TCOD_COLCTRL_1,
                                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                                    monvector[b].name, TCOD_COLCTRL_STOP);
                                            } else {   
                                                r_panel->print((win_x-MAP_WIDTH_AREA)-2, 3+(n), "***[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                                    monvector[b].name, TCOD_COLCTRL_STOP);
                                            }
                                        }    
                                    }
                                }
                            }
                        }
                            TCODConsole::flush(); // this block to update player rendering before monster TURN

                            bool seehere = false;
                            seehere = fov_map->isInFov(monvector[b].x,monvector[b].y);
                            std::cout << "monster doing something" << std::endl;
                            int ctl = 0;
                            while (monvector[b].combat_move > 0){
                                std::cout << "monster move: " << monvector[b].combat_move;

                                // take turn (monster)
                                if (monvector[b].myai->take_turn(monvector[b], player, monvector[b].pl_x,
                                        monvector[b].pl_y,seehere) );// render_all();

                                if (player.stats.hp < 1 && !alreadydead ){
                                    player_death();
                                    alreadydead = 1;
                                    goto jump;
                                } else {
                                    //player.move(m_x, m_y, monvector);
                                    //fov_recompute = true; // disables monster trail
                                }

                                if (ctl > 1){
                                    con->clear();
                                    fov_recompute = true;
                                    ctl = 0;
                                }
                                
                                widget_popup->clear();
                                TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
                                widget_popup->print(0, 0, "%cMONSTER TURNS%c", TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                                render_all();
                                TCODConsole::blit(widget_popup,0,0,13,1,TCODConsole::root, (MAP_WIDTH_AREA/2)-6, 66);
                                TCODConsole::flush();
                                Sleep(100);
                                ++ctl; // for trail animation on monster movement
                            }
                            if(monvector[b].color == orc) monvector[b].combat_move = 6;
                            else monvector[b].combat_move = 10;
                        } // if
                        // MONSTER BLOCK
                        // MONSTER BLOCK

                    } // for
                } // else
            }
            
            std::cout << "END COMBAT TURN" << std::endl;
            std::cout << std::endl;
            msg_log msg2;
            sprintf(msg2.message, "%cTURN END%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            msg2.color1 = TCODColor::magenta;
                //if(msg_log_list.size() > 0) msg_log_list.pop_back();
            msg_log_list.push_back(msg2);
            
            
        } // while combat_move

        } else { // no combat
            player.combat_move = 8; // for attack when debug disables combat
        } // reset list if combat didn't happen


        end:
        if (quit_now) break;
       
        is_handle_combat = false;

        // maybe needed for death
        player.move(0, 0, monvector);
        //    fov_recompute = true;

        for (unsigned int i = 0; i<monvector.size(); ++i) { 
            if (monvector[i].alive){
                monvector[i].c_mode = false;
            }
        } // deactivates combat mode for all monsters, so they are properly re-flagged on next loop 
        
        player_action = handle_keys(player);


        if (player_action == quit) break; // break main while loop to exit program


        if (game_state == playing && player_action != no_turn){
            
            for (unsigned int i = 0; i<monvector.size(); ++i) { 
                
                if (monvector[i].alive){ // take turn for every monster alive

                    for (unsigned int u = 0; u<monvector.size(); ++u){
                        // check if alive or dead monsters block path
                        if(monvector[i].alive){
                            //1 = you can, see, walk
                            fov_map_mons_path1->setProperties(monvector[u].x, monvector[u].y, 1, 0);
                        }    
                    }

                    bool in_sight;
                    fov_recompute = true;

                    in_sight = fov_map->isInFov(monvector[i].x,monvector[i].y);
                    // take turn for monster in sight OR monster chasing and not bored
                    if (in_sight || (monvector[i].chasing && !monvector[i].boren)){

                        // compute mons-fov to awake other monsters in view
                        fov_map_mons->computeFov(monvector[i].x,monvector[i].y, MON_RADIUS, FOV_LIGHT_WALLS,FOV_ALGO);
                        //fov_map_mons_path->computeFov(monvector[i].x,monvector[i].y, MON_RADIUS, FOV_LIGHT_WALLS,FOV_ALGO);

                        // if at destination OR stuck -> pick a random destination
                        if ( (monvector[i].pl_x == monvector[i].x && monvector[i].pl_y == monvector[i].y) ||
                            monvector[i].stuck){
                            TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

                            // randomly pick a destination that is valid on the map
                            int vagab_x = 0;
                            int vagab_y = 0;
                            bool retry = true;
                            while(retry){ 
                                vagab_x = wtf->getInt(monvector[i].x - 30, monvector[i].x + 30, 0);
                                if (vagab_x < 0) vagab_x = 0;
                                if (vagab_x > MAP_WIDTH) vagab_x = MAP_WIDTH;
                                vagab_y = wtf->getInt(monvector[i].y - 30, monvector[i].y + 30, 0);
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
                            fov_map_mons_path1->setProperties(monvector[u].x, monvector[u].y, 1, 1);
                        }
                     
                        if (monvector[i].myai->take_turn(monvector[i], player, monvector[i].pl_x,
                                monvector[i].pl_y,in_sight)); //render_all();
                   
                        // awake monsters seen
                        for (unsigned int l = 0; l<monvector.size(); ++l) {
                            if(fov_map_mons->isInFov(monvector[l].x,monvector[l].y) && !monvector[l].chasing) {
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
            player_death();
            alreadydead = 1;
        } else {
            player.move(m_x, m_y, monvector);
            fov_recompute = true;
        } // this updates the player movement after the monster turn

        } // game state

        

        // recuperates turns off combat
        for (unsigned int i = 0; i<monvector.size(); ++i) {
            if (monvector[i].alive){
                if(monvector[i].combat_move < monvector[i].combat_move_max) ++monvector[i].combat_move;
            }    
        }    
        if(player.combat_move < 8) ++player.combat_move; 

    } // main while cycle END
    return 0;
}

/*
timin1 = TCODSystem::getElapsedMilli();
timin2 = TCODSystem::getElapsedMilli();
std::cout << "UI block: " << timin2 - timin1 << std::endl;
*/
      

/* 
                      TCODLine::init(monvector[i].x,monvector[i].y,player.x,player.y);
                    int step_x = monvector[i].x;
                    int step_y = monvector[i].y;
                    bool out_of_sight;
                    out_of_sight = false;
                    do { 
                        if(map_array[step_y * MAP_WIDTH + step_x].blocked){
                            out_of_sight = true;
                            break; 
                        }    
                    } while(!TCODLine::step(&step_x, &step_y));
                    */
