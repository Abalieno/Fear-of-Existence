// WHALES!

#ifndef _RNG_H_
#define _RNG_H_

#include <random>

static std::uniform_int_distribution<long> u1d4     {1, 4};
static std::uniform_int_distribution<long> u1d6     {1, 6};
static std::uniform_int_distribution<long> u1d8     {1, 8};
static std::uniform_int_distribution<long> u1d10    {1, 10};
static std::uniform_int_distribution<long> u1d20    {1, 20};
static std::uniform_int_distribution<long> u1d100   {1, 100};

void rng_init();
long rng(long low, long high);
bool one_in(int chance);
int dice(int number, int sides);

#endif
