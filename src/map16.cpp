#include "libtcod.hpp"

void map_16x16_tile(){
    TCODConsole::mapAsciiCodeToFont(501,14,16);
    TCODConsole::mapAsciiCodeToFont(601,15,16);
    TCODConsole::mapAsciiCodeToFont(701,14,17);
    TCODConsole::mapAsciiCodeToFont(801,15,17); // tile #1 floor

    TCODConsole::mapAsciiCodeToFont(502,10,16);
    TCODConsole::mapAsciiCodeToFont(602,11,16);
    TCODConsole::mapAsciiCodeToFont(702,10,17);
    TCODConsole::mapAsciiCodeToFont(802,11,17); // tile #2 player

    TCODConsole::mapAsciiCodeToFont(503,12,16);
    TCODConsole::mapAsciiCodeToFont(603,13,16);
    TCODConsole::mapAsciiCodeToFont(703,12,17);
    TCODConsole::mapAsciiCodeToFont(803,13,17); // tile #3 null

    TCODConsole::mapAsciiCodeToFont(504,10,18);
    TCODConsole::mapAsciiCodeToFont(604,11,18);
    TCODConsole::mapAsciiCodeToFont(704,10,19);
    TCODConsole::mapAsciiCodeToFont(804,11,19); // tile #4 troll

    TCODConsole::mapAsciiCodeToFont(505,10,20);
    TCODConsole::mapAsciiCodeToFont(605,11,20);
    TCODConsole::mapAsciiCodeToFont(705,10,21);
    TCODConsole::mapAsciiCodeToFont(805,11,21); // tile #5 orc

    TCODConsole::mapAsciiCodeToFont(506,10,22);
    TCODConsole::mapAsciiCodeToFont(606,11,22);
    TCODConsole::mapAsciiCodeToFont(706,10,23);
    TCODConsole::mapAsciiCodeToFont(806,11,23); // tile #6 corpse

    TCODConsole::mapAsciiCodeToFont(507,14,18);
    TCODConsole::mapAsciiCodeToFont(607,15,18);
    TCODConsole::mapAsciiCodeToFont(707,14,19);
    TCODConsole::mapAsciiCodeToFont(807,15,19); // tile #7 wall

    TCODConsole::mapAsciiCodeToFont(508,14,20);
    TCODConsole::mapAsciiCodeToFont(608,15,20);
    TCODConsole::mapAsciiCodeToFont(708,14,21);
    TCODConsole::mapAsciiCodeToFont(808,15,21); // tile #8 door

    TCODConsole::mapAsciiCodeToFont(509,14,22);
    TCODConsole::mapAsciiCodeToFont(609,15,22);
    TCODConsole::mapAsciiCodeToFont(709,14,23);
    TCODConsole::mapAsciiCodeToFont(809,15,23); // tile #9 floor.a

    TCODConsole::mapAsciiCodeToFont(510,14,24);
    TCODConsole::mapAsciiCodeToFont(610,15,24);
    TCODConsole::mapAsciiCodeToFont(710,14,25);
    TCODConsole::mapAsciiCodeToFont(810,15,25); // tile #10 door.h

    TCODConsole::mapAsciiCodeToFont(511,14,26);
    TCODConsole::mapAsciiCodeToFont(611,15,26);
    TCODConsole::mapAsciiCodeToFont(711,14,27);
    TCODConsole::mapAsciiCodeToFont(811,15,27); // tile #11 door.v

    TCODConsole::mapAsciiCodeToFont(512,14,28);
    TCODConsole::mapAsciiCodeToFont(612,15,28);
    TCODConsole::mapAsciiCodeToFont(712,14,29);
    TCODConsole::mapAsciiCodeToFont(812,15,29); // tile #12 wall

    TCODConsole::mapAsciiCodeToFont(513,14,30);
    TCODConsole::mapAsciiCodeToFont(613,15,30);
    TCODConsole::mapAsciiCodeToFont(713,14,31);
    TCODConsole::mapAsciiCodeToFont(813,15,31); // tile #13 wall.T-left

    TCODConsole::mapAsciiCodeToFont(514,14,32);
    TCODConsole::mapAsciiCodeToFont(614,15,32);
    TCODConsole::mapAsciiCodeToFont(714,14,33);
    TCODConsole::mapAsciiCodeToFont(814,15,33); // tile #14 wall.v

    TCODConsole::mapAsciiCodeToFont(515,14,34);
    TCODConsole::mapAsciiCodeToFont(615,15,34);
    TCODConsole::mapAsciiCodeToFont(715,14,35);
    TCODConsole::mapAsciiCodeToFont(815,15,35); // tile #15 wall.B

    TCODConsole::mapAsciiCodeToFont(516,14,36);
    TCODConsole::mapAsciiCodeToFont(616,15,36);
    TCODConsole::mapAsciiCodeToFont(716,14,37);
    TCODConsole::mapAsciiCodeToFont(816,15,37); // tile #16 wall.full

    TCODConsole::mapAsciiCodeToFont(517,14,38);
    TCODConsole::mapAsciiCodeToFont(617,15,38);
    TCODConsole::mapAsciiCodeToFont(717,14,39);
    TCODConsole::mapAsciiCodeToFont(817,15,39); // tile #17 wall.T-right

    TCODConsole::mapAsciiCodeToFont(518,14,40);
    TCODConsole::mapAsciiCodeToFont(618,15,40);
    TCODConsole::mapAsciiCodeToFont(718,14,41);
    TCODConsole::mapAsciiCodeToFont(818,15,41); // tile #18 wall.B-inv


    TCODConsole::mapAsciiCodeToFont(519,14,42);
    TCODConsole::mapAsciiCodeToFont(619,15,42);
    TCODConsole::mapAsciiCodeToFont(719,14,43);
    TCODConsole::mapAsciiCodeToFont(819,15,43); // tile 28_door

    TCODConsole::mapAsciiCodeToFont(520,14,44);
    TCODConsole::mapAsciiCodeToFont(620,15,44);
    TCODConsole::mapAsciiCodeToFont(720,14,45);
    TCODConsole::mapAsciiCodeToFont(820,15,45); // tile 28_player

    TCODConsole::mapAsciiCodeToFont(521,14,46);
    TCODConsole::mapAsciiCodeToFont(621,15,46);
    TCODConsole::mapAsciiCodeToFont(721,14,47);
    TCODConsole::mapAsciiCodeToFont(821,15,47); // tile 28_wall

    TCODConsole::mapAsciiCodeToFont(522,14,48);
    TCODConsole::mapAsciiCodeToFont(622,15,48);
    TCODConsole::mapAsciiCodeToFont(722,14,49);
    TCODConsole::mapAsciiCodeToFont(822,15,49); // tile 28_floor1

    TCODConsole::mapAsciiCodeToFont(523,14,50);
    TCODConsole::mapAsciiCodeToFont(623,15,50);
    TCODConsole::mapAsciiCodeToFont(723,14,51);
    TCODConsole::mapAsciiCodeToFont(823,15,51); // tile 28_floor2

    TCODConsole::mapAsciiCodeToFont(524,14,52);
    TCODConsole::mapAsciiCodeToFont(624,15,52);
    TCODConsole::mapAsciiCodeToFont(724,14,53);
    TCODConsole::mapAsciiCodeToFont(824,15,53); // tile 28_orc

    TCODConsole::mapAsciiCodeToFont(525,14,54);
    TCODConsole::mapAsciiCodeToFont(625,15,54);
    TCODConsole::mapAsciiCodeToFont(725,14,55);
    TCODConsole::mapAsciiCodeToFont(825,15,55); // tile 28_troll

    TCODConsole::mapAsciiCodeToFont(526,14,56);
    TCODConsole::mapAsciiCodeToFont(626,15,56);
    TCODConsole::mapAsciiCodeToFont(726,14,57);
    TCODConsole::mapAsciiCodeToFont(826,15,57); // tile 28_corpse
}
