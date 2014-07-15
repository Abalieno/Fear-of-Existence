#include <iostream> // debug
#include <algorithm>    // std::sort
#include <math.h> // for rounding

#include "chargen.h"
#include "rng.h"
#include "debug.h"
#include "gui.h"

extern int win_y;
extern int win_x;

void gen_species(Game &GAME){
    int roll = 0;
    roll = rng(1, 100);
    if(roll > 0 && roll < 91) GAME.player->species = Human;
    if(roll > 90 && roll < 96) GAME.player->species = Sindarin;
    if(roll > 95 && roll < 101) GAME.player->species = Khuzdul;
    return;
}
void gen_sex(Game &GAME){
    int roll = 0;
    roll = rng(1, 100);
    switch(GAME.player->species){
        case Human:
            if(roll > 0 && roll < 49) GAME.player->sex = Male;
            else GAME.player->sex = Female;
            break;
        case Sindarin:
            if(roll > 0 && roll < 46) GAME.player->sex = Male;
            else GAME.player->sex = Female;
            break;
        case Khuzdul:
            if(roll > 0 && roll < 76) GAME.player->sex = Male;
            else GAME.player->sex = Female;
            break;
    }    
    return;
}

void gen_name(Game &GAME){
    static int nbSets = 0; 
    static int curSet = 0;
    static TCODList<char *> sets;
    if ( nbSets == 0 ) {
        TCODList<char *> files=TCODSystem::getDirectoryContent("data/namegen","*.cfg");
        for (char **it=files.begin(); it != files.end(); it++) {
            char tmp[256];
            sprintf(tmp, "data/namegen/%s",*it);
            TCODNamegen::parse(tmp);
        }
        // get the sets list
		sets = TCODNamegen::getSets();
		nbSets = sets.size();
    }
    if(GAME.player->sex == Male){
        do{ 
            curSet = rng(0,15);
        } while (curSet % 2 != 0);
    } else {
        do curSet = rng(0,15);
        while (curSet % 2 == 0);
    } 
    if(GAME.player->species == Sindarin){
        if(GAME.player->sex == Male){
            do{ 
            curSet = rng(0,15);
        } while (curSet % 2 != 0 || curSet >= 5);
        } else {
            do{ 
            curSet = rng(0,15);
        } while (curSet % 2 == 0 || curSet >= 5);
        }    
    } else if(GAME.player->species == Khuzdul){
        if(GAME.player->sex == Male){
            curSet = rng(0,1);
            if(curSet == 0) curSet = 8;
            else curSet = 10;
        } else {
            curSet = rng(0,1);
            if(curSet == 0) curSet = 9;
            else curSet = 11;
        }
    }  
    std::cout << "Name set n: " << curSet << std::endl;
    delete GAME.player->name2;
    GAME.player->name2 = TCODNamegen::generate(sets.get(curSet),true);
    std::cout << "Name set: " << sets.get(curSet) << std::endl;
    //strcpy(GAME.player->name, "Placeholderguy");
    return;
}

void gen_birthdate(Game &GAME){
    int roll1 = 0;
    roll1 = rng(1, 12);
    switch(roll1){
        case 1:
            GAME.player->bmonth = Nuzyael;
            break;
        case 2:
            GAME.player->bmonth = Peonu;
            break;
        case 3:
            GAME.player->bmonth = Kelen;
            break;
        case 4:
            GAME.player->bmonth = Nolus;
            break;    
        case 5:
            GAME.player->bmonth = Larane;
            break;
        case 6:
            GAME.player->bmonth = Agrazhar;
            break;
        case 7:
            GAME.player->bmonth = Azura;
            break;
        case 8:
            GAME.player->bmonth = Halane;
            break;
        case 9:
            GAME.player->bmonth = Savor;
            break;
        case 10:
            GAME.player->bmonth = Ilvin;
            break;
        case 11:
            GAME.player->bmonth = Navek;
            break;
        case 12:
            GAME.player->bmonth = Morgat;
            break;
    }    
    int roll2 = 0;
    roll2 = rng(1, 30);
    GAME.player->bday = roll2;
    return;
}
void gen_sunsign(Game &GAME){
    int general = 0;
    general = (30*GAME.player->bmonth) + GAME.player->bday;
    if(general >  3 && general < 34) GAME.player->sunsign = Ulandus;
    else if(general > 33 && general < 63) GAME.player->sunsign = Aralius;
    else if(general > 62 && general < 94) GAME.player->sunsign = Feniri;
    else if(general > 93 && general < 125) GAME.player->sunsign = Ahnu;
    else if(general > 124 && general < 157) GAME.player->sunsign = Angberelius;
    else if(general > 156 && general < 186) GAME.player->sunsign = Nadai;
    else if(general > 185 && general < 215) GAME.player->sunsign = Hirin;
    else if(general > 214 && general < 244) GAME.player->sunsign = Tarael;
    else if(general > 243 && general < 273) GAME.player->sunsign = Tai;
    else if(general > 272 && general < 303) GAME.player->sunsign = Skorus;
    else if(general > 302 && general < 332) GAME.player->sunsign = Masara;
    else if( (general > 331 && general < 361) || (general > 0 && general < 4) ) GAME.player->sunsign = Lado;
    else debugmsg("Sunsign generation broken.");
    return;
}
void gen_sibrank(Game &GAME){
    int famsize = 0;
    switch(GAME.player->species){
        case Human:
            famsize = dice(6,2) - 5;
            if(famsize == 7){
                int roll = 0;
                while(rng(1,2) == 2){
                    roll = rng(1,2);
                    famsize += roll;
                } 
            }    
            break;
        case Sindarin:
            famsize = dice(3,2) - 2;
            if(famsize == 4){
                int roll = 0;
                while(rng(1,2) == 2){
                    roll = rng(1,2);
                    famsize += roll;
                } 
            }
            break;
        case Khuzdul:
            famsize = dice(3,2) - 2;
            if(famsize == 4){
                int roll = 0;
                while(rng(1,2) == 2){
                    roll = rng(1,2);
                    famsize += roll;
                }
            }
            break;
    }  
    GAME.player->famsize = famsize;
    GAME.player->sibrank = rng(1,famsize);
    return;
}
void gen_strange(Game &GAME){
    int roll = 0;
    roll = rng(1,100);
    if(roll <= 10) GAME.player->strange = Outcast; 
    if(roll >= 11 && roll <= 40) GAME.player->strange = Unpopular;
    if(roll >= 41 && roll <= 60) GAME.player->strange = Average;
    if(roll >= 61 && roll <= 95) GAME.player->strange = Popular;
    if(roll >= 96 && roll <= 100) GAME.player->strange = Favorite;
    return;
}
void gen_hand(Game &GAME){
    int roll = 0;
    roll = rng(1,100);
    if(GAME.player->sex == Female) roll += 5;
    if(GAME.player->species == Sindarin) roll += 10;
    if(GAME.player->species == Khuzdul) roll += 10;
    if(roll <= 85) GAME.player->hand = Righthanded;
    else if(roll >= 86 && roll <= 95) GAME.player->hand = Lefthanded;
    else if(roll >= 96) GAME.player->hand = Ambidextrous;
    return;
}
void gen_height(Game &GAME){
    int roll = 0;
    if(GAME.player->species == Human){ 
        roll = rng(5,30);
        if(GAME.player->sex == Male) roll += 50;
        else roll += 48;
    } else {
        roll = rng(4,24);
        if(GAME.player->species == Sindarin){
            if(GAME.player->sex == Male) roll += 51;
            else roll += 50;
        } else if(GAME.player->species == Khuzdul) roll += 40;
    }    
    GAME.player->height = roll;
    return;
}
void gen_frame(Game &GAME){
    int roll = 0;
    roll = dice(3,6);
    if(GAME.player->sex == Female && GAME.player->species == Human) roll -= 3;
    if(GAME.player->species == Sindarin) roll -= 2;
    if(GAME.player->species == Khuzdul) roll += 4;

    if(roll <= 5) GAME.player->frame = Scant;
    else if(roll >= 6 && roll <= 8) GAME.player->frame = Light;
    else if(roll >= 9 && roll <= 12) GAME.player->frame = Medium;
    else if(roll >= 13 && roll <= 15) GAME.player->frame = Heavy;
    else if(roll >= 16) GAME.player->frame = Massive;
    return;
}
void gen_weight(Game &GAME){
    if(GAME.player->height >= 40 && GAME.player->height <= 51) GAME.player->weight = 75 + ((GAME.player->height - 40)*2);
    if(GAME.player->height >= 52 && GAME.player->height <= 63) GAME.player->weight = 100 + ((GAME.player->height - 52)*3);
    if(GAME.player->height >= 64 && GAME.player->height <= 69) GAME.player->weight = 137 + ((GAME.player->height - 64)*4);
    if(GAME.player->height >= 70) GAME.player->weight = 160 + ((GAME.player->height - 70)*5);

    if(GAME.player->frame == Scant) GAME.player->weight -= (GAME.player->weight / 100) * 20;
    else if(GAME.player->frame == Light) GAME.player->weight -= (GAME.player->weight / 100) * 10;
    else if(GAME.player->frame == Heavy) GAME.player->weight += (GAME.player->weight / 100) * 10;
    else if(GAME.player->frame == Massive) GAME.player->weight += (GAME.player->weight / 100) * 20;

    if(GAME.player->weight >= 61 && GAME.player->weight <= 65) GAME.player->size = 1;
    else if(GAME.player->weight >= 66 && GAME.player->weight <= 75) GAME.player->size = 2;
    else if(GAME.player->weight >= 76 && GAME.player->weight <= 90) GAME.player->size = 3;
    else if(GAME.player->weight >= 91 && GAME.player->weight <= 110) GAME.player->size = 4;
    else if(GAME.player->weight >= 111 && GAME.player->weight <= 135) GAME.player->size = 5;
    else if(GAME.player->weight >= 136 && GAME.player->weight <= 165) GAME.player->size = 6;
    else if(GAME.player->weight >= 166 && GAME.player->weight <= 200) GAME.player->size = 7;
    else if(GAME.player->weight >= 201 && GAME.player->weight <= 240) GAME.player->size = 8;
    else if(GAME.player->weight >= 241 && GAME.player->weight <= 285) GAME.player->size = 9;
    else if(GAME.player->weight >= 286 && GAME.player->weight <= 335) GAME.player->size = 10;
    return;
}

