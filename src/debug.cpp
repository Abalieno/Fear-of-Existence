#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include "libtcod.hpp"

void debugmsg(const char *mes, ...)
{
    va_list ap;
    va_start(ap, mes);
    char buff[2048];
    vsprintf(buff, mes, ap);
    va_end(ap);
    std::cout << "DEBUG: " << buff << std::endl;
 /*
 va_list ap;
 va_start(ap, mes);
 char buff[2048];
 vsprintf(buff, mes, ap);
 va_end(ap);
 attron(COLOR_PAIR(3));
 mvprintw(0, 0, "DEBUG: %s      \n  Press spacebar...", buff);
 while(getch() != ' ');
 attroff(COLOR_PAIR(3));
 */
}

void g_debugmsg(const char *mes, ...)
{
    int x = 0;
    int y = 3;
    va_list ap;
    va_start(ap, mes);
    char buff[2048];
    vsprintf(buff, mes, ap);
    va_end(ap);
    x = strlen(buff)+8; // +7 because of added DEBUG: tag
    if (x < 29) x = 29;
    TCODConsole *bugger = new TCODConsole(x, y); // should this stay preserved in GAME object?
    bugger->setAlignment(TCOD_LEFT);
    TCODConsole::setColorControl(TCOD_COLCTRL_1,TCODColor::red,TCODColor::black);
    TCODConsole::setColorControl(TCOD_COLCTRL_2,TCODColor::yellow,TCODColor::black);
    std::cout << "DEBUG: " << buff << std::endl;
    bugger->printf(0,0, "%cDEBUG: %c%c%s%c", TCOD_COLCTRL_1,TCOD_COLCTRL_STOP,TCOD_COLCTRL_2, buff, TCOD_COLCTRL_STOP);
    bugger->printf(0,2, "<Press a key to continue...>");
    TCODConsole::blit(bugger,0,0,x,3,TCODConsole::root, 5, 5);
    TCODConsole::flush();
    TCODConsole::waitForKeypress(true);
    delete bugger;
}
