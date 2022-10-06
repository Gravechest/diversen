#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <intrin.h>

#define DEPTH 5

#pragma warning(disable:4996)

typedef char i8;
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct{
    u8 src;
    u8 dst;
}CHESSMOVE;

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

char board[64] = {
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

i8 i8minAI(u8 depth);
i8 i8maxAI(u8 depth);

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
        return -50;
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
        return 50;
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

void i8maxAIdiagonalLow0(u8 i,u8 itt,i8 stp,i8 *pts){
    for(int i2 = i - itt;board[i2] < 'a' || board[i2] > 'z';i2-=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *pts = i8max(*pts,whitePcsToScore(board[i2]));
            break;
        }
        if(i2 < 8 || (i2 & stp) == 0){
            break;
        }
    }
}

void i8maxAIdiagonalHigh0(u8 i,u8 itt,i8 stp,i8 *pts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *pts = i8max(*pts,whitePcsToScore(board[i2]));
            break;
        }
        if(i2 > 54 || (i2 & stp) == 0){
            break;
        }
    }
}

i8 i8maxAIdiagonal0(u8 i,i8 pts){
    if(i > 7){
        if((i & 7) != 0){
            i8maxAIdiagonalLow0(i,9,0,&pts);
        }
        if((i & 7) != 7){
            i8maxAIdiagonalLow0(i,7,7,&pts);
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            i8maxAIdiagonalHigh0(i,9,7,&pts);
        }
        if((i & 7) != 0){
            i8maxAIdiagonalHigh0(i,7,0,&pts);
        }
    }
    return pts;
}

void i8maxAIdiagonalLow(u8 i,u8 depth,u8 itt,i8 stp,i8 *pts){
    for(int i2 = i - itt;board[i2] < 'a' || board[i2] > 'z';i2-=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            char lpts = whitePcsToScore(board[i2]);
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *pts = i8max(*pts,i8minAI(depth-1) + lpts);
            board[i] = board[i2];
            board[i2] = tpcs;
            break;
        }
        else{
            movePieceD(i,i2);
            *pts = i8max(*pts,i8minAI(depth-1));
            movePieceD(i2,i);
        }
        if(i2 < 8 || (i2 & stp) == 0){
            break;
        }
    }
}

void i8maxAIdiagonalHigh(u8 i,u8 depth,u8 itt,i8 stp,i8 *pts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            char lpts = whitePcsToScore(board[i2]);
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *pts = i8max(*pts,i8minAI(depth-1) + lpts);
            board[i] = board[i2];
            board[i2] = tpcs;
            break;
        }
        else{
            movePieceD(i,i2);
            *pts = i8max(*pts,i8minAI(depth-1));
            movePieceD(i2,i);
        }
        if(i2 > 54 || (i2 & stp) == 7){
            break;
        }
    }
}

i8 i8maxAIdiagonal(u8 i,u8 depth,i8 pts){
    if(i > 7){
        if((i & 7) != 0){
            i8maxAIdiagonalLow(i,depth,9,0,&pts);
        }
        if((i & 7) != 7){
            i8maxAIdiagonalLow(i,depth,7,7,&pts);
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            i8maxAIdiagonalHigh(i,depth,9,7,&pts);
        }
        if((i & 7) != 0){
            i8maxAIdiagonalHigh(i,depth,7,0,&pts);
        }
    }
    return pts;
}

void i8maxAIstraighHor0(u8 i,i8 itt,i8 stp,i8 *pts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            *pts = i8max(*pts,whitePcsToScore(board[i2]));
            break;
        }
        if((i2 & 7) == stp){
            break;
        }
    }
}

