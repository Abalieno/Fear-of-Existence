#include "gui.h"
#include "game.h"
#include "debug.h"

/*
 * 0 top panel 0 127
*/

void UI_register(Game &tgame, int ID, int x, int y, int w, int h){
    bool check = false;
    for (auto i : localhook) {
        if(i.ID == ID) check = true; // if ID present, register should fail
    } 
    if (check) debugmsg("UI_register fails, ID present: %d", ID);
    else {
        UIhook thisui;
        thisui.ID = ID;
        thisui.x = x;
        thisui.y = y;
        thisui.w = w;
        thisui.h = h;
        localhook.push_back(thisui);
    }
}   

void UI_hook(Game &tgame, int ID){
    int check = -1;
    for (unsigned int i = 0; i<tgame.gstate.UI_hook.size(); ++i) {
        if(tgame.gstate.UI_hook[i].ID == ID) check = i;
    }    
    if (check != -1) g_debugmsg("UI_hook fails, ID(%d) already in vector: %d", ID, check);
    else tgame.gstate.UI_hook.push_back(localhook[ID]);
}   

void UI_unhook(Game &tgame, int ID){
    int check = -1;
    for (unsigned int i = 0; i<tgame.gstate.UI_hook.size(); ++i) { 
        if(tgame.gstate.UI_hook[i].ID == ID) check = i;
    }
    if (check != -1) tgame.gstate.UI_hook.erase(tgame.gstate.UI_hook.begin()+check);
    else g_debugmsg("UI_unhook fails, no ID in vector: %d", ID);
}   

int menu_key(TCOD_event_t &eve, char &sel){
    TCOD_key_t key;
    TCOD_mouse_t mouse;
    eve = TCODSystem::checkForEvent(TCOD_EVENT_ANY,&key,&mouse);

    if (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_UP ){
        return move_up;
    }    
    if (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_LEFT ){
        return move_up;
    }
    if (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_DOWN ){
        return move_down;
    }    
    if (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_RIGHT ){
        return move_down;
    }    
    if ((eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_ENTER) || 
            (eve == TCOD_EVENT_KEY_PRESS && key.vk == TCODK_SPACE) ){
        return action;
    }
    if (key.vk != TCODK_NONE){ 
        sel = key.c;
        return keysel;
    }    
    return 0;
}

void buildframe(int totx, int toty, std::shared_ptr<TCODConsole> menu){
    menu->setDefaultForeground(TCODColor::lighterGrey);
    menu->setDefaultBackground(TCODColor::black);
    for(int x = 0; x < totx; ++x) menu->putChar(x, 0, '=', TCOD_BKGND_SET);
    for(int x = 0; x < totx; ++x) menu->putChar(x, toty-1, '-', TCOD_BKGND_SET);
}   

