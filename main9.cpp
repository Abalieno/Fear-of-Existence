#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "libtcod.hpp"
#include <windows.h> // for Sleep()

// #include <process.h> //used for threading?

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

int insto_rows = 3; // rows in in sight UI


int bigg = 0; // sets 16x16 font
int bigg2 = 0; // is in minimap
int bigg3 = 0; // is in tinymap

//parameters for dungeon generator
int ROOM_MAX_SIZE = 18;
int ROOM_MIN_SIZE = 6;
int MAX_ROOMS = 30;
int MAX_ROOM_MONSTERS = 3;
unsigned int MAX_TOTAL_MONSTERS = 15;

TCOD_fov_algorithm_t FOV_ALGO = FOV_BASIC; //default FOV algorithm
bool FOV_LIGHT_WALLS = true;
int TORCH_RADIUS = 10;
int MON_RADIUS = 8; // used for monster FoV

//TCODColor color_dark_wall(0, 0, 50);
//TCODColor color_dark_ground(15, 15, 80);
TCODColor color_dark_wall(50, 50, 50);
TCODColor color_dark_ground(15, 15, 15);
TCODColor blood(255, 0, 0);
TCODColor blood1(200, 0, 0);
TCODColor blood2(160, 0, 0);
TCODColor blood3(120, 0, 0);
TCODColor blood4(100, 0, 0);
TCODColor door_c(222, 136, 0);

int off_xx = 0;
    int off_yy = 0;

TCODColor color_light_wall(0, 0, 100);
TCODColor color_light_ground(50, 50, 150);
TCODColor color_dark_groundF(TCODColor::darkGrey);
TCODColor color_dark_wallF(0, 0, 50);
//TCODColor color_light_ground(200, 180, 50);

TCODColor orc(0, 200, 0);
TCODColor troll(0, 255, 0);
TCODColor monsterdead(TCODColor::darkGreen);

bool fov_recompute;

bool debug = false; // shows all monsters on map, when set
bool no_combat = false; // disable combat mode


TCODConsole *con = new TCODConsole(440, 280);
TCODConsole *con_mini = new TCODConsole(MAP_WIDTH+2, MAP_HEIGHT+2);

TCODConsole *con_tini = new TCODConsole(MAP_WIDTH_AREA+2, MAP_HEIGHT_AREA+2); // for tinymap
TCODConsole *mesg = new TCODConsole(33, 3);  // message pop-up drawn on top of "con"
TCODConsole *load = new TCODConsole(win_x, win_y);  // load screen
TCODConsole *widget_1 = new TCODConsole(4, 1);  // UI numbers mapmodes
TCODConsole *widget_2 = new TCODConsole(8, 1);  // UI top widget for objects in sight
TCODConsole *widget_2_p = new TCODConsole(80, MAP_HEIGHT-10); // UI pop up object widget

TCODConsole *panel = new TCODConsole(win_x, (win_y - MAP_HEIGHT_AREA));  // combat UI panel
TCODConsole *r_panel = new TCODConsole((win_x - MAP_WIDTH_AREA), MAP_HEIGHT_AREA); // panel on right of map 
// 30, 46 message log
int BAR_WIDTH = 20;
int MSG_X = 30; // BAR_WIDTH + 2;
int MSG_WIDTH = 93; // SCREEN_WIDTH - BAR_WIDTH - 2 was 63!
unsigned int MSG_HEIGHT = 15;// PANEL_HEIGHT - 1

TCODMap * fov_map = new TCODMap(MAP_WIDTH,MAP_HEIGHT);
TCODMap * fov_map_mons = new TCODMap(MAP_WIDTH,MAP_HEIGHT);

// bool used for background color presence
struct msg_log { char message [94]; TCODColor color1; TCODColor color2; TCODColor color3; TCODColor color4; TCODColor
    color5; TCODColor color6; TCODColor color7; TCODColor color8; TCODColor color9; TCODColor color10; bool c1;
    bool c2; bool c3; bool c4; bool c5; bool c6; bool c7; bool c8; bool c9; bool c10;
    TCODColor bcolor1; TCODColor bcolor2; TCODColor bcolor3; TCODColor bcolor4; TCODColor bcolor5; TCODColor bcolor6;
    TCODColor bcolor7; TCODColor bcolor8; TCODColor bcolor9; TCODColor bcolor10;};

std::vector<msg_log> msg_log_list;

void map_16x16_tile(){
    TCODConsole::mapAsciiCodeToFont(501,14,16);
    TCODConsole::mapAsciiCodeToFont(601,15,16);
    TCODConsole::mapAsciiCodeToFont(701,14,17);
    TCODConsole::mapAsciiCodeToFont(801,15,17); // tile #1 floor

    TCODConsole::mapAsciiCodeToFont(502,10,16);
    TCODConsole::mapAsciiCodeToFont(602,11,16);
    TCODConsole::mapAsciiCodeToFont(702,10,17);
    TCODConsole::mapAsciiCodeToFont(802,11,17); // tile #2 player

    TCODConsole::mapAsciiCodeToFont(503,12,16);
    TCODConsole::mapAsciiCodeToFont(603,13,16);
    TCODConsole::mapAsciiCodeToFont(703,12,17);
    TCODConsole::mapAsciiCodeToFont(803,13,17); // tile #3 null

    TCODConsole::mapAsciiCodeToFont(504,10,18);
    TCODConsole::mapAsciiCodeToFont(604,11,18);
    TCODConsole::mapAsciiCodeToFont(704,10,19);
    TCODConsole::mapAsciiCodeToFont(804,11,19); // tile #4 troll

    TCODConsole::mapAsciiCodeToFont(505,10,20);
    TCODConsole::mapAsciiCodeToFont(605,11,20);
    TCODConsole::mapAsciiCodeToFont(705,10,21);
    TCODConsole::mapAsciiCodeToFont(805,11,21); // tile #5 orc

    TCODConsole::mapAsciiCodeToFont(506,10,22);
    TCODConsole::mapAsciiCodeToFont(606,11,22);
    TCODConsole::mapAsciiCodeToFont(706,10,23);
    TCODConsole::mapAsciiCodeToFont(806,11,23); // tile #6 corpse

    TCODConsole::mapAsciiCodeToFont(507,14,18);
    TCODConsole::mapAsciiCodeToFont(607,15,18);
    TCODConsole::mapAsciiCodeToFont(707,14,19);
    TCODConsole::mapAsciiCodeToFont(807,15,19); // tile #7 wall

    TCODConsole::mapAsciiCodeToFont(508,14,20);
    TCODConsole::mapAsciiCodeToFont(608,15,20);
    TCODConsole::mapAsciiCodeToFont(708,14,21);
    TCODConsole::mapAsciiCodeToFont(808,15,21); // tile #8 door
}

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

class AI;

class Object_monster;


class Object_player;

class Weapon_use {
public:
    char wpn_type[15];

    int wpn_AC;
    int wpn_DC;

    int wpn_B;
    int wpn_E;
    int wpn_P;

    int wpn_aspect;

    Weapon_use(){    
    }

};    

// used both on monsters & players
class Fighter {
public:
    int speed;
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
    TCODColor color;
    TCODColor colorb;
    int h; // temp hit points
    int bloody;
    bool blocks;
    Fighter stats;
    AI * myai;
    bool alive;
    int chase; // used to move monster after player for a short while
    int active; // used for wandering mode
    int pl_x;
    int pl_y;
    bool stuck;
    int bored;
    bool boren; // 100 bored, if to 0, boren true, start recuperating
    bool in_sight;

    int combat_move;
    bool c_mode; // flag monsters for active combat mode
    int speed;
    int initiative;
    int temp_init; // total initiative value for messages/list

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
        if (!uh){
            if(bigg){
                colorb = con->getCharBackground((x*2), (y*2));
            } else {    
                colorb = con->getCharBackground(x, y);
            }
        }

        con->setDefaultBackground(colorb);