i8 i8maxAIstraight0(u8 i,i8 pts){
    if((i & 7) != 7){
        i8maxAIstraighHor0(i,1,7,&pts);
    }
    if((i & 7) != 0){
        i8maxAIstraighHor0(i,-1,0,&pts);
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                pts = i8max(pts,whitePcsToScore(board[i2]));
                break;
            }
            if(i2 < 7){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                pts = i8max(pts,whitePcsToScore(board[i2]));
                break;
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return pts;
}

void i8maxAIstraighHor(u8 i,u8 depth,i8 itt,i8 stp,i8 *pts){
    for(int i2 = i + itt;board[i2] < 'a' || board[i2] > 'z';i2+=itt){
        if(board[i2] > 'A' && board[i2] < 'Z'){
            char lpts = whitePcsToScore(board[i2]);
            char tpcs = board[i2];
            capturePieceD(i,i2);
            *pts = i8max(*pts,i8minAI(depth-1) + lpts);
            board[i] = board[i2];
            board[i2] = tpcs;
            break;
        }
        else{
            movePieceD(i,i2);
            *pts = i8max(*pts,i8minAI(depth-1));
            movePieceD(i2,i);
        }
        if((i2 & 7) == stp){
            break;
        }
    }
}

i8 i8maxAIstraight(u8 i,u8 depth,i8 pts){
    if((i & 7) != 7){
        i8maxAIstraighHor(i,depth,1,7,&pts);
    }
    if((i & 7) != 0){
        i8maxAIstraighHor(i,depth,-1,0,&pts);
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                char lpts = whitePcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                pts = i8max(pts,i8minAI(depth-1) + lpts);
                board[i] = board[i2];
                board[i2] = tpcs;
                break;
            }
            else{
                movePieceD(i,i2);
                pts = i8max(pts,i8minAI(depth-1));
                movePieceD(i2,i);
            }
            if(i2 < 7){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                char lpts = whitePcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                pts = i8max(pts,i8minAI(depth-1) + lpts);
                board[i] = board[i2];
                board[i2] = tpcs;
                break;
            }
            else{
                movePieceD(i,i2);
                pts = i8max(pts,i8minAI(depth-1));
                movePieceD(i2,i);
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return pts;
}

void i8maxAIknight(i8 i,i8 dst,u8 depth,i8 *pts){
    if(board[i+dst] < 'Z' && board[i+dst] > 'A'){
        char lpts = whitePcsToScore(board[i+dst]);
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *pts = i8max(*pts,i8minAI(depth-1) + lpts);
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
    }
    else if(board[i+dst] == '*'){
        movePieceD(i,i+dst);
        *pts = i8max(*pts,i8minAI(depth-1));
        movePieceD(i+dst,i);
    }
}

i8 i8maxAI(u8 depth){
    i8 pts = -128;
    if(!depth){
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(board[i+9] > 'a' && board[i+9] < 'z' && (i & 7) != 0){
                    pts = i8max(pts,whitePcsToScore(board[i+9]));
                }
                if(board[i+7] > 'a' && board[i+7] < 'z' && (i & 7) != 7){
                    pts = i8max(pts,whitePcsToScore(board[i+7]));
                }
                break;
            case 'n':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy > 1){
                        pts = i8max(pts,whitePcsToScore(board[i-6]));
                    }
                    if(sy < 6){
                        pts = i8max(pts,whitePcsToScore(board[i-10]));
                    }
                    if(sx > 1){
                        if(sy > 0){
                            pts = i8max(pts,whitePcsToScore(board[i-15]));
                        }
                        if(sy < 7){
                            pts = i8max(pts,whitePcsToScore(board[i-17]));
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        pts = i8max(pts,whitePcsToScore(board[i+6]));
                    }
                    if(sy < 6){
                        pts = i8max(pts,whitePcsToScore(board[i+10]));
                    }
                    if(sx < 6){
                        if(sy > 0){
                            pts = i8max(pts,whitePcsToScore(board[i+15]));
                        }
                        if(sy < 7){
                            pts = i8max(pts,whitePcsToScore(board[i+17]));
                        }
                    }
                }
                break;
                }
            case 'b':
                pts = i8maxAIdiagonal0(i,pts);
                break;
            case 'r':
                pts = i8maxAIstraight0(i,pts);
                break;
            case 'q':
                pts = i8maxAIstraight0(i,pts);
                pts = i8maxAIdiagonal0(i,pts);
                break;
            case 'k':
                for(int i2 = -1;i2 < 2;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i2+i3!=0&&i+i2+i3>0&&i+i2+i3<64){
                            pts = i8max(pts,whitePcsToScore(board[i+i2+i3]));
                        }
                    }
                }
                break;
            }
        }
    }
    else{
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(board[i+8] == '*'){
                    movePieceD(i,i+8);
                    pts = i8max(pts,i8minAI(depth-1));
                    movePieceD(i+8,i);
                    if(i > 7 && i < 16 && board[i+16] == '*'){
                        movePieceD(i,i+16);
                        pts = i8max(pts,i8minAI(depth-1));
                        movePieceD(i+16,i);
                    }
                }
                if(board[i+9] > 'a' && board[i+9] < 'z' && (i & 7) != 7){
                    char lpts = whitePcsToScore(board[i+9]);
                    char tpcs = board[i+9];
                    capturePieceD(i,i+9);
                    pts = i8max(pts,i8minAI(depth-1) + lpts);
                    board[i] = board[i+9];
                    board[i+9] = tpcs;
                }
                if(board[i+7] > 'a' && board[i+7] < 'z' && (i & 7) != 0){
                    char lpts = whitePcsToScore(board[i+7]);
                    char tpcs = board[i+7];
                    capturePieceD(i,i+7);
                    pts = i8max(pts,i8minAI(depth-1) + lpts);
                    board[i] = board[i+7];
                    board[i+7] = tpcs;
                }
                break;
            case 'n':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy > 1){
                        i8maxAIknight(i,-6,depth,&pts);
                    }
                    if(sy < 6){
                        i8maxAIknight(i,-10,depth,&pts);
                    }
                    if(sx > 1){
                        if(sy > 0){
                            i8maxAIknight(i,-15,depth,&pts);
                        }
                        if(sy < 7){
                            i8maxAIknight(i,-107,depth,&pts);
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        i8maxAIknight(i,6,depth,&pts);
                    }
                    if(sy < 6){
                        i8maxAIknight(i,10,depth,&pts);
                    }
                    if(sx < 6){
                        if(sy > 0){
                            i8maxAIknight(i,15,depth,&pts);
                        }
                        if(sy < 7){
                            i8maxAIknight(i,17,depth,&pts);
                        }
                    }
                }
                break;
                }
            case 'b':
                pts = i8maxAIdiagonal(i,depth,pts);
                break;
            case 'r':
                pts = i8maxAIstraight(i,depth,pts);
                break;
            case 'q':
                pts = i8maxAIstraight(i,depth,pts);
                pts = i8maxAIdiagonal(i,depth,pts);
                break;
            case 'k':
                for(int i2 = -1;i2 < 2;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i2+i3!=0&&i+i2+i3>0&&i+i2+i3<64){
                            if(board[i+i2+i3] < 'Z' && board[i+i2+i3] > 'A'){
                                char lpts = whitePcsToScore(board[i+i2+i3]);
                                char tpcs = board[i+i2+i3];
                                capturePieceD(i,i+i2+i3);
                                pts = i8max(pts,i8minAI(depth-1) + lpts);
                                board[i] = board[i+i2+i3];
                                board[i+i2+i3] = tpcs;
                            }
                            else if(board[i+i2+i3] == '*'){
                                movePieceD(i,i+i2+i3);
                                pts = i8max(pts,i8minAI(depth-1));
                                movePieceD(i+i2+i3,i);
                            }
                        }
                    }
                }
                break;
            }
        }
    }
    return pts;
}

