#include <fstream>
#include <iostream>

#include "loader.h"
#include "fileops.h"


void load_stuff(){
    if(load_from("dummy.txt")) std::cout << "yay!" << std::endl;
    else std::cout << "nay!" << std::endl;
    return;
}    
