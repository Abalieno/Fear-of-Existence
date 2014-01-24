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
        int parseint = 0;

        data >> parse;
        while (parse != '[') {
            data >> parse;
        }    
        data >> parse; // e
        while (parse != 'e' || parse != ']') {
            if(parse == ']') break; // break while, hopefully
            if(parse == 'e'){
                data >> parse; // [
                data >> parseint; // load first element after e
                std::cout << "countnum: " << parseint << std::endl;
                data >> parse;
                while(parse != ':'){
                    if(parse == ','){
                        data >> parseint;
                        std::cout << "countnum: " << parseint << std::endl;
                    }  
                    data >> parse; // load anothr char for the while
                }        
                if(parse == ':'){     
                    data >> parseint;
                    std::cout << "percent: " << parseint << std::endl;
                    data >> parse;
                }    
            }
            data >> parse;
        }
        
        // monster types block
        data >> ident; // monster number type ID (not used?)
        while(ident != "end" && !data.eof()){ // stops on "end" or end of file
            debugmsg("%s", ident.c_str());
            data >> ident; // [
            while(ident != "]"){
                if (ident == "s_hp:") {data >> ident;}
                else if (ident == "s_defense:") {data >> ident;}
                else if (ident == "s_power:") {data >> ident;}
                else if (ident == "s_speed:") {data >> ident;}
                else if (ident == "name:") {data >> ident; debugmsg("%s", ident.c_str());}
                else if (ident == "selfchar:") {data >> ident;}
                else if (ident == "color:") {data >> ident;}
                else if (ident == "colorb:") {data >> ident;}
                else if (ident == "h:") {data >> ident;}
                else if (ident == "combat_move:") {data >> ident;}
                else if (ident == "speed:") {data >> ident;}
                else if (ident == "wpn_AC:") {data >> ident;}
                else if (ident == "wpn_DC:") {data >> ident;}
                else if (ident == "wpn_B:") {data >> ident;}
                else if (ident == "wpn_aspect:") {data >> ident;}
                else if (ident == "ML:") {data >> ident; }   
                data >> ident;
            }   
            data >> ident;
        }
        return true;
}
