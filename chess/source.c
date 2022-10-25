#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <intrin.h>

#include "source.h"

CHESSMASTER master;

i8 i8max(i8 p1,i8 p2){
    return p1 > p2 ? p1 : p2;
}

i8 i8min(i8 p1,i8 p2){
    return p1 < p2 ? p1 : p2;
}

char board[64] ={
    'r','n','b','q','k','b','n','r',
    'p','p','p','p','p','p','p','p',
    '_','_','_','_','_','_','_','_',
    '_','_','_','_','_','_','_','_',
    '_','_','_','_','_','_','_','_',
    '_','_','_','_','_','_','_','_',
    'P','P','P','P','P','P','P','P',
    'R','N','B','Q','K','B','N','R',
};

char pcsVal[122] ={
    ['p'] = -1,['b'] = -3,['n'] = -3,['r'] = -5,['q'] = -9,['k'] = -75,
    ['P'] =  1,['B'] =  3,['N'] =  3,['R'] =  5,['Q'] =  9,['K'] =  75
};

u8 castleRights = 0b00001111;

i8 minBlackAI(u8 depth,EVAL eval);
i8 minWhiteAI(u8 depth,EVAL eval);
i8 maxBlackAI(u8 depth,EVAL eval);
i8 maxWhiteAI(u8 depth,EVAL eval);

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

void printBoardWhite(){
    printf("   ");
    for(int i = 0; i < 8; i++) {
        printf("%c",'A'+i);
        printf("%c",' ');
    }
    printf("\n  ");
    for(int i = 0; i < 16; i++) {
        printf("%c",'-');
    }
    printf("%c",'\n');
    for(int i = 0; i < 8; i++) {
        printf("%i",i);
        printf("| ");
        for(int i2 = 0; i2 < 8; i2++) {
            if(board[i*8+i2] == '_'){
                if((i2 + (i & 1)) & 1){
                    printf("%c",'.');
                }
                else{
                    printf("%c",',');
                }
            }
            else{
                printf("%c",board[i*8+i2]);
            }
            printf("%c",' ');
        }
        printf("%c",'\n');
    }
    printf("  ");
    for(int i = 0; i < 16; i++) {
        printf("%c",'-');
    }
    printf("%c",'\n');
}

void printBoardBlack(){
    printf("   ");
    for(int i = 7;i >= 0;i--) {
        printf("%c",'A'+i);
        printf("%c",' ');
    }
    printf("\n  ");
    for(int i = 0;i < 16;i++) {
        printf("%c",'-');
    }
    printf("%c",'\n');
    for(int i = 7;i >= 0;i--) {
        printf("%i",i);
        printf("| ");
        for(int i2 = 7;i2 >= 0;i2--) {
            if(board[i*8+i2] == '_'){
                if((i2 + (i & 1)) & 1){
                    printf("%c",'.');
                }
                else{
                    printf("%c",',');
                }
            }
            else{
                printf("%c",board[i*8+i2]);
            }
            printf("%c",' ');
        }
        printf("%c",'\n');
    }
    printf("  ");
    for(int i = 0; i < 16; i++) {
        printf("%c",'-');
    }
    printf("%c",'\n');
}

void movePiece(int SX,int SY,int DX,int DY){
    board[SX*8+SY] ^= board[DX*8+DY];
    board[DX*8+DY] ^= board[SX*8+SY];
    board[SX*8+SY] ^= board[DX*8+DY];
}

void capturePiece(int SX,int SY,int DX,int DY){
    board[DX*8+DY] = board[SX*8+SY];
    board[SX*8+SY] = '_';
}

void movePieceD(u8 src,u8 dst){
    char t = board[src];
    board[src] = board[dst];
    board[dst] = t;
}

void capturePieceD(u8 src,u8 dst){
    board[dst] = board[src];
    board[src] = '_';
}

void movePieceWhite(int SX,int SY,int DX,int DY){
    if(board[DX*8+DY] == '_'){
        movePiece(SX,SY,DX,DY);
    }
    else if(board[DX*8+DY] >= 'a' && board[DX*8+DY] <= 'z'){
        capturePiece(SX,SY,DX,DY);
    }
}

void movePieceBlack(int SX,int SY,int DX,int DY){
    if(board[DX*8+DY] == '_'){
        movePiece(SX,SY,DX,DY);
    }
    else if(board[DX*8+DY] >= 'A' && board[DX*8+DY] <= 'Z'){
        capturePiece(SX,SY,DX,DY);
    }
}

void printMove(u8 src,u8 dst){
    printf("%c",(src&7)+'A');
    printf("%c",(src>>3)+'0');
    printf("%c",'-');
    printf("%c",(dst&7)+'A');
    printf("%c = ",(dst>>3)+'0');
}

