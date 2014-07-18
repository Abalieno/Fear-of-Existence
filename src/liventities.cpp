#include <iostream>
#include <windows.h> // for Sleep() and not currently used
#include <stdio.h>

//#include "tilevalues.h"
#include "game.h"
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

bool BasicMonster::take_turn(Object_monster &monster, Object_player &player, int p_x, int p_y, bool myfov, Game &tgame){
            //float dist = 0;
            //std::cout << monster.combat_move << std::endl; 
   
            std::cout << "The " << monster.name << " is active! " << std::endl;
            //dist = monster.distance_to(p_x, p_y);

            // 1.1 so the monster attacks, but still moves closer instead of stopping diagonally
            if ( (monster.distance_to(p_x, p_y) >= 1.1) || (monster.chasing && !myfov)){

                if (tgame.gstate.no_combat || monster.combat_move >= 1){ // move up to and including player pos    
                    monster.move_towards(p_x, p_y);
                    if(!tgame.gstate.no_combat){
                        monster.combat_move -= 1;
                    }    
                    std::cout << "The " << monster.name << " is moving." << std::endl;
                    return false;
                }

            } else if (myfov){ 
                if (tgame.gstate.no_combat || (monster.combat_move >= 4)) {
                //TCODConsole::root->clear();
                
                monster.path_mode = 0;

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
                tgame.gstate.mesg->print(1, 1, "Hit!");
                //if(!(player.y > MAP_HEIGHT-8 )) TCODConsole::blit(mesg,0,0,33,3,con,1,MAP_HEIGHT-4);
                //else TCODConsole::blit(mesg,0,0,33,3,con,MAP_WIDTH-37,1);

                if(tgame.gstate.bigg){
                    TCODConsole::blit(tgame.gstate.con, 0, 0, 0, 0, TCODConsole::root,0,0);
                    //TCODConsole::blit(tgame.gstate.con,tgame.gstate.offbig_x,tgame.gstate.offbig_y+2,110,68,TCODConsole::root,0,2);
                } else {    
                    TCODConsole::blit(tgame.gstate.con, 0, 0, 0, 0, TCODConsole::root,0,0);
                    //TCODConsole::blit(tgame.gstate.con,tgame.gstate.off_xx,tgame.gstate.off_yy+1,110,69,TCODConsole::root,0,1);
                }
                //std::cout << "SMALLOFF: " << off_xx << " " << off_yy << std::endl;

                TCODConsole::flush();

                Sleep(250); // shitty way for attack "animation", uses windows.h
                player.colorb = tgame.gstate.color_dark_ground;
                monster.colorb = tgame.gstate.color_dark_ground;
                player.selfchar = '@';
                player.draw(0, tgame);
                monster.draw(0, tgame);
                tgame.gstate.con->clear();
                tgame.gstate.fov_recompute = true;
                //render_all();
                
                monster.stats.attack(player, monster, 1); // calls attack function for monsters
                TCODConsole::flush();
                if(!tgame.gstate.no_combat)monster.combat_move -= 4; // decrease the movement points for attack
                return true;
            } else if (myfov && !tgame.gstate.no_combat) monster.combat_move = 0; // movement points to 0 if couldn't make the attack
            }
        
        //else std::cout << "The " << monster.name << " lurks in the dark! " ;
        return false; 
    }

void Fighter::attack(Object_player &player, Object_monster &monster, bool who){




    int damage = 0;

    if (who){       

        // calculate AML
        int a_AML = monster.stats.ML; // basic skill
        a_AML += monster.stats.wpn1.wpn_AC; // adding weapon Attack Class
        // should check for walls here
        int d_DML = player.skill.lswdML; // basic monster skill
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

void move_obj(int x, int y, std::vector<Generic_object> &wrd_inv){
    //drop_to(x, y);
}    
