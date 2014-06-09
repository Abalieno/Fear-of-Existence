// WHALES!

#include <random>
//#include <iostream>
#include "rng.h"

void rng_init(){
    static std::mt19937 rnumber; // uses Mersenne Twister
    rnumber.seed(1);
}    

long rng(long low, long high)
{
    //static std::default_random_engine rnumber; // uses minstd_rand0 ?
    static std::mt19937 rnumber; // uses Mersenne Twister
    if(low == 1 && (high == 4 || high == 6 || high == 8 || high == 10 || high == 20 || high == 100) ){
        switch (high){
            case 4:
                return u1d4(rnumber);
                break;
            case 6:
                return u1d6(rnumber);
                break;
            case 8:
                return u1d8(rnumber);
                break;
            case 10:
                return u1d10(rnumber);
                break;
            case 20:
                return u1d20(rnumber);
                break;
            case 100:
                return u1d100(rnumber);
                break;
            default:
                return 0;
        }    
    } else {    
        std::uniform_int_distribution<long> u {low, high};
        return u(rnumber);
    }    
}

bool one_in(int chance)
{
 if (chance <= 1 || rng(0, chance - 1) == 0)
  return true;
 return false;
}

int dice(int number, int sides)
{
 int ret = 0;
 for (int i = 0; i < number; i++)
  ret += rng(1, sides);
 return ret;
}