void gen_complexion(Game &GAME){
    int roll = 0;
    roll = rng(1,100);
    if(GAME.player->species == Human){
        if(roll <= 6) GAME.player->complexion = CPallid;
        else if (roll >= 7 && roll <= 94) GAME.player->complexion = CFair;
        else if (roll >= 95 && roll <= 99) GAME.player->complexion = CMedium;
        else if (roll >= 100) GAME.player->complexion = CDark;
    } else if(GAME.player->species == Sindarin){
        if (roll >= 1 && roll <= 97) GAME.player->complexion = CFair;
        else if (roll >= 98 && roll <= 99) GAME.player->complexion = CMedium;
        else if (roll >= 100) GAME.player->complexion = CDark;
    } else if(GAME.player->species == Khuzdul){
        if(roll <= 73) GAME.player->complexion = CPallid;
        else if (roll >= 74 && roll <= 90) GAME.player->complexion = CFair;
        else if (roll >= 91 && roll <= 99) GAME.player->complexion = CMedium;
        else if (roll >= 100) GAME.player->complexion = CDark;
    }    
    return;
}
void gen_hair(Game &GAME){
    int roll = 0;
    roll = rng(1,100);
    std::cout << "Hair roll: " << roll << std::endl;
    GAME.player->hair = HNull;
    if(GAME.player->species == Human && (GAME.player->complexion == CFair || GAME.player->complexion == CPallid) ){
        std::cout << "Hair roll: " << roll << std::endl;
        if (roll <= 5) GAME.player->hair = HBlack;
        else if (roll >= 6 && roll <= 40) GAME.player->hair = HBrown;
        else if (roll >= 41 && roll <= 45) {std::cout << "WTF" << std::endl; GAME.player->hair = HRed;}
        else if (roll >= 46 && roll <= 55) GAME.player->hair = HAuburn;
        else if (roll >= 56 && roll <= 70) GAME.player->hair = HDarkblonde;
        else if (roll >= 71 && roll <= 95) GAME.player->hair = HBlonde;
        else if (roll >= 96 && roll <= 100) GAME.player->hair = HGreywhite;
        std::cout << "Hair roll: " << roll << std::endl;
        std::cout << "Hair into: " << GAME.player->hair << std::endl;
    } else if(GAME.player->species == Human && GAME.player->complexion == CMedium){ 
        if (roll <= 15) GAME.player->hair = HBlack;
        else if (roll >= 16 && roll <= 55) GAME.player->hair = HBrown;
        else if (roll >= 56 && roll <= 58) GAME.player->hair = HRed;
        else if (roll >= 59 && roll <= 65) GAME.player->hair = HAuburn;
        else if (roll >= 66 && roll <= 80) GAME.player->hair = HDarkblonde;
        else if (roll >= 81 && roll <= 95) GAME.player->hair = HBlonde;
        else if (roll >= 96 && roll <= 100) GAME.player->hair = HGreywhite;
    } else if(GAME.player->species == Human && GAME.player->complexion == CDark){
        if (roll <= 45) GAME.player->hair = HBlack;
        else if (roll >= 46 && roll <= 85) GAME.player->hair = HBrown;
        else if (roll == 86) GAME.player->hair = HRed;
        else if (roll >= 87 && roll <= 90) GAME.player->hair = HAuburn;
        else if (roll >= 91 && roll <= 93) GAME.player->hair = HDarkblonde;
        else if (roll >= 94 && roll <= 95) GAME.player->hair = HBlonde;
        else if (roll >= 96 && roll <= 100) GAME.player->hair = HGreywhite;
    } else if(GAME.player->species == Human && GAME.player->complexion == CBlack){
        if (roll <= 65) GAME.player->hair = HBlack;
        else if (roll >= 66 && roll <= 95) GAME.player->hair = HBrown;
        else if (roll == 96) GAME.player->hair = HAuburn;
        else if (roll == 97) GAME.player->hair = HDarkblonde;
        else if (roll == 98) GAME.player->hair = HBlonde;
        else if (roll >= 99 && roll <= 100) GAME.player->hair = HGreywhite;
    } else if(GAME.player->species == Sindarin){
        if (roll <= 25) GAME.player->hair = HBlack;
        else if (roll >= 26 && roll <= 48) GAME.player->hair = HBrown;
        else if (roll >= 49 && roll <= 50) GAME.player->hair = HAuburn;
        else if (roll >= 51 && roll <= 65) GAME.player->hair = HDarkblonde;
        else if (roll >= 66 && roll <= 90) GAME.player->hair = HBlonde;
        else if (roll >= 91 && roll <= 100) GAME.player->hair = HGreywhite;
    } else if(GAME.player->species == Khuzdul){
        if (roll <= 5) GAME.player->hair = HBlack;
        else if (roll >= 6 && roll <= 45) GAME.player->hair = HBrown;
        else if (roll >= 46 && roll <= 55) GAME.player->hair = HRed;
        else if (roll >= 56 && roll <= 70) GAME.player->hair = HAuburn;
        else if (roll >= 71 && roll <= 75) GAME.player->hair = HDarkblonde;
        else if (roll >= 76 && roll <= 80) GAME.player->hair = HBlonde;
        else if (roll >= 81 && roll <= 100) GAME.player->hair = HGreywhite;
    } else {
        debugmsg("Hair broken!");
    }    
    return;
}
void gen_eye(Game &GAME){
    int roll = 0;
    roll = rng(1,100);
    if(GAME.player->species == Sindarin){
        if (roll <= 6) GAME.player->eye = EBrown;
        else if (roll >= 7 && roll <= 15) GAME.player->eye = EHazel;
        else if (roll >= 16 && roll <= 35) GAME.player->eye = EBlue;
        else if (roll >= 36 && roll <= 85) GAME.player->eye = EGrey;
        else if (roll >= 86 && roll <= 90) GAME.player->eye = EGreen;
        else if (roll >= 91 && roll <= 100) GAME.player->eye = EViolet;
    } else if(GAME.player->complexion == CMedium){
        if (roll <= 45) GAME.player->eye = EBrown;
        else if (roll >= 46 && roll <= 55) GAME.player->eye = EHazel;
        else if (roll >= 56 && roll <= 75) GAME.player->eye = EBlue;
        else if (roll >= 76 && roll <= 95) GAME.player->eye = EGrey;
        else if (roll >= 96 && roll <= 99) GAME.player->eye = EGreen;
        else if (roll == 100) GAME.player->eye = EViolet;
    } else if(GAME.player->complexion == CFair || GAME.player->complexion == CPallid){
        if (roll <= 5) GAME.player->eye = EBrown;
        else if (roll >= 6 && roll <= 10) GAME.player->eye = EHazel;
        else if (roll >= 11 && roll <= 50) GAME.player->eye = EBlue;
        else if (roll >= 51 && roll <= 85) GAME.player->eye = EGrey;
        else if (roll >= 86 && roll <= 99) GAME.player->eye = EGreen;
        else if (roll == 100) GAME.player->eye = EViolet;
    } else if(GAME.player->complexion == CDark || GAME.player->complexion == CBlack){
        if (roll <= 80) GAME.player->eye = EBrown;
        else if (roll >= 81 && roll <= 90) GAME.player->eye = EHazel;
        else if (roll >= 91 && roll <= 95) GAME.player->eye = EBlue;
        else if (roll >= 96 && roll <= 98) GAME.player->eye = EGrey;
        else if (roll == 99) GAME.player->eye = EGreen;
        else if (roll == 100) GAME.player->eye = EViolet;
    }  
    return;
}

