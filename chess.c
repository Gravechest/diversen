#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <intrin.h>

#define DEPTH 4

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

i8 minAI(u8 depth);

void removeMov(CHESSMOVE *arr,u8 item,u8 *counter){
    for(int i = item;i < *counter;i++){
        arr[item] = arr[item+1];
    }
    --*counter;
}

i8 blackPcsToScore(u8 piece){
    switch(piece){
    case 'p':
        return -1;
    case 'b':
        return -3;
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
        return 3;
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

i8 maxAI(u8 depth){
    i8 pts = -128;
    for(int i = 0;i < 64;i++){
        switch(board[i]){
        case 'p':
            if(!depth){
                if(board[i+9] > 'a' && board[i+9] < 'z' && (i & 7) != 7){
                    pts = max(pts,whitePcsToScore(board[i+9]));
                }
                if(board[i+7] > 'a' && board[i+7] < 'z' && (i & 7) != 0){
                    pts = max(pts,whitePcsToScore(board[i+7]));
                }
            }
            else{
                if(board[i+8] == '*'){
                    movePieceD(i,i+8);
                    pts = max(pts,minAI(depth-1));
                    movePieceD(i+8,i);
                    if(i > 7 && i < 16 && board[i+16] == '*'){
                        movePieceD(i,i+16);
                        pts = max(pts,minAI(depth-1));
                        movePieceD(i+16,i);
                    }
                }
                if(board[i+9] > 'a' && board[i+9] < 'z' && (i & 7) != 7){
                    char lpts = whitePcsToScore(board[i+9]);
                    char tpcs = board[i+9];
                    capturePieceD(i,i+9);
                    pts = max(pts,minAI(depth-1) + lpts);
                    board[i] = board[i+9];
                    board[i+9] = tpcs;
                }
                if(board[i+7] > 'a' && board[i+7] < 'z' && (i & 7) != 0){
                    char lpts = whitePcsToScore(board[i+7]);
                    char tpcs = board[i+7];
                    capturePieceD(i,i+7);
                    pts = max(pts,minAI(depth-1) + lpts);
                    board[i] = board[i+7];
                    board[i+7] = tpcs;
                }
            }
            break;
       case 'n':
            u8 sx = i >> 3;
            u8 sy = i & 7;
            if(sx > 0){
                if(sy > 1){
                    if(!depth){
                        pts = max(pts,whitePcsToScore(board[i-6]));
                    }
                    else{
                        if(board[i-6] > 'Z' && board[i-6] < 'A'){
                            char lpts = whitePcsToScore(board[i-6]);
                            char tpcs = board[i-6];
                            capturePieceD(i,i-6);
                            pts = max(pts,minAI(depth-1) + lpts);
                            board[i] = board[i-6];
                            board[i-6] = tpcs;
                        }
                        else if(board[i-6] == '*'){
                            movePieceD(i,i-6);
                            pts = max(pts,minAI(depth-1));
                            movePieceD(i-6,i);
                        }
                    }
                }
                if(sy < 6){
                    if(!depth){
                        pts = max(pts,whitePcsToScore(board[i-10]));
                    }
                    else{
                        if(board[i-10] > 'Z' && board[i-10] < 'A'){
                            char lpts = whitePcsToScore(board[i-10]);
                            char tpcs = board[i-10];
                            capturePieceD(i,i-10);
                            pts = max(pts,minAI(depth-1) + lpts);
                            board[i] = board[i-10];
                            board[i-10] = tpcs;
                        }
                        else if(board[i-10] == '*'){
                            movePieceD(i,i-10);
                            pts = max(pts,minAI(depth-1));
                            movePieceD(i-10,i);
                        }
                    }
                }
                if(sx > 1){
                    if(sy > 0){
                        if(!depth){
                            pts = max(pts,whitePcsToScore(board[i-15]));
                        }
                        else{
                            if(board[i-15] > 'Z' && board[i-15] < 'A'){
                                char lpts = whitePcsToScore(board[i-15]);
                                char tpcs = board[i-15];
                                capturePieceD(i,i-15);
                                pts = max(pts,minAI(depth-1) + lpts);
                                board[i] = board[i-15];
                                board[i-15] = tpcs;
                            }
                            else if(board[i-15] == '*'){
                                movePieceD(i,i-15);
                                pts = max(pts,minAI(depth-1));
                                movePieceD(i-15,i);
                            }
                        }
                    }
                    if(sy < 7){
                        if(!depth){
                            pts = max(pts,whitePcsToScore(board[i-17]));
                        }
                        else{
                            if(board[i-17] > 'Z' && board[i-17] < 'A'){
                                char lpts = whitePcsToScore(board[i-17]);
                                char tpcs = board[i-17];
                                capturePieceD(i,i-17);
                                pts = max(pts,minAI(depth-1) + lpts);
                                board[i] = board[i-17];
                                board[i-17] = tpcs;
                            }
                            else if(board[i-17] == '*'){
                                movePieceD(i,i-17);
                                pts = max(pts,minAI(depth-1));
                                movePieceD(i-17,i);
                            }
                        }
                    }
                }
            }
            if(sx < 7){
                if(sy > 1){
                    if(!depth){
                        pts = max(pts,whitePcsToScore(board[i+6]));
                    }
                    else{
                        if(board[i+6] > 'Z' && board[i+6] < 'A'){
                            char lpts = whitePcsToScore(board[i+6]);
                            char tpcs = board[i+6];
                            capturePieceD(i,i+6);
                            pts = max(pts,minAI(depth-1) + lpts);
                            board[i] = board[i+6];
                            board[i+6] = tpcs;
                        }
                        else if(board[i+6] == '*'){
                            movePieceD(i,i+6);
                            pts = max(pts,minAI(depth-1));
                            movePieceD(i+6,i);
                        }
                    }
                }
                if(sy < 6){
                    if(!depth){
                        pts = max(pts,whitePcsToScore(board[i+10]));
                    }
                    else{
                        if(board[i+10] > 'Z' && board[i+10] < 'A'){
                            char lpts = whitePcsToScore(board[i+10]);
                            char tpcs = board[i+10];
                            capturePieceD(i,i+10);
                            pts = max(pts,minAI(depth-1) + lpts);
                            board[i] = board[i+10];
                            board[i+10] = tpcs;
                        }
                        else if(board[i+10] == '*'){
                            movePieceD(i,i+10);
                            pts = max(pts,minAI(depth-1));
                            movePieceD(i+10,i);
                        }
                    }
                }
                if(sx < 6){
                    if(sy > 0){
                        if(!depth){
                            pts = max(pts,whitePcsToScore(board[i+15]));
                        }
                        else{
                            if(board[i+15] > 'Z' && board[i+15] < 'A'){
                                char lpts = whitePcsToScore(board[i+15]);
                                char tpcs = board[i+15];
                                capturePieceD(i,i+15);
                                pts = max(pts,minAI(depth-1) + lpts);
                                board[i] = board[i+15];
                                board[i+15] = tpcs;
                            }
                            else if(board[i+15] == '*'){
                                movePieceD(i,i+15);
                                pts = max(pts,minAI(depth-1));
                                movePieceD(i+15,i);
                            }
                        }
                    }
                    if(sy < 7){
                        if(!depth){
                            pts = max(pts,whitePcsToScore(board[i+17]));
                        }
                        else{
                            if(board[i+17] > 'Z' && board[i+17] < 'A'){
                                char lpts = whitePcsToScore(board[i+17]);
                                char tpcs = board[i+17];
                                capturePieceD(i,i+17);
                                pts = max(pts,minAI(depth-1) + lpts);
                                board[i] = board[i+17];
                                board[i+17] = tpcs;
                            }
                            else if(board[i+17] == '*'){
                                movePieceD(i,i+17);
                                pts = max(pts,minAI(depth-1));
                                movePieceD(i+17,i);
                            }
                        }
                    }
                }
            }
            break;
        case 'b':
            break;
            if(i > 7){
                if((i & 7) != 7){
                    if(!depth){
                        for(int i2 = i - 9;board[i2] < 'a' || board[i2] > 'z';i2-=9){
                            if(board[i2] > 'A' && board[i2] < 'Z'){
                                pts = max(pts,whitePcsToScore(board[i2]));
                                break;
                            }
                            if(i2 < 0 || (i2 & 7) == 0){
                                break;
                            }
                        }
                    }
                    else{
                        for(int i2 = i - 9;board[i2] < 'a' || board[i2] > 'z';i2-=9){
                            if(board[i2] > 'A' && board[i2] < 'Z'){
                                char lpts = whitePcsToScore(board[i2]);
                                char tpcs = board[i2];
                                capturePieceD(i,i2);
                                pts = max(pts,minAI(depth-1) + lpts);
                                board[i] = board[i2];
                                board[i2] = tpcs;
                                break;
                            }
                            else{
                                movePieceD(i,i2);
                                pts = max(pts,minAI(depth-1));
                                movePieceD(i2,i);
                            }
                            if(i2 < 0 || (i2 & 7) == 0){
                                break;
                            }
                        }
                    }
                }
                if((i & 7) != 0){
                    if(!depth){
                        for(int i2 = i - 7;board[i2] < 'a' || board[i2] > 'z';i2-=7){
                            if(board[i2] > 'A' && board[i2] < 'Z'){
                                pts = max(pts,whitePcsToScore(board[i2]));
                                break;
                            }
                            if(i2 < 0 || (i2 & 7) == 7){
                                break;
                            }
                        }
                    }
                    else{
                        for(int i2 = i - 7;board[i2] < 'a' || board[i2] > 'z';i2-=7){
                            if(board[i2] > 'A' && board[i2] < 'Z'){
                                char lpts = whitePcsToScore(board[i2]);
                                char tpcs = board[i2];
                                capturePieceD(i,i2);
                                pts = max(pts,minAI(depth-1) + lpts);
                                board[i] = board[i2];
                                board[i2] = tpcs;
                                break;
                            }
                            else{
                                movePieceD(i,i2);
                                pts = max(pts,minAI(depth-1));
                                movePieceD(i2,i);
                            }
                            if(i2 < 0 || (i2 & 7) == 7){
                                break;
                            }
                        }
                    }
                }
            }
            if(i < 55){
                if((i & 7) != 7){
                    if(!depth){
                        for(int i2 = i + 9;board[i2] < 'a' || board[i2] > 'z';i2+=9){
                            if(board[i2] > 'A' && board[i2] < 'Z'){
                                pts = max(pts,whitePcsToScore(board[i2]));
                                break;
                            }
                            if(i2 > 63 || (i2 & 7) == 7){
                                break;
                            }
                        }
                    }
                    else{
                        for(int i2 = i + 9;board[i2] < 'a' || board[i2] > 'z';i2+=9){
                            if(board[i2] > 'A' && board[i2] < 'Z'){
                                char lpts = whitePcsToScore(board[i2]);
                                char tpcs = board[i2];
                                capturePieceD(i,i2);
                                pts = max(pts,minAI(depth-1) + lpts);
                                board[i] = board[i2];
                                board[i2] = tpcs;
                                break;
                            }
                            else{
                                movePieceD(i,i2);
                                pts = max(pts,minAI(depth-1));
                                movePieceD(i2,i);
                            }
                            if(i2 > 63 || (i2 & 7) == 7){
                                break;
                            }
                        }
                    }
                }
                if((i & 7) != 0){
                    if(!depth){
                        for(int i2 = i + 7;board[i2] < 'a' || board[i2] > 'z';i2+=7){
                            if(board[i2] > 'A' && board[i2] < 'Z'){
                                pts = max(pts,whitePcsToScore(board[i2]));
                                break;
                            }
                            if(i2 > 63  || (i2 & 7) == 0){
                                break;
                            }
                        }
                    }
                    else{
                        for(int i2 = i + 7;board[i2] < 'a' || board[i2] > 'z';i2+=7){
                            if(board[i2] > 'A' && board[i2] < 'Z'){
                                char lpts = whitePcsToScore(board[i2]);
                                char tpcs = board[i2];
                                capturePieceD(i,i2);
                                pts = max(pts,minAI(depth-1) + lpts);
                                board[i] = board[i2];
                                board[i2] = tpcs;
                                break;
                            }
                            else{
                                movePieceD(i,i2);
                                pts = max(pts,minAI(depth-1));
                                movePieceD(i2,i);
                            }
                            if(i2 > 63  || (i2 & 7) == 0){
                                break;
                            }
                        }
                    }
                }
            }
            break;
        case 'r':
            if((i & 7) != 7){
                if(!depth){
                    for(int i2 = i + 1;board[i2] < 'a' || board[i2] > 'z';i2++){
                        if(board[i2] > 'A' && board[i2] < 'Z'){
                            pts = max(pts,whitePcsToScore(board[i2]));
                            break;
                        }
                        if((i2 & 7) == 7){
                            break;
                        }
                    }
                }
                else{
                    for(int i2 = i + 1;board[i2] < 'a' || board[i2] > 'z';i2++){
                        if(board[i2] > 'A' && board[i2] < 'Z'){
                            char lpts = whitePcsToScore(board[i2]);
                            char tpcs = board[i2];
                            capturePieceD(i,i2);
                            pts = max(pts,minAI(depth-1) + lpts);
                            board[i] = board[i2];
                            board[i2] = tpcs;
                            break;
                        }
                        else{
                            movePieceD(i,i2);
                            pts = max(pts,minAI(depth-1));
                            movePieceD(i2,i);
                        }
                        if((i2 & 7) == 7){
                            break;
                        }
                    }
                }
            }
            if((i & 7) != 0){
                if(!depth){
                    for(int i2 = i - 1;board[i2] < 'a' || board[i2] > 'z';i2--){
                        if(board[i2] > 'A' && board[i2] < 'Z'){
                            pts = max(pts,whitePcsToScore(board[i2]));
                            break;
                        }
                        if((i2 & 7) == 0){
                            break;
                        }
                    }
                }
                else{
                    for(int i2 = i - 1;board[i2] < 'a' || board[i2] > 'z';i2--){
                        if(board[i2] > 'A' && board[i2] < 'Z'){
                            char lpts = whitePcsToScore(board[i2]);
                            char tpcs = board[i2];
                            capturePieceD(i,i2);
                            pts = max(pts,minAI(depth-1) + lpts);
                            board[i] = board[i2];
                            board[i2] = tpcs;
                            break;
                        }
                        else{
                            movePieceD(i,i2);
                            pts = max(pts,minAI(depth-1));
                            movePieceD(i2,i);
                        }
                        if((i2 & 7) == 0){
                            break;
                        }
                    }
                }
            }
            if(i > 7){
                if(!depth){
                    for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
                        if(board[i2] > 'A' && board[i2] < 'Z'){
                            pts = max(pts,whitePcsToScore(board[i2]));
                            break;
                        }
                        if(i2 < 7){
                            break;
                        }
                    }
                }
                else{
                    for(int i2 = i - 8;board[i2] < 'a' || board[i2] > 'z';i2-=8){
                        if(board[i2] > 'A' && board[i2] < 'Z'){
                            char lpts = whitePcsToScore(board[i2]);
                            char tpcs = board[i2];
                            capturePieceD(i,i2);
                            pts = max(pts,minAI(depth-1) + lpts);
                            board[i] = board[i2];
                            board[i2] = tpcs;
                            break;
                        }
                        else{
                            movePieceD(i,i2);
                            pts = max(pts,minAI(depth-1));
                            movePieceD(i2,i);
                        }
                        if(i2 < 7){
                            break;
                        }
                    }
                }
            }
            if(i < 55){
                if(!depth){
                    for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
                        if(board[i2] > 'A' && board[i2] < 'Z'){
                            pts = max(pts,whitePcsToScore(board[i2]));
                            break;
                        }
                        if(i2 > 55){
                            break;
                        }
                    }
                }
                else{
                    for(int i2 = i + 8;board[i2] < 'a' || board[i2] > 'z';i2+=8){
                        if(board[i2] > 'A' && board[i2] < 'Z'){
                            char lpts = whitePcsToScore(board[i2]);
                            char tpcs = board[i2];
                            capturePieceD(i,i2);
                            pts = max(pts,minAI(depth-1) + lpts);
                            board[i] = board[i2];
                            board[i2] = tpcs;
                            break;
                        }
                        else{
                            movePieceD(i,i2);
                            pts = max(pts,minAI(depth-1));
                            movePieceD(i2,i);
                        }
                        if(i2 > 55){
                            break;
                        }
                    }
                }
            }
            break;
        }
    }
    return pts;
}