        if (fov_map->isInFov(x,y) || debug == 1){
            if(bigg){ // if 16x16        
                if (selfchar == 'T'){
                    con->putChar((x*2), (y*2), 504, TCOD_BKGND_SET);
                    con->putChar((x*2)+1, (y*2), 604, TCOD_BKGND_SET);
                    con->putChar((x*2), (y*2)+1, 704, TCOD_BKGND_SET);
                    con->putChar((x*2)+1, (y*2)+1, 804, TCOD_BKGND_SET);
                    //std::cout << "TROLL";
                } else if (selfchar == 'o'){
                    con->putChar((x*2), (y*2), 505, TCOD_BKGND_SET);
                    con->putChar((x*2)+1, (y*2), 605, TCOD_BKGND_SET);
                    con->putChar((x*2), (y*2)+1, 705, TCOD_BKGND_SET);
                    con->putChar((x*2)+1, (y*2)+1, 805, TCOD_BKGND_SET);
                    //std::cout << "ORC";
                } else if (selfchar == '%'){
                    con->putChar((x*2), (y*2), 506, TCOD_BKGND_SET);
                    con->putChar((x*2)+1, (y*2), 606, TCOD_BKGND_SET);
                    con->putChar((x*2), (y*2)+1, 706, TCOD_BKGND_SET);
                    con->putChar((x*2)+1, (y*2)+1, 806, TCOD_BKGND_SET);
                } 
            } else {
                con->setDefaultForeground(color);
                con->putChar(x, y, selfchar, TCOD_BKGND_SET);
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
    int speed;
    int initiative;
    int temp_init; // total initiative value for messages/list

    Object_player(int a, int b, char pchar, TCODColor oc, TCODColor oc2, int health, Fighter loc_fighter, Statistics
            loc_sts) :
        stats(loc_fighter), sts(loc_sts) {
        x = a;
        y = b;
        selfchar = pchar;
        color = oc;
        colorb = oc2;
        h = health;
        bloody = 0;
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
                colorb = con->getCharBackground(x, y);
            }  
        }    
        con->setDefaultBackground(colorb);
        if (fov_map->isInFov(x,y)){
            if(bigg){
                con->putChar((x*2), (y*2), 502, TCOD_BKGND_SET);
                con->putChar((x*2)+1, (y*2), 602, TCOD_BKGND_SET);
                con->putChar((x*2), (y*2)+1, 702, TCOD_BKGND_SET);
                con->putChar((x*2)+1, (y*2)+1, 802, TCOD_BKGND_SET);
            } else {
                con->putChar(x, y, selfchar, TCOD_BKGND_SET);
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
            if ( (monster.distance_to(p_x, p_y) >= 2) || (monster.chase == 1 && !myfov)){
            if (no_combat || monster.combat_move >= 1)    
            
            { // move up to and including player pos
                
                monster.move_towards(p_x, p_y);
                if(!no_combat)monster.combat_move -= 1;
                std::cout << "The " << monster.name << " is moving." << std::endl;
                return false;
            }} else if (myfov){ 
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
                if(!(player.y > MAP_HEIGHT-8 )) TCODConsole::blit(mesg,0,0,33,3,con,1,MAP_HEIGHT-4);
                else TCODConsole::blit(mesg,0,0,33,3,con,MAP_WIDTH-37,1);

                //if(!bigg2 && !bigg){
                //    TCODConsole::blit(con,0,0,110,70,TCODConsole::root,0,0);
                //} else if(bigg){
                    TCODConsole::blit(con,off_xx,off_yy,110,70,TCODConsole::root,0,0);
                //}

                //TCODConsole::blit(con,0,0,0,0,TCODConsole::root,0,0);
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
                
                monster.stats.attack(player, monster, 1);
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
        int p_AML = monster.stats.ML; // basic skill
        p_AML += monster.stats.wpn1.wpn_AC; // adding weapon Attack class
        // should check for walls here
        int m_DML = player.stats.ML; // basic monster skill
        m_DML += player.stats.wpn1.wpn_AC; // adding weapon Attack class

        TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why
        short int p_d100 = wtf->getInt(1, 100, 0);
        short int m_d100 = wtf->getInt(1, 100, 0);

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

        std::cout << "pML: " << monster.stats.ML << " p_AML: " << p_AML << " p_d100: " << p_d100 << std::endl;
        std::cout << "mML: " << player.stats.ML << " m_DML: " << m_DML << " m_d100: " << m_d100 << std::endl;
        std::cout << "P Success Level: " << p_success_level << std::endl;
        std::cout << "M Success Level: " << m_success_level << std::endl;
        std::cout << "Melee Result: " << melee_res[p_success_level][m_success_level] << std::endl;

        msg_log msgd;
        sprintf(msgd.message, "%c*%cMonster's skill(%c%d%c) %c1d100%c(%c%d%c) VS Player's defense(%c%d%c) %c1d100%c(%c%d%c)",
                TCOD_COLCTRL_5, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, p_AML, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_3, p_d100, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, m_DML, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_4, m_d100, TCOD_COLCTRL_STOP);
        msgd.color1 = TCODColor::cyan;
        msgd.color2 = TCODColor::yellow;
        msgd.color5 = TCODColor::red;
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
                sprintf(msg1.message, "%c*%cThe monster hits you for %c%d%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, monster.stats.power, TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 6:
                player.stats.hp -= monster.stats.power * 2;
                std::cout << "You attack does double damage!" << std::endl;
                sprintf(msg1.message, "%c*%cCritial attack! %c%d%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, (monster.stats.power * 2), TCOD_COLCTRL_STOP);
                msg1.color1 = TCODColor::red;
                break;
            case 7:
                player.stats.hp -= monster.stats.power * 3;
                std::cout << "Your attack does triple damage!" << std::endl;
                sprintf(msg1.message, "%c*%cTriple damage! %c%d%c HP.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, (monster.stats.power * 3), TCOD_COLCTRL_STOP);
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
       // monvector[i].draw(0);
    }
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
       
        int tempx = 0;
        int tempy = 0;
        tempx = x + dx;
        tempy = y + dy;

        if (!is_blocked(tempx,tempy)){
            x += dx;
            y += dy;
            stuck = false;
            if (bloody > 0){
                if (bloody >= map_array[y * MAP_WIDTH + x].bloodyt)
                map_array[y * MAP_WIDTH + x].bloodyt = bloody;
            }
        } else { 
            if (dy == -1 && dx == 0){
                if (p_dir == false){
                    if (!is_blocked(tempx-1,y)){
                        x += (dx-1);
                        stuck = false;
                    } else stuck = true;
                }    
                else if (!is_blocked(tempx+1,y)){
                    x += (dx+1);
                    stuck = false;
                } else stuck = true;
            } 
            else if (dx == -1 && dy == 0){
                if (p_dir == false){
                    if (!is_blocked(x,tempy-1)){
                        y += (dy-1);
                        stuck = false;
                    } else stuck = true;
                }    
                else if (!is_blocked(x,tempy+1)){
                    y += (dy+1);
                    stuck = false;
                } else stuck = true;
            }
            else if (dy == 1 && dx == 0){
                if (p_dir == false){
                    if (!is_blocked(tempx-1,y)){
                        x += (dx-1);
                        stuck = false;
                    } else stuck = true;
                }    
                else if (!is_blocked(tempx+1,y)){
                    x += (dx+1);
                    stuck = false;
                } else stuck = true;
            } 
            else if (dx == 1 && dy == 0){
                if (p_dir == false){
                    if (!is_blocked(x,tempy-1)){
                        y += (dy-1);
                        stuck = false;
                    } else stuck = true;
                }    
                else if (!is_blocked(x,tempy+1))
                {y += (dy+1);
                    stuck = false;
                    } else stuck = true;
            }
            //std::cout << "Fuck, it's blocked. " << std::endl;
            stuck = true;
        }    

    }

void Object_player::move(int dx, int dy, std::vector<Object_monster> smonvector) {
       
        int tempx = 0;
        int tempy = 0;
        tempx = x + dx;
        tempy = y + dy;

        if (!is_blocked(tempx,tempy)){
            x += dx;
            y += dy;
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


void place_objects(Rect room){

    if (monvector.size() >= MAX_TOTAL_MONSTERS) return ;

    TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

    //int empty_room = wtf->getInt(0, 100, 0);
    //if (empty_room < 60){
    int num_mosters = wtf->getInt(0, MAX_ROOM_MONSTERS, 0);

    int x, y;

    Fighter fighter_component(0, 0, 0, 0);
    Object_monster monster(0, 0, 'i', TCODColor::black, TCODColor::black, 0, fighter_component);

    for (int i = 0; i < num_mosters; ++i){
        x = wtf->getInt((room.x1+1), (room.x2-1), 0);
        y = wtf->getInt((room.y1+1), (room.y2-1), 0);
        if (!is_blocked(x,y)){
        if ( wtf->getInt(0, 100, 0) < 80){
            Fighter fighter_component(10, 0, 2, 2); // hp, defense, power, speed
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
            monster.chase = 0;
            monster.stuck = 0;
            monster.bored = 500;
            monster.boren = false;
            monster.in_sight = false;
            monster.combat_move = 6;
            monster.c_mode = false;
            monster.speed = 4; // for initiative

            monster.stats.wpn1.wpn_AC = 12;
            monster.stats.wpn1.wpn_DC = 4;
            monster.stats.wpn1.wpn_B = 3;
            monster.stats.wpn1.wpn_aspect = 1;

            monster.stats.ML = 35;
          
        }
        else {
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
            monster.chase = 0;
            monster.stuck = 0;
            monster.bored = 500;
            monster.boren = false;
            monster.in_sight = false;
            monster.combat_move = 10;
            monster.c_mode = false;
            monster.speed = 8;

            monster.stats.wpn1.wpn_AC = 15;
            monster.stats.wpn1.wpn_DC = 5;
            monster.stats.wpn1.wpn_B = 4;
            monster.stats.wpn1.wpn_aspect = 1;

            monster.stats.ML = 50;

        }  
        monvector.push_back(monster);
        }
     }
 
    std::cout << " Monster array: " << myvector.size() << std::endl;
   // }
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
            monster.chase = 0;
            monster.stuck = 0;
            monster.bored = 500;
            monster.boren = false;
            monster.in_sight = false;
            monster.combat_move = 6;
            monster.c_mode = false;
            monster.speed = 4; // for initiative
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
            monster.chase = 0;
            monster.stuck = 0;
            monster.bored = 500;
            monster.boren = false;
            monster.in_sight = false;
            monster.combat_move = 10;
            monster.c_mode = false;
            monster.speed = 8;
            monvector.push_back(monster);
            }
        }

    }

    duh.x = 110; // player set position
    duh.y = 70;
    killall = monvector.size();
}

void place_doors(){

    TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why

    for (int i = 0; i < MAP_HEIGHT ;++i){
        for (int l = 0; l < MAP_WIDTH ;++l){
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

}

void make_map(Object_player &duh){
           
    map_array.resize(MAP_HEIGHT * MAP_WIDTH);
        
    int row = 0;
    
    for (int i = 0; i < MAP_HEIGHT ;++i){
        
        for (int l = 0; l < MAP_WIDTH ;++l) {
           map_array[row * MAP_WIDTH + l] = Tile(1,1);
           //map_array[3597] = Tile(1,1);
        }
        ++row;
    } // fills map with walls 

    

    std::vector<Rect> rooms;
    int num_rooms = 0;

    int prev_x = 0;
    int prev_y = 0;

    int cycle = 0;
    bool secd = false;

    for (int r = 0; r < MAX_ROOMS; ++r){

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
            /* 
            load->setAlignment(TCOD_CENTER);
            if (cycle){
                load->print(win_x/2, (win_y/2)-1, " %c ",TCOD_CHAR_HLINE);
                load->print(win_x/2, win_y/2,     "%c%c%c",TCOD_CHAR_VLINE,TCOD_CHAR_CHECKBOX_UNSET,TCOD_CHAR_VLINE);
                load->print(win_x/2, (win_y/2)+1, " %c ", TCOD_CHAR_HLINE);
                //load->print(win_x/2, (win_y/2)-1, " %c ",TCOD_CHAR_ARROW2_N);
                //load->print(win_x/2, win_y/2,     "%c%c%c",TCOD_CHAR_ARROW2_W,TCOD_CHAR_CHECKBOX_UNSET,TCOD_CHAR_ARROW2_E);
                //load->print(win_x/2, (win_y/2)+1, " %c ", TCOD_CHAR_ARROW2_S);
                load->print(win_x/2, (win_y/2)-3, "Generating new level");
            cycle = false;
            } else {
                load->print(win_x/2, (win_y/2)-1, "%c %c",TCOD_CHAR_NW,TCOD_CHAR_NE);
                load->print(win_x/2, win_y/2,     " %c ", TCOD_CHAR_CHECKBOX_UNSET);
                load->print(win_x/2, (win_y/2)+1, "%c %c",TCOD_CHAR_SW,TCOD_CHAR_SE);
                load->print(win_x/2, (win_y/2)-3, "Generating new level");
            cycle = true;
            } */
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
            
            if (num_rooms == 0){
                duh.x = new_room.center_x;
                duh.y = new_room.center_y; // new player coordinates from room 0
            } else {
               // npc.x = new_room.center_x;
               // npc.y = new_room.center_y; // new npc coordinates from whatever room
              
                place_objects(new_room); // only add monsters if not first room
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
    ++init;

    place_doors();
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

// in combat
void I_am_moused(){
    //TCOD_key_t key;
    //TCOD_mouse_t mouse;
    //TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
    mousez = TCODMouse::getStatus();
    int x = mousez.cx;
    int y = mousez.cy;

    char *whatis;

    if(mousez.lbutton && mousez.cx == 0 && mousez.cy == 0) {stance_pos = 1; bigg = 0;}
    if(mousez.lbutton && mousez.cx == 1 && mousez.cy == 0) {stance_pos = 2; bigg =1;}
    //if(mousez.lbutton && mousez.cx == 2 && mousez.cy == 0) stance_pos = 3;
    if(mousez.cx == 2 && mousez.cy == 0) {
        bigg2 = 1;
    } else {
        bigg2 = 0;
    }  
    if(mousez.cx == 3 && mousez.cy == 0) {
        bigg3 = 1;
    } else {
        bigg3 = 0;
    }
    if( (mousez.cx >= 6 && mousez.cx < 14) && mousez.cy == 0) {
        stance_pos2 = 1;
    } else {
        stance_pos2 = 0;
    }

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


// out of combat
void I_am_moused2(){ // doubled because of main loop, changes where messages are printed
    //TCOD_key_t key;
    //TCOD_mouse_t mouse;
    //TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
    mousez = TCODMouse::getStatus();
    int x = mousez.cx;
    int y = mousez.cy;

    char *whatis;

    if(mousez.lbutton && mousez.cx == 0 && mousez.cy == 0) {stance_pos = 1; bigg = 0;}
    if(mousez.lbutton && mousez.cx == 1 && mousez.cy == 0) {stance_pos = 2; bigg =1;}
    //if(mousez.lbutton && mousez.cx == 2 && mousez.cy == 0) stance_pos = 3;
    if(mousez.cx == 2 && mousez.cy == 0) {
        bigg2 = 1;
    } else {
        bigg2 = 0;
    }
    if(mousez.cx == 3 && mousez.cy == 0) {
        bigg3 = 1;
    } else {
        bigg3 = 0;
    }
    if( (mousez.cx >= 6 && mousez.cx < 14) && mousez.cy == 0) {
        stance_pos2 = 1;
    } else {
        stance_pos2 = 0;
    }

    //std::cout << "MOUSE " << x << " " << y << std::endl;

    bool found = false;
    TCODColor col_obj; // color of object

    if(!bigg){
    
    if ((!bigg && x == player.x && y == player.y) || (bigg && (x/2) == player.x && (y/2) == player.y)){
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->print(1, win_y-5, "Mouse on [Player] at [%d.%d]", x, y);
        TCODConsole::root->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
    } else {
        for (unsigned int n = 0; n<monvector.size(); ++n) {
        if( (!bigg && (x == monvector[n].x && y == monvector[n].y && monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y)) ||
                (bigg && ((x/2) == monvector[n].x && (y/2) == monvector[n].y && monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y)) ){
            whatis = &(monvector[n].name[0]);
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setAlignment(TCOD_LEFT);
            col_obj = monvector[n].color;
            TCODConsole::setColorControl(TCOD_COLCTRL_1,col_obj,TCODColor::black);
            TCODConsole::root->print(1, win_y-5, "Mouse on [%c%s%c] at [%d.%d]",TCOD_COLCTRL_1, whatis, TCOD_COLCTRL_STOP, x, y);
            TCODConsole::root->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
            found = true;
        } 
    }
        if (!found){ // only if no moster alive found
        for (unsigned int n = 0; n<monvector.size(); ++n) {
        if(( (!bigg && (x == monvector[n].x && y == monvector[n].y) && !(monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y)) ||
                (bigg && ((x/2) == monvector[n].x && (y/2) == monvector[n].y) && !(monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y))    )){
            whatis = &(monvector[n].name[0]);
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::root->setAlignment(TCOD_LEFT);
            col_obj = monvector[n].color;
            TCODConsole::setColorControl(TCOD_COLCTRL_1,col_obj,TCODColor::black);
            TCODConsole::root->print(1, win_y-5, "Mouse on [dead %c%s%c] at [%d.%d]",TCOD_COLCTRL_1, whatis, TCOD_COLCTRL_STOP, x, y);
            TCODConsole::root->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
            found = true;
        } 
    }
    }
    
    if (!found){
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->print(1, win_y-5, "Mouse on [Nothing] at [%d.%d]", x, y);
        TCODConsole::root->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
        found = false;
    }
    }

    }

    int asciiwhat = 0;
    if(bigg){
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->setAlignment(TCOD_LEFT);
        asciiwhat= TCODConsole::root->getChar(x, y);
        if (asciiwhat == 502 || asciiwhat == 602 || asciiwhat == 702 || asciiwhat == 802){
            TCODConsole::root->print(1, win_y-5, "Mouse on [Player] at [%d.%d]", x, y);
        } else if(asciiwhat == 501 || asciiwhat == 601 || asciiwhat == 701 || asciiwhat == 801){
            TCODConsole::root->print(1, win_y-5, "Mouse on [Floor] at [%d.%d]", x, y);
        } else if(asciiwhat == 504 || asciiwhat == 604 || asciiwhat == 704 || asciiwhat == 804){
            TCODConsole::root->print(1, win_y-5, "Mouse on [Troll] at [%d.%d]", x, y);
        } else if(asciiwhat == 505 || asciiwhat == 605 || asciiwhat == 705 || asciiwhat == 805){
            TCODConsole::root->print(1, win_y-5, "Mouse on [Orc] at [%d.%d]", x, y);
        } else if(asciiwhat == 506 || asciiwhat == 606 || asciiwhat == 706 || asciiwhat == 806){
            TCODConsole::root->print(1, win_y-5, "Mouse on [Remains] at [%d.%d]", x, y);
        }
        TCODConsole::root->setDefaultBackground(TCODColor::black);
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

int draw_obj_list(){
return 0;
}

void render_all (){

    bool wall = false;
    bool visible = false;
    int isbloody = 0;

    if (fov_recompute){
        fov_map->computeFov(player.x,player.y, TORCH_RADIUS, FOV_LIGHT_WALLS,FOV_ALGO);

        for (int i = 0; i < MAP_HEIGHT ;++i){ // i = column
            for (int l = 0; l < MAP_WIDTH ;++l) { // l = row
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

                if (!visible){
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
                    }
                    else {
                        if(bigg){
                            con->putChar(l*2,i*2, 501, TCOD_BKGND_SET);
                            con->putChar((l*2)+1,i*2, 601, TCOD_BKGND_SET);
                            con->putChar(l*2,(i*2)+1, 701, TCOD_BKGND_SET);
                            con->putChar((l*2)+1,(i*2)+1, 801, TCOD_BKGND_SET); 
                        
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
                } else {
                    if (wall){
                        if(bigg){
                            if(!(map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH].blocked)){ // isometric walls
                                con->putChar(l*2,i*2, 507, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, 607, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, 707, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, 807, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_dark_wallF, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_dark_wallF, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_dark_wallF, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_dark_wallF, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall);
                            } else {    
                                con->putChar(l*2,i*2, 503, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,i*2, 603, TCOD_BKGND_SET);
                                con->putChar(l*2,(i*2)+1, 703, TCOD_BKGND_SET);
                                con->putChar((l*2)+1,(i*2)+1, 803, TCOD_BKGND_SET);

                                con->setCharBackground((l*2), (i*2), color_light_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2), color_light_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2), (i*2)+1, color_light_wall, TCOD_BKGND_SET);
                                con->setCharBackground((l*2)+1, (i*2)+1, color_light_wall, TCOD_BKGND_SET);

                                con->setCharForeground((l*2), (i*2), color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2), color_light_wall);
                                con->setCharForeground((l*2), (i*2)+1, color_light_wall);
                                con->setCharForeground((l*2)+1, (i*2)+1, color_light_wall);
                            }        
                        } else {
                            if(!(map_array[(i * MAP_WIDTH + l)-1].blocked) && 
                                    !(map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH].blocked)){ // isometric walls
                                con->putChar(l, i, 668, TCOD_BKGND_SET);
                                con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, color_light_wall); 
                            } else if( (map_array[(i * MAP_WIDTH + l)-1].blocked) && 
                                    (map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH].blocked) && 
                                    !(map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH-1].blocked) ){
                                con->putChar(l, i, 669, TCOD_BKGND_SET);
                                con->setCharBackground(l, i, color_light_wall, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, color_dark_wallF);

                            } else if(!(map_array[(i * MAP_WIDTH + l)+ MAP_WIDTH].blocked)){
                                con->putChar(l, i, 666, TCOD_BKGND_SET);
                                con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, color_light_wall); 
                            } else if(!(map_array[(i * MAP_WIDTH + l)-1].blocked)){
                                con->putChar(l, i, 667, TCOD_BKGND_SET);
                                con->setCharBackground(l, i, color_dark_wallF, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, color_light_wall);
                            } else {    
                                con->putChar(l, i, '#', TCOD_BKGND_SET);
                                con->setCharBackground(l, i, color_light_wall, TCOD_BKGND_SET);
                                con->setCharForeground(l, i, color_light_wall);
                            }
                        }
                        if (bigg2){
                            con_mini->putChar(l+1, i+1, '#', TCOD_BKGND_SET);
                            con_mini->setCharBackground(l+1, i+1, color_light_wall, TCOD_BKGND_SET);
                            con_mini->setCharForeground(l+1, i+1, color_light_wall);
                        }
                    }
                    else {
                        if(bigg){
                            con->putChar(l*2,i*2, 501, TCOD_BKGND_SET);
                            con->putChar((l*2)+1,i*2, 601, TCOD_BKGND_SET);
                            con->putChar(l*2,(i*2)+1, 701, TCOD_BKGND_SET);
                            con->putChar((l*2)+1,(i*2)+1, 801, TCOD_BKGND_SET);

                            con->setCharForeground((l*2), (i*2), TCODColor::white);
                            con->setCharForeground((l*2)+1, (i*2), TCODColor::white);
                            //con->setCharForeground((l*2), (i*2)+1, TCODColor::white);
                            //con->setCharForeground((l*2)+1, (i*2)+1, TCODColor::white);

                            //con->setCharBackground((l*2), (i*2), color_light_ground, TCOD_BKGND_SET);
                            //con->setCharBackground((l*2)+1, (i*2), color_light_ground, TCOD_BKGND_SET);
                            //con->setCharBackground((l*2), (i*2)+1, color_light_ground, TCOD_BKGND_SET);
                            //con->setCharBackground((l*2)+1, (i*2)+1, color_light_ground, TCOD_BKGND_SET);
                        } else {
                            con->putChar(l, i, '.', TCOD_BKGND_SET);
                            con->setCharForeground(l, i, TCODColor::white);
                            con->setCharBackground(l, i, color_light_ground, TCOD_BKGND_SET);
                        }
                        if (bigg2){
                            con_mini->putChar(l+1, i+1, '.', TCOD_BKGND_SET);
                            con_mini->setCharForeground(l+1, i+1, TCODColor::white);
                            con_mini->setCharBackground(l+1, i+1, color_light_ground, TCOD_BKGND_SET);
                        }
                        
                        
                    }
                    if (isbloody > 0){
                if (isbloody >= 4) blood = blood1;
                if (isbloody == 3) blood = blood2;
                if (isbloody == 2) blood = blood3;
                if (isbloody < 2) blood = blood4;
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
                con->putChar(doors[i].x*2,doors[i].y*2, 508, TCOD_BKGND_SET);
                con->putChar((doors[i].x*2)+1,doors[i].y*2, 608, TCOD_BKGND_SET);
                con->putChar(doors[i].x*2,(doors[i].y*2)+1, 708, TCOD_BKGND_SET);
                con->putChar((doors[i].x*2)+1,(doors[i].y*2)+1, 808, TCOD_BKGND_SET);

                con->setCharBackground((doors[i].x*2), (doors[i].y*2), door_c, TCOD_BKGND_SET);
                con->setCharBackground((doors[i].x*2)+1, (doors[i].y*2), door_c, TCOD_BKGND_SET);
                con->setCharBackground((doors[i].x*2), (doors[i].y*2)+1, door_c, TCOD_BKGND_SET);
                con->setCharBackground((doors[i].x*2)+1, (doors[i].y*2)+1, door_c, TCOD_BKGND_SET);

                con->setCharForeground((doors[i].x*2), (doors[i].y*2), TCODColor::black);
                con->setCharForeground((doors[i].x*2)+1, (doors[i].y*2), TCODColor::black);
                con->setCharForeground((doors[i].x*2), (doors[i].y*2)+1, TCODColor::black);
                con->setCharForeground((doors[i].x*2)+1, (doors[i].y*2)+1, TCODColor::black);
            } else {    
                con->putChar(doors[i].x, doors[i].y, TCOD_CHAR_CROSS, TCOD_BKGND_SET);
                con->setCharBackground(doors[i].x, doors[i].y, door_c, TCOD_BKGND_SET);
                con->setCharForeground(doors[i].x, doors[i].y, TCODColor::black);
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

    

    /* 
    con->putChar(10, 10, 666, TCOD_BKGND_SET);
    con->putChar(10+1, 10, 667, TCOD_BKGND_SET);
    con->putChar(10, 10+1, 668, TCOD_BKGND_SET);
    con->putChar(10+1, 10+1, 669, TCOD_BKGND_SET);
    */
    /*
    print_8x16(10, 10, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", TCODColor::red, TCODColor::black);
    print_8x16(10, 12, "abcdefghijklmnopqrstuvwxyz", TCODColor::red, TCODColor::black);
    print_8x16(10, 14, "\"Hello world!\" 1234", TCODColor::white, TCODColor::black);
    

    con->setDefaultForeground(TCODColor::red);
    con->setDefaultBackground(TCODColor::black);
    con->putChar(10, 17, 501, TCOD_BKGND_SET);
    con->putChar(11, 17, 601, TCOD_BKGND_SET);
    con->putChar(10, 18, 701, TCOD_BKGND_SET);
    con->putChar(11, 18, 801, TCOD_BKGND_SET);
    */
    int off_x = 0;
    int off_y = 0;

    if (bigg){
        off_x = (player.x * 2) - 55; // centered 
        off_y = (player.y * 2) - 35;
    } else {
        off_x = player.x - 55; // centered 
        off_y = player.y - 35;
    }    
    if (off_x < 0) off_x = 0;
    if (off_y < 0) off_y = 0;

    off_xx = off_x; // used in monster attack cycle
    off_yy = off_y;
    
    if(!bigg){
        TCODConsole::blit(con,off_x,off_y,110,70,TCODConsole::root,0,0);
    } else {
        // bigg: 
        TCODConsole::blit(con,off_x,off_y,110,70,TCODConsole::root,0,0);
    }
   
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
        TCODConsole::blit(con_mini,off_x,off_y,112,72,TCODConsole::root,0,0); // minimap layer
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

    //TCODImage *pix = new TCODImage(TCODConsole::root);
   // if(imageinit){
    /* 
    float whattimenow = 0;
    whattimenow = TCODSystem::getElapsedSeconds();
    if((whattime+5) < whattimenow || whattime == 0){
        whattime = TCODSystem::getElapsedSeconds();
        TCODImage *pix = new TCODImage(TCODConsole::root);
        //imageinit = 0;
        //pix->save("mypic.bmp");
        pix->scale(110,70); // if blitted2x it shows at half size
        //pix->blit2x(TCODConsole::root, 0, 0, 0, 0, -1, -1);
        //pix->save("mypic2.bmp");
  //  }   
        //pix->scale(1024,728);   
        std::cout << "loopin";
        //pix->refreshConsole(TCODConsole::root);
        //pix->scale(220,140);
        //pix->blitRect(TCODConsole::root, 0, 0,  -1, -1);
        pix->blit2x(TCODConsole::root, 0, 0, 0, 0, -1, -1);
       // imageinit = 1;

        //TCODConsole::blit(con_tini,0,0,0,0,TCODConsole::root,0,0); // minimap layer
        //con_tini->clear();
        delete pix; 
    } */ 
        TCODConsole::blit(con_tini,0,0,0,0,TCODConsole::root,0,0); // minimap layer
    } // end bigg3 tinimap

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

    widget_1->setBackgroundFlag(TCOD_BKGND_SET);
    widget_1->print(0, 0, "%c1%c%c2%c%c34%c",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,
            TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
    //widget_1->print(10, 0, "Hello.");
    TCODConsole::blit(widget_1,0,0,0,0,TCODConsole::root,0,0);

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
        TCODConsole::blit(widget_2_p,0,0,50,obj_line+2,TCODConsole::root,6,1);  
    } else {
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
    }   
    widget_2->setBackgroundFlag(TCOD_BKGND_SET);
    widget_2->print(0, 0, "%cIn Sight%c",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::blit(widget_2,0,0,0,0,TCODConsole::root,6,0);
    

    // fps count
        int fpscount = TCODSystem::getFps();
        TCODConsole::root->print(100, 0, "FPS: %d", fpscount);
    
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

void player_move_attack(int dx, int dy){
    
    int x = player.x + dx;
    int y = player.y + dy;

    //Object *target;
     unsigned   int target = 0;
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

        TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why
        short int p_d100 = wtf->getInt(1, 100, 0);
        short int m_d100 = wtf->getInt(1, 100, 0);

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
                    player.colorb = TCODColor::black;
                    if (!bigg) monvector[target].selfchar = '-';
                    player.draw(1);
                    monvector[target].draw(1);

                    //if (!bigg){    
                    //    TCODConsole::blit(con,0,0,110,70,TCODConsole::root,0,0);
                    //} else {
                        TCODConsole::blit(con,off_xx,off_yy,110,70,TCODConsole::root,0,0);
                    //}

                    //TCODConsole::blit(con,0,0,0,0,TCODConsole::root,0,0);
                    TCODConsole::flush();
                    Sleep(250); // shitty way for attack "animation", uses windows.h
                    monvector[target].colorb = color_dark_ground;
                    player.colorb = color_dark_ground;
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
                if(!(player.y > MAP_HEIGHT-8 )) TCODConsole::blit(mesg,0,0,33,3,con,1,MAP_HEIGHT-4);
                else TCODConsole::blit(mesg,0,0,33,3,con,MAP_WIDTH-37,1);
               
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
  
    if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'q' ) return quit;

    if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'v' ){ if (revealdungeon) revealdungeon = false; else revealdungeon = true; Sleep (100);}

    if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'd' ){ if (debug) debug = false; else debug = true; Sleep (100);}

    if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'r' ){

        m_x = 0;
        m_y = 0;

        std::cout << " Monster array: " << monvector.size() << std::endl;
        unsigned int b = monvector.size();
        for (unsigned int i = 0; i < b; ++i) monvector.erase (monvector.begin()+i); // erase monster vector on map regen
        std::cout << " Monster array: " << monvector.size() << std::endl; // 0

        b = doors.size();
        for (unsigned int i = 0; i < b; ++i) doors.erase (doors.begin()+i); // erase monster vector on map regen

        //Sleep(4000);
        if (mapmode == 1){
            make_map(duh);
            mapmode = 0;
        } else {
            make_map2(duh);
            mapmode = 1;
        }    
        duh.bloody = 0;
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                fov_map->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
                //map_array[row * MAP_WIDTH + l] = Tile(1,1);
            }

        }
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
                //map_array[row * MAP_WIDTH + l] = Tile(1,1);
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
    }

    if (game_state == playing) {

    if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'p' ){
        m_x = 0;
        m_y = 0;
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

    else {
        m_x = 0;
        m_y = 0;
        return no_turn;
    }
    
    // end KEY RIGHT cycle
    }
    //std::cout << "player.x: " << duh.x << " player.y: " << duh.y << std::endl; 
    return 0;
}    

int handle_combat(Object_player &duh) {

    bool mycase_p;
    mycase_p = 0;

    TCOD_key_t keyr;
    TCOD_mouse_t mouser;
    //TCOD_key_t key = TCODConsole::waitForKeypress(true);
    TCOD_event_t eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&keyr,&mouser);
    
    //TCOD_key_t key = TCODConsole::waitForKeypress(true); 

    if (bloodycount < 0) bloodycount = 0; // if ... change color 
  
    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'x' ) return quit; // quit combat

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'q' ) return quit2;

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'd' ){ if (debug) debug = false; else debug = true; Sleep (100);}

    if ( eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'r' ){

        m_x = 0;
        m_y = 0;

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
        for (int i = 0; i < MAP_HEIGHT ;++i){
            for (int l = 0; l < MAP_WIDTH ;++l) {
                fov_map_mons->setProperties(l, i, !(map_array[i * MAP_WIDTH + l].block_sight), !(map_array[i * MAP_WIDTH +
                        l].blocked));
                //map_array[row * MAP_WIDTH + l] = Tile(1,1);
            }

        }
        fov_recompute = true;
        player.stats.hp = 30;
        player.selfchar = '@';
        player.combat_move = 8;
        game_state = playing;
        set_black();
        alreadydead = 0;
        //    fov_recompute = true;
        render_all();
            TCODConsole::flush(); // this updates the screen
            return quit;
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

    if (eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_UP){
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

    else if (eve == TCOD_EVENT_KEY_PRESS && keyr.vk == TCODK_RIGHT){
        --bloodycount; 
        --duh.bloody; 
        player_move_attack(1, 0);
    }

    else {
        m_x = 0;
        m_y = 0;
        //std::cout << "playern.x: " << duh.x << " playern.y: " << duh.y << std::endl;
        return no_turn;
    }
    
    // end KEY RIGHT cycle
    }
    std::cout << "player.x: " << duh.x << " player.y: " << duh.y << std::endl; 
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

void Message_Log(){
    if(msg_log_list.size() > 0){
        panel->setDefaultForeground(TCODColor::white);
        panel->setBackgroundFlag(TCOD_BKGND_SET);
        panel->print(34, 2, ">");
         while(msg_log_list.size() > MSG_HEIGHT){
            msg_log_list.erase(msg_log_list.begin(),msg_log_list.begin()+1);
        }
        //int i = msg_log_list.size() + 1
         int a =2;
         int first = 1;
        for(int i = (msg_log_list.size()-1); i >= 0 ; i--){
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
        panel->print(35, a, msg_log_list[i].message,TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,
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
            panel->print(35, a+1, msg_log_list[i].message,TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,
            TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, TCOD_COLCTRL_STOP,TCOD_COLCTRL_4,
            TCOD_COLCTRL_STOP,TCOD_COLCTRL_5, TCOD_COLCTRL_STOP );
        a++;
            }
        }

        for (int n = 0; n < 7; ++n){
            panel->setDefaultForeground(TCODColor::lighterGrey);
            panel->setDefaultBackground(TCODColor::black);
            panel->print(32, n+1, "%c", TCOD_CHAR_VLINE);
        //TCODConsole::root->print(win_x-1, n, "%c", TCOD_CHAR_VLINE);
        }
    for (int n = 0; n < 20; ++n){
        panel->setDefaultForeground(TCODColor::lighterGrey);
        panel->setDefaultBackground(TCODColor::black);
        panel->print(n+33, 0, "%c", TCOD_CHAR_HLINE);
        //TCODConsole::root->print(n, win_y-1, "%c", TCOD_CHAR_HLINE);
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
    }

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
   
    make_map(player);

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

    player.colorb = con->getCharBackground(player.x, player.y);
    //npc.colorb = con->getCharBackground(npc.x, npc.y);

    player.colorb = color_dark_ground;
    //npc.colorb = color_dark_ground;

    strcpy(player.name, "Playername");

    fov_recompute = true;

    TCODConsole::initRoot(win_x, win_y, "FoE", false);

    game_state = playing;
    player.combat_move = 8;
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

    uint32 timin1 = 0;
    uint32 timin2 = 0; 

    while (! TCODConsole::isWindowClosed()) {


        //player.stats.hp = 5;

        /* int seco;
        
        seco = TCODSystem::getElapsedSeconds();
        if (seco > 5 && !loped){
            _beginthread( threadm, 0, NULL );
            loped = true;
            std::cout << "LOOPED" << std::endl;

        } */

        jump:
        //std::cout << "MAIN LOOP" << std::endl;

        int player_action = 0;
        //TCODConsole::root->putChar( 10,10, 0x2500 );

        if (!alreadydead){
        con->clear();
        TCODConsole::root->clear();
        }
        fov_recompute = true;

        
        render_all();
      
        
        

        


        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->print(1, MAP_HEIGHT_AREA+2, "Use arrows to move");
        TCODConsole::root->print(1, win_y-3, "Press 'q' to quit");
        
        TCODConsole::root->setAlignment(TCOD_RIGHT);
        if (player.stats.hp < 7) TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
        else TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
        TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
            TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+2, "HP: %c%d%c/%d",TCOD_COLCTRL_1, player.stats.hp,TCOD_COLCTRL_STOP , player.stats.max_hp);
            if (player.combat_move < 4) TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
            else TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
            TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+3, "Movement Points: %c%d%c",TCOD_COLCTRL_1,
                    player.combat_move,TCOD_COLCTRL_STOP);
        //TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+5, "Press 'p' to punch walls");
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+6, "Press 'r' to regenerate layout/revive player");
        TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+8, "Press 'd' for DEBUG");
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::yellow,TCODColor::black);
        if (debug) TCODConsole::root->print(win_x-2, MAP_HEIGHT_AREA+9, "%cMonster count%c: %d",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, killall);

