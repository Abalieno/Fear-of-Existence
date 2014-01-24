#include <fstream>
#include <iostream>

#include "loader.h"
#include "fileops.h"


void load_stuff(lvl1 &enc){
    if(load_from("dummy.txt", enc)) std::cout << "yay!" << std::endl;
    else std::cout << "nay!" << std::endl;
    return;
}    