i8 minAI(u8 depth){
    i8 pts = 127;
    for(int i = 0;i < 64;i++){
        switch(board[i]){
        case 'P':
            if(!depth){
                if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 7){
                    pts = min(pts,blackPcsToScore(board[i-9]));
                }
                if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 0){
                    pts = min(pts,blackPcsToScore(board[i-7]));
                }
            }
            else{
                if(board[i-8] == '*'){
                    movePieceD(i,i-8);
                    pts = min(pts,maxAI(depth-1));
                    movePieceD(i-8,i);
                    if(i > 47 && i < 56 && board[i-16] == '*'){
                        movePieceD(i,i-16);
                        pts = min(pts,maxAI(depth-1));
                        movePieceD(i-16,i);
                    }
                }
                if(board[i-9] > 'a' && board[i-9] < 'z' && (i & 7) != 7){
                    char lpts = blackPcsToScore(board[i-9]);
                    char tpcs = board[i-9];
                    capturePieceD(i,i-9);
                    pts = min(pts,maxAI(0,depth-1) + lpts);
                    board[i] = board[i-9];
                    board[i-9] = tpcs;
                }
                if(board[i-7] > 'a' && board[i-7] < 'z' && (i & 7) != 0){
                    char lpts = blackPcsToScore(board[i-7]);
                    char tpcs = board[i-7];
                    capturePieceD(i,i-7);
                    pts = min(pts,maxAI(0,depth-1) + lpts);
                    board[i] = board[i-7];
                    board[i-7] = tpcs;
                }
            }
            break;
        case 'N':
            u8 sx = i >> 3;
            u8 sy = i & 7;
            if(sx > 0){
                if(sy > 1){
                    if(!depth){
                        pts = min(pts,blackPcsToScore(board[i-6]));
                    }
                    else{
                        if(board[i-6] > 'Z' && board[i-6] < 'A'){
                            char lpts = blackPcsToScore(board[i-6]);
                            char tpcs = board[i-6];
                            capturePieceD(i,i-6);
                            pts = min(pts,maxAI(depth-1) + lpts);
                            board[i] = board[i-6];
                            board[i-6] = tpcs;
                        }
                        else if(board[i-6] == '*'){
                            movePieceD(i,i-6);
                            pts = min(pts,maxAI(depth-1));
                            movePieceD(i-6,i);
                        }
                    }
                }
                if(sy < 6){
                    if(!depth){
                        pts = min(pts,blackPcsToScore(board[i-10]));
                    }
                    else{
                        if(board[i-10] > 'Z' && board[i-10] < 'A'){
                            char lpts = blackPcsToScore(board[i-10]);
                            char tpcs = board[i-10];
                            capturePieceD(i,i-10);
                            pts = min(pts,maxAI(depth-1) + lpts);
                            board[i] = board[i-10];
                            board[i-10] = tpcs;
                        }
                        else if(board[i-10] == '*'){
                            movePieceD(i,i-10);
                            pts = min(pts,maxAI(depth-1));
                            movePieceD(i-10,i);
                        }
                    }
                }
                if(sx > 1){
                    if(sy > 0){
                        if(!depth){
                            pts = min(pts,blackPcsToScore(board[i-15]));
                        }
                        else{
                            if(board[i-15] > 'Z' && board[i-15] < 'A'){
                                char lpts = blackPcsToScore(board[i-15]);
                                char tpcs = board[i-15];
                                capturePieceD(i,i-15);
                                pts = min(pts,maxAI(depth-1) + lpts);
                                board[i] = board[i-15];
                                board[i-15] = tpcs;
                            }
                            else if(board[i-15] == '*'){
                                movePieceD(i,i-15);
                                pts = min(pts,maxAI(depth-1));
                                movePieceD(i-15,i);
                            }
                        }
                    }
                    if(sy < 7){
                        if(!depth){
                            pts = min(pts,blackPcsToScore(board[i-17]));
                        }
                        else{
                            if(board[i-17] > 'Z' && board[i-17] < 'A'){
                                char lpts = blackPcsToScore(board[i-17]);
                                char tpcs = board[i-17];
                                capturePieceD(i,i-17);
                                pts = min(pts,maxAI(depth-1) + lpts);
                                board[i] = board[i-17];
                                board[i-17] = tpcs;
                            }
                            else if(board[i-17] == '*'){
                                movePieceD(i,i-17);
                                pts = min(pts,maxAI(depth-1));
                                movePieceD(i-17,i);
                            }
                        }
                    }
                }
            }
            if(sx < 7){
                if(sy > 1){
                    if(!depth){
                        pts = min(pts,blackPcsToScore(board[i+6]));
                    }
                    else{
                        if(board[i+6] > 'Z' && board[i+6] < 'A'){
                            char lpts = blackPcsToScore(board[i+6]);
                            char tpcs = board[i+6];
                            capturePieceD(i,i+6);
                            pts = min(pts,maxAI(depth-1) + lpts);
                            board[i] = board[i+6];
                            board[i+6] = tpcs;
                        }
                        else if(board[i+6] == '*'){
                            movePieceD(i,i+6);
                            pts = min(pts,maxAI(depth-1));
                            movePieceD(i+6,i);
                        }
                    }
                }
                if(sy < 6){
                    if(!depth){
                        pts = min(pts,blackPcsToScore(board[i+10]));
                    }
                    else{
                        if(board[i+10] > 'Z' && board[i+10] < 'A'){
                            char lpts = blackPcsToScore(board[i+10]);
                            char tpcs = board[i+10];
                            capturePieceD(i,i+10);
                            pts = min(pts,maxAI(depth-1) + lpts);
                            board[i] = board[i+10];
                            board[i+10] = tpcs;
                        }
                        else if(board[i+10] == '*'){
                            movePieceD(i,i+10);
                            pts = min(pts,maxAI(depth-1));
                            movePieceD(i+10,i);
                        }
                    }
                }
                if(sx < 6){
                    if(sy > 0){
                        if(!depth){
                            pts = min(pts,blackPcsToScore(board[i+15]));
                        }
                        else{
                            if(board[i+15] > 'Z' && board[i+15] < 'A'){
                                char lpts = blackPcsToScore(board[i-15]);
                                char tpcs = board[i+15];
                                capturePieceD(i,i+15);
                                pts = min(pts,maxAI(depth-1)+ lpts);
                                board[i] = board[i+15];
                                board[i+15] = tpcs;
                            }
                            else if(board[i+15] == '*'){
                                movePieceD(i,i+15);
                                pts = min(pts,maxAI(depth-1));
                                movePieceD(i+15,i);
                            }
                        }
                    }
                    if(sy < 7){
                        if(!depth){
                            pts = min(pts,blackPcsToScore(board[i+17]));
                        }
                        else{
                            if(board[i+17] > 'Z' && board[i+17] < 'A'){
                                char lpts = blackPcsToScore(board[i+17]);
                                char tpcs = board[i+17];
                                capturePieceD(i,i+17);
                                pts = min(pts,maxAI(depth-1) + lpts);
                                board[i] = board[i+17];
                                board[i+17] = tpcs;
                            }
                            else if(board[i+17] == '*'){
                                movePieceD(i,i+17);
                                pts = min(pts,maxAI(depth-1));
                                movePieceD(i+17,i);
                            }
                        }
                    }
                }
            }
            break;
        case 'B':
            if(i > 7){
                if((i & 7) != 7){
                    if(!depth){
                        for(int i2 = i - 9;board[i2] < 'A' || board[i2] > 'Z';i2-=9){
                            if(board[i2] > 'a' && board[i2] < 'z'){
                                pts = min(pts,blackPcsToScore(board[i2]));
                                break;
                            }
                            if(i2 < 0 || (i2 & 7) == 0){
                                break;
                            }
                        }
                    }
                    else{
                        for(int i2 = i - 9;board[i2] < 'A' || board[i2] > 'Z';i2-=9){
                            if(board[i2] > 'a' && board[i2] < 'z'){
                                char lpts = blackPcsToScore(board[i2]);
                                char tpcs = board[i2];
                                capturePieceD(i,i2);
                                pts = min(pts,maxAI(depth-1) + lpts);
                                board[i] = board[i2];
                                board[i2] = tpcs;
                                break;
                            }
                            else{
                                movePieceD(i,i2);
                                pts = min(pts,maxAI(depth-1));
                                movePieceD(i2,i);
                            }
                            if(i2 < 0 || (i2 & 7) == 0){
                                break;
                            }
                        }
                    }
                }
                if((i & 7) != 0){
                    if(!depth){
                        for(int i2 = i - 7;board[i2] < 'A' || board[i2] > 'Z';i2-=7){
                            if(board[i2] > 'a' && board[i2] < 'z'){
                                pts = min(pts,blackPcsToScore(board[i2]));
                                break;
                            }
                            if(i2 < 0 || (i2 & 7) == 7){
                                break;
                            }
                        }
                    }
                    else{
                        for(int i2 = i - 7;board[i2] < 'A' || board[i2] > 'Z';i2-=7){
                            if(board[i2] > 'a' && board[i2] < 'z'){
                                char lpts = blackPcsToScore(board[i2]);
                                char tpcs = board[i2];
                                capturePieceD(i,i2);
                                pts = min(pts,maxAI(depth-1) + lpts);
                                board[i] = board[i2];
                                board[i2] = tpcs;
                                break;
                            }
                            else{
                                movePieceD(i,i2);
                                pts = min(pts,maxAI(depth-1));
                                movePieceD(i2,i);
                            }
                            if(i2 < 0 || (i2 & 7) == 7){
                                break;
                            }
                        }
                    }
                }
            }
            if(i < 55){
                if((i & 7) != 7){
                    if(!depth){
                        for(int i2 = i + 9;board[i2] < 'A' || board[i2] > 'Z';i2+=9){
                            if(board[i2] > 'a' && board[i2] < 'z'){
                                pts = min(pts,blackPcsToScore(board[i2]));
                                break;
                            }
                            if(i2 > 63 || (i2 & 7) == 7){
                                break;
                            }
                        }
                    }
                    else{
                        for(int i2 = i + 9;board[i2] < 'A' || board[i2] > 'Z';i2+=9){
                            if(board[i2] > 'a' && board[i2] < 'z'){
                                char lpts = blackPcsToScore(board[i2]);
                                char tpcs = board[i2];
                                capturePieceD(i,i2);
                                pts = min(pts,maxAI(depth-1) + lpts);
                                board[i] = board[i2];
                                board[i2] = tpcs;
                                break;
                            }
                            else{
                                movePieceD(i,i2);
                                pts = min(pts,maxAI(depth-1));
                                movePieceD(i2,i);
                            }
                            if(i2 > 63 || (i2 & 7) == 7){
                                break;
                            }
                        }
                    }
                }
                if((i & 7) != 0){
                    if(!depth){
                        for(int i2 = i + 7;board[i2] < 'A' || board[i2] > 'Z';i2+=7){
                            if(board[i2] > 'a' && board[i2] < 'z'){
                                pts = min(pts,blackPcsToScore(board[i2]));
                                break;
                            }
                            if(i2 > 63  || (i2 & 7) == 0){
                                break;
                            }
                        }
                    }
                    else{
                        for(int i2 = i + 7;board[i2] < 'A' || board[i2] > 'Z';i2+=7){
                            if(board[i2] > 'a' && board[i2] < 'z'){
                                char lpts = blackPcsToScore(board[i2]);
                                char tpcs = board[i2];
                                capturePieceD(i,i2);
                                pts = min(pts,maxAI(depth-1) + lpts);
                                board[i] = board[i2];
                                board[i2] = tpcs;
                                break;
                            }
                            else{
                                movePieceD(i,i2);
                                pts = min(pts,maxAI(depth-1));
                                movePieceD(i2,i);
                            }
                            if(i2 > 63  || (i2 & 7) == 0){
                                break;
                            }
                        }
                    }
                }
            }
            break;
        case 'R':
            if((i & 7) != 7){
                if(!depth){
                    for(int i2 = i + 1;board[i2] < 'A' || board[i2] > 'Z';i2++){
                        if(board[i2] > 'a' && board[i2] < 'z'){
                            pts = min(pts,blackPcsToScore(board[i2]));
                            break;
                        }
                        if((i2 & 7) != 7){
                            break;
                        }
                    }
                }
                else{
                    for(int i2 = i + 1;board[i2] < 'A' || board[i2] > 'Z';i2++){
                        if(board[i2] > 'a' && board[i2] < 'z'){
                            char lpts = blackPcsToScore(board[i2]);
                            char tpcs = board[i2];
                            capturePieceD(i,i2);
                            pts = min(pts,maxAI(depth-1) + lpts);
                            board[i] = board[i2];
                            board[i2] = tpcs;
                            break;
                        }
                        else{
                            movePieceD(i,i2);
                            pts = min(pts,maxAI(depth-1));
                            movePieceD(i2,i);
                        }
                        if((i2 & 7) != 7){
                            break;
                        }
                    }
                }
            }
            if((i & 7) != 0){
                if(!depth){
                    for(int i2 = i - 1;board[i2] < 'A' || board[i2] > 'Z';i2--){
                        if(board[i2] > 'a' && board[i2] < 'z'){
                            pts = min(pts,blackPcsToScore(board[i2]));
                            break;
                        }
                        if((i2 & 7) != 0){
                            break;
                        }
                    }
                }
                else{
                    for(int i2 = i - 1;board[i2] < 'A' || board[i2] > 'Z';i2--){
                        if(board[i2] > 'a' && board[i2] < 'z'){
                            char lpts = blackPcsToScore(board[i2]);
                            char tpcs = board[i2];
                            capturePieceD(i,i2);
                            pts = min(pts,maxAI(depth-1) + lpts);
                            board[i] = board[i2];
                            board[i2] = tpcs;
                            break;
                        }
                        else{
                            movePieceD(i,i2);
                            pts = min(pts,maxAI(depth-1));
                            movePieceD(i2,i);
                        }
                        if((i2 & 7) != 0){
                            break;
                        }
                    }
                }
            }
            if(i > 7){
                if(!depth){
                    for(int i2 = i - 8;board[i2] < 'A' || board[i2] > 'Z';i2-=8){
                        if(board[i2] > 'a' && board[i2] < 'z'){
                            pts = min(pts,blackPcsToScore(board[i2]));
                            break;
                        }
                        if(i2 > 7){
                            break;
                        }
                    }
                }
                else{
                    for(int i2 = i - 8;board[i2] < 'A' || board[i2] > 'Z';i2-=8){
                        if(board[i2] > 'a' && board[i2] < 'z'){
                            char lpts = blackPcsToScore(board[i2]);
                            char tpcs = board[i2];
                            capturePieceD(i,i2);
                            pts = min(pts,maxAI(depth-1) + lpts);
                            board[i] = board[i2];
                            board[i2] = tpcs;
                            break;
                        }
                        else{
                            movePieceD(i,i2);
                            pts = min(pts,maxAI(depth-1));
                            movePieceD(i2,i);
                        }
                        if(i2 > 7){
                            break;
                        }
                    }
                }
            }
            if(i < 55){
                if(!depth){
                    for(int i2 = i + 7;board[i2] < 'A' || board[i2] > 'Z';i2+=7){
                        if(board[i2] > 'a' && board[i2] < 'z'){
                            pts = min(pts,blackPcsToScore(board[i2]));
                            break;
                        }
                        if(i2 < 55){
                            break;
                        }
                    }
                }
                else{
                    for(int i2 = i + 8;board[i2] < 'A' || board[i2] > 'Z';i2+=8){
                        if(board[i2] > 'a' && board[i2] < 'z'){
                            char lpts = blackPcsToScore(board[i2]);
                            char tpcs = board[i2];
                            capturePieceD(i,i2);
                            pts = min(pts,maxAI(depth-1) + lpts);
                            board[i] = board[i2];
                            board[i2] = tpcs;
                            break;
                        }
                        else{
                            movePieceD(i,i2);
                            pts = min(pts,maxAI(depth-1));
                            movePieceD(i2,i);
                        }
                        if(i2 < 55){
                            break;
                        }
                    }
                }
            }
            break;
        }
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