i8 i8minAIdiagonal0(u8 i,i8 pts){
    if(i > 7){
        if((i & 7) != 0){
            for(int i2 = i - 9;board[i2] < 'A' || board[i2] > 'Z';i2-=9){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    pts = i8min(pts,blackPcsToScore(board[i2]));
                    break;
                }
                if(i2 < 8 || (i2 & 7) == 0){
                    break;
                }
            }
            
        }
        if((i & 7) != 7){
            for(int i2 = i - 7;board[i2] < 'A' || board[i2] > 'Z';i2-=7){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    pts = i8min(pts,blackPcsToScore(board[i2]));
                    break;
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
                    pts = i8min(pts,blackPcsToScore(board[i2]));
                    break;
                }
                if(i2 > 54 || (i2 & 7) == 7){
                    break;
                }
            }
        }
        if((i & 7) != 0){
            for(int i2 = i + 7;board[i2] < 'A' || board[i2] > 'Z';i2+=7){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    pts = i8min(pts,blackPcsToScore(board[i2]));
                    break;
                }
                if(i2 > 54 || (i2 & 7) == 0){
                    break;
                }
            }
        }
    }
    return pts;
}

i8 i8minAIdiagonal(u8 i,u8 depth,i8 pts){
    if(i > 7){
        if((i & 7) != 0){
            for(int i2 = i - 9;board[i2] < 'A' || board[i2] > 'Z';i2-=9){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    char lpts = blackPcsToScore(board[i2]);
                    char tpcs = board[i2];
                    capturePieceD(i,i2);
                    pts = i8min(pts,i8maxAI(depth-1) + lpts);
                    board[i] = board[i2];
                    board[i2] = tpcs;
                    break;
                }
                else{
                    movePieceD(i,i2);
                    pts = i8min(pts,i8maxAI(depth-1));
                    movePieceD(i2,i);
                }
                if(i2 < 8 || (i2 & 7) == 0){
                    break;
                }
            }
        }
        if((i & 7) != 7){
            for(int i2 = i - 7;board[i2] < 'A' || board[i2] > 'Z';i2-=7){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    char lpts = blackPcsToScore(board[i2]);
                    char tpcs = board[i2];
                    capturePieceD(i,i2);
                    pts = i8min(pts,i8maxAI(depth-1) + lpts);
                    board[i] = board[i2];
                    board[i2] = tpcs;
                    break;
                }
                else{
                    movePieceD(i,i2);
                    pts = i8min(pts,i8maxAI(depth-1));
                    movePieceD(i2,i);
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
                    char lpts = blackPcsToScore(board[i2]);
                    char tpcs = board[i2];
                    capturePieceD(i,i2);
                    pts = i8min(pts,i8maxAI(depth-1) + lpts);
                    board[i] = board[i2];
                    board[i2] = tpcs;
                    break;
                }
                else{
                    movePieceD(i,i2);
                    pts = i8min(pts,i8maxAI(depth-1));
                    movePieceD(i2,i);
                }
                if(i2 > 54 || (i2 & 7) == 7){
                    break;
                }
            }
        }
        if((i & 7) != 0){
            for(int i2 = i + 7;board[i2] < 'A' || board[i2] > 'Z';i2+=7){
                if(board[i2] > 'a' && board[i2] < 'z'){
                    char lpts = blackPcsToScore(board[i2]);
                    char tpcs = board[i2];
                    capturePieceD(i,i2);
                    pts = i8min(pts,i8maxAI(depth-1) + lpts);
                    board[i] = board[i2];
                    board[i2] = tpcs;
                    break;
                }
                else{
                    movePieceD(i,i2);
                    pts = i8min(pts,i8maxAI(depth-1));
                    movePieceD(i2,i);
                }
                if(i2 > 54 || (i2 & 7) == 0){
                    break;
                }
            }
        }
    }
    return pts;
}

