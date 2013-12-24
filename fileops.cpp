#include <string>
#include <istream>
#include <fstream>
#include "fileops.h"

bool load_from(std::string filename)
  {
    std::ifstream fin;
    fin.open(filename.c_str());
    if (!fin.is_open()) {
      //debugmsg("Failed to open '%s'", filename.c_str());
      return false;
    }
    /* 
    while (!fin.eof()) {
      if (!load_element(fin)) {
        return false;
      }
    }
    */
    return true;
  }
