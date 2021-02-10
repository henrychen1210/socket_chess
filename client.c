#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <limits.h>
#include <netdb.h>
#include <errno.h>

#define RED   "\x1B[31m"
#define RESET "\x1B[0m"
#define GREEN "\x1B[32m"

#define YEL  "\033[5;30;43m"
#define REDD "\033[1;30;41m"

#define white_king   0x2654 // ♔
#define white_queen  0x2655 // ♕
#define white_rook   0x2656 // ♖
#define white_bishop 0x2657 // ♗
#define white_knight 0x2658 // ♘
#define white_pawn   0x2659 // ♙
#define black_king   0x265A // ♚
#define black_queen  0x265B // ♛
#define black_rook   0x265C // ♜
#define black_bishop 0x265D // ♝
#define black_knight 0x265E // ♞
#define black_pawn   0x265F // ♟

int END = 0 , sockfd , move_term = 1;
int color[8][8] = { 0 };

char BOARD[64] = {  '7', '8', '9', 'a', 'b', '9', '8', '7',
                    'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c',
                    '0', '0', '0', '0', '0', '0', '0', '0',
                    '0', '0', '0', '0', '0', '0', '0', '0',
                    '0', '0', '0', '0', '0', '0', '0', '0',
                    '0', '0', '0', '0', '0', '0', '0', '0',
                    '6', '6', '6', '6', '6', '6', '6', '6',
                    '1', '2', '3', '4', '5', '3', '2', '1'};

int a = 0, b = 0 ,p ;
char A[8] = "abcdefgh";
char xy[8];
char xyy[8];
int ch;

static wchar_t translate_piece (char char_piece) {
  if (char_piece == '1') return white_rook;
  else if (char_piece == '2') return white_knight;
  else if (char_piece == '3') return white_bishop;
  else if (char_piece == '4') return white_queen;
  else if (char_piece == '5') return white_king;
  else if (char_piece == '6') return white_pawn;
  else if (char_piece == '7') return black_rook;
  else if (char_piece == '8') return black_knight;
  else if (char_piece == '9') return black_bishop;
  else if (char_piece == 'a') return black_queen;
  else if (char_piece == 'b') return black_king;
  else if (char_piece == 'c') return black_pawn;
  else return ' ';
}

void timestamp(char * ubuf) {
    char fmt[64], buf[64];
    struct timeval tv;
    struct tm *tm;

    gettimeofday(&tv, NULL);

    if((tm = localtime(&tv.tv_sec)) != NULL) {
            strftime(fmt, sizeof (fmt), "[%H:%M:%S]", tm);
            snprintf(buf, sizeof (buf), fmt, tv.tv_usec);
            memcpy(ubuf, buf, sizeof (buf));
    }
}

void initial_background() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            color[i][j] = 0;
        }
    }
}