void gen_comeliness(Game &GAME){
    int roll = 0;
    roll = dice(3,6);
    if(GAME.player->species == Sindarin) roll += 2;
    std::cout << "comeliness roll: " << roll << std::endl;
    if(roll <= 5) GAME.player->comeliness = Ugly;
    else if(roll >= 6 && roll <= 8) GAME.player->comeliness = Plain;
    else if(roll >= 9 && roll <= 12) GAME.player->comeliness = BAverage;
    else if(roll >= 13 && roll <= 15) GAME.player->comeliness = Attractive;
    else if(roll >= 16) GAME.player->comeliness = Handsome;
}   

void gen_STR(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::cout << "str1 roll: " << dice1 << std::endl;
    std::cout << "str2 roll: " << dice2 << std::endl;
    std::cout << "str3 roll: " << dice3 << std::endl;
    std::cout << "str4 roll: " << dice4 << std::endl;
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4); // use 4th dice only if we not use points
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->STR = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Khuzdul) GAME.player->STR += 4;
    else if(GAME.player->species == Sindarin) GAME.player->STR += 1;

    if(GAME.player->weight <= 20) GAME.player->STR -= 6;
    else if(GAME.player->weight >= 21 && GAME.player->weight <= 55) GAME.player->STR -= 5;
    else if(GAME.player->weight >= 56 && GAME.player->weight <= 85) GAME.player->STR -= 4;
    else if(GAME.player->weight >= 86 && GAME.player->weight <= 110) GAME.player->STR -= 3;
    else if(GAME.player->weight >= 111 && GAME.player->weight <= 130) GAME.player->STR -= 2;
    else if(GAME.player->weight >= 131 && GAME.player->weight <= 145) GAME.player->STR -= 1;
    else if(GAME.player->weight >= 156 && GAME.player->weight <= 170) GAME.player->STR += 1;
    else if(GAME.player->weight >= 171 && GAME.player->weight <= 190) GAME.player->STR += 2;
    else if(GAME.player->weight >= 191 && GAME.player->weight <= 215) GAME.player->STR += 3;
    else if(GAME.player->weight >= 216 && GAME.player->weight <= 245) GAME.player->STR += 4;
    else if(GAME.player->weight >= 246 && GAME.player->weight <= 280) GAME.player->STR += 5;
    else if(GAME.player->weight >= 281 && GAME.player->weight <= 320) GAME.player->STR += 6;
    else if(GAME.player->weight >= 321 && GAME.player->weight <= 375) GAME.player->STR += 7;
    else if(GAME.player->weight >= 366 && GAME.player->weight <= 415) GAME.player->STR += 8;
}    

void gen_END(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    int dice5 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    dice5 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    vdice.push_back(dice4);
    if(!method) vdice.push_back(dice5);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->END = vdice[2] + vdice[3] + vdice [4];

    if(GAME.player->species == Khuzdul) GAME.player->END += 2;
}

void gen_DEX(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->DEX = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Khuzdul) GAME.player->DEX += 1;
    else if(GAME.player->species == Sindarin) GAME.player->DEX += 2;
    if(GAME.player->hand == Ambidextrous) GAME.player->DEX += 2;
    else if(GAME.player->hand == Lefthanded) GAME.player->DEX += 1;
}