i8 i8minAIstraight0(u8 i,i8 pts){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'A' || board[i2] > 'Z';i2++){
            if(board[i2] > 'a' && board[i2] < 'z'){
                pts = i8min(pts,blackPcsToScore(board[i2]));
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
                pts = i8min(pts,blackPcsToScore(board[i2]));
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
                pts = i8min(pts,blackPcsToScore(board[i2]));
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
                pts = i8min(pts,blackPcsToScore(board[i2]));
                break;
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return pts;
}

i8 i8minAIstraight(u8 i,u8 depth,i8 pts){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'A' || board[i2] > 'Z';i2++){
            if(board[i2] > 'a' && board[i2] < 'z'){
                char lpts = blackPcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                pts = i8min(pts,i8maxAI(depth-1) + lpts);
                board[i] = board[i2];
                board[i2] = tpcs;
                break;
            }
            else{
                movePieceD(i,i2);
                pts = i8min(pts,i8maxAI(depth-1));
                movePieceD(i2,i);
            }
            if((i2 & 7) == 7){
                break;
            }
        }
    }
    if((i & 7) != 0){
        for(int i2 = i - 1;board[i2] < 'A' || board[i2] > 'Z';i2--){
            if(board[i2] > 'a' && board[i2] < 'z'){
                char lpts = blackPcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                pts = i8min(pts,i8maxAI(depth-1) + lpts);
                board[i] = board[i2];
                board[i2] = tpcs;
                break;
            }
            else{
                movePieceD(i,i2);
                pts = i8min(pts,i8maxAI(depth-1));
                movePieceD(i2,i);
            }
            if((i2 & 7) == 0){
                break;
            }
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'A' || board[i2] > 'Z';i2-=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
                char lpts = blackPcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                pts = i8min(pts,i8maxAI(depth-1) + lpts);
                board[i] = board[i2];
                board[i2] = tpcs;
                break;
            }
            else{
                movePieceD(i,i2);
                pts = i8min(pts,i8maxAI(depth-1));
                movePieceD(i2,i);
            }
            if(i2 < 8){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'A' || board[i2] > 'Z';i2+=8){
            if(board[i2] > 'a' && board[i2] < 'z'){
                char lpts = blackPcsToScore(board[i2]);
                char tpcs = board[i2];
                capturePieceD(i,i2);
                pts = i8min(pts,i8maxAI(depth-1) + lpts);
                board[i] = board[i2];
                board[i2] = tpcs;
                break;
            }
            else{
                movePieceD(i,i2);
                pts = i8min(pts,i8maxAI(depth-1));
                movePieceD(i2,i);
            }
            if(i2 > 54){
                break;
            }
        }
    }
    return pts;
}