        TCODConsole::root->print(win_x-1, 0, "Mode-N");

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

        
        
       
        I_am_moused2();

        
        //TCODConsole::root->putChar( 10,10, 0x2500 );
        TCODConsole::flush(); // this updates the screen

        
        for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->clear(); // player array, clar previous

        //int pla_x = player.x;
        //int pla_y = player.y;

        bool combat_mode = false;

        bool in_sight;

        

        if (!no_combat){ // debug flag

        for (unsigned int i = 0; i<monvector.size(); ++i) {
            in_sight = fov_map->isInFov(monvector[i].x,monvector[i].y);
            if(in_sight && monvector[i].alive == true){ 
                combat_mode = true; // trigger combat mode, if monster is sighted
            }    
        } // activates combat mode as soon a monster is in sight, deactivates on subsequent loops

        

        //player.combat_move = 8; // 1 cost for movement, 4 for attack
        while (combat_mode){

            for (unsigned int i = 0; i<monvector.size(); ++i) { 
                monvector[i].initiative = -1;
            }    

            if (alreadydead) break;
         
            std::vector<Monster> monsters; // vector used for initiative juggling

            con->clear();
            TCODConsole::root->clear(); 

            /* 
            TCODConsole::root->setAlignment(TCOD_CENTER);
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
            TCODConsole::root->print(win_x/2,win_y-5,"%cNEW COMBAT TURN, Press any key%c",
                    TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
            */        
               
            fov_recompute = true;
            render_all();
            TCODConsole::flush(); // this updates the screen
                
            //TCODConsole::waitForKeypress(true);

            bool break_combat = true;
            TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why
            for (unsigned int i = 0; i<monvector.size(); ++i) {
                in_sight = fov_map->isInFov(monvector[i].x,monvector[i].y);
                if(in_sight && monvector[i].alive == true){
                    monvector[i].c_mode = true;
                    monvector[i].pl_x = player.x; // if player in sight, store player pos
                    monvector[i].pl_y = player.y;
                    monvector[i].chase = 1;
                    monvector[i].bored = 400;
                    monvector[i].boren = false;
                    monvector[i].stuck = false;

                    break_combat = false; // set flag, so that if this cycle never entered, combat is interrupted

                    int roll = 0;
                    roll = wtf->getInt(1, 10, 0);
                    monvector[i].initiative = monvector[i].speed + roll;
                    monvector[i].temp_init = monvector[i].initiative;

                    Monster tempm;
                    tempm.initiative = &monvector[i].initiative;
                    tempm.speed = &monvector[i].speed;
                    monsters.push_back(tempm);

                    msg_log msg1;
                    sprintf(msg1.message, "%c>%c%s initiative: %c1d10%c(%d) + SPD(%d) Total: %d.",
                            TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,
                        monvector[i].name, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, roll, 
                        *tempm.speed, monvector[i].initiative);        
                    //    monvector[i].name, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, roll, 
                    //    monvector[i].speed, *tempm.initiative);
                    msg1.color1 = TCODColor::yellow;
                    msg_log_list.push_back(msg1);
                }
            }
            if (break_combat) break; // break combat mode if monsters all dead or out of FoV

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
            r_panel->print((win_x-MAP_WIDTH_AREA)-1, 3, "Initiative list");

            unsigned int player_own = player.initiative;

            // Create initiative UI list
            r_panel->setDefaultForeground(TCODColor::white);
            TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightAmber,TCODColor::black);
            for (unsigned int i = 0; i<monsters.size(); ++i) {
                if (i == (player_own - 1)){
                    r_panel->print((win_x-MAP_WIDTH_AREA)-2, 5+(i), "[%c%d%c] Player <", TCOD_COLCTRL_1, 
                            player.temp_init, TCOD_COLCTRL_STOP);
                } else {
                    for (unsigned int b = 0; b<monvector.size(); ++b) {
                        unsigned int monster_ini =  monvector[b].initiative;
                        if ((i+1) == monster_ini){
                            r_panel->print((win_x-MAP_WIDTH_AREA)-2, 5+ (i), "[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                    monvector[b].name, TCOD_COLCTRL_STOP);
                        }
                    }
                }
            }  

            r_panel->setBackgroundFlag(TCOD_BKGND_SET);
            TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightYellow, TCODColor::darkestRed);
            TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::lighterGreen, TCODColor::darkestRed);
            TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::darkGreen, TCODColor::darkestRed);
            TCODConsole::setColorControl(TCOD_COLCTRL_4, TCODColor::darkGreen, TCODColor::darkestRed);
            r_panel->print((win_x-MAP_WIDTH_AREA)-2, 16, "%cAF%c%cAF%c%cA*%c%cA*%c", TCOD_COLCTRL_1, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);
            r_panel->print((win_x-MAP_WIDTH_AREA)-2, 17, "%cDF%c%cD*%c%cDA%c%cDA%c", TCOD_COLCTRL_1, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);
            TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lighterRed, TCODColor::darkestRed);
            TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::lighterGreen, TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::lightYellow, TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_4, TCODColor::darkGreen, TCODColor::darkestRed);
            r_panel->print((win_x-MAP_WIDTH_AREA)-2, 18, "%cA*%c%c**%c%c**%c%cA*%c", TCOD_COLCTRL_1, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);
            r_panel->print((win_x-MAP_WIDTH_AREA)-2, 19, "%cDF%c%cDB%c%c**%c%cDA%c", TCOD_COLCTRL_1, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);
            TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::red, TCODColor::darkestRed);
            TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::lighterGreen, TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_4, TCODColor::lightYellow, TCODColor::darkestRed);
            r_panel->print((win_x-MAP_WIDTH_AREA)-2, 20, "%cA2%c%cA1%c%c**%c%c**%c", TCOD_COLCTRL_1, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);
            r_panel->print((win_x-MAP_WIDTH_AREA)-2, 21, "%cD*%c%cD*%c%cDB%c%c**%c", TCOD_COLCTRL_1, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);
            TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::red, TCODColor::darkestRed);
            TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::darkestRed);
            TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::red, TCODColor::darkestRed);
            TCODConsole::setColorControl(TCOD_COLCTRL_4, TCODColor::lighterGreen, TCODColor::darkestRed);
            r_panel->print((win_x-MAP_WIDTH_AREA)-2, 22, "%cA3%c%cA2%c%cA1%c%c**%c", TCOD_COLCTRL_1, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);
            r_panel->print((win_x-MAP_WIDTH_AREA)-2, 23, "%cD*%c%cD*%c%cD*%c%cDB%c", TCOD_COLCTRL_1, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, 
                    TCOD_COLCTRL_STOP,TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);


            TCODConsole::root->setAlignment(TCOD_CENTER);
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::black,TCODColor::white);
            TCODConsole::root->print(win_x/2,win_y-6,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",TCOD_COLCTRL_2,TCOD_CHAR_NW,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_NE,TCOD_COLCTRL_STOP);
            TCODConsole::root->print(win_x/2,win_y-5,"%c%c %c%cSTART COMBAT TURN, Press any key%c%c %c%c"
                    ,TCOD_COLCTRL_2,TCOD_CHAR_VLINE,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_CHAR_VLINE,TCOD_COLCTRL_STOP);
            TCODConsole::root->print(win_x/2,win_y-4,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",TCOD_COLCTRL_2,TCOD_CHAR_SW,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_SE,TCOD_COLCTRL_STOP);
            
            fov_recompute = true;
            render_all();
            TCODConsole::blit(r_panel,0,0,0,0,TCODConsole::root,MAP_WIDTH_AREA, 0);
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

                    // PLAYER BLOCK
                    // PLAYER BLOCK
                    while (player.combat_move >= 1){

                        player_action = handle_combat(player);

                        if (player_action == quit){
                            Sleep(100);
                            combat_mode = false;
                            goto jump;
                            break;
                        } // exits combat? 
                        if (player_action == quit2){
                        quit_now = true;
                        goto end;
                        } // exits program

                        if ((m_x != 0 || m_y != 0) && player.combat_move > 0){
                            player.move(m_x, m_y, monvector);
                            --player.combat_move;
                        } // player action

                        con->clear();
                        TCODConsole::root->clear();

                        fov_recompute = true;
                        render_all();

                        
                        for (unsigned int n = 0; n<monvector.size(); ++n) {
                            in_sight = fov_map->isInFov(monvector[n].x,monvector[n].y);
                            if(in_sight && monvector[n].alive == true){
                                monvector[n].c_mode = true;
                                monvector[n].pl_x = player.x; // if player in sight, store player pos
                                monvector[n].pl_y = player.y;
                                monvector[n].chase = 1;
                                monvector[n].bored = 400;
                                monvector[n].boren = false;
                                monvector[n].stuck = false;
                            }
                        }

                        // SIDEBAR UI
                        TCODConsole::root->setAlignment(TCOD_RIGHT);
                        TCODConsole::root->print(win_x-1, 0, "Mode-C");

                        TCODConsole::root->print(win_x-1, 3, "Initiative list");

                        // Initiative UI list in player turn
                        TCODConsole::root->setDefaultForeground(TCODColor::white);
                        TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
                        TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightAmber,TCODColor::black);
                        TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::darkRed, TCODColor::black);
                        for (unsigned int n = 0; n<monsters.size(); ++n) {
                            if (n == (player_own - 1)){
                                TCODConsole::root->print(win_x-2, 5+(n), "[%c%d%c] Player <", TCOD_COLCTRL_1, 
                                    player.temp_init, TCOD_COLCTRL_STOP);
                            } else {
                                for (unsigned int b = 0; b<monvector.size(); ++b) {
                                    unsigned int monster_ini = monvector[b].initiative;
                                    if ((n+1) == monster_ini){
                                        if(monster_ini < player_own){
                                            TCODConsole::root->print(win_x-2, 5+(n), "%c[%d] %s%c", TCOD_COLCTRL_3,
                                            monvector[b].temp_init, monvector[b].name, TCOD_COLCTRL_STOP);
                                            
                                        } else {    
                                        TCODConsole::root->print(win_x-2, 5+(n), "[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                            monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                            monvector[b].name, TCOD_COLCTRL_STOP);
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
                        TCODConsole::blit(panel,0,0,0,0,TCODConsole::root,0,MAP_HEIGHT_AREA+2);

                        TCODConsole::flush(); // this updates the screen
                    }
                    // PLAYER BLOCK
                    // PLAYER BLOCK

                    //con->clear();
                    //TCODConsole::root->clear(); 

             
            TCODConsole::root->setAlignment(TCOD_CENTER);
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
            TCODConsole::root->print(win_x/2,win_y-21,"%cpress any key to END Player's turn%c",
                    TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
                    
               
            fov_recompute = true;
            render_all();
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
                            TCODConsole::root->print(win_x/2,win_y-4,"%cMONSTER TURN%c",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);

                            TCODConsole::root->clear();
                            fov_recompute = true;
                            render_all();
                            // SIDEBAR UI
                                TCODConsole::root->setAlignment(TCOD_RIGHT);
                                TCODConsole::root->print(win_x-1, 0, "Mode-C");

                                TCODConsole::root->print(win_x-1, 3, "Initiative list");

                        // Initiative UI list in monster turn
                        TCODConsole::root->setDefaultForeground(TCODColor::white);
                        TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
                        TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightAmber,TCODColor::black);
                        TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::darkRed, TCODColor::black);
                        for (unsigned int n = 0; n<monsters.size(); ++n) {
                            if (n == (player_own - 1)){
                                if(player_own < (i+1)){
                                    TCODConsole::root->print(win_x-2, 5+(n), "%c[%d] Player%c", TCOD_COLCTRL_3, 
                                    player.temp_init, TCOD_COLCTRL_STOP);
                                } else {
                                TCODConsole::root->print(win_x-2, 5+(n), "[%c%d%c] Player", TCOD_COLCTRL_1, 
                                    player.temp_init, TCOD_COLCTRL_STOP);
                                }
                            } else {
                                for (unsigned int b = 0; b<monvector.size(); ++b) {
                                    unsigned int monster_ini = monvector[b].initiative;
                                    if ((n+1) == monster_ini){
                                        if(monster_ini < (i+1)){
                                            TCODConsole::root->print(win_x-2, 5+(n), "%c[%d] %s%c", TCOD_COLCTRL_3,
                                            monvector[b].temp_init, monvector[b].name, TCOD_COLCTRL_STOP);
                                            
                                        } else if(monster_ini == (i+1)){
                                            TCODConsole::root->print(win_x-2, 5+(n), "[%c%d%c] %c%s%c <", TCOD_COLCTRL_1,
                                            monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                            monvector[b].name, TCOD_COLCTRL_STOP);
                                        } else {    
                                        TCODConsole::root->print(win_x-2, 5+(n), "[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                            monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                            monvector[b].name, TCOD_COLCTRL_STOP);
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
                                if (monvector[b].myai->take_turn(monvector[b], player, monvector[b].pl_x,
                                        monvector[b].pl_y,seehere) ) render_all();

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
                                
                                render_all();
                                TCODConsole::root->print(win_x/2,win_y-4,"%cMONSTER TURN%c",TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);

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

        

        // maybe needed for death
        player.move(0, 0, monvector);
        //    fov_recompute = true;
        //render_all();
        //    TCODConsole::flush(); // this updates the screen

        for (unsigned int i = 0; i<monvector.size(); ++i) { 
                    if (monvector[i].alive){
                        monvector[i].c_mode = false;
                    }
        } // deactivates combat mode for all monsters, so they are properly re-flagged on next loop 
        
        TCODConsole::root->print(win_x-1, 0, "Mode-N");
        player_action = handle_keys(player);

        

        if (player_action == quit) break; // break main while loop to exit program

        

        if (game_state == playing && player_action != no_turn){
            for (unsigned int i = 0; i<monvector.size(); ++i) { 
                if (monvector[i].alive){ // take turn for every monster alive

                    bool in_sight;
                    fov_recompute = true;
                    //render_all(); // maybe helps for fov

                    // take turn for monster in sight OR monster chasing and not bored
                    in_sight = fov_map->isInFov(monvector[i].x,monvector[i].y);
                    if (in_sight || (monvector[i].chase == 1 && !monvector[i].boren)){

                        // compute mons-fov to awake other monsters in view
                        fov_map_mons->computeFov(monvector[i].x,monvector[i].y, MON_RADIUS, FOV_LIGHT_WALLS,FOV_ALGO);

                        // if at destination OR stuck -> pick a random destination
                        if ( (monvector[i].pl_x == monvector[i].x && monvector[i].pl_y == monvector[i].y) ||
                            monvector[i].stuck){
                            TCODRandom * wtf = TCODRandom::getInstance(); // initializer for random, no idea why
                            int vagab_x = 0;
                            int vagab_y = 0;
                            vagab_x = wtf->getInt(monvector[i].x - 10, monvector[i].x + 10, 0);
                            if (vagab_x < 0) vagab_x = 0;
                            if (vagab_x > MAP_WIDTH) vagab_x = MAP_WIDTH;
                            vagab_y = wtf->getInt(monvector[i].y - 10, monvector[i].y + 10, 0);
                            if (vagab_y < 0) vagab_y = 0;
                            if (vagab_y > MAP_HEIGHT) vagab_y = MAP_HEIGHT;
                            monvector[i].pl_x = vagab_x; // pick a random spot in FoV
                            monvector[i].pl_y = vagab_y;
                        } 

                        --monvector[i].bored;

                        // aims for player if player is seen
                        if (in_sight){
                            monvector[i].pl_x = player.x; // if player in sight, store player pos
                            monvector[i].pl_y = player.y;
                            monvector[i].chase = 1;
                            monvector[i].boren = false;
                            monvector[i].bored = 400;
                            monvector[i].stuck = false;
                            //if (monvector[i].distance_to(player.x, player.y) < 2){
                            //    monvector[i].pl_x = player.x; // if player in sight, store player pos
                            //    monvector[i].pl_y = player.y;
                                //monvector[i].chase = 1;
                            //} // this was needed so monsters one step away don't step ON the player, since
                            // the monster moves after the player moved, so both move and overlap
                        }
                     
                        if (monvector[i].myai->take_turn(monvector[i], player, monvector[i].pl_x,
                                monvector[i].pl_y,in_sight)) render_all();
                   
                        // awake monsters seen
                        for (unsigned int l = 0; l<monvector.size(); ++l) {
                            if(fov_map_mons->isInFov(monvector[l].x,monvector[l].y) && monvector[l].chase == 0) {
                                monvector[l].pl_x = monvector[i].pl_x;
                                monvector[l].pl_y = monvector[i].pl_y;
                                monvector[l].chase = 1;
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
                            monvector[i].chase = 0;
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
                if((monvector[i].color == orc) && (monvector[i].combat_move < 6)) ++monvector[i].combat_move;
                else if (monvector[i].color == troll && monvector[i].combat_move < 10) ++monvector[i].combat_move;
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
