// WHALES!

#include <random>
#include "rng.h"

long rng(long low, long high)
{
    //static std::default_random_engine rnumber; // uses minstd_rand0 ?
    static std::mt19937 rnumber; // uses Mersenne Twister
    std::uniform_int_distribution<uint32_t> u(low, high);
    return u(rnumber);
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
