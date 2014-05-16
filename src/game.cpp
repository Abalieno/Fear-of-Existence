#include "libtcod.hpp"
#include "game.h"

Game::Game()
{
  }

Game::~Game()
{
  }

bool Game::setup()
{
    gstate.bigg = 0;
    gstate.bigg2 = 0;
    gstate.bigg3 = 0;

    gstate.off_xx = 0; // offsets to blit during attacks
    gstate.off_yy = 0;
    gstate.offbig_x = 0;
    gstate.offbig_y = 0;

    gstate.color_dark_ground.r = 15;
    gstate.color_dark_ground.g = 15;
    gstate.color_dark_ground.b = 15;

    gstate.con = new TCODConsole(128, 91);
    //gstate.con = new TCODConsole(256, 182); // TO DO only draw the blitted part, split bigg and small con
    gstate.mesg = new TCODConsole(33, 3);
    gstate.no_combat = false;
    gstate.debug = false;
    gstate.fov_map = new TCODMap(220, 140);


    tileval.U8 = false; // sprite or ASCII, false = ASCII

 tileval.t8_player = 0; //global player sprite




// 16 universal
tileval.u16_door = 0; // door
tileval.u16_doorv = 0; // doorv
tileval.u16_player = 0; // player
 tileval.u16_wall = 0; // wall
tileval.u16_vwall = 0; // v.wall
tileval.u16_trwall = 0; // t-r.wall
tileval.u16_tlwall = 0; // t-l.wall
 tileval.u16_bwall = 0; // B.wall
 tileval.u16_ibwall = 0; // B.wall
 tileval.u16_floor1 = 0; // floor1
 tileval.u16_floor2 = 0; // floor2
tileval.u16_orc = 0; // orc
 tileval.u16_troll = 0; // troll
 tileval.u16_corpse = 0; // corpse

// 8 universal
 tileval.u8_door = 0; // door
tileval.u8_doorv = 0; // doorv
 tileval.u8_player = 0; // player
 tileval.u8_wall = 0; // wall
 tileval.u8_vwall = 0; // v.wall
 tileval.u8_trwall = 0; // t-r.wall
 tileval.u8_tlwall = 0; // t-l.wall
 tileval.u8_bwall = 0; // B.wall
 tileval.u8_ibwall = 0; // B.wall
 tileval.u8_floor1 = 0; // floor1
 tileval.u8_floor2 = 0; // floor2
tileval.u8_orc = 0; // orc
 tileval.u8_troll = 0; // troll
tileval.u8_corpse = 0; // corpse
 tileval.u8_hwall = 0;

    return true;
}