i8 maxBlackAIdiagonal0(u8 i,i8 pts,i8 tpts){
    for(int i2 = i - 9;i2 >= 0 && (i2 & 7) != 7 && board[i2] < 'a';i2-=9){
        if(board[i2] < 'Z'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 7;i2 >= 0 && (i2 & 7) != 0 && board[i2] < 'a';i2-=7){
        if(board[i2] < 'Z'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 9;i2 < 64 && (i2 & 7) != 0 && board[i2] < 'a';i2+=9){
        if(board[i2] < 'Z'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 7;i2 < 64 && (i2 & 7) != 7 && board[i2] < 'a';i2+=7){
        if(board[i2] < 'Z'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    return tpts;
}

i8 maxWhiteAIdiagonal0(u8 i,i8 pts,i8 tpts){
    for(int i2 = i - 9;i2 >= 0 && (i2 & 7) != 7 && board[i2] > 'Z';i2-=9){
        if(board[i2] > 'a'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 7;i2 >= 0 && (i2 & 7) != 0 && board[i2] > 'Z';i2-=7){
        if(board[i2] > 'a'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 9;i2 < 64 && (i2 & 7) != 0 && board[i2] > 'Z';i2+=9){
        if(board[i2] > 'a'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 7;i2 < 64 && (i2 & 7) != 7 && board[i2] > 'Z';i2+=7){
        if(board[i2] > 'a'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    return tpts;
}

i8 minBlackAIdiagonal0(u8 i,i8 pts,i8 tpts){
    for(int i2 = i - 9;i2 >= 0 && (i2 & 7) != 7 && board[i2] > 'Z';i2-=9){
        if(board[i2] > 'a'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 7;i2 >= 0 && (i2 & 7) != 0 && board[i2] > 'Z';i2-=7){
        if(board[i2] > 'a'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 9;i2 < 64 && (i2 & 7) != 0 && board[i2] > 'Z';i2+=9){
        if(board[i2] > 'a'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 7;i2 < 64 && (i2 & 7) != 7 && board[i2] > 'Z';i2+=7){
        if(board[i2] > 'a'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    return tpts;
}

i8 minWhiteAIdiagonal0(u8 i,i8 pts,i8 tpts){
    for(int i2 = i - 9;i2 >= 0 && (i2 & 7) != 7 && board[i2] < 'a';i2-=9){
        if(board[i2] < 'Z'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 7;i2 >= 0 && (i2 & 7) != 0 && board[i2] < 'a';i2-=7){
        if(board[i2] < 'Z'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 9; i2 < 64 && (i2 & 7) != 0 && board[i2] < 'a';i2+=9){
        if(board[i2] < 'Z'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 7;i2 < 64 && (i2 & 7) != 7 && board[i2] < 'a';i2+=7){
        if(board[i2] < 'Z'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    return tpts;
}

i8 maxBlackAIstraight0(u8 i,i8 pts,i8 tpts){
    for(int i2 = i + 1;(i2 & 7) != 0 && board[i2] < 'a';i2++){
        if(board[i2] < 'Z'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 1;(i2 & 7) != 7 && board[i2] < 'a';i2--){
        if(board[i2] < 'Z'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 8;i2 >= 0 && board[i2] < 'a';i2-=8){
        if(board[i2] < 'Z'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 8;i2 < 64 && board[i2] < 'a';i2+=8){
        if(board[i2] < 'Z'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    return tpts;
}

i8 maxWhiteAIstraight0(u8 i,i8 pts,i8 tpts){
    for(int i2 = i + 1;(i2 & 7) != 0 && board[i2] > 'Z';i2++){
        if(board[i2] > 'a'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 1;(i2 & 7) != 7 && board[i2] > 'Z';i2--){
        if(board[i2] > 'a'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 8;i2 >= 0 && board[i2] > 'Z';i2-=8){
        if(board[i2] > 'a'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 8;i2 < 64 && board[i2] > 'Z';i2+=8){
        if(board[i2] > 'a'){
            tpts = i8max(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    return tpts;
}

i8 minBlackAIstraight0(u8 i,i8 pts,i8 tpts){
    for(int i2 = i + 1;(i2 & 7) != 0 && board[i2] > 'Z';i2++){
        if(board[i2] > 'a'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 1;(i2 & 7) != 7 && board[i2] > 'Z';i2--){
        if(board[i2] > 'a'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 8;i2 >= 0 && board[i2] > 'Z';i2-=8){
        if(board[i2] > 'a'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 8;i2 < 64 && board[i2] > 'Z';i2+=8){
        if(board[i2] > 'a'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    return tpts;
}

i8 minWhiteAIstraight0(u8 i,i8 pts,i8 tpts){
    for(int i2 = i + 1; (i2 & 7) != 0 && board[i2] < 'a';i2++){
        if(board[i2] < 'Z'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 1; (i2 & 7) != 7 && board[i2] < 'a';i2--){
        if(board[i2] < 'Z'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i - 8;i2 >= 0 && board[i2] < 'a';i2-=8){
        if(board[i2] < 'Z'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    for(int i2 = i + 8;i2 < 64 && board[i2] < 'a';i2+=8){
        if(board[i2] < 'Z'){
            tpts = i8min(tpts,pts+pcsVal[board[i2]]);
            break;
        }
    }
    return tpts;
}

void maxBlackAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    for(int i2 = i - 9;board[i2] < 'a' && i2 >= 0 && (i2 & 7) != 7;i2-=9){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 7;board[i2] < 'a' && i2 >= 0 && (i2 & 7) != 0;i2-=7){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 9;board[i2] < 'a' && i2 < 64 && (i2 & 7) != 0;i2+=9){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 7;board[i2] < 'a' && i2 < 64 && (i2 & 7) != 7;i2+=7){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
}

void maxWhiteAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    for(int i2 = i - 9;i2 >= 0 && (i2 & 7) != 7 && board[i2] > 'Z';i2-=9){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 7;i2 >= 0 && (i2 & 7) != 0 && board[i2] > 'Z';i2-=7){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 9;i2 < 64 && (i2 & 7) != 0 && board[i2] > 'Z';i2+=9){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 7;i2 < 64 && (i2 & 7) != 7 && board[i2] > 'Z';i2+=7){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
}

void minBlackAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    for(int i2 = i - 9;i2 >= 0 && (i2 & 7) != 7 && board[i2] > 'Z';i2-=9){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 7;i2 >= 0 && (i2 & 7) != 0 && board[i2] > 'Z';i2-=7){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 9;i2 < 64 && (i2 & 7) != 0 && board[i2] > 'Z';i2+=9){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 7;i2 < 64 && (i2 & 7) != 7 && board[i2] > 'Z';i2+=7){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
}

void minWhiteAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    for(int i2 = i - 9;i2 >= 0 && (i2 & 7) != 7 && board[i2] < 'a';i2-=9){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 7;i2 >= 0 && (i2 & 7) != 0 && board[i2] < 'a';i2-=7){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 9;i2 < 64 && (i2 & 7) != 0 && board[i2] < 'a';i2+=9){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 7;i2 < 64 && (i2 & 7) != 7 && board[i2] < 'a';i2+=7){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
}

void maxBlackAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    for(int i2 = i + 1;(board[i2] < 'a' || board[i2] > 'z') && (i2 & 7) != 0;i2++){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 1;(board[i2] < 'a' || board[i2] > 'z') && (i2 & 7) != 7;i2--){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 8;(board[i2] < 'a' || board[i2] > 'z') && i2 >= 0;i2-=8){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 8;(board[i2] < 'a' || board[i2] > 'z') && i2 < 64;i2+=8){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
}

void maxWhiteAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    for(int i2 = i + 1;(i2 & 7) != 0 && board[i2] > 'Z';i2++){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 1;(i2 & 7) != 7 && board[i2] > 'Z';i2--){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 8;i2 >= 0 && board[i2] > 'Z';i2-=8){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 8;i2 < 64 && board[i2] > 'Z';i2+=8){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
}

void minBlackAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    for(int i2 = i + 1;(i2 & 7) != 0 && board[i2] > 'Z';i2++){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 1;(i2 & 7) != 7 && board[i2] > 'Z';i2--){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 8;i2 >= 0 && board[i2] > 'Z';i2-=8){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 8;i2 < 64 && board[i2] > 'Z';i2+=8){
        if(board[i2] > 'a'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
}

void minWhiteAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    for(int i2 = i + 1;(i2 & 7) != 0 && board[i2] < 'a';i2++){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 1;(i2 & 7) != 7 && board[i2] < 'a';i2--){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i - 8;i2 >= 0 && board[i2] < 'a';i2-=8){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
            break;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    for(int i2 = i + 8;i2 < 64 && board[i2] < 'a';i2+=8){
        if(board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
}

void maxBlackAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] < 'Z'){
        eval->pts += pcsVal[board[i+dst]];
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
    else if(board[i+dst] == '_'){
        movePieceD(i,i+dst);
        *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
}

void maxWhiteAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] > 'a'){
        eval->pts += pcsVal[board[i+dst]];
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
    else if(board[i+dst] == '_'){
        movePieceD(i,i+dst);
        *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
}

void minBlackAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] > 'a'){
        eval->pts += pcsVal[board[i+dst]];
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
    else if(board[i+dst] == '_'){
        movePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
}

void minWhiteAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] < 'Z'){
        eval->pts += pcsVal[board[i+dst]];
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
    else if(board[i+dst] == '_'){
        movePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
}

i8 maxBlackAI(u8 depth,EVAL eval){
    i8 tpts;
    if(!depth){
        tpts = eval.pts;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 47){
                    if((i & 7) != 7){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+9]]+8);
                    }
                    if((i & 7) != 0){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+7]]+8);
                    }
                    else if(board[i+8] == '_'){
                        tpts = i8max(tpts,eval.pts+8);
                    }
                }
                else{
                    if((i & 7) != 7){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+9]]);
                    }
                    if((i & 7) != 0){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+7]]);
                    }
                }
                break;
            case 'n':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i-6]]);
                    }
                    if(sy > 1){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i-10]]);
                    }
                    if(sx > 1){
                        if(sy < 7){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i-15]]);
                        }
                        if(sy > 0){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i-17]]);
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+6]]);
                    }
                    if(sy < 6){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+10]]);
                    }
                    if(sx < 6){
                        if(sy > 0){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i+15]]);
                        }
                        if(sy < 7){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i+17]]);
                        }
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
                int i2 = -1,lm = 2;
                if((i & 7) == 0){
                    i2++;
                }
                if((i & 7) == 7){
                    lm--;
                }
                for(;i2 < lm;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i2+i3!=0&&i+i2+i3>=0&&i+i2+i3<64){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i+i2+i3]]);
                        }
                    }
                }
                break;
            }
            }
        }
    }
    else{
        tpts = -75;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 47){
                    if(board[i+8] == '_'){
                        board[i] = 'q';
                        movePieceD(i,i+8);
                        eval.pts += 8;
                        tpts = i8max(tpts,minBlackAI(depth-1,eval));
                        eval.pts -= 8;
                        movePieceD(i+8,i);
                        board[i] = 'p';
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if((i & 7) != 7 && board[i+9] < 'Z'){
                        eval.pts += pcsVal[board[i+9]] + 8;
                        char tpcs = board[i+9];
                        board[i] = 'q';
                        capturePieceD(i,i+9);
                        tpts = i8max(tpts,minBlackAI(depth-1,eval));
                        board[i] = 'p';
                        board[i+9] = tpcs;
                        eval.pts -= pcsVal[board[i+9]] + 8;
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if((i & 7) != 0 && board[i+7] < 'Z'){
                        eval.pts += pcsVal[board[i+7]] + 8;
                        char tpcs = board[i+7];
                        board[i] = 'q';
                        capturePieceD(i,i+7);
                        tpts = i8max(tpts,minBlackAI(depth-1,eval));
                        board[i] = 'p';
                        board[i+7] = tpcs;
                        eval.pts -= pcsVal[board[i+7]] + 8;
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                else{
                    if(board[i+8] == '_'){
                        movePieceD(i,i+8);
                        tpts = i8max(tpts,minBlackAI(depth-1,eval));
                        movePieceD(i+8,i);
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i > 7 && i < 16 && board[i+16] == '_'){
                            movePieceD(i,i+16);
                            tpts = i8max(tpts,minBlackAI(depth-1,eval));
                            movePieceD(i+16,i);
                            eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                    if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
                        eval.pts += pcsVal[board[i+9]];
                        char tpcs = board[i+9];
                        capturePieceD(i,i+9);
                        tpts = i8max(tpts,minBlackAI(depth-1,eval));
                        board[i] = board[i+9];
                        board[i+9] = tpcs;
                        eval.pts -= pcsVal[board[i+9]];
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
                        eval.pts += pcsVal[board[i+7]];
                        char tpcs = board[i+7];
                        capturePieceD(i,i+7);
                        tpts = i8max(tpts,minBlackAI(depth-1,eval));
                        board[i] = board[i+7];
                        board[i+7] = tpcs;
                        eval.pts -= pcsVal[board[i+7]];
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                break;
            case 'n':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        maxBlackAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy > 1){
                        maxBlackAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx > 1){
                        if(sy < 7){
                            maxBlackAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy > 0){
                            maxBlackAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        maxBlackAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy < 6){
                        maxBlackAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx < 6){
                        if(sy > 0){
                            maxBlackAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy < 7){
                            maxBlackAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                break;
            }
            case 'b':
                maxBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'r':
                maxBlackAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'q':
                maxBlackAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                maxBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'k':{
                int i2 = -1,lm = 2;
                if((i & 7) == 0){
                    i2++;
                }
                if((i & 7) == 7){
                    lm--;
                }
                for(;i2 < lm;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i2+i3!=0&&i+i2+i3>=0&&i+i2+i3<64){
                            if(board[i+i2+i3] < 'Z' && board[i+i2+i3] > 'A'){
                                eval.pts += pcsVal[board[i+i2+i3]];
                                char tpcs = board[i+i2+i3];
                                capturePieceD(i,i+i2+i3);
                                tpts = i8max(tpts,minBlackAI(depth-1,eval));
                                board[i] = board[i+i2+i3];
                                board[i+i2+i3] = tpcs;
                                eval.pts -= pcsVal[board[i+i2+i3]];
                                eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;
                                }
                            }
                            else if(board[i+i2+i3] == '_'){
                                movePieceD(i,i+i2+i3);
                                tpts = i8max(tpts,minBlackAI(depth-1,eval));
                                movePieceD(i+i2+i3,i);
                                eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;
                                }
                            }
                        }
                    }
                }
                if(i == 4){
                    if(castleRights & 1 && board[5] == '_' && board[6] == '_'){
                        movePieceD(4,6);
                        movePieceD(7,5);
                        castleRights &= ~3;
                        tpts = i8max(tpts,minBlackAI(depth-1,eval));
                        movePieceD(6,4);
                        movePieceD(5,7);
                        castleRights |= 3;
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;;
                        }
                    }
                    if(castleRights & 2 && board[3] == '_' && board[2] == '_' && board[1] == '_'){
                        movePieceD(4,2);
                        movePieceD(0,3);
                        castleRights &= ~3;
                        tpts = i8max(tpts,minBlackAI(depth-1,eval));
                        movePieceD(2,4);
                        movePieceD(3,0);
                        castleRights |= 3;
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;;
                        }
                    }
                }
                break;
            }
            }
        }
    }
    return tpts;
}

