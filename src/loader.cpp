#include <fstream>
#include <iostream>

#include "loader.h"
#include "fileops.h"


void load_stuff(lvl1 &enc){
    if(load_from("dummy.txt", enc)) std::cout << "yay!" << std::endl;
    else std::cout << "nay!" << std::endl;
    return;
}  

void load_level(lvl1_map &map){
    if(load_from_map("first.csv", map)) std::cout << "Loading map from csv successful!" << std::endl;
    else std::cout << "Problem loading map from csv!" << std::endl;
    return;
}
