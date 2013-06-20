#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "libtcod.hpp"
#include <windows.h> // for Sleep()

#include <process.h>

const int   win_x   =   95; // window width in cells
const int   win_y   =   63; // window height in cells
const int   LIMIT_FPS = 20;

const int MAP_WIDTH = 80;
const int MAP_HEIGHT = 45;

const int quit = 1;
const int move_up = 5;
const int move_down = 10;
const int action = 15;
const int quit2 = 99; // combat mode exit game
const int playing = 1;
const int no_turn = 2;
const int dead = 99;

int stance_pos = 1;


//parameters for dungeon generator
int ROOM_MAX_SIZE = 10;
int ROOM_MIN_SIZE = 6;
int MAX_ROOMS = 30;
int MAX_ROOM_MONSTERS = 3;
unsigned int MAX_TOTAL_MONSTERS = 20;

TCOD_fov_algorithm_t FOV_ALGO = FOV_BASIC; //default FOV algorithm
bool FOV_LIGHT_WALLS = true;
int TORCH_RADIUS = 10;
int MON_RADIUS = 8; // used for monster FoV

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
TCODColor monsterdead(TCODColor::darkGreen);

bool fov_recompute;

bool debug = false; // shows all monsters on map, when set
bool no_combat = false; // disable combat mode


TCODConsole *con = new TCODConsole(MAP_WIDTH, MAP_HEIGHT);
TCODConsole *mesg = new TCODConsole(33, 3);  // message pop-up drawn on top of "con"
TCODConsole *load = new TCODConsole(win_x, win_y);  // load screen

TCODConsole *panel = new TCODConsole(win_x, (win_y - MAP_HEIGHT));  // combat UI panel
// 30, 46 message log
int BAR_WIDTH = 20;
int MSG_X = 30; // BAR_WIDTH + 2;
int MSG_WIDTH = 63; // SCREEN_WIDTH - BAR_WIDTH - 2
int MSG_HEIGHT = 12;// PANEL_HEIGHT - 1

TCODMap * fov_map = new TCODMap(MAP_WIDTH,MAP_HEIGHT);
TCODMap * fov_map_mons = new TCODMap(MAP_WIDTH,MAP_HEIGHT);

struct msg_log { char message [50]; TCODColor color1; TCODColor color2; TCODColor color3; TCODColor color4; TCODColor
    color5;};

std::vector<msg_log> msg_log_list; 

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

class Fighter {
public:
    int speed;
    int max_hp;
    int hp;
    int defense;
    int power;

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
        con->setDefaultForeground(color);
        if (!uh) colorb = con->getCharBackground(x, y);
        con->setDefaultBackground(colorb);
        if (fov_map->isInFov(x,y) || debug == 1)
        con->putChar(x, y, selfchar, TCOD_BKGND_SET);
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
        if (fov_map->isInFov(x,y))
        con->putChar(x, y, ' ', TCOD_BKGND_NONE);
    }

    ~Object_monster(){} // ?
};

class AI {
public:
     virtual bool take_turn(Object_monster &monster, Object_player &player, int p_x, int p_y, bool myfov) {return false;} 

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

    int combat_move;
    int speed;
    int initiative;
    int temp_init; // total initiative value for messages/list

    Object_player(int a, int b, char pchar, TCODColor oc, TCODColor oc2, int health, Fighter loc_fighter) : stats(loc_fighter) {
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
        if (!uh) colorb = con->getCharBackground(x, y);
        con->setDefaultBackground(colorb);
        if (fov_map->isInFov(x,y))
        con->putChar(x, y, selfchar, TCOD_BKGND_SET);
    }

    void clear() {
        if (fov_map->isInFov(x,y))
        con->putChar(x, y, ' ', TCOD_BKGND_NONE);
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
                player.colorb = TCODColor::red;
                monster.colorb = TCODColor::black;
                player.selfchar = '/';
                player.draw(1);
                monster.draw(1);
                mesg->print(1, 1, "Hit!");
                if(!(player.y > MAP_HEIGHT-8 )) TCODConsole::blit(mesg,0,0,33,3,con,1,MAP_HEIGHT-4);
                else TCODConsole::blit(mesg,0,0,33,3,con,MAP_WIDTH-37,1);
                TCODConsole::blit(con,0,0,0,0,TCODConsole::root,0,0);
                TCODConsole::flush();

                

                Sleep(200); // shitty way for attack "animation", uses windows.h
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
        
    } else {
        damage = player.stats.power - monster.stats.defense;

        if (damage > 0){
            std::cout << player.name << " attacks " << monster.name << " for " << damage << " hit points." << std::endl;
            monster.stats.take_damage(damage);
        } else std::cout << player.name << " attacks " << monster.name << " but it has no effect!" << std::endl;       
    }

}