i8 maxWhiteAI(u8 depth,EVAL eval){
    i8 tpts;
    if(!depth){
        tpts = eval.pts;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if((i & 7) != 0){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i-9]]+8);
                    }
                    if((i & 7) != 7){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i-7]]+8);
                    }
                    else if(board[i-8] == '_'){
                        tpts = i8max(tpts,eval.pts+8);
                    }
                }
                else{
                    if((i & 7) != 0){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i-9]]);
                    }
                    if((i & 7) != 7){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i-7]]);
                    }
                }
                break;
            case 'N':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i-6]]);
                    }
                    if(sy > 1){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i-10]]);
                    }
                    if(sx > 1){
                        if(sy < 7){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i-15]]);
                        }
                        if(sy > 0){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i-17]]);
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+6]]);
                    }
                    if(sy < 6){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+10]]);
                    }
                    if(sx < 6){
                        if(sy > 0){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i+15]]);
                        }
                        if(sy < 7){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i+17]]);
                        }
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
                int i2 = -1,lm = 2;
                if((i & 7) == 0){
                    i2++;
                }
                if((i & 7) == 7){
                    lm--;
                }
                for(;i2 < lm;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i2+i3!=0&&i+i2+i3>=0&&i+i2+i3<64){
                            tpts = i8max(tpts,eval.pts+pcsVal[board[i+i2+i3]]);
                        }
                    }
                }
                break;
            }
            }
        }
    }
    else{
        tpts = -75;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if(board[i-8] == '_'){
                        board[i] = 'Q';
                        movePieceD(i,i-8);
                        eval.pts += 8;
                        tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                        eval.pts -= 8;
                        movePieceD(i-8,i);
                        board[i] = 'P';
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if((i & 7) != 0 && board[i-9] > 'a'){
                        eval.pts += pcsVal[board[i-9]] + 8;
                        char tpcs = board[i-9];
                        board[i] = 'Q';
                        capturePieceD(i,i-9);
                        tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                        board[i] = 'P';
                        board[i-9] = tpcs;
                        eval.pts -= pcsVal[board[i-9]] + 8;
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if((i & 7) != 7 && board[i+7] > 'a'){
                        eval.pts += pcsVal[board[i-7]] + 8;
                        char tpcs = board[i-7];
                        board[i] = 'Q';
                        capturePieceD(i,i-7);
                        tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                        board[i] = 'P';
                        board[i-7] = tpcs;
                        eval.pts -= pcsVal[board[i-7]] + 8;
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                else{
                    if(board[i-8] == '_'){
                        movePieceD(i,i-8);
                        tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                        movePieceD(i-8,i);
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i > 47 && board[i-16] == '_'){
                            movePieceD(i,i-16);
                            tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                            movePieceD(i-16,i);
                            eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                    if((i & 7) != 0 && board[i-9] > 'a'){
                        eval.pts += pcsVal[board[i-9]];
                        char tpcs = board[i-9];
                        capturePieceD(i,i-9);
                        tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                        board[i] = board[i-9];
                        board[i-9] = tpcs;
                        eval.pts -= pcsVal[board[i-9]];
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if((i & 7) != 7 && board[i-7] > 'a'){
                        eval.pts += pcsVal[board[i-7]];
                        char tpcs = board[i-7];
                        capturePieceD(i,i-7);
                        tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                        board[i] = board[i-7];
                        board[i-7] = tpcs;
                        eval.pts -= pcsVal[board[i-7]];
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                break;
            case 'N':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        maxWhiteAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy > 1){
                        maxWhiteAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx > 1){
                        if(sy < 7){
                            maxWhiteAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy > 0){
                            maxWhiteAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        maxWhiteAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy < 6){
                        maxWhiteAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx < 6){
                        if(sy > 0){
                            maxWhiteAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy < 7){
                            maxWhiteAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                break;
            }
            case 'B':
                maxWhiteAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'R':
                maxWhiteAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'Q':
                maxWhiteAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                maxWhiteAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'K':{
                int i2 = -1,lm = 2;
                if((i & 7) == 0){
                    i2++;
                }
                if((i & 7) == 7){
                    lm--;
                }
                for(;i2 < lm;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i2+i3!=0&&i+i2+i3>=0&&i+i2+i3<64){
                            if(board[i+i2+i3] > 'a'){
                                eval.pts += pcsVal[board[i+i2+i3]];
                                char tpcs = board[i+i2+i3];
                                capturePieceD(i,i+i2+i3);
                                tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                                board[i] = board[i+i2+i3];
                                board[i+i2+i3] = tpcs;
                                eval.pts -= pcsVal[board[i+i2+i3]];
                                eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;
                                }
                            }
                            else if(board[i+i2+i3] == '_'){
                                movePieceD(i,i+i2+i3);
                                tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                                movePieceD(i+i2+i3,i);
                                eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;
                                }
                            }
                        }
                    }
                }
                if(i == 60){
                    if(castleRights & 4 && board[61] == '_' && board[62] == '_'){
                        movePieceD(60,62);
                        movePieceD(63,61);
                        castleRights &= ~12;
                        tpts = i8min(tpts,minWhiteAI(depth-1,eval));
                        movePieceD(62,60);
                        movePieceD(60,61);
                        castleRights |= 12;
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;;
                        }
                    }
                    if(castleRights & 8 && board[59] == '_' && board[58] == '_' && board[57] == '_'){
                        movePieceD(60,58);
                        movePieceD(56,59);
                        castleRights &= ~12;
                        tpts = i8min(tpts,minWhiteAI(depth-1,eval));
                        movePieceD(58,60);
                        movePieceD(59,56);
                        castleRights |= 12;
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;;
                        }
                    }
                }
                break;
            }
            }
        }
    }
    return tpts;
}

