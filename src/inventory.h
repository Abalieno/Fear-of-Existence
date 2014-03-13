#ifndef _INVENTORY_H_
#define _INVENTORY_H_

class Generic_object {
public:
    char name[60];
    char glyph_8;

Generic_object(); 

void drop_to(int x1, int y1);

int posx();
int posy();


private:
    int x;
    int y;
    

};    



#endif
