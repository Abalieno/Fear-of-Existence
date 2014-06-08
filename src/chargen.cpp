#include "chargen.h"
#include "rng.h"
#include "debug.h"
#include "gui.h"

extern int win_y;
extern int win_x;

void gen_name(Game &GAME){
    strcpy(GAME.player->name, "Placeholderguy");
    return;
}
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
    return;
}
void gen_strange(Game &GAME){
    return;
}
void gen_hand(Game &GAME){
    return;
}
void gen_height(Game &GAME){
    return;
}
void gen_frame(Game &GAME){
    return;
}
void gen_weight(Game &GAME){
    return;
}
void gen_size(Game &GAME){
    return;
}
void gen_complexion(Game &GAME){
    return;
}
void gen_hair(Game &GAME){
    return;
}
void gen_eye(Game &GAME){
    return;
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

int chargen(Game &GAME){
    bool redofromstart = true;
    while(redofromstart){
        TCODConsole::root->clear();
        gen_name(GAME);
        gen_species(GAME);
        gen_sex(GAME);
        gen_birthdate(GAME);
        gen_sunsign(GAME);
        gen_sibrank(GAME);
        gen_strange(GAME);
        gen_hand(GAME);
        gen_height(GAME);
        gen_frame(GAME);
        gen_weight(GAME);
        gen_size(GAME);
        gen_complexion(GAME);
        gen_hair(GAME);
        gen_eye(GAME);
        draw_frame("CHARACTOR GENERATOR", "Pick your fool");
        TCODConsole::root->setAlignment(TCOD_LEFT);
        TCODConsole::root->setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);
        TCODConsole::root->setColorControl(TCOD_COLCTRL_2, TCODColor::yellow, TCODColor::black);
        TCODConsole::root->print(5, 5, "Player name:");
        TCODConsole::root->print(25, 5, "%c%s%c", TCOD_COLCTRL_1, GAME.player->name, TCOD_COLCTRL_STOP);
        char dump[20];
        txt_species(dump, GAME);
        TCODConsole::root->print(5, 6, "Species:");
        TCODConsole::root->print(25, 6, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        txt_sex(dump, GAME);
        TCODConsole::root->print(5, 7, "Sex:");
        TCODConsole::root->print(25, 7, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        txt_birthdate(dump, GAME);
        TCODConsole::root->print(5, 8, "Birthdate:");
        TCODConsole::root->print(25, 8, "%c%s, %d%c", TCOD_COLCTRL_2, dump, GAME.player->bday, TCOD_COLCTRL_STOP);
        txt_sunsign(dump, GAME);
        TCODConsole::root->print(5, 9, "Sunsign:");
        TCODConsole::root->print(25, 9, "%c%s%c", TCOD_COLCTRL_2, dump, TCOD_COLCTRL_STOP);
        std::vector<std::string> vecstr;
        std::string st1 = "&Accept";
        std::string st2 = "&Reroll";
        std::string st3 = "&Skip";
        std::string st4 = "&QUIT";
        vecstr.push_back(st1);
        vecstr.push_back(st2);
        vecstr.push_back(st3);
        vecstr.push_back(st4);
        //int menu_index = 1;
        switch ( UI_menu(5, 32, vecstr, 1) ){
            case 4:
                return -1;
                break;
            case 1:
                return 0;
                break;
            case 2:

                break;
            case 3:
                return 0;
                break;
            case -1:
                return -1;
                break;
        }
    }
    return 0;
}