void i8minAIknight(i8 i,i8 dst,u8 depth,i8 *pts){
    if(board[i+dst] < 'z' && board[i+dst] > 'a'){
        char lpts = blackPcsToScore(board[i+dst]);
        char tpcs = board[i+dst];
        capturePieceD(i,i+dst);
        *pts = i8min(*pts,i8maxAI(depth-1) + lpts);
        board[i] = board[i+dst];
        board[i+dst] = tpcs;
    }
    else if(board[i+dst] == '*'){
        movePieceD(i,i+dst);
        *pts = i8min(*pts,i8maxAI(depth-1));
        movePieceD(i+dst,i);
    }
}

i8 i8minAI(u8 depth){
    i8 pts = 127;
    if(!depth){
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 0){
                    pts = i8min(pts,blackPcsToScore(board[i-9]));
                }
                if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 7){
                    pts = i8min(pts,blackPcsToScore(board[i-7]));
                }
                break;
            case 'N':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy > 1){
                        pts = i8min(pts,blackPcsToScore(board[i-6]));
                    }
                    if(sy < 6){
                        pts = i8min(pts,blackPcsToScore(board[i-10]));
                    }
                    if(sx > 1){
                        if(sy > 0){
                            pts = i8min(pts,blackPcsToScore(board[i-15]));
                        }
                        if(sy < 7){
                            pts = i8min(pts,blackPcsToScore(board[i-17]));
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        pts = i8min(pts,blackPcsToScore(board[i+6]));
                    }
                    if(sy < 6){
                        pts = i8min(pts,blackPcsToScore(board[i+10]));
                    }
                    if(sx < 6){
                        if(sy > 0){
                            pts = i8min(pts,blackPcsToScore(board[i+15]));
                        }
                        if(sy < 7){
                            pts = i8min(pts,blackPcsToScore(board[i+17]));
                        }
                    }
                }
                break;
                }
            case 'B':
                pts = i8minAIdiagonal0(i,pts);
                break;
            case 'R':
                pts = i8minAIstraight0(i,pts);
                break;
            case 'Q':
                pts = i8minAIdiagonal0(i,pts);
                pts = i8minAIstraight0(i,pts);
                break;
            case 'K':
                for(int i2 = -1;i2 < 2;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i+i2+i3>=0&&i+i2+i3<64){
                            if(board[i+i2+i3] < 'z' && board[i+i2+i3] > 'a'){
                                pts = i8min(pts,blackPcsToScore(board[i+i2+i3]));
                            }
                        }
                    }
                }
            break;
            }
        }
    }
    else{
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'P':
                if(board[i-8] == '*'){
                    movePieceD(i,i-8);
                    pts = i8min(pts,i8maxAI(depth-1));
                    movePieceD(i-8,i);
                    if(i > 47 && i < 56 && board[i-16] == '*'){
                        movePieceD(i,i-16);
                        pts = i8min(pts,i8maxAI(depth-1));
                        movePieceD(i-16,i);
                    }
                }
                if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 7){
                    char lpts = blackPcsToScore(board[i-9]);
                    char tpcs = board[i-9];
                    capturePieceD(i,i-9);
                    pts = i8min(pts,i8maxAI(0,depth-1) + lpts);
                    board[i] = board[i-9];
                    board[i-9] = tpcs;
                }
                if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 0){
                    char lpts = blackPcsToScore(board[i-7]);
                    char tpcs = board[i-7];
                    capturePieceD(i,i-7);
                    pts = i8min(pts,i8maxAI(0,depth-1) + lpts);
                    board[i] = board[i-7];
                    board[i-7] = tpcs;
                }
                break;
            case 'N':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy > 1){
                        i8minAIknight(i,-6,depth,&pts);
                    }
                    if(sy < 6){{
                        i8minAIknight(i,-10,depth,&pts);
                    }
                    if(sx > 1){
                        if(sy > 0){
                            i8minAIknight(i,-15,depth,&pts);
                        }
                        if(sy < 7){
                            i8minAIknight(i,-17,depth,&pts);
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 1){
                        i8minAIknight(i,6,depth,&pts);
                    }
                    if(sy < 6){
                        i8minAIknight(i,10,depth,&pts);
                    }
                    if(sx < 6){
                        if(sy > 0){
                            i8minAIknight(i,15,depth,&pts);
                        }
                        if(sy < 7){
                            i8minAIknight(i,17,depth,&pts);
                        }
                    }
                }
                break;
                }
            case 'B':
                pts = i8minAIdiagonal(i,depth,pts);
                break;
            case 'R':
                pts = i8minAIstraight(i,depth,pts);
                break;
            case 'Q':
                pts = i8minAIdiagonal(i,depth,pts);
                pts = i8minAIstraight(i,depth,pts);
                break;
            case 'K':
                for(int i2 = -1;i2 < 2;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i+i2+i3>=0&&i+i2+i3<64){
                            if(board[i+i2+i3] < 'z' && board[i+i2+i3] > 'a'){
                                char lpts = blackPcsToScore(board[i+i2+i3]);
                                char tpcs = board[i+i2+i3];
                                capturePieceD(i,i+i2+i3);
                                pts = i8min(pts,i8maxAI(depth-1) + lpts);
                                board[i] = board[i+i2+i3];
                                board[i+i2+i3] = tpcs;
                            }
                            else if(board[i+i2+i3] == '*'){
                                movePieceD(i,i+i2+i3);
                                pts = i8min(pts,i8maxAI(depth-1));
                                movePieceD(i+i2+i3,i);
                            }
                        }
                    }
                }
                break;
                }
            }
        }
    }
    if(pts==127){
        return 0;
    }
    return pts;
}