Fighter fighter_component(30, 2, 5, 8); // hp, defense, power
Object_player playera(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5, fighter_component);
Object_player playerb(win_x/2, win_y/2, '@', TCODColor::white, TCODColor::black, 5, fighter_component);

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
        }  
        monvector.push_back(monster);
        }
     }
    
    std::cout << " Monster array: " << myvector.size() << std::endl;
   // }
}

int killall = 0; // monster count

int init = 0;

void make_map(Object_player &duh){
           
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

TCOD_key_t keyz;
    TCOD_mouse_t mousez;
    TCOD_event_t ev;

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

void I_am_moused(){
    //TCOD_key_t key;
    //TCOD_mouse_t mouse;
    //TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
    mousez = TCODMouse::getStatus();
    int x = mousez.cx;
    int y = mousez.cy;

    char *whatis;

    //std::cout << "MOUSE " << x << " " << y << std::endl;

    bool found = false;
    TCODColor col_obj; // color of object

    if (x == player.x && y == player.y){
        panel->setDefaultForeground(TCODColor::white);
        panel->setAlignment(TCOD_LEFT);
        panel->print(1, 5, "Mouse on [Player] at [%d.%d]", x, y);
        panel->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
    } else {
        for (unsigned int n = 0; n<monvector.size(); ++n) {
            if( (x == monvector[n].x && y == monvector[n].y) && !(monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y)){
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
        for (unsigned int n = 0; n<monvector.size(); ++n) {
            if( (x == monvector[n].x && y == monvector[n].y && monvector[n].alive) 
                    && fov_map->isInFov(monvector[n].x,monvector[n].y)){
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
        if (!found){
            panel->setDefaultForeground(TCODColor::white);
            panel->setAlignment(TCOD_LEFT);
            panel->print(1, 5, "Mouse on [Nothing] at [%d.%d]", x, y);
            panel->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
            found = false;
        }
    }
}

void I_am_moused2(){ // doubled because of main loop
    //TCOD_key_t key;
    //TCOD_mouse_t mouse;
    //TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
    mousez = TCODMouse::getStatus();
    int x = mousez.cx;
    int y = mousez.cy;

    char *whatis;

    if(mousez.lbutton && mousez.cx == 0 && mousez.cy == 0) stance_pos = 1;
    if(mousez.lbutton && mousez.cx == 1 && mousez.cy == 0) stance_pos = 2;
    if(mousez.lbutton && mousez.cx == 2 && mousez.cy == 0) stance_pos = 3;

    //std::cout << "MOUSE " << x << " " << y << std::endl;

    bool found = false;
    TCODColor col_obj; // color of object

    if (x == player.x && y == player.y){
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->print(1, win_y-5, "Mouse on [Player] at [%d.%d]", x, y);
        TCODConsole::root->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
    } else {
        for (unsigned int n = 0; n<monvector.size(); ++n) {
        if( (x == monvector[n].x && y == monvector[n].y) && !(monvector[n].alive)
                && fov_map->isInFov(monvector[n].x,monvector[n].y)){
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
    for (unsigned int n = 0; n<monvector.size(); ++n) {
        if( (x == monvector[n].x && y == monvector[n].y) && monvector[n].alive
                && fov_map->isInFov(monvector[n].x,monvector[n].y)){
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
    if (!found){
        TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->print(1, win_y-5, "Mouse on [Nothing] at [%d.%d]", x, y);
        TCODConsole::root->setDefaultBackground(TCODColor::black); // sets the rest of the screen as black
        found = false;
    }
    }
}

//def render_bar(x, y, total_width, name, value, maximum, bar_color, back_color):
void render_bar(int x, int y, int total_width, const char *name, 
        int value, int maximum, TCODColor bar_color, TCODColor back_color){

    int bar_width = int(float(value) / maximum * total_width);

    panel->setDefaultBackground(back_color);
    panel->rect(x, y, total_width, 1, false,TCOD_BKGND_SET);

    //std::cout << "BAR " << bar_width << std::endl;
    //std::cout << "value " << value << std::endl;
    //std::cout << "maximum " << maximum << std::endl;
    //std::cout << "total_width " << total_width << std::endl;
    panel->setDefaultBackground(bar_color);
    if (bar_width > 0) 
        panel->rect(x, y, bar_width, 1, false,TCOD_BKGND_SET);
 
    panel->setDefaultForeground(TCODColor::white);
    panel->setAlignment(TCOD_CENTER);
    panel->print((x + total_width) / 2, y, "%s: %d/%d",name , value, maximum);
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

void render_all (){

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

    
    for (unsigned int i = 0; i<monvector.size(); ++i) {
        if (monvector[i].selfchar == '%')
        monvector[i].draw(0); // first draws dead bodies

    }

    for (unsigned int i = 0; i<monvector.size(); ++i) {
        if (monvector[i].selfchar != '%')
        monvector[i].draw(0); // then draws monsters still alive

    }


    for (unsigned int i = 0; i<myvector.size(); ++i) myvector[i]->draw(0); // player vector

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
    con->setBackgroundFlag(TCOD_BKGND_SET);
    con->print(0, 0, "%c1%c%c2%c%c3%c",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,
            TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
    
    TCODConsole::blit(con,0,0,0,0,TCODConsole::root,0,0);
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
            if (monvector[i].alive == true){target = i;
            is_it = true;}
        }
    }

    if (is_it && monvector[target].alive && player.combat_move >= 4){
        player.stats.attack(player, monvector[target], 0);
        if(!no_combat)player.combat_move -= 4; // decrease combat movement only if in combat mode
        if (monvector[target].stats.hp < 1){
                //monvector.erase (target); 
                --killall;
                bloodsplat(monvector[target]);
                bloodycount = 5;

                msg_log msg1;
                sprintf(msg1.message, "You've killed a %c%s%c!",TCOD_COLCTRL_1, monvector[target].name, TCOD_COLCTRL_STOP);
                msg1.color1 = monvector[target].color;
                msg_log_list.push_back(msg1);

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
                char tchar = monvector[target].selfchar;
                //TCODConsole::root->clear();
                mesg->setAlignment(TCOD_LEFT);
                mesg->setDefaultForeground(TCODColor::yellow);
                mesg->setDefaultBackground(TCODColor::black);
                monvector[target].colorb = TCODColor::red;
                monvector[target].selfchar = '-';
                monvector[target].draw(1);
                mesg->print(1, 1, "Hit!");

                msg_log msg1;
                sprintf(msg1.message, "You've hit a %c%s%c!",TCOD_COLCTRL_1, monvector[target].name, TCOD_COLCTRL_STOP);
                msg1.color1 = monvector[target].color;
                //if(msg_log_list.size() > 0) msg_log_list.pop_back();
                msg_log_list.push_back(msg1);
                

                if(!(player.y > MAP_HEIGHT-8 )) TCODConsole::blit(mesg,0,0,33,3,con,1,MAP_HEIGHT-4);
                else TCODConsole::blit(mesg,0,0,33,3,con,MAP_WIDTH-37,1);
                TCODConsole::blit(con,0,0,0,0,TCODConsole::root,0,0);
                TCODConsole::flush();
                Sleep(200); // shitty way for attack "animation", uses windows.h
                monvector[target].colorb = color_dark_ground;
                monvector[target].selfchar = tchar;
                monvector[target].draw(0);
                con->clear();
                fov_recompute = true;
                render_all();
                TCODConsole::flush();
                std::cout << "monster target hp: " << monvector[target].stats.hp << std::endl;
                m_x = 0;
        m_y = 0;
            }
        std::cout << "The " << monvector[target].name << " laughs of your attack!" << std::endl;
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

    if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'd' ){ if (debug) debug = false; else debug = true; Sleep (100);}

    if (eve == TCOD_EVENT_KEY_PRESS && keyr.c == 'r' ){

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
    std::cout << "player.x: " << duh.x << " player.y: " << duh.y << std::endl; 
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
        panel->print(34, 2, ">");
         while(msg_log_list.size() > 12){
            msg_log_list.erase(msg_log_list.begin(),msg_log_list.begin()+1);
        }
        //int i = msg_log_list.size() + 1
         int a =2;
         int first = 1;
        for(int i = (msg_log_list.size()-1); i >= 0 ; i--){
            if (first){
                TCODConsole::setColorControl(TCOD_COLCTRL_1,msg_log_list[i].color1,TCODColor::black);
                TCODConsole::setColorControl(TCOD_COLCTRL_2,msg_log_list[i].color2,TCODColor::black);
                TCODConsole::setColorControl(TCOD_COLCTRL_3,msg_log_list[i].color3,TCODColor::black);
                TCODConsole::setColorControl(TCOD_COLCTRL_4,msg_log_list[i].color4,TCODColor::black);
                TCODConsole::setColorControl(TCOD_COLCTRL_5,msg_log_list[i].color5,TCODColor::black);
        panel->print(35, a, msg_log_list[i].message,TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,
            TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, TCOD_COLCTRL_STOP,TCOD_COLCTRL_4,
            TCOD_COLCTRL_STOP,TCOD_COLCTRL_5, TCOD_COLCTRL_STOP );
        a++;
        first = 0;
            } else{
        TCODConsole::setColorControl(TCOD_COLCTRL_1,msg_log_list[i].color1,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_2,msg_log_list[i].color2,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_3,msg_log_list[i].color3,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_4,msg_log_list[i].color4,TCODColor::black);
        TCODConsole::setColorControl(TCOD_COLCTRL_5,msg_log_list[i].color5,TCODColor::black);
        panel->print(35, a+1, msg_log_list[i].message,TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,
            TCOD_COLCTRL_STOP,TCOD_COLCTRL_3, TCOD_COLCTRL_STOP,TCOD_COLCTRL_4,
            TCOD_COLCTRL_STOP,TCOD_COLCTRL_5, TCOD_COLCTRL_STOP );
        a++;
            }
        }

        panel->setDefaultForeground(TCODColor::grey);
        panel->print(32, 0, "o");
        panel->print(90, 0, "o");
        panel->print(32, 16, "o");
        panel->print(90, 16, "o");
        for (int n = 1; n < (90-32); n++) panel->print((32+n), 0, "%c",TCOD_CHAR_HLINE);
        for (int n = 1; n < (90-32); n++) panel->print((32+n), 16, "%c",TCOD_CHAR_HLINE);
        for (int n = 1; n < (16); n++) panel->print(32, n, "%c",TCOD_CHAR_VLINE);
        for (int n = 1; n < (16); n++) panel->print(90, n, "%c",TCOD_CHAR_VLINE);
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

    TCODConsole::setCustomFont("arial10x10.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
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
    bool quit_now = false;

    bool loped = false;

    TCODConsole::mapAsciiCodeToFont('s',4,0);

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
    
    while (! TCODConsole::isWindowClosed()) {

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
        TCODConsole::root->print(1, MAP_HEIGHT+1, "Use arrows to move");
        TCODConsole::root->print(1, win_y-3, "Press 'q' to quit");
        
        TCODConsole::root->setAlignment(TCOD_RIGHT);
        if (player.stats.hp < 7) TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
        else TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
        TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
            TCODConsole::root->print(win_x-2, MAP_HEIGHT+1, "HP: %c%d%c/%d",TCOD_COLCTRL_1, player.stats.hp,TCOD_COLCTRL_STOP , player.stats.max_hp);
            if (player.combat_move < 4) TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
            else TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
            TCODConsole::root->print(win_x-2, MAP_HEIGHT+2, "Movement Points: %c%d%c",TCOD_COLCTRL_1,
                    player.combat_move,TCOD_COLCTRL_STOP);
        //TCODConsole::root->setDefaultForeground(TCODColor::white);
        TCODConsole::root->print(win_x-2, MAP_HEIGHT+4, "Press 'p' to punch walls");
        TCODConsole::root->print(win_x-2, MAP_HEIGHT+5, "Press 'r' to regenerate layout/revive player");
        TCODConsole::root->print(win_x-2, MAP_HEIGHT+7, "Press 'd' for DEBUG");
        TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::yellow,TCODColor::black);
        if (debug) TCODConsole::root->print(win_x-2, MAP_HEIGHT+8, "%cMonster count%c: %d",TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, killall);
        
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

            if (alreadydead) break;
         
            std::vector<Monster> monsters; // vector used for initiative juggling

            con->clear();
            TCODConsole::root->clear(); 

            TCODConsole::root->setAlignment(TCOD_CENTER);
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::black,TCODColor::white);
            TCODConsole::root->print(win_x/2,win_y-5,"%cNEW COMBAT TURN, Press any key%c",
                    TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
               
            fov_recompute = true;
            render_all();
            TCODConsole::flush(); // this updates the screen
            TCOD_key_t key = TCODConsole::waitForKeypress(true);

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
                    sprintf(msg1.message, "%s initiative: %c1d10%c(%d) + SPD(%d) Total: %d.",
                        monvector[i].name, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, roll, 
                        monvector[i].speed, *tempm.initiative);
                    msg1.color1 = TCODColor::yellow;
                    msg_log_list.push_back(msg1);
                }
            }
            if (break_combat) break; // break combat mode if monsters all dead or out of FoV

            con->clear();
            TCODConsole::root->clear();

            int myroll = 0;
            myroll = wtf->getInt(1, 10, 0);
            player.initiative = player.speed + myroll;
            player.temp_init = player.initiative;

            Monster tempm;
            tempm.initiative = &player.initiative;
            tempm.speed = &player.speed;
            monsters.push_back(tempm);

            msg_log msg1;
            sprintf(msg1.message, "Player initiative: %c1d10%c(%d) + SPD(%d) Total: %d.",
                TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, myroll, 
                player.speed, player.initiative);
            msg1.color1 = TCODColor::yellow;
            msg_log_list.push_back(msg1);
           
            // SORTING INITIATIVE
            std::sort(monsters.begin(), monsters.end(), compare);
            for (unsigned int i = 0; i<monsters.size(); ++i) {
                *(monsters[i].initiative) = i+1;
            }  

            TCODConsole::root->setAlignment(TCOD_RIGHT);
            TCODConsole::root->print(win_x-1, 3, "Initiative list");

            unsigned int player_own = player.initiative;

            // Create initiative UI list
            TCODConsole::root->setDefaultForeground(TCODColor::white);
            TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::lightAmber,TCODColor::black);
            for (unsigned int i = 0; i<monsters.size(); ++i) {
                if (i == (player_own - 1)){
                    TCODConsole::root->print(win_x-2, 5+(i), "[%c%d%c] Player <", TCOD_COLCTRL_1, 
                            player.temp_init, TCOD_COLCTRL_STOP);
                } else {
                    for (unsigned int b = 0; b<monvector.size(); ++b) {
                        unsigned int monster_ini =  monvector[b].initiative;
                        if ((i+1) == monster_ini){
                            TCODConsole::root->print(win_x-2, 5+ (i), "[%c%d%c] %c%s%c", TCOD_COLCTRL_1,
                                    monvector[b].temp_init, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, 
                                    monvector[b].name, TCOD_COLCTRL_STOP);
                        }
                    }
                }
            }    

            TCODConsole::root->setAlignment(TCOD_CENTER);
            TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::white,TCODColor::black);
            TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::black,TCODColor::white);
            TCODConsole::root->print(win_x/2,win_y-6,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",TCOD_COLCTRL_2,TCOD_CHAR_NW,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_NE,TCOD_COLCTRL_STOP);
            TCODConsole::root->print(win_x/2,win_y-5,"%c%c %c%cSTART COMBAT TURN, Press any key%c%c %c%c"
                    ,TCOD_COLCTRL_2,TCOD_CHAR_VLINE,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2,TCOD_CHAR_VLINE,TCOD_COLCTRL_STOP);
            TCODConsole::root->print(win_x/2,win_y-4,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",TCOD_COLCTRL_2,TCOD_CHAR_SW,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_HLINE,TCOD_CHAR_SE,TCOD_COLCTRL_STOP);
            
            fov_recompute = true;
            render_all();
            TCODConsole::flush(); // this updates the screen
            TCOD_key_t key2 = TCODConsole::waitForKeypress(true); // to start combat

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
                        TCODConsole::blit(panel,0,0,0,0,TCODConsole::root,0,MAP_HEIGHT);

                        TCODConsole::flush(); // this updates the screen
                    }
                    // PLAYER BLOCK
                    // PLAYER BLOCK

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
                msg2.color1 = TCODColor::red;
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
                    render_all(); // maybe helps for fov

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

        std::cout << "END MONSTER TURN" << std::endl;

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
