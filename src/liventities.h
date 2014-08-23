#ifndef _LIVENTITIES_H_
#define _LIVENTITIES_H_

#include <math.h>
#include <vector>

#include "libtcod.hpp"
#include "game.h"
#include "inventory.h"
#include "armor.h"

class Game;

class AI;

class Object_monster;

class Object_player;

class Weapon_use {
public:
    char wpn_type[15];

    int wpn_AC;
    int wpn_DC;

    int wpn_B; // blunt
    int wpn_E;
    int wpn_P;
    int reach; // 1Hand, 2Short, 3Medium, 4Long, 5Very Long, 6Extremely Long

    int wpn_aspect;

    Weapon_use(){    
    }

};    

// used both on monsters & players
class Fighter {
public:
    int speed; // used for initiative SPD
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

    void attack(Object_player &player, Object_monster &monster, bool who, int overpowering); 

    void take_damage(int damage){
        if (damage > 0) hp -= damage;
    }
};

class Skill {
    public: 
        int condSB;
        int slthSB;
        int thrwSB;
        int condML;
        int slthML;
        int thrwML;

        int dodgSB;
        int dodgML;
        int initSB;
        int initML;

        int lswdASB;
        int lswdDSB;
        int lswdAML;
        int lswdDML;
        int lswdAB;
        int lswdDB;

        int mobiSB;
        int mobiML;
        int uarmSB;
        int uarmML;

        int awarSB;
        int awarML;
        int intrSB;
        int intrML;
        int oratSB;
        int oratML;
        int rhetSB;
        int rhetML;
};    

class Object_monster {

public: // public should be moved down, but I keep it here for debug messages

    char name[20];
    int x;
    int y;
    char selfchar;
    int self_8; // int of the corresponding tile 
    int self_16; // int of the first tile of 4
    TCODColor color;
    TCODColor colorb;
    int h; // temp hit points
    int bloody;
    bool blocks;
    Fighter stats;
    AI * myai;
    bool alive;
    bool chasing; // used to move monster after player for a short while
    int active; // used for wandering mode

    int pl_x; // these are for monster current target/destination
    int pl_y;
    TCODPath *path0;
    TCODPath *path1;
    int path_mode; // 0 = standard 1 = path with monster positions
    int facing; // 0 to 3, N E S W
    int initML;

    int wasstuck; // used for switching path

    bool stuck;
    int bored;
    bool boren; // 100 bored, if to 0, boren true, start recuperating
    bool in_sight;

    bool hit;

    int combat_move;
    int combat_move_max;
    int move_counter; // how many steps moved in the turn
    bool c_mode; // flag monsters for active combat mode
    int speed;
    int initiative;
    int temp_init; // raw unchanged initiative value for messages/list
    unsigned int cflag_attacks; // combat-flag, attacks during turn received
    int overpower_l; // overpower counts
    int distance; // reach rules
    bool hasmoved; // this and the one below used to "phase" monster actions
    int phase_attack; // number of attacks this phase
    int step; // how many points used for movement in same phase
    bool pass; // pass a phase

    int phase; // combat round phase speed
    int att_phase; // weapon speed

    std::vector<Generic_object> inventory;

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
       
    
    void draw(bool uh, Game &tgame); 
        

    void move_towards(int target_x, int target_y){

        bool pdir = false;
        int dx = target_x - x;
        int dy = target_y - y;
        float distance = sqrt (pow(dx, 2) + pow(dy, 2));
        
        dx = (int)(round(dx / distance));
        dy = (int)(round(dy / distance));

        if (dx == 0 && target_x > x) pdir = true;
        if (dy == 0 && target_y > y) pdir = true;

        if ( (dx < 2 && dx > -2) && (dy < 2 && dy > -2) ) // -1 +1 range
            move(dx, dy, pdir);
        //std::cout << "move: " << dx << dy << std::endl;
    }

    float distance_to(int other_x, int other_y){
        int dx = other_x - x;
        int dy = other_y - y;
        return sqrt (pow(dx, 2) + pow(dy, 2));
    }

    void drop(std::vector<Generic_object> &wrd_inv);

    void clear(Game &tgame); 

    ~Object_monster(){} // ?
};

class AI {
public:
     virtual bool take_turn(Object_monster &monster, Object_player &player, int p_x, int p_y, bool myfov, Game &tgame) {return false;} 

};

class Object_player {

public: // public should be moved down, but I keep it here for debug messages

    // new chargen block
    char name[20];
    char *name2;
    int species;
    int sex;
    int bmonth;
    int bday;
    int sunsign;
    int sibrank;
    int famsize;
    int strange;
    int hand;
    int height;
    int frame;
    int weight;
    int size;
    int complexion;
    int hair;
    int eye;
    int comeliness;
    int STR;
    int CON;
    int END;
    int DEX;
    int AGI;
    int PER;
    int VOI;
    int INT;
    int AUR;
    int WIL;
    int MYS;
    int MOR;
    float Load;     
    int enc_pen; // encumberance penalty

    int x;
    int y;
    char selfchar;
    TCODColor color;
    TCODColor colorb;
    int h; // temp hit points
    int bloody;
    bool blocks;
    Fighter stats;
    Skill skill;

    int combat_move;
    int combat_move_max;
    int move_counter; // how many steps moved in the turn
    int speed;
    int initiative;
    unsigned int player_own; // used to build initiative UI
    int temp_init; // total initiative value for messages/list
    unsigned int cflag_attacks; // combat-flag - number of attacks received in current turn
    int overpower_l; // overpower counts
    int distance; // reach rules
    int phase_attack; // number of attacks this phase

    int facing; // 0 to 3, N E S W
    int phase;
    int att_phase; // weapon speed

    std::vector<Armor> armor_worn;

    std::vector<Generic_object> inventory;

    Object_player(int a, int b, char pchar, TCODColor oc, TCODColor oc2, int health, Fighter loc_fighter) : stats(loc_fighter) {
        x = a;
        y = b;
        selfchar = pchar;
        color = oc;
        colorb = oc2;
        h = health;
        bloody = 0;
        combat_move_max = 8;
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

    void draw(bool uh, Game &tgame); 

    void move_obj(int x, int y, std::vector<Generic_object> &wrd_inv);

    void clear(Game &tgame);

    ~Object_player(){} // ?
};

class BasicMonster : public AI {
public:

    BasicMonster(){

    }

    virtual bool take_turn(Object_monster &monster, Object_player &player, int p_x, int p_y, bool myfov, Game &tgame);

};


bool is_overpower(int askill, int aroll, int dskill, int droll);




#endif