void printMove(u8 src,u8 dst,u8 pts){
    printf("%c",(src&7)+'A');
    printf("%c",(src>>3)+'0');
    printf("%c",'-');
    printf("%c",(dst&7)+'A');
    printf("%c = ",(dst>>3)+'0');
    printf("%hhi\n",pts);
}

void captureAI(u8 src,u8 dst,i8 *pts,u8 *movC,u8 depth){
    char tpcs = board[dst];
    capturePieceD(src,dst);
    i8 tpts = i8minAI(depth);
    board[src] = board[dst];
    board[dst] = tpcs;
    tpts += whitePcsToScore(tpcs);
    if(depth == DEPTH){
        printMove(src,dst,tpts);
    }
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

void moveAI(u8 src,u8 dst,i8 *pts,u8 *movC,u8 depth){
    movePieceD(src,dst);
    i8 tpts = i8minAI(depth);
    movePieceD(dst,src);
    if(depth == DEPTH){
        printMove(src,dst,tpts);
    }
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

void moveCaptureAI(u8 src,u8 dst,i8 *pts,u8 *movC,u8 depth){
    if(board[dst] > 'A' && board[dst] < 'Z'){
        captureAI(src,dst,pts,movC,depth);
    }
    else if(board[dst] == '*'){
        moveAI(src,dst,pts,movC,depth);
    }
}

void straightAI(u8 i,i8 *pts,u8 *movC,u8 depth){
    if((i & 7) != 7){
        for(int i2 = i + 1;board[i2] < 'a' || board[i2] > 'z';i2++){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureAI(i,i2,pts,movC,depth);
                break;
            }
            else{
                moveAI(i,i2,pts,movC,depth);
            }
            if((i2 & 7) == 7){
                break;
            }
        }
    }
    if((i & 7) != 0){
        for(int i2 = i - 1;board[i2] < 'a' || board[i2] > 'z';i2--){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureAI(i,i2,pts,movC,depth);
                break;
            }
            else{
                moveAI(i,i2,pts,movC,depth);
            }
            if((i2 & 7) == 0){
                break;
            }
        }
    }
    if(i > 7){
        for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureAI(i,i2,pts,movC,depth);
                break;
            }
            else{
                moveAI(i,i2,pts,movC,depth);
            }
            if(i2 < 7){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureAI(i,i2,pts,movC,depth);
                break;
            }
            else{
                moveAI(i,i2,pts,movC,depth);
            }
            if(i2 > 55){
                break;
            }
        }
    }
}