void captureAI(u8 src,u8 dst,i8 *pts,u8 *movC){
    char tpcs = board[dst];
    capturePieceD(src,dst);
    i8 tpts = minAI(DEPTH);
    board[src] = board[dst];
    board[dst] = tpcs;
    tpts += whitePcsToScore(tpcs);
    printMove(src,dst,tpts);
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
    movePieceD(src,dst);
    i8 tpts = minAI(DEPTH);
    movePieceD(dst,src);
    printMove(src,dst,tpts);
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
            if((i & 7) == 7){
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
            if((i & 7) == 0){
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
            if(i < 7){
                break;
            }
        }
    }
    if(i < 55){
        for(int i2 = i + 7;board[i2] < 'a' || board[i2] > 'z';i2+=7){
            if(board[i2] > 'A' && board[i2] < 'Z'){
                captureAI(i,i2,pts,movC);
                break;
            }
            else{
                moveAI(i,i2,pts,movC);
            }
            if(i > 55){
                break;
            }
        }
    }
}

void diagonalAI(u8 i,i8 *pts,u8 *movC){
    if(i > 7){
        if((i & 7) != 7){
            for(int i2 = i - 9;board[i2] < 'a' || board[i2] > 'z';i2-=9){
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
        if((i & 7) != 0){
            for(int i2 = i - 7;board[i2] < 'a' || board[i2] > 'z';i2-=7){
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
        printf("%c",'a');
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
            break;
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
            if(!checkDiagonal(SX,SY,DX,DY)){continue;}
            if(!checkStraight(SX,SY,DX,DY)){continue;}
            break;
        case 'N': {
            int MNX = abs(SX - DX);
            int MNY = abs(SY - DY);
            if ((MNX == 1 && MNY == 2) || (MNX == 2 && MNY == 1)) {
                movePiece(SX, SY, DX, DY);
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
        master.mov = malloc(sizeof(CHESSMOVE)*64);
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
                    if(sy > 1){
                        moveCaptureAI(i,i-6,&pts,&movC);
                    }
                    if(sy < 6){
                        moveCaptureAI(i,i-10,&pts,&movC);
                    }
                    if(sx > 1){
                        if(sy > 0){
                            moveCaptureAI(i,i-15,&pts,&movC);
                        }
                        if(sy < 7){
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
            }
        }
        u8 mv = __rdtsc()%movC;
        u8 mvsrc = master.mov[mv].src;
        u8 mvdst = master.mov[mv].dst;
        free(master.mov);
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
