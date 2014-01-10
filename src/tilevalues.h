#ifndef _TILEVALUES_H_
#define _TILEVALUES_H_

#include "libtcod.hpp"

int t8_player = 0; //global player sprite

// sprite 8 made into 16
const int t28_door = 519; // door
const int t28_player = 520; // player
const int t28_wall = 521; // wall
const int t28_nwall = 503; // null.wall
const int t28_floor1 = 522; // floor1
const int t28_floor2 = 523; // floor2
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
int u16_door = 0; // door
int u16_doorv = 0; // doorv
int u16_player = 0; // player
int u16_wall = 0; // wall
int u16_vwall = 0; // v.wall
int u16_trwall = 0; // t-r.wall
int u16_tlwall = 0; // t-l.wall
int u16_bwall = 0; // B.wall
int u16_ibwall = 0; // B.wall
int u16_floor1 = 0; // floor1
int u16_floor2 = 0; // floor2
int u16_orc = 0; // orc
int u16_troll = 0; // troll
int u16_corpse = 0; // corpse

// 8 universal
int u8_door = 0; // door
int u8_doorv = 0; // doorv
int u8_player = 0; // player
int u8_wall = 0; // wall
int u8_vwall = 0; // v.wall
int u8_trwall = 0; // t-r.wall
int u8_tlwall = 0; // t-l.wall
int u8_bwall = 0; // B.wall
int u8_ibwall = 0; // B.wall
int u8_floor1 = 0; // floor1
int u8_floor2 = 0; // floor2
int u8_orc = 0; // orc
int u8_troll = 0; // troll
int u8_corpse = 0; // corpse
int u8_hwall = 0;

TCODColor u8_wallCa; // black
TCODColor u8_wallCb;
TCODColor u8_wallCc; // black
TCODColor u8_wallCd;
TCODColor u8_floorCa;
TCODColor u8_floorCb;

#endif
