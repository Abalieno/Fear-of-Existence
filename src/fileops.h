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
    TCODColor color;
    TCODColor colorb;
    int h; // temp hit points
    int combat_move; // total action points
    int speed; // for initiative

    int wpn_AC, wpn_DC, wpn_B, wpn_aspect;
    int ML;
};  

struct lvl1 {
    std::vector<room_enc> cave1; // types of encounters
    std::vector<mob_types> vmob_types; // all monsters on level
};    

bool load_from(std::string filename, lvl1 &enc);
bool load_element(std::istream &data, lvl1 &enc);
bool load_data(std::istream &data, lvl1 &enc);

#endif
