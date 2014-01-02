#ifndef _FILEOPS_H_
#define _FILEOPS_H_

#include <string>

bool load_from(std::string filename);
bool load_element(std::istream &data);
bool load_data(std::istream&);

#endif
