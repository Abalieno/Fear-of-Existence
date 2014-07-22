#include "screens.h"
#include "chargen.h"

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



