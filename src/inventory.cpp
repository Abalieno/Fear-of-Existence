#include "inventory.h"

Generic_object::Generic_object(){
}  

void Generic_object::drop_to(int x1, int y1){
    x = x1;
    y = y1;
}

int Generic_object::posx(){
    return x;
}

int Generic_object::posy(){
    return y;
}  

    

