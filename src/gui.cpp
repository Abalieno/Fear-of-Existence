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
