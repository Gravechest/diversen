#pragma once

#define AIMAXTHINKINGTIME 999999999
#define MTIME AIMAXTHINKINGTIME << 16
#define DEPTH 6

#pragma warning(disable:4996)

typedef char i8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long long u64;

typedef struct{
    u8 src;
    u8 dst;
}CHESSMOVE;

typedef struct{
    i8 alpha;
    i8 beta;
}ALPHABETA;

typedef struct{
    i8 pts;
    u8 movC;
    i8 minDpt;
}AIMOVEDATA;

typedef struct{
    ALPHABETA ab;
    i8 pts;
}EVAL;

typedef struct{
    CHESSMOVE *mov;
}CHESSMASTER;

extern char board[64];
extern char pcsVal[122];
i8 i8max(i8 p1,i8 p2);
i8 i8min(i8 p1,i8 p2);
void movePieceD(u8 src,u8 dst);
void capturePieceD(u8 src,u8 dst);
