#include <iostream>

#include "screens.h"
#include "chargen.h"
#include "gui.h"

extern void print_8x16(TCODConsole* &loc, int where_x, int where_y, const char *msg, TCODColor front, TCODColor back);
extern void print_c64(TCODConsole* &loc, int where_x, int where_y, const char *msg, TCODColor front, TCODColor back);

void char_sheet(Game &GAME){
    TCODConsole *wg_char = new TCODConsole(128, 90);  // UI topbar
    wg_char->setAlignment(TCOD_LEFT);
    TCOD_key_t key;
    TCOD_mouse_t mouse;
    bool button = false; // used to make sure the button is unpressed at first
    bool flagged = true;
    unsigned int x = 0;
    unsigned int y = 0;
    while(1){
        TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);
        if (TCODConsole::isWindowClosed()) return;
        mouse = TCODMouse::getStatus();
        unsigned int mousex = mouse.cx;
        unsigned int mousey = mouse.cy;
        if(!mouse.lbutton) flagged = false; 
        if(mouse.lbutton && !flagged) { x = mousex; y = mousey; flagged = true;}
        if(mouse.lbutton && ( x != mousex || y != mousey) ) { button = false; }
        else button = true;
        if((mousex > 2 && mousex < 9) && mousey == 3){
            wg_char->setColorControl(TCOD_COLCTRL_1, TCODColor::black, TCODColor::white);
            wg_char->print(3, 2, "%c>EXIT<%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            if(button && mouse.lbutton){
                delete wg_char;
                return;
            }    
        } else {    
            wg_char->setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);
            wg_char->print(3, 2, "%c>EXIT<%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        }
        compile_sheet(wg_char, GAME, 3, 5); // chargen.cpp
        TCODConsole::blit(wg_char,0,0,0,0, TCODConsole::root, 0, 1);
        TCODConsole::flush(); // this updates the screen
    }   
    return;
}

void draw_frame(int x, int y, TCODConsole *thisone){ 
    for (int n = 0; n <= y; ++n){
        thisone->setDefaultForeground(TCODColor::lighterGrey);
        thisone->setDefaultBackground(TCODColor::black);
        thisone->print(0, n, "%c", TCOD_CHAR_VLINE);
        thisone->print(x-1, n, "%c", TCOD_CHAR_VLINE);
    }
    for (int n = 0; n <= x; ++n){
        thisone->setDefaultForeground(TCODColor::lighterGrey);
        thisone->setDefaultBackground(TCODColor::black);
        thisone->print(n, 0, "%c", TCOD_CHAR_HLINE);
        thisone->print(n, y-1, "%c", TCOD_CHAR_HLINE);
    }
    thisone->print(0, 0, "%c", TCOD_CHAR_NW);
    thisone->print(x-1, 0, "%c", TCOD_CHAR_NE);
    thisone->print(0, y-1, "%c", TCOD_CHAR_SW);
    thisone->print(x-1, y-1, "%c", TCOD_CHAR_SE);
    return;
}    