i8 minBlackAI(u8 depth,EVAL eval){
    i8 tpts;
    if(!depth){
        tpts = eval.pts;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if((i & 7) != 0){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-9]]-8);
                    }
                    if((i & 7) != 7){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-7]]-8);
                    }
                    else if(board[i-8] == '_'){
                        tpts = i8min(tpts,eval.pts-8);
                    }
                }
                else{
                    if((i & 7) != 0){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-9]]);
                    }
                    if((i & 7) != 7){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-7]]);
                    }
                }
                break;
            case 'N':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-6]]);
                    }
                    if(sy > 1){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-10]]);
                    }
                    if(sx > 1){
                        if(sy < 7){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i-15]]);
                        }
                        if(sy > 0){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i-17]]);
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i+6]]);
                    }
                    if(sy < 6){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i+10]]);
                    }
                    if(sx < 6){
                        if(sy > 0){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i+15]]);
                        }
                        if(sy < 7){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i+17]]);
                        }
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
                int i2 = -1,lm = 2;
                if((i & 7) == 0){
                    i2++;
                }
                if((i & 7) == 7){
                    lm--;
                }
                for(;i2 < lm;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i+i2+i3>=0&&i+i2+i3<64){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i+i2+i3]]);
                        }
                    }
                }
                break;
            }
            }
        }
    }
    else{
        tpts = 75;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if(board[i-8] == '_'){
                        board[i] = 'Q';
                        movePieceD(i,i-8);
                        eval.pts -= 8;
                        tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                        eval.pts += 8;
                        movePieceD(i-8,i);
                        board[i] = 'P';
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                        eval.pts += pcsVal[board[i-9]];
                        char tpcs = board[i-9];
                        board[i] = 'Q';
                        capturePieceD(i,i-9);
                        tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                        board[i-9] = tpcs;
                        board[i] = 'P';
                        eval.pts -= pcsVal[board[i-9]];
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                        eval.pts += pcsVal[board[i-7]];
                        char tpcs = board[i-7];
                        board[i] = 'Q';
                        capturePieceD(i,i-7);
                        tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                        board[i-7] = tpcs;
                        board[i] = 'P';
                        eval.pts -= pcsVal[board[i-7]];
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                else{
                    if(board[i-8] == '_'){
                        movePieceD(i,i-8);
                        tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                        movePieceD(i-8,i);
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i > 47 && i < 56 && board[i-16] == '_'){
                            movePieceD(i,i-16);
                            tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                            movePieceD(i-16,i);
                            eval.ab.beta = i8min(eval.ab.beta,tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                        eval.pts += pcsVal[board[i-9]];
                        char tpcs = board[i-9];
                        capturePieceD(i,i-9);
                        tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                        board[i] = board[i-9];
                        board[i-9] = tpcs;
                        eval.pts -= pcsVal[board[i-9]];
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                        eval.pts += pcsVal[board[i-7]];
                        char tpcs = board[i-7];
                        capturePieceD(i,i-7);
                        tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                        board[i] = board[i-7];
                        board[i-7] = tpcs;
                        eval.pts -= pcsVal[board[i-7]];
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                break;
            case 'N':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        minBlackAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy > 1){
                        minBlackAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx > 1){
                        if(sy < 7){
                            minBlackAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy > 0){
                            minBlackAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        minBlackAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy < 6){
                        minBlackAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx < 6){
                        if(sy > 0){
                            minBlackAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy < 7){
                            minBlackAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                break;
            }
            case 'B':
                minBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'R':
                minBlackAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'Q':
                minBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                minBlackAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'K':{
                int i2 = -1,lm = 2;
                if((i & 7) == 0){
                    i2++;
                }
                if((i & 7) == 7){
                    lm--;
                }
                for(;i2 < lm;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i+i2+i3>=0&&i+i2+i3<64){
                            if(board[i+i2+i3] < 'z' && board[i+i2+i3] > 'a'){
                                eval.pts += pcsVal[board[i+i2+i3]];
                                char tpcs = board[i+i2+i3];
                                capturePieceD(i,i+i2+i3);
                                tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                                board[i] = board[i+i2+i3];
                                board[i+i2+i3] = tpcs;
                                eval.pts -= pcsVal[board[i+i2+i3]];
                                eval.ab.beta = i8min(eval.ab.beta,tpts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;
                                }
                            }
                            else if(board[i+i2+i3] == '_'){
                                movePieceD(i,i+i2+i3);
                                tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                                movePieceD(i+i2+i3,i);
                                eval.ab.beta = i8min(eval.ab.beta,tpts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;;
                                }
                            }
                        }
                    }
                }
                if(i == 60){
                    if(castleRights & 4 && board[61] == '_' && board[62] == '_'){
                        movePieceD(60,62);
                        movePieceD(63,61);
                        castleRights &= ~12;
                        tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                        movePieceD(62,60);
                        movePieceD(61,63);
                        castleRights |= 12;
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;;
                        }
                    }
                    if(castleRights & 8 && board[59] == '_' && board[58] == '_' && board[57] == '_'){
                        movePieceD(60,58);
                        movePieceD(56,59);
                        castleRights &= ~12;
                        tpts = i8max(tpts,maxBlackAI(depth-1,eval));
                        movePieceD(58,60);
                        movePieceD(59,56);
                        castleRights |= 12;
                         eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;;
                        }
                    }
                }
                break;
            }
            }
        }
    }
    return tpts;
}

