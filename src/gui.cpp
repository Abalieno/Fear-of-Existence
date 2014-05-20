#include "gui.h"
#include "game.h"

/*
 * 0 top panel 0 127
*/

bool UI_register(Game &tgame, int ID, int x, int y, int w, int h){
    bool check = false;
    for (auto i : localhook) {
        if(i.ID == ID) check = true; // if ID present, register should fail
    } 
    if (check) return true;
    else {
        UIhook thisui;
        thisui.ID = ID;
        thisui.x = x;
        thisui.y = y;
        thisui.w = w;
        thisui.h = h;
        localhook.push_back(thisui);
    }
    return false;
}   

bool UI_hook(Game &tgame, int ID){
    bool check = false;
    for (auto i : tgame.gstate.UI_hook) {
        if(i.ID == ID) check = true;
    }    
    if (check) return true;
    else tgame.gstate.UI_hook.push_back(localhook[ID]);
    return false;    
}   

bool UI_unhook(Game &tgame, int ID){
    bool check = true;
    for (unsigned int i = 0; i<tgame.gstate.UI_hook.size(); ++i) { 
        if(tgame.gstate.UI_hook[i].ID == ID){ 
            tgame.gstate.UI_hook.erase(tgame.gstate.UI_hook.begin()+i);
            check = false;
        }
    }
    if (check) return true;
    else return false;
}    