//玩家一棋盤
void printboard_one(char * board) {
    
    system("clear");

    printf("         a   b   c   d   e   f   g   h                          \n");
    printf("       ┌───┬───┬───┬───┬───┬───┬───┬───┐   \n");
    printf("     8 │");
    for (int i = 0; i < 8; i++) {
        if (color[7][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 0 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[7][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 0 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if(color[7][i]==0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 0 + i]));//預設
        }
    }
    printf(" 8 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     7 │");
    for (int i = 0; i < 8; i++) {
        if (color[6][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 1 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[6][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 1 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[6][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 1 + i]));//預設
        }
    }
    printf(" 7 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     6 │");
    for (int i = 0; i < 8; i++) {
        if (color[5][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 2 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[5][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 2 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[5][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 2 + i]));//預設
        }
    }
    printf(" 6 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     5 │");
    for (int i = 0; i < 8; i++) {
        if (color[4][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 3 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[4][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 3 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[4][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 3 + i]));//預設
        }
    }
    printf(" 5 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     4 │");
    for (int i = 0; i < 8; i++) {
        if (color[3][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 4 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[3][i] == 12) {
            printf(REDD"%lc "RESET, translate_piece(board[8 * 4 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[3][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 4 + i]));//預設
        }
    }
    printf(" 4 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     3 │");
    for (int i = 0; i < 8; i++) {
        if (color[2][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 5 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[2][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 5 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[2][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 5 + i]));//預設
        }
    }
    printf(" 3 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     2 │");
    for (int i = 0; i < 8; i++) {
        if (color[1][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 6 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[1][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 6 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[1][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 6 + i]));//預設
        }
    }
    printf(" 2 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     1 │");
    for (int i = 0; i < 8; i++) {
        if (color[0][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 7 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[0][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 7 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[0][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 7 + i]));//預設
        }
    }
    printf(" 8 \n");
    printf("       └───┴───┴───┴───┴───┴───┴───┴───┘   \n");
    printf("         a   b   c   d   e   f   g   h     \n");
    
   
}

//玩家二棋盤
void printboard_two(char * board) {
    
    system("clear");

    printf("         a   b   c   d   e   f   g   h                          \n");
    printf("       ┌───┬───┬───┬───┬───┬───┬───┬───┐   \n");
    printf("     1 │");
    for (int i = 0; i < 8; i++) {
        if (color[7][i] == 14) {
            printf(YEL" %lc │"RESET, translate_piece(board[8 * 7 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[7][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 7 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if(color[7][i]==0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 7 + i]));//預設
        }
    }
    printf(" 1 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     2 │");
    for (int i = 0; i < 8; i++) {
        if (color[6][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 6 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[6][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 6 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[6][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 6 + i]));//預設
        }
    }
    printf(" 2 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     3 │");
    for (int i = 0; i < 8; i++) {
        if (color[5][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 5 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[5][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 5 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[5][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 5 + i]));//預設
        }
    }
    printf(" 3 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     4 │");
    for (int i = 0; i < 8; i++) {
        if (color[4][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 4 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[4][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 4 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[4][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 4 + i]));//預設
        }
    }
    printf(" 4 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     5 │");
    for (int i = 0; i < 8; i++) {
        if (color[3][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 3 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[3][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 3 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[3][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 3 + i]));//預設
        }
    }
    printf(" 5 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     6 │");
    for (int i = 0; i < 8; i++) {
        if (color[2][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 2 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[2][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 2 + i]));//非閃爍的紅底白字
            printf(RESET"│");
           }
        else if (color[2][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 2 + i]));//預設
        }
    }
    printf(" 6 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     7 │");
    for (int i = 0; i < 8; i++) {
        if (color[1][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[1][i] == 12) {
            printf(REDD" %lc "RESET, translate_piece(board[8 * 1 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[1][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 1 + i]));//預設
        }
    }
    printf(" 7 \n");
    printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
    printf("     8 │");
    for (int i = 0; i < 8; i++) {
        if (color[0][i] == 14) {
            printf(YEL" %lc "RESET, translate_piece(board[8 * 0 + i]));//閃爍的黃底白字
            printf(RESET"│");
        }
        else if (color[0][i] == 12) {
               printf(REDD" %lc "RESET, translate_piece(board[8 * 0 + i]));//非閃爍的紅底白字
            printf(RESET"│");
        }
        else if (color[0][i] == 0) {
            printf(RESET" %lc │"RESET, translate_piece(board[8 * 0 + i]));//預設
        }
    }
    printf(" 8 \n");
    printf("       └───┴───┴───┴───┴───┴───┴───┴───┘   \n");
    printf("         a   b   c   d   e   f   g   h     \n");
    
}


void input() {//方向鍵
    if(ch == 91){
        ch = getchar();

        switch (ch){
                case 65://up
                    if (a < 7) {
                        color[a][b] = 0;
                        a++;
                        color[a][b] = 14;
                    }
                    //printf("%d %d\n", a, b);
                    break;
    
                case 66://down
                    if (a > 0) {
                        color[a][b] = 0;
                        a--;
                        color[a][b] = 14;
                    }
                    //printf("%d %d\n", a, b);
                    break;
    
                case 68://left
                    if (b > 0) {
                        color[a][b] = 0;
                        b--;
                        color[a][b] = 14;
                    }
                    //printf("%d %d\n", a, b);
                    break;
    
                case 67://right
                    if (b < 7) {
                        color[a][b] = 0;
                        b++;
                        color[a][b] = 14;
                    }
                    //printf("%d %d\n", a, b);
                    break;
    
                default:
                    break;
        }
    }
}

//判斷白棋座標
void coord_white() {
    int y;
    char yy[2];
    for (int i = 0; i < 8; i++) {
        if (b == i) {
            strncpy(xyy, A + i, 1);
            strcat(xy, xyy);
            for (int j = 0; j < 8; j++) {
                if (a == j) {
                    y = a + 1;
                    sprintf(yy, "%d", y);
                    strcat(xy, yy);
                    break;
                }
            }
        }
    }
}

//判斷黑棋座標
void coord_black() {
    int y;
    char yy[2];
    for (int i = 0; i < 8; i++) {
        if (b == i) {
            strncpy(xyy, A + i, 1);
            strcat(xy, xyy);
            for (int j = 0; j < 8; j++) {
                if (a == j) {
                    y = 9 - a - 1;
                    sprintf(yy, "%d", y);
                    strcat(xy, yy);
                    break;
                }
            }
        }
    }
}


static void print_board_buff(char * board) {
  system("clear");
  setlocale( LC_ALL, "en_US.UTF-8" );
  printf("         a   b   c   d   e   f   g   h                          \n");
  printf("       ┌───┬───┬───┬───┬───┬───┬───┬───┐                        \n");
  printf("     8 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 8      **********超極屌西洋棋**********\n", translate_piece(board[0]), translate_piece(board[1]), translate_piece(board[2]), translate_piece(board[3]), translate_piece(board[4]), translate_piece(board[5]), translate_piece(board[6]), translate_piece(board[7]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     7 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 7      您是超極屌玩家一（黑棋）\n", translate_piece(board[8]), translate_piece(board[9]), translate_piece(board[10]), translate_piece(board[11]), translate_piece(board[12]), translate_piece(board[13]), translate_piece(board[14]), translate_piece(board[15]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     6 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 6      投降 -------------- 'q'\n", translate_piece(board[16]), translate_piece(board[17]), translate_piece(board[18]), translate_piece(board[19]), translate_piece(board[20]), translate_piece(board[21]), translate_piece(board[22]), translate_piece(board[23]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     5 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 5      傳訊息 ------------- 't'\n", translate_piece(board[24]), translate_piece(board[25]), translate_piece(board[26]), translate_piece(board[27]), translate_piece(board[28]), translate_piece(board[29]), translate_piece(board[30]), translate_piece(board[31]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     4 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 4      移動棋子 ----------- 'm'\n", translate_piece(board[32]), translate_piece(board[33]), translate_piece(board[34]), translate_piece(board[35]), translate_piece(board[36]), translate_piece(board[37]), translate_piece(board[38]), translate_piece(board[39]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     3 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 3      選定棋子 ----------- 's'\n", translate_piece(board[40]), translate_piece(board[41]), translate_piece(board[42]), translate_piece(board[43]), translate_piece(board[44]), translate_piece(board[45]), translate_piece(board[46]), translate_piece(board[47]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     2 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 2      \n", translate_piece(board[48]), translate_piece(board[49]), translate_piece(board[50]), translate_piece(board[51]), translate_piece(board[52]), translate_piece(board[53]), translate_piece(board[54]), translate_piece(board[55]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     1 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 1      \n", translate_piece(board[56]), translate_piece(board[57]), translate_piece(board[58]), translate_piece(board[59]), translate_piece(board[60]), translate_piece(board[61]), translate_piece(board[62]), translate_piece(board[63]));
  printf("       └───┴───┴───┴───┴───┴───┴───┴───┘   \n");
  printf("         a   b   c   d   e   f   g   h     \n");
}

static void print_board_buff_inverted(char * board) {
  system("clear");
  setlocale( LC_ALL, "en_US.UTF-8" );
  printf("         a   b   c   d   e   f   g   h                          \n");
  printf("       ┌───┬───┬───┬───┬───┬───┬───┬───┐   \n");
  printf("     1 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 1      **********超極屌西洋棋**********\n", translate_piece(board[56]), translate_piece(board[57]), translate_piece(board[58]), translate_piece(board[59]), translate_piece(board[60]), translate_piece(board[61]), translate_piece(board[62]), translate_piece(board[63]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     2 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 2      您是超極屌玩家二（白棋)\n", translate_piece(board[48]), translate_piece(board[49]), translate_piece(board[50]), translate_piece(board[51]), translate_piece(board[52]), translate_piece(board[53]), translate_piece(board[54]), translate_piece(board[55]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     3 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 3      投降 -------------- 'q'\n", translate_piece(board[40]), translate_piece(board[41]), translate_piece(board[42]), translate_piece(board[43]), translate_piece(board[44]), translate_piece(board[45]), translate_piece(board[46]), translate_piece(board[47]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     4 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 4      傳訊息 ------------- 't'\n", translate_piece(board[32]), translate_piece(board[33]), translate_piece(board[34]), translate_piece(board[35]), translate_piece(board[36]), translate_piece(board[37]), translate_piece(board[38]), translate_piece(board[39]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     5 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 5      移動棋子 ----------- 'm'\n", translate_piece(board[24]), translate_piece(board[25]), translate_piece(board[26]), translate_piece(board[27]), translate_piece(board[28]), translate_piece(board[29]), translate_piece(board[30]), translate_piece(board[31]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     6 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 6      選定棋子 ----------- 's'\n", translate_piece(board[16]), translate_piece(board[17]), translate_piece(board[18]), translate_piece(board[19]), translate_piece(board[20]), translate_piece(board[21]), translate_piece(board[22]), translate_piece(board[23]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     7 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 7      \n", translate_piece(board[8]), translate_piece(board[9]), translate_piece(board[10]), translate_piece(board[11]), translate_piece(board[12]), translate_piece(board[13]), translate_piece(board[14]), translate_piece(board[15]));
  printf("       ├───┼───┼───┼───┼───┼───┼───┼───┤   \n");
  printf("     8 │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ 8 \n", translate_piece(board[0]), translate_piece(board[1]), translate_piece(board[2]), translate_piece(board[3]), translate_piece(board[4]), translate_piece(board[5]), translate_piece(board[6]), translate_piece(board[7]));
  printf("       └───┴───┴───┴───┴───┴───┴───┴───┘   \n");
  printf("         a   b   c   d   e   f   g   h     \n");
}



void * on_signal(void * sockfd) {
    char buffer[128];
    int socket = *(int *)sockfd;
    int * player = (int *)malloc(sizeof(int *));
    char tmp[64];

    bzero(&tmp, 64);
    
    while (1) {
        bzero(buffer, 128);
        
        if (recv(socket, buffer, sizeof(buffer), 0) < 0) {
            perror("ERROR 讀取socket");
            exit(1);
        }
        
        if (buffer[0] == 'w'){
            system("clear");
            if(buffer[1] == '1'){
                if (*player == 1) {
                    printf(RED"輸了～～～～\n");
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf("                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf(" $                                                                                    \n");
                    printf("                     **    **       **********    **********                  !       \n");
                    printf("          !          **    **   **  **      **    **      **        ^                 \n");
                    printf("     @                **   **   **  **********    **********                          \n");
                    printf("                      **  **   **   **      **    **      **                  -       \n");
                    printf("         (             ** **  **    **********    **********    $                     \n");
                    printf("                         **  **     **         **         **                 *        \n");
                    printf("                        ** **       **    ************    **           @              \n");
                    printf("   !                   **          **         **         **                           \n");
                    printf("                       ** **        **     **********     **                          \n");
                    printf("      @        )       **   **       **    ** * ** * **    **   +                     \n");
                    printf("                      **     **     **     **********     **                     >    \n");
                    printf("                     **        **   **      ** ** **      **           ^              \n");
                    printf("        @            **        **   **     **  **  **     **                          \n");
                    printf("                     **        **   **    **   **   **    **             +            \n");
                    printf("              ^      **        **   **         **       * **    =             !       \n");
                    printf("                                    **                   ***                          \n");
                    printf("                 {             }                                           @          \n");
                    printf("       ^                                                   _                          \n");
                    printf("                                         你超爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf("                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf(" $                                                                                    \n");
                    printf("                     **    **       **********    **********                  !       \n");
                    printf("          !          **    **   **  **      **    **      **        ^                 \n");
                    printf("     @                **   **   **  **********    **********                          \n");
                    printf("                      **  **   **   **      **    **      **                  -       \n");
                    printf("         (             ** **  **    **********    **********    $                     \n");
                    printf("                         **  **     **         **         **                 *        \n");
                    printf("                        ** **       **    ************    **           @              \n");
                    printf("   !                   **          **         **         **                           \n");
                    printf("                       ** **        **     **********     **                          \n");
                    printf("      @        )       **   **       **    ** * ** * **    **   +                     \n");
                    printf("                      **     **     **     **********     **                     >    \n");
                    printf("                     **        **   **      ** ** **      **           ^              \n");
                    printf("        @            **        **   **     **  **  **     **                          \n");
                    printf("                     **        **   **    **   **   **    **             +            \n");
                    printf("              ^      **        **   **         **       * **    =             !       \n");
                    printf("                                    **                   ***                          \n");
                    printf("                 {             }                                           @          \n");
                    printf("       ^                                                   _                          \n");
                    printf("                                         你超爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf("                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("afplay /Users/henry/Xcode/chess_client/chess_client/lose.m4a");
                } else {
                    printf("獲勝!!!!\n");
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("            `                    **              **                               ~   \n");
                    printf("    @                            **              **                @                  \n");
                    printf("                                 ******************                                   \n");
                    printf("                                **                         @            ?             \n");
                    printf("                   &     n     **    ************                                     \n");
                    printf("                               **    **        **                                     \n");
                    printf("    @                         **     **        **                                     \n");
                    printf("                              **     ************                +                    \n");
                    printf("            +                **           **             '                            \n");
                    printf("                             **           **                                          \n");
                    printf("                            **      ***************                                   \n");
                    printf("                    @       **      **    **     **                |                  \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("    !                      **       **    **    ***        $                @         \n");
                    printf("                                          **                       /                  \n");
                    printf("                 &                        **                                          \n");
                    printf("                                          **                                          \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("            `                    **              **                               ~   \n");
                    printf("    @                            **              **                @                  \n");
                    printf("                                 ******************                                   \n");
                    printf("                                **                         @            ?             \n");
                    printf("                   &     n     **    ************                                     \n");
                    printf("                               **    **        **                                     \n");
                    printf("    @                         **     **        **                                     \n");
                    printf("                              **     ************                +                    \n");
                    printf("            +                **           **             '                            \n");
                    printf("                             **           **                                          \n");
                    printf("                            **      ***************                                   \n");
                    printf("                    @       **      **    **     **                |                  \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("    !                      **       **    **    ***        $                @         \n");
                    printf("                                          **                       /                  \n");
                    printf("                 &                        **                                          \n");
                    printf("                                          **                                          \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("afplay /Users/henry/Xcode/chess_client/chess_client/Win.m4a");
                }
                END = 1;
            }
            else if(buffer[1] == '2'){
                if (*player == 1) {
                    printf("獲勝!!!!\n");
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("            `                    **              **                               ~   \n");
                    printf("    @                            **              **                @                  \n");
                    printf("                                 ******************                                   \n");
                    printf("                                **                         @            ?             \n");
                    printf("                   &     n     **    ************                                     \n");
                    printf("                               **    **        **                                     \n");
                    printf("    @                         **     **        **                                     \n");
                    printf("                              **     ************                +                    \n");
                    printf("            +                **           **             '                            \n");
                    printf("                             **           **                                          \n");
                    printf("                            **      ***************                                   \n");
                    printf("                    @       **      **    **     **                |                  \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("    !                      **       **    **    ***        $                @         \n");
                    printf("                                          **                       /                  \n");
                    printf("                 &                        **                                          \n");
                    printf("                                          **                                          \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("            `                    **              **                               ~   \n");
                    printf("    @                            **              **                @                  \n");
                    printf("                                 ******************                                   \n");
                    printf("                                **                         @            ?             \n");
                    printf("                   &     n     **    ************                                     \n");
                    printf("                               **    **        **                                     \n");
                    printf("    @                         **     **        **                                     \n");
                    printf("                              **     ************                +                    \n");
                    printf("            +                **           **             '                            \n");
                    printf("                             **           **                                          \n");
                    printf("                            **      ***************                                   \n");
                    printf("                    @       **      **    **     **                |                  \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("    !                      **       **    **    ***        $                @         \n");
                    printf("                                          **                       /                  \n");
                    printf("                 &                        **                                          \n");
                    printf("                                          **                                          \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("afplay /Users/henry/Xcode/chess_client/chess_client/Win.m4a");
                } else {
                    printf("輸了～～～～!!!!!!!!\n");
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf("                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf(" $                                                                                    \n");
                    printf("                     **    **       **********    **********                  !       \n");
                    printf("          !          **    **   **  **      **    **      **        ^                 \n");
                    printf("     @                **   **   **  **********    **********                          \n");
                    printf("                      **  **   **   **      **    **      **                  -       \n");
                    printf("         (             ** **  **    **********    **********    $                     \n");
                    printf("                         **  **     **         **         **                 *        \n");
                    printf("                        ** **       **    ************    **           @              \n");
                    printf("   !                   **          **         **         **                           \n");
                    printf("                       ** **        **     **********     **                          \n");
                    printf("      @        )       **   **       **    ** * ** * **    **   +                     \n");
                    printf("                      **     **     **     **********     **                     >    \n");
                    printf("                     **        **   **      ** ** **      **           ^              \n");
                    printf("        @            **        **   **     **  **  **     **                          \n");
                    printf("                     **        **   **    **   **   **    **             +            \n");
                    printf("              ^      **        **   **         **       * **    =             !       \n");
                    printf("                                    **                   ***                          \n");
                    printf("                 {             }                                           @          \n");
                    printf("       ^                                                   _                          \n");
                    printf("                                         你超爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf(RED"                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf(" $                                                                                    \n");
                    printf("                     **    **       **********    **********                  !       \n");
                    printf("          !          **    **   **  **      **    **      **        ^                 \n");
                    printf("     @                **   **   **  **********    **********                          \n");
                    printf("                      **  **   **   **      **    **      **                  -       \n");
                    printf("         (             ** **  **    **********    **********    $                     \n");
                    printf("                         **  **     **         **         **                 *        \n");
                    printf("                        ** **       **    ************    **           @              \n");
                    printf("   !                   **          **         **         **                           \n");
                    printf("                       ** **        **     **********     **                          \n");
                    printf("      @        )       **   **       **    ** * ** * **    **   +                     \n");
                    printf("                      **     **     **     **********     **                     >    \n");
                    printf("                     **        **   **      ** ** **      **           ^              \n");
                    printf("        @            **        **   **     **  **  **     **                          \n");
                    printf("                     **        **   **    **   **   **    **             +            \n");
                    printf("              ^      **        **   **         **       * **    =             !       \n");
                    printf("                                    **                   ***                          \n");
                    printf("                 {             }                                           @          \n");
                    printf("       ^                                                   _                          \n");
                    printf("                                         你超爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf("                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("afplay /Users/henry/Xcode/chess_client/chess_client/lose.m4a");
                }
                END = 1;
            }
            exit(1);
        }
        else if (buffer[0] == 'n'){
            printf(RED"還沒換您喔～\n"RESET);
        }
        else if (buffer[0] == 'q'){
            system("clear");
            if(buffer[1] == '1'){
                if (*player == 1) {
                    printf("輸了～～～～!!!!!!!!\n");
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf("                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf(" $                                                                                    \n");
                    printf("                     **    **       **********    **********                  !       \n");
                    printf("          !          **    **   **  **      **    **      **        ^                 \n");
                    printf("     @                **   **   **  **********    **********                          \n");
                    printf("                      **  **   **   **      **    **      **                  -       \n");
                    printf("         (             ** **  **    **********    **********    $                     \n");
                    printf("                         **  **     **         **         **                 *        \n");
                    printf("                        ** **       **    ************    **           @              \n");
                    printf("   !                   **          **         **         **                           \n");
                    printf("                       ** **        **     **********     **                          \n");
                    printf("      @        )       **   **       **    ** * ** * **    **   +                     \n");
                    printf("                      **     **     **     **********     **                     >    \n");
                    printf("                     **        **   **      ** ** **      **           ^              \n");
                    printf("        @            **        **   **     **  **  **     **                          \n");
                    printf("                     **        **   **    **   **   **    **             +            \n");
                    printf("              ^      **        **   **         **       * **    =             !       \n");
                    printf("                                    **                   ***                          \n");
                    printf("                 {             }                                           @          \n");
                    printf("       ^                                                   _                          \n");
                    printf("                                         你超爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf("                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf(" $                                                                                    \n");
                    printf("                     **    **       **********    **********                  !       \n");
                    printf("          !          **    **   **  **      **    **      **        ^                 \n");
                    printf("     @                **   **   **  **********    **********                          \n");
                    printf("                      **  **   **   **      **    **      **                  -       \n");
                    printf("         (             ** **  **    **********    **********    $                     \n");
                    printf("                         **  **     **         **         **                 *        \n");
                    printf("                        ** **       **    ************    **           @              \n");
                    printf("   !                   **          **         **         **                           \n");
                    printf("                       ** **        **     **********     **                          \n");
                    printf("      @        )       **   **       **    ** * ** * **    **   +                     \n");
                    printf("                      **     **     **     **********     **                     >    \n");
                    printf("                     **        **   **      ** ** **      **           ^              \n");
                    printf("        @            **        **   **     **  **  **     **                          \n");
                    printf("                     **        **   **    **   **   **    **             +            \n");
                    printf("              ^      **        **   **         **       * **    =             !       \n");
                    printf("                                    **                   ***                          \n");
                    printf("                 {             }                                           @          \n");
                    printf("       ^                                                   _                          \n");
                    printf("                                         你超爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf(RED"                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("afplay /Users/henry/Xcode/chess_client/chess_client/lose.m4a");
                } else {
                    printf("獲勝!!!!\n");
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("            `                    **              **                               ~   \n");
                    printf("    @                            **              **                @                  \n");
                    printf("                                 ******************                                   \n");
                    printf("                                **                         @            ?             \n");
                    printf("                   &     n     **    ************                                     \n");
                    printf("                               **    **        **                                     \n");
                    printf("    @                         **     **        **                                     \n");
                    printf("                              **     ************                +                    \n");
                    printf("            +                **           **             '                            \n");
                    printf("                             **           **                                          \n");
                    printf("                            **      ***************                                   \n");
                    printf("                    @       **      **    **     **                |                  \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("    !                      **       **    **    ***        $                @         \n");
                    printf("                                          **                       /                  \n");
                    printf("                 &                        **                                          \n");
                    printf("                                          **                                          \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("            `                    **              **                               ~   \n");
                    printf("    @                            **              **                @                  \n");
                    printf("                                 ******************                                   \n");
                    printf("                                **                         @            ?             \n");
                    printf("                   &     n     **    ************                                     \n");
                    printf("                               **    **        **                                     \n");
                    printf("    @                         **     **        **                                     \n");
                    printf("                              **     ************                +                    \n");
                    printf("            +                **           **             '                            \n");
                    printf("                             **           **                                          \n");
                    printf("                            **      ***************                                   \n");
                    printf("                    @       **      **    **     **                |                  \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("    !                      **       **    **    ***        $                @         \n");
                    printf("                                          **                       /                  \n");
                    printf("                 &                        **                                          \n");
                    printf("                                          **                                          \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("afplay /Users/henry/Xcode/chess_client/chess_client/Win.m4a");
                }
                END = 1;
            }
            else if(buffer[1] == '2'){
                if (*player == 1) {
                    printf("獲勝!!!!\n");
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("            `                    **              **                               ~   \n");
                    printf("    @                            **              **                @                  \n");
                    printf("                                 ******************                                   \n");
                    printf("                                **                         @            ?             \n");
                    printf("                   &     n     **    ************                                     \n");
                    printf("                               **    **        **                                     \n");
                    printf("    @                         **     **        **                                     \n");
                    printf("                              **     ************                +                    \n");
                    printf("            +                **           **             '                            \n");
                    printf("                             **           **                                          \n");
                    printf("                            **      ***************                                   \n");
                    printf("                    @       **      **    **     **                |                  \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("    !                      **       **    **    ***        $                @         \n");
                    printf("                                          **                       /                  \n");
                    printf("                 &                        **                                          \n");
                    printf("                                          **                                          \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("            `                    **              **                               ~   \n");
                    printf("    @                            **              **                @                  \n");
                    printf("                                 ******************                                   \n");
                    printf("                                **                         @            ?             \n");
                    printf("                   &     n     **    ************                                     \n");
                    printf("                               **    **        **                                     \n");
                    printf("    @                         **     **        **                                     \n");
                    printf("                              **     ************                +                    \n");
                    printf("            +                **           **             '                            \n");
                    printf("                             **           **                                          \n");
                    printf("                            **      ***************                                   \n");
                    printf("                    @       **      **    **     **                |                  \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("    !                      **       **    **    ***        $                @         \n");
                    printf("                                          **                       /                  \n");
                    printf("                 &                        **                                          \n");
                    printf("                                          **                                          \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(GREEN"\n\n\n\n\n\n\n\n\n");
                    printf("                                 ******************                                   \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 **              **           ******                  \n");
                    printf("                                 ******************           ******                  \n");
                    printf("                                **                            ******                  \n");
                    printf("                               **    ************             ******                  \n");
                    printf("                               **    **        **             ******                  \n");
                    printf("                              **     **        **             ******                  \n");
                    printf("                              **     ************             ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                             **           **                  ******                  \n");
                    printf("                            **      ***************           ******                  \n");
                    printf("                            **      **    **     **                                   \n");
                    printf("                           **       **    **   * **                                   \n");
                    printf("                           **       **    **    ***                                   \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                          **                  ******                  \n");
                    printf("                                                                                      \n");
                    printf("                                        恭喜獲勝！                                      \n");
                    usleep(400000);
                    
                    system("afplay /Users/henry/Xcode/chess_client/chess_client/Win.m4a");
                } else {
                    printf("輸了～～～～!!!!!!!!\n");
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf("                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf(" $                                                                                    \n");
                    printf("                     **    **       **********    **********                  !       \n");
                    printf("          !          **    **   **  **      **    **      **        ^                 \n");
                    printf("     @                **   **   **  **********    **********                          \n");
                    printf("                      **  **   **   **      **    **      **                  -       \n");
                    printf("         (             ** **  **    **********    **********    $                     \n");
                    printf("                         **  **     **         **         **                 *        \n");
                    printf("                        ** **       **    ************    **           @              \n");
                    printf("   !                   **          **         **         **                           \n");
                    printf("                       ** **        **     **********     **                          \n");
                    printf("      @        )       **   **       **    ** * ** * **    **   +                     \n");
                    printf("                      **     **     **     **********     **                     >    \n");
                    printf("                     **        **   **      ** ** **      **           ^              \n");
                    printf("        @            **        **   **     **  **  **     **                          \n");
                    printf("                     **        **   **    **   **   **    **             +            \n");
                    printf("              ^      **        **   **         **       * **    =             !       \n");
                    printf("                                    **                   ***                          \n");
                    printf("                 {             }                                           @          \n");
                    printf("       ^                                                   _                          \n");
                    printf(RED"                                         你超爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf("                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RESET"\n\n\n\n\n\n\n\n\n");
                    printf(" $                                                                                    \n");
                    printf("                     **    **       **********    **********                  !       \n");
                    printf("          !          **    **   **  **      **    **      **        ^                 \n");
                    printf("     @                **   **   **  **********    **********                          \n");
                    printf("                      **  **   **   **      **    **      **                  -       \n");
                    printf("         (             ** **  **    **********    **********    $                     \n");
                    printf("                         **  **     **         **         **                 *        \n");
                    printf("                        ** **       **    ************    **           @              \n");
                    printf("   !                   **          **         **         **                           \n");
                    printf("                       ** **        **     **********     **                          \n");
                    printf("      @        )       **   **       **    ** * ** * **    **   +                     \n");
                    printf("                      **     **     **     **********     **                     >    \n");
                    printf("                     **        **   **      ** ** **      **           ^              \n");
                    printf("        @            **        **   **     **  **  **     **                          \n");
                    printf("                     **        **   **    **   **   **    **             +            \n");
                    printf("              ^      **        **   **         **       * **    =             !       \n");
                    printf("                                    **                   ***                          \n");
                    printf("                 {             }                                           @          \n");
                    printf("       ^                                                   _                          \n");
                    printf("                                         你超爛！                                       \n");
                    usleep(400000);
                    
                    system("clear");
                    
                    printf(RED"\n\n\n\n\n\n\n\n\n");
                    printf("                                                                                      \n");
                    printf("                 **    **       **********    **********        *****                 \n");
                    printf("                 **    **   **  **      **    **      **        *****                 \n");
                    printf("                  **   **   **  **********    **********        *****                 \n");
                    printf("                  **  **   **   **      **    **      **        *****                 \n");
                    printf("                   ** **  **    **********    **********        *****                 \n");
                    printf("                     **  **     **         **         **        *****                 \n");
                    printf("                    ** **       **    ************    **        *****                 \n");
                    printf("                    **          **         **         **        *****                 \n");
                    printf("                   ** **        **     **********     **        *****                 \n");
                    printf("                  **   **       **    ** * ** * **    **        *****                 \n");
                    printf("                  **     **     **     **********     **                              \n");
                    printf("                  **       **   **      ** ** **      **                              \n");
                    printf("                 **        **   **     **  **  **     **        *****                 \n");
                    printf("                 **        **   **    **   **   **    **        *****                 \n");
                    printf("                 **        **   **         **       * **        *****                 \n");
                    printf("                                **                   ***                              \n");
                    printf("                                                                                      \n");
                    printf("                                                                                      \n");
                    printf(RED"                                         你就爛！                                       \n");
                    usleep(400000);
                    
                    system("afplay /Users/henry/Xcode/chess_client/chess_client/lose.m4a");
                }
                END = 1;
            }
            exit(1);
        }
        else if(buffer[0] == 't'){
            system("afplay /Users/henry/Xcode/chess_client/chess_client/check.m4a");
            if (*player == 1) {
                buffer[0] = ' ';
                timestamp(tmp);
                printf("%s玩家二說 :%s", tmp, buffer);
            
            } else {
                buffer[0] = ' ';
                timestamp(tmp);
                printf("%s玩家一說 :%s", tmp, buffer);
            }
        }
        else if(buffer[0] == 'i' || buffer[0] == 'e' || buffer[0] == '\0') {
            if (buffer[0] == 'i') {
                if (buffer[2] == 't') {
                    printf("\n請移動棋子，也可輸入訊息：\n");
                }
                if (buffer[2] == 'n') {
                    printf("\n等待對方移動棋子，您可以輸入訊息：\n");
                }
                if (buffer[2] == 'p') {
                    *player = atoi(&buffer[3]);
                    if (*player == 1) {
                        printf("您是超極屌玩家一（黑棋）\n");
                        p = 1;
                    } else {
                        printf("您是超極屌玩家二（白棋）\n");
                        p = 2;
                    }
              }
            }
            //error訊息
            else if (buffer[0] == 'e') {
                // Syntax errors
                if (buffer[2] == '0') {
                    /*switch (buffer[3]) {
                        case '0':
                            printf(RED "  ↑ (少了'-')\n" RESET);
                            break;
                        case '1':
                            printf(RED "↑   (應該是字母)\n" RESET);
                            break;
                        case '2':
                            printf(RED "   ↑ (應該是字母)\n" RESET);
                            break;
                        case '3':
                            printf(RED " ↑ (應概是數字)\n" RESET);
                            break;
                        case '4':
                            printf(RED " ↑ (超出界線)\n" RESET);
                            break;
                        case '5':
                            printf(RED "   ↑ (應該是數字)\n" RESET);
                            break;
                        case '6':
                            printf(RED "   ↑ (超出界線)\n" RESET);
                            break;
                        case '7':
                            printf(RED "(超出界線)\n" RESET);
                            break;
                    }
                    */
                }
                system("clear");
                
                if (*player == 1) {
                    print_board_buff(BOARD);
                } else {
                    print_board_buff_inverted(BOARD);
                }
                
                printf(RED"\n這走法不合規則喔！會不會玩啊....\n"RESET);
                printf("請移動棋子，也可輸入訊息：\n");
          }
      }
      else {
          // 印 board
          system("clear");
          
          system("afplay /Users/henry/Xcode/chess_client/chess_client/check.m4a");
          
          if (*player == 1) {
              for(int n = 0; n < 64; n++){
                  BOARD[n] = buffer [n];
              }
              print_board_buff(buffer);
          } else {
              for(int n = 0; n < 64; n++){
                  BOARD[n] = buffer [n];
              }
              print_board_buff_inverted(buffer);
          }
      }
      bzero(buffer, 128);
      usleep(100);
     }
    
}

int stdin_ready() {
    fd_set fdset;
    struct timeval timeout;
    int fd;
    
    fd = fileno(stdin);
    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);
    timeout.tv_sec = 0;
    timeout.tv_usec = 1;
    
    usleep(5000);
    
    return select(fd + 1, &fdset, NULL, NULL, &timeout) == 1 ? 1 : 0;
}

int main(void) {
    char ip[10];
    int sockfd, portno, i=0, aa, bb;
    struct sockaddr_in serv_addr;
    struct hostent * server;
    char tmp[64];

    
    bzero(&tmp, 64);
        
    setlocale(LC_ALL, "en_US.UTF-8");
    char buffer[128];

    system("clear");
    system("clear");
    system("clear");
    
    printf("                                        ***        \n");
    printf("                                    ****   ****    \n");
    printf("                                    ****   ****    \n");
    printf("                                        * *        \n");
    printf("                                  ******* *******  \n");
    printf("                                  *             *  \n");
    printf("                                  ****       ****  \n");
    printf("                                      *     *      \n");
    printf("                                       *   *       \n");
    printf("                                      *     *      \n");
    printf("                                     *       *     \n");
    printf("                                     *       *     \n");
    printf("                                      *     *      \n");
    printf("                                      *     *      \n");
    printf("                                      *     *      \n");
    printf("                                     *       *     \n");
    printf("                                    *         *    \n");
    printf("                                   *           *   \n");
    printf("                                  *    client   *  \n");
    printf("                                  ***************  \n");
    
    printf("輸入IP：");
    scanf("%s", &ip[1]);
    printf("輸入port：");
    scanf("%d", &portno);
       
    system("clear");
    
    printf("\n\n\n");
    printf("                                                                         ***           \n");
    printf("                                                                     ****   ****       \n");
    printf("                                                                     ****   ****       \n");
    printf("                                                                         * *           \n");
    printf("                                                                   ******* *******     \n");
    printf("                                                                   *             *     \n");
    printf("                                                                   ****       ****     \n");
    printf("                                                                       *     *         \n");
    printf("                                                                        *   *          \n");
    printf("                                                                       *     *         \n");
    printf("                                                                      *       *        \n");
    printf("                                                                      *       *        \n");
    printf("                                                                       *     *         \n");
    printf("                                                                       *     *         \n");
    printf("                                                                       *     *         \n");
    printf("                                                                      *       *        \n");
    printf("                                                                     *         *       \n");
    printf("                                                                    *           *      \n");
    printf("                                                                   *             *     \n");
    printf("                                                                   ***************     \n\n");
    usleep(400000);
    
    system("clear");

    printf("\n\n\n");
    printf("                                                             ***                       \n");
    printf("                                                         ****   ****                   \n");
    printf("                                                         ****   ****                   \n");
    printf("                                                             * *              **       \n");
    printf("                                                       ******* *******      **  **     \n");
    printf("                                                       *             *    **      **   \n");
    printf("                                                       ****       ****  **             \n");
    printf("                                                           *     *      **             \n");
    printf("                                                            *   *       **             \n");
    printf("                                                           *     *      **             \n");
    printf("                                                          *       *     **             \n");
    printf("                                                          *       *     **             \n");
    printf("                                                           *     *      **             \n");
    printf("                                                           *     *        **      **   \n");
    printf("                                                           *     *          **  **     \n");
    printf("                                                          *       *           **       \n");
    printf("                                                         *         *                   \n");
    printf("                                                        *           *                  \n");
    printf("                                                       *             *                 \n");
    printf("                                                       ***************                 \n\n");
    usleep(400000);
    
    system("clear");

    printf("\n\n\n");
    printf("                                                 ***                                   \n");
    printf("                                             ****   ****                               \n");
    printf("                                             ****   ****                               \n");
    printf("                                                 * *              **       **     **   \n");
    printf("                                           ******* *******      **  **     **     **   \n");
    printf("                                           *             *    **      **   **     **   \n");
    printf("                                           ****       ****  **             **     **   \n");
    printf("                                               *     *      **             **     **   \n");
    printf("                                                *   *       **             **     **   \n");
    printf("                                               *     *      **             *********   \n");
    printf("                                              *       *     **             **     **   \n");
    printf("                                              *       *     **             **     **   \n");
    printf("                                               *     *      **             **     **   \n");
    printf("                                               *     *        **      **   **     **   \n");
    printf("                                               *     *          **  **     **     **   \n");
    printf("                                              *       *           **       **     **   \n");
    printf("                                             *         *                               \n");
    printf("                                            *           *                              \n");
    printf("                                           *             *                             \n");
    printf("                                           ***************                             \n\n");
    usleep(400000);
    
    system("clear");

    printf("\n\n\n");
    printf("                                       ***                                             \n");
    printf("                                   ****   ****                                         \n");
    printf("                                   ****   ****                                         \n");
    printf("                                       * *              **       **     **   ********  \n");
    printf("                                 ******* *******      **  **     **     **   ********  \n");
    printf("                                 *             *    **      **   **     **   **        \n");
    printf("                                 ****       ****  **             **     **   **        \n");
    printf("                                     *     *      **             **     **   **        \n");
    printf("                                      *   *       **             **     **   **        \n");
    printf("                                     *     *      **             *********   ********  \n");
    printf("                                    *       *     **             **     **   **        \n");
    printf("                                    *       *     **             **     **   **        \n");
    printf("                                     *     *      **             **     **   **        \n");
    printf("                                     *     *        **      **   **     **   **        \n");
    printf("                                     *     *          **  **     **     **   ********  \n");
    printf("                                    *       *           **       **     **   ********  \n");
    printf("                                   *         *                                         \n");
    printf("                                  *           *                                        \n");
    printf("                                 *             *                                       \n");
    printf("                                 ***************                                       \n\n");
    usleep(400000);
    
    system("clear");

    printf("\n\n\n");
    printf("                         ***                                                           \n");
    printf("                     ****   ****                                                       \n");
    printf("                     ****   ****                                                       \n");
    printf("                         * *              **       **     **   ********       **       \n");
    printf("                   ******* *******      **  **     **     **   ********     **  **     \n");
    printf("                   *             *    **      **   **     **   **         **      **   \n");
    printf("                   ****       ****  **             **     **   **        **            \n");
    printf("                       *     *      **             **     **   **          **          \n");
    printf("                        *   *       **             **     **   **            **        \n");
    printf("                       *     *      **             *********   ********        **      \n");
    printf("                      *       *     **             **     **   **                **    \n");
    printf("                      *       *     **             **     **   **                  **  \n");
    printf("                       *     *      **             **     **   **                   ** \n");
    printf("                       *     *        **      **   **     **   **         **      **   \n");
    printf("                       *     *          **  **     **     **   ********     **  **     \n");
    printf("                      *       *           **       **     **   ********       **       \n");
    printf("                     *         *                                                       \n");
    printf("                    *           *                                                      \n");
    printf("                   *             *                                                     \n");
    printf("                   ***************                                                     \n\n");
    usleep(400000);
    
    printf("\n");

    system("clear");

    printf("\n\n\n");
    printf("          ***                                                                          \n");
    printf("      ****   ****                                                                      \n");
    printf("      ****   ****                                                                      \n");
    printf("          * *              **       **     **   ********       **            **        \n");
    printf("    ******* *******      **  **     **     **   ********     **  **        **  **      \n");
    printf("    *             *    **      **   **     **   **         **      **    **      **    \n");
    printf("    ****       ****  **             **     **   **        **            **             \n");
    printf("        *     *      **             **     **   **          **            **           \n");
    printf("         *   *       **             **     **   **            **            **         \n");
    printf("        *     *      **             *********   ********        **            **       \n");
    printf("       *       *     **             **     **   **                **            **     \n");
    printf("       *       *     **             **     **   **                  **            **   \n");
    printf("        *     *      **             **     **   **                   **            **  \n");
    printf("        *     *        **      **   **     **   **         **      **    **      **    \n");
    printf("        *     *          **  **     **     **   ********     **  **        **  **      \n");
    printf("       *       *           **       **     **   ********       **            **        \n");
    printf("      *         *                                                                      \n");
    printf("     *           *                                                                     \n");
    printf("    *             *                                                                    \n");
    printf("    ***************                                                           client   \n\n");
    usleep(400000);
    
    printf("\n");
  
    printf("連接到 %s : %d\n", &ip[1], portno);
    system("afplay /Users/henry/Xcode/chess_client/chess_client/Gametime.m4a");
    
    /* 開啟socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
       perror("ERROR 開啟socket");
       exit(1);
    }

    server = gethostbyname(&ip[1]);

    if (server == NULL) {
       fprintf(stderr,"ERROR, 沒有找到server\n");
       exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
       perror("ERROR 連接");
       exit(1);
    }

    pthread_t tid[2];

    bzero(buffer, 128);
    
    pthread_create(&tid[0], NULL, &on_signal, &sockfd);  // 開啟接收 thread
    
    while (1) {
        
        if (END == 1){
            printf("遊戲結束！！！");
            break;
        }
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
                
        //移動訊息
        if(buffer[0] == 'm'){
            
            if(p == 1){
                    //玩家一選擇要移動的棋子
                    color[a][b] = 14;
                    printboard_one(BOARD);
                    
                    while ((ch = getchar()) != 115 || translate_piece(BOARD[56 - a * 8 + b]) == 0)//有棋子且不為黑棋，後按s可離開
                    {
                        color[a][b] = 14;
                        input();
                        printboard_one(BOARD);
                    }
                    coord_white(); //判斷白棋座標 存在xy(格式為a1)
                        
                    aa = a;
                    bb = b;
                    color[aa][bb] = 12;
                
                    //白棋選擇要移動到的位置
                    printboard_one(BOARD);
                    while ((ch = getchar()) != 115) //移動的方式正確(我不會判斷)後按enter離開
                    {
                        input();
                        color[aa][bb] = 12;//將要移動的棋子背景變紅
                        printboard_one(BOARD);
                    }
                    coord_white(); //判斷白棋座標 存在xy(格式為a1a6)
                        
                    initial_background();//背景變全黑
                
            }
            else{
                
                //玩家二選擇要移動的棋子
                            
                color[a][b] = 14;
                printboard_two(BOARD);
                while ((ch = getchar()) != 115 || BOARD[a * 8 + b] == 0)//有棋子且不為白棋，後按s可離開
                {
                    //printf("%c", ch);
                    color[a][b] = 14;
                    input();
                    printboard_two(BOARD);
                }
                coord_black();
                    
                aa = a;
                bb = b;
                color[aa][bb] = 12;
            
                //選擇要移動到的位置
                printboard_two(BOARD);
                while ((ch = getchar()) != 115) //移動的方式正確，按s離開
                {
                    //printf("%c", ch);
                    input();
                    color[aa][bb] = 12;//將要移動的棋子背景變紅
                    printboard_two(BOARD);
                }
                coord_black(); //判斷黑棋座標 存在xy(格式為a1a6)
                
                //printf("%s", xy);  //印出座標
                
                initial_background();//背景變全黑
            }
        
            buffer[0] = xy[0];
            buffer[1] = xy[1];
            buffer[2] = '-';
            buffer[3] = xy[2];
            buffer[4] = xy[3];
            
            printf("xy : %s\n", xy);
            printf("buffer : %s\n", buffer);
            long n = write(sockfd, buffer, sizeof(buffer));
            
            if (n < 0) {
                perror("ERROR 寫入socket");
                exit(1);
            }
            bzero(xy, 8);
            a = 0;
            b = 0;
        }
        //投降訊息
        else if (buffer[0] == 'q'){
            
            long n = write(sockfd, buffer, sizeof(buffer));
            
            if (n < 0) {
                perror("ERROR 寫入socket");
                exit(1);
            }
        }
        //聊天訊息
        else if (buffer[0] == 't'){
            long n = write(sockfd, buffer, sizeof(buffer));
            
            buffer[0] = ' ';
            timestamp(tmp);
            printf("%s您說 :%s", tmp,buffer);
            if (n < 0) {
                perror("ERROR 寫入socket");
                exit(1);
            }
        }
        //輸入錯誤
        else{
            if(i == 0){
                i=1;
                continue;
            }
            printf("輸入錯誤，請重新輸入！\n");
        }
        usleep(100);
    }
   return 0;
}
