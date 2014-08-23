#ifndef _ARMOR_H_
#define _ARMOR_H_

#include <vector>

#define PLATE 0
#define SCALE 1
#define LEATHER 6

#define SK  0
#define FA  1
#define NK  2

#define TX  3
#define AB  4
#define HP  5
#define GR  6

#define SH  7
#define UA  8
#define EL  9
#define FO  10
#define HA  11

#define TH  12
#define KN  13
#define CF  14
#define FE  15

class Armor{
    public:
        char name[20];
        int material;
        float weight;
        std::vector<std::pair<int,int>> loc;
};

Armor make_piece(const char *name, int material, std::vector<int> location_list);

int ratio(int location);

float calc_weight(Armor &piece);

#endif
