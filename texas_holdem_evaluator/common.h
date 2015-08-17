#ifndef COMMON_H_
#define COMMON_H_
#include "stdio.h"
#pragma pack(1)

typedef unsigned char uint8;
typedef unsigned int uint32;

enum
{
	SPADES = 0,
	HEARTS,
	CLUBS,
	DIAMONDS
};

enum
{
	HOLD = 0,
	FLOP,
	TURN,
	RIVER
};

enum
{
	CHECK = 0,
	CALL,
	RAISE,
	ALL_IN,
	FOLD,
	CMD_NUM
};


typedef struct _card
{
	uint8 color_val;
	uint8 card_val;
}CARD;


typedef struct
{
	int pot_val;
	int jetton_val;
	int money_val;
	int bet;
}MONEY;

typedef struct
{
	char idbuff[10];
	uint8 id_length;
}PLAYER_ID;

#define pot_com_len 12
//#define DEBUG
#pragma pack()

#endif