void gen_AGI(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->AGI = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Sindarin) GAME.player->AGI += 2;
    if(GAME.player->frame == Scant) GAME.player->AGI += 2;
    else if(GAME.player->frame == Light) GAME.player->AGI += 1;
    else if(GAME.player->frame == Heavy) GAME.player->AGI -= 1;
    else if(GAME.player->frame == Massive) GAME.player->AGI -= 2;
}

void gen_SPD(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->SPD = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Human && GAME.player->sex == Female) GAME.player->SPD -= 2;
    if(GAME.player->SPD < (GAME.player->AGI - 4) ) GAME.player->SPD = (GAME.player->AGI - 4);
}

void gen_EYE(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->EYE = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Sindarin) GAME.player->EYE += 3;
}

void gen_HEA(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->HEA = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Sindarin) GAME.player->HEA += 3;
    else if(GAME.player->species == Khuzdul) GAME.player->HEA += 2;
}

void gen_SMT(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->SMT = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Sindarin) GAME.player->SMT += 3;
    else if(GAME.player->species == Human && GAME.player->sex == Female) GAME.player->SMT += 1;
}

void gen_TCH(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->TCH = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Sindarin) GAME.player->TCH += 2;
    else if(GAME.player->species == Khuzdul) GAME.player->TCH += 1;
}

void gen_VOI(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->VOI = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Sindarin) GAME.player->VOI += 3;
}

void gen_INT(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->INT = vdice[1] + vdice[2] + vdice [3];
}

void gen_AUR(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->AUR = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Sindarin) GAME.player->AUR += 4;
    else if(GAME.player->species == Khuzdul) GAME.player->AUR -= 2;
    else if(GAME.player->species == Human && GAME.player->sex == Female) GAME.player->AUR -= 2;
}

void gen_WIL(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    int dice4 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    dice4 = dice(1,6);
    std::vector<int> vdice;
    vdice.push_back(dice1);
    vdice.push_back(dice2);
    vdice.push_back(dice3);
    if(!method) vdice.push_back(dice4);
    else vdice.push_back(0);
    std::sort(vdice.begin(), vdice.end());
    GAME.player->WIL = vdice[1] + vdice[2] + vdice [3];

    if(GAME.player->species == Khuzdul) GAME.player->WIL += 3;
}

void gen_MOR(Game &GAME, bool method){
    int dice1 = 0;
    int dice2 = 0;
    int dice3 = 0;
    dice1 = dice(1,6);
    dice2 = dice(1,6);
    dice3 = dice(1,6);
    GAME.player->MOR = dice1 + dice2 + dice3;

    if(GAME.player->species == Sindarin) GAME.player->MOR += 4;
}

int skillSB(int a, int b, int c){
    int x = a + b + c;
    return round(x/3.0);
}    

void gen_rollskill(Game &GAME){
    GAME.player->skill.condSB = skillSB(GAME.player->END, GAME.player->END, GAME.player->WIL); 
    GAME.player->skill.slthSB = skillSB(GAME.player->AGI, GAME.player->TCH, GAME.player->TCH);
    GAME.player->skill.thrwSB = skillSB(GAME.player->STR, GAME.player->DEX, GAME.player->EYE);
    GAME.player->skill.condML = GAME.player->skill.condSB * 5;
    GAME.player->skill.slthML = GAME.player->skill.slthSB * 3;
    GAME.player->skill.thrwML = GAME.player->skill.thrwSB * 4;

    GAME.player->skill.dodgSB = skillSB(GAME.player->AGI, GAME.player->AGI, GAME.player->SPD);
    GAME.player->skill.initSB = skillSB(GAME.player->AGI, GAME.player->SPD, GAME.player->WIL);
    GAME.player->skill.lswdSB = skillSB(GAME.player->STR, GAME.player->STR, GAME.player->DEX);
    GAME.player->skill.mobiSB = skillSB(GAME.player->AGI, GAME.player->SPD, GAME.player->SPD);
    GAME.player->skill.uarmSB = skillSB(GAME.player->STR, GAME.player->DEX, GAME.player->AGI);
    GAME.player->skill.dodgML = GAME.player->skill.dodgSB * 5;
    GAME.player->skill.initML = GAME.player->skill.initSB * 5;
    GAME.player->skill.lswdML = GAME.player->skill.lswdSB * 3;
    GAME.player->skill.mobiML = GAME.player->skill.mobiSB * 5;
    GAME.player->skill.uarmML = GAME.player->skill.uarmSB * 3;

    GAME.player->skill.awarSB = skillSB(GAME.player->EYE, GAME.player->HEA, GAME.player->SMT);
    GAME.player->skill.intrSB = skillSB(GAME.player->INT, GAME.player->AUR, GAME.player->WIL);
    GAME.player->skill.oratSB = skillSB(GAME.player->comeliness, GAME.player->VOI, GAME.player->INT);
    GAME.player->skill.rhetSB = skillSB(GAME.player->VOI, GAME.player->INT, GAME.player->WIL);
    GAME.player->skill.awarML = GAME.player->skill.awarSB * 4;
    GAME.player->skill.intrML = GAME.player->skill.intrSB * 3;
    GAME.player->skill.oratML = GAME.player->skill.oratSB * 2;
    GAME.player->skill.rhetML = GAME.player->skill.rhetSB * 3;
}

void draw_frame(const char *title1, const char *title2){
    for (int n = 0; n < win_y; ++n){
        TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
        TCODConsole::root->setDefaultBackground(TCODColor::black);
        TCODConsole::root->print(0, n, "%c", TCOD_CHAR_VLINE);
        TCODConsole::root->print(win_x-1, n, "%c", TCOD_CHAR_VLINE);
    }
    for (int n = 0; n < win_x; ++n){
        TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
        TCODConsole::root->setDefaultBackground(TCODColor::black);
        TCODConsole::root->print(n, 0, "%c", TCOD_CHAR_HLINE);
        TCODConsole::root->print(n, win_y-1, "%c", TCOD_CHAR_HLINE);
    }
    TCODConsole::root->print(0, 0, "%c", TCOD_CHAR_NW);
    TCODConsole::root->print(win_x-1, 0, "%c", TCOD_CHAR_NE);
    TCODConsole::root->print(0, win_y-1, "%c", TCOD_CHAR_SW);
    TCODConsole::root->print(win_x-1, win_y-1, "%c", TCOD_CHAR_SE);

    TCODConsole::root->setAlignment(TCOD_CENTER);
    TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);
    TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::white, TCODColor::black);
    TCODConsole::root->print(win_x/2, 0, "[ %c%s%c - %c%s%c ]", TCOD_COLCTRL_1, title1, TCOD_COLCTRL_STOP,
            TCOD_COLCTRL_2, title2, TCOD_COLCTRL_STOP);

}