i8 minWhiteAI(u8 depth,EVAL eval){
    i8 tpts;
    if(!depth){
        tpts = eval.pts;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 47){
                    if((i & 7) != 7){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i+9]]-8);
                    }
                    if((i & 7) != 0){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i+7]]-8);
                    }
                    else if(board[i+8] == '_'){
                        tpts = i8min(tpts,eval.pts-8);
                    }
                }
                else{
                    if((i & 7) != 7){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i+9]]);
                    }
                    if((i & 7) != 0){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i+7]]);
                    }
                }
                break;
            case 'n':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-6]]);
                    }
                    if(sy > 1){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-10]]);
                    }
                    if(sx > 1){
                        if(sy < 7){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i-15]]);
                        }
                        if(sy > 0){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i-17]]);
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i+6]]);
                    }
                    if(sy < 6){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i+10]]);
                    }
                    if(sx < 6){
                        if(sy > 0){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i+15]]);
                        }
                        if(sy < 7){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i+17]]);
                        }
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
                int i2 = -1,lm = 2;
                if((i & 7) == 0){
                    i2++;
                }
                if((i & 7) == 7){
                    lm--;
                }
                for(;i2 < lm;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i+i2+i3>=0&&i+i2+i3<64){
                            tpts = i8min(tpts,eval.pts+pcsVal[board[i+i2+i3]]);
                        }
                    }
                }
                break;
            }
            }
        }
    }
    else{
        tpts = 75;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 47){
                    if(board[i+8] == '_'){
                        board[i] = 'q';
                        movePieceD(i,i+8);
                        eval.pts -= 8;
                        tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                        eval.pts += 8;
                        movePieceD(i+8,i);
                        board[i] = 'p';
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if((i & 7) != 7 && board[i+9] < 'Z'){
                        eval.pts += pcsVal[board[i+9]];
                        char tpcs = board[i+9];
                        board[i] = 'q';
                        capturePieceD(i,i+9);
                        tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                        board[i+9] = tpcs;
                        board[i] = 'p';
                        eval.pts -= pcsVal[board[i+9]];
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if((i & 7) != 0 && board[i+7] < 'Z'){
                        eval.pts += pcsVal[board[i+7]];
                        char tpcs = board[i+7];
                        board[i] = 'q';
                        capturePieceD(i,i+7);
                        tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                        board[i+7] = tpcs;
                        board[i] = 'p';
                        eval.pts -= pcsVal[board[i+7]];
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                else{
                    if(board[i+8] == '_'){
                        movePieceD(i,i+8);
                        tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                        movePieceD(i+8,i);
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i < 16 && board[i+16] == '_'){
                            movePieceD(i,i+16);
                            tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                            movePieceD(i+16,i);
                            eval.ab.beta = i8min(eval.ab.beta,tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                    if((i & 7) != 7 && board[i+9] < 'Z'){
                        eval.pts += pcsVal[board[i+9]];
                        char tpcs = board[i+9];
                        capturePieceD(i,i+9);
                        tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                        board[i] = board[i+9];
                        board[i+9] = tpcs;
                        eval.pts -= pcsVal[board[i+9]];
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if((i & 7) != 0 && board[i+7] < 'Z'){
                        eval.pts += pcsVal[board[i+7]];
                        char tpcs = board[i+7];
                        capturePieceD(i,i+7);
                        tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                        board[i] = board[i+7];
                        board[i+7] = tpcs;
                        eval.pts -= pcsVal[board[i+7]];
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                break;
            case 'n':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        minWhiteAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy > 1){
                        minWhiteAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx > 1){
                        if(sy < 7){
                            minWhiteAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy > 0){
                            minWhiteAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        minWhiteAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy < 6){
                        minWhiteAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx < 6){
                        if(sy > 0){
                            minWhiteAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy < 7){
                            minWhiteAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                break;
            }
            case 'b':
                minWhiteAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'r':
                minWhiteAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'q':
                minWhiteAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                minWhiteAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'k':{
                int i2 = -1,lm = 2;
                if((i & 7) == 0){
                    i2++;
                }
                if((i & 7) == 7){
                    lm--;
                }
                for(;i2 < lm;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i+i2+i3>=0&&i+i2+i3<64){
                            if(board[i+i2+i3] < 'Z'){
                                eval.pts += pcsVal[board[i+i2+i3]];
                                char tpcs = board[i+i2+i3];
                                capturePieceD(i,i+i2+i3);
                                tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                                board[i] = board[i+i2+i3];
                                board[i+i2+i3] = tpcs;
                                eval.pts -= pcsVal[board[i+i2+i3]];
                                eval.ab.beta = i8min(eval.ab.beta,tpts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;
                                }
                            }
                            else if(board[i+i2+i3] == '_'){
                                movePieceD(i,i+i2+i3);
                                tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                                movePieceD(i+i2+i3,i);
                                eval.ab.beta = i8min(eval.ab.beta,tpts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;;
                                }
                            }
                        }
                    }
                }
                if(i == 4){
                    if(castleRights & 1 && board[5] == '_' && board[6] == '_'){
                        movePieceD(4,6);
                        movePieceD(7,5);
                        castleRights &= ~3;
                        tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                        movePieceD(6,4);
                        movePieceD(5,7);
                        castleRights |= 3;
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;;
                        }
                    }
                    if(castleRights & 2 && board[3] == '_' && board[2] == '_' && board[1] == '_'){
                        movePieceD(4,2);
                        movePieceD(0,3);
                        castleRights &= ~3;
                        tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                        movePieceD(2,4);
                        movePieceD(3,0);
                        castleRights |= 3;
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;;
                        }
                    }
                }
                break;
            }
            }
        }
    }
    return tpts;
}

void captureBlackAI(u8 src,u8 dst,AIMOVEDATA *data){
    printMove(src,dst);
    char tpcs = board[dst];
    capturePieceD(src,dst);
    printf("%c",'|');
    i8 tpts;
    for(int i = 0;i <= DEPTH;i++){
        printf("dpt %i = ",i);
        printf("pts ");
        u64 mTime  = __rdtsc();
        tpts = minBlackAI(i,(EVAL){ -75,75,pcsVal[tpcs] });
        u64 mTimeE = __rdtsc();
        if(tpts >= 0 && tpts < 10){
            printf("%c",' ');
        }
        if(tpts > -10){
            printf("%c",' ');
        }
        printf("%i|",tpts);
        if(tpts < -50 || tpts > 50){
            data->minDpt = i8max(data->minDpt,i);
            goto n;
        }
        if(mTimeE-mTime>MTIME){
            break;
        }
    }
    data->minDpt = i8max(data->minDpt,DEPTH);
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

void captureWhiteAI(u8 src,u8 dst,AIMOVEDATA *data){
    printMove(src,dst);
    char tpcs = board[dst];
    capturePieceD(src,dst);
    printf("%c",'|');
    i8 tpts;
    for(int i = 0;i <= DEPTH;i++){
        printf("dpt %i = ",i);
        printf("pts ");
        u64 mTime  = __rdtsc();
        tpts = minWhiteAI(i,(EVAL){ -75,75,pcsVal[tpcs]});
        u64 mTimeE = __rdtsc();
        if(tpts >= 0 && tpts < 10){
            printf("%c",' ');
        }
        if(tpts > -10){
            printf("%c",' ');
        }
        printf("%i|",tpts);
        if(tpts < -50 || tpts > 50){
            data->minDpt = i8max(data->minDpt,i);
            goto n;
        }
        if(mTimeE-mTime>MTIME){
            break;
        }
    }
    data->minDpt = i8max(data->minDpt,DEPTH);
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

void moveBlackAI(u8 src,u8 dst,AIMOVEDATA *data){
    printMove(src,dst);
    movePieceD(src,dst);
    printf("%c",'|');
    i8 tpts;
    for(int i = 0;i <= DEPTH;i++){
        u64 mTime = __rdtsc();
        tpts = minBlackAI(i,(EVAL){-75,75,0});
        u64 mTimeE = __rdtsc();
        printf("dpt %i = ",i);
        printf("pts ");
        if(tpts >= 0 && tpts < 10){
            printf("%c",' ');
        }
        if(tpts > -10){
            printf("%c",' ');
        }
        printf("%i|",tpts);
        if(tpts < -50 || tpts > 50){
            data->minDpt = i8max(data->minDpt,i);
            goto n;
        }
        if(mTimeE-mTime>MTIME){
            break;
        }
    }
    data->minDpt = i8max(data->minDpt,DEPTH);
n:
    printf("%c",'\n');
    movePieceD(dst,src);
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

void moveWhiteAI(u8 src,u8 dst,AIMOVEDATA *data){
    printMove(src,dst);
    movePieceD(src,dst);
    printf("%c",'|');
    i8 tpts;
    for(int i = 0;i <= DEPTH;i++){
        u64 mTime = __rdtsc();
        tpts = minWhiteAI(i,(EVAL){ -75,75,0 });
        u64 mTimeE = __rdtsc();
        printf("dpt %i = ",i);
        printf("pts ");
        if(tpts >= 0 && tpts < 10){
            printf("%c",' ');
        }
        if(tpts > -10){
            printf("%c",' ');
        }
        printf("%i|",tpts);
        if(tpts < -50 || tpts > 50){
            data->minDpt = i8max(data->minDpt,i);
            goto n;
        }
        if(mTimeE-mTime>MTIME){
            break;
        }
    }
n:
    data->minDpt = i8max(data->minDpt,DEPTH);
    printf("%c",'\n');
    movePieceD(dst,src);
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

void moveCaptureBlackAI(u8 src,u8 dst,AIMOVEDATA *data){
    if(board[dst] > 'A' && board[dst] < 'Z'){
        captureBlackAI(src,dst,data);
    }
    else if(board[dst] == '_'){
        moveBlackAI(src,dst,data);
    }
}

void moveCaptureWhiteAI(u8 src,u8 dst,AIMOVEDATA *data){
    if(board[dst] > 'a' && board[dst] < 'z'){
        captureWhiteAI(src,dst,data);
    }
    else if(board[dst] == '_'){
        moveWhiteAI(src,dst,data);
    }
}

void straightBlackAI(u8 i,AIMOVEDATA *data){
    for(int i2 = i + 1;(board[i2] < 'a' || board[i2] > 'z') && (i2 & 7) != 0;i2++){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            captureBlackAI(i,i2,data);
            break;
        }
        else{
            moveBlackAI(i,i2,data);
        }
    }
    for(int i2 = i - 1;(board[i2] < 'a' || board[i2] > 'z') && (i2 & 7) != 7;i2--){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            captureBlackAI(i,i2,data);
            break;
        }
        else{
            moveBlackAI(i,i2,data);
        }
    }
    for(int i2 = i - 8;(board[i2] < 'a' || board[i2] > 'z') && i2 >= 0;i2-=8){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            captureBlackAI(i,i2,data);
            break;
        }
        else{
            moveBlackAI(i,i2,data);
        }
    }
    for(int i2 = i + 8;(board[i2] < 'a' || board[i2] > 'z') && i2 < 64;i2+=8){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            captureBlackAI(i,i2,data);
            break;
        }
        else{
            moveBlackAI(i,i2,data);
        }
    }
}

void straightWhiteAI(u8 i,AIMOVEDATA *data){
    for(int i2 = i + 1;(board[i2] < 'A' || board[i2] > 'Z') && (i2 & 7) != 0;i2++){
        if(board[i2] > 'a' && board[i2] < 'z'){
            captureWhiteAI(i,i2,data);
            break;
        }
        else{
            moveWhiteAI(i,i2,data);
        }
    }
    for(int i2 = i - 1;(board[i2] < 'A' || board[i2] > 'Z') && (i2 & 7) != 7;i2--){
        if(board[i2] > 'a' && board[i2] < 'z'){
            captureWhiteAI(i,i2,data);
            break;
        }
        else{
            moveWhiteAI(i,i2,data);
        }
    }
    for(int i2 = i - 8;(board[i2] < 'A' || board[i2] > 'Z') && i2 >= 0;i2-=8){
        if(board[i2] > 'a' && board[i2] < 'z'){
            captureWhiteAI(i,i2,data);
            break;
        }
        else{
            moveWhiteAI(i,i2,data);
        }
    }
    for(int i2 = i + 8;(board[i2] < 'A' || board[i2] > 'Z') && i2 < 64;i2+=8){
        if(board[i2] > 'a' && board[i2] < 'z'){
            captureWhiteAI(i,i2,data);
            break;
        }
        else{
            moveWhiteAI(i,i2,data);
        }
    }
}

void diagonalBlackAI(u8 i,AIMOVEDATA *data){
    for(int i2 = i - 9;(board[i2] < 'a' || board[i2] > 'z') && i2 >= 0 && (i2 & 7) != 7;i2-=9){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            captureBlackAI(i,i2,data);
            break;
        }
        else{
            moveBlackAI(i,i2,data);
        }
    }
    for(int i2 = i - 7;(board[i2] < 'a' || board[i2] > 'z') && i2 >= 0 && (i2 & 7) != 0;i2-=7){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            captureBlackAI(i,i2,data);
            break;
        }
        else{
            moveBlackAI(i,i2,data);
        }
    }
    for(int i2 = i + 9;(board[i2] < 'a' || board[i2] > 'z') && i2 < 64 && (i2 & 7) != 0;i2+=9){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            captureBlackAI(i,i2,data);
            break;
        }
        else{
            moveBlackAI(i,i2,data);
        }
    }
    for(int i2 = i + 7;(board[i2] < 'a' || board[i2] > 'z') && i2 < 64 && (i2 & 7) != 7;i2+=7){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            captureBlackAI(i,i2,data);
            break;
        }
        else{
            moveBlackAI(i,i2,data);
        }
    }
}

