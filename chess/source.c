#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <intrin.h>

#pragma once

#pragma warning(disable:4996)

#define AIMAXTHINKINGTIME 999999999
#define MTIME AIMAXTHINKINGTIME << 16
#define DEPTH 7

typedef char               i1;
typedef short              i2;
typedef int                i4;
typedef long long          i8;
typedef unsigned char      u1;
typedef unsigned short     u2;
typedef unsigned int       u4;
typedef unsigned long long u8;
typedef float              f4;
typedef double             f8;

typedef struct{
    u1 src;
    u1 dst;
}CHESSMOVE;

typedef struct{
    i1 alpha;
    i1 beta;
}ALPHABETA;

typedef struct{
    i1 pts;
    u1 movC;
    i1 minDpt;
}AIMOVEDATA;

typedef struct{
    ALPHABETA ab;
    i1 pts;
}EVAL;

typedef struct{
    CHESSMOVE *mov;
}CHESSMASTER;

CHESSMASTER master;

i1 i1max(i1 p1,i1 p2){
    return p1 > p2 ? p1 : p2;
}

i1 i1min(i1 p1,i1 p2){
    return p1 < p2 ? p1 : p2;
}

i1 board[64] ={
    'r','n','b','q','k','b','n','r',
    'p','p','p','p','p','p','p','p',
    '_','_','_','_','_','_','_','_',
    '_','_','_','_','_','_','_','_',
    '_','_','_','_','_','_','_','_',
    '_','_','_','_','_','_','_','_',
    'P','P','P','P','P','P','P','P',
    'R','N','B','Q','K','B','N','R',
};

i1 pcsVal[122] ={
    ['p'] = -1,['b'] = -3,['n'] = -3,['r'] = -5,['q'] = -9,['k'] = -75,
    ['P'] =  1,['B'] =  3,['N'] =  3,['R'] =  5,['Q'] =  9,['K'] =  75
};

u1 castleRightWhiteShort = 1;
u1 castleRightWhiteLong = 1;
u1 castleRightBlackShort = 1;
u1 castleRightBlackLong = 1;

i1 minBlackAI(u1 depth,EVAL eval);
i1 minWhiteAI(u1 depth,EVAL eval);
i1 maxBlackAI(u1 depth,EVAL eval);
i1 maxWhiteAI(u1 depth,EVAL eval);

void pcsValWhite(){
    pcsVal['p'] = 1;
    pcsVal['b'] = 3;
    pcsVal['n'] = 3;
    pcsVal['r'] = 5;
    pcsVal['q'] = 9;
    pcsVal['k'] = 75;
    pcsVal['P'] = -1;
    pcsVal['B'] = -3;
    pcsVal['N'] = -3;
    pcsVal['R'] = -5;
    pcsVal['Q'] = -9;
    pcsVal['K'] = -75;
}

void pcsValBlack(){
    pcsVal['p'] = -1;
    pcsVal['b'] = -3;
    pcsVal['n'] = -3;
    pcsVal['r'] = -5;
    pcsVal['q'] = -9;
    pcsVal['k'] = -75;
    pcsVal['P'] = 1;
    pcsVal['B'] = 3;
    pcsVal['N'] = 3;
    pcsVal['R'] = 5;
    pcsVal['Q'] = 9;
    pcsVal['K'] = 75;
}

void printBoard(){
    printf("   ");
    for(i4 i = 0;i < 8;i++) {
        printf("%c",'A'+i);
        printf("%c",' ');
    }
    printf("\n  ");
    for(i4 i = 0; i < 16; i++) printf("%c",'-');
    printf("%c",'\n');
    for(i4 i = 0;i < 8;i++) {
        printf("%i",i);
        printf("| ");
        for(i4 j = 0; j < 8; j++) {
            if(board[i*8+j] == '_'){
                if((j + (i & 1)) & 1) printf("%c",'.');
                else printf("%c",',');
            }
            else printf("%c",board[i*8+j]);
            printf("%c",' ');
        }
        printf("%c",'\n');
    }
    printf("  ");
    for(i4 i = 0;i < 16;i++) printf("%c",'-');
    printf("%c",'\n');
}

void movePiece(u1 src,u1 dst){
    i1 t = board[src];
    board[src] = board[dst];
    board[dst] = t;
}

void capturePiece(u1 src,u1 dst){
    board[dst] = board[src];
    board[src] = '_';
}

void movePieceWhite(u1 src,u1 dst){
    if(board[dst] == '_') movePiece(src,dst);
    else if(board[dst] >= 'a' && board[dst] <= 'z') capturePiece(src,dst);
}

void movePieceBlack(u1 src,u1 dst){
    if(board[dst] == '_') movePiece(src,dst);
    else if(board[dst] >= 'A' && board[dst] <= 'Z') capturePiece(src,dst);
}

void whitePromotion(u1 src,u1 dst){
    if((dst>>3) == 0){
        i1 inp;
        printf("promotion");
        do scanf("%s",&inp);
        while(inp!='Q'&&inp!='R'&&inp!='N'&&inp!='B');
        board[src] = inp;
    }
}

void blackPromotion(u1 src,u1 dst){
    if((dst>>3) == 7){
        i1 inp;
        printf("promotion");
        do scanf("%s",&inp);
        while(inp!='q'&&inp!='r'&&inp!='n'&&inp!='b');
        board[src] = inp;
    }
}

void printMove(u1 src,u1 dst){
    printf("%c",(src&7)+'A');
    printf("%c",(src>>3)+'0');
    printf("%c",'-');
    printf("%c",(dst&7)+'A');
    printf("%c = ",(dst>>3)+'0');
}

i1 maxBlackAIdiagonal0direction(u1 origin,i1 dir,i1 bound,i1 pts,i1 tpts){
    for(u4 i = origin;i < 64 && (i & 7) != bound && board[i] < 'a';i+=dir){
        if(board[i] < 'Z'){
            tpts = i1max(tpts,pts+pcsVal[board[i]]);
            break;
        }
    }
    return tpts;
}

i1 maxWhiteAIdiagonal0direction(u1 origin,i1 dir,i1 bound,i1 pts,i1 tpts){
    for(u4 i = origin;i < 64 && (i & 7) != bound && board[i] > 'Z';i+=dir){
        if(board[i] > 'a'){
            tpts = i1max(tpts,pts+pcsVal[board[i]]);
            break;
        }
    }
    return tpts;
}

i1 minBlackAIdiagonal0direction(u1 origin,i1 dir,i1 bound,i1 pts,i1 tpts){
    for(u4 i = origin;i < 64 && (i & 7) != bound && board[i] > 'Z';i+=dir){
        if(board[i] > 'a'){
            tpts = i1min(tpts,pts+pcsVal[board[i]]);
            break;
        }
    }
    return tpts;
}

i1 minWhiteAIdiagonal0direction(u1 origin,i1 dir,i1 bound,i1 pts,i1 tpts){
    for(u4 i = origin;i < 64 && (i & 7) != bound && board[i] < 'a';i+=dir){
        if(board[i] < 'Z'){
            tpts = i1min(tpts,pts+pcsVal[board[i]]);
            break;
        }
    }
    return tpts;
}

i1 maxBlackAIdiagonal0(u1 i,i1 pts,i1 tpts){
    tpts = maxBlackAIdiagonal0direction(i - 9,-9,7,pts,tpts);
    tpts = maxBlackAIdiagonal0direction(i - 7,-7,0,pts,tpts);
    tpts = maxBlackAIdiagonal0direction(i + 9, 9,0,pts,tpts);
    tpts = maxBlackAIdiagonal0direction(i + 7, 7,7,pts,tpts);
    return tpts;
}

i1 maxWhiteAIdiagonal0(u1 i,i1 pts,i1 tpts){
    tpts = maxWhiteAIdiagonal0direction(i - 9,-9,7,pts,tpts);
    tpts = maxWhiteAIdiagonal0direction(i - 7,-7,0,pts,tpts);
    tpts = maxWhiteAIdiagonal0direction(i + 9, 9,0,pts,tpts);
    tpts = maxWhiteAIdiagonal0direction(i + 7, 7,7,pts,tpts);
    return tpts;
}

i1 minBlackAIdiagonal0(u1 i,i1 pts,i1 tpts){
    tpts = minBlackAIdiagonal0direction(i - 9,-9,7,pts,tpts);
    tpts = minBlackAIdiagonal0direction(i - 7,-7,0,pts,tpts);
    tpts = minBlackAIdiagonal0direction(i + 9, 9,0,pts,tpts);
    tpts = minBlackAIdiagonal0direction(i + 7, 7,7,pts,tpts);
    return tpts;
}

i1 minWhiteAIdiagonal0(u1 i,i1 pts,i1 tpts){
    tpts = minWhiteAIdiagonal0direction(i - 9,-9,7,pts,tpts);
    tpts = minWhiteAIdiagonal0direction(i - 7,-7,0,pts,tpts);
    tpts = minWhiteAIdiagonal0direction(i + 9, 9,0,pts,tpts);
    tpts = minWhiteAIdiagonal0direction(i + 7, 7,7,pts,tpts);
    return tpts;
}

