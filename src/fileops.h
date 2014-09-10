#ifndef _FILEOPS_H_
#define _FILEOPS_H_

#include <string>
#include <vector>
#include "libtcod.hpp"


std::string no_caps(const std::string &orig);
std::string trim(const std::string &orig);

// each room has one
struct room_enc {
    std::vector<int> enc;
    int probability; // probability of each encounter
};    

struct mob_types {

    int s_hp, s_defense, s_power, s_speed;

    char name[20];
    char selfchar;
    int self_8;
    int self_16;
    TCODColor color;
    TCODColor colorb;
    int h; // temp hit points
    int combat_move; // total action points
    int speed; // for initiative
    int STR;
    int armor;

    int wpn_AC, wpn_DC, wpn_B, wpn_E, wpn_P, wpn_aspect, reach;
    int ML;
};  

struct lvl1 {
    std::vector<room_enc> cave1; // types of encounters
    std::vector<mob_types> vmob_types; // all monsters on level (0,1)
}; 

struct lvl1_map {
    int max_x;
    int max_y;
    std::vector<int> map_int;
};    

bool load_from(std::string filename, lvl1 &enc);
bool load_from_map(std::string filename, lvl1_map &map);
bool load_thismap(std::istream &data, lvl1_map &map);
bool load_element(std::istream &data, lvl1 &enc);
bool load_data(std::istream &data, lvl1 &enc);

#endif
