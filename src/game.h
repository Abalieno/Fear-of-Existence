#ifndef _GAME_H_
#define _GAME_H_

#include "liventities.h"

struct tilevalues{
 bool U8; // sprite or ASCII, false = ASCII

 int t8_player; //global player sprite

// sprite 8 made into 16
const int t28_door = 519; // door
const int t28_player = 520; // player
const int t28_wall = 521; // wall
const int t28_nwall = 503; // null.wall
const int t28_floor1 = 522; // floor1
const int t28_floor2 = 523; // floor2
const int t28_floor3 = 523; // floor2
const int t28_floor4 = 523; // floor2
const int t28_orc = 524; // orc
const int t28_troll = 525; // troll
const int t28_corpse = 526; // corpse

// 16 demake
const int t16_door = 510; // door
const int t16_doorv = 511; // doorv
const int t16_player = 502; // player
const int t16_wall = 512; // wall
const int t16_vwall = 514; // v.wall
const int t16_trwall = 517; // t-r.wall
const int t16_tlwall = 513; // t-l.wall
const int t16_bwall = 515; // B.wall
const int t16_ibwall = 518; // iB.wall
const int t16_floor1 = 509; // floor1
const int t16_floor2 = 509; // floor2
const int t16_orc = 505; // orc
const int t16_troll = 504; // troll
const int t16_corpse = 506; // corpse

// 16 universal
 int u16_door ; // door
 int u16_doorv ; // doorv
 int u16_player ; // player
 int u16_wall ; // wall
 int u16_vwall ; // v.wall
 int u16_trwall ; // t-r.wall
 int u16_tlwall ; // t-l.wall
 int u16_bwall ; // B.wall
 int u16_ibwall ; // B.wall
 int u16_floor1 ; // floor1
 int u16_floor2 ; // floor2
 int u16_orc ; // orc
 int u16_troll ; // troll
 int u16_corpse ; // corpse

// 8 universal
 int u8_door ; // door
 int u8_doorv ; // doorv
 int u8_player ; // player
 int u8_wall ; // wall
 int u8_vwall ; // v.wall
 int u8_trwall ; // t-r.wall
 int u8_tlwall ; // t-l.wall
 int u8_bwall ; // B.wall
 int u8_ibwall ; // B.wall
 int u8_floor1 ; // floor1
 int u8_floor2 ; // floor2
 int u8_floor3 ;
 int u8_floor4 ;
 int u8_orc ; // orc
 int u8_troll; // troll
 int u8_corpse ; // corpse
 int u8_hwall;

 TCODColor u8_wallCa; // black
 TCODColor u8_wallCb;
 TCODColor u8_wallCc; // black
 TCODColor u8_wallCd;
 TCODColor u8_floorCa;
 TCODColor u8_floorCb;
};

// bool used for background color presence
struct msg_log { char message [94]; TCODColor color1; TCODColor color2; TCODColor color3; TCODColor color4; TCODColor
    color5; TCODColor color6; TCODColor color7; TCODColor color8; TCODColor color9; TCODColor color10; bool c1;
    bool c2; bool c3; bool c4; bool c5; bool c6; bool c7; bool c8; bool c9; bool c10;
    TCODColor bcolor1; TCODColor bcolor2; TCODColor bcolor3; TCODColor bcolor4; TCODColor bcolor5; TCODColor bcolor6;
    TCODColor bcolor7; TCODColor bcolor8; TCODColor bcolor9; TCODColor bcolor10;};

struct msg_log_c { bool is_context; int ltype;}; 

//static std::vector<msg_log> msg_log_list;
//static std::vector<msg_log_c> msg_log_context;

class Tile {

public:

    bool blocked;
    bool block_sight;
    int feature;
    int bloodyt; // amount of blood on Tile
    bool explored;

    Tile() { blocked = false; block_sight = false; bloodyt = 0; }

    Tile(int isblocked, int isblock_sight){
        bloodyt = 0;
        blocked = isblocked;
        block_sight = isblock_sight;
        if (blocked) block_sight = true;
        explored = false;
        feature = 0;
    }
};

struct UIhook{
    int ID;
    int x;
    int y;
    int w;
    int h;
};    

struct global_state{
    int bigg;
    int bigg2;
    int bigg3;
    int mapmode;

    bool first; // used to set player combat states
    bool second;
    bool third;
    bool fourth;
    bool mode_move;
    bool mode_attack;
    bool mode_pass;

    int off_xx; // offsets to blit during attacks
    int off_yy;
    int offbig_x;
    int offbig_y;

    bool fov_recompute;

    TCODColor color_dark_ground;

    TCODConsole *con;
    TCODConsole *mesg; // message pop-up drawn on top of "con"

    bool no_combat; // disable combat mode
    bool debug; // shows all monsters on map, when set

    TCODMap * fov_map;

    // hardcoded sizes 
    TCODMap * fov_map_mons = new TCODMap(220,140);

    TCODMap * fov_map_mons_path0; 
    TCODMap * fov_map_mons_path1 = new TCODMap(220,140);
    

    std::vector<Generic_object> wd_object;

    std::vector<UIhook> UI_hook;
};    

class Game
{
public:

    global_state gstate;
    tilevalues tileval;

  Game();
  ~Game();

  bool setup();
  bool main_loop();

  //Map* map;
  //Overmap* overmap;
  Object_player* player;
  

private:
  //Window *w_map;
  //cuss::interface i_main;
};



#endif
