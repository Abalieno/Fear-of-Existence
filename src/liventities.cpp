#include <iostream>
#include <windows.h> // for Sleep() and not currently used
#include <stdio.h>
#include <algorithm> // sort

//#include "tilevalues.h"
#include "game.h"
#include "rng.h"
#include "liventities.h"

extern std::vector<msg_log> msg_log_list;
extern std::vector<msg_log_c> msg_log_context;

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
            if(monster.step < 4){ // max in a phase
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
    here.lrange = 1;
    here.hrange = 1;
    here.ploc = 16;
    prec_loc[0].push_back(here);
    here.lrange = 2;
    here.hrange = 4;
    here.ploc = 15;
    prec_loc[0].push_back(here);
    here.lrange = 5;
    here.hrange = 6;
    here.ploc = 14;
    prec_loc[0].push_back(here);

    here.lrange = 1;
    here.hrange = 2;
    here.ploc = 14;
    prec_loc[1].push_back(here);
    here.lrange = 3;
    here.hrange = 5;
    here.ploc = 13;
    prec_loc[1].push_back(here);
    here.lrange = 6;
    here.hrange = 6;
    here.ploc = 12;
    prec_loc[1].push_back(here);

    here.lrange = 1;
    here.hrange = 1;
    here.ploc = 12;
    prec_loc[2].push_back(here);
    here.lrange = 2;
    here.hrange = 3;
    here.ploc = 10;
    prec_loc[2].push_back(here);
    here.lrange = 4;
    here.hrange = 5;
    here.ploc = 9;
    prec_loc[2].push_back(here);
    here.lrange = 6;
    here.hrange = 6;
    here.ploc = 77; // reroll for zone 6 [5]
    prec_loc[2].push_back(here);

    here.lrange = 1;
    here.hrange = 2;
    here.ploc = 4;
    prec_loc[3].push_back(here);
    here.lrange = 3;
    here.hrange = 3;
    here.ploc = 9;
    prec_loc[3].push_back(here);
    here.lrange = 4;
    here.hrange = 4;
    here.ploc = 3;
    prec_loc[3].push_back(here);
    here.lrange = 5;
    here.hrange = 5;
    here.ploc = 2; 
    prec_loc[3].push_back(here);
    here.lrange = 6;
    here.hrange = 6;
    here.ploc = 1; 
    prec_loc[3].push_back(here);

    here.lrange = 1;
    here.hrange = 3;
    here.ploc = 1;
    prec_loc[4].push_back(here);
    here.lrange = 4;
    here.hrange = 4;
    here.ploc = 2;
    prec_loc[4].push_back(here);
    here.lrange = 5;
    here.hrange = 6;
    here.ploc = 4;
    prec_loc[4].push_back(here);

    here.lrange = 1;
    here.hrange = 3;
    here.ploc = 13;
    prec_loc[5].push_back(here);
    here.lrange = 4;
    here.hrange = 4;
    here.ploc = 11;
    prec_loc[5].push_back(here);
    here.lrange = 5;
    here.hrange = 5;
    here.ploc = 10;
    prec_loc[5].push_back(here);
    here.lrange = 6;
    here.hrange = 6;
    here.ploc = 9; 
    prec_loc[5].push_back(here);

    here.lrange = 1;
    here.hrange = 1;
    here.ploc = 8;
    prec_loc[6].push_back(here);
    here.lrange = 2;
    here.hrange = 3;
    here.ploc = 7;
    prec_loc[6].push_back(here);
    here.lrange = 4;
    here.hrange = 4;
    here.ploc = 6;
    prec_loc[6].push_back(here);
    here.lrange = 5;
    here.hrange = 6;
    here.ploc = 5; 
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
    int weapond;
    int *overpower_who;
    int Areach;
    int Dreach;
    int *Adistance;
    int *Ddistance;
    std::vector<std::pair<int,int>> aspects;

    std::vector<std::pair<int,int>> locator; // locations
    build_loc(locator);
    
    std::vector<std::vector <preciseloc>> prec_loc;
    build_preciseloc(prec_loc);

    if(who){ // if MONSTER
        AML = monster.stats.ML; // basic skill
        wpn_AC = monster.stats.wpn1.wpn_AC;
        monsterS = &AML;
        ADB = 0; // monster no skill bonus
        AHP = &monster.stats.hp;
        weapond = monster.stats.power;
        overpower_who = &monster.overpower_l;
        Areach = monster.stats.wpn1.reach;
        Adistance = &monster.distance;
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
    } else{ // if PLAYER
        AML = player.skill.lswdAML; // basic skill
        wpn_AC = player.stats.wpn1.wpn_AC;
        playerS = &AML;
        ADB = player.skill.lswdAB;
        AHP = &player.stats.hp;
        Areach = player.stats.wpn1.reach;
        Adistance = &player.distance;
        DML = monster.stats.ML; // basic monster skill
        wpn_DC = monster.stats.wpn1.wpn_DC;
        monsterS = &DML;
        DDB = 0; // monster no skill bonus
        DHP = &monster.stats.hp;
        Dreach = monster.stats.wpn1.reach;
        weapond = player.stats.power;
        overpower_who = &player.overpower_l;
        Ddistance = &monster.distance;
        aspects.push_back(player.stats.wpn1.wp_B); 
        aspects.push_back(player.stats.wpn1.wp_E);
        aspects.push_back(player.stats.wpn1.wp_P);
    }   

    // crit chances
    int a_crit = AML / 10;
    int d_crit = DML / 10;
    int raw_AML = AML;
    int raw_DML = DML;
    int a_critF = (100 - raw_AML) / 20; // half of index
    if(a_critF < 1) a_critF = 1;
    std::cout << "Attack critF: " << a_critF << std::endl;
    int d_critF = (100 - raw_DML) / 20;
    if(d_critF < 1) d_critF = 1;
    std::cout << "Defense critF: " << d_critF << std::endl;

    // WEAPON COMPARISON TABLE
    if(wpn_AC > wpn_DC)
        AML += 5 * (wpn_AC - wpn_DC);
    if(wpn_DC > wpn_AC)
        DML += 5 * (wpn_DC - wpn_AC);

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
    msgd.color2 = TCODColor::yellow;
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

    // random weapon aspect
    std::pair<int,int> aspect;
    int location = 20; // if 20 = broken
    if(result >= 5 && result <= 7){
        int rnd_asp = dice(1,6);
        std::cout << "Weapon aspect roll: " << rnd_asp << std::endl;
        std::sort(aspects.begin(), aspects.end(), pairCompare);
        std::cout << "Primary: " << aspects[0].first << "," << aspects[0].second << 
            " Secondary: " << aspects[1].first << "," << aspects[1].second << " Tertiary: " << 
            aspects[2].first << "," << aspects[2].second << std::endl;
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
            for (unsigned int n = 0; n < prec_loc[6].size(); ++n){
                if(locroll >= prec_loc[6][n].lrange && locroll <= prec_loc[6][n].hrange) finloc = prec_loc[6][n].ploc; 
            } 
        }    
        std::cout << "Location hit(1-16): " << finloc << "(" << locroll << ")" << std::endl;
    }    

    msg_log msg1;
    switch(result){
        case 1:
            *AHP -= 1;
            *DHP -= 1;
            sprintf(msg1.message, "%c%c%cBoth fumble! %c-1%c to HP for both.", TCOD_COLCTRL_1, whois, TCOD_COLCTRL_STOP, 
                    TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            break;
        case 2:
            *AHP -= 1;
            sprintf(msg1.message, "%c%c%cThe %s fumbles the attack! %c-1%c to HP.", TCOD_COLCTRL_1, whois, TCOD_COLCTRL_STOP, 
                    a_string, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            break;
        case 3:
            *DHP -= 1;
            sprintf(msg1.message, "%c%c%cThe %s misses, but the %s fumbles the parry and hurt himself! %c-1%c to HP.", 
                    TCOD_COLCTRL_1, whois, TCOD_COLCTRL_STOP, a_string, d_string, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            break;
        case 4:
            sprintf(msg1.message, "%c%c%cThe %s swings the weapon, but the %s parries.", TCOD_COLCTRL_1, whois, TCOD_COLCTRL_STOP,
                    a_string, d_string);
            break;
        case 5:
            if(Areach < Dreach) *Ddistance = Areach;
            if(*Adistance < Areach) *Adistance = Areach;
            *DHP -= weapond;
            sprintf(msg1.message, "%c!%cThe %s hits the %s for %c%d%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, 
                    a_string, d_string, TCOD_COLCTRL_1, weapond, TCOD_COLCTRL_STOP);
            break;
        case 6:
            if(Areach < Dreach) *Ddistance = Areach;
            if(*Adistance < Areach) *Adistance = Areach;
            *DHP -= weapond * 2;
            sprintf(msg1.message, "%c!%cCritial attack! %c%d%c damage.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, (weapond * 2), TCOD_COLCTRL_STOP);
            break;
        case 7:
            if(Areach < Dreach) *Ddistance = Areach;
            if(*Adistance < Areach) *Adistance = Areach;
            *DHP -= weapond * 3;
            sprintf(msg1.message, "%c!%cTriple damage! %c%d%c HP.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, (weapond * 3), TCOD_COLCTRL_STOP);
            msg1.color1 = TCODColor::red;
            break;
        case 8:
            sprintf(msg1.message, "%c*%cThe %s misses.", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, a_string);
            break;    
    }
    if(who) msg1.color1 = TCODColor::red;
    else msg1.color1 = TCODColor::lighterBlue;
    msg_log_list.push_back(msg1);
}

void move_obj(int x, int y, std::vector<Generic_object> &wrd_inv){
    //drop_to(x, y);
}    
