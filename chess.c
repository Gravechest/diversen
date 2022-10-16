#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <intrin.h>

#define DEPTH 6

#pragma warning(disable:4996)

typedef char i8;
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct{
    u8 src;
    u8 dst;
}CHESSMOVE;

typedef struct{
    i8 alpha;
    i8 beta;
}ALPHABETA;

typedef struct{
    ALPHABETA ab;
    i8 pts;
}EVAL;

typedef struct{
    CHESSMOVE *mov;
}CHESSMASTER;

CHESSMASTER master;

inline i8 i8max(i8 p1,i8 p2){
    return p1 > p2 ? p1 : p2;
}

inline i8 i8min(i8 p1,i8 p2){
    return p1 < p2 ? p1 : p2;
}

char board[64] ={
    'r','n','b','q','k','b','n','r',
    'p','p','p','p','p','p','p','p',
    '*','*','*','*','*','*','*','*',
    '*','*','*','*','*','*','*','*',
    '*','*','*','*','*','*','*','*',
    '*','*','*','*','*','*','*','*',
    'P','P','P','P','P','P','P','P',
    'R','N','B','Q','K','B','N','R',
};

char turn;  
char castleRights = 0b00001111;

i8 minAI(u8 depth,EVAL eval);
i8 maxAI(u8 depth,EVAL eval);

void printBoard(){
    printf("   ");
    for (int i = 0; i < 8; i++) {
        printf("%c",'A'+i);
        printf("%c",' ');
    }
    printf("\n  ");
    for (int i = 0; i < 16; i++) {
        printf("%c",'_');
    }
    printf("%c",'\n');
    for (int i = 0; i < 8; i++) {
        printf("%i",i);
        printf("| ");
        for (int i2 = 0; i2 < 8; i2++) {
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

i8 blackPcsToScore(u8 piece){
    switch(piece){
    case 'p':
        return -1;
    case 'b':
    case 'n':
        return -3;
    case 'r':
        return -5;
    case 'q':
        return -9;
    case 'k':
        return -75;
    default:
        return 0;
    }
}

i8 whitePcsToScore(u8 piece){
    switch(piece){
    case 'P':
        return 1;
    case 'B':
    case 'N':
        return 3;
    case 'R':
        return 5;
    case 'Q':
        return 9;
    case 'K':
        return 75;
    default:
        return 0;
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
    board[src] ^= board[dst];
    board[dst] ^= board[src];
    board[src] ^= board[dst];
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

void maxAIdiagonalLow0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'a' || board[i2] > 'z';i2-=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *tpts = i8max(*tpts,pts+whitePcsToScore(board[i2]));
            return;
        }
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void minAIdiagonalLow0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'A' || board[i2] > 'Z';i2-=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            *tpts = i8min(*tpts,pts+blackPcsToScore(board[i2]));
            return;
        }
        if(i2 < 8 || (i2 & 7) == stp){
            return;
        }
    }
}

void maxAIdiagonalHigh0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *tpts = i8max(*tpts,pts+whitePcsToScore(board[i2]));
            return;
        }
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

void minAIdiagonalHigh0(u8 i,u8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'A' || board[i2] > 'Z';i2+=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            *tpts = i8min(*tpts,pts+blackPcsToScore(board[i2]));
            return;
        }
        if(i2 > 54 || (i2 & 7) == stp){
            return;
        }
    }
}

i8 maxAIdiagonal0(u8 i,i8 pts,i8 tpts){
    if(i > 7){
        if((i & 7) != 0){
            maxAIdiagonalLow0(i,9,0,pts,&tpts);
        }
        if((i & 7) != 7){
            maxAIdiagonalLow0(i,7,7,pts,&tpts);
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            maxAIdiagonalHigh0(i,9,7,pts,&tpts);
        }
        if((i & 7) != 0){
            maxAIdiagonalHigh0(i,7,0,pts,&tpts);
        }
    }
    return tpts;
}

i8 minAIdiagonal0(u8 i,i8 pts,i8 tpts){
    if(i > 7){
        if((i & 7) != 0){
            minAIdiagonalLow0(i,9,0,pts,&tpts);
        }
        if((i & 7) != 7){
            minAIdiagonalLow0(i,7,7,pts,&tpts);
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            minAIdiagonalHigh0(i,9,7,pts,&tpts);
        }
        if((i & 7) != 0){
            minAIdiagonalHigh0(i,7,0,pts,&tpts);
        }
    }
    return pts;
}