int UI_menu (unsigned int posx, unsigned int posy, std::vector<std::string> pack, bool type){
    int what_menu = 0; // constant-based for keys
    unsigned int menu_index = 1; // point at selected option
    unsigned int options = pack.size(); // number of options
    std::vector<int> select; // vector with int of letter highlight

    // block to parse strings and check for highlight letter '&'
    for ( auto &z : pack){ // walk string vector
        int cnt = 1;
        for( auto iter = z.begin() ; iter != z.end() ; ++iter){ // walk string
            if( *iter == '&' ){ 
                select.push_back(cnt);
                iter = z.erase(iter);
                break;
            } 
            ++cnt;
        }    
    }   

    int sizx = 0;
    int annoy = 0;
    for (auto count : pack){
        annoy = count.length();
        if (sizx < annoy) sizx = count.length();
    }    
    sizx += 2; // some space
    int sizy = options+2; // +2 lines for some space

    int tempsiz = sizx - 2;
    if(type){ // HORIZONTAL STYLE
        sizy = 1;
        sizx = (options * tempsiz) + ((options-1) * 3);
    }    
    
    std::shared_ptr<TCODConsole> menu (new TCODConsole(sizx, sizy+2)); // should be preserved in GAME object?
    
    bool button = false; // used to make sure the button is unpressed as first
    bool keypress = false; // used to make sure no key is already pressed
    char sel = '!';
    TCOD_event_t eve;
    while(1){
        menu->clear();
        buildframe(sizx, sizy+2, menu); // menu frame
        menu->setAlignment(TCOD_LEFT);
        if(type) menu->setAlignment(TCOD_CENTER);
        menu->setBackgroundFlag(TCOD_BKGND_SET);

        unsigned int index = 1;
        if(!type){
            for (auto count : pack){
                if (index == menu_index){
                    menu->setDefaultForeground(TCODColor::black);
                    menu->setDefaultBackground(TCODColor::white);
                    menu->print(1, index+1, "%s", count.c_str());
                    menu->setDefaultForeground(TCODColor::white);
                    menu->setDefaultBackground(TCODColor::black);
                } else {
                    menu->setDefaultForeground(colorbase);
                    menu->setDefaultBackground(TCODColor::black);
                    menu->print(1, index+1, "%s", count.c_str());
                }
                ++index;
            } // next block highlight the letter 
            for (unsigned int z = 0; z < select.size(); ++z){
                if (select[z]){ // only highlight if non-0 
                    if( (z+1) != menu_index) menu->setCharBackground(select[z], z+2, TCODColor::red);
                    else menu->setCharForeground(select[z], z+2, TCODColor::red);
                }    
            }
        } else {
            for (auto count : pack){
                int a = 0; // for spacing between options and "/"
                if (index == menu_index){
                    menu->setDefaultForeground(TCODColor::black);
                    menu->setDefaultBackground(TCODColor::white);
                    a = 3 * (index-1);
                    menu->print(((index-1)*tempsiz)+(tempsiz/2)+a, 1, "%s", count.c_str());
                    menu->setDefaultForeground(TCODColor::white);
                    menu->setDefaultBackground(TCODColor::black);
                    for(int fill = 0; fill < tempsiz; ++fill) menu->setCharBackground(fill+(tempsiz*(index-1))+a, 1, TCODColor::white);
                } else {
                    menu->setDefaultForeground(colorbase);
                    menu->setDefaultBackground(TCODColor::black);
                    a = 3 * (index-1);
                    menu->print((index-1)*tempsiz+(tempsiz/2)+a, 1, "%s", count.c_str());
                }
                ++index;
            } // next block highlight the letter 
            for (unsigned int z = 0; z < select.size(); ++z){
                if (select[z]){ // only highlight if non-0
                    int a = 0; // for spacing between options and "/"
                    a = 3 * (z);
                    int origin = 0;
                    origin = (tempsiz-pack[z].length()) /2;
                    if( (tempsiz-pack[z].length()) % 2 == 1) origin += 1;
                    if( (z+1) != menu_index) menu->setCharBackground(select[z]-1+(tempsiz*z)+origin+a, 1, TCODColor::red);
                    else menu->setCharForeground(select[z]-1+(tempsiz*z)+origin+a, 1, TCODColor::red);
                }    
            }
        }    

        what_menu = menu_key(eve, sel); // polls keyboard

        if (what_menu == move_up){
            if(menu_index == 1){ 
                menu_index = options;
            } else --menu_index;
        }    
        
        if (what_menu == move_down){
            if(menu_index == options){ 
                menu_index = 1;
            } else ++menu_index; 
        }

        if (what_menu == action) return menu_index;

        if (TCODConsole::isWindowClosed()) return -1;

        if (eve == TCOD_EVENT_KEY_RELEASE){ keypress = true;}
        
        if (what_menu == keysel && keypress){
            for (unsigned int z = 0; z < select.size(); ++z){
                if(select[z]){ // only if non-0 so if index present 
                    if (sel == tolower(pack[z][select[z]-1]) ) return z+1; 
                } 
            }    
        }    

        bool flagged = false;
        TCOD_mouse_t mouse;
        mouse = TCODMouse::getStatus();
        unsigned int mousex = mouse.cx;
        unsigned int mousey = mouse.cy;
        if(!type){
            for(unsigned int op = 1; op <= options; ++op){
                if( (mousex >= posx  && mousex < (posx+sizx) ) && mousey == (op+posy+1)){
                    flagged = true;
                    menu_index = op;
                }
            }
        } else {
            for(unsigned int op = 0; op < options; ++op){
                int a = 0; // for spacing between options and "/"
                a = 3 * (op);
                if( (mousex >= (posx+(tempsiz*op))+a  && mousex < (posx+(tempsiz*op)+tempsiz)+a ) && mousey == posy+1){
                    flagged = true;
                    menu_index = op+1;
                }
            }
        }    
        if(!mouse.lbutton) button = true;
        if(button) // only execute if the button was depressed once
            if(mouse.lbutton && flagged) return menu_index;

        TCODConsole::blit(menu.get(),0,0,0,0,TCODConsole::root, posx, posy);
        TCODConsole::flush(); // this updates the screen
        keypress = false; // so that is true only when release event happens
    }

    return 666; 
}
