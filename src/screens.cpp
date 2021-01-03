#include <iostream>

#include "screens.h"
#include "chargen.h"
#include "gui.h"
#include "rng.h"

extern int print_8x16(TCODConsole* &loc, int where_x, int where_y, const char *msg, TCODColor front, TCODColor back, int linemax = 0);
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
            wg_char->printf(3, 2, "%c>EXIT<%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
            if(button && mouse.lbutton){
                delete wg_char;
                return;
            }    
        } else {    
            wg_char->setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);
            wg_char->printf(3, 2, "%c>EXIT<%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        }
        compile_sheet(wg_char, GAME, 3, 5, 0); // chargen.cpp
        TCODConsole::blit(wg_char,0,0,0,0, TCODConsole::root, 0, 1);
        TCODConsole::flush(); // this updates the screen
    }   
    return;
}

void draw_frame(int x, int y, TCODConsole *thisone){ 
    for (int n = 0; n <= y; ++n){
        thisone->setDefaultForeground(TCODColor::lighterGrey);
        thisone->setDefaultBackground(TCODColor::black);
        //thisone->print(0, n, "%c", TCOD_CHAR_VLINE);
        thisone->putChar(0, n, TCOD_CHAR_VLINE);
        //thisone->print(x-1, n, "%c", TCOD_CHAR_VLINE);
        thisone->putChar(x-1, n, TCOD_CHAR_VLINE);
    }
    for (int n = 0; n <= x; ++n){
        thisone->setDefaultForeground(TCODColor::lighterGrey);
        thisone->setDefaultBackground(TCODColor::black);
        //thisone->print(n, 0, "%c", TCOD_CHAR_HLINE);
        thisone->putChar(n, 0, TCOD_CHAR_HLINE);
        //thisone->print(n, y-1, "%c", TCOD_CHAR_HLINE);
        thisone->putChar(n, y-1, TCOD_CHAR_HLINE);
    }
    //thisone->print(0, 0, "%c", TCOD_CHAR_NW);
    //thisone->print(x-1, 0, "%c", TCOD_CHAR_NE);
    //thisone->print(0, y-1, "%c", TCOD_CHAR_SW);
    //thisone->print(x-1, y-1, "%c", TCOD_CHAR_SE);
    thisone->putChar(0, 0, TCOD_CHAR_NW);
    thisone->putChar(x-1, 0, TCOD_CHAR_NE);
    thisone->putChar(0, y-1, TCOD_CHAR_SW);
    thisone->putChar(x-1, y-1, TCOD_CHAR_SE);
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
    draw_frame(85, 75, wg_char);

    int cursor_at = 2;
    cursor_at = print_8x16(wg_char, 2, cursor_at, GAME.gstate.features[sender].content, TCODColor::lightGrey, TCODColor::black, 81); 
    cursor_at++;
    for (int n = 0; n <= 80; ++n){ // adds the line spacing
    wg_char->putChar(2+n, cursor_at, TCOD_CHAR_HLINE);
    }
    cursor_at += 2;

    bool fade = false;

    bool mouseactive = false;
    unsigned int mouseVx = 0;
    unsigned int mouseVy = 0;
    int framer = 0;
    int sec = 0;

    int colfr = 100;
    int colfg = 100;
    int colfb = 100;
    int colr = 255;
    unsigned selected = 1;
    TCODColor fadedark(90, 90, 90);

    std::vector<std::string> hereoptions;
    std::vector<unsigned int> optionsize; // used to store the y pointer to use for mouse selection
    char thisoption[3000];
    bool multiline[GAME.gstate.features[sender].options.size()];
    for(unsigned cy = 0; cy < GAME.gstate.features[sender].options.size(); cy++){
        sprintf(thisoption, "%d. ", cy+1);
        strcat(thisoption, GAME.gstate.features[sender].options[cy].c_str());
        hereoptions.push_back(thisoption);
        optionsize.push_back(0); // int is set below when the string is printed
    }
    optionsize.push_back(0); // adds another to store where the y cursor ends up

    int what_menu = 1; // constant-based for keys
    TCOD_event_t eve;
    char sel = '!';

    int cursor_orig = cursor_at;

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

            cursor_at = cursor_orig;
            for(unsigned cy = 0; cy < hereoptions.size(); cy++){ // line_n is offset from above
                if(selected == cy+1){
                    optionsize[cy] =  cursor_at;
                    if(multiline[cy]){
                        cursor_at = print_8x16(wg_char, 2, cursor_at, 
                        hereoptions[cy].c_str(), fadec, TCODColor::black, 81);
                    }else {
                        cursor_at = print_8x16(wg_char, 2, cursor_at,  
                        hereoptions[cy].c_str(), fadec, TCODColor::black, 81);
                    }  
                    cursor_at++;
                }else{
                    optionsize[cy] = cursor_at;
                    if(multiline[cy]){
                        cursor_at = print_8x16(wg_char, 2, cursor_at, 
                            hereoptions[cy].c_str(), fadedark, TCODColor::black, 81);
                    }else {
                        cursor_at = print_8x16(wg_char, 2, cursor_at,  
                            hereoptions[cy].c_str(), fadedark, TCODColor::black, 81);
                    }
                    cursor_at++;
                }    
            }
            optionsize[optionsize.size()-1] = cursor_at ;
        }

        if (TCODConsole::isWindowClosed()) return;
        mouse = TCODMouse::getStatus();
        unsigned int mousex = mouse.cx;
        if(mousex != mouseVx) {mouseVx = mousex; mouseactive = true;} // enable mouse only if moved
        unsigned int mousey = mouse.cy;
        if(mousey != mouseVy) {mouseVy = mousey; mouseactive = true;}
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
            colr=255; // white if menu moves
            mouseactive = false; // disable mouse (until it moves) if keyboard is used
            if(selected == 1){ 
                selected = hereoptions.size();
            } else --selected;
        }    
        
        if (what_menu == move_down){
            colr=255;
            mouseactive = false;
            if(selected == hereoptions.size()){ 
                selected = 1;
            } else ++selected; 
        }

        // mouse hover
        if(mouseactive){
            mouseactive = false; // disable mouse to check for dialogue options area
        for(unsigned int op = 0; op < hereoptions.size(); ++op){
            if( (mousex >= (2 + wx)  && mousex < (83 + wx) ) && 
                    ( mousey >= optionsize[op]+wy && mousey < optionsize[op+1]+wy ) ){
                 
                if(selected != op+1) colr=255;
                selected = op+1;
                mouseactive = true; // enable mouse only if within dialogue options space
            }
        }
    }

        if (what_menu == action || (mouse.lbutton && mouseactive)){
            colr=255;
            for( int tinycount = 30; tinycount >0 ; tinycount--){
            colfr = colr;
            colfg = colr;
            colfb = colr;
            TCODColor fadec(colfr, colfg, colfb);
            if(colr >= 150) colr = rng(50,150); else colr = rng(151,255);


            cursor_at = cursor_orig;
            for(unsigned cy = 0; cy < hereoptions.size(); cy++){ // line_n is offset from above
                if(selected == cy+1){
                    optionsize[cy] =  cursor_at;
                    if(multiline[cy]){
                        cursor_at = print_8x16(wg_char, 2, cursor_at, 
                        hereoptions[cy].c_str(), fadec, TCODColor::black, 81);
                    }else {
                        cursor_at = print_8x16(wg_char, 2, cursor_at,  
                        hereoptions[cy].c_str(), fadec, TCODColor::black, 81);
                    }  
                    cursor_at++;
                }else{
                    optionsize[cy] = cursor_at;
                    if(multiline[cy]){
                        cursor_at = print_8x16(wg_char, 2, cursor_at, 
                            hereoptions[cy].c_str(), fadedark, TCODColor::black, 81);
                    }else {
                        cursor_at = print_8x16(wg_char, 2, cursor_at,  
                            hereoptions[cy].c_str(), fadedark, TCODColor::black, 81);
                    }
                    cursor_at++;
                }    
            }
            optionsize[optionsize.size()-1] = cursor_at ;
               
            what_menu = menu_key(eve, sel); // polls keyboard

            wg_char->setDefaultForeground(TCODColor::lighterGrey);
        wg_char->printf(62, 74, "Mouse at [%d.%d]", mousex - 20, mousey - 5);

        framer++;
        if(framer >= 30) {sec++; framer = 0;}
        wg_char->printf(32, 74, "seconds: %d", sec);

        wg_char->setDefaultForeground(TCODColor::darkGrey);
        wg_char->printf(2, 0, " Press %cESC%c to close ", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::blit(wg_char,0,0,0,0, TCODConsole::root, wx, wy);
        TCODConsole::flush(); // this updates the screen
            }
        }


        wg_char->setDefaultForeground(TCODColor::lighterGrey);
        wg_char->printf(62, 74, "Mouse at [%d.%d]", mousex - 20, mousey - 5);

        framer++;
        if(framer >= 30) {sec++; framer = 0;}
        wg_char->printf(32, 74, "seconds: %d", sec);

        wg_char->setDefaultForeground(TCODColor::darkGrey);
        wg_char->printf(2, 0, " Press %cESC%c to close ", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
        TCODConsole::blit(wg_char,0,0,0,0, TCODConsole::root, wx, wy);
        TCODConsole::flush(); // this updates the screen
    }    
    return;
}    