void maxAIstraighHor0(u8 i,i8 itt,i8 stp,i8 pts,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *tpts = i8max(*tpts,pts+whitePcsToScore(board[i2]));
            return;
        }
        if((i2 & 7) == stp){
            return;
        }
    }
}

i8 minAIstraight0(u8 i,i8 pts,i8 tpts){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'A' || board[i2] > 'Z';i2++){
            if(board[i2] > 'a' && board[i2] < 'z'){
                tpts = i8min(tpts,pts+blackPcsToScore(board[i2]));
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
                tpts = i8min(tpts,pts+blackPcsToScore(board[i2]));
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
                tpts = i8min(tpts,pts+blackPcsToScore(board[i2]));
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
                tpts = i8min(tpts,pts+blackPcsToScore(board[i2]));
                break;
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return tpts;
}

i8 maxAIstraight0(u8 i,i8 pts,i8 tpts){
    if((i & 7) != 7){
        maxAIstraighHor0(i,1,7,pts,&tpts);
    }
    if((i & 7) != 0){
        maxAIstraighHor0(i,-1,0,pts,&tpts);
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                tpts = i8max(tpts,pts+whitePcsToScore(board[i2]));
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
                tpts = i8max(tpts,pts+whitePcsToScore(board[i2]));
                break;
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return tpts;
}

void maxAIdiagonalLow(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'a' || board[i2] > 'z';i2-=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += whitePcsToScore(board[i2]);
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= whitePcsToScore(board[i2]);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minAI(depth-1,*eval));
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

void maxAIdiagonalHigh(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += whitePcsToScore(board[i2]);
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= whitePcsToScore(board[i2]);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minAI(depth-1,*eval));
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

void minAIdiagonalLow(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i - itt;board[i2] < 'A' || board[i2] > 'Z';i2-=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            eval->pts += blackPcsToScore(board[i2]);
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8min(*tpts,maxAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= blackPcsToScore(board[i2]);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8min(*tpts,maxAI(depth-1,*eval));
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

void minAIdiagonalHigh(u8 i,u8 depth,u8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'A' || board[i2] > 'Z';i2+=itt){
        if(board[i2] > 'a' && board[i2] < 'z'){
            eval->pts += blackPcsToScore(board[i2]);
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,maxAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= blackPcsToScore(board[i2]);
            eval->ab.beta = i8min(eval->ab.beta,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,maxAI(depth-1,*eval));
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

void maxAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if(i > 7){
        if((i & 7) != 0){
            maxAIdiagonalLow(i,depth,9,0,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
        if((i & 7) != 7){
            maxAIdiagonalLow(i,depth,7,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            maxAIdiagonalHigh(i,depth,9,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return;
            }
        }
        if((i & 7) != 0){
            maxAIdiagonalHigh(i,depth,7,0,eval,tpts);
        }
    }
}

void minAIdiagonal(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if(i > 7){
        if((i & 7) != 0){
            minAIdiagonalLow(i,depth,9,0,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return eval->pts;
            }
        }
        if((i & 7) != 7){
            minAIdiagonalLow(i,depth,7,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return eval->pts;
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            minAIdiagonalHigh(i,depth,9,7,eval,tpts);
            if(eval->ab.beta <= eval->ab.alpha){
                return eval->pts;
            }
        }
        if((i & 7) != 0){
            minAIdiagonalHigh(i,depth,7,0,eval,tpts);
        }
    }
}

void maxAIstraighHor(u8 i,u8 depth,i8 itt,i8 stp,EVAL *eval,i8 *tpts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            eval->pts += whitePcsToScore(board[i2]);
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *tpts = i8max(*tpts,minAI(depth-1,*eval));
            board[i] = board[i2];
            board[i2] = tpcs;
            eval->pts -= whitePcsToScore(board[i2]);
            eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
            return;
        }
        else{
            movePieceD(i,i2);
            *tpts = i8max(*tpts,minAI(depth-1,*eval));
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

void maxAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if((i & 7) != 7){
        maxAIstraighHor(i,depth,1,7,eval,tpts);
        if(eval->ab.beta <= eval->ab.alpha){
            return;
        }
    }
    if((i & 7) != 0){
        maxAIstraighHor(i,depth,-1,0,eval,tpts);
        if(eval->ab.beta <= eval->ab.alpha){
            return;
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                eval->pts += whitePcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                *tpts = i8max(*tpts,minAI(depth-1,*eval));
                board[i] = board[i2];
                board[i2] = tpcs;
                eval->pts -= whitePcsToScore(board[i2]);
                eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
                if(eval->ab.beta <= eval->ab.alpha){
                    return;
                }
                break;
            }
            else{
                movePieceD(i,i2);
                *tpts = i8max(*tpts,minAI(depth-1,*eval));
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
                eval->pts += whitePcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                *tpts = i8max(*tpts,minAI(depth-1,*eval));
                board[i] = board[i2];
                board[i2] = tpcs;
                eval->pts -= whitePcsToScore(board[i2]);
                eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
                if(eval->ab.beta <= eval->ab.alpha){
                    return;
                }
                break;
            }
            else{
                movePieceD(i,i2);
                *tpts = i8max(*tpts,minAI(depth-1,*eval));
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

void minAIstraight(u8 i,u8 depth,EVAL *eval,i8 *tpts){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'A' || board[i2] > 'Z';i2++){
            if(board[i2] > 'a' && board[i2] < 'z'){
                eval->pts += blackPcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                *tpts = i8min(*tpts,maxAI(depth-1,*eval));
                board[i] = board[i2];
                board[i2] = tpcs;
                eval->pts -= blackPcsToScore(board[i2]);
                eval->ab.beta = i8min(eval->ab.beta,*tpts);
                if(eval->ab.beta <= eval->ab.alpha){
                    return;
                }
                break;
            }
            else{
                movePieceD(i,i2);
                *tpts = i8min(*tpts,maxAI(depth-1,*eval));
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
                eval->pts += blackPcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                *tpts = i8min(*tpts,maxAI(depth-1,*eval));
                board[i] = board[i2];
                board[i2] = tpcs;
                eval->pts -= blackPcsToScore(board[i2]);
                eval->ab.beta = i8min(eval->ab.beta,*tpts);
                if(eval->ab.beta <= eval->ab.alpha){
                    return;
                }
                break;
            }
            else{
                movePieceD(i,i2);
                *tpts = i8min(*tpts,maxAI(depth-1,*eval));
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
                eval->pts += blackPcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                *tpts = i8min(*tpts,maxAI(depth-1,*eval));
                board[i] = board[i2];
                board[i2] = tpcs;
                eval->pts -= blackPcsToScore(board[i2]);
                eval->ab.beta = i8min(eval->ab.beta,*tpts);
                if(eval->ab.beta <= eval->ab.alpha){
                    return;
                }
                break;
            }
            else{
                movePieceD(i,i2);
                *tpts = i8min(*tpts,maxAI(depth-1,*eval));
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
                eval->pts += blackPcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                *tpts = i8min(*tpts,maxAI(depth-1,*eval));
                board[i] = board[i2];
                board[i2] = tpcs;
                eval->pts -= blackPcsToScore(board[i2]);
                eval->ab.beta = i8min(eval->ab.beta,*tpts);
                return;
            }
            else{
                movePieceD(i,i2);
                *tpts = i8min(*tpts,maxAI(depth-1,*eval));
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

void maxAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] < 'Z' && board[i+dst] > 'A'){
        eval->pts += whitePcsToScore(board[i+dst]);
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8max(*tpts,minAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= whitePcsToScore(board[i+dst]);
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
    else if(board[i+dst] == '*'){
        movePieceD(i,i+dst);
        *tpts = i8max(*tpts,minAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.alpha = i8max(eval->ab.alpha,*tpts);
    }
}

void minAIknight(i8 i,i8 dst,u8 depth,EVAL *eval,i8 *tpts){
    if(board[i+dst] < 'z' && board[i+dst] > 'a'){
        eval->pts += blackPcsToScore(board[i+dst]);
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxAI(depth-1,*eval));
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
        eval->pts -= blackPcsToScore(board[i+dst]);
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
    else if(board[i+dst] == '*'){
        movePieceD(i,i+dst);
        *tpts = i8min(*tpts,maxAI(depth-1,*eval));
        movePieceD(i+dst,i);
        eval->ab.beta = i8min(eval->ab.beta,*tpts);
    }
}

i8 maxAI(u8 depth,EVAL eval){
    i8 tpts = -75;
    if(!depth){
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(i > 49){
                    if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
                        tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i+9])+8);
                    }
                    if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
                        tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i+7])+8);
                    }
                    if(board[i+8] == '*'){
                        tpts = i8max(tpts,eval.pts+8);
                    }
                }
                else{
                    if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
                        tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i+9]));
                    }
                    if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
                        tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i+7]));
                    }
                    if(board[i+8] == '*'){
                        tpts = i8max(tpts,eval.pts);
                    }
                }
                break;
            case 'n':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i-6]));
                    }
                    if(sy > 1){
                        tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i-10]));
                    }
                    if(sx > 1){
                        if(sy < 7){
                            tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i-15]));
                        }
                        if(sy > 0){
                            tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i-17]));
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i+6]));
                    }
                    if(sy < 6){
                        tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i+10]));
                    }
                    if(sx < 6){
                        if(sy > 0){
                            tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i+15]));
                        }
                        if(sy < 7){
                            tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i+17]));
                        }
                    }
                }
                break;
                }
            case 'b':
                tpts = maxAIdiagonal0(i,eval.pts,tpts);
                break;
            case 'r':
                tpts = maxAIstraight0(i,eval.pts,tpts);
                break;
            case 'q':
                tpts = maxAIstraight0(i,eval.pts,tpts);
                tpts = maxAIdiagonal0(i,eval.pts,tpts);
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
                            tpts = i8max(tpts,eval.pts+whitePcsToScore(board[i+i2+i3]));
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
                        tpts = i8max(tpts,minAI(depth-1,eval));
                        eval.pts -= 8;
                        movePieceD(i+8,i);
                        board[i] = 'p';
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
                        eval.pts += whitePcsToScore(board[i+9]) + 8;
                        char tpcs = board[i+9];
                        board[i] = 'q';
                        capturePieceD(i,i+9);
                        tpts = i8max(tpts,minAI(depth-1,eval));
                        board[i] = 'p';
                        board[i+9] = tpcs;
                        eval.pts -= whitePcsToScore(board[i+9]) + 8;
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
                        eval.pts += whitePcsToScore(board[i+7]) + 8;
                        char tpcs = board[i+7];
                        board[i] = 'q';
                        capturePieceD(i,i+7);
                        tpts = i8max(tpts,minAI(depth-1,eval));
                        board[i] = 'p';
                        board[i+7] = tpcs;
                        eval.pts -= whitePcsToScore(board[i+7]) + 8;
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                else{
                    if(board[i+8] == '*'){
                        movePieceD(i,i+8);
                        tpts = i8max(tpts,minAI(depth-1,eval));
                        movePieceD(i+8,i);
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i > 7 && i < 16 && board[i+16] == '*'){
                            movePieceD(i,i+16);
                            tpts = i8max(tpts,minAI(depth-1,eval));
                            movePieceD(i+16,i);
                            eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                    if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
                        eval.pts += whitePcsToScore(board[i+9]);
                        char tpcs = board[i+9];
                        capturePieceD(i,i+9);
                        tpts = i8max(tpts,minAI(depth-1,eval));
                        board[i] = board[i+9];
                        board[i+9] = tpcs;
                        eval.pts -= whitePcsToScore(board[i+9]);
                        eval.ab.alpha = i8max(eval.ab.alpha,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
                        eval.pts += whitePcsToScore(board[i+7]);
                        char tpcs = board[i+7];
                        capturePieceD(i,i+7);
                        tpts = i8max(tpts,minAI(depth-1,eval));
                        board[i] = board[i+7];
                        board[i+7] = tpcs;
                        eval.pts -= whitePcsToScore(board[i+7]);
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
                        maxAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy > 1){
                        maxAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx > 1){
                        if(sy < 7){
                            maxAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy > 0){
                            maxAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        maxAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy < 6){
                        maxAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx < 6){
                        if(sy > 0){
                            maxAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy < 7){
                            maxAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                break;
                }
            case 'b':
                maxAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'r':
                maxAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'q':
                maxAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                maxAIdiagonal(i,depth,&eval,&tpts);
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
                                char tpcs = board[i+i2+i3];
                                eval.pts += whitePcsToScore(board[i+i2+i3]);
                                capturePieceD(i,i+i2+i3);
                                eval.pts = i8max(eval.pts,minAI(depth-1,eval));
                                board[i] = board[i+i2+i3];
                                board[i+i2+i3] = tpcs;
                                eval.ab.alpha = i8max(eval.ab.alpha,eval.pts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;
                                }
                            }
                            else if(board[i+i2+i3] == '*'){
                                movePieceD(i,i+i2+i3);
                                eval.pts = i8max(eval.pts,minAI(depth-1,eval));
                                movePieceD(i+i2+i3,i);
                                eval.ab.alpha = i8max(eval.ab.alpha,eval.pts);
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

i8 minAI(u8 depth,EVAL eval){
    i8 tpts = 75;
    if(!depth){
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(i < 16 && i > 7){
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                        tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i-9])-8);
                    }
                    if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                        tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i-7])-8);
                    }
                    if(board[i-8] == '*'){
                        tpts = i8min(tpts,eval.pts-8);
                    }
                }
                else{
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                        tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i-9]));
                    }
                    if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                        tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i-7]));
                    }
                    if(board[i-8] == '*'){
                        tpts = i8min(tpts,eval.pts);
                    }
                }
                break;
            case 'N':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i-6]));
                    }
                    if(sy > 1){
                        tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i-10]));
                    }
                    if(sx > 1){
                        if(sy < 7){
                            tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i-15]));
                        }
                        if(sy > 0){
                            tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i-17]));
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i+6]));
                    }
                    if(sy < 6){
                        tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i+10]));
                    }
                    if(sx < 6){
                        if(sy > 0){
                            tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i+15]));
                        }
                        if(sy < 7){
                            tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i+17]));
                        }
                    }
                }
                break;
                }
            case 'B':
                tpts = minAIdiagonal0(i,eval.pts,tpts);
                break;
            case 'R':
                tpts = minAIstraight0(i,eval.pts,tpts);
                break;
            case 'Q':
                tpts = minAIdiagonal0(i,eval.pts,tpts);
                tpts = minAIstraight0(i,eval.pts,tpts);
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
                                tpts = i8min(tpts,eval.pts+blackPcsToScore(board[i+i2+i3]));
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
                        tpts = i8min(tpts,maxAI(depth-1,eval));
                        eval.pts += 8;
                        movePieceD(i-8,i);
                        board[i] = 'P';
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                        eval.pts += blackPcsToScore(board[i-9]);
                        char tpcs = board[i-9];
                        board[i] = 'Q';
                        capturePieceD(i,i-9);
                        tpts = i8min(tpts,maxAI(depth-1,eval));
                        board[i-9] = tpcs;
                        board[i] = 'P';
                        eval.pts -= blackPcsToScore(board[i-9]);
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                        eval.pts += blackPcsToScore(board[i-7]);
                        char tpcs = board[i-7];
                        board[i] = 'Q';
                        capturePieceD(i,i-7);
                        tpts = i8min(tpts,maxAI(depth-1,eval));
                        board[i-7] = tpcs;
                        board[i] = 'P';
                        eval.pts -= blackPcsToScore(board[i-7]);
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                }
                else{
                    if(board[i-8] == '*'){
                        movePieceD(i,i-8);
                        tpts = i8min(tpts,maxAI(depth-1,eval));
                        movePieceD(i-8,i);
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                        if(i > 47 && i < 56 && board[i-16] == '*'){
                            movePieceD(i,i-16);
                            tpts = i8min(tpts,maxAI(depth-1,eval));
                            movePieceD(i-16,i);
                            eval.ab.beta = i8min(eval.ab.beta,tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                    if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                        eval.pts += blackPcsToScore(board[i-9]);
                        char tpcs = board[i-9];
                        capturePieceD(i,i-9);
                        tpts = i8min(tpts,maxAI(depth-1,eval));
                        board[i] = board[i-9];
                        board[i-9] = tpcs;
                        eval.pts -= blackPcsToScore(board[i-9]);
                        eval.ab.beta = i8min(eval.ab.beta,tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                        eval.pts += blackPcsToScore(board[i-7]);
                        char tpcs = board[i-7];
                        capturePieceD(i,i-7);
                        tpts = i8min(tpts,maxAI(depth-1,eval));
                        board[i] = board[i-7];
                        board[i-7] = tpcs;
                        eval.pts -= blackPcsToScore(board[i-7]);
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
                        minAIknight(i,-6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy > 1){
                        minAIknight(i,-10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx > 1){
                        if(sy < 7){
                            minAIknight(i,-15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy > 0){
                            minAIknight(i,-17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        minAIknight(i,6,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sy < 6){
                        minAIknight(i,10,depth,&eval,&tpts);
                        if(eval.ab.beta <= eval.ab.alpha){
                            return tpts;
                        }
                    }
                    if(sx < 6){
                        if(sy > 0){
                            minAIknight(i,15,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                        if(sy < 7){
                            minAIknight(i,17,depth,&eval,&tpts);
                            if(eval.ab.beta <= eval.ab.alpha){
                                return tpts;
                            }
                        }
                    }
                }
                break;
                }
            case 'B':
                minAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'R':
                minAIstraight(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                break;
            case 'Q':
                minAIdiagonal(i,depth,&eval,&tpts);
                if(eval.ab.beta <= eval.ab.alpha){
                    return tpts;
                }
                minAIstraight(i,depth,&eval,&tpts);
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
                                eval.pts += blackPcsToScore(board[i+i2+i3]);
                                char tpcs = board[i+i2+i3];
                                capturePieceD(i,i+i2+i3);
                                tpts = i8min(tpts,maxAI(depth-1,eval));
                                board[i] = board[i+i2+i3];
                                board[i+i2+i3] = tpcs;
                                eval.pts -= blackPcsToScore(board[i+i2+i3]);
                                eval.ab.beta = i8min(eval.ab.beta,tpts);
                                if(eval.ab.beta <= eval.ab.alpha){
                                    return tpts;
                                }
                            }
                            else if(board[i+i2+i3] == '*'){
                                movePieceD(i,i+i2+i3);
                                tpts = i8min(tpts,maxAI(depth-1,eval));
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

void printMove(u8 src,u8 dst){
    printf("%c",(src&7)+'A');
    printf("%c",(src>>3)+'0');
    printf("%c",'-');
    printf("%c",(dst&7)+'A');
    printf("%c = ",(dst>>3)+'0');
}

void captureAI(u8 src,u8 dst,i8 *pts,u8 *movC){
    printMove(src,dst);
    char tpcs = board[dst];
    capturePieceD(src,dst);
    printf("%c",'|');
    i8 tpts,tpts2;
    for(int i = 0;i < DEPTH;i++){
        printf("dpt %i = ",i);
        printf("pts ");
        tpts = minAI(i,(EVAL){-75,75,0});
        tpts2 = whitePcsToScore(tpcs);
        if(tpts2 + tpts < 128){
            tpts += tpts2;
        }
        if(tpts >= 0 && tpts < 10){
            printf("%c",' ');
        }
        if(tpts > -10){
            printf("%c",' ');
        }
        printf("%i|",tpts);
        if(tpts < -50 || tpts > 50){
            printf("%c",'\n');
            goto stopSearching; 
        }
    }
    tpts  = minAI(DEPTH,(EVAL){-75,75,0});
    tpts2 = whitePcsToScore(tpcs);
    if(tpts2 + tpts < 128){
        tpts += tpts2;
    }
    printf("dpt %i = ",DEPTH);
    printf("pts %i\n",tpts);
stopSearching:
    board[src] = board[dst];
    board[dst] = tpcs;
    if(*pts <= tpts){
        if(*pts != tpts){
            *pts = tpts;
            *movC = 0;
        }
        master.mov[*movC].src = src;
        master.mov[*movC].dst = dst;
        ++*movC;
    }
}

void moveAI(u8 src,u8 dst,i8 *pts,u8 *movC){
    printMove(src,dst);
    movePieceD(src,dst);
    printf("%c",'|');
    i8 tpts;
    for(int i = 0;i < DEPTH;i++){
        tpts = minAI(i,(EVAL){-75,75,0});
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
            printf("%c",'\n');
            goto stopSearching;
        }
    }
    long long t = __rdtsc();
    tpts = minAI(DEPTH,(EVAL){-75,75,0});
    long long t2 = __rdtsc();
    printf("dpt %i = ",DEPTH);
    printf("pts %i,",tpts);
    printf("t = %i\n",t2-t>>16);
stopSearching:
    movePieceD(dst,src);
    if(*pts <= tpts){
        if(*pts != tpts){
            *pts = tpts;
            *movC = 0;
        }
        master.mov[*movC].src = src;
        master.mov[*movC].dst = dst;
        ++*movC;
    }
}

void moveCaptureAI(u8 src,u8 dst,i8 *pts,u8 *movC){
    if(board[dst] > 'A' && board[dst] < 'Z'){
        captureAI(src,dst,pts,movC);
    }
    else if(board[dst] == '*'){
        moveAI(src,dst,pts,movC);
    }
}

void straightAI(u8 i,i8 *pts,u8 *movC){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'a' || board[i2] > 'z';i2++){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureAI(i,i2,pts,movC);
                break;
            }
            else{
                moveAI(i,i2,pts,movC);
            }
            if((i2 & 7) == 7){
                break;
            }
        }
    }
    if((i & 7) != 0){
        for(int i2 = i - 1;board[i2] < 'a' || board[i2] > 'z';i2--){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureAI(i,i2,pts,movC);
                break;
            }
            else{
                moveAI(i,i2,pts,movC);
            }
            if((i2 & 7) == 0){
                break;
            }
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureAI(i,i2,pts,movC);
                break;
            }
            else{
                moveAI(i,i2,pts,movC);
            }
            if(i2 < 7){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureAI(i,i2,pts,movC);
                break;
            }
            else{
                moveAI(i,i2,pts,movC);
            }
            if(i2 > 55){
                break;
            }
        }
    }
}

void diagonalAI(u8 i,i8 *pts,u8 *movC){
    if(i > 7){
        if((i & 7) != 0){
            for(int i2 = i - 9;board[i2] < 'a' || board[i2] > 'z';i2-=9){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureAI(i,i2,pts,movC);
                    break;
                }
                else{
                    moveAI(i,i2,pts,movC);
                }
                if(i2 < 0 || (i2 & 7) == 0){
                    break;
                }
            }
        }
        if((i & 7) != 7){
            for(int i2 = i - 7;board[i2] < 'a' || board[i2] > 'z';i2-=7){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureAI(i,i2,pts,movC);
                    break;
                }
                else{
                    moveAI(i,i2,pts,movC);
                }
                if(i2 < 0 || (i2 & 7) == 7){
                    break;
                }
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            for(int i2 = i + 9;board[i2] < 'a' || board[i2] > 'z';i2+=9){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureAI(i,i2,pts,movC);
                    break;
                }
                else{
                    moveAI(i,i2,pts,movC);
                }
                if(i2 > 55 || (i2 & 7) == 7){
                    break;
                }
            }
        }
        if((i & 7) != 0){
            for(int i2 = i + 7;board[i2] < 'a' || board[i2] > 'z';i2+=7){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureAI(i,i2,pts,movC);
                    break;
                }
                else{
                    moveAI(i,i2,pts,movC);
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

void main(){
    master.mov = malloc(sizeof(CHESSMOVE)*64);
    printf("welcome to chessTM\n");
    printBoard();
    char inp[5];
mainLoop:
    for(;;) {
        scanf("%s",inp);
        u8 SX = inp[1] - '0';
        u8 SY = inp[0] - 'A';
        u8 DX = inp[4] - '0';
        u8 DY = inp[3] - 'A';
        if(SX*8+SY < 0 || SX*8+SY > 63 || DX*8+DY < 0 || DX*8+DY > 63){
            continue;
        }
        switch(board[SX*8+SY]){
        case '*':
            continue;
        case 'K': {
            int MNX = abs(SX - DX);
            int MNY = abs(SY - DY);
            if (MNX == 1 || MNY == 1) {
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
                        goto mainLoop;
                    }
                }
                movePiece(SX,SY,DX,DY);
                movePiece(7,7,7,5);
            }
            else if(DY == 1 && (castleRights & 0b0010) == 0b0010){
                for(int i = 3;i > 0;i--){
                    if(board[DX*8+i] != '*'){
                        goto mainLoop;
                    }
                }
                movePiece(SX,SY,DX,DY);
                movePiece(0,0,0,2);
            }
            else{
                continue;
            }
            break;
        }
        case 'Q':
            if(SX==DX||SY==DY){
                if(!checkStraight(SX,SY,DX,DY)){continue;}
            }
            else{
                if(!checkDiagonal(SX,SY,DX,DY)){continue;}
            }
            break;
        case 'N': {
            int MNX = abs(SX - DX);
            int MNY = abs(SY - DY);
            if ((MNX == 1 && MNY == 2) || (MNX == 2 && MNY == 1)) {
                if(board[DX*8+DY] == '*'){
                    movePiece(SX, SY, DX, DY);
                }
                else if(board[DX*8+DY] < 'z' && board[DX*8+DY] > 'a'){
                    capturePiece(SX,SY,DX,DY);
                }
            }
            break;
        }
        case 'B':
            if(!checkDiagonal(SX,SY,DX,DY)){continue;}
            break;
        case 'R':
            if(DY == 0){
                castleRights &= ~0b0010;
            }
            if(DY == 7){
                castleRights &= ~0b0001;
            }
            if(!checkStraight(SX,SY,DX,DY)){continue;}
            break;
        case 'P':
            if (SX == DX + 1) {
                if (SY == DY) {
                    if (board[DX*8+DY] == '*') {
                        if (DX == 0){
                            printf("promotion");
                            do{
                                scanf("%s",inp);
                            }while(inp[0]!='Q'&&inp[0]!='R'&&inp[0]!='N'&&inp[0]!='B');
                            board[SX*8+SY] = inp[0];
                        }
                        movePiece(SX,SY,DX,DY);
                    }
                    else{
                        continue;
                    }
                }
                else if (SY == DY + 1 || SY == DY - 1) {
                    if (board[DX*8+DY] >= 'a' && board[DX*8+DY] <= 'z') {
                        if (DX == 0){
                            printf("promotion");
                            do{
                                scanf("%s",inp);
                            }while(inp[0]!='Q'&&inp[0]!='R'&&inp[0]!='N'&&inp[0]!='B');
                            board[SX*8+SY] = inp[0];
                        }
                        capturePiece(SX,SY,DX,DY);
                    }
                    else{
                        continue;
                    }
                }
            }
            else if(SX == DX + 2){
                if(SX == 6){
                    if (board[(DX-1)*8+DY] == '*' && board[DX*8+DY] == '*') {
                        movePiece(SX,SY,DX,DY);
                    }
                }
            }
            else{
                continue;
            }
            break;
        }
        printBoard();
        u8 movC = 0;
        i8 pts = -128;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(board[i+8] == '*'){
                    moveAI(i,i+8,&pts,&movC);
                    if(i > 7 && i < 16){
                        if(board[i+16] == '*'){
                            moveAI(i,i+16,&pts,&movC);
                        }
                    }
                }
                if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
                    captureAI(i,i+9,&pts,&movC);
                }
                if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
                    captureAI(i,i+7,&pts,&movC);
                }
                break;
            case 'n':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        moveCaptureAI(i,i-6,&pts,&movC);
                    }
                    if(sy > 1){
                        moveCaptureAI(i,i-10,&pts,&movC);
                    }
                    if(sx > 1){
                        if(sy < 7){
                            moveCaptureAI(i,i-15,&pts,&movC);
                        }
                        if(sy > 0){
                            moveCaptureAI(i,i-17,&pts,&movC);
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        moveCaptureAI(i,i+6,&pts,&movC);
                    }
                    if(sy < 6){
                        moveCaptureAI(i,i+10,&pts,&movC);
                    }
                    if(sx < 6){
                        if(sy > 0){
                            moveCaptureAI(i,i+15,&pts,&movC);
                        }
                        if(sy < 7){
                            moveCaptureAI(i,i+17,&pts,&movC);
                        }
                    }
                }
                break;
            }
            case 'b':
                diagonalAI(i,&pts,&movC);
                break;
            case 'r':
                straightAI(i,&pts,&movC);
                break;
            case 'q':
                diagonalAI(i,&pts,&movC);
                straightAI(i,&pts,&movC);
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
                            moveCaptureAI(i,i+i2+i3,&pts,&movC);
                        }
                    }
                }
                break;
            }
        }
        if(!movC){
            printf("stalemate");
            exit(0);
        }
        u8 mv = __rdtsc()%movC;
        u8 mvsrc = master.mov[mv].src;
        u8 mvdst = master.mov[mv].dst;
        movePieceBlack(mvsrc>>3,mvsrc&7,mvdst>>3,mvdst&7);
        printf("\n%c",(mvsrc&7)+'A');
        printf("%c",(mvsrc>>3)+'0');
        printf("%c",'-');
        printf("%c",(mvdst&7)+'A');
        printf("%c = ",(mvdst>>3)+'0');
        printf("%hhi\n",pts);
        printBoard();
    }
}
