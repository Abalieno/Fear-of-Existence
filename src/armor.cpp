#include <iostream>
#include <iomanip>

#include <windows.h> // for strcopy

#include "armor.h"

int ratio(int location){
    if(location == SK) return 4;
    else if(location == FA) return 3; 
    else if(location == NK) return 2;

    else if(location == TX) return 12;
    else if(location == AB) return 12;
    else if(location == HP) return 9;
    else if(location == GR) return 1;

    else if(location == SH) return 3;
    else if(location == UA) return 6;
    else if(location == EL) return 2;
    else if(location == FO) return 5;
    else if(location == HA) return 5;

    else if(location == TH) return 14;
    else if(location == KN) return 3;
    else if(location == CF) return 12;
    else if(location == FE) return 7;
    return 666;
} 

//printf("%.2f\n", x/y);

float calc_weight(Armor &piece){
    int target;
    switch(piece.material){
        case LEATHER:
            target = 12;
            break;
        case SCALE:
            target = 45;
            break;
        case PLATE:
            target = 80;
            break;
    }  
    float ratio = 0; // to make division work, needs float
    for(unsigned int n = 0; n < piece.loc.size(); ++n){
        ratio += piece.loc[n].second;
    }
    return (ratio * target) / 100;
}  

Armor make_piece(const char *name, int material, std::vector<int> location_list){
    Armor temp_piece;
    strcpy(temp_piece.name, name);
    temp_piece.material = material;
    for(unsigned int n = 0; n < location_list.size(); ++n){
        temp_piece.loc.push_back(std::pair<int, int> (location_list[n], ratio(location_list[n])));
    }   
    temp_piece.weight = calc_weight(temp_piece);
    return temp_piece;
}    
