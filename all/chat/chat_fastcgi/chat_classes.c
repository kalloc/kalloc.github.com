#include <stdio.h>
#include <stdlib.h>
#ifdef BUILD

#define NETH
#define DEFINE
#include "var.h"
#include <ctype.h>
#include <iconv.h>

#endif
/*

I variant
    fraction	2 bit
    race	4 bit
    spec	8 bit
    prof	32 bit
    
II variant
    fraction/race char
    spec	  char
    prof	  unsigned int    

*/


#define RACE_WEST 0x01
#define RACE_EAST 0x02



#define WARRIOR_MASK 1
#define MAG_MASK 2
#define MONK_MASK 4
#define THEIF_MASK 8
#define PATHFINDER_MASK 16
#define PALADIN_MASK 32
#define DRUID_MASK 64
#define NECROMANT_MASK 128

char EMPTY[]="";
int race;
int fraction;
int prof;


static struct classess {char *name_eng;char *name_rus;int fracmask;int bitmask;} classes[] = {
        {0,0},
        {"warrior","воин", WARRIOR_MASK},
        {"mag","маг", MAG_MASK},
        {"monk","монах", MONK_MASK},
	{"theif","вор", THEIF_MASK},
        {"pathfinder","следопыт", PATHFINDER_MASK},
        {"paladin","паладин", PALADIN_MASK},
        {"druid","друид", DRUID_MASK},
        {"necromant","некромант", NECROMANT_MASK}
};


static struct fractionss {char *name_eng;char *name_rus;int race;int bitmask;} fractions[] = {
        {0,0},
        {"humans","люди",RACE_WEST, WARRIOR_MASK+MAG_MASK+MONK_MASK+THEIF_MASK+PATHFINDER_MASK+PALADIN_MASK},
        {"dwarfs","гномы",RACE_WEST, WARRIOR_MASK+MAG_MASK+THEIF_MASK+PALADIN_MASK},
        {"orks","орки",RACE_EAST,  WARRIOR_MASK+THEIF_MASK+PATHFINDER_MASK},
	{"daemons","демоны",RACE_EAST,WARRIOR_MASK+MAG_MASK+MONK_MASK+THEIF_MASK+PATHFINDER_MASK+NECROMANT_MASK}
};
										
/*

I variant
    fraction	2 bit
    race	4 bit
    spec	8 bit
    prof	32 bit
    
II variant
    fraction/race char
    spec	  char
    prof	  unsigned int    




*/

void init_frac(char ch) {
    if(((ch & 0xf0) == 0x0) || ((ch & 0x0f) == 0) )  {
        fraction=race=0;
    } else {
        fraction=ch & 0xf;
	race=ch>>4;
    }
}


int check_frac(char ch) {
    init_frac(ch);
    if(fraction != RACE_WEST && fraction != RACE_EAST) return 0;
    if(fractions[race].name_eng == NULL) return 0;
    return 1;
}

int check_kingdom(char ch) {
    if(ch != RACE_WEST && ch != RACE_EAST) return 0;
    return 1;
}


char * get_class(unsigned char ch) {
    if(race ==0 || classes[ch].name_eng == NULL) return EMPTY;
    return classes[ch].name_eng;
}

char * get_frac() {
    if(race ==0 || fractions[race].name_eng == NULL) return EMPTY;
    return fractions[race].name_eng;
}


int check_class(unsigned char ch) {
    
    if(race >0 && (classes[ch].fracmask & fractions[race].bitmask) == classes[ch].fracmask) return 1;
    return 0;
}

int check_mask_class(unsigned char ch) {
    if( (ch&fractions[race].bitmask) == ch) return 1;
    return 0;
}






#ifndef BUILD
int main () {

    printf("frac ret:%d\n",check_frac(0x01));
    printf("frac ret:%s\n",get_frac());
    printf("check class ret:%x\n",check_class(2));
    printf("check mask class ret:%x\n",check_mask_class(MONK_MASK));
    printf("get class ret:%s\n",get_class(2));
    printf("fr=%x : race=%x : prof=%x\n",fraction,race,prof);
    return 0;
}
#endif

//EOF
