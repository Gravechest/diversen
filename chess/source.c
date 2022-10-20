#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <intrin.h>

#include "source.h"
#include "maxAI.h"
#include "minAI.h"

CHESSMASTER master;

i8 i8max(i8 p1,i8 p2){
    return p1 > p2 ? p1 : p2;
}

i8 i8min(i8 p1,i8 p2){
    return p1 < p2 ? p1 : p2;
}

char board[64] ={
    '*','*','*','*','k','*','*','*',
    '*','*','*','p','p','p','*','*',
    '*','*','*','*','*','*','*','*',
    '*','*','*','*','*','*','*','*',
    '*','*','*','*','*','*','*','*',
    '*','*','*','*','*','*','*','*',
    '*','*','*','P','P','P','*','*',
    '*','*','*','*','K','*','*','*',
};

char pcsVal[122] ={
    ['p'] = -1,['b'] = -3,['n'] = -3,['r'] = -5,['q'] = -9,['k'] = -75,
    ['P'] =  1,['B'] =  3,['N'] =  3,['R'] =  5,['Q'] =  9,['K'] =  75
};

char turn;
char castleRights = 0b00001111;

i8 minBlackAI(u8 depth,EVAL eval);
i8 minWhiteAI(u8 depth,EVAL eval);

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
    for(int i = 0; i < 8; i++) {
        printf("%c",'A'+i);
        printf("%c",' ');
    }
    printf("\n  ");
    for(int i = 0; i < 16; i++) {
        printf("%c",'_');
    }
    printf("%c",'\n');
    for(int i = 0; i < 8; i++) {
        printf("%i",i);
        printf("| ");
        for(int i2 = 0; i2 < 8; i2++) {
            if(board[i*8+i2] == '*'){
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
}

void movePiece(int SX,int SY,int DX,int DY){
    board[SX*8+SY] ^= board[DX*8+DY];
    board[DX*8+DY] ^= board[SX*8+SY];
    board[SX*8+SY] ^= board[DX*8+DY];
}

void capturePiece(int SX,int SY,int DX,int DY){
    board[DX*8+DY] = board[SX*8+SY];
    board[SX*8+SY] = '*';
}

void movePieceD(u8 src,u8 dst){
    char t = board[src];
    board[src] = board[dst];
    board[dst] = t;
}

void capturePieceD(u8 src,u8 dst){
    board[dst] = board[src];
    board[src] = '*';
}

void movePieceWhite(int SX,int SY,int DX,int DY){
    if(board[DX*8+DY] == '*'){
        movePiece(SX,SY,DX,DY);
    }
    else if(board[DX*8+DY] >= 'a' && board[DX*8+DY] <= 'z'){
        capturePiece(SX,SY,DX,DY);
    }
}

void movePieceBlack(int SX,int SY,int DX,int DY){
    if(board[DX*8+DY] == '*'){
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

void maxBlackAIdiagonalLow0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'a' || board[i2] > 'z';i2-=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *tpts = i8max(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void maxBlackAIdiagonalHigh0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *tpts = i8max(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

i8 maxBlackAIdiagonal0(u8 i,i8 pts,i8 tpts){
    if(i > 7){
        if((i & 7) != 0){
            maxBlackAIdiagonalLow0(i,9,0,pts,&tpts);
        }
        if((i & 7) != 7){
            maxBlackAIdiagonalLow0(i,7,7,pts,&tpts);
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            maxBlackAIdiagonalHigh0(i,9,7,pts,&tpts);
        }
        if((i & 7) != 0){
            maxBlackAIdiagonalHigh0(i,7,0,pts,&tpts);
        }
    }
    return tpts;
}

void maxBlackAIstraighHor0(u8 i,i8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *tpts = i8max(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if((i2 & 7) == stp){
            return;
        }
    }
}

i8 maxBlackAIstraight0(u8 i,i8 pts,i8 tpts){
    if((i & 7) != 7){
        maxBlackAIstraighHor0(i,1,7,pts,&tpts);
    }
    if((i & 7) != 0){
        maxBlackAIstraighHor0(i,-1,0,pts,&tpts);
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                tpts = i8max(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if(i2 < 8){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                tpts = i8max(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return tpts;
}

void maxBlackAIdiagonalLow(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'a' || board[i2] > 'z';i2-=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            return;
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
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void maxBlackAIdiagonalHigh(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            return;
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
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

void maxBlackAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if(i > 7){
        if((i & 7) != 0){
            maxBlackAIdiagonalLow(i,depth,9,0,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
        if((i & 7) != 7){
            maxBlackAIdiagonalLow(i,depth,7,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            maxBlackAIdiagonalHigh(i,depth,9,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
        if((i & 7) != 0){
            maxBlackAIdiagonalHigh(i,depth,7,0,eval,tpts);
        }
    }
}

void maxBlackAIstraighHor(u8 i,u8 depth,i8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            return;
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
        if((i2 & 7) == stp){
            return;
        }
    }
}

void maxBlackAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if((i & 7) != 7){
        maxBlackAIstraighHor(i,depth,1,7,eval,tpts);
        if(eval->ab.beta <= eval->ab.alpha){
            return;
        }
    }
    if((i & 7) != 0){
        maxBlackAIstraighHor(i,depth,-1,0,eval,tpts);
        if(eval->ab.beta <= eval->ab.alpha){
            return;
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
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
            if(i2 < 7){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
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
            if(i2 > 54){
                break;
            }
        }
    }
}

void maxBlackAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] < 'Z' && board[i+dst] > 'A'){
        eval->pts += pcsVal[board[i+dst]];
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
    else if(board[i+dst] == '*'){
        movePieceD(i,i+dst);
        *tpts = i8max(*tpts,minBlackAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
}

i8 maxBlackAI(u8 depth,EVAL eval){
    i8 tpts = -75;
    if(!depth){
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 49){
                    if((i & 7) != 7){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+9]]+8);
                    }
                    if((i & 7) != 0){
                        tpts = i8max(tpts,eval.pts+pcsVal[board[i+7]]+8);
                    }
                    else if(board[i+8] == '*'){
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
                    else if(board[i+8] == '*'){
                        tpts = i8max(tpts,eval.pts);
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
                        if(i2+i3!=0&&i+i2+i3>0&&i+i2+i3<64){
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
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 55){
                    if(board[i+8] == '*'){
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
                    if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
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
                    if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
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
                    if(board[i+8] == '*'){
                        movePieceD(i,i+8);
                        tpts = i8max(tpts,minBlackAI(depth-1,eval));
                        movePieceD(i+8,i);
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i > 7 && i < 16 && board[i+16] == '*'){
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
                        if(i2+i3!=0&&i+i2+i3>0&&i+i2+i3<64){
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
                            else if(board[i+i2+i3] == '*'){
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
                break;
            }
            }
        }
    }
    return tpts;
}

void minBlackAIdiagonalLow0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'A' || board[i2] > 'Z';i2-=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            *tpts = i8min(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void minBlackAIdiagonalHigh0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'A' || board[i2] > 'Z';i2+=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            *tpts = i8min(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

i8 minBlackAIdiagonal0(u8 i,i8 pts,i8 tpts){
    if(i > 7){
        if((i & 7) != 0){
            minBlackAIdiagonalLow0(i,9,0,pts,&tpts);
        }
        if((i & 7) != 7){
            minBlackAIdiagonalLow0(i,7,7,pts,&tpts);
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            minBlackAIdiagonalHigh0(i,9,7,pts,&tpts);
        }
        if((i & 7) != 0){
            minBlackAIdiagonalHigh0(i,7,0,pts,&tpts);
        }
    }
    return tpts;
}

i8 minBlackAIstraight0(u8 i,i8 pts,i8 tpts){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'A' || board[i2] > 'Z';i2++){
            if(board[i2] > 'a' && board[i2] < 'z'){
                tpts = i8min(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if((i2 & 7) == 7){
                break;
            }
        }
    }
    if((i & 7) != 0){
        for(int i2 = i - 1;board[i2] < 'A' || board[i2] > 'Z';i2--){
            if(board[i2] > 'a' && board[i2] < 'z'){
                tpts = i8min(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if((i2 & 7) == 0){
                break;
            }
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'A' || board[i2] > 'Z';i2-=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
                tpts = i8min(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if(i2 < 8){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'A' || board[i2] > 'Z';i2+=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
                tpts = i8min(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return tpts;
}

void minBlackAIdiagonalLow(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'A' || board[i2] > 'Z';i2-=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
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
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void minBlackAIdiagonalHigh(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'A' || board[i2] > 'Z';i2+=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,maxBlackAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,maxBlackAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

void minBlackAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if(i > 7){
        if((i & 7) != 0){
            minBlackAIdiagonalLow(i,depth,9,0,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return eval->pts;
            }
        }
        if((i & 7) != 7){
            minBlackAIdiagonalLow(i,depth,7,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return eval->pts;
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            minBlackAIdiagonalHigh(i,depth,9,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return eval->pts;
            }
        }
        if((i & 7) != 0){
            minBlackAIdiagonalHigh(i,depth,7,0,eval,tpts);
        }
    }
}

void minBlackAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'A' || board[i2] > 'Z';i2++){
            if(board[i2] > 'a' && board[i2] < 'z'){
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
            if((i2 & 7) == 7){
                break;
            }
        }
    }
    if((i & 7) != 0){
        for(int i2 = i - 1;board[i2] < 'A' || board[i2] > 'Z';i2--){
            if(board[i2] > 'a' && board[i2] < 'z'){
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
            if((i2 & 7) == 0){
                break;
            }
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'A' || board[i2] > 'Z';i2-=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
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
            if(i2 < 8){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'A' || board[i2] > 'Z';i2+=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
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
            if(i2 > 54){
                return;
            }
        }
    }
}

void minBlackAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] < 'z' && board[i+dst] > 'a'){
        eval->pts += pcsVal[board[i+dst]];
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
    else if(board[i+dst] == '*'){
        movePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxBlackAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
}

i8 minBlackAI(u8 depth,EVAL eval){
    i8 tpts = 75;
    if(!depth){
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
                    else if(board[i-8] == '*'){
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
                    else if(board[i-8] == '*'){
                        tpts = i8min(tpts,eval.pts);
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
                            if(board[i+i2+i3] < 'z' && board[i+i2+i3] > 'a'){
                                tpts = i8min(tpts,eval.pts+pcsVal[board[i+i2+i3]]);
                            }
                        }
                    }
                }
                break;
            }
            }
        }
    }
    else{
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if(board[i-8] == '*'){
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
                    if(board[i-8] == '*'){
                        movePieceD(i,i-8);
                        tpts = i8min(tpts,maxBlackAI(depth-1,eval));
                        movePieceD(i-8,i);
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i > 47 && i < 56 && board[i-16] == '*'){
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
                            else if(board[i+i2+i3] == '*'){
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
                break;
            }
            }
        }
    }
    return tpts;
}

void maxWhiteAIdiagonalLow0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'A' || board[i2] > 'Z';i2-=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            *tpts = i8max(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void maxWhiteAIdiagonalHigh0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'A' || board[i2] > 'Z';i2+=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            *tpts = i8max(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

i8 maxWhiteAIdiagonal0(u8 i,i8 pts,i8 tpts){
    if(i > 7){
        if((i & 7) != 0){
            maxWhiteAIdiagonalLow0(i,9,0,pts,&tpts);
        }
        if((i & 7) != 7){
            maxWhiteAIdiagonalLow0(i,7,7,pts,&tpts);
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            maxWhiteAIdiagonalHigh0(i,9,7,pts,&tpts);
        }
        if((i & 7) != 0){
            maxWhiteAIdiagonalHigh0(i,7,0,pts,&tpts);
        }
    }
    return tpts;
}

void maxWhiteAIstraighHor0(u8 i,i8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'A' || board[i2] > 'Z';i2+=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            *tpts = i8max(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if((i2 & 7) == stp){
            return;
        }
    }
}

i8 maxWhiteAIstraight0(u8 i,i8 pts,i8 tpts){
    if((i & 7) != 7){
        maxWhiteAIstraighHor0(i,1,7,pts,&tpts);
    }
    if((i & 7) != 0){
        maxWhiteAIstraighHor0(i,-1,0,pts,&tpts);
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'A' || board[i2] > 'Z';i2-=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
                tpts = i8max(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if(i2 < 8){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'A' || board[i2] > 'Z';i2+=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
                tpts = i8max(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return tpts;
}

void maxWhiteAIdiagonalLow(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'A' || board[i2] > 'Z';i2-=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            return;
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
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void maxWhiteAIdiagonalHigh(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            return;
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
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

void maxWhiteAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if(i > 7){
        if((i & 7) != 0){
            maxWhiteAIdiagonalLow(i,depth,9,0,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
        if((i & 7) != 7){
            maxWhiteAIdiagonalLow(i,depth,7,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            maxWhiteAIdiagonalHigh(i,depth,9,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
        if((i & 7) != 0){
            maxWhiteAIdiagonalHigh(i,depth,7,0,eval,tpts);
        }
    }
}

void maxWhiteAIstraighHor(u8 i,u8 depth,i8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'A' || board[i2] > 'Z';i2+=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            return;
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
        if((i2 & 7) == stp){
            return;
        }
    }
}

void maxWhiteAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if((i & 7) != 7){
        maxWhiteAIstraighHor(i,depth,1,7,eval,tpts);
        if(eval->ab.beta <= eval->ab.alpha){
            return;
        }
    }
    if((i & 7) != 0){
        maxWhiteAIstraighHor(i,depth,-1,0,eval,tpts);
        if(eval->ab.beta <= eval->ab.alpha){
            return;
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'A' || board[i2] > 'Z';i2-=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
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
            if(i2 < 7){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'A' || board[i2] > 'Z';i2+=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
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
            if(i2 > 54){
                break;
            }
        }
    }
}

void maxWhiteAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] < 'z' && board[i+dst] > 'a'){
        eval->pts += pcsVal[board[i+dst]];
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
    else if(board[i+dst] == '*'){
        movePieceD(i,i+dst);
        *tpts = i8max(*tpts,minWhiteAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
}

i8 maxWhiteAI(u8 depth,EVAL eval){
    i8 tpts = -75;
    if(!depth){
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
                    else if(board[i-8] == '*'){
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
                    else if(board[i-8] == '*'){
                        tpts = i8max(tpts,eval.pts);
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
                        if(i2+i3!=0&&i+i2+i3>0&&i+i2+i3<64){
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
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16){
                    if(board[i-8] == '*'){
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
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
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
                    if(board[i+7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
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
                    if(board[i-8] == '*'){
                        movePieceD(i,i-8);
                        tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                        movePieceD(i-8,i);
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i > 47 && board[i-16] == '*'){
                            movePieceD(i,i-16);
                            tpts = i8max(tpts,minWhiteAI(depth-1,eval));
                            movePieceD(i-16,i);
                            eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
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
                    if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
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
                        if(i2+i3!=0&&i+i2+i3>0&&i+i2+i3<64){
                            if(board[i+i2+i3] < 'z' && board[i+i2+i3] > 'a'){
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
                            else if(board[i+i2+i3] == '*'){
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
                break;
            }
            }
        }
    }
    return tpts;
}

void minWhiteAIdiagonalLow0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'a' || board[i2] > 'z';i2-=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *tpts = i8min(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void minWhiteAIdiagonalHigh0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *tpts = i8min(*tpts,pts+pcsVal[board[i2]]);
            return;
        }
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

i8 minWhiteAIdiagonal0(u8 i,i8 pts,i8 tpts){
    if(i > 7){
        if((i & 7) != 0){
            minWhiteAIdiagonalLow0(i,9,0,pts,&tpts);
        }
        if((i & 7) != 7){
            minWhiteAIdiagonalLow0(i,7,7,pts,&tpts);
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            minWhiteAIdiagonalHigh0(i,9,7,pts,&tpts);
        }
        if((i & 7) != 0){
            minWhiteAIdiagonalHigh0(i,7,0,pts,&tpts);
        }
    }
    return tpts;
}

i8 minWhiteAIstraight0(u8 i,i8 pts,i8 tpts){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'a' || board[i2] > 'z';i2++){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                tpts = i8min(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if((i2 & 7) == 7){
                break;
            }
        }
    }
    if((i & 7) != 0){
        for(int i2 = i - 1;board[i2] < 'a' || board[i2] > 'z';i2--){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                tpts = i8min(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if((i2 & 7) == 0){
                break;
            }
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                tpts = i8min(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if(i2 < 8){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                tpts = i8min(tpts,pts+pcsVal[board[i2]]);
                break;
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return tpts;
}

void minWhiteAIdiagonalLow(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'a' || board[i2] > 'z';i2-=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
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
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void minWhiteAIdiagonalHigh(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += pcsVal[board[i2]];
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,maxWhiteAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= pcsVal[board[i2]];
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,maxWhiteAI(depth-1,*eval));
            movePieceD(i2,i);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

void minWhiteAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if(i > 7){
        if((i & 7) != 0){
            minWhiteAIdiagonalLow(i,depth,9,0,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return eval->pts;
            }
        }
        if((i & 7) != 7){
            minWhiteAIdiagonalLow(i,depth,7,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return eval->pts;
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            minWhiteAIdiagonalHigh(i,depth,9,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return eval->pts;
            }
        }
        if((i & 7) != 0){
            minWhiteAIdiagonalHigh(i,depth,7,0,eval,tpts);
        }
    }
}

void minWhiteAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'a' || board[i2] > 'z';i2++){
            if(board[i2] > 'A' && board[i2] < 'Z'){
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
            if((i2 & 7) == 7){
                break;
            }
        }
    }
    if((i & 7) != 0){
        for(int i2 = i - 1;board[i2] < 'A' || board[i2] > 'Z';i2--){
            if(board[i2] > 'a' && board[i2] < 'z'){
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
            if((i2 & 7) == 0){
                break;
            }
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'A' || board[i2] > 'Z';i2-=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
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
            if(i2 < 8){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'A' || board[i2] > 'Z';i2+=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
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
            if(i2 > 54){
                return;
            }
        }
    }
}

void minWhiteAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] < 'Z' && board[i+dst] > 'A'){
        eval->pts += pcsVal[board[i+dst]];
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= pcsVal[board[i+dst]];
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
    else if(board[i+dst] == '*'){
        movePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxWhiteAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
}

i8 minWhiteAI(u8 depth,EVAL eval){
    i8 tpts = 75;
    if(!depth){
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 47){
                    if((i & 7) != 0){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-9]]-8);
                    }
                    if((i & 7) != 7){
                        tpts = i8min(tpts,eval.pts+pcsVal[board[i-7]]-8);
                    }
                    else if(board[i-8] == '*'){
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
                    else if(board[i-8] == '*'){
                        tpts = i8min(tpts,eval.pts);
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
                            if(board[i+i2+i3] < 'Z' && board[i+i2+i3] > 'A'){
                                tpts = i8min(tpts,eval.pts+pcsVal[board[i+i2+i3]]);
                            }
                        }
                    }
                }
                break;
            }
            }
        }
    }
    else{
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 53){
                    if(board[i+8] == '*'){
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
                    if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
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
                    if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
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
                    if(board[i+8] == '*'){
                        movePieceD(i,i+8);
                        tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                        movePieceD(i+8,i);
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i < 16 && board[i+16] == '*'){
                            movePieceD(i,i+16);
                            tpts = i8min(tpts,maxWhiteAI(depth-1,eval));
                            movePieceD(i+16,i);
                            eval.ab.beta = i8min(eval.ab.beta,tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                    if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
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
                    if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
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
            case 'b':
                minBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'r':
                minBlackAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'q':
                minBlackAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                minBlackAIstraight(i,depth,&eval,&tpts);
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
                            if(board[i+i2+i3] < 'Z' && board[i+i2+i3] > 'A'){
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
                            else if(board[i+i2+i3] == '*'){
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
        tpts = minBlackAI(i,(EVAL){ -75,75,0 });
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
    else if(board[dst] == '*'){
        moveBlackAI(src,dst,data);
    }
}

void moveCaptureWhiteAI(u8 src,u8 dst,AIMOVEDATA *data){
    if(board[dst] > 'a' && board[dst] < 'z'){
        captureWhiteAI(src,dst,data);
    }
    else if(board[dst] == '*'){
        moveWhiteAI(src,dst,data);
    }
}

void straightBlackAI(u8 i,AIMOVEDATA *data){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'a' || board[i2] > 'z';i2++){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureBlackAI(i,i2,data);
                break;
            }
            else{
                moveBlackAI(i,i2,data);
            }
            if((i2 & 7) == 7){
                break;
            }
        }
    }
    if((i & 7) != 0){
        for(int i2 = i - 1;board[i2] < 'a' || board[i2] > 'z';i2--){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureBlackAI(i,i2,data);
                break;
            }
            else{
                moveBlackAI(i,i2,data);
            }
            if((i2 & 7) == 0){
                break;
            }
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureBlackAI(i,i2,data);
                break;
            }
            else{
                moveBlackAI(i,i2,data);
            }
            if(i2 < 7){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureBlackAI(i,i2,data);
                break;
            }
            else{
                moveBlackAI(i,i2,data);
            }
            if(i2 > 55){
                break;
            }
        }
    }
}

void straightWhiteAI(u8 i,AIMOVEDATA *data){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'A' || board[i2] > 'Z';i2++){
            if(board[i2] > 'a' && board[i2] < 'z'){
                captureWhiteAI(i,i2,data);
                break;
            }
            else{
                moveWhiteAI(i,i2,data);
            }
            if((i2 & 7) == 7){
                break;
            }
        }
    }
    if((i & 7) != 0){
        for(int i2 = i - 1;board[i2] < 'A' || board[i2] > 'Z';i2--){
            if(board[i2] > 'a' && board[i2] < 'z'){
                captureWhiteAI(i,i2,data);
                break;
            }
            else{
                moveWhiteAI(i,i2,data);
            }
            if((i2 & 7) == 0){
                break;
            }
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'A' || board[i2] > 'Z';i2-=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
                captureWhiteAI(i,i2,data);
                break;
            }
            else{
                moveWhiteAI(i,i2,data);
            }
            if(i2 < 7){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'A' || board[i2] > 'Z';i2+=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
                captureWhiteAI(i,i2,data);
                break;
            }
            else{
                moveWhiteAI(i,i2,data);
            }
            if(i2 > 55){
                break;
            }
        }
    }
}

void diagonalBlackAI(u8 i,AIMOVEDATA *data){
    if(i > 7){
        if((i & 7) != 0){
            for(int i2 = i - 9;board[i2] < 'a' || board[i2] > 'z';i2-=9){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureBlackAI(i,i2,data);
                    break;
                }
                else{
                    moveBlackAI(i,i2,data);
                }
                if(i2 < 8 || (i2 & 7) == 0){
                    break;
                }
            }
        }
        if((i & 7) != 7){
            for(int i2 = i - 7;board[i2] < 'a' || board[i2] > 'z';i2-=7){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureBlackAI(i,i2,data);
                    break;
                }
                else{
                    moveBlackAI(i,i2,data);
                }
                if(i2 < 8 || (i2 & 7) == 7){
                    break;
                }
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            for(int i2 = i + 9;board[i2] < 'a' || board[i2] > 'z';i2+=9){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureBlackAI(i,i2,data);
                    break;
                }
                else{
                    moveBlackAI(i,i2,data);
                }
                if(i2 > 55 || (i2 & 7) == 7){
                    break;
                }
            }
        }
        if((i & 7) != 0){
            for(int i2 = i + 7;board[i2] < 'a' || board[i2] > 'z';i2+=7){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureBlackAI(i,i2,data);
                    break;
                }
                else{
                    moveBlackAI(i,i2,data);
                }
                if(i2 > 55  || (i2 & 7) == 0){
                    break;
                }
            }
        }
    }
}

void diagonalWhiteAI(u8 i,AIMOVEDATA *data){
    if(i > 7){
        if((i & 7) != 0){
            for(int i2 = i - 9;board[i2] < 'A' || board[i2] > 'Z';i2-=9){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    captureWhiteAI(i,i2,data);
                    break;
                }
                else{
                    moveWhiteAI(i,i2,data);
                }
                if(i2 < 8 || (i2 & 7) == 0){
                    break;
                }
            }
        }
        if((i & 7) != 7){
            for(int i2 = i - 7;board[i2] < 'A' || board[i2] > 'Z';i2-=7){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    captureWhiteAI(i,i2,data);
                    break;
                }
                else{
                    moveWhiteAI(i,i2,data);
                }
                if(i2 < 8 || (i2 & 7) == 7){
                    break;
                }
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            for(int i2 = i + 9;board[i2] < 'A' || board[i2] > 'Z';i2+=9){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    captureWhiteAI(i,i2,data);
                    break;
                }
                else{
                    moveWhiteAI(i,i2,data);
                }
                if(i2 > 55 || (i2 & 7) == 7){
                    break;
                }
            }
        }
        if((i & 7) != 0){
            for(int i2 = i + 7;board[i2] < 'A' || board[i2] > 'Z';i2+=7){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    captureWhiteAI(i,i2,data);
                    break;
                }
                else{
                    moveWhiteAI(i,i2,data);
                }
                if(i2 > 55  || (i2 & 7) == 0){
                    break;
                }
            }
        }
    }
}

u8 checkDiagonal(int SX,int SY,int DX,int DY){
    if(SX-DX==SY-DY){
        if(SX<DX){
            for(int i = 1;i < (DX-SX)+1;i++){
                if(board[(SX+i)*8+(SY+i)] != '*'){
                    return 0;
                }
            }
        }
        else{
            for(int i = 0;i < (SX-DX);i++){
                if(board[(DX+i)*8+(DY+i)] != '*'){
                    return 0;
                }
            }
        }
        movePiece(SX,SY,DX,DY);
        return 1;
    }
    else if(SX-DX==DY-SY){
        if(SX<DX){
            for(int i = 1;i < (DX-SX)+1;i++){
                if(board[(SX+i)*8+(SY-i)] != '*'){
                    return 0;
                }
            }
        }
        else{
            for(int i = 0;i < (SX-DX);i++){
                if(board[(DX+i)*8+(DY-i)] != '*'){
                    return 0;
                }
            }
        }
        movePiece(SX,SY,DX,DY);
        return 1;
    }
    return 0;
}

u8 checkStraight(int SX,int SY,int DX,int DY){
    if(SX == DX){
        if(SY < DY){
            for(int i = SY+1;i < DY;i++){
                if(board[SX*8+i] != '*'){
                    return 0;
                }
            }
        }
        else{
            for(int i = DY+1;i < SY;i++){
                if(board[SX*8+i] != '*'){
                    return 0;
                }
            }
        }
        movePiece(SX,SY,DX,DY);
        return 1;
    }
    else if(SY == DY){
        if(SX < DX){
            for(int i = SX+1;i < DX;i++){
                if(board[i*8+SY] != '*'){
                    return 0;
                }
            }
        }
        else{
            for(int i = DX+1;i < SX;i++){
                if(board[i*8+SY] != '*'){
                    return 0;
                }
            }
        }
        movePiece(SX,SY,DX,DY);
        return 1;
    }
    return 0;
}

AIMOVEDATA whiteAI(){
    AIMOVEDATA data = {.movC = 0,.pts = -128,.minDpt = 0};
    for(int i = 0;i < 64;i++){
        switch(board[i]){
        case 'P':
            if(board[i-8] == '*'){
                moveWhiteAI(i,i-8,&data);
                if(i > 47){
                    if(board[i-16] == '*'){
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
            break;
        }
    }
    return data;
}

AIMOVEDATA blackAI(){
    AIMOVEDATA data ={.movC = 0,.pts = -128,.minDpt = 0};
    for(int i = 0;i < 64;i++){
        switch(board[i]){
        case 'p':
            if(board[i+8] == '*'){
                moveBlackAI(i,i+8,&data);
                if(i < 16){
                    if(board[i+16] == '*'){
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
            break;
        }
    }
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
    if(SX*8+SY < 0 || SX*8+SY > 63 || DX*8+DY < 0 || DX*8+DY > 63){
        goto input;
    }
    switch(board[SX*8+SY]){
    case '*':
        goto input;
    case 'K': {
        int MNX = abs(SX - DX);
        int MNY = abs(SY - DY);
        if(MNX == 1 || MNY == 1) {
            if(board[DX*8+DY] == '*'){
                movePiece(SX,SY,DX,DY);
            }
            else{
                capturePiece(SX,SY,DX,DY);
            }
            castleRights &= ~0b0011;
        }
        else if(DY == 6 && (castleRights & 0b0001) == 0b0001){
            for(int i = 5;i < 7;i++){
                if(board[DX*8+i] != '*'){
                    goto input;
                }
            }
            movePiece(SX,SY,DX,DY);
            movePiece(7,7,7,5);
        }
        else if(DY == 1 && (castleRights & 0b0010) == 0b0010){
            for(int i = 3;i > 0;i--){
                if(board[DX*8+i] != '*'){
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
            if(board[DX*8+DY] == '*'){
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
                if(board[DX*8+DY] == '*') {
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
                if(board[(DX+1)*8+DY] == '*' && board[DX*8+DY] == '*') {
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
    master.mov = malloc(sizeof(CHESSMOVE)*64);
    printf("welcome to chessTM\n");
    printBoard();
    printf("which gamemode?\n");
    printf("0 = player vs AI\n");
    printf("1 = AI vs AI\n");
    char gameMode;
    scanf("%i",&gameMode);
    switch(gameMode){
    case 0:
        for(;;) {
            whitePlayer();
            printBoard();
            u64 mTime = __rdtsc();
            AIMOVEDATA data = blackAI();
            printf("total time elapsed = %i\n",__rdtsc()-mTime>>16);
            if(!data.movC){
                printf("stalemate");
                exit(0);
            }
            if(data.pts == 75){
                printf("AI has won :)");
                exit(0);
            }
            else if(data.pts == -75){
                printf("player has won!");
                exit(0);
            }
            u8 mv = __rdtsc()%data.movC;
            u8 mvsrc = master.mov[mv].src;
            u8 mvdst = master.mov[mv].dst;
            movePieceBlack(mvsrc>>3,mvsrc&7,mvdst>>3,mvdst&7);
            if(master.mov[mv].dst > 54 && board[master.mov[mv].dst] == 'p'){
                board[master.mov[mv].dst] = 'q';
            }
            printf("\n%c",(mvsrc&7)+'A');
            printf("%c",(mvsrc>>3)+'0');
            printf("%c",'-');
            printf("%c",(mvdst&7)+'A');
            printf("%c = ",(mvdst>>3)+'0');
            printf("%hhi\n",data.pts);
            printBoard();
        }
        break;
    case 1:
        for(;;) {
            pcsValWhite();
            u64 mTime = __rdtsc();
            AIMOVEDATA data = whiteAI();
            printf("total time elapsed = %i\n",__rdtsc()-mTime>>16);
            if(!data.movC){
                printf("stalemate");
                exit(0);
            }
            if(!data.minDpt){
                printf("White has won the game!");
                exit(0);
            }
            u8 mv = __rdtsc()%data.movC;
            u8 mvsrc = master.mov[mv].src;
            u8 mvdst = master.mov[mv].dst;
            movePieceBlack(mvsrc>>3,mvsrc&7,mvdst>>3,mvdst&7);
            if(master.mov[mv].dst > 54 && board[master.mov[mv].dst] == 'p'){
                board[master.mov[mv].dst] = 'q';
            }
            printf("\n%c",(mvsrc&7)+'A');
            printf("%c",(mvsrc>>3)+'0');
            printf("%c",'-');
            printf("%c",(mvdst&7)+'A');
            printf("%c = ",(mvdst>>3)+'0');
            printf("%hhi\n",data.pts);
            printBoard();

            pcsValBlack();

            mTime = __rdtsc();
            data = blackAI();
            printf("total time elapsed = %i\n",__rdtsc()-mTime>>16);
            if(!data.movC){
                printf("stalemate");
                exit(0);
            }
            if(!data.minDpt){
                printf("Black has won the game!");
                exit(0);
            }
            mv = __rdtsc()%data.movC;
            mvsrc = master.mov[mv].src;
            mvdst = master.mov[mv].dst;
            movePieceBlack(mvsrc>>3,mvsrc&7,mvdst>>3,mvdst&7);
            if(master.mov[mv].dst > 54 && board[master.mov[mv].dst] == 'p'){
                board[master.mov[mv].dst] = 'q';
            }
            printf("\n%c",(mvsrc&7)+'A');
            printf("%c",(mvsrc>>3)+'0');
            printf("%c",'-');
            printf("%c",(mvdst&7)+'A');
            printf("%c = ",(mvdst>>3)+'0');
            printf("%hhi\n",data.pts);
            printBoard();
        }
        break;
    }
}
