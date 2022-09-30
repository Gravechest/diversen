#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <intrin.h>

#pragma warning(disable:4996)

typedef char i8;
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct{
    u8 src;
    u8 dst;
    i8 pts;
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

void removeMov(CHESSMOVE *arr,u8 item,u8 *counter){
    for(int i = item;i < *counter;i++){
        arr[item] = arr[item+1];
    }
    *counter--;
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
            printf("%c",board[i*8+i2]);
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
        switch(board[SX*8+SY]){
        case '*':
            break;
        case 'K': {
            int MNX = abs(SX - DX);
            int MNY = abs(SY - DY);
            if (MNX == 1 || MNY == 1) {
                movePiece(SX, SY, DX, DY);
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
        i8 pts = 0;
        for(int i = 0;i < 64;i++){
            switch(board[i]){
            case 'p':
                if(board[i+8] == '*'){
                    master.mov[movC].src = i;
                    master.mov[movC].dst = i+8;
                    movC++;
                    if(i > 7 && i < 16){
                        if(board[i+16] == '*'){
                            master.mov[movC].src = i;
                            master.mov[movC].dst = i+16;
                            movC++;
                        }
                    }
                }
                break;
            case 'r':
                break;
            }
        }
        u8 mv = __rdtsc()%movC;
        u8 mvsrc = master.mov[mv].src;
        u8 mvdst = master.mov[mv].dst;
        printBoard();
    }
}