void event_description(Game &GAME, int sender){
    TCODConsole *wg_char = new TCODConsole(85, 75);  // UI topbar
    unsigned int wx = 20;
    unsigned int wy = 5; // windows offsets
    wg_char->setAlignment(TCOD_LEFT);
    TCOD_key_t key;
    TCOD_mouse_t mouse;
    bool button = false; // used to make sure the button is unpressed at first
    bool flagged = true;
    unsigned int x = 0;
    unsigned int y = 0;
    bool endline = false;
    draw_frame(85, 75, wg_char);
    //print_c64(wg_char, 5, 10, "HELLO", TCODColor::lightGrey, TCODColor::black);
    //print_8x16(wg_char, 10, 5, "In eternity, where there is no time,", TCODColor::lightGrey, TCODColor::black);
    char line[80];
    int line_n = 0; // counter for line
    unsigned i = 0;
    unsigned endi = 80;
    unsigned int inner = 0;
    while(!endline){
        inner = 0;
        bool skip = false;
        while(i <= endi){
            if(GAME.gstate.features[sender].content[i] == '%' &&
                    GAME.gstate.features[sender].content[i+1] == 't'){
                line[inner] = '\0';
                //line_n = line_n + 2;
                ++i;
                ++inner;
                endi = i;
                skip = true;
                //break;
            } else{   
                line[inner] = GAME.gstate.features[sender].content[i];
                if(GAME.gstate.features[sender].content[i] == '\0') endline = true;
                ++i;
                ++inner;
            }
        } 
        if(!skip){
        if(!endline){
            while(line[inner] != ' '){
                --endi;
                --i;
                --inner;
                if (endi == 0) break;
            } 
            ++i;
        }
        line[inner] = '\0';
        }
        //print_8x16(wg_char, 2, 3,  GAME.gstate.features[0].content , TCODColor::lightGrey, TCODColor::black);
        print_8x16(wg_char, 2, 3 + line_n, line, TCODColor::lightGrey, TCODColor::black);
        endi += 83;
        line_n = line_n + 2;
        if(skip) {line_n = line_n + 2; ++i;}
        //if(line_n > 16) break;
    }
    for (int n = 0; n <= 80; ++n){
    wg_char->print(2+n, 4 + line_n, "%c", TCOD_CHAR_HLINE);
    }
    bool fade = false;
    int colfr = 100;
    int colfg = 100;
    int colfb = 100;
    int colr = 100;
    unsigned selected = 1;
    TCODColor fadedark(90, 90, 90);

    std::vector<std::string> hereoptions;
    char thisoption[200];
    for(unsigned cy = 0; cy < GAME.gstate.features[sender].options.size(); cy++){
        sprintf(thisoption, "%d. ", cy+1);
        strcat(thisoption, GAME.gstate.features[sender].options[cy].c_str());
        hereoptions.push_back(thisoption);
    }  

    int what_menu = 1; // constant-based for keys
    TCOD_event_t eve;
    char sel = '!';

    while(1){

        if(GAME.gstate.features[sender].isoption){
            if(colfr >= 255) fade = false;
            if(colfr <= 150) fade = true;

            if(fade){
                colr += 3;
            }else{
                colr -= 2;
            }
            if(colr > 255) colr = 255;
            colfr = colr;
            colfg = colr;
            colfb = colr;
            TCODColor fadec(colfr, colfg, colfb);

            for(unsigned cy = 0; cy < hereoptions.size(); cy++){
                if(selected == cy+1){
                    print_8x16(wg_char, 2, 6 + line_n + (3*cy), //"%d. %s", cy+1, 
                        hereoptions[cy].c_str(), fadec, TCODColor::black);
                    
                }else{ 
                    print_8x16(wg_char, 2, 6 + line_n + (3*cy), //"%d. %s", cy+1, 
                        hereoptions[cy].c_str(), fadedark, TCODColor::black);
                }    
            }
        }

        if (TCODConsole::isWindowClosed()) return;
        mouse = TCODMouse::getStatus();
        unsigned int mousex = mouse.cx;
        unsigned int mousey = mouse.cy;
        if(!mouse.lbutton) flagged = false; 
        if(mouse.lbutton && !flagged) { x = mousex; y = mousey; flagged = true;}
        if(mouse.lbutton && ( x != mousex || y != mousey) ) { button = false; }
        else button = true;
        if( (mousex > (wx+8) && mousex < (wx+12)) && mousey == (wy+0)){ // mouse cursor on ESC
            wg_char->setColorControl(TCOD_COLCTRL_1, TCODColor::black, TCODColor::white);
            if(button && mouse.lbutton){
                delete wg_char;
                return;
            }    
        } else {    
            wg_char->setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);
        }

        // keyboard -> gui.cpp | uses int constants as commands
        what_menu = menu_key(eve, sel); // polls keyboard
        if(what_menu == outhere){ // if ESC
            delete wg_char;
            return;
        }    
        if (what_menu == move_up){
            if(selected == 1){ 
                selected = hereoptions.size();
            } else --selected;
        }    
        
        if (what_menu == move_down){
            if(selected == hereoptions.size()){ 
                selected = 1;
            } else ++selected; 
        }

        // mouse hover
        for(unsigned int op = 0; op < hereoptions.size(); ++op){
            if( (mousex >= (2 + wx)  && mousex < (83 + wx) ) && (mousey == (6+wy) + line_n + (3*op) || mousey == (7+wy) + line_n + (3*op) ) ){
                selected = op+1;
            }
        }

        wg_char->setDefaultForeground(TCODColor::darkGrey);
        wg_char->print(2, 0, " Press %cESC%c to close ", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::blit(wg_char,0,0,0,0, TCODConsole::root, wx, wy);
        TCODConsole::flush(); // this updates the screen
    }    
    return;
}    