void diagonalWhiteAI(u8 i,AIMOVEDATA *data){
    for(int i2 = i - 9;(board[i2] < 'A' || board[i2] > 'Z') && i2 >= 0 && (i2 & 7) != 7;i2-=9){
        if(board[i2] > 'a' && board[i2] < 'z'){
            captureWhiteAI(i,i2,data);
            break;
        }
        else{
            moveWhiteAI(i,i2,data);
        }
    }
    for(int i2 = i - 7;(board[i2] < 'A' || board[i2] > 'Z') && i2 >= 0 && (i2 & 7) != 0;i2-=7){
        if(board[i2] > 'a' && board[i2] < 'z'){
            captureWhiteAI(i,i2,data);
            break;
        }
        else{
            moveWhiteAI(i,i2,data);
        }
    }
    for(int i2 = i + 9;(board[i2] < 'A' || board[i2] > 'Z') && i2 < 64 && (i2 & 7) != 0;i2+=9){
        if(board[i2] > 'a' && board[i2] < 'z'){
            captureWhiteAI(i,i2,data);
            break;
        }
        else{
            moveWhiteAI(i,i2,data);
        }
    }
    for(int i2 = i + 7;(board[i2] < 'A' || board[i2] > 'Z') && i2 < 64 && (i2 & 7) != 7;i2+=7){
        if(board[i2] > 'a' && board[i2] < 'z'){
            captureWhiteAI(i,i2,data);
            break;
        }
        else{
            moveWhiteAI(i,i2,data);
        }
    }
}

u8 checkDiagonal(int SX,int SY,int DX,int DY){
    if(SX-DX==SY-DY){
        if(SX<DX){
            for(int i = 1;i < (DX-SX);i++){
                if(board[(SX+i)*8+(SY+i)] != '_'){
                    return 0;
                }
            }
        }
        else{
            for(int i = 1;i < (SX-DX);i++){
                if(board[(DX+i)*8+(DY+i)] != '_'){
                    return 0;
                }
            }
        }
        if(board[DX*8+DY] == '_'){
            movePiece(SX,SY,DX,DY);
        }
        else{
            capturePiece(SX,SY,DX,DY);
        }
        return 1;
    }
    else{
        if(SX<DX){
            for(int i = 1;i < (DX-SX);i++){
                if(board[(SX+i)*8+(SY-i)] != '_'){
                    return 0;
                }
            }
        }
        else{
            for(int i = 1;i < (SX-DX);i++){
                if(board[(DX+i)*8+(DY-i)] != '_'){
                    return 0;
                }
            }
        }
        if(board[DX*8+DY] == '_'){
            movePiece(SX,SY,DX,DY);
        }
        else{
            capturePiece(SX,SY,DX,DY);
        }
        return 1;
    }
    return 0;
}

u8 checkStraight(int SX,int SY,int DX,int DY){
    if(SX == DX){
        if(SY < DY){
            for(int i = SY+1;i < DY;i++){
                if(board[SX*8+i] != '_'){
                    return 0;
                }
            }
        }
        else{
            for(int i = DY+1;i < SY;i++){
                if(board[SX*8+i] != '_'){
                    return 0;
                }
            }
        }
        if(board[DX*8+DY] == '_'){
            movePiece(SX,SY,DX,DY);
        }
        else{
            capturePiece(SX,SY,DX,DY);
        }
        return 1;
    }
    else if(SY == DY){
        if(SX < DX){
            for(int i = SX+1;i < DX;i++){
                if(board[i*8+SY] != '_'){
                    return 0;
                }
            }
        }
        else{
            for(int i = DX+1;i < SX;i++){
                if(board[i*8+SY] != '_'){
                    return 0;
                }
            }
        }
        if(board[DX*8+DY] == '_'){
            movePiece(SX,SY,DX,DY);
        }
        else{
            capturePiece(SX,SY,DX,DY);
        }
        return 1;
    }
    return 0;
}

