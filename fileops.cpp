#include <string>
#include <istream>
#include <fstream>
#include "fileops.h"  
#include "debug.h" // debugmsg()

bool load_from(std::string filename)
{
    std::ifstream fin;
    fin.open(filename.c_str());
    if (!fin.is_open()) {
      debugmsg("Failed to open '%s'", filename.c_str());
      return false;
    }
     
    while (!fin.eof()) {
      if (!load_element(fin)) {
        return false;
      }
    }
    
    return true;
}

bool load_element(std::istream &data)
{
    if (!load_data(data)) {
        debugmsg("Failed!");
        debugmsg("Failed!");
        debugmsg("Failed!");
        debugmsg("Failed!");
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

bool load_data(std::istream&){
    return false;
}
