#include <iostream>
#include <stdio.h>
#include <stdarg.h>

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
