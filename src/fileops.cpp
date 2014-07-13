#include <iostream>

#include <string>
#include <istream>
#include <fstream>
#include "fileops.h"  
#include "debug.h" // debugmsg()

std::string no_caps(const std::string &orig)
{
  std::string ret = orig;
  for (unsigned int i = 0; i < ret.length(); i++) {
    if (ret[i] >= 'A' && ret[i] <= 'Z')
      ret[i] += 'a' - 'A';
  }

  return ret;
}

std::string trim(const std::string &orig)
{
  std::string ret = orig;
  unsigned int front = 0, back = ret.length() - 1;
  while (front < ret.length() &&
         (ret[front] == ' ' || ret[front] == '\n' || ret[front] == '\t'))
    front++;

  ret = ret.substr(front);

  back = ret.length() - 1;

  while (back >= 0 &&
         (ret[back] == ' ' || ret[back] == '\n' || ret[back] == '\t'))
    back--;

  ret = ret.substr(0, back + 1);

  return ret;
}

bool load_from(std::string filename, lvl1 &enc)
{
    std::ifstream fin;
    fin.open(filename.c_str());
    if (!fin.is_open()) {
      debugmsg("Failed to open '%s'", filename.c_str());
      return false;
    }
     
    while (!fin.eof()) {
      if (!load_element(fin, enc)) {
        return false;
      }
    }
    
    return true;
}

bool load_from_map(std::string filename, lvl1_map &map){
    std::ifstream fin;
    fin.open(filename.c_str());
    if (!fin.is_open()) {
        debugmsg("Failed to open '%s'", filename.c_str());
        return false;
    }

    map.max_x = 0;
    map.max_y = 0;

    while (!fin.eof()) {
        if (!load_thismap(fin, map)) {
            return false;
        }
    }

    for(unsigned int i = 0; i<map.map_int.size(); ++i)
        std::cout << map.map_int[i] << " " ;
    std::cout << map.max_x << " " ;
    std::cout << map.max_y << " " ;
    return true;
}

bool load_thismap(std::istream &data, lvl1_map &map){
    std::string line;
    getline(data,line); // skip first line
    std::string ident;
    int parseint = 0;
    char parsechar;
    data >> parseint; // x
    if((parseint + 1) > map.max_x) map.max_x = parseint + 1; // set x size
    data >> parsechar;
    data >> parseint; // y
    if((parseint + 1) > map.max_y) map.max_y = parseint + 1; // set y size
    data >> parsechar;
    data >> parseint; // tile
    if(parseint == 219) map.map_int.push_back(0);
    else map.map_int.push_back(1);    
    data >> ident;
    //debugmsg("%s", ident.c_str());
    return true;
}    

bool load_element(std::istream &data, lvl1 &enc)
{
    if (!load_data(data, enc)) {
        debugmsg("Failed taking data from file!");
        return false;
    }

      
    return true;
    /*  
    T* tmp = new T;
    
    tmp->assign_uid(next_uid);
    instances.push_back(tmp);
    uid_map[next_uid] = tmp;
    name_map[tmp->get_name()] = tmp;
    next_uid++;
    return true;
    */
  }


bool load_data(std::istream &data, lvl1 &enc){
    std::string ident, junk;
    //do {
        for(int x = 0; x < 3; x++){
            if ( ! (data >> ident) ) { // loads a words, removes it from stream
                return false; // finds nothing in the file
            }
            ident = no_caps(ident);
            switch (x){
                case 0:
                    if (ident == "cave") { debugmsg("%s", ident.c_str()); } // looks at the word it grabbed
                    break;
                case 1:    
                    if (ident == "lvl") { debugmsg("%s", ident.c_str()); }
                    break;
                case 2:
                    if (ident == "1") { debugmsg("%s", ident.c_str()); }
                    break;
                default:
                    debugmsg("I'm broken.");
                    break;
            } 
        } 
        // encounter block
        char parse;
        int parseint = 0; // temp int to use in push back
        data >> parse;
        while (parse != '[') {
            data >> parse;
        }    
        data >> parse; // e
        while (parse != 'e' || parse != ']') {
            if(parse == ']') break; // break while, hopefully
            if(parse == 'e'){
                std::vector<int> l_enc;
                room_enc t_room; // temp object
                data >> parse; // [
                data >> parseint; // load first element after e
                l_enc.push_back(parseint);
                std::cout << "countnum: " << parseint << std::endl;
                data >> parse;
                while(parse != ':'){
                    if(parse == ','){
                        data >> parseint;
                        l_enc.push_back(parseint);
                        std::cout << "countnum: " << parseint << std::endl;
                    }  
                    data >> parse; // load anothr char for the while
                }        
                if(parse == ':'){     
                    data >> t_room.probability;
                    t_room.enc = l_enc;
                    enc.cave1.push_back(t_room);
                    std::cout << "percent: " << t_room.probability << std::endl;
                    data >> parse;
                }    
            }
            data >> parse;
        }

        // check if all encounters fill all possibilities
        std::cout << "total percent: " << enc.cave1[enc.cave1.size()-1].probability << std::endl;
        if (enc.cave1[enc.cave1.size()-1].probability != 100) return false; // fail
        
        // monster types block
        data >> ident; // monster number type ID (not used?)
        while(ident != "end" && !data.eof()){ // stops on "end" or end of file
            debugmsg("%s", ident.c_str());
            data >> ident; // [
            mob_types tempmob; // object
            while(ident != "]"){
                if (ident == "s_hp:") {data >> tempmob.s_hp;}
                else if (ident == "s_defense:") {data >> tempmob.s_defense;}
                else if (ident == "s_power:") {data >> tempmob.s_power;}
                else if (ident == "s_speed:") {data >> tempmob.s_speed;}
                else if (ident == "name:") {data >> ident; strcpy(tempmob.name, ident.c_str());}
                else if (ident == "selfchar:") {data >> tempmob.selfchar;}
                else if (ident == "self_8:") {data >> tempmob.self_8;}
                else if (ident == "self_16:") {data >> tempmob.self_16;}
                else if (ident == "color:") {
                    int a, b, c = 0;
                    data >> a; data >> parse;
                    data >> b; data >> parse;
                    data >> c;
                    TCODColor tempcolor(a, b, c);
                    tempmob.color = tempcolor;
                }    
                else if (ident == "colorb:") {data >> ident;}
                else if (ident == "h:") {data >> tempmob.h;}
                else if (ident == "combat_move:") {data >> tempmob.combat_move;}
                else if (ident == "speed:") {data >> tempmob.speed;}
                else if (ident == "wpn_AC:") {data >> tempmob.wpn_AC;}
                else if (ident == "wpn_DC:") {data >> tempmob.wpn_DC;}
                else if (ident == "wpn_B:") {data >> tempmob.wpn_B;}
                else if (ident == "wpn_aspect:") {data >> tempmob.wpn_aspect;}
                else if (ident == "ML:") {data >> tempmob.ML; }   
                data >> ident;
            } 
            enc.vmob_types.push_back(tempmob);
            data >> ident;
        }
        return true;
}