i1 maxBlackAIstraight0(u1 i,i1 pts,i1 tpts){
    for(i4 j = i + 1;(j & 7) != 0 && board[j] < 'a';j++){
        if(board[j] < 'Z'){
            tpts = i1max(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i - 1;(j & 7) != 7 && board[j] < 'a';j--){
        if(board[j] < 'Z'){
            tpts = i1max(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i - 8;j >= 0 && board[j] < 'a';j-=8){
        if(board[j] < 'Z'){
            tpts = i1max(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i + 8;j < 64 && board[j] < 'a';j+=8){
        if(board[j] < 'Z'){
            tpts = i1max(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    return tpts;
}

i1 maxWhiteAIstraight0(u1 i,i1 pts,i1 tpts){
    for(i4 j = i + 1;(j & 7) != 0 && board[j] > 'Z';j++){
        if(board[j] > 'a'){
            tpts = i1max(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i - 1;(j & 7) != 7 && board[j] > 'Z';j--){
        if(board[j] > 'a'){
            tpts = i1max(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i - 8;j >= 0 && board[j] > 'Z';j-=8){
        if(board[j] > 'a'){
            tpts = i1max(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i + 8;j < 64 && board[j] > 'Z';j+=8){
        if(board[j] > 'a'){
            tpts = i1max(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    return tpts;
}

i1 minBlackAIstraight0(u1 i,i1 pts,i1 tpts){
    for(i4 j = i + 1;(j & 7) != 0 && board[j] > 'Z';j++){
        if(board[j] > 'a'){
            tpts = i1min(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i - 1;(j & 7) != 7 && board[j] > 'Z';j--){
        if(board[j] > 'a'){
            tpts = i1min(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i - 8;j >= 0 && board[j] > 'Z';j-=8){
        if(board[j] > 'a'){
            tpts = i1min(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i + 8;j < 64 && board[j] > 'Z';j+=8){
        if(board[j] > 'a'){
            tpts = i1min(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    return tpts;
}

i1 minWhiteAIstraight0(u1 i,i1 pts,i1 tpts){
    for(i4 j = i + 1; (j & 7) != 0 && board[j] < 'a';j++){
        if(board[j] < 'Z'){
            tpts = i1min(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i - 1; (j & 7) != 7 && board[j] < 'a';j--){
        if(board[j] < 'Z'){
            tpts = i1min(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i - 8;j >= 0 && board[j] < 'a';j-=8){
        if(board[j] < 'Z'){
            tpts = i1min(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    for(i4 j = i + 8;j < 64 && board[j] < 'a';j+=8){
        if(board[j] < 'Z'){
            tpts = i1min(tpts,pts+pcsVal[board[j]]);
            break;
        }
    }
    return tpts;
}

void maxBlackAIdiagonalDirection(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir,u1 bound){
    for(u1 j = i + dir;board[j] < 'a' && j < 64 && (j & 7) != bound;j+=dir){
        if(board[j] < 'Z'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1max(*tpts,minBlackAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void maxWhiteAIdiagonalDirection(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir,u1 bound){
    for(u1 j = i + dir;j < 64 && (j & 7) != bound && board[j] > 'Z';j+=dir){
        if(board[j] > 'a'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1max(*tpts,minWhiteAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void minBlackAIdiagonalDirection(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir,u1 bound){
    for(u1 j = i + dir;j < 64 && (j & 7) != bound && board[j] > 'Z';j+=dir){
        if(board[j] > 'a'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1min(*tpts,maxBlackAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void minWhiteAIdiagonalDirection(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir,u1 bound){
    for(u1 j = i + dir;j < 64 && (j & 7) != bound && board[j] < 'a';j+=dir){
        if(board[j] < 'Z'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1min(*tpts,maxWhiteAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void maxBlackAIdiagonal(u1 i,u1 depth,EVAL* eval,i1* tpts){
    maxBlackAIdiagonalDirection(i,depth,eval,tpts,-9,7);
    maxBlackAIdiagonalDirection(i,depth,eval,tpts,-7,0);
    maxBlackAIdiagonalDirection(i,depth,eval,tpts, 9,0);
    maxBlackAIdiagonalDirection(i,depth,eval,tpts, 7,7);
}

void maxWhiteAIdiagonal(u1 i,u1 depth,EVAL* eval,i1* tpts){
    maxWhiteAIdiagonalDirection(i,depth,eval,tpts,-9,7);
    maxWhiteAIdiagonalDirection(i,depth,eval,tpts,-7,0);
    maxWhiteAIdiagonalDirection(i,depth,eval,tpts, 9,0);
    maxWhiteAIdiagonalDirection(i,depth,eval,tpts, 7,7);
}
                                                     
void minBlackAIdiagonal(u1 i,u1 depth,EVAL* eval,i1* tpts){
    minBlackAIdiagonalDirection(i,depth,eval,tpts,-9,7);
    minBlackAIdiagonalDirection(i,depth,eval,tpts,-7,0);
    minBlackAIdiagonalDirection(i,depth,eval,tpts, 9,0);
    minBlackAIdiagonalDirection(i,depth,eval,tpts, 7,7);
}
                                                     
void minWhiteAIdiagonal(u1 i,u1 depth,EVAL* eval,i1* tpts){
    minWhiteAIdiagonalDirection(i,depth,eval,tpts,-9,7);
    minWhiteAIdiagonalDirection(i,depth,eval,tpts,-7,0);
    minWhiteAIdiagonalDirection(i,depth,eval,tpts, 9,0);
    minWhiteAIdiagonalDirection(i,depth,eval,tpts, 7,7);
}

void maxBlackAIstraightDirectionHorizontal(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir,u1 bound){
    for(u1 j = i + dir;(j & 7) != bound && board[j] < 'a';j+=dir){
        if(board[j] < 'Z'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1max(*tpts,minBlackAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void maxWhiteAIstraightDirectionHorizontal(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir,u1 bound){
    for(u1 j = i + dir;(j & 7) != bound && board[j] > 'Z';j+=dir){
        if(board[j] > 'a'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1max(*tpts,minWhiteAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void minBlackAIstraightDirectionHorizontal(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir,u1 bound){
    for(u1 j = i + dir;(j & 7) != bound && board[j] > 'Z';j+=dir){
        if(board[j] > 'a'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1min(*tpts,maxBlackAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void minWhiteAIstraightDirectionHorizontal(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir,u1 bound){
    for(u1 j = i + dir;(j & 7) != bound && board[j] < 'a';j+=dir){
        if(board[j] < 'Z'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1min(*tpts,maxWhiteAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void maxBlackAIstraightDirectionVertical(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir){
    for(u1 j = i + dir;j < 64 && board[j] < 'a';j+=dir){
        if(board[j] < 'Z'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1max(*tpts,minBlackAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void maxWhiteAIstraightDirectionVertical(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir){
    for(u1 j = i + dir;j < 64 && board[j] > 'Z';j+=dir){
        if(board[j] > 'a'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1max(*tpts,minWhiteAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void minBlackAIstraightDirectionVertical(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir){
    for(u1 j = i + dir;j < 64 && board[j] > 'Z';j+=dir){
        if(board[j] > 'a'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1min(*tpts,maxBlackAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void minWhiteAIstraightDirectionVertical(u1 i,u1 depth,EVAL* eval,i1* tpts,i1 dir){
    for(u1 j = i + dir;j < 64 && board[j] < 'a';j+=dir){
        if(board[j] < 'Z'){
            eval->pts += pcsVal[board[j]];
            i1 tpcs = board[j];
            capturePiece(i,j);
            *tpts = i1min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[j];
            board[j] = tpcs;
            eval->pts -= pcsVal[board[j]];
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
            break;
        }
        else{
            movePiece(i,j);
            *tpts = i1min(*tpts,maxWhiteAI(depth-1,*eval));
            movePiece(j,i);
            eval->ab.beta = i1min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha) return;
        }
    }
}

void maxBlackAIstraight(u1 i,u1 depth,EVAL* eval,i1* tpts){
    maxBlackAIstraightDirectionHorizontal(i,depth,eval,tpts, 1,0);
    maxBlackAIstraightDirectionHorizontal(i,depth,eval,tpts,-1,7);
    maxBlackAIstraightDirectionVertical(i,depth,eval,tpts,-8);
    maxBlackAIstraightDirectionVertical(i,depth,eval,tpts, 8);
}
                                                     
void maxWhiteAIstraight(u1 i,u1 depth,EVAL* eval,i1* tpts){
    maxWhiteAIstraightDirectionHorizontal(i,depth,eval,tpts, 1,0);
    maxWhiteAIstraightDirectionHorizontal(i,depth,eval,tpts,-1,7);
    maxWhiteAIstraightDirectionVertical(i,depth,eval,tpts,-8);
    maxWhiteAIstraightDirectionVertical(i,depth,eval,tpts, 8);
}
                                                     
void minBlackAIstraight(u1 i,u1 depth,EVAL* eval,i1* tpts){
    minBlackAIstraightDirectionHorizontal(i,depth,eval,tpts, 1,0);
    minBlackAIstraightDirectionHorizontal(i,depth,eval,tpts,-1,7);
    minBlackAIstraightDirectionVertical(i,depth,eval,tpts,-8);
    minBlackAIstraightDirectionVertical(i,depth,eval,tpts, 8);
}
                                                     
void minWhiteAIstraight(u1 i,u1 depth,EVAL* eval,i1* tpts){
    minWhiteAIstraightDirectionHorizontal(i,depth,eval,tpts, 1,0);
    minWhiteAIstraightDirectionHorizontal(i,depth,eval,tpts,-1,7);
    minWhiteAIstraightDirectionVertical(i,depth,eval,tpts,-8);
    minWhiteAIstraightDirectionVertical(i,depth,eval,tpts, 8);
}

void maxBlackAIknight(i1 i,i1 dst,u1 depth,EVAL* eval,i1* tpts){
    if(board[i+dst] < 'Z'){
        eval->pts += pcsVal[board[i+dst]];
        i1 tpcs = board[i+dst];
        capturePiece(i,i+dst);
        *tpts = i1max(*tpts,minBlackAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
    }
    else if(board[i+dst] == '_'){
        movePiece(i,i+dst);
        *tpts = i1max(*tpts,minBlackAI(depth-1,*eval));
        movePiece(i+dst,i);
        eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
    }
}
                                                          
void maxWhiteAIknight(i1 i,i1 dst,u1 depth,EVAL* eval,i1* tpts){
    if(board[i+dst] > 'a'){
        eval->pts += pcsVal[board[i+dst]];
        i1 tpcs = board[i+dst];
        capturePiece(i,i+dst);
        *tpts = i1max(*tpts,minWhiteAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
    }
    else if(board[i+dst] == '_'){
        movePiece(i,i+dst);
        *tpts = i1max(*tpts,minWhiteAI(depth-1,*eval));
        movePiece(i+dst,i);
        eval->ab.alpha = i1max(eval->ab.alpha,*tpts);
    }
}
                                                          
void minBlackAIknight(i1 i,i1 dst,u1 depth,EVAL* eval,i1* tpts){
    if(board[i+dst] > 'a'){
        eval->pts += pcsVal[board[i+dst]];
        i1 tpcs = board[i+dst];
        capturePiece(i,i+dst);
        *tpts = i1min(*tpts,maxBlackAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.beta = i1min(eval->ab.beta,*tpts);
    }
    else if(board[i+dst] == '_'){
        movePiece(i,i+dst);
        *tpts = i1min(*tpts,maxBlackAI(depth-1,*eval));
        movePiece(i+dst,i);
        eval->ab.beta = i1min(eval->ab.beta,*tpts);
    }
}
                                                          
void minWhiteAIknight(i1 i,i1 dst,u1 depth,EVAL* eval,i1* tpts){
    if(board[i+dst] < 'Z'){
        eval->pts += pcsVal[board[i+dst]];
        i1 tpcs = board[i+dst];
        capturePiece(i,i+dst);
        *tpts = i1min(*tpts,maxWhiteAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.beta = i1min(eval->ab.beta,*tpts);
    }
    else if(board[i+dst] == '_'){
        movePiece(i,i+dst);
        *tpts = i1min(*tpts,maxWhiteAI(depth-1,*eval));
        movePiece(i+dst,i);
        eval->ab.beta = i1min(eval->ab.beta,*tpts);
    }
}

i1 maxBlackAI(u1 depth,EVAL eval){
    i1 tpts;
    if(!depth){
        tpts = eval.pts;
        for(i4 i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 47){
                    if((i & 7) != 7) tpts = i1max(tpts,eval.pts+pcsVal[board[i+9]]+8);
                    if((i & 7) != 0) tpts = i1max(tpts,eval.pts+pcsVal[board[i+7]]+8);
                    else if(board[i+8] == '_') tpts = i1max(tpts,eval.pts+8);
                }
                else{
                    if((i & 7) != 7) tpts = i1max(tpts,eval.pts+pcsVal[board[i+9]]);
                    if((i & 7) != 0) tpts = i1max(tpts,eval.pts+pcsVal[board[i+7]]);
                }
                break;
            case 'n':{
                u1 sx = i >> 3;
                u1 sy = i & 7;
                if(sx > 0){
                    if(sy < 6) tpts = i1max(tpts,eval.pts+pcsVal[board[i-6]]);
                    if(sy > 1) tpts = i1max(tpts,eval.pts+pcsVal[board[i-10]]);
                    if(sx > 1){
                        if(sy < 7) tpts = i1max(tpts,eval.pts+pcsVal[board[i-15]]);
                        if(sy > 0) tpts = i1max(tpts,eval.pts+pcsVal[board[i-17]]);
                    }
                }
                if(sx < 7){
                    if(sy > 1) tpts = i1max(tpts,eval.pts+pcsVal[board[i+6]]);
                    if(sy < 6) tpts = i1max(tpts,eval.pts+pcsVal[board[i+10]]);
                    if(sx < 6){
                        if(sy > 0) tpts = i1max(tpts,eval.pts+pcsVal[board[i+15]]);
                        if(sy < 7) tpts = i1max(tpts,eval.pts+pcsVal[board[i+17]]);
                    }
                }
                break;
            }
            case 'b':
                tpts = maxBlackAIdiagonal0(i,eval.pts,tpts);
                break;
            case 'r':
                tpts = maxBlackAIstraight0(i,eval.pts,tpts);
                break;
            case 'q':
                tpts = maxBlackAIstraight0(i,eval.pts,tpts);
                tpts = maxBlackAIdiagonal0(i,eval.pts,tpts);
                break;
            case 'k':{
                i4 j = -1,lm = 2;
                if((i & 7) == 0) j++;
                if((i & 7) == 7) lm--;
                for(;j < lm;j++){
                    for(i4 i3 = -8;i3 < 9;i3+=8){
                        if(j+i3!=0&&i+j+i3>=0&&i+j+i3<64) tpts = i1max(tpts,eval.pts+pcsVal[board[i+j+i3]]);
                    }
                }
                break;
            }
            }
        }
    }
    else{
        tpts = -75;
        for(i4 i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 47){
                    if(board[i+8] == '_'){
                        board[i] = 'q';
                        movePiece(i,i+8);
                        eval.pts += 8;
                        tpts = i1max(tpts,minBlackAI(depth-1,eval));
                        eval.pts -= 8;
                        movePiece(i+8,i);
                        board[i] = 'p';
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if((i & 7) != 7 && board[i+9] < 'Z'){
                        eval.pts += pcsVal[board[i+9]] + 8;
                        i1 tpcs = board[i+9];
                        board[i] = 'q';
                        capturePiece(i,i+9);
                        tpts = i1max(tpts,minBlackAI(depth-1,eval));
                        board[i] = 'p';
                        board[i+9] = tpcs;
                        eval.pts -= pcsVal[board[i+9]] + 8;
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if((i & 7) != 0 && board[i+7] < 'Z'){
                        eval.pts += pcsVal[board[i+7]] + 8;
                        i1 tpcs = board[i+7];
                        board[i] = 'q';
                        capturePiece(i,i+7);
                        tpts = i1max(tpts,minBlackAI(depth-1,eval));
                        board[i] = 'p';
                        board[i+7] = tpcs;
                        eval.pts -= pcsVal[board[i+7]] + 8;
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                else{
                    if(board[i+8] == '_'){
                        movePiece(i,i+8);
                        tpts = i1max(tpts,minBlackAI(depth-1,eval));
                        movePiece(i+8,i);
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        if(i > 7 && i < 16 && board[i+16] == '_'){
                            movePiece(i,i+16);
                            tpts = i1max(tpts,minBlackAI(depth-1,eval));
                            movePiece(i+16,i);
                            eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                    if((i & 7) != 7 && board[i+9] < 'Z'){
                        eval.pts += pcsVal[board[i+9]];
                        i1 tpcs = board[i+9];
                        capturePiece(i,i+9);
                        tpts = i1max(tpts,minBlackAI(depth-1,eval));
                        board[i] = board[i+9];
                        board[i+9] = tpcs;
                        eval.pts -= pcsVal[board[i+9]];
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if((i & 7) != 0 && board[i+7] < 'Z'){
                        eval.pts += pcsVal[board[i+7]];
                        i1 tpcs = board[i+7];
                        capturePiece(i,i+7);
                        tpts = i1max(tpts,minBlackAI(depth-1,eval));
                        board[i] = board[i+7];
                        board[i+7] = tpcs;
                        eval.pts -= pcsVal[board[i+7]];
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                break;
            case 'n':{
                u1 sx = i >> 3;
                u1 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        maxBlackAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sy > 1){
                        maxBlackAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sx > 1){
                        if(sy < 7){
                            maxBlackAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                        if(sy > 0){
                            maxBlackAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        maxBlackAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sy < 6){
                        maxBlackAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sx < 6){
                        if(sy > 0){
                            maxBlackAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                        if(sy < 7){
                            maxBlackAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                }
                break;
            }
            case 'b':
                maxBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'r':
                maxBlackAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'q':
                maxBlackAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                maxBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'k':{
                i4 j = -1,lm = 2;
                if((i & 7) == 0) j++;
                if((i & 7) == 7) lm--;
                for(;j < lm;j++){
                    for(i4 i3 = -8;i3 < 9;i3+=8){
                        if(j+i3!=0&&i+j+i3>=0&&i+j+i3<64){
                            if(board[i+j+i3] < 'Z'){
                                eval.pts += pcsVal[board[i+j+i3]];
                                i1 tpcs = board[i+j+i3];
                                u1 castleTempS = castleRightWhiteShort;
                                u1 castleTempL = castleRightWhiteLong;
                                castleRightWhiteShort = 0;
                                castleRightWhiteLong = 0;
                                capturePiece(i,i+j+i3);
                                tpts = i1max(tpts,minBlackAI(depth-1,eval));
                                castleRightWhiteShort = castleTempS;
                                castleRightWhiteLong = castleTempL;
                                board[i] = board[i+j+i3];
                                board[i+j+i3] = tpcs;
                                eval.pts -= pcsVal[board[i+j+i3]];
                                eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                            }
                            else if(board[i+j+i3] == '_'){
                                u1 castleTempS = castleRightWhiteShort;
                                u1 castleTempL = castleRightWhiteLong;
                                castleRightWhiteShort = 0;
                                castleRightWhiteLong = 0;
                                movePiece(i,i+j+i3);
                                tpts = i1max(tpts,minBlackAI(depth-1,eval));
                                movePiece(i+j+i3,i);
                                castleRightWhiteShort = castleTempS;
                                castleRightWhiteLong = castleTempL;
                                eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                            }
                        }
                    }
                }
                if(i == 4){
                    if(castleRightWhiteShort && board[5] == '_' && board[6] == '_'){
                        movePiece(4,6);
                        movePiece(7,5);
                        u1 castleTempS = castleRightWhiteShort;
                        u1 castleTempL = castleRightWhiteLong;
                        castleRightWhiteShort = 0;
                        castleRightWhiteLong = 0;
                        tpts = i1max(tpts,minBlackAI(depth-1,eval));
                        castleRightWhiteShort = castleTempS;
                        castleRightWhiteLong = castleTempL;
                        movePiece(6,4);
                        movePiece(5,7);
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(castleRightWhiteLong && board[3] == '_' && board[2] == '_' && board[1] == '_'){
                        movePiece(4,2);
                        movePiece(0,3);
                        u1 castleTempS = castleRightWhiteShort;
                        u1 castleTempL = castleRightWhiteLong;
                        castleRightWhiteShort = 0;
                        castleRightWhiteLong = 0;
                        tpts = i1max(tpts,minBlackAI(depth-1,eval));
                        castleRightWhiteShort = castleTempS;
                        castleRightWhiteLong = castleTempL;
                        movePiece(2,4);
                        movePiece(3,0);
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                break;
            }
            }
        }
    }
    return tpts;
}

i1 maxWhiteAI(u1 depth,EVAL eval){
    i1 tpts;
    if(!depth){
        tpts = eval.pts;
        for(i4 i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if((i & 7) != 0) tpts = i1max(tpts,eval.pts+pcsVal[board[i-9]]+8);
                    if((i & 7) != 7) tpts = i1max(tpts,eval.pts+pcsVal[board[i-7]]+8);
                    else if(board[i-8] == '_') tpts = i1max(tpts,eval.pts+8);
                }
                else{
                    if((i & 7) != 0) tpts = i1max(tpts,eval.pts+pcsVal[board[i-9]]);
                    if((i & 7) != 7) tpts = i1max(tpts,eval.pts+pcsVal[board[i-7]]);
                }
                break;
            case 'N':{
                u1 sx = i >> 3;
                u1 sy = i & 7;
                if(sx > 0){
                    if(sy < 6) tpts = i1max(tpts,eval.pts+pcsVal[board[i-6]]);
                    if(sy > 1) tpts = i1max(tpts,eval.pts+pcsVal[board[i-10]]);
                    if(sx > 1){
                        if(sy < 7) tpts = i1max(tpts,eval.pts+pcsVal[board[i-15]]);
                        if(sy > 0) tpts = i1max(tpts,eval.pts+pcsVal[board[i-17]]);
                    }
                }
                if(sx < 7){
                    if(sy > 1) tpts = i1max(tpts,eval.pts+pcsVal[board[i+6]]);
                    if(sy < 6) tpts = i1max(tpts,eval.pts+pcsVal[board[i+10]]);
                    if(sx < 6){
                        if(sy > 0) tpts = i1max(tpts,eval.pts+pcsVal[board[i+15]]);
                        if(sy < 7) tpts = i1max(tpts,eval.pts+pcsVal[board[i+17]]);
                    }
                }
                break;
            }
            case 'B':
                tpts = maxWhiteAIdiagonal0(i,eval.pts,tpts);
                break;
            case 'R':
                tpts = maxWhiteAIstraight0(i,eval.pts,tpts);
                break;
            case 'Q':
                tpts = maxWhiteAIstraight0(i,eval.pts,tpts);
                tpts = maxWhiteAIdiagonal0(i,eval.pts,tpts);
                break;
            case 'K':{
                i4 j = -1,lm = 2;
                if((i & 7) == 0) j++;
                if((i & 7) == 7) lm--;
                for(;j < lm;j++){
                    for(i4 i3 = -8;i3 < 9;i3+=8){
                        if(j+i3!=0&&i+j+i3>=0&&i+j+i3<64) tpts = i1max(tpts,eval.pts+pcsVal[board[i+j+i3]]);
                    }
                }
                break;
            }
            }
        }
    }
    else{
        tpts = -75;
        for(i4 i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if(board[i-8] == '_'){
                        board[i] = 'Q';
                        movePiece(i,i-8);
                        eval.pts += 8;
                        tpts = i1max(tpts,minWhiteAI(depth-1,eval));
                        eval.pts -= 8;
                        movePiece(i-8,i);
                        board[i] = 'P';
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if((i & 7) != 0 && board[i-9] > 'a'){
                        eval.pts += pcsVal[board[i-9]] + 8;
                        i1 tpcs = board[i-9];
                        board[i] = 'Q';
                        capturePiece(i,i-9);
                        tpts = i1max(tpts,minWhiteAI(depth-1,eval));
                        board[i] = 'P';
                        board[i-9] = tpcs;
                        eval.pts -= pcsVal[board[i-9]] + 8;
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if((i & 7) != 7 && board[i+7] > 'a'){
                        eval.pts += pcsVal[board[i-7]] + 8;
                        i1 tpcs = board[i-7];
                        board[i] = 'Q';
                        capturePiece(i,i-7);
                        tpts = i1max(tpts,minWhiteAI(depth-1,eval));
                        board[i] = 'P';
                        board[i-7] = tpcs;
                        eval.pts -= pcsVal[board[i-7]] + 8;
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                else{
                    if(board[i-8] == '_'){
                        movePiece(i,i-8);
                        tpts = i1max(tpts,minWhiteAI(depth-1,eval));
                        movePiece(i-8,i);
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        if(i > 47 && board[i-16] == '_'){
                            movePiece(i,i-16);
                            tpts = i1max(tpts,minWhiteAI(depth-1,eval));
                            movePiece(i-16,i);
                            eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                    if((i & 7) != 0 && board[i-9] > 'a'){
                        eval.pts += pcsVal[board[i-9]];
                        i1 tpcs = board[i-9];
                        capturePiece(i,i-9);
                        tpts = i1max(tpts,minWhiteAI(depth-1,eval));
                        board[i] = board[i-9];
                        board[i-9] = tpcs;
                        eval.pts -= pcsVal[board[i-9]];
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if((i & 7) != 7 && board[i-7] > 'a'){
                        eval.pts += pcsVal[board[i-7]];
                        i1 tpcs = board[i-7];
                        capturePiece(i,i-7);
                        tpts = i1max(tpts,minWhiteAI(depth-1,eval));
                        board[i] = board[i-7];
                        board[i-7] = tpcs;
                        eval.pts -= pcsVal[board[i-7]];
                        eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                break;
            case 'N':{
                u1 sx = i >> 3;
                u1 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        maxWhiteAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sy > 1){
                        maxWhiteAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sx > 1){
                        if(sy < 7){
                            maxWhiteAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                        if(sy > 0){
                            maxWhiteAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        maxWhiteAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sy < 6){
                        maxWhiteAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sx < 6){
                        if(sy > 0){
                            maxWhiteAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                        if(sy < 7){
                            maxWhiteAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                }
                break;
            }
            case 'B':
                maxWhiteAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'R':
                maxWhiteAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'Q':
                maxWhiteAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                maxWhiteAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'K':{
                i4 j = -1,lm = 2;
                if((i & 7) == 0) j++;
                if((i & 7) == 7) lm--;
                for(;j < lm;j++){
                    for(i4 i3 = -8;i3 < 9;i3+=8){
                        if(j+i3!=0&&i+j+i3>=0&&i+j+i3<64){
                            if(board[i+j+i3] > 'a'){
                                eval.pts += pcsVal[board[i+j+i3]];
                                i1 tpcs = board[i+j+i3];
                                capturePiece(i,i+j+i3);
                                tpts = i1max(tpts,minWhiteAI(depth-1,eval));
                                board[i] = board[i+j+i3];
                                board[i+j+i3] = tpcs;
                                eval.pts -= pcsVal[board[i+j+i3]];
                                eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                            }
                            else if(board[i+j+i3] == '_'){
                                movePiece(i,i+j+i3);
                                tpts = i1max(tpts,minWhiteAI(depth-1,eval));
                                movePiece(i+j+i3,i);
                                eval.ab.alpha = i1max(eval.ab.alpha,tpts);
                                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                            }
                        }
                    }
                }
                if(i == 60){
                    if(castleRightBlackShort && board[61] == '_' && board[62] == '_'){
                        movePiece(60,62);
                        movePiece(63,61);
                        u1 castleTempL = castleRightBlackLong;
                        castleRightBlackShort = 0;
                        castleRightBlackLong = 0;
                        tpts = i1min(tpts,minWhiteAI(depth-1,eval));
                        movePiece(62,60);
                        movePiece(60,61);
                        castleRightBlackShort = 1;
                        castleRightBlackLong  = castleTempL;
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(castleRightBlackLong && board[59] == '_' && board[58] == '_' && board[57] == '_'){
                        movePiece(60,58);
                        movePiece(56,59);
                        u1 castleTempS = castleRightBlackShort;
                        castleRightBlackShort = 0;
                        castleRightBlackLong = 0;
                        tpts = i1min(tpts,minWhiteAI(depth-1,eval));
                        movePiece(58,60);
                        movePiece(59,56);
                        castleRightBlackLong = 1;
                        castleRightBlackShort = castleTempS;
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                break;
            }
            }
        }
    }
    return tpts;
}

i1 minBlackAI(u1 depth,EVAL eval){
    i1 tpts;
    if(!depth){
        tpts = eval.pts;
        for(i4 i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if((i & 7) != 0) tpts = i1min(tpts,eval.pts+pcsVal[board[i-9]]-8);
                    if((i & 7) != 7) tpts = i1min(tpts,eval.pts+pcsVal[board[i-7]]-8);
                    else if(board[i-8] == '_') tpts = i1min(tpts,eval.pts-8);
                }
                else{
                    if((i & 7) != 0) tpts = i1min(tpts,eval.pts+pcsVal[board[i-9]]);
                    if((i & 7) != 7) tpts = i1min(tpts,eval.pts+pcsVal[board[i-7]]);
                }
                break;
            case 'N':{
                u1 sx = i >> 3;
                u1 sy = i & 7;
                if(sx > 0){
                    if(sy < 6) tpts = i1min(tpts,eval.pts+pcsVal[board[i-6]]);
                    if(sy > 1) tpts = i1min(tpts,eval.pts+pcsVal[board[i-10]]);
                    if(sx > 1){
                        if(sy < 7) tpts = i1min(tpts,eval.pts+pcsVal[board[i-15]]);
                        if(sy > 0) tpts = i1min(tpts,eval.pts+pcsVal[board[i-17]]);
                    }
                }
                if(sx < 7){
                    if(sy > 1) tpts = i1min(tpts,eval.pts+pcsVal[board[i+6]]);
                    if(sy < 6) tpts = i1min(tpts,eval.pts+pcsVal[board[i+10]]);
                    if(sx < 6){
                        if(sy > 0) tpts = i1min(tpts,eval.pts+pcsVal[board[i+15]]);
                        if(sy < 7) tpts = i1min(tpts,eval.pts+pcsVal[board[i+17]]);
                    }
                }
                break;
            }
            case 'B':
                tpts = minBlackAIdiagonal0(i,eval.pts,tpts);
                break;
            case 'R':
                tpts = minBlackAIstraight0(i,eval.pts,tpts);
                break;
            case 'Q':
                tpts = minBlackAIdiagonal0(i,eval.pts,tpts);
                tpts = minBlackAIstraight0(i,eval.pts,tpts);
                break;
            case 'K':{
                i4 j = -1,lm = 2;
                if((i & 7) == 0) j++;
                if((i & 7) == 7) lm--;
                for(;j < lm;j++){
                    for(i4 i3 = -8;i3 < 9;i3+=8){
                        if(i+j+i3>=0&&i+j+i3<64) tpts = i1min(tpts,eval.pts+pcsVal[board[i+j+i3]]);
                    }
                }
                break;
            }
            }
        }
    }
    else{
        tpts = 75;
        for(i4 i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if(board[i-8] == '_'){
                        board[i] = 'Q';
                        movePiece(i,i-8);
                        eval.pts -= 8;
                        tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                        eval.pts += 8;
                        movePiece(i-8,i);
                        board[i] = 'P';
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                        eval.pts += pcsVal[board[i-9]];
                        i1 tpcs = board[i-9];
                        board[i] = 'Q';
                        capturePiece(i,i-9);
                        tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                        board[i-9] = tpcs;
                        board[i] = 'P';
                        eval.pts -= pcsVal[board[i-9]];
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                        eval.pts += pcsVal[board[i-7]];
                        i1 tpcs = board[i-7];
                        board[i] = 'Q';
                        capturePiece(i,i-7);
                        tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                        board[i-7] = tpcs;
                        board[i] = 'P';
                        eval.pts -= pcsVal[board[i-7]];
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                else{
                    if(board[i-8] == '_'){
                        movePiece(i,i-8);
                        tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                        movePiece(i-8,i);
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        if(i > 47 && i < 56 && board[i-16] == '_'){
                            movePiece(i,i-16);
                            tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                            movePiece(i-16,i);
                            eval.ab.beta = i1min(eval.ab.beta,tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                        eval.pts += pcsVal[board[i-9]];
                        i1 tpcs = board[i-9];
                        capturePiece(i,i-9);
                        tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                        board[i] = board[i-9];
                        board[i-9] = tpcs;
                        eval.pts -= pcsVal[board[i-9]];
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                        eval.pts += pcsVal[board[i-7]];
                        i1 tpcs = board[i-7];
                        capturePiece(i,i-7);
                        tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                        board[i] = board[i-7];
                        board[i-7] = tpcs;
                        eval.pts -= pcsVal[board[i-7]];
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                break;
            case 'N':{
                u1 sx = i >> 3;
                u1 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        minBlackAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sy > 1){
                        minBlackAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sx > 1){
                        if(sy < 7){
                            minBlackAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                        if(sy > 0){
                            minBlackAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        minBlackAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sy < 6){
                        minBlackAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sx < 6){
                        if(sy > 0){
                            minBlackAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                        if(sy < 7){
                            minBlackAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                }
                break;
            }
            case 'B':
                minBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'R':
                minBlackAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'Q':
                minBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                minBlackAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'K':{
                i4 j = -1,lm = 2;
                if((i & 7) == 0) j++;
                if((i & 7) == 7) lm--;
                for(;j < lm;j++){
                    for(i4 i3 = -8;i3 < 9;i3+=8){
                        if(i+j+i3>=0&&i+j+i3<64){
                            if(board[i+j+i3] < 'z' && board[i+j+i3] > 'a'){
                                eval.pts += pcsVal[board[i+j+i3]];
                                i1 tpcs = board[i+j+i3];
                                u1 castleTempS = castleRightWhiteShort;
                                u1 castleTempL = castleRightWhiteLong;
                                castleRightWhiteShort = 0;
                                castleRightWhiteLong = 0;
                                capturePiece(i,i+j+i3);
                                tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                                castleRightWhiteShort = castleTempS;
                                castleRightWhiteLong = castleTempL;
                                board[i] = board[i+j+i3];
                                board[i+j+i3] = tpcs;
                                eval.pts -= pcsVal[board[i+j+i3]];
                                eval.ab.beta = i1min(eval.ab.beta,tpts);
                                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                            }
                            else if(board[i+j+i3] == '_'){
                                u1 castleTempS = castleRightWhiteShort;
                                u1 castleTempL = castleRightWhiteLong;
                                castleRightWhiteShort = 0;
                                castleRightWhiteLong = 0;
                                movePiece(i,i+j+i3);
                                tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                                movePiece(i+j+i3,i);
                                castleRightWhiteShort = castleTempS;
                                castleRightWhiteLong = castleTempL;
                                eval.ab.beta = i1min(eval.ab.beta,tpts);
                                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                            }
                        }
                    }
                }
                if(i == 60){
                    if(castleRightWhiteShort && board[61] == '_' && board[62] == '_'){
                        movePiece(60,62);
                        movePiece(63,61);
                        u1 castleTempL = castleRightWhiteLong;
                        castleRightWhiteLong = 0;
                        castleRightWhiteShort = 0;
                        tpts = i1min(tpts,maxBlackAI(depth-1,eval));
                        castleRightWhiteLong = castleTempL;
                        castleRightWhiteShort = 0;
                        movePiece(62,60);
                        movePiece(61,63);
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(castleRightWhiteLong && board[59] == '_' && board[58] == '_' && board[57] == '_'){
                        movePiece(60,58);
                        movePiece(56,59);
                        u1 castleTempS = castleRightWhiteShort;
                        castleRightWhiteLong = 0;
                        castleRightWhiteShort = 0;
                        tpts = i1max(tpts,maxBlackAI(depth-1,eval));
                        castleRightWhiteShort = castleTempS;
                        castleRightWhiteLong = 0;
                        movePiece(58,60);
                        movePiece(59,56);
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                break;
            }
            }
        }
    }
    return tpts;
}

i1 minWhiteAI(u1 depth,EVAL eval){
    i1 tpts;
    if(!depth){
        tpts = eval.pts;
        for(i4 i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 47){
                    if((i & 7) != 7) tpts = i1min(tpts,eval.pts+pcsVal[board[i+9]]-8);
                    if((i & 7) != 0) tpts = i1min(tpts,eval.pts+pcsVal[board[i+7]]-8);
                    else if(board[i+8] == '_') tpts = i1min(tpts,eval.pts-8);
                }
                else{
                    if((i & 7) != 7) tpts = i1min(tpts,eval.pts+pcsVal[board[i+9]]);
                    if((i & 7) != 0) tpts = i1min(tpts,eval.pts+pcsVal[board[i+7]]);
                }
                break;
            case 'n':{
                u1 sx = i >> 3;
                u1 sy = i & 7;
                if(sx > 0){
                    if(sy < 6) tpts = i1min(tpts,eval.pts+pcsVal[board[i-6]]);
                    if(sy > 1) tpts = i1min(tpts,eval.pts+pcsVal[board[i-10]]);
                    if(sx > 1){
                        if(sy < 7) tpts = i1min(tpts,eval.pts+pcsVal[board[i-15]]);
                        if(sy > 0) tpts = i1min(tpts,eval.pts+pcsVal[board[i-17]]);
                    }
                }
                if(sx < 7){
                    if(sy > 1) tpts = i1min(tpts,eval.pts+pcsVal[board[i+6]]);
                    if(sy < 6) tpts = i1min(tpts,eval.pts+pcsVal[board[i+10]]);
                    if(sx < 6){
                        if(sy > 0) tpts = i1min(tpts,eval.pts+pcsVal[board[i+15]]);
                        if(sy < 7) tpts = i1min(tpts,eval.pts+pcsVal[board[i+17]]);
                    }
                }
                break;
            }
            case 'b':
                tpts = minWhiteAIdiagonal0(i,eval.pts,tpts);
                break;
            case 'r':
                tpts = minWhiteAIstraight0(i,eval.pts,tpts);
                break;
            case 'q':
                tpts = minWhiteAIdiagonal0(i,eval.pts,tpts);
                tpts = minWhiteAIstraight0(i,eval.pts,tpts);
                break;
            case 'k':{
                i4 j = -1,lm = 2;
                if((i & 7) == 0) j++;
                if((i & 7) == 7) lm--;
                for(;j < lm;j++){
                    for(i4 i3 = -8;i3 < 9;i3+=8){
                        if(i+j+i3>=0&&i+j+i3<64) tpts = i1min(tpts,eval.pts+pcsVal[board[i+j+i3]]);
                    }
                }
                break;
            }
            }
        }
    }
    else{
        tpts = 75;
        for(i4 i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 47){
                    if(board[i+8] == '_'){
                        board[i] = 'q';
                        movePiece(i,i+8);
                        eval.pts -= 8;
                        tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                        eval.pts += 8;
                        movePiece(i+8,i);
                        board[i] = 'p';
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if((i & 7) != 7 && board[i+9] < 'Z'){
                        eval.pts += pcsVal[board[i+9]];
                        i1 tpcs = board[i+9];
                        board[i] = 'q';
                        capturePiece(i,i+9);
                        tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                        board[i+9] = tpcs;
                        board[i] = 'p';
                        eval.pts -= pcsVal[board[i+9]];
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if((i & 7) != 0 && board[i+7] < 'Z'){
                        eval.pts += pcsVal[board[i+7]];
                        i1 tpcs = board[i+7];
                        board[i] = 'q';
                        capturePiece(i,i+7);
                        tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                        board[i+7] = tpcs;
                        board[i] = 'p';
                        eval.pts -= pcsVal[board[i+7]];
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                else{
                    if(board[i+8] == '_'){
                        movePiece(i,i+8);
                        tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                        movePiece(i+8,i);
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        if(i < 16 && board[i+16] == '_'){
                            movePiece(i,i+16);
                            tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                            movePiece(i+16,i);
                            eval.ab.beta = i1min(eval.ab.beta,tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                    if((i & 7) != 7 && board[i+9] < 'Z'){
                        eval.pts += pcsVal[board[i+9]];
                        i1 tpcs = board[i+9];
                        capturePiece(i,i+9);
                        tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                        board[i] = board[i+9];
                        board[i+9] = tpcs;
                        eval.pts -= pcsVal[board[i+9]];
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if((i & 7) != 0 && board[i+7] < 'Z'){
                        eval.pts += pcsVal[board[i+7]];
                        i1 tpcs = board[i+7];
                        capturePiece(i,i+7);
                        tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                        board[i] = board[i+7];
                        board[i+7] = tpcs;
                        eval.pts -= pcsVal[board[i+7]];
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                break;
            case 'n':{
                u1 sx = i >> 3;
                u1 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        minWhiteAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sy > 1){
                        minWhiteAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sx > 1){
                        if(sy < 7){
                            minWhiteAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                        if(sy > 0){
                            minWhiteAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        minWhiteAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sy < 6){
                        minWhiteAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(sx < 6){
                        if(sy > 0){
                            minWhiteAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                        if(sy < 7){
                            minWhiteAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha) return tpts;
                        }
                    }
                }
                break;
            }
            case 'b':
                minWhiteAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'r':
                minWhiteAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'q':
                minWhiteAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                minWhiteAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                break;
            case 'k':{
                i4 j = -1,lm = 2;
                if((i & 7) == 0) j++;
                if((i & 7) == 7) lm--;
                for(;j < lm;j++){
                    for(i4 i3 = -8;i3 < 9;i3+=8){
                        if(i+j+i3>=0&&i+j+i3<64){
                            if(board[i+j+i3] < 'Z'){
                                eval.pts += pcsVal[board[i+j+i3]];
                                i1 tpcs = board[i+j+i3];
                                capturePiece(i,i+j+i3);
                                tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                                board[i] = board[i+j+i3];
                                board[i+j+i3] = tpcs;
                                eval.pts -= pcsVal[board[i+j+i3]];
                                eval.ab.beta = i1min(eval.ab.beta,tpts);
                                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                            }
                            else if(board[i+j+i3] == '_'){
                                movePiece(i,i+j+i3);
                                tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                                movePiece(i+j+i3,i);
                                eval.ab.beta = i1min(eval.ab.beta,tpts);
                                if(eval.ab.beta <= eval.ab.alpha) return tpts;
                            }
                        }
                    }
                }
                if(i == 4){
                    if(castleRightBlackShort && board[5] == '_' && board[6] == '_'){
                        movePiece(4,6);
                        movePiece(7,5);
                        u1 castleTempL = castleRightBlackLong;
                        castleRightBlackLong = 0;
                        castleRightBlackShort = 0;
                        tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                        castleRightBlackLong = castleTempL;
                        castleRightBlackShort = 0;
                        movePiece(6,4);
                        movePiece(5,7);
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                    if(castleRightBlackLong && board[3] == '_' && board[2] == '_' && board[1] == '_'){
                        movePiece(4,2);
                        movePiece(0,3);
                        u1 castleTempS = castleRightBlackShort;
                        castleRightBlackShort = 0;
                        castleRightBlackLong = 0;
                        tpts = i1min(tpts,maxWhiteAI(depth-1,eval));
                        castleRightBlackShort = castleTempS;
                        castleRightBlackLong = 0;
                        movePiece(2,4);
                        movePiece(3,0);
                        eval.ab.beta = i1min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha) return tpts;
                    }
                }
                break;
            }
            }
        }
    }
    return tpts;
}

void captureBlackAI(u1 src,u1 dst,AIMOVEDATA *data){
    printMove(src,dst);
    i1 tpcs = board[dst];
    capturePiece(src,dst);
    printf("%c",'|');
    i1 tpts;
    for(i4 i = 0;i <= DEPTH;i++){
        printf("dpt %i = ",i);
        printf("pts ");
        u8 mTime  = __rdtsc();
        tpts = minBlackAI(i,(EVAL){ -75,75,pcsVal[tpcs] });
        u8 mTimeE = __rdtsc();
        if(tpts >= 0 && tpts < 10) printf("%c",' ');
        if(tpts > -10) printf("%c",' ');
        printf("%i|",tpts);
        if(tpts < -50 || tpts > 50){
            data->minDpt = i1max(data->minDpt,i);
            goto n;
        }
        if(mTimeE-mTime>MTIME) break;
    }
    data->minDpt = i1max(data->minDpt,DEPTH);
n:
    printf("%c",'\n');
    board[src] = board[dst];
    board[dst] = tpcs;
    if(data->pts <= tpts){
        if(data->pts != tpts){
            data->pts = tpts;
            data->movC = 0;
        }
        master.mov[data->movC].src = src;
        master.mov[data->movC].dst = dst;
        ++data->movC;
    }
}

void captureWhiteAI(u1 src,u1 dst,AIMOVEDATA *data){
    printMove(src,dst);
    i1 tpcs = board[dst];
    capturePiece(src,dst);
    printf("%c",'|');
    i1 tpts;
    for(i4 i = 0;i <= DEPTH;i++){
        printf("dpt %i = ",i);
        printf("pts ");
        u8 mTime  = __rdtsc();
        tpts = minWhiteAI(i,(EVAL){ -75,75,pcsVal[tpcs] });
        u8 mTimeE = __rdtsc();
        if(tpts >= 0 && tpts < 10) printf("%c",' ');
        if(tpts > -10) printf("%c",' ');
        printf("%i|",tpts);
        if(tpts < -50 || tpts > 50){
            data->minDpt = i1max(data->minDpt,i);
            goto n;
        }
        if(mTimeE-mTime>MTIME) break;
    }
    data->minDpt = i1max(data->minDpt,DEPTH);
n:
    printf("%c",'\n');
    board[src] = board[dst];
    board[dst] = tpcs;
    if(data->pts <= tpts){
        if(data->pts != tpts){
            data->pts = tpts;
            data->movC = 0;
        }
        master.mov[data->movC].src = src;
        master.mov[data->movC].dst = dst;
        ++data->movC;
    }
}

void moveBlackAI(u1 src,u1 dst,AIMOVEDATA *data){
    printMove(src,dst);
    movePiece(src,dst);
    printf("%c",'|');
    i1 tpts;
    for(i4 i = 0;i <= DEPTH;i++){
        u8 mTime = __rdtsc();
        tpts = minBlackAI(i,(EVAL){ -75,75,0 });
        u8 mTimeE = __rdtsc();
        printf("dpt %i = ",i);
        printf("pts ");
        if(tpts >= 0 && tpts < 10) printf("%c",' ');
        if(tpts > -10) printf("%c",' ');
        printf("%i|",tpts);
        if(tpts < -50 || tpts > 50){
            data->minDpt = i1max(data->minDpt,i);
            goto n;
        }
        if(mTimeE-mTime>MTIME) break;
    }
    data->minDpt = i1max(data->minDpt,DEPTH);
n:
    printf("%c",'\n');
    movePiece(dst,src);
    if(data->pts <= tpts){
        if(data->pts != tpts){
            data->pts = tpts;
            data->movC = 0;
        }
        master.mov[data->movC].src = src;
        master.mov[data->movC].dst = dst;
        ++data->movC;
    }
}

void moveWhiteAI(u1 src,u1 dst,AIMOVEDATA *data){
    printMove(src,dst);
    movePiece(src,dst);
    printf("%c",'|');
    i1 tpts;
    for(i4 i = 0;i <= DEPTH;i++){
        u8 mTime = __rdtsc();
        tpts = minWhiteAI(i,(EVAL){ -75,75,0 });
        u8 mTimeE = __rdtsc();
        printf("dpt %i = ",i);
        printf("pts ");
        if(tpts >= 0 && tpts < 10) printf("%c",' ');
        if(tpts > -10) printf("%c",' ');
        printf("%i|",tpts);
        if(tpts < -50 || tpts > 50){
            data->minDpt = i1max(data->minDpt,i);
            goto n;
        }
        if(mTimeE-mTime>MTIME) break;
    }
n:
    data->minDpt = i1max(data->minDpt,DEPTH);
    printf("%c",'\n');
    movePiece(dst,src);
    if(data->pts <= tpts){
        if(data->pts != tpts){
            data->pts = tpts;
            data->movC = 0;
        }
        master.mov[data->movC].src = src;
        master.mov[data->movC].dst = dst;
        ++data->movC;
    }
}

void moveCaptureBlackAI(u1 src,u1 dst,AIMOVEDATA *data){
    if(board[dst] > 'A' && board[dst] < 'Z') captureBlackAI(src,dst,data);
    else if(board[dst] == '_')               moveBlackAI(src,dst,data);
}

void moveCaptureWhiteAI(u1 src,u1 dst,AIMOVEDATA *data){
    if(board[dst] > 'a' && board[dst] < 'z') captureWhiteAI(src,dst,data);
    else if(board[dst] == '_')               moveWhiteAI(src,dst,data);
}

void straightBlackAI(u1 i,AIMOVEDATA *data){
    for(u1 j = i + 1;(board[j] < 'a' || board[j] > 'z') && (j & 7) != 0;j++){
        if(board[j] > 'A' && board[j] < 'Z'){
            captureBlackAI(i,j,data);
            break;
        }
        else moveBlackAI(i,j,data);
    }
    for(u1 j = i - 1;(board[j] < 'a' || board[j] > 'z') && (j & 7) != 7;j--){
        if(board[j] > 'A' && board[j] < 'Z'){
            captureBlackAI(i,j,data);
            break;
        }
        else moveBlackAI(i,j,data);
    }
    for(u1 j = i - 8;(board[j] < 'a' || board[j] > 'z') && j < 64;j-=8){
        if(board[j] > 'A' && board[j] < 'Z'){
            captureBlackAI(i,j,data);
            break;
        }
        else moveBlackAI(i,j,data);
    }
    for(u1 j = i + 8;(board[j] < 'a' || board[j] > 'z') && j < 64;j+=8){
        if(board[j] > 'A' && board[j] < 'Z'){
            captureBlackAI(i,j,data);
            break;
        }
        else moveBlackAI(i,j,data);
    }
}

void straightWhiteAI(u1 i,AIMOVEDATA *data){
    for(u1 j = i + 1;(board[j] < 'A' || board[j] > 'Z') && (j & 7) != 0;j++){
        if(board[j] > 'a' && board[j] < 'z'){
            captureWhiteAI(i,j,data);
            break;
        }
        else moveWhiteAI(i,j,data);
    }
    for(u1 j = i - 1;(board[j] < 'A' || board[j] > 'Z') && (j & 7) != 7;j--){
        if(board[j] > 'a' && board[j] < 'z'){
            captureWhiteAI(i,j,data);
            break;
        }
        else moveWhiteAI(i,j,data);
    }
    for(u1 j = i - 8;(board[j] < 'A' || board[j] > 'Z') && j < 64;j-=8){
        if(board[j] > 'a' && board[j] < 'z'){
            captureWhiteAI(i,j,data);
            break;
        }
        else moveWhiteAI(i,j,data);
    }
    for(u1 j = i + 8;(board[j] < 'A' || board[j] > 'Z') && j < 64;j+=8){
        if(board[j] > 'a' && board[j] < 'z'){
            captureWhiteAI(i,j,data);
            break;
        }
        else moveWhiteAI(i,j,data);
    }
}

void diagonalBlackAI(u1 i,AIMOVEDATA *data){
    for(u1 j = i - 9;(board[j] < 'a' || board[j] > 'z') && j < 64 && (j & 7) != 7;j-=9){
        if(board[j] > 'A' && board[j] < 'Z'){
            captureBlackAI(i,j,data);
            break;
        }
        else moveBlackAI(i,j,data);
    }
    for(u1 j = i - 7;(board[j] < 'a' || board[j] > 'z') && j < 64 && (j & 7) != 0;j-=7){
        if(board[j] > 'A' && board[j] < 'Z'){
            captureBlackAI(i,j,data);
            break;
        }
        else moveBlackAI(i,j,data);
    }
    for(u1 j = i + 9;(board[j] < 'a' || board[j] > 'z') && j < 64 && (j & 7) != 0;j+=9){
        if(board[j] > 'A' && board[j] < 'Z'){
            captureBlackAI(i,j,data);
            break;
        }
        else moveBlackAI(i,j,data);
    }
    for(u1 j = i + 7;(board[j] < 'a' || board[j] > 'z') && j < 64 && (j & 7) != 7;j+=7){
        if(board[j] > 'A' && board[j] < 'Z'){
            captureBlackAI(i,j,data);
            break;
        }
        else moveBlackAI(i,j,data);
    }
}

void diagonalWhiteAI(u1 i,AIMOVEDATA *data){
    for(u1 j = i - 9;(board[j] < 'A' || board[j] > 'Z') && j < 64 && (j & 7) != 7;j-=9){
        if(board[j] > 'a' && board[j] < 'z'){
            captureWhiteAI(i,j,data);
            break;
        }
        else moveWhiteAI(i,j,data);
    }
    for(u1 j = i - 7;(board[j] < 'A' || board[j] > 'Z') && j < 64 && (j & 7) != 0;j-=7){
        if(board[j] > 'a' && board[j] < 'z'){
            captureWhiteAI(i,j,data);
            break;
        }
        else moveWhiteAI(i,j,data);
    }
    for(u1 j = i + 9;(board[j] < 'A' || board[j] > 'Z') && j < 64 && (j & 7) != 0;j+=9){
        if(board[j] > 'a' && board[j] < 'z'){
            captureWhiteAI(i,j,data);
            break;
        }
        else moveWhiteAI(i,j,data);
    }
    for(u1 j = i + 7;(board[j] < 'A' || board[j] > 'Z') && j < 64 && (j & 7) != 7;j+=7){
        if(board[j] > 'a' && board[j] < 'z'){
            captureWhiteAI(i,j,data);
            break;
        }
        else moveWhiteAI(i,j,data);
    }
}

u1 checkDiagonal(i4 src,i4 dst){
    if((src-dst) % 9 == 9){
        if(src<dst) for(i4 i = src+9;i != dst;i+=9) if(board[i] != '_') return 0;
        else        for(i4 i = src-9;i != dst;i-=9) if(board[i] != '_') return 0;
        board[dst] == '_' ? movePiece(src,dst) : capturePiece(src,dst);
        return 1;
    }
    else{
        if(src<dst) for(i4 i = src+7;i != dst;i+=7) if(board[i] != '_') return 0;
        else        for(i4 i = src-7;i != dst;i-=7) if(board[i] != '_') return 0;
        board[dst] == '_' ? movePiece(src,dst) : capturePiece(src,dst);                   
        return 1;
    }
    return 0;
}

u1 checkStraight(i4 src,i4 dst){
    if(!(src-dst&7)){
        if(src<dst) for(i4 i = src+1;i != dst;i++) if(board[i] != '_') return 0;
        else        for(i4 i = src-1;i != dst;i--) if(board[i] != '_') return 0;
        board[dst] == '_' ? movePiece(src,dst) : capturePiece(src,dst);                 
        return 1;
    }
    else{
        if(src<dst) for(i4 i = src+8;i != dst;i+=8) if(board[i] != '_') return 0;
        else        for(i4 i = src-8;i != dst;i-=8) if(board[i] != '_') return 0;
        board[dst] == '_' ? movePiece(src,dst) : capturePiece(src,dst);
        return 1;
    }
    return 0;
}

void whiteAI(){
    u8 mTime = __rdtsc();
    AIMOVEDATA data ={.movC = 0,.pts = -128,.minDpt = 0};
    for(i4 i = 0;i < 64;i++){
        switch(board[i]){
        case 'P':
            if(board[i-8] == '_'){
                moveWhiteAI(i,i-8,&data);
                if(i > 47 && board[i-16] == '_') moveWhiteAI(i,i-16,&data);
            }
            if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0) captureWhiteAI(i,i-9,&data);
            if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7) captureWhiteAI(i,i-7,&data);
            break;
        case 'N':{
            u1 sx = i >> 3;
            u1 sy = i & 7;
            if(sx > 0){
                if(sy < 6) moveCaptureWhiteAI(i,i-6,&data);
                if(sy > 1) moveCaptureWhiteAI(i,i-10,&data);
                if(sx > 1){
                    if(sy < 7) moveCaptureWhiteAI(i,i-15,&data);
                    if(sy > 0) moveCaptureWhiteAI(i,i-17,&data);
                }
            }
            if(sx < 7){
                if(sy > 1) moveCaptureWhiteAI(i,i+6,&data);
                if(sy < 6) moveCaptureWhiteAI(i,i+10,&data);
                if(sx < 6){
                    if(sy > 0) moveCaptureWhiteAI(i,i+15,&data);
                    if(sy < 7) moveCaptureWhiteAI(i,i+17,&data);
                }
            }
            break;
        }
        case 'B':
            diagonalWhiteAI(i,&data);
            break;
        case 'R':
            straightWhiteAI(i,&data);
            break;
        case 'Q':
            diagonalWhiteAI(i,&data);
            straightWhiteAI(i,&data);
            break;
        case 'K':
            i4 j = -1,lm = 2;
            if((i & 7) == 0) j++;
            if((i & 7) == 7) lm--;
            for(;j < lm;j++){
                for(i4 i3 = -8;i3 < 9;i3+=8) if(j+i3!=0&&i+j+i3>0&&i+j+i3<64) moveCaptureWhiteAI(i,i+j+i3,&data);
            }
            if(castleRightWhiteShort && board[5] == '_' && board[6] == '_'){
                movePiece(63,61);
                moveBlackAI(60,62,&data);
                movePiece(61,63);
            }
            if(castleRightWhiteLong && board[3] == '_' && board[2] == '_' && board[1] == '_'){
                movePiece(56,59);
                moveBlackAI(60,58,&data);
                movePiece(59,56);
            }
            break;
        }
    }
    printf("total time elapsed = %i\n",__rdtsc()-mTime>>16);
    if(!data.movC){
        printf("stalemate");
        scanf("");
        exit(0);
    }
    if(data.pts > 50){
        printf("White AI has won!");
        scanf("");
        exit(0);
    }
    if(data.pts < -50){
        printf("Black AI has won!");
        scanf("");
        exit(0);
    }
    u1 mv = __rdtsc()%data.movC;
    movePieceWhite(master.mov[mv].src,master.mov[mv].dst);
    switch(board[master.mov[mv].dst]){
    case 'P':
        if(master.mov[mv].dst < 8) board[master.mov[mv].dst] = 'Q';
        break;
    case 'K':
        if(master.mov[mv].src == 60){
            if(master.mov[mv].dst == 62){
                movePiece(63,61);
                castleRightWhiteShort = 0;
                castleRightWhiteLong = 0;
            }
            else if(master.mov[mv].dst == 58){
                movePiece(56,59);
                castleRightWhiteShort = 0;
                castleRightWhiteLong = 0;
            }
        }
        break;
    case 'R':
        if     (master.mov[mv].src == 63) castleRightWhiteShort = 0;
        else if(master.mov[mv].src == 56) castleRightWhiteLong  = 0;
        break;
    }
    printMove(master.mov[mv].src,master.mov[mv].dst);
    printf("%hhi\n",data.pts);
    printBoard();
}

void blackAI(){
    u8 mTime = __rdtsc();
    AIMOVEDATA data ={.movC = 0,.pts = -128,.minDpt = 0};
    for(i4 i = 0;i < 64;i++){
        switch(board[i]){
        case 'p':
            if(board[i+8] == '_'){
                moveBlackAI(i,i+8,&data);
                if(i < 16 && board[i+16] == '_') moveBlackAI(i,i+16,&data);
            }
            if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7) captureBlackAI(i,i+9,&data);
            if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0) captureBlackAI(i,i+7,&data);
            break;
        case 'n':{
            u1 sx = i >> 3;
            u1 sy = i & 7;
            if(sx > 0){
                if(sy < 6) moveCaptureBlackAI(i,i-6,&data);
                if(sy > 1) moveCaptureBlackAI(i,i-10,&data);
                if(sx > 1){
                    if(sy < 7) moveCaptureBlackAI(i,i-15,&data);
                    if(sy > 0) moveCaptureBlackAI(i,i-17,&data);
                }
            }
            if(sx < 7){
                if(sy > 1) moveCaptureBlackAI(i,i+6,&data);
                if(sy < 6) moveCaptureBlackAI(i,i+10,&data);
                if(sx < 6){
                    if(sy > 0) moveCaptureBlackAI(i,i+15,&data);
                    if(sy < 7) moveCaptureBlackAI(i,i+17,&data);
                }
            }
            break;
        }
        case 'b':
            diagonalBlackAI(i,&data);
            break;
        case 'r':
            straightBlackAI(i,&data);
            break;
        case 'q':
            diagonalBlackAI(i,&data);
            straightBlackAI(i,&data);
            break;
        case 'k':
            i4 j = -1,lm = 2;
            if((i & 7) == 0) j++;
            if((i & 7) == 7) lm--;
            for(;j < lm;j++){
                for(i4 i3 = -8;i3 < 9;i3+=8) if(j+i3!=0&&i+j+i3>0&&i+j+i3<64) moveCaptureBlackAI(i,i+j+i3,&data);
            }
            if(castleRightBlackShort && board[5] == '_' && board[6] == '_'){
                movePiece(7,5);
                moveBlackAI(4,6,&data);
                movePiece(5,7);
            }
            if(castleRightBlackLong && board[3] == '_' && board[2] == '_' && board[1] == '_'){
                movePiece(0,3);
                moveBlackAI(4,2,&data);
                movePiece(3,0);
            }
            break;
        }
    }
    printf("total time elapsed = %i\n",__rdtsc()-mTime>>16);
    if(!data.movC){
        printf("stalemate");
        scanf("");
        exit(0);
    }
    if(data.pts == 75){
        printf("AI has won :)");
        scanf("");
        exit(0);
    }
    if(data.pts == -75){
        printf("player has won!");
        scanf("");
        exit(0);
    }
    u1 mv = __rdtsc()%data.movC;
    movePieceBlack(master.mov[mv].src,master.mov[mv].dst);
    switch(board[master.mov[mv].dst]){
    case 'p':
        if(master.mov[mv].dst > 54) board[master.mov[mv].dst] = 'q';
        break;
    case 'k':
        if(master.mov[mv].src == 4){
            if(master.mov[mv].dst == 6){
                movePiece(7,5);
                castleRightBlackShort = 0;
                castleRightBlackLong = 0;
            }
            else if(master.mov[mv].dst == 2){
                movePiece(0,3);
                castleRightBlackShort = 0;
                castleRightBlackLong = 0;
            }
        }
        break;
    case 'r':
        if     (master.mov[mv].src == 7) castleRightBlackShort = 0;
        else if(master.mov[mv].src == 0) castleRightBlackLong  = 0;
        break;
    }
    printMove(master.mov[mv].src,master.mov[mv].dst);
    printf("%hhi\n",data.pts);
    printBoard();
}

void whitePlayer(){
    i1 inp[5];
input:
    scanf("%s",inp);
    u1 src = (inp[1] - '0' << 3) + inp[0] - 'A';
    u1 dst = (inp[4] - '0' << 3) + inp[3] - 'A';
    if(src > 63 || dst > 63 || (board[dst] > 'A' && board[dst] < 'Z')) goto input;
    switch(board[src]){
    case '_': goto input;
    case 'K':
        if(abs((src&7) - (dst&7)) == 1 || abs((src>>3) - (dst>>3)) == 1){
            board[dst] == '_' ? movePiece(src,dst) : capturePiece(src,dst);
            castleRightWhiteShort = 0;
            castleRightWhiteLong  = 0;
        }
        else if(dst == 62 && castleRightWhiteShort){
            for(i4 i = 61;i < 63;i++) if(board[i] != '_') goto input;
            movePiece(src,dst);
            movePiece(63,61);
        }
        else if(dst == 58 && castleRightWhiteLong){
            for(i4 i = 59;i < 56;i--) if(board[i] != '_') goto input;
            movePiece(src,dst);
            movePiece(56,59);
        }
        else goto input;
        break;
    case 'Q':
        if(((src>>3)==(dst>>3)||(src&7)==(dst&7)) && !checkStraight(src,dst)) goto input;
        else if(!checkDiagonal(src,dst)) goto input; 
        break;
    case 'N':
        if((abs((src&7)-(dst&7))==1&&abs((src>>3)-(dst>>3))==2)||(abs((src&7)-(dst&7))==2&&abs((src>>3)-(dst>>3))==1)) {
            if(board[dst] == '_') movePiece(src,dst);
            else if(board[dst] < 'z' && board[dst] > 'a') capturePiece(src,dst);
        }
        break;
    case 'B':
        if(!checkDiagonal(src,dst)) goto input; 
        break;
    case 'R':
        if(dst == 0) castleRightWhiteLong  = 0;
        if(dst == 7) castleRightWhiteShort = 0;
        if(!checkStraight(src,dst)) goto input; 
        break;
    case 'P':
        if((src>>3)-1 == (dst>>3)) {
            if((src&7)==(dst&7)) {
                if(board[dst] == '_') {
                    whitePromotion(src,dst);
                    movePiece(src,dst);
                }
                else goto input;
            }
            else if(abs((src&7) - (dst&7)) == 1){
                if(board[dst] >= 'a' && board[dst] <= 'z'){
                    whitePromotion(src,dst);
                    capturePiece(src,dst);
                }
                else goto input;
            }
        }
        else if((src>>3)-2 == (dst>>3) && (src>>3)==6 && board[src-8]=='_' && board[dst]=='_') movePiece(src,dst);
        else goto input;
        break;
    }
    printBoard();
}

void blackPlayer(){
    i1 inp[5];
input:
    scanf("%s",inp);
    u1 src = (inp[1] - '0' << 3) + inp[0] - 'A';
    u1 dst = (inp[4] - '0' << 3) + inp[3] - 'A';
    if(src > 63 || dst > 63 || (board[dst] > 'a' && board[dst] < 'z')) goto input;
    switch(board[src]){
    case '_': goto input;
    case 'k': 
        if(abs((src&7) - (dst&7)) == 1 || abs((src>>3) - (dst>>3)) == 1) {
            board[dst] == '_' ? movePiece(src,dst) : capturePiece(src,dst);
            castleRightBlackLong = 0;
            castleRightBlackShort = 0;
        }
        else if(dst == 6 && castleRightBlackShort){
            for(i4 i = 5;i < 7;i++) if(board[i] != '_') goto input;
            movePiece(src,dst);
            movePiece(7,5);
        }
        else if(dst == 2 && castleRightBlackLong){
            for(i4 i = 3;i > 0;i--) if(board[i] != '_') goto input;
            movePiece(src,dst);
            movePiece(0,3);
        }
        else goto input;
        break;
    case 'q':
        if(((src>>3)==(dst>>3)||(src&7)==(dst&7)) && !checkStraight(src,dst)) goto input;
        else if(!checkDiagonal(src,dst)) goto input;
        break;
    case 'n': {
        if((abs((src&7)-(dst&7))==1&&abs((src>>3)-(dst>>3))==2)||(abs((src&7)-(dst&7))==2&&abs((src>>3)-(dst>>3))==1)){
            if(board[dst] == '_') movePiece(src,dst);
            else if(board[dst] < 'Z' && board[dst] > 'A') capturePiece(src,dst);
        }
        break;
    }
    case 'b':
        if(!checkDiagonal(src,dst)) goto input;
        break;
    case 'r':
        if(dst == 0) castleRightBlackLong  = 0;
        if(dst == 7) castleRightBlackShort = 0;
        if(!checkStraight(src,dst)) goto input;
        break;
    case 'p':
        if((src>>3) == (dst>>3)+1){
            if((src&7) == (dst&7)){
                if(board[dst] == '_'){
                    blackPromotion(src,dst);
                    movePiece(src,dst);
                }
                else goto input;
            }
            else if(abs((src&7) - (dst&7))==1){
                if(board[dst] >= 'A' && board[dst] <= 'Z'){
                    blackPromotion(src,dst);
                    capturePiece(src,dst);
                }
                else goto input;
            }
        }
        else if((src>>3)+2 == (dst>>3) && (src>>3)==6 && board[src+8]=='_' && board[dst]=='_') movePiece(src,dst);
        else goto input;
        break;
    }
    printBoard();
}

void main(){
    i1 gameMode;
    if(board[56] != 'R') castleRightWhiteLong  = 0;
    if(board[63] != 'R') castleRightWhiteShort = 0;
    if(board[0] != 'r')  castleRightBlackLong  = 0;
    if(board[7] != 'r')  castleRightBlackShort = 0;
    master.mov = malloc(sizeof(CHESSMOVE)*64);
    printf("welcome to chessTM\nwhich gamemode?\n0 = player vs AI\n1 = AI vs AI\n2 = AI vs player\n3 = player vs player\n");
    scanf("%i",&gameMode);
    switch(gameMode){
    case 0:
        printBoard();
        for(;;) {
            whitePlayer();
            blackAI();
        }
        break;
    case 1:
        printBoard();
        for(;;){
            pcsValWhite();
            whiteAI();
            pcsValBlack();
            blackAI();
        }
        break;
    case 2:
        pcsValWhite();
        for(;;){
            whiteAI();
            blackPlayer();
        }
        break;
    case 3:
        printBoard();
        for(;;){
            whitePlayer();
            blackPlayer();
        }
        break;
    }
}