AIMOVEDATA whiteAI(){
    u64 mTime = __rdtsc();
    AIMOVEDATA data = {.movC = 0,.pts = -128,.minDpt = 0};
    for(int i = 0;i < 64;i++){
        switch(board[i]){
        case 'P':
            if(board[i-8] == '_'){
                moveWhiteAI(i,i-8,&data);
                if(i > 47){
                    if(board[i-16] == '_'){
                        moveWhiteAI(i,i-16,&data);
                    }
                }
            }
            if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                captureWhiteAI(i,i-9,&data);
            }
            if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                captureWhiteAI(i,i-7,&data);
            }
            break;
        case 'N':{
            u8 sx = i >> 3;
            u8 sy = i & 7;
            if(sx > 0){
                if(sy < 6){
                    moveCaptureWhiteAI(i,i-6,&data);
                }
                if(sy > 1){
                    moveCaptureWhiteAI(i,i-10,&data);
                }
                if(sx > 1){
                    if(sy < 7){
                        moveCaptureWhiteAI(i,i-15,&data);
                    }
                    if(sy > 0){
                        moveCaptureWhiteAI(i,i-17,&data);
                    }
                }
            }
            if(sx < 7){
                if(sy > 1){
                    moveCaptureWhiteAI(i,i+6,&data);
                }
                if(sy < 6){
                    moveCaptureWhiteAI(i,i+10,&data);
                }
                if(sx < 6){
                    if(sy > 0){
                        moveCaptureWhiteAI(i,i+15,&data);
                    }
                    if(sy < 7){
                        moveCaptureWhiteAI(i,i+17,&data);
                    }
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
            int i2 = -1,lm = 2;
            if((i & 7) == 0){
                i2++;
            }
            if((i & 7) == 7){
                lm--;
            }
            for(;i2 < lm;i2++){
                for(int i3 = -8;i3 < 9;i3+=8){
                    if(i2+i3!=0&&i+i2+i3>0&&i+i2+i3<64){
                        moveCaptureWhiteAI(i,i+i2+i3,&data);
                    }
                }
            }
            if(castleRights & 1 && board[5] == '_' && board[6] == '_'){
                movePieceD(63,61);
                moveBlackAI(60,62,&data);
                movePieceD(61,63);
            }
            if(castleRights & 2 && board[3] == '_' && board[2] == '_' && board[1] == '_'){
                movePieceD(56,59);
                moveBlackAI(60,58,&data);
                movePieceD(59,56);
            }
            break;
        }
    }
    printf("total time elapsed = %i\n",__rdtsc()-mTime>>16);
    return data;
}

AIMOVEDATA blackAI(){
    u64 mTime = __rdtsc();
    AIMOVEDATA data ={.movC = 0,.pts = -128,.minDpt = 0};
    for(int i = 0;i < 64;i++){
        switch(board[i]){
        case 'p':
            if(board[i+8] == '_'){
                moveBlackAI(i,i+8,&data);
                if(i < 16){
                    if(board[i+16] == '_'){
                        moveBlackAI(i,i+16,&data);
                    }
                }
            }
            if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
                captureBlackAI(i,i+9,&data);
            }
            if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
                captureBlackAI(i,i+7,&data);
            }
            break;
        case 'n':{
            u8 sx = i >> 3;
            u8 sy = i & 7;
            if(sx > 0){
                if(sy < 6){
                    moveCaptureBlackAI(i,i-6,&data);
                }
                if(sy > 1){
                    moveCaptureBlackAI(i,i-10,&data);
                }
                if(sx > 1){
                    if(sy < 7){
                        moveCaptureBlackAI(i,i-15,&data);
                    }
                    if(sy > 0){
                        moveCaptureBlackAI(i,i-17,&data);
                    }
                }
            }
            if(sx < 7){
                if(sy > 1){
                    moveCaptureBlackAI(i,i+6,&data);
                }
                if(sy < 6){
                    moveCaptureBlackAI(i,i+10,&data);
                }
                if(sx < 6){
                    if(sy > 0){
                        moveCaptureBlackAI(i,i+15,&data);
                    }
                    if(sy < 7){
                        moveCaptureBlackAI(i,i+17,&data);
                    }
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
            int i2 = -1,lm = 2;
            if((i & 7) == 0){
                i2++;
            }
            if((i & 7) == 7){
                lm--;
            }
            for(;i2 < lm;i2++){
                for(int i3 = -8;i3 < 9;i3+=8){
                    if(i2+i3!=0&&i+i2+i3>0&&i+i2+i3<64){
                        moveCaptureBlackAI(i,i+i2+i3,&data);
                    }
                }
            }
            if(castleRights & 4 && board[5] == '_' && board[6] == '_'){
                movePieceD(7,5);
                moveBlackAI(4,6,&data);
                movePieceD(5,7);
            }
            if(castleRights & 8 && board[3] == '_' && board[2] == '_' && board[1] == '_'){
                movePieceD(0,3);
                moveBlackAI(4,2,&data);
                movePieceD(3,0);
            }
            break;
        }
    }
    printf("total time elapsed = %i\n",__rdtsc()-mTime>>16);
    return data;
}

void whitePlayer(){
    char inp[5];
input:
    scanf("%s",inp);
    u8 SX = inp[1] - '0';
    u8 SY = inp[0] - 'A';
    u8 DX = inp[4] - '0';
    u8 DY = inp[3] - 'A';
    if(SX*8+SY < 0 || SX*8+SY > 63 || DX*8+DY < 0 || DX*8+DY > 63 || (board[DX*8+DY] > 'A' && board[DX*8+DY] < 'Z')){
        goto input;
    }
    switch(board[SX*8+SY]){
    case '_':
        goto input;
    case 'K': {
        int MNX = abs(SX - DX);
        int MNY = abs(SY - DY);
        if(MNX == 1 || MNY == 1) {
            if(board[DX*8+DY] == '_'){
                movePiece(SX,SY,DX,DY);
            }
            else{
                capturePiece(SX,SY,DX,DY);
            }
            castleRights &= ~0b0011;
        }
        else if(DY == 6 && (castleRights & 0b0001) == 0b0001){
            for(int i = 5;i < 7;i++){
                if(board[DX*8+i] != '_'){
                    goto input;
                }
            }
            movePiece(SX,SY,DX,DY);
            movePiece(7,7,7,5);
        }
        else if(DY == 1 && (castleRights & 0b0010) == 0b0010){
            for(int i = 3;i > 0;i--){
                if(board[DX*8+i] != '_'){
                     goto input;
                }
            }
            movePiece(SX,SY,DX,DY);
            movePiece(0,0,0,2);
        }
        else{
            goto input;
        }
        break;
    }
    case 'Q':
        if(SX==DX||SY==DY){
            if(!checkStraight(SX,SY,DX,DY)){ goto input; }
        }
        else{
            if(!checkDiagonal(SX,SY,DX,DY)){ goto input; }
        }
        break;
    case 'N': {
        int MNX = abs(SX - DX);
        int MNY = abs(SY - DY);
        if((MNX == 1 && MNY == 2) || (MNX == 2 && MNY == 1)) {
            if(board[DX*8+DY] == '_'){
                movePiece(SX,SY,DX,DY);
            }
            else if(board[DX*8+DY] < 'z' && board[DX*8+DY] > 'a'){
                capturePiece(SX,SY,DX,DY);
            }
        }
        break;
    }
    case 'B':
        if(!checkDiagonal(SX,SY,DX,DY)){ goto input; }
        break;
    case 'R':
        if(DY == 0){
            castleRights &= ~0b0010;
        }
        if(DY == 7){
            castleRights &= ~0b0001;
        }
        if(!checkStraight(SX,SY,DX,DY)){ goto input; }
        break;
    case 'P':
        if(SX == DX + 1) {
            if(SY == DY) {
                if(board[DX*8+DY] == '_') {
                    if(DX == 0){
                        printf("promotion");
                        do{
                            scanf("%s",inp);
                        } while(inp[0]!='Q'&&inp[0]!='R'&&inp[0]!='N'&&inp[0]!='B');
                        board[SX*8+SY] = inp[0];
                    }
                    movePiece(SX,SY,DX,DY);
                }
                else{
                    goto input;
                }
            }
            else if(SY == DY + 1 || SY == DY - 1) {
                if(board[DX*8+DY] >= 'a' && board[DX*8+DY] <= 'z') {
                    if(DX == 0){
                        printf("promotion");
                        do{
                            scanf("%s",inp);
                        } while(inp[0]!='Q'&&inp[0]!='R'&&inp[0]!='N'&&inp[0]!='B');
                        board[SX*8+SY] = inp[0];
                    }
                    capturePiece(SX,SY,DX,DY);
                }
                else{
                    goto input;
                }
            }
        }
        else if(SX == DX + 2){
            if(SX == 6){
                if(board[(DX+1)*8+DY] == '_' && board[DX*8+DY] == '_') {
                    movePiece(SX,SY,DX,DY);
                }
                else{
                    goto input;
                }
            }
            else{
                goto input;
            }
        }
        else{
            goto input;
        }
        break;
    }
}

void blackPlayer(){
    char inp[5];
input:
    scanf("%s",inp);
    u8 SX = inp[1] - '0';
    u8 SY = inp[0] - 'A';
    u8 DX = inp[4] - '0';
    u8 DY = inp[3] - 'A';
    if(SX*8+SY < 0 || SX*8+SY > 63 || DX*8+DY < 0 || DX*8+DY > 63 || (board[DX*8+DY] > 'a' && board[DX*8+DY] < 'z')){
        goto input;
    }
    switch(board[SX*8+SY]){
    case '_':
        goto input;
    case 'k': {
        int MNX = abs(SX - DX);
        int MNY = abs(SY - DY);
        if(MNX == 1 || MNY == 1) {
            if(board[DX*8+DY] == '_'){
                movePiece(SX,SY,DX,DY);
            }
            else{
                capturePiece(SX,SY,DX,DY);
            }
            castleRights &= ~0b1100;
        }
        else if(DY == 6 && (castleRights & 0b0100) == 0b0100){
            for(int i = 5;i < 7;i++){
                if(board[DX*8+i] != '_'){
                    goto input;
                }
            }
            movePiece(SX,SY,DX,DY);
            movePiece(7,7,7,5);
        }
        else if(DY == 1 && (castleRights & 0b1000) == 0b1000){
            for(int i = 3;i > 0;i--){
                if(board[DX*8+i] != '_'){
                     goto input;
                }
            }
            movePiece(SX,SY,DX,DY);
            movePiece(0,0,0,2);
        }
        else{
            goto input;
        }
        break;
    }
    case 'q':
        if(SX==DX||SY==DY){
            if(!checkStraight(SX,SY,DX,DY)){ goto input; }
        }
        else{
            if(!checkDiagonal(SX,SY,DX,DY)){ goto input; }
        }
        break;
    case 'n': {
        int MNX = abs(SX - DX);
        int MNY = abs(SY - DY);
        if((MNX == 1 && MNY == 2) || (MNX == 2 && MNY == 1)) {
            if(board[DX*8+DY] == '_'){
                movePiece(SX,SY,DX,DY);
            }
            else if(board[DX*8+DY] < 'Z' && board[DX*8+DY] > 'A'){
                capturePiece(SX,SY,DX,DY);
            }
        }
        break;
    }
    case 'b':
        if(!checkDiagonal(SX,SY,DX,DY)){ goto input; }
        break;
    case 'r':
        if(DY == 0){
            castleRights &= ~0b0010;
        }
        if(DY == 7){
            castleRights &= ~0b0001;
        }
        if(!checkStraight(SX,SY,DX,DY)){ goto input; }
        break;
    case 'p':
        if(SX == DX - 1) {
            if(SY == DY) {
                if(board[DX*8+DY] == '_') {
                    if(DX == 0){
                        printf("promotion");
                        do{
                            scanf("%s",inp);
                        } while(inp[0]!='q'&&inp[0]!='r'&&inp[0]!='n'&&inp[0]!='b');
                        board[SX*8+SY] = inp[0];
                    }
                    movePiece(SX,SY,DX,DY);
                }
                else{
                    goto input;
                }
            }
            else if(SY == DY + 1 || SY == DY - 1) {
                if(board[DX*8+DY] >= 'A' && board[DX*8+DY] <= 'Z') {
                    if(DX == 0){
                        printf("promotion");
                        do{
                            scanf("%s",inp);
                        } while(inp[0]!='q'&&inp[0]!='r'&&inp[0]!='n'&&inp[0]!='b');
                        board[SX*8+SY] = inp[0];
                    }
                    capturePiece(SX,SY,DX,DY);
                }
                else{
                    goto input;
                }
            }
        }
        else if(SX == DX - 2){
            if(SX == 1){
                if(board[(DX-1)*8+DY] == '_' && board[DX*8+DY] == '_') {
                    movePiece(SX,SY,DX,DY);
                }
                else{
                    goto input;
                }
            }
            else{
                goto input;
            }
        }
        else{
            goto input;
        }
        break;
    }
}

void main(){
    if(board[56] != 'R'){
        castleRights &= ~1;
    }
    if(board[63] != 'R'){
        castleRights &= ~2;
    }
    if(board[0] != 'r'){
        castleRights &= ~4;
    }
    if(board[7] != 'r'){
        castleRights &= ~8;
    }
    master.mov = malloc(sizeof(CHESSMOVE)*64);
    printf("welcome to chessTM\n");
    printf("which gamemode?\n");
    printf("0 = player vs AI\n");
    printf("1 = AI vs AI\n");
    printf("2 = AI vs player\n");
    printf("3 = player vs player\n");
    char gameMode;
    scanf("%i",&gameMode);
    switch(gameMode){
    case 0:
        printBoardWhite();
        for(;;) {
            whitePlayer();
            printBoardWhite();
            AIMOVEDATA data = blackAI();
            if(!data.movC){
                printf("stalemate");
                exit(0);
            }
            if(data.pts == 75){
                printf("AI has won :)");
                exit(0);
            }
            if(data.pts == -75){
                printf("player has won!");
                exit(0);
            }
            u8 mv = __rdtsc()%data.movC;
            u8 mvsrc = master.mov[mv].src;
            u8 mvdst = master.mov[mv].dst;
            movePieceBlack(mvsrc>>3,mvsrc&7,mvdst>>3,mvdst&7);
            switch(board[master.mov[mv].dst]){
            case 'p':
                if(master.mov[mv].dst > 54){
                    board[master.mov[mv].dst] = 'q';
                }
                break;
            case 'k':
                if(master.mov[mv].src == 4){
                    if(master.mov[mv].dst == 6){
                        movePieceD(7,5);
                        castleRights &= ~3;
                    }
                    else if(master.mov[mv].dst == 2){
                        movePieceD(0,3);
                        castleRights &= ~3;
                    }
                }
                break;
            case 'r':
                if(master.mov[mv].src == 7){
                    castleRights &= ~1;
                }
                else if(master.mov[mv].src == 0){
                    castleRights &= ~2;
                }
                break;
            }
            printf("\n%c",(mvsrc&7)+'A');
            printf("%c",(mvsrc>>3)+'0');
            printf("%c",'-');
            printf("%c",(mvdst&7)+'A');
            printf("%c = ",(mvdst>>3)+'0');
            printf("%hhi\n",data.pts);
            printBoardWhite();
        }
        break;
    case 1:
        printBoardWhite();
        for(;;){
            pcsValWhite();
            AIMOVEDATA data = whiteAI();
            if(!data.movC){
                printf("stalemate");
                scanf("");
                exit(0);
            }
            if(data.pts > 50){
                printf("White AI has won!");
                exit(0);
            }
            if(data.pts < -50){
                printf("Black AI has won!");;
                exit(0);
            }
            u8 mv = __rdtsc()%data.movC;
            u8 mvsrc = master.mov[mv].src;
            u8 mvdst = master.mov[mv].dst;
            movePieceWhite(mvsrc>>3,mvsrc&7,mvdst>>3,mvdst&7);
            switch(board[master.mov[mv].dst]){
            case 'P':
                if(master.mov[mv].dst < 8){
                    board[master.mov[mv].dst] = 'Q';
                }
                break;
            case 'K':
                if(master.mov[mv].src == 60){
                    if(master.mov[mv].dst == 62){
                        movePieceD(63,61);
                        castleRights &= ~12;
                    }
                    else if(master.mov[mv].dst == 58){
                        movePieceD(56,59);
                        castleRights &= ~12;
                    }
                }
                break;
            case 'R':
                if(master.mov[mv].src == 63){
                    castleRights &= ~4;
                }
                else if(master.mov[mv].src == 56){
                    castleRights &= ~8;
                }
                break;
            }
            printf("\n%c",(mvsrc&7)+'A');
            printf("%c",(mvsrc>>3)+'0');
            printf("%c",'-');
            printf("%c",(mvdst&7)+'A');
            printf("%c = ",(mvdst>>3)+'0');
            printf("%hhi\n",data.pts);
            printBoardWhite();

            pcsValBlack();
            data = blackAI();
            if(!data.movC){
                printf("stalemate");
                scanf("");
                exit(0);
            }
            if(data.pts > 50){
                printf("Black AI has won!");
                exit(0);
            }
            if(data.pts < -50){
                printf("White AI has won!");;
                exit(0);
            }
            mv = __rdtsc()%data.movC;
            mvsrc = master.mov[mv].src;
            mvdst = master.mov[mv].dst;
            movePieceBlack(mvsrc>>3,mvsrc&7,mvdst>>3,mvdst&7);
            switch(board[master.mov[mv].dst]){
            case 'p':
                if(master.mov[mv].dst > 55){
                    board[master.mov[mv].dst] = 'q';
                }
                break;
            case 'k':
                if(master.mov[mv].src == 4){
                    if(master.mov[mv].dst == 6){
                        movePieceD(7,5);
                        castleRights &= ~3;
                    }
                    else if(master.mov[mv].dst == 2){
                        movePieceD(0,3);
                        castleRights &= ~3;
                    }
                }
                break;
            case 'r':
                if(master.mov[mv].src == 7){
                    castleRights &= ~1;
                }
                else if(master.mov[mv].src == 0){
                    castleRights &= ~2;
                }
                break;
            }
            printf("\n%c",(mvsrc&7)+'A');
            printf("%c",(mvsrc>>3)+'0');
            printf("%c",'-');
            printf("%c",(mvdst&7)+'A');
            printf("%c = ",(mvdst>>3)+'0');
            printf("%hhi\n",data.pts);
            printBoardWhite();
        }
        break;
    case 2:
        pcsValWhite();
        for(;;){
            AIMOVEDATA data = whiteAI();
            if(!data.movC){
                printf("stalemate");
                scanf("");
                exit(0);
            }
            if(!data.minDpt){
                printf("White has won the game!");
                scanf("");
                exit(0);
            }
            u8 mv = __rdtsc()%data.movC;
            u8 mvsrc = master.mov[mv].src;
            u8 mvdst = master.mov[mv].dst;
            movePieceWhite(mvsrc>>3,mvsrc&7,mvdst>>3,mvdst&7);
            switch(board[master.mov[mv].dst]){
            case 'P':
                if(master.mov[mv].dst < 8){
                    board[master.mov[mv].dst] = 'Q';
                }
                break;
            case 'K':
                if(master.mov[mv].src == 60){
                    if(master.mov[mv].dst == 62){
                        movePieceD(63,61);
                        castleRights &= ~12;
                    }
                    else if(master.mov[mv].dst == 58){
                        movePieceD(56,59);
                        castleRights &= ~12;
                    }
                }
                break;
            case 'R':
                if(master.mov[mv].src == 63){
                    castleRights &= ~4;
                }
                else if(master.mov[mv].src == 56){
                    castleRights &= ~8;
                }
                break;
            }
            printf("\n%c",(mvsrc&7)+'A');
            printf("%c",(mvsrc>>3)+'0');
            printf("%c",'-');
            printf("%c",(mvdst&7)+'A');
            printf("%c = ",(mvdst>>3)+'0');
            printf("%hhi\n",data.pts);
            printBoardWhite();
            blackPlayer();
            printBoardWhite();
        }
        break;
    case 3:
        printBoardWhite();
        for(;;){
            whitePlayer();
            printBoardWhite();
            blackPlayer();
            printBoardWhite();
        }
        break;
    }
}
