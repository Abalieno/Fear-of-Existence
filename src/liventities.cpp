#include <iostream>
#include <windows.h> // for Sleep() and not currently used
#include <stdio.h>
#include <algorithm> // sort

//#include "tilevalues.h"
#include "game.h"
#include "rng.h"
#include "liventities.h"
#include "colors.h"

#include "debug.h"

extern std::vector<msg_log> msg_log_list;
extern std::vector<msg_log_c> msg_log_context;

extern const int MAP_WIDTH;
extern const int MAP_HEIGHT;

extern std::vector<Tile> map_array;

extern int monster_this(int x, int y, const std::vector<Object_monster> &monvector);

void Object_monster::draw(bool uh, Game &tgame) {
    //con->setDefaultForeground(color);
    
    int newx = 0;
    int newy = 0;
    if (!tgame.gstate.bigg){
        newx = x-tgame.gstate.off_xx;
        newy = y-tgame.gstate.off_yy;
    } else if (tgame.gstate.bigg){
        newx = x-tgame.gstate.off_xx-28;
        newy = y-tgame.gstate.off_yy-18;
    }
        
        // executes only outside combat
        if (!uh){ // if 0
            if(tgame.gstate.bigg){
                colorb = tgame.gstate.con->getCharBackground((newx*2), (newy*2));
            } else {  
                if (selfchar == '%') colorb = tgame.gstate.con->getCharBackground(newx, newy);
                else colorb = TCODColor::black;
                //colorb = con->getCharBackground(x, y);
                
            }
        }

        tgame.gstate.con->setDefaultBackground(colorb);

        if (tgame.gstate.fov_map->isInFov(x,y) || tgame.gstate.debug == 1){ 
            if(tgame.gstate.bigg){ // if 16x16  
               
                // sets color only if sprite isn't colored (lame check)
                tgame.gstate.con->setDefaultForeground(color);
                
                    tgame.gstate.con->putChar((newx*2), (newy*2), self_16, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((newx*2)+1, (newy*2), self_16+100, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((newx*2), (newy*2)+1, self_16+200, TCOD_BKGND_SET);
                    tgame.gstate.con->putChar((newx*2)+1, (newy*2)+1, self_16+300, TCOD_BKGND_SET);
             
            } else { // if 8x8
                tgame.gstate.con->setDefaultForeground(color);
                if(hit) colorb = TCODColor::red;
                else colorb = tgame.gstate.con->getCharBackground(newx, newy);

                tgame.gstate.con->setCharBackground(newx, newy, colorb, TCOD_BKGND_SET);

                if (tgame.tileval.U8 == true){
                    tgame.gstate.con->putChar(newx, newy, self_8, TCOD_BKGND_SET);
                    
                } else {
                    tgame.gstate.con->putChar(newx,newy, selfchar, TCOD_BKGND_SET);
                }    
            }
        }    
        tgame.gstate.con->setDefaultBackground(TCODColor::black); // reset background for smaller map
    }

void Object_monster::clear(Game &tgame) {
        if (tgame.gstate.fov_map->isInFov(x,y)){
            if(tgame.gstate.bigg){
                tgame.gstate.con->putChar((x*2), (y*2), 503, TCOD_BKGND_NONE);
                tgame.gstate.con->putChar((x*2)+1, (y*2), 603, TCOD_BKGND_NONE);
                tgame.gstate.con->putChar((x*2), (y*2)+1, 703, TCOD_BKGND_NONE);
                tgame.gstate.con->putChar((x*2)+1, (y*2)+1, 803, TCOD_BKGND_NONE);
            } else {
                tgame.gstate.con->putChar(x, y, ' ', TCOD_BKGND_NONE);
            }
        }
    }

void Object_monster::drop(std::vector<Generic_object> &wrd_inv){
    for(auto i : inventory){
        i.drop_to(x, y);
        wrd_inv.push_back(i);
    }    
    inventory.clear();
}    

void Object_player::draw(bool uh, Game &tgame) {

    // both newx and xx 110 70
    int newx = 0;
    int newy = 0;
    if (!tgame.gstate.bigg){
        newx = x-tgame.gstate.off_xx;
        newy = y-tgame.gstate.off_yy;
    } else if (tgame.gstate.bigg){
        newx = x-tgame.gstate.off_xx-28;
        newy = y-tgame.gstate.off_yy-18;
    }    
        
    //tgame.gstate.con->print(0, 68, "Mouse on [x,y] at [%d.%d]", x, y);
    //tgame.gstate.con->print(0, 67, "Mouse on [newx,newy] at [%d.%d]", newx, newy);
    //tgame.gstate.con->print(0, 66, "Mouse on [offx,offy] at [%d.%d]", tgame.gstate.off_xx, tgame.gstate.off_yy);

        tgame.gstate.con->setDefaultForeground(color);
        // uh is for attack animation. if uh is 0 then the background is taken from cell
        // if uh is 1 then look at colorb 
        if (!uh){  
            if(tgame.gstate.bigg){
                colorb = tgame.gstate.con->getCharBackground((newx*2), (newy*2));
            } else {    
                //colorb = con->getCharBackground(x, y);
                colorb = TCODColor::black; // for sprite
            }  
        }    
        tgame.gstate.con->setDefaultBackground(colorb);
        if (tgame.gstate.fov_map->isInFov(x,y)){ // isn't it always?
            if(tgame.gstate.bigg){
                tgame.gstate.con->putChar((newx*2), (newy*2), tgame.tileval.u16_player, TCOD_BKGND_SET);
                tgame.gstate.con->putChar((newx*2)+1, (newy*2), tgame.tileval.u16_player+100, TCOD_BKGND_SET);
                tgame.gstate.con->putChar((newx*2), (newy*2)+1, tgame.tileval.u16_player+200, TCOD_BKGND_SET);
                tgame.gstate.con->putChar((newx*2)+1, (newy*2)+1, tgame.tileval.u16_player+300, TCOD_BKGND_SET);
            } else {
                //con->putChar(x, y, 445, TCOD_BKGND_SET);
                tgame.gstate.con->putChar(newx, newy, tgame.tileval.u8_player, TCOD_BKGND_SET);
            }
        }
        tgame.gstate.con->setDefaultBackground(TCODColor::black); // reset background for smaller map
      
    }

void Object_player::clear(Game &tgame) {
    // disabled due to black holes showing in bigg mode
    /*
    if (tgame.gstate.fov_map->isInFov(x,y)){
        if(tgame.gstate.bigg){
            tgame.gstate.con->putChar((x*2), (y*2), 503, TCOD_BKGND_NONE);
            tgame.gstate.con->putChar((x*2)+1, (y*2), 603, TCOD_BKGND_NONE);
            tgame.gstate.con->putChar((x*2), (y*2)+1, 703, TCOD_BKGND_NONE);
            tgame.gstate.con->putChar((x*2)+1, (y*2)+1, 803, TCOD_BKGND_NONE);
        } else {
            tgame.gstate.con->putChar(x, y, ' ', TCOD_BKGND_NONE);
        }
    }
    */
}

bool BasicMonster::take_turn(Object_monster &monster, Object_player &player, int p_x, int p_y, bool myfov, Game &tgame){

    std::cout << "The " << monster.name << " is active." << std::endl;

    // 1.1 so the monster attacks but still moves closer instead of stopping diagonally
    if ( (monster.distance_to(p_x, p_y) >= 1.1) || (monster.chasing && !myfov)){

        if (tgame.gstate.no_combat || monster.combat_move >= 1){ // move up to and including player pos 
            if(monster.step < 4 || !tgame.gstate.iscombat){ // max in a phase || out of combat movement
                monster.move_towards(p_x, p_y);
                if(!tgame.gstate.no_combat){
                    monster.combat_move -= 1;
                    ++monster.step;
                    monster.hasmoved = true;
                }    
                std::cout << "The " << monster.name << " moves." << std::endl;
                return false;
            } else monster.pass = true;
            return false;
        }

    } else if (myfov){
        if(monster.combat_move < 4) monster.combat_move = 0; // if in range but can't attack, desist
        if (tgame.gstate.no_combat || ((monster.combat_move >= 4) && !monster.hasmoved && monster.phase_attack == 0)) {

            monster.path_mode = 0; // switches back to default pathing on attack

            // sets facing
            if(monster.x < player.x) monster.facing = 1;
            if(monster.x > player.x) monster.facing = 3;
            if(monster.y > player.y) monster.facing = 0;
            if(monster.y < player.y) monster.facing = 2;

            tgame.gstate.mesg->setAlignment(TCOD_LEFT);
            tgame.gstate.mesg->setDefaultForeground(TCODColor::yellow);
            tgame.gstate.mesg->setDefaultBackground(TCODColor::black);
            player.colorb = TCODColor::red; // otherwise colorb is set in draw(), by looking at floor
            monster.colorb = TCODColor::black;
            player.selfchar = '/';
            player.draw(1, tgame);
            monster.draw(1, tgame);

            TCODConsole::blit(tgame.gstate.con, 0, 0, 0, 0, TCODConsole::root,0,0);
            TCODConsole::flush();

            Sleep(250); // shitty way for attack "animation", uses windows.h
            player.colorb = tgame.gstate.color_dark_ground;
            monster.colorb = tgame.gstate.color_dark_ground;
            player.selfchar = '@';
            player.draw(0, tgame);
            monster.draw(0, tgame);
            tgame.gstate.con->clear();
            tgame.gstate.fov_recompute = true;

            do monster.stats.attack(player, monster, 1, monster.overpower_l); // calls attack function for monsters
            while (monster.overpower_l > 0);
            monster.overpower_l = 0; // reset
            ++monster.phase_attack; // add one attack executed, for phases

            TCODConsole::flush();
            if(!tgame.gstate.no_combat)monster.combat_move -= 4; // spends movement points for attack
            return true;
        } else if (myfov && !tgame.gstate.no_combat) monster.pass = true; // can't attack, so "pass"
    }
    return false; 
    }

const char *txt_atype(int type){
    if(type == 0) return "lower legs";
    else if(type == 1) return "upper legs";
    else if(type == 2) return "horizontal";
    else if(type == 3) return "overhand";
    else if(type == 4) return "vertical";
    else if(type == 5) return "upward";
    else return "NONE";
}

const char *txt_aspect(int aspect){
    if(aspect == 0) return "Blunt";
    else if(aspect == 1) return "Edge";
    else if(aspect == 2) return "Point";
    else return "NONE";
}

const char *txt_loc(int loc){
    if(loc == 15) return "Feet";
    else if(loc == 14) return "Calf";
    else if(loc == 13) return "Knee";
    else if(loc == 12) return "Thigh";
    else if(loc == 11) return "Hand";
    else if(loc == 10) return "Forearm";
    else if(loc == 9) return "Elbow";
    else if(loc == 8) return "Upper Arm";
    else if(loc == 7) return "Shoulder";
    else if(loc == 6) return "Groin";
    else if(loc == 5) return "Hip";
    else if(loc == 4) return "Abs";
    else if(loc == 3) return "Thorax";
    else if(loc == 2) return "Neck";
    else if(loc == 1) return "Face";
    else if(loc == 0) return "Skull";
    else return "NONE";
}

bool is_overpower(int askill, int aroll, int dskill, int droll){
    int aindex;
    aindex = (askill / 10) - (aroll / 10);
    std::cout << "Aindex: " << aindex;
    if(aindex < 5) {std::cout << " "; return false;}
    if(dskill == 0) {std::cout << " "; return true;} // if defender does not defend
    if(droll <= dskill) {std::cout << " "; return false;} // parried
    int dindex;
    dindex = abs((dskill / 10) - (droll / 10));
    std::cout << " Dindex: " << dindex << std::endl;
    if(aindex + dindex >= 9) return true;
    else return false;
}

bool wayToSort(int i, int j) { return i > j; }

bool pairCompare(const std::pair<int, int>& firstElem, const std::pair<int, int>& secondElem){
  return firstElem.first > secondElem.first; // descending
}

void build_loc(std::vector<std::pair<int, int>> &locator){
    locator.push_back({86,100}); // 15% lower legs
    locator.push_back({61,86}); // 25% upper legs
    locator.push_back({21,60}); // 40% horizontal

    locator.push_back({11,20}); // 10% overhand
    locator.push_back({6,10}); // 5% vertical
    locator.push_back({1,5}); // 5% upward
} 

struct preciseloc{
    int lrange; // d6 lower result
    int hrange;
    int ploc; // precise location 1-16
};   

void build_preciseloc(std::vector<std::vector <preciseloc>> &prec_loc){
    std::vector<preciseloc> tempvect;
    prec_loc.push_back(tempvect);
    prec_loc.push_back(tempvect);
    prec_loc.push_back(tempvect);
    prec_loc.push_back(tempvect);
    prec_loc.push_back(tempvect);
    prec_loc.push_back(tempvect);
    prec_loc.push_back(tempvect);
    preciseloc here;

    // lower legs
    here.lrange = 1;
    here.hrange = 1;
    here.ploc = 15; // feet
    prec_loc[0].push_back(here);
    here.lrange = 2;
    here.hrange = 4;
    here.ploc = 14; // calf
    prec_loc[0].push_back(here);
    here.lrange = 5;
    here.hrange = 6;
    here.ploc = 13; // knee
    prec_loc[0].push_back(here);

    // upper legs
    here.lrange = 1;
    here.hrange = 2;
    here.ploc = 13; // knee
    prec_loc[1].push_back(here);
    here.lrange = 3;
    here.hrange = 5;
    here.ploc = 12; // thigh
    prec_loc[1].push_back(here);
    here.lrange = 6;
    here.hrange = 6;
    here.ploc = 5; // hip
    prec_loc[1].push_back(here);

    // horizontal
    here.lrange = 1;
    here.hrange = 1;
    here.ploc = 5; // hip
    prec_loc[2].push_back(here);
    here.lrange = 2;
    here.hrange = 3;
    here.ploc = 4; // abs
    prec_loc[2].push_back(here);
    here.lrange = 4;
    here.hrange = 5;
    here.ploc = 3; // thorax
    prec_loc[2].push_back(here);
    here.lrange = 6;
    here.hrange = 6;
    here.ploc = 77; // reroll for zone 6 [5]
    prec_loc[2].push_back(here);

    // overhand
    here.lrange = 1;
    here.hrange = 2;
    here.ploc = 7; // shoulder
    prec_loc[3].push_back(here);
    here.lrange = 3;
    here.hrange = 3;
    here.ploc = 3; // thorax
    prec_loc[3].push_back(here);
    here.lrange = 4;
    here.hrange = 4;
    here.ploc = 2; // neck
    prec_loc[3].push_back(here);
    here.lrange = 5;
    here.hrange = 5;
    here.ploc = 1; // face
    prec_loc[3].push_back(here);
    here.lrange = 6;
    here.hrange = 6;
    here.ploc = 0; // skull
    prec_loc[3].push_back(here);

    // vertical
    here.lrange = 1;
    here.hrange = 3;
    here.ploc = 0; // skull
    prec_loc[4].push_back(here);
    here.lrange = 4;
    here.hrange = 4;
    here.ploc = 1; // face
    prec_loc[4].push_back(here);
    here.lrange = 5;
    here.hrange = 6;
    here.ploc = 7; // shoulder
    prec_loc[4].push_back(here);

    // upward
    here.lrange = 1;
    here.hrange = 3;
    here.ploc = 12; // thigh
    prec_loc[5].push_back(here);
    here.lrange = 4;
    here.hrange = 4;
    here.ploc = 6; // groin
    prec_loc[5].push_back(here);
    here.lrange = 5;
    here.hrange = 5;
    here.ploc = 4; // abs
    prec_loc[5].push_back(here);
    here.lrange = 6;
    here.hrange = 6;
    here.ploc = 3; // thorax
    prec_loc[5].push_back(here);

    here.lrange = 1;
    here.hrange = 1;
    here.ploc = 11; // hand
    prec_loc[6].push_back(here);
    here.lrange = 2;
    here.hrange = 3;
    here.ploc = 10; // forearm
    prec_loc[6].push_back(here);
    here.lrange = 4;
    here.hrange = 4;
    here.ploc = 9; // elbow
    prec_loc[6].push_back(here);
    here.lrange = 5;
    here.hrange = 6;
    here.ploc = 8; // upper arm
    prec_loc[6].push_back(here);

}    

void Fighter::attack(Object_player &player, Object_monster &monster, bool who, int overpowering){

    int AML; // basic skill
    int wpn_AC;
    int DML; // basic monster skill
    int wpn_DC;
    int *monsterS;
    int *playerS;
    int ADB;
    int DDB;
    int *AHP;
    int *DHP;
    int *overpower_who;
    int Areach;
    int Dreach;
    int *Adistance;
    int *Ddistance;
    int astrength; // strength of attacker, for damage calc
    std::vector<armorsection> armor_used;
    std::vector<std::pair<int,int>> aspects;

    std::vector<std::pair<int,int>> locator; // locations
    build_loc(locator);
    
    std::vector<std::vector <preciseloc>> prec_loc;
    build_preciseloc(prec_loc);

    if(who){ // if MONSTER is attacking
        AML = monster.stats.ML; // basic skill
        wpn_AC = monster.stats.wpn1.wpn_AC;
        monsterS = &AML;
        ADB = 0; // monster no skill bonus
        AHP = &monster.stats.hp;
        overpower_who = &monster.overpower_l;
        Areach = monster.stats.wpn1.reach;
        Adistance = &monster.distance;
        astrength = monster.STR;
        DML = player.skill.lswdDML; // basic monster skill
        wpn_DC = player.stats.wpn1.wpn_DC;
        playerS = &DML;
        DDB = player.skill.lswdDB;
        DHP = &player.stats.hp;
        Dreach = player.stats.wpn1.reach;
        Ddistance = &player.distance;
        aspects.push_back(monster.stats.wpn1.wp_B); 
        aspects.push_back(monster.stats.wpn1.wp_E);
        aspects.push_back(monster.stats.wpn1.wp_P);
        for(unsigned int n = 0; n < 16; ++n){
            armorsection armortemp;
            armortemp.B = player.armor_tot[n].B;
            armortemp.E = player.armor_tot[n].E;
            armortemp.P = player.armor_tot[n].P;
            armor_used.push_back(armortemp);
        }
    }else{ // if PLAYER
        AML = player.skill.lswdAML; // basic skill
        wpn_AC = player.stats.wpn1.wpn_AC;
        playerS = &AML;
        ADB = player.skill.lswdAB;
        AHP = &player.stats.hp;
        Areach = player.stats.wpn1.reach;
        Adistance = &player.distance;
        astrength = player.STR;
        DML = monster.stats.ML; // basic monster skill
        wpn_DC = monster.stats.wpn1.wpn_DC;
        monsterS = &DML;
        DDB = 0; // monster no skill bonus
        DHP = &monster.stats.hp;
        Dreach = monster.stats.wpn1.reach;
        overpower_who = &player.overpower_l;
        Ddistance = &monster.distance;
        aspects.push_back(player.stats.wpn1.wp_B); 
        aspects.push_back(player.stats.wpn1.wp_E);
        aspects.push_back(player.stats.wpn1.wp_P);
        for(unsigned int n = 0; n < 16; ++n){
            armorsection armortemp;
            armortemp.B = monster.armor_tot[n].B;
            armortemp.E = monster.armor_tot[n].E;
            armortemp.P = monster.armor_tot[n].P;
            armor_used.push_back(armortemp);
        }
    }

    //TCODConsole::waitForKeypress(true);

    std::cout << "------------------------" << std::endl;

    // facing
    if(monster.x > player.x) {player.facing = 1; monster.facing = 3;}
    if(monster.x < player.x) {player.facing = 3; monster.facing = 1;}
    if(monster.y < player.y) {player.facing = 0; monster.facing = 2;}
    if(monster.y > player.y) {player.facing = 2; monster.facing = 0;}

    // close quarters
    bool close_quarters = false; 
    switch(player.facing){
        case 0:
            if( (map_array[player.y * MAP_WIDTH + player.x+1].blocked && map_array[player.y * MAP_WIDTH + player.x-1].blocked) ||
                (map_array[(player.y-1) * MAP_WIDTH + player.x+1].blocked && map_array[(player.y-1) * MAP_WIDTH + player.x-1].blocked) ){
                close_quarters = true;
            }    
            break;
        case 1:
            if( (map_array[player.y-1 * MAP_WIDTH + player.x].blocked && map_array[player.y+1 * MAP_WIDTH + player.x].blocked) ||
                (map_array[player.y-1 * MAP_WIDTH + player.x+1].blocked && map_array[player.y+1 * MAP_WIDTH + player.x+1].blocked) ){
                close_quarters = true;
            }
            break;
        case 2:
            if( (map_array[player.y * MAP_WIDTH + player.x+1].blocked && map_array[player.y * MAP_WIDTH + player.x-1].blocked) ||
                (map_array[(player.y+1) * MAP_WIDTH + player.x+1].blocked && map_array[(player.y+1) * MAP_WIDTH + player.x-1].blocked) ){
                close_quarters = true;
            }
            break;
        case 3:
            if( (map_array[(player.y-1) * MAP_WIDTH + player.x].blocked && map_array[(player.y+1) * MAP_WIDTH + player.x].blocked) ||
                (map_array[(player.y-1) * MAP_WIDTH + player.x-1].blocked && map_array[(player.y+1) * MAP_WIDTH + player.x-1].blocked) ){
                close_quarters = true;
            }
            break;
    }  
    if(close_quarters)
        std::cout << "Close Quarters. " << player.facing << std::endl;
    else std::cout << "Open Space. " << player.facing << std::endl;
    // random weapon aspect
    std::pair<int,int> aspect;
    int rnd_asp = dice(1,6);
    std::cout << "Weapon aspect roll: " << rnd_asp << std::endl;
    std::sort(aspects.begin(), aspects.end(), pairCompare);
    // blunt = 0, edge = 1, point = 2
    std::cout << "Primary: " << aspects[0].first << "-" << txt_aspect(aspects[0].second) << 
        " Secondary: " << aspects[1].first << "-" << txt_aspect(aspects[1].second) << " Tertiary: " << 
        aspects[2].first << "-" << txt_aspect(aspects[2].second) << std::endl;
    if(rnd_asp >= 4){ 
        std::cout << "Using Primary." << std::endl;
        aspect.first = aspects[0].first;
        aspect.second = aspects[0].second;
    }    
    if(rnd_asp == 2 || rnd_asp == 3){
        if(aspects[1].first == 0){ 
            std::cout << "Using Primary." << std::endl;
            aspect.first = aspects[0].first;
            aspect.second = aspects[0].second;
        }else{ 
            std::cout << "Using Secondary." << std::endl;
            aspect.first = aspects[1].first;
            aspect.second = aspects[1].second;
        }    
    }  
    if(rnd_asp == 1){
        if(aspects[2].first == 0){
            if(aspects[1].first == 0){ 
                std::cout << "Using Primary." << std::endl;
                aspect.first = aspects[0].first;
                aspect.second = aspects[0].second;
            }else{ 
                std::cout << "Using Secondary." << std::endl;
                aspect.first = aspects[1].first;
                aspect.second = aspects[1].second;
            }    
        }else{ 
            std::cout << "Using Tertiary." << std::endl; 
            aspect.first = aspects[2].first;
            aspect.second = aspects[2].second;
        }    
    }
    if( (aspect.second == 0 || aspect.second == 1) && close_quarters ){
        wpn_AC = 5 - wpn_AC;
    }   

    //TCODConsole::waitForKeypress(true);

    // crit chances
    int a_crit = AML / 10;
    if(a_crit < 1) a_crit = 1;
    int d_crit = DML / 10;
    if(d_crit < 1) d_crit = 1;
    int raw_AML = AML;
    int raw_DML = DML;
    int a_critF = (100 - raw_AML) / 20; // half of index
    if(a_critF < 1) a_critF = 1;
    std::cout << "Attack critF: " << a_critF << std::endl;
    int d_critF = (100 - raw_DML) / 20;
    if(d_critF < 1) d_critF = 1;
    std::cout << "Defense critF: " << d_critF << std::endl;
    std::cout << "Raw AML: " << raw_AML << " DML: " << raw_DML << std::endl;

    // WEAPON COMPARISON TABLE
    if(wpn_AC > wpn_DC)
        AML += 5 * (wpn_AC - wpn_DC);
    if(wpn_DC > wpn_AC)
        DML += 5 * (wpn_DC - wpn_AC);
    std::cout << "Weapon Class AML: " << AML << " DML: " << DML << std::endl;

    // REACH
    if(Areach > *Adistance){ 
        AML -= 5 * (Areach - *Adistance); // longer attack weapon is engaged at closer range
        std::cout << "Longer attack weapon in close range: -" << 5 * (Areach - *Adistance) << std::endl;
    }    
    if(Dreach > *Ddistance){ 
        DML -= 5 * (Dreach - *Ddistance); // longer defending weapon is engaged at closer range
        std::cout << "Longer defense weapon in close range: -" << 5 * (Dreach - *Ddistance) << std::endl;
    }    
    if(Areach < *Ddistance){ 
        AML -= 5 * (*Ddistance - Areach); // shorter attacking weapon is engaged out of its range
        std::cout << "Shorter attack weapon out of range: -" << 5 * (*Ddistance - Areach) << std::endl;
    }
    std::cout << "Weapon Reach AML: " << AML << " DML: " << DML << std::endl;

    // for every attack the player defends from or does, a -10 penality is applied
    if(player.cflag_attacks >= 1){
        *playerS += (player.cflag_attacks * 10) * -1;
        std::cout << "Player actions this round: " << player.cflag_attacks << std::endl;
    }  
    player.cflag_attacks++; // increment counter (reset at beginning of combat turn)

    // for every attack the monster defends from or does, a -10 penality is applied
    if(monster.cflag_attacks >= 1){
        *monsterS += (monster.cflag_attacks * 10) * -1;
        std::cout << "Monster actions this round: " << monster.cflag_attacks << std::endl;
    }  
    monster.cflag_attacks++; // increment counter (reset at beginning of combat turn)

    // skill bonuses
    AML += ADB;
    DML += DDB;
    std::cout << "Attacker skill bonus: " << ADB << std::endl;
    std::cout << "Defender skill bonus: " << DDB << std::endl;

    // roll two 1d100, one for player, one for monster
    int a_d100 = rng(1, 100);
    int d_d100 = rng(1, 100);

    int a_success_level = 0;
    if (a_d100 <= a_crit){
        a_success_level = 0; // CS Critical Success
    } else if(a_d100 <= AML) a_success_level = 1; // MS Marginal Success
    else a_success_level = 2; // MF Marginal Failure
    if(a_d100 > (100 - a_critF)) a_success_level = 3; // CF Critical Failure

    int d_success_level = 0;
    if (d_d100 <= d_crit){
        d_success_level = 0; // CS Critical Success
    } else if(d_d100 <= DML) d_success_level = 1; // MS Marginal Success
    else d_success_level = 2; // MF Marginal Failure
    if(d_d100 > (100 - d_critF)) d_success_level = 3; // CF Critical Failure

    bool overpower = false;
    int skilltot = raw_AML + ADB;
    if(raw_AML >= 80 && skilltot >= 80){ // checks if bonus is malus
        overpower = is_overpower(AML, a_d100, DML, d_d100);
        std::cout << "Overpower: " << overpower << std::endl;
    }
    if(overpower) ++*overpower_who;
    else *overpower_who = 0;
    if(overpowering){
        msg_log msgo;
        msgo.c1 = 1; // sets background color (?)
        msgo.color1 = TCODColor::white;
        msgo.bcolor1 = TCODColor::red;
        sprintf(msgo.message, "%cOverpowering!%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        msg_log_list.push_back(msgo);
    }

    const int melee_res[4][4] = 
    {
        { 4, 5, 6, 7, },
        { 4, 4, 5, 6, },
        { 2, 8, 4, 3, },
        { 2, 2, 2, 1, }
    };

    std::cout << "Pure AML: " << raw_AML << " Final AML: " << AML << " a_d100: " << a_d100 << std::endl;
    std::cout << "Pure DML: " << raw_DML << " Final DML: " << DML << " d_d100: " << d_d100 << std::endl;
    std::cout << "A Success Level: " << a_success_level << std::endl;
    std::cout << "D Success Level: " << d_success_level << std::endl;
    std::cout << "Melee Result: " << melee_res[a_success_level][d_success_level] << std::endl;

    msg_log msgd;
    char whois;
    char a_string[20];
    char d_string[20];
    if(who) {whois = '*'; strcpy(a_string, monster.name); strcpy(d_string, "Player");}
    else {whois = '>'; strcpy(a_string, "Player"); strcpy(d_string, monster.name);}
    sprintf(msgd.message, "%c%c%c%s's skill(%d/%c%d%c) %c1d100%c(%c%d%c) VS %s's defense(%d/%c%d%c) %c1d100%c(%c%d%c)",
            TCOD_COLCTRL_5, whois, TCOD_COLCTRL_STOP, a_string, raw_AML,
            TCOD_COLCTRL_1, AML, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
            TCOD_COLCTRL_3, a_d100, TCOD_COLCTRL_STOP, d_string, raw_DML,
            TCOD_COLCTRL_1, DML, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
            TCOD_COLCTRL_4, d_d100, TCOD_COLCTRL_STOP);
    msgd.color1 = TCODColor::cyan;
    msgd.color2 = dicec;
    if(who) msgd.color5 = TCODColor::red;
    else msgd.color5 = TCODColor::lighterBlue;
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

    int result = melee_res[a_success_level][d_success_level];

    int location = 20; // if 20 = broken
    int dam_index;
    if(result >= 5 && result <= 7){
        // location roll
        int locroll = dice(1,100);
        for (unsigned int n = 0; n < locator.size(); ++n){
            if(locroll >= locator[n].first && locroll <= locator[n].second) location = n; 
        }
        std::cout << "Location swing(1-6): " << location+1 << "(" << locroll << ")" << std::endl;
        locroll = dice(1,6);
        int finloc = 666;
        for (unsigned int n = 0; n < prec_loc[location].size(); ++n){
            if(locroll >= prec_loc[location][n].lrange && locroll <= prec_loc[location][n].hrange) finloc = prec_loc[location][n].ploc; 
        }
        if(finloc == 77){ // reroll for arms
            std::cout << "Rerolling for arms." << std::endl;
            locroll = dice(1,6);
            for (unsigned int n = 0; n < prec_loc[6].size(); ++n){ // loc 6 is arms area
                if(locroll >= prec_loc[6][n].lrange && locroll <= prec_loc[6][n].hrange) finloc = prec_loc[6][n].ploc; 
            } 
        }    
        std::cout << "Location hit(0-15): " << finloc << "(" << locroll << ")" << std::endl;

        // damage calculation
        dam_index = (AML / 10) - (a_d100 / 10); // attack
        if(dam_index < 1) dam_index = 1;
        dam_index += ( (d_d100 / 10) - (DML / 10) ) / 2; // defense (half the index)
        std::cout << "Damage Index: " << dam_index << std::endl;
        int txt_index = dam_index;
        dam_index += astrength;
        dam_index += aspect.first;
        std::cout << "Damage total (STR, weapon value): " << dam_index << "(" << astrength << ", " << aspect.first << ")" << std::endl;
        int armor_prot = 0;
        std::cout << "Aspect: " << aspect.second << std::endl;
        switch(aspect.second){
            case 0:
                armor_prot = armor_used[finloc].B;
                break;
            case 1:
                armor_prot = armor_used[finloc].E;
                break;
            case 2:
                armor_prot = armor_used[finloc].P;
                break;
        }    
        std::cout << "Armor protection factor: " << armor_prot << std::endl;
        dam_index -= armor_prot;
        std::cout << "Final damage: " << dam_index << std::endl;
        int dam_text = dam_index;

        if(aspect.second == 0) dam_index -= 5;
        if(aspect.second == 2) dam_index -= 2;

        if(finloc == 3) dam_index -= 6;
        if(finloc == 8) dam_index -= 6;
        if(finloc == 10) dam_index -= 6;
        if(finloc == 14) dam_index -= 6;

        if(finloc == 1) dam_index += 6;
        if(finloc == 2) dam_index += 6;
        if(finloc == 6) dam_index += 6;
        
        if(dam_index <= 0) dam_index = 0;
        else if(dam_index >= 1 && dam_index < 7) dam_index = 1;
        else if(dam_index >= 7 && dam_index < 13) dam_index = 2;
        else if(dam_index >= 13 && dam_index < 19) dam_index = 3;
        else if(dam_index >= 19 && dam_index < 25) dam_index = 4;
        else if(dam_index >= 25 && dam_index < 30) dam_index = 5;
        else if(dam_index >= 30 && dam_index < 90) dam_index = 60;

        msg_log msgh;
        msgh.c1 = 1; // sets background color (?)
        msgh.color1 = TCODColor::yellow;
        msgh.bcolor1 = TCODColor::black;
        msgh.color2 = TCODColor::lighterBlue;
        msgh.bcolor2 = TCODColor::black;
        sprintf(msgh.message, "  Attack type %c%s%c with aspect %c%s%c, hitting %c%s%c.", 
                TCOD_COLCTRL_1, txt_atype(location), TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, txt_aspect(aspect.second), TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, txt_loc(finloc), TCOD_COLCTRL_STOP);
        msg_log_list.push_back(msgh);
        sprintf(msgh.message, "  Damage done: %c%d%c. Index(%c%d%c) + STR(%c%d%c) + Weapon(%c%d%c) - Armor Abs.(%c%d%c)", 
                TCOD_COLCTRL_2, dam_text, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, txt_index, TCOD_COLCTRL_STOP, 
                TCOD_COLCTRL_1, astrength, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, aspect.first, TCOD_COLCTRL_STOP, 
                TCOD_COLCTRL_1, armor_prot, TCOD_COLCTRL_STOP);
        msg_log_list.push_back(msgh);
        msgh.color3 = TCODColor::lighterRed;
        msgh.bcolor3 = TCODColor::black;
        sprintf(msgh.message, "  Wound severity: %c%d%c Temp damage %c%d%c.", 
                TCOD_COLCTRL_3, dam_index, TCOD_COLCTRL_STOP, TCOD_COLCTRL_3, dam_index, TCOD_COLCTRL_STOP);
        msg_log_list.push_back(msgh);
        // 1 7 13 19 25 30M
        
        *DHP -= dam_index;
    }    

    msg_log msg1;
    bool skip = false;
    switch(result){
        case 1:
            *AHP -= 1;
            *DHP -= 1;
            sprintf(msg1.message, "  Both fumble! %c-1%c to HP for both.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            break;
        case 2:
            *AHP -= 1;
            sprintf(msg1.message, "  The %s fumbles the attack! %c-1%c to HP.", a_string, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);  
            break;
        case 3:
            *DHP -= 1;
            sprintf(msg1.message, "  The %s misses, but the %s fumbles the parry and hurt himself! %c-1%c to HP.", 
                    a_string, d_string, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            break;
        case 4:
            sprintf(msg1.message, "  The %s swings the weapon, but the %s parries.", 
                    a_string, d_string);
            break;
        case 5:
            if(Areach < Dreach) *Ddistance = Areach;
            if(*Adistance < Areach) *Adistance = Areach;
            skip = true;
            //*DHP -= weapond;
            //sprintf(msg1.message, "%c!%cThe %s hits the %s for %c%d%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, 
            //        a_string, d_string, TCOD_COLCTRL_1, weapond, TCOD_COLCTRL_STOP);
            break;
        case 6:
            if(Areach < Dreach) *Ddistance = Areach;
            if(*Adistance < Areach) *Adistance = Areach;
            skip = true;
            //*DHP -= weapond * 2;
            //sprintf(msg1.message, "%c!%cCritial attack! %c%d%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, (weapond * 2), TCOD_COLCTRL_STOP);
            break;
        case 7:
            if(Areach < Dreach) *Ddistance = Areach;
            if(*Adistance < Areach) *Adistance = Areach;
            skip = true;
            //*DHP -= weapond * 3;
            //sprintf(msg1.message, "%c!%cTriple damage! %c%d%c HP.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, (weapond * 3), TCOD_COLCTRL_STOP);
            msg1.color1 = TCODColor::red;
            break;
        case 8:
            sprintf(msg1.message, "  The %s misses.", a_string);
            break;    
    }
    if(who) msg1.color1 = TCODColor::red;
    else msg1.color1 = TCODColor::lighterBlue;
    if(skip){
    } else msg_log_list.push_back(msg1);
}

void move_obj(int x, int y, std::vector<Generic_object> &wrd_inv){
    //drop_to(x, y);
}

int switchweapon(Game &GAME, bool mode){

    // AP usage to swap equipment, based on Agility
    int action = 8;
    if (GAME.player->AGI >= 12 && GAME.player->AGI <= 14) action -= 1;
    else if (GAME.player->AGI == 15 || GAME.player->AGI == 16) action -= 2;
    else if (GAME.player->AGI == 17) action -= 3;
    else if (GAME.player->AGI == 18) action -= 4;
    else if (GAME.player->AGI > 18) action -= 4 + (GAME.player->AGI - 18);
    if(action <= 0) action = 1;

    msg_log msgd;
    int sindex = GAME.player->skill.bowML / 10;
    debugmsg("Bow skill index: %d", sindex);
    int formula = (GAME.player->STR + sindex) * 5;
    debugmsg("Usable Draw Weight: %d", formula);
    bool is_ok = false;
    if(formula >= GAME.player->eRangedDW) is_ok = true;

    if(GAME.player->AP < action && !GAME.player->forcedswap){ 
        //GAME.player->rangeweapon = false;
        sprintf(msgd.message, "Not enough movement points to swap weapons (need %d). Press again to force.", action);
        msg_log_list.push_back(msgd);
        GAME.player->forcedswap = true;
        return 0;
    }    

    if(GAME.player->rangeweapon){ // switch back to melee, always possible 
        sprintf(msgd.message, "%c>%cPlayer weapon switch in progress. (cost: %c%d%cAP)", 
                TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_2, action, TCOD_COLCTRL_STOP);
        msgd.color1 = TCODColor::lighterBlue;
        msgd.color2 = TCODColor::red;
        msg_log_list.push_back(msgd);
        GAME.player->APburn = action;
        //GAME.player->AP -= action;
        GAME.player->forcedswap = false;
        GAME.player->rangeaim = 0; // resets aim phase
        GAME.player->aim = 0; // resets aim
        GAME.player->ranged_target = -2;
        if(mode) return 6;
        else return 0;
    } else if (is_ok){ // if melee to ranged, and check passed, do the switch
        sprintf(msgd.message, "%c>%cPlayer weapon switch in progress. (cost: %c%d%cAP)", 
                TCOD_COLCTRL_1, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_2, action, TCOD_COLCTRL_STOP);
        msgd.color1 = TCODColor::lighterBlue;
        msgd.color2 = TCODColor::red;
        msg_log_list.push_back(msgd);
        GAME.player->APburn = action;
        //GAME.player->AP -= action;
        GAME.player->forcedswap = false;
        GAME.player->rangeaim = 0; // resets aim phase
        GAME.player->aim = 0; // resets aim
        GAME.player->ranged_target = -2;
        if(mode) return 6;
        else return 0;
    } else { // if melee to ranged, but check not passed
        debugmsg("Can't equip bow. Needed: %d Have: %d", GAME.player->eRangedDW, formula);
        msgd.color1 = dicec;
        msgd.color2 = TCODColor::red;
        sprintf(msgd.message, "Can't equip bow. Needed: %c%d%c Have: %c%d%c", TCOD_COLCTRL_1, GAME.player->eRangedDW, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_2, formula, TCOD_COLCTRL_STOP);
        msg_log_list.push_back(msgd);
        msgd.color1 = dicec;
        sprintf(msgd.message, "  Draw Weight = (STR + Bow Skill Index) * 5 / (%c%d%c+%c%d%c)*5 = %c%d%c", 
                TCOD_COLCTRL_1, GAME.player->STR, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, sindex, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, formula, TCOD_COLCTRL_STOP);
        msg_log_list.push_back(msgd);
        GAME.player->forcedswap = false;
        return 0;
    } 
    return 0;
}

void switchweapon_ex(Game &GAME){
    msg_log msgd;
    int sindex = GAME.player->skill.bowML / 10;
    int formula = (GAME.player->STR + sindex) * 5;
    if(GAME.player->rangeweapon){ // switch back to melee, always possible 
        GAME.player->rangeweapon = false;
        GAME.player->att_phase = 3; // average phase weapon
        GAME.player->attAP = 4; // attack APs
        sprintf(msgd.message, "Sword equipped. Melee mode active.");
        msg_log_list.push_back(msgd);
        return;
    } else { 
        GAME.player->rangeweapon = true;
        GAME.player->att_phase = 3; // average phase weapon
        GAME.player->attAP = 2; // default attack APs
        if(GAME.player->DEX >= 10 && GAME.player->DEX < 20) GAME.player->attAP = 1;
        if(GAME.player->DEX >= 20) GAME.player->attAP = 0;
        msgd.color1 = dicec;
        msgd.color2 = TCODColor::lightGreen;
        sprintf(msgd.message, "Bow equipped. Draw Weight needed: %c%d%c Have: %c%d%c", TCOD_COLCTRL_1, GAME.player->eRangedDW, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_2, formula, TCOD_COLCTRL_STOP);
        msg_log_list.push_back(msgd);
        msgd.color1 = dicec;
        sprintf(msgd.message, "  Draw Weight = (STR + Bow Skill Index) * 5 / (%c%d%c+%c%d%c)*5 = %c%d%c", 
                TCOD_COLCTRL_1, GAME.player->STR, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, sindex, TCOD_COLCTRL_STOP,
                TCOD_COLCTRL_1, formula, TCOD_COLCTRL_STOP);
        msg_log_list.push_back(msgd);
        return;
    }
    return;
}   

bool is_threat(Game &GAME, const std::vector<Object_monster> &monvector){
    // checks if monster is in nearby cells
    for (unsigned int i = 0; i<monvector.size(); ++i){ 
        if (monvector[i].x == GAME.player->x && monvector[i].y == GAME.player->y-1)
            if (monvector[i].alive == true) return true;
        if (monvector[i].x == GAME.player->x && monvector[i].y == GAME.player->y+1)
            if (monvector[i].alive == true) return true;
        if (monvector[i].x == GAME.player->x-1 && monvector[i].y == GAME.player->y)
            if (monvector[i].alive == true) return true;
        if (monvector[i].x == GAME.player->x+1 && monvector[i].y == GAME.player->y)
            if (monvector[i].alive == true) return true;
    }
    return false;
}   

void success_l(msg_log &msgd, int s_level){
    switch(s_level){
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
    return;
}

bool fire_target(Game &GAME, int &phasemove, std::vector<Object_monster> &monsters, int &whohit){
    msg_log msgd;

    if(GAME.player->AP == 0){
        sprintf(msgd.message, "Not enough Action Points for the action.");
        msg_log_list.push_back(msgd);
        return 0;
    } else if(phasemove >= 4){ // shouldn't happen
        sprintf(msgd.message, "Not enough APs, this phase."); 
        msg_log_list.push_back(msgd);
        return 0;
    }
    if(GAME.player->ranged_target == -2){ // shouldn't happen
        sprintf(msgd.message, "No valid target."); 
        msg_log_list.push_back(msgd);
        return 0;
    }  
    if(GAME.player->ranged_target >= 0){ // if targeting a monster
        if(!monsters[GAME.player->ranged_target].alive){ // if target has been killed
            GAME.player->ranged_target = -1; // convert to point blank
        } else {
            GAME.player->tlocx = monsters[GAME.player->ranged_target].x; // update target pos
            GAME.player->tlocy = monsters[GAME.player->ranged_target].y;
        } 
        if(!GAME.gstate.fov_map->isInFov(monsters[GAME.player->ranged_target].x,monsters[GAME.player->ranged_target].y)){
            GAME.player->ranged_target = -2; // reset if gone out of fov
            GAME.player->aim = 0; // resets aim
            if(GAME.player->rangeaim > 2) GAME.player->rangeaim = 2; // resets rangephase, but not unlock target
            sprintf(msgd.message, "Target not visible."); 
            msg_log_list.push_back(msgd);
            return 0;
        }
    }

    // initial base to-hit
    bool didhit = false; 
    int firstroll = 0; // first to-hit roll 
    int firstchance = 0; // first to-hit corrected by distance 
    int stepdistance = 0; // distance
    int rangepenalty = 0;
    stepdistance = (int)sqrt(pow(GAME.player->tlocx-GAME.player->x,2)+pow(GAME.player->tlocy-GAME.player->y,2));
    if(stepdistance <= GAME.player->rangedD1) rangepenalty = 10;
    else if(stepdistance > GAME.player->rangedD1 && stepdistance <= GAME.player->rangedD2) rangepenalty = 5;
    else if(stepdistance > GAME.player->rangedD2 && stepdistance <= GAME.player->rangedD3) rangepenalty = 0;
    else if(stepdistance > GAME.player->rangedD3 && stepdistance <= GAME.player->rangedD4) rangepenalty = -20;
    else if(stepdistance > GAME.player->rangedD4 && stepdistance <= GAME.player->rangedD5) rangepenalty = -40; 
    else if(stepdistance > GAME.player->rangedD5 && stepdistance <= GAME.player->rangedD6) rangepenalty = -80;
    if(stepdistance <= GAME.player->rangedD4 && stepdistance > 0){ // not too close and not hitting ceiling
        firstchance = GAME.player->aim + rangepenalty;
        if(firstchance > 0){
            // should add here target modificators, like target is moving
            firstroll = dice(1,100);

            // to-hit success levels
            int critS = GAME.player->skill.bowML / 10;
            if(critS < 1) critS = 1;
            int critF = (100 - GAME.player->skill.bowML) / 20;
            if(critF < 1) critF = 1;
            int success_level = 0;
            if (firstroll <= critS) success_level = 0; // CS Critical Success
            else if(firstroll <= firstchance) success_level = 1; // MS Marginal Success
            else success_level = 2; // MF Marginal Failure
            if(firstroll > (100 - critF)) success_level = 3; // CF Critical Failure

            sprintf(msgd.message, 
                    "%c>%cThe player fires an arrow. Aim(%c%d%%%c) + distance(%c%d%%%c) %c1d100%c(%c%d%c)",
                    TCOD_COLCTRL_5, TCOD_COLCTRL_STOP,
                    TCOD_COLCTRL_1, GAME.player->aim, TCOD_COLCTRL_STOP,
                    TCOD_COLCTRL_1, rangepenalty, TCOD_COLCTRL_STOP,
                    TCOD_COLCTRL_2, TCOD_COLCTRL_STOP, 
                    TCOD_COLCTRL_4, firstroll, TCOD_COLCTRL_STOP);
            msgd.color5 = TCODColor::lighterBlue;
            msgd.color1 = TCODColor::lighterGreen;
            msgd.color2 = dicec;
            success_l(msgd, success_level);
            msg_log_list.push_back(msgd);

            if(success_level < 2){ // 0/1 successes 2/3 failures
                int x = GAME.player->x;
                int y = GAME.player->y;
                int impiled = 0; // numbers of monsters passing through
                TCODLine::init(GAME.player->x,GAME.player->y,GAME.player->tlocx,GAME.player->tlocy);
                while (!TCODLine::step(&x,&y)) {
                    if(map_array[y * MAP_WIDTH + x].blocked){ 
                        if(GAME.gstate.fov_map->isInFov(x,y)){
                                GAME.player->tlocx = x;
                                GAME.player->tlocy = y; // updates coord with wall hit
                                sprintf(msgd.message, "  The arrow struck a wall.");
                        } else {
                            if((int)sqrt(pow(x-GAME.player->x,2)+pow(y-GAME.player->y,2)) <= 20)
                                sprintf(msgd.message, "  You hear the sound of the arrow hitting a wall.");
                            else sprintf(msgd.message, "  The arrow flies out of sight."); 
                        }    
                        msg_log_list.push_back(msgd);
                        didhit = true; // CHECK CHECK CHECK
                        break; // hit a wall
                    }
                    int i = monster_this(x, y, monsters);
                    if(i != -1){ // if found a monster alive at coord
                        if(x == GAME.player->tlocx && y == GAME.player->tlocy){
                            if(impiled == 0){ // hit designated
                                whohit = i;
                                monsters[i].stats.hp -= 20;
                                didhit = true;
                            } else { // reached the target, but after going through
                                int secondroll = dice(1,100);
                                if(secondroll <= 100 - (20 * (impiled + 1)) ){ // hit
                                    whohit = i;
                                    monsters[i].stats.hp -= 20;
                                    didhit = true;
                                }    
                            }    
                        }else { // found a different monster
                            if(impiled == 4){ // no more chance, abort
                                GAME.player->deflx = x;
                                GAME.player->defly = y;
                                didhit = true;
                                break;
                            }    
                            int secondroll = dice(1,100);
                            if(secondroll <= 100 - (20 * (impiled + 1)) ){ // hit
                                whohit = i;
                                monsters[i].stats.hp -= 20;
                                GAME.player->deflected = true;
                                GAME.player->deflx = x;
                                GAME.player->defly = y;
                                msgd.color4 = TCODColor::lighterBlue;
                                didhit = true;
                            } else msgd.color4 = TCODColor::red;
                            sprintf(msgd.message, 
                                    "  There's a different monster in the arrow's path: chance(%c%d%%%c) %c1d100%c(%c%d%c)", 
                                    TCOD_COLCTRL_1, 100 - (20 * (impiled + 1)), TCOD_COLCTRL_STOP,
                                    TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                                    TCOD_COLCTRL_4, secondroll, TCOD_COLCTRL_STOP);
                            msgd.color1 = TCODColor::lighterGreen;
                            msgd.color2 = dicec;
                            msg_log_list.push_back(msgd);
                            ++impiled;
                        }    
                    }        
                    if (didhit) break; // stop arrow
                }    
            } else { // change trajectory
                GAME.player->deflected = true;
                int fail_index = (firstroll - firstchance) / 10;
                if(fail_index < 1) fail_index = 1;
                int stepdistance = (int)sqrt(pow(GAME.player->tlocx-GAME.player->x,2)+
                        pow(GAME.player->tlocy-GAME.player->y,2));
                if(stepdistance < fail_index) fail_index = stepdistance; // avoid shooting arrows backwards
                sprintf(msgd.message, "  Fail index: %d", fail_index); 
                msg_log_list.push_back(msgd);
                bool devx = one_in(2); // to determine negative or not
                bool devy = one_in(2);
                do{
                    GAME.player->deflx = rng(0,fail_index);
                    GAME.player->defly = rng(0,fail_index);
                } while (GAME.player->deflx == 0 && GAME.player->defly == 0);
                if(!devx) GAME.player->deflx = -abs(GAME.player->deflx); // make negative
                if(!devy) GAME.player->defly = -abs(GAME.player->defly); // make negative
                sprintf(msgd.message, "  The arrow flies away from your target: x,y(%d,%d)", 
                        GAME.player->deflx, GAME.player->defly); 
                msg_log_list.push_back(msgd);

                GAME.player->deflx = GAME.player->tlocx + GAME.player->deflx;
                GAME.player->defly = GAME.player->tlocy + GAME.player->defly;

                int x = GAME.player->x;
                int y = GAME.player->y;
                int impiled = 0; // numbers of monsters passing through
                TCODLine::init(GAME.player->x,GAME.player->y,GAME.player->deflx,GAME.player->defly);
                while (!TCODLine::step(&x,&y)) {
                    if(map_array[y * MAP_WIDTH + x].blocked){ 
                        if(GAME.gstate.fov_map->isInFov(x,y)){
                            sprintf(msgd.message, "  The arrow struck a wall.");
                        } else {
                            if((int)sqrt(pow(x-GAME.player->x,2)+pow(y-GAME.player->y,2)) <= 20)
                                sprintf(msgd.message, "  You hear the sound of the arrow hitting a wall.");
                            else sprintf(msgd.message, "  The arrow flies out of sight."); 
                        }   
                        GAME.player->deflx = x;
                        GAME.player->defly = y;
                        didhit = true;
                        msg_log_list.push_back(msgd);
                        break; // hit a wall
                    }
                    int i = monster_this(x, y, monsters);
                    if(i != -1){ // if found a monster alive at coord
                        int secondroll = dice(1,100);
                        if(firstchance / (2 * (impiled + 1)) < 5){ // no skill, arrow stops there 
                            GAME.player->deflx = x;
                            GAME.player->defly = y;
                            didhit = true; 
                            break;
                        }
                        if(secondroll <= firstchance / (2 * (impiled + 1))){ // hit
                            whohit = i; // sent to ragekill function in main.cpp
                            monsters[i].stats.hp -= 20;
                            GAME.player->deflx = x;
                            GAME.player->defly = y;
                            didhit = true;
                            msgd.color4 = TCODColor::lighterBlue;
                        } else msgd.color4 = TCODColor::red;
                        sprintf(msgd.message, 
                                "  There's a different monster in the arrow's path: aim/%d(%c%d%%%c) %c1d100%c(%c%d%c)",
                                2*(impiled+1), 
                                TCOD_COLCTRL_1, firstchance / (2 * (impiled + 1)), TCOD_COLCTRL_STOP,
                                TCOD_COLCTRL_2, TCOD_COLCTRL_STOP,
                                TCOD_COLCTRL_4, secondroll, TCOD_COLCTRL_STOP);
                        msgd.color2 = dicec;
                        msgd.color1 = TCODColor::lighterGreen;
                        msg_log_list.push_back(msgd);
                        ++impiled;
                    }
                    if (didhit) break; // stop arrow    
                }
                
            }    
        } else { // if firstchance <= 0
            sprintf(msgd.message, "Not enough aim, aborted."); 
            msg_log_list.push_back(msgd);
            return 0;
        }    
    } else { 
        sprintf(msgd.message, "Either too close or too far."); 
        msg_log_list.push_back(msgd);
        return 0;
    } 

    GAME.player->AP -= GAME.player->attAP;
    phasemove += GAME.player->attAP;
    GAME.player->aim = 0; // resets aim
    GAME.player->rangeaim = 0; // resets rangephase, but not unlock target 
    return didhit;
    // verify all returns for spending points even when not hitting anything and exiting earlier
}

int player_aim(Game &GAME, int &phasemove, const std::vector<Object_monster> &monsters){
    msg_log msgd;

    bool nothreat = false;
    nothreat = !is_threat(GAME, monsters); nothreat = true; //DISABLE 
    int cost = 2; // cost in AP for the aim action
    if(GAME.player->rangeaim == 0 && !nothreat){
        sprintf(msgd.message, "Cannot nock arror, monster too close.");
        msg_log_list.push_back(msgd);
        return 0;
    }    
    if(GAME.player->rangeaim == 0 && nothreat){ // NOCK
        if(GAME.player->AGI >= 16) cost -= 1; 
        if(GAME.player->AP >= cost && (phasemove + cost) <= 4){
            phasemove += cost;
            GAME.player->AP -= cost;
            GAME.player->rangeaim += 1; // nocking (1)
            sprintf(msgd.message, "%c>%cPlayer nocking arrow (%dAP)", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, cost);
            msgd.color1 = TCODColor::lighterBlue;
            msg_log_list.push_back(msgd);
            if(GAME.player->DEX >= 18){ // no costs, so drawing too 
                GAME.player->rangeaim += 1; // drawing (2)
                sprintf(msgd.message, "%c>%cplayer drawing (0AP)", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
                msgd.color1 = TCODColor::lighterBlue;
                msg_log_list.push_back(msgd);
            } 
            return 0;
        } else if(GAME.player->AP < cost){
            sprintf(msgd.message, "Not enough Action Points for the action.");
            msg_log_list.push_back(msgd);
            return 0;
        } else {
            sprintf(msgd.message, "Not enough APs, this phase."); // replace with action in progress
            msg_log_list.push_back(msgd);
            return 0;
        }    
    }   
    if(GAME.player->rangeaim == 1){ // DRAW
        if(GAME.player->DEX == 18){
            GAME.player->rangeaim += 1;
            sprintf(msgd.message, "%c>%cplayer drawing (0AP)", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            msgd.color1 = TCODColor::lighterBlue;
            msg_log_list.push_back(msgd);  
            return 0;
        } else if(GAME.player->DEX >= 15) cost -= 1;
        if(GAME.player->AP >= cost && (phasemove + cost) <= 4){
            phasemove += cost;
            GAME.player->AP -= cost;
            GAME.player->rangeaim += 1;
            sprintf(msgd.message, "%c>%cPlayer drawing (%dAP)", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, cost);
            msgd.color1 = TCODColor::lighterBlue;
            msg_log_list.push_back(msgd);
            return 0;
        } else if(GAME.player->AP < cost){
            sprintf(msgd.message, "Not enough Action Points for the action.");
            msg_log_list.push_back(msgd);
            return 0;
        } else {
            sprintf(msgd.message, "Not enough APs, this phase."); // replace with action in progress
            msg_log_list.push_back(msgd);
            return 0;
        }   
    } 
    if(GAME.player->rangeaim >= 2){ // AIM
        if(GAME.player->AP == 0){
            sprintf(msgd.message, "Not enough Action Points for the action.");
            msg_log_list.push_back(msgd);
            return 0;
        } else if(phasemove >= 4){ // shouldn't happen
            sprintf(msgd.message, "Not enough APs, this phase."); 
            msg_log_list.push_back(msgd);
            return 0;
        }  
        --GAME.player->AP;
        ++phasemove;
        GAME.player->rangeaim += 1;
        char formula[8];
        int aimwas = GAME.player->aim;
        switch(GAME.player->rangeaim - 2){ // calculating aim steps
            case 1:
                GAME.player->aim += GAME.player->DEX;
                GAME.player->aim += GAME.player->DEX;
                strcpy(formula, "DEX * 2");
                break;
            case 2:
                GAME.player->aim += GAME.player->DEX;
                strcpy(formula, "DEX");
                break;
        }    
        if((GAME.player->rangeaim - 2) >= 3) { GAME.player->aim += GAME.player->DEX / 2; strcpy(formula, "DEX / 2"); }

        if(GAME.player->ranged_target >= 0 && !monsters[GAME.player->ranged_target].alive)
            GAME.player->ranged_target = -1; // if monster died, revert to point blank
        if(GAME.player->ranged_target == -1){
            sprintf(msgd.message, "%c>%cPlayer targeting point-blank. x(%d) y(%d)", 
                    TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, GAME.player->tlocx, GAME.player->tlocy); 
            msgd.color1 = TCODColor::lighterBlue;
            msg_log_list.push_back(msgd);
        } else {
            char st_tar[20];
            strcpy(st_tar, monsters[GAME.player->ranged_target].name);
            sprintf(msgd.message, "%c>%cPlayer targeting %s. x(%d) y(%d)", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, st_tar,
                    monsters[GAME.player->ranged_target].x, monsters[GAME.player->ranged_target].y); 
            msgd.color1 = TCODColor::lighterBlue;
            msg_log_list.push_back(msgd);
        }

        if(GAME.player->aim > GAME.player->skill.bowML){ 
            GAME.player->aim = GAME.player->skill.bowML; // max aim
            sprintf(msgd.message, "  Player reached maximum accuracy. aim(%d) %c%d%%%c", 
                    GAME.player->rangeaim - 2, TCOD_COLCTRL_1, GAME.player->aim, TCOD_COLCTRL_STOP);
        } else {    
            sprintf(msgd.message, "  Player taking aim(%d) Aim step: (%s) = %d%% -> %c%d%%%c",  
                    GAME.player->rangeaim - 2, formula, aimwas, TCOD_COLCTRL_1, GAME.player->aim, TCOD_COLCTRL_STOP);
        }    
        msgd.color1 = TCODColor::lighterGreen;
        msg_log_list.push_back(msgd);
    }    
    return 0;
}