void txt_sex(char* here, Game &GAME){
    if(GAME.player->sex == 0) strcpy(here, "Female");
    else strcpy(here, "Male");
    return;
}  
void txt_species(char* here, Game &GAME){
    if(GAME.player->species == 0) strcpy(here, "Human");
    else if(GAME.player->species == 1) strcpy(here, "Sindarin");
    else if(GAME.player->species == 2) strcpy(here, "Khuzdul");
    return;
}
void txt_birthdate(char* here, Game &GAME){
    if(GAME.player->bmonth == 0) strcpy(here, "Nuzyael");
    else if(GAME.player->bmonth == 1) strcpy(here, "Peonu");
    else if(GAME.player->bmonth == 2) strcpy(here, "Kelen");
    else if(GAME.player->bmonth == 3) strcpy(here, "Nolus");
    else if(GAME.player->bmonth == 4) strcpy(here, "Larane");
    else if(GAME.player->bmonth == 5) strcpy(here, "Agrazhar");
    else if(GAME.player->bmonth == 6) strcpy(here, "Azura");
    else if(GAME.player->bmonth == 7) strcpy(here, "Halane");
    else if(GAME.player->bmonth == 8) strcpy(here, "Savor");
    else if(GAME.player->bmonth == 9) strcpy(here, "Ilvin");
    else if(GAME.player->bmonth == 10) strcpy(here, "Navek");
    else if(GAME.player->bmonth == 11) strcpy(here, "Morgat");
    return;
}
void txt_sunsign(char* here, Game &GAME){
    if(GAME.player->sunsign == 0) strcpy(here, "Ulandus");
    else if(GAME.player->sunsign == 1) strcpy(here, "Aralius");
    else if(GAME.player->sunsign == 2) strcpy(here, "Feniri");
    else if(GAME.player->sunsign == 3) strcpy(here, "Ahnu");
    else if(GAME.player->sunsign == 4) strcpy(here, "Angberelius");
    else if(GAME.player->sunsign == 5) strcpy(here, "Nadai");
    else if(GAME.player->sunsign == 6) strcpy(here, "Hirin");
    else if(GAME.player->sunsign == 7) strcpy(here, "Tarael");
    else if(GAME.player->sunsign == 8) strcpy(here, "Tai");
    else if(GAME.player->sunsign == 9) strcpy(here, "Skorus");
    else if(GAME.player->sunsign == 10) strcpy(here, "Masara");
    else if(GAME.player->sunsign == 11) strcpy(here, "Lado");
    return;
}

void txt_enstrangement(char* here, Game &GAME){
    if(GAME.player->strange == 0) strcpy(here, "Outcast");
    else if(GAME.player->strange == 1) strcpy(here, "Unpopular");
    else if(GAME.player->strange == 2) strcpy(here, "Average");
    else if(GAME.player->strange == 3) strcpy(here, "Popular");
    else if(GAME.player->strange == 4) strcpy(here, "Favorite");
    return;
}

void txt_handedness(char* here, Game &GAME){
    if(GAME.player->hand == 0) strcpy(here, "Right Handed");
    else if(GAME.player->hand == 1) strcpy(here, "Left Handed");
    else if(GAME.player->hand == 2) strcpy(here, "Ambidextrous");
    return;
}

void txt_frame(char* here, Game &GAME){
    if(GAME.player->frame == 0) strcpy(here, "Scant");
    else if(GAME.player->frame == 1) strcpy(here, "Light");
    else if(GAME.player->frame == 2) strcpy(here, "Medium");
    else if(GAME.player->frame == 3) strcpy(here, "Heavy");
    else if(GAME.player->frame == 4) strcpy(here, "Massive");
    return;
}

void txt_complexion(char* here, Game &GAME){
    if(GAME.player->complexion == 0) strcpy(here, "Pallid");
    else if(GAME.player->complexion == 1) strcpy(here, "Fair");
    else if(GAME.player->complexion == 2) strcpy(here, "Medium");
    else if(GAME.player->complexion == 3) strcpy(here, "Dark");
    else if(GAME.player->complexion == 4) strcpy(here, "Black");
    return;
}

void txt_hair(char* here, Game &GAME){
    std::cout << "value in hair: " << GAME.player->hair << std::endl;
    if(GAME.player->hair == 0) strcpy(here, "Black");
    else if(GAME.player->hair == 1) strcpy(here, "Brown");
    else if(GAME.player->hair == 2) strcpy(here, "Red");
    else if(GAME.player->hair == 3) strcpy(here, "Auburn");
    else if(GAME.player->hair == 4) strcpy(here, "Dark Blonde");
    else if(GAME.player->hair == 5) strcpy(here, "Blonde");
    else if(GAME.player->hair == 6) strcpy(here, "White/Grey");
    else if(GAME.player->hair == -1) strcpy(here, "Null");
    return;
}

void txt_eye(char* here, Game &GAME){
    if(GAME.player->eye == 0) strcpy(here, "Brown");
    else if(GAME.player->eye == 1) strcpy(here, "Hazel");
    else if(GAME.player->eye == 2) strcpy(here, "Blue");
    else if(GAME.player->eye == 3) strcpy(here, "Grey");
    else if(GAME.player->eye == 4) strcpy(here, "Green");
    else if(GAME.player->eye == 5) strcpy(here, "Violet");
    return;
}

void txt_comeliness(char* here, Game &GAME){
    if(GAME.player->comeliness == 0) strcpy(here, "Ugly");
    else if(GAME.player->comeliness == 1) strcpy(here, "Plain");
    else if(GAME.player->comeliness == 2) strcpy(here, "Average");
    else if(GAME.player->comeliness == 3) strcpy(here, "Attractive");
    else if(GAME.player->comeliness == 4) strcpy(here, "Handsome");
    return;
}

const char *txt_voice(int voice){
    if(voice <= 4) return "Unbearable";
    else if(voice >= 5 && voice <= 8) return "Unpleasant";
    else if(voice >= 9 && voice <= 12) return "Average";
    else if(voice >= 13 && voice <= 15) return "Pleasant";
    else if(voice >= 16 && voice <= 17) return "Excellent";
    else if(voice >= 18) return "Unearthly";
    else return "Null";
}

const char *txt_intelligence(int intel){
    if(intel <= 5) return "Absent Minded";
    else if(intel >= 6 && intel <= 8) return "Forgetful";
    else if(intel >= 9 && intel <= 12) return "Average";
    else if(intel >= 13 && intel <= 15) return "Good";
    else if(intel >= 16) return "Excellent";
    else return "Null";
}

const char *txt_morality(Game &GAME){
    if(GAME.player->MOR <= 4) return "Diabolical";
    else if(GAME.player->MOR >= 5 && GAME.player->MOR <= 7) return "Unscrupulous";
    else if(GAME.player->MOR >= 8 && GAME.player->MOR <= 10) return "Corruptible";
    else if(GAME.player->MOR >= 11 && GAME.player->MOR <= 13) return "Law-Abiding";
    else if(GAME.player->MOR >= 14 && GAME.player->MOR <= 16) return "Principled";
    else if(GAME.player->MOR >= 17) return "Exemplary";
    else return "Null";
}