void diagonalAI(u8 i,i8 *pts,u8 *movC,u8 depth){
    if(i > 7){
        if((i & 7) != 7){
            for(int i2 = i - 9;board[i2] < 'a' || board[i2] > 'z';i2-=9){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureAI(i,i2,pts,movC,depth);
                    break;
                }
                else{
                    moveAI(i,i2,pts,movC,depth);
                }
                if(i2 < 0 || (i2 & 7) == 7){
                    break;
                }
            }
        }
        if((i & 7) != 0){
            for(int i2 = i - 7;board[i2] < 'a' || board[i2] > 'z';i2-=7){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureAI(i,i2,pts,movC,depth);
                    break;
                }
                else{
                    moveAI(i,i2,pts,movC,depth);
                }
                if(i2 < 0 || (i2 & 7) == 0){
                    break;
                }
            }
        }
    }
    if(i < 55){
        if((i & 7) != 7){
            for(int i2 = i + 9;board[i2] < 'a' || board[i2] > 'z';i2+=9){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureAI(i,i2,pts,movC,depth);
                    break;
                }
                else{
                    moveAI(i,i2,pts,movC,depth);
                }
                if(i2 > 55 || (i2 & 7) == 7){
                    break;
                }
            }
        }
        if((i & 7) != 0){
            for(int i2 = i + 7;board[i2] < 'a' || board[i2] > 'z';i2+=7){
                if(board[i2] > 'A' && board[i2] < 'Z'){
                    captureAI(i,i2,pts,movC,depth);
                    break;
                }
                else{
                    moveAI(i,i2,pts,movC,depth);
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
                movePiece(SX,SY,DX,DY);
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
        u8 movC;
        i8 pts;
        movC = 0;
        pts = -128;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(board[i+8] == '*'){
                    moveAI(i,i+8,&pts,&movC,DEPTH);
                    if(i > 7 && i < 16){
                        if(board[i+16] == '*'){
                            moveAI(i,i+16,&pts,&movC,DEPTH);
                        }
                    }
                }
                if(board[i+9] > 'A' && board[i+9] < 'Z' && (i & 7) != 7){
                    captureAI(i,i+9,&pts,&movC,DEPTH);
                }
                if(board[i+7] > 'A' && board[i+7] < 'Z' && (i & 7) != 0){
                    captureAI(i,i+7,&pts,&movC,DEPTH);
                }
                break;
            case 'n':{
                u8 sx = i >> 3;
                u8 sy = i & 7;
                if(sx > 0){
                    if(sy < 6){
                        moveCaptureAI(i,i-6,&pts,&movC,DEPTH);
                    }
                    if(sy > 1){
                        moveCaptureAI(i,i-10,&pts,&movC,DEPTH);
                    }
                    if(sx > 1){
                        if(sy < 7){
                            moveCaptureAI(i,i-15,&pts,&movC,DEPTH);
                        }
                        if(sy > 0){
                            moveCaptureAI(i,i-17,&pts,&movC,DEPTH);
                        }
                    }
                }
                if(sx < 7){
                    if(sy > 6){
                        moveCaptureAI(i,i+6,&pts,&movC,DEPTH);

                    }
                    if(sy < 1){
                        moveCaptureAI(i,i+10,&pts,&movC,DEPTH);

                    }
                    if(sx < 6){
                        if(sy < 7){
                            moveCaptureAI(i,i+15,&pts,&movC,DEPTH);
                        }
                        if(sy > 0){
                            moveCaptureAI(i,i+17,&pts,&movC,DEPTH);
                        }
                    }
                }
                break;
            }
            case 'b':
                diagonalAI(i,&pts,&movC,DEPTH);
                break;
            case 'r':
                straightAI(i,&pts,&movC,DEPTH);
                break;
            case 'q':
                diagonalAI(i,&pts,&movC,DEPTH);
                straightAI(i,&pts,&movC,DEPTH);
                break;
            case 'k':
                for(int i2 = -1;i2 < 2;i2++){
                    for(int i3 = -8;i3 < 9;i3+=8){
                        if(i2+i3!=0&&i+i2+i3>0&&i+i2+i3<64){
                            moveCaptureAI(i,i+i2+i3,&pts,&movC,DEPTH);
                        }
                    }
                }
                break;
            }
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