void print_choice(){
    TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
    TCODConsole::root->print(43, 15, "Generation method:");
    TCODConsole::root->setColorControl(TCOD_COLCTRL_5, TCODColor::black, TCODColor::white);
    TCODConsole::root->setColorControl(TCOD_COLCTRL_2, TCODColor::lightGrey, TCODColor::black);
    TCODConsole::root->printRect(43, 17, 44,4, "%cFour dice%c %c- Characteristics are obtained by rolling 3d6. In this mode four dice are rolled, the lower value is then discarded.%c", TCOD_COLCTRL_5, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);
    TCODConsole::root->printRect(43, 21, 44,4, "%cDistribute points%c %c- The standard 3d6 are rolled, but you get to allocate manually a small pool of points between all characteristics.%c", TCOD_COLCTRL_5, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);        
}

void print_choice_s(){
    TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
    TCODConsole::root->print(43, 15, "Select race:");
}

void print_choice_se(){
    TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);
    TCODConsole::root->print(43, 15, "Select gender:");
}

void addpoint(unsigned int &stat, int ostat, int &points, bool subadd, int who){
    if(subadd){
        if(ostat+stat > 14 && ostat+stat < 18 && points > 2) { ++stat; points = points - 3; }
        else if(ostat+stat > 11 && ostat+stat < 15 && points > 1) {++stat; points = points - 2; }
        else if(ostat+stat < 12 && points > 0) {++stat; --points; }
    } else {
        if(stat > 0){
            if(ostat+stat > 12 && ostat+stat < 16) { --stat; points = points + 2; }
            else if(ostat+stat > 15) { --stat; points = points + 3; }
            else { --stat; ++points; }
        }   
    } 
    if(stat == 0) TCODConsole::root->setColorControl(TCOD_COLCTRL_2, TCODColor::lighterYellow, TCODColor::black);
    else TCODConsole::root->setColorControl(TCOD_COLCTRL_2, TCODColor::lighterBlue, TCODColor::black);
    if(who == 9) TCODConsole::root->print(57, 18, "%c%d (%s)     %c", TCOD_COLCTRL_2, ostat+stat, txt_voice(ostat+stat), TCOD_COLCTRL_STOP); 
    else if(who == 10) TCODConsole::root->print(57, 22, "%c%d (%s)      %c", TCOD_COLCTRL_2, ostat+stat, txt_intelligence(ostat+stat), TCOD_COLCTRL_STOP);
    else if(who > 10) TCODConsole::root->print(57, 12+who, "%c%d  %c", TCOD_COLCTRL_2, ostat+stat, TCOD_COLCTRL_STOP);
    else TCODConsole::root->print(57, 9+who, "%c%d  %c", TCOD_COLCTRL_2, ostat+stat, TCOD_COLCTRL_STOP);
}   

int edit_char(Game &GAME, int points){
    unsigned int main_osetx = 4;
    TCODConsole::root->setColorControl(TCOD_COLCTRL_1, TCODColor::black, TCODColor::white);
    TCODConsole::root->setColorControl(TCOD_COLCTRL_2, TCODColor::lighterBlue, TCODColor::black);
    TCODConsole::root->print(main_osetx+35, 9, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 10, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 11, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 12, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 13, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 14, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 15, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 16, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 17, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 18, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

    TCODConsole::root->print(main_osetx+35, 22, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 23, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    TCODConsole::root->print(main_osetx+35, 24, "%c-+%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

    TCODConsole::root->print(21, 5, "%c*%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

    std::shared_ptr<TCODConsole> menu (new TCODConsole(60, 3));
    TCOD_key_t key;
    TCOD_mouse_t mouse;
    bool button = false; // used to make sure the button is unpressed as first
    bool accepted = false;
    unsigned int str = 0, end = 0, dex = 0, agi = 0, spd = 0, eye = 0, hea = 0, smt = 0, tch = 0, voi = 0, intel = 0, aur = 0, wil = 0;
    while(accepted == false){
        TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
        if (TCODConsole::isWindowClosed()) return 0;
        mouse = TCODMouse::getStatus();
        unsigned int mousex = mouse.cx;
        unsigned int mousey = mouse.cy;
        if( (mousex >= 5 && mousex <= 10) && mousey == 33){ // menu
            menu->setDefaultForeground(TCODColor::black);
            menu->setDefaultBackground(TCODColor::white);
            menu->print(0, 1, "Accept");
            menu->setDefaultForeground(TCODColor::white);
            menu->setDefaultBackground(TCODColor::black);
            if(mouse.lbutton) accepted = true;
        } else {
            menu->setDefaultForeground(colorbase);
            menu->setDefaultBackground(TCODColor::black);
            menu->print(0, 1, "Accept");
        }    
        if(!mouse.lbutton) button = true;
        if(button && mouse.lbutton){
            if(mousex == main_osetx+35 && mousey == 9){
                addpoint(str, GAME.player->STR, points, 0, 0);
            }    
            else if(mousex == main_osetx+36 && mousey == 9){
                addpoint(str, GAME.player->STR, points, 1, 0);
            }    
            else if(mousex == main_osetx+35 && mousey == 10){
                addpoint(end, GAME.player->END, points, 0, 1);
            }
            else if(mousex == main_osetx+36 && mousey == 10){
                addpoint(end, GAME.player->END, points, 1, 1);
            }
            else if(mousex == main_osetx+35 && mousey == 11){
                addpoint(dex, GAME.player->DEX, points, 0, 2);
            }
            else if(mousex == main_osetx+36 && mousey == 11){
                addpoint(dex, GAME.player->DEX, points, 1, 2);
            }
            else if(mousex == main_osetx+35 && mousey == 12){
                addpoint(agi, GAME.player->AGI, points, 0, 3);
            }
            else if(mousex == main_osetx+36 && mousey == 12){
                addpoint(agi, GAME.player->AGI, points, 1, 3);
            }  
            else if(mousex == main_osetx+35 && mousey == 13){
                addpoint(spd, GAME.player->SPD, points, 0, 4);
            }
            else if(mousex == main_osetx+36 && mousey == 13){
                addpoint(spd, GAME.player->SPD, points, 1, 4);
            }
            else if(mousex == main_osetx+35 && mousey == 14){
                addpoint(eye, GAME.player->EYE, points, 0, 5);
            }
            else if(mousex == main_osetx+36 && mousey == 14){
                addpoint(eye, GAME.player->EYE, points, 1, 5);
            }
            else if(mousex == main_osetx+35 && mousey == 15){
                addpoint(hea, GAME.player->HEA, points, 0, 6);
            }
            else if(mousex == main_osetx+36 && mousey == 15){
                addpoint(hea, GAME.player->HEA, points, 1, 6);
            }
            else if(mousex == main_osetx+35 && mousey == 16){
                addpoint(smt, GAME.player->SMT, points, 0, 7);
            }
            else if(mousex == main_osetx+36 && mousey == 16){
                addpoint(smt, GAME.player->SMT, points, 1, 7);
            }
            else if(mousex == main_osetx+35 && mousey == 17){
                addpoint(tch, GAME.player->TCH, points, 0, 8);
            }
            else if(mousex == main_osetx+36 && mousey == 17){
                addpoint(tch, GAME.player->TCH, points, 1, 8);
            }
            else if(mousex == main_osetx+35 && mousey == 18){
                addpoint(voi, GAME.player->VOI, points, 0, 9);
            }
            else if(mousex == main_osetx+36 && mousey == 18){
                addpoint(voi, GAME.player->VOI, points, 1, 9);
            } 

            else if(mousex == main_osetx+35 && mousey == 22){
                addpoint(intel, GAME.player->INT, points, 0, 10);
            }
            else if(mousex == main_osetx+36 && mousey == 22){
                addpoint(intel, GAME.player->INT, points, 1, 10);
            }
            else if(mousex == main_osetx+35 && mousey == 23){
                addpoint(aur, GAME.player->AUR, points, 0, 11);
            }
            else if(mousex == main_osetx+36 && mousey == 23){
                addpoint(aur, GAME.player->AUR, points, 1, 11);
            }
            else if(mousex == main_osetx+35 && mousey == 24){
                addpoint(wil, GAME.player->WIL, points, 0, 12);
            }
            else if(mousex == main_osetx+36 && mousey == 24){
                addpoint(wil, GAME.player->WIL, points, 1, 12);
            }

            else if(mousex == 21 && mousey == 5){
                gen_name(GAME);
            }

            button = false; // reset mouse button
        }
        TCODConsole::root->print(main_osetx+1, 28, "%cTo distribute:%c %c%d%c  ", TCOD_COLCTRL_3, TCOD_COLCTRL_STOP, TCOD_COLCTRL_4, points, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+1, 29, "%c2 points when > 12, 3 points when > 15%c", TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
        menu->setDefaultForeground(TCODColor::lighterGrey);
        menu->setDefaultBackground(TCODColor::black);
        for(int x = 0; x < 6; ++x) menu->putChar(x, 0, '=', TCOD_BKGND_SET);
        for(int x = 0; x < 6; ++x) menu->putChar(x, 3-1, '-', TCOD_BKGND_SET);
        TCODConsole::root->print(22, 5, "%c%s%c                    ", TCOD_COLCTRL_1, GAME.player->name2, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(5, 90, "[%d.%d]%c%c", mousex, mousey, TCOD_CHAR_HLINE, TCOD_CHAR_HLINE);
        TCODConsole::blit(menu.get(),0,0,0,0,TCODConsole::root, 5, 32);
        TCODConsole::flush(); // this updates the screen
    } 
    GAME.player->STR += str;
    GAME.player->END += end;
    GAME.player->DEX += dex;
    GAME.player->AGI += agi;
    GAME.player->SPD += spd;
    GAME.player->EYE += eye;
    GAME.player->HEA += hea;
    GAME.player->SMT += smt;
    GAME.player->TCH += tch;
    GAME.player->VOI += voi;
    GAME.player->INT += intel;
    GAME.player->AUR += aur;
    GAME.player->WIL += wil;
    return points;
}    

int chargen(Game &GAME){
    bool method = false; // using 4 dice or point allocation
    std::vector<std::string> vecstr;
    std::string st1 = "Use four &dice";
    std::string st2 = "Distribute &points";
    vecstr.push_back(st1);
    vecstr.push_back(st2);
    print_choice();
    switch ( UI_menu(45, 29, vecstr, 1) ){
        case 1:
            method = false;
            break;
        case 2:
            method = true;
            break;
    }
    TCODConsole::root->clear();
    vecstr.clear();
    std::string ss1 = "Random";
    std::string ss2 = "Human";
    std::string ss3 = "Sindarin";
    std::string ss4 = "Khuzdul";
    bool lockspecies = false;
    vecstr.push_back(ss1);
    vecstr.push_back(ss2);
    vecstr.push_back(ss3);
    vecstr.push_back(ss4);
    print_choice_s();
    switch ( UI_menu(44, 17, vecstr, 0) ){
        case 1:
            break;
        case 2:
            GAME.player->species = Human;
            lockspecies = true;
            break;
            case 3:
            GAME.player->species = Sindarin;
            lockspecies = true;
            break;
            case 4:
            GAME.player->species = Khuzdul;
            lockspecies = true;
            break;
    }
    vecstr.clear();
    TCODConsole::root->clear();
    std::string se1 = "Random";
    std::string se2 = "Male";
    std::string se3 = "Female";
    bool locksex = false;
    vecstr.push_back(se1);
    vecstr.push_back(se2);
    vecstr.push_back(se3);
    print_choice_se();
    switch ( UI_menu(44, 17, vecstr, 0) ){
        case 1:
            break;
        case 2:
            GAME.player->sex = Male;
            locksex = true;
            break;
            case 3:
            GAME.player->sex = Female;
            locksex = true;
            break;
    }
    bool redofromstart = true;
    while(redofromstart){
        if(!lockspecies) gen_species(GAME);
        if(!locksex) gen_sex(GAME);
        gen_name(GAME);
        gen_birthdate(GAME);
        gen_sunsign(GAME);
        gen_sibrank(GAME);
        gen_strange(GAME);
        gen_hand(GAME);
        gen_height(GAME);
        gen_frame(GAME);
        gen_weight(GAME);
        gen_complexion(GAME);
        gen_hair(GAME);
        gen_eye(GAME);
        gen_comeliness(GAME);
        gen_STR(GAME, method);
        gen_END(GAME, method);
        gen_DEX(GAME, method);
        gen_AGI(GAME, method);
        gen_SPD(GAME, method);
        gen_EYE(GAME, method);
        gen_HEA(GAME, method);
        gen_SMT(GAME, method);
        gen_TCH(GAME, method);
        gen_VOI(GAME, method);
        gen_INT(GAME, method);
        gen_AUR(GAME, method);
        gen_WIL(GAME, method);
        gen_MOR(GAME, method);
        int main_osetx = 5;
        int alpoint = 0;
        if(method){
            TCODConsole::root->setColorControl(TCOD_COLCTRL_3, TCODColor::lighterBlue, TCODColor::black);
            TCODConsole::root->setColorControl(TCOD_COLCTRL_4, TCODColor::black, TCODColor::lighterBlue);
            alpoint = rng(8,12);
        }
        override:
        gen_rollskill(GAME);
        TCODConsole::root->clear();
        if(method) TCODConsole::root->print(main_osetx, 28, "%cTo distribute:%c %c%d%c", TCOD_COLCTRL_3, TCOD_COLCTRL_STOP, TCOD_COLCTRL_4, alpoint, TCOD_COLCTRL_STOP);
        draw_frame("CHARACTOR GENERATOR", "Pick your fool");
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);
        TCODConsole::root->setColorControl(TCOD_COLCTRL_2, TCODColor::lighterYellow, TCODColor::black);
        TCODConsole::root->print(5, 5, "Character name:");
        TCODConsole::root->print(22, 5, "%c%s%c", TCOD_COLCTRL_1, GAME.player->name2, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx, 7, "%cBIRTH%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        char dump[20];
        txt_species(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 9, "Species:");
        TCODConsole::root->print(main_osetx+17, 9, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        txt_sex(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 10, "Sex:");
        TCODConsole::root->print(main_osetx+17, 10, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        txt_birthdate(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 11, "Birthdate:");
        TCODConsole::root->print(main_osetx+17, 11, "%c%s, %d%c", TCOD_COLCTRL_2, dump, GAME.player->bday, TCOD_COLCTRL_STOP);
        txt_sunsign(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 12, "Sunsign:");
        TCODConsole::root->print(main_osetx+17, 12, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+2, 13, "Sibling Rank:");
        TCODConsole::root->print(main_osetx+17, 13, "%c%d (of %d)%c", TCOD_COLCTRL_2, GAME.player->sibrank, GAME.player->famsize, TCOD_COLCTRL_STOP);
        txt_enstrangement(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 14, "Enstrangement:");
        TCODConsole::root->print(main_osetx+17, 14, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        txt_handedness(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 15, "Handedness:");
        TCODConsole::root->print(main_osetx+17, 15, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx, 17, "%cAPPEARANCE%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+2, 19, "Height:");
        TCODConsole::root->print(main_osetx+17, 19, "%c%d'%c", TCOD_COLCTRL_2, GAME.player->height, TCOD_COLCTRL_STOP);
        txt_frame(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 20, "Frame:");
        TCODConsole::root->print(main_osetx+17, 20, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+2, 21, "Weight (Size):");
        TCODConsole::root->print(main_osetx+17, 21, "%c%dp/%d%c", TCOD_COLCTRL_2, GAME.player->weight, GAME.player->size, TCOD_COLCTRL_STOP);
        txt_complexion(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 22, "Complexion:");
        TCODConsole::root->print(main_osetx+17, 22, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        txt_hair(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 23, "Hair:");
        TCODConsole::root->print(main_osetx+17, 23, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        txt_eye(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 24, "Eyes:");
        TCODConsole::root->print(main_osetx+17, 24, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        txt_comeliness(dump, GAME);
        TCODConsole::root->print(main_osetx+2, 25, "Comeliness:");
        TCODConsole::root->print(main_osetx+17, 25, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+35, 7, "%cPHYSICAL%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 9, "Strength:");
        TCODConsole::root->print(57, 9, "%c%d%c", TCOD_COLCTRL_2, GAME.player->STR, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 10, "Endurance:");
        TCODConsole::root->print(57, 10, "%c%d%c", TCOD_COLCTRL_2, GAME.player->END, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 11, "Dexterity:");
        TCODConsole::root->print(57, 11, "%c%d%c", TCOD_COLCTRL_2, GAME.player->DEX, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 12, "Agility:");
        TCODConsole::root->print(57, 12, "%c%d%c", TCOD_COLCTRL_2, GAME.player->AGI, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 13, "Speed:");
        TCODConsole::root->print(57, 13, "%c%d%c", TCOD_COLCTRL_2, GAME.player->SPD, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 14, "Eyesight:");
        TCODConsole::root->print(57, 14, "%c%d%c", TCOD_COLCTRL_2, GAME.player->EYE, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 15, "Hearing:");
        TCODConsole::root->print(57, 15, "%c%d%c", TCOD_COLCTRL_2, GAME.player->HEA, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 16, "Smell/Taste:");
        TCODConsole::root->print(57, 16, "%c%d%c", TCOD_COLCTRL_2, GAME.player->SMT, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 17, "Touch:");
        TCODConsole::root->print(57, 17, "%c%d%c", TCOD_COLCTRL_2, GAME.player->TCH, TCOD_COLCTRL_STOP);
        //txt_voice(dump, GAME);
        TCODConsole::root->print(main_osetx+37, 18, "Voice:");
        TCODConsole::root->print(57, 18, "%c%d (%s)%c", TCOD_COLCTRL_2, GAME.player->VOI, txt_voice(GAME.player->VOI), TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+35, 20, "%cPERSONALITY%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 22, "Intelligence:");
        TCODConsole::root->print(57, 22, "%c%d (%s)%c", TCOD_COLCTRL_2, GAME.player->INT, txt_intelligence(GAME.player->INT), TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 23, "Aura:");
        TCODConsole::root->print(57, 23, "%c%d%c", TCOD_COLCTRL_2, GAME.player->AUR, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 24, "Will:");
        TCODConsole::root->print(57, 24, "%c%d%c", TCOD_COLCTRL_2, GAME.player->WIL, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+37, 25, "Morality:");
        TCODConsole::root->print(57, 25, "%c%d (%s)%c", TCOD_COLCTRL_2, GAME.player->MOR, txt_morality(GAME), TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+70, 7, "%cPHYSICAL SKILLS%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+90, 7, "%cSB%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 7, "%cML%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 9, "CONDITION");
        TCODConsole::root->print(main_osetx+90, 9, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.condSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 9, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.condML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 10, "STEALTH");
        TCODConsole::root->print(main_osetx+90, 10, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.slthSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 10, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.slthML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 11, "THROWING");
        TCODConsole::root->print(main_osetx+90, 11, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.thrwSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 11, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.thrwML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+70, 13, "%cCOMBAT SKILLS%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+90, 13, "%cSB%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 13, "%cML%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 15, "DODGE");
        TCODConsole::root->print(main_osetx+90, 15, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.dodgSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 15, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.dodgML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 16, "INITIATIVE");
        TCODConsole::root->print(main_osetx+90, 16, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.initSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 16, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.initML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 17, "Longswords");
        TCODConsole::root->print(main_osetx+90, 17, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.lswdSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 17, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.lswdML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 18, "MOBILITY");
        TCODConsole::root->print(main_osetx+90, 18, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.mobiSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 18, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.mobiML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 19, "UNARMED");
        TCODConsole::root->print(main_osetx+90, 19, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.uarmSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 19, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.uarmML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+70, 21, "%cCOMMUNICATION%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+90, 21, "%cSB%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 21, "%cML%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 23, "AWARENESS");
        TCODConsole::root->print(main_osetx+90, 23, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.awarSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 23, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.awarML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 24, "INTRIGUE");
        TCODConsole::root->print(main_osetx+90, 24, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.intrSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 24, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.intrML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 25, "ORATORY");
        TCODConsole::root->print(main_osetx+90, 25, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.oratSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 25, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.oratML, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+72, 26, "RHETORIC");
        TCODConsole::root->print(main_osetx+90, 26, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.rhetSB, TCOD_COLCTRL_STOP);
        TCODConsole::root->print(main_osetx+95, 26, "%c%d%%%c", TCOD_COLCTRL_2, GAME.player->skill.rhetML, TCOD_COLCTRL_STOP);
        vecstr.clear();
        std::string str1 = "&Accept";
        std::string str2 = "&Reroll"; // fix this shit
        std::string str3 = "&Edit";
        std::string str4 = "&Back";
        std::string str5 = "&QUIT";
        vecstr.push_back(str1);
        vecstr.push_back(str2);
        if(method) vecstr.push_back(str3);
        vecstr.push_back(str4);
        vecstr.push_back(str5);
    
        switch ( UI_menu(5, 32, vecstr, 1) ){
            case 5:
                return -1;
                break;
            case 1:
                return 0;
                break;
            case 2:

                break;
            case 4:
                if(method) return 1;
                else return -1;
                break;
            case -1:
                return -1;
                break;
            case 3:
                if(method){
                    alpoint = edit_char(GAME, alpoint);
                }
                else return 1;
                goto override;       
        }
    }
    return 0;
}



