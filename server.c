#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>
#include <signal.h>
#include <wchar.h>
#include <locale.h>

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

int PORT ,END = 0 ,player1, player2;

// 玩家狀態變數
pthread_cond_t player_to_join;
pthread_mutex_t general_mutex;

bool is_diagonal(int, int);

static wchar_t ** create_board() {

    wchar_t ** board = (wchar_t **) malloc(sizeof(wchar_t *)*8);
    for (int i = 0; i < 8; i++) {
        board[i] = (wchar_t *) malloc(sizeof(wchar_t)*8);
    }

    return board;
}

static void initialize_board(wchar_t ** p_board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            if (i == 0) {
                if (j == 0 || j == 7) p_board[i][j] = white_rook;
                if (j == 1 || j == 6) p_board[i][j] = white_knight;
                if (j == 2 || j == 5) p_board[i][j] = white_bishop;
                if (j == 3) p_board[i][j] = white_queen;
                if (j == 4) p_board[i][j] = white_king;
            }

            if (i == 1) p_board[i][j] = white_pawn;

            if (i > 1 && i < 6) p_board[i][j] = 0;

            if (i == 6) p_board[i][j] = black_pawn;
            if (i == 7) {
                if(j == 0 || j == 7) p_board[i][j] = black_rook;
                if(j == 1 || j == 6) p_board[i][j] = black_knight;
                if(j == 2 || j == 5) p_board[i][j] = black_bishop;
                if(j == 3) p_board[i][j] = black_queen;
                if(j == 4) p_board[i][j] = black_king;
            }
        }
    }
}

static char * create_od_board() {
    char * board = (char *) malloc(sizeof(char *) * 8 * 8);
    return board;
}

//轉為一維陣列
static void to_one_dimension_char(wchar_t ** board, char * od_board) {

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == white_rook) od_board[8*i+j] = '1';
            else if (board[i][j] == white_knight) od_board[8*i+j] = '2';
            else if (board[i][j] == white_bishop) od_board[8*i+j] = '3';
            else if (board[i][j] == white_queen) od_board[8*i+j] = '4';
            else if (board[i][j] == white_king) od_board[8*i+j] = '5';
            else if (board[i][j] == white_pawn) od_board[8*i+j] = '6';
            else if (board[i][j] == black_rook) od_board[8*i+j] = '7';
            else if (board[i][j] == black_knight) od_board[8*i+j] = '8';
            else if (board[i][j] == black_bishop) od_board[8*i+j] = '9';
            else if (board[i][j] == black_queen) od_board[8*i+j] = 'a';
            else if (board[i][j] == black_king) od_board[8*i+j] = 'b';
            else if (board[i][j] == black_pawn) od_board[8*i+j] = 'c';
            else od_board[8*i+j] = '0';
        }
    }
}

// 玩家配對
int challenging_player = 0;
int player_is_waiting = 0;

void move_piece(wchar_t ** board, int * move) {
    board[move[2]][move[3]] = board[*move][move[1]];
    board[*move][move[1]] = 0;
}

bool emit(int client, char * message, int message_size) {
    return true;
}

// 轉換成 move 轉換成陣列
void translate_to_move(int * move, char * buffer , char * text ,int player) {

    *(move) = 8-(*(buffer+1)-'0');
    move[1] = (*(buffer)-'0')-49;
    move[2] = 8-(*(buffer+4)-'0');
    move[3] = (*(buffer+3)-'0')-49;
    
    if(player == 1)
        text[0] = 1;
    else if(player == 2)
        text[0] = 2;

    for(int i = 1; i < 251; i++)
        text[i] = buffer[i + 5];

}

bool is_diagonal_clear(wchar_t ** board, int * move) {

    int * x_moves = (int *)malloc(sizeof(int));
    int * y_moves = (int *)malloc(sizeof(int));

    *(x_moves) = *(move) - move[2];
    *(y_moves) = move[1] - move[3];

    int * index = (int *)malloc(sizeof(int));
    *(index) =  abs(*x_moves) - 1;
    wchar_t * item_at_position = (wchar_t *)malloc(sizeof(wchar_t));

    while (*index > 0) {

        if (*x_moves > 0 && *y_moves > 0) { printf("%lc [%d,%d]\n", board[*move - *index][move[1]- *index],*move - *index,move[1]- *index); *item_at_position = board[*move - *index][move[1]- *index]; }
        if (*x_moves > 0 && *y_moves < 0) { printf("%lc [%d,%d]\n", board[*move - *index][move[1]+  *index],*move - *index,move[1]+ *index); *item_at_position = board[*move - *index][move[1]+ *index]; }
        if (*x_moves < 0 && *y_moves < 0 ) { printf("%lc [%d,%d]\n", board[*move + *index][move[1]+ *index],*move + *index,move[1]+ *index); *item_at_position = board[*move + *index][move[1]+ *index]; }
        if (*x_moves < 0 && *y_moves > 0 ) { printf("%lc [%d,%d]\n", board[*move + *index][move[1]- *index],*move + *index,move[1]- *index); *item_at_position = board[*move + *index][move[1]- *index]; }

        if (*item_at_position != 0) {
            free(index);
            free(x_moves);
            free(y_moves);
            free(item_at_position);
            return false;
        }

        (*index)--;
    }

    free(index);
    free(x_moves);
    free(y_moves);
    free(item_at_position);

    return true;
}

// 檢查走法
bool is_syntax_valid(int player, char * move) {
    // 檢查 '-'
    if (move[2] != '-'){ send(player, "e-00", 4, 0);return false; }
    //First and 3th should be characters
    if (move[0]-'0' < 10) { send(player, "e-01", 4, 0); return false; }
    if (move[3]-'0' < 10) { send(player, "e-02", 4, 0); return false; }

    //第2和5要為數字
    if (move[1]-'0' > 10) { send(player, "e-03", 4, 0); return false; }
    if (move[1]-'0' > 8) { send(player, "e-04", 4, 0); return false; }
    if (move[4]-'0' > 10) { send(player, "e-05", 4, 0); return false; }
    if (move[4]-'0' > 8) { send(player, "e-06", 4, 0); return false; }
    //移動出界
    if (move[0]-'0' > 56 || move[3]-'0' > 56) { send(player, "e-07", 4, 0); return false; }

    return true;
}

//廣播
void broadcast(wchar_t ** board, char * one_dimension_board, int player_one, int player_two, char * text) {
    int black_win = 1, white_win = 1;
    
    to_one_dimension_char(board, one_dimension_board);
    
    for(int i = 0; i < 64; i++){
        if(one_dimension_board[i] == '5')
            black_win = 0;
        else if(one_dimension_board[i] == 'b')
            white_win = 0;
    }
    
    if(black_win == 0 && white_win == 0 ){
        send(player_one, one_dimension_board, 128, 0);
        send(player_two, one_dimension_board, 128, 0);
        END = 0;
    }
    else if(white_win == 1){
        printf("超級屌玩家二獲勝!!!!\n");
        one_dimension_board[0]='w';
        one_dimension_board[1]='1';
        send(player_one, one_dimension_board, 128, 0);
        send(player_two, one_dimension_board, 128, 0);
        END = 1;
        exit(1);
    }
    else if(black_win == 1){
        printf("超級屌玩家一獲勝!!!!!!!!\n");
        one_dimension_board[0]='w';
        one_dimension_board[1]='2';
        send(player_one, one_dimension_board, 128, 0);
        send(player_two, one_dimension_board, 128, 0);
        END = 1;
        exit(1);
    }
    END = 0;

}

int get_piece_team(wchar_t ** board, int x, int y) {

    switch (board[x][y]) {
        case white_king: return -1;
        case white_queen: return -1;
        case white_rook: return -1;
        case white_bishop: return -1;
        case white_knight: return -1;
        case white_pawn: return -1;
        case black_king: return 1;
        case black_queen: return 1;
        case black_rook: return 1;
        case black_bishop: return 1;
        case black_knight: return 1;
        case black_pawn: return 1;
  }

    return 0;
}

void promote_piece(wchar_t ** board, int destX, int destY, int team) {
    if (team == 1) {
        board[destX][destY] = black_queen;
    } else if (team == -1) {
        board[destX][destY] = white_queen;
    }
}

//棋子代號
int get_piece_type(wchar_t piece) {

    switch (piece) {
        case white_king: return 0;
        case white_queen: return 1;
        case white_rook: return 2;
        case white_bishop: return 3;
        case white_knight: return 4;
        case white_pawn: return 5;
        case black_king: return 0;
        case black_queen: return 1;
        case black_rook: return 2;
        case black_bishop: return 3;
        case black_knight: return 4;
        case black_pawn: return 5;
    }
    return -1;

}

bool is_rect(int * move) {

    int * x_moves = (int *)malloc(sizeof(int));
    int * y_moves = (int *)malloc(sizeof(int));

    *x_moves = *(move) - move[2];
    *y_moves = move[1] - move[3];


    if ((x_moves != 0 && y_moves == 0) || (y_moves != 0 && x_moves == 0)) {
        free(x_moves);
        free(y_moves);
        return true;
    }

    free(x_moves);
    free(y_moves);
    return false;
}

int is_rect_clear(wchar_t ** board, int * move, int x_moves, int y_moves ) {

    int * index = (int *)malloc(sizeof(int));

      if (abs(x_moves) > abs(y_moves)) {
        *index = abs(x_moves) - 1;
      } else {
          *index = abs(y_moves) - 1;
      }
    
    while (*index > 0) {

        if (*(move) - move[2] > 0) { if (board[*move-(*index)][move[1]] != 0) { free(index); return false; } }
        if (*(move) - move[2] < 0) { if (board[*move+(*index)][move[1]] != 0) { free(index); return false; } }
        if (move[1] - move[3] > 0 ) { if (board[*move][move[1]-(*index)] != 0) { free(index); return false; } }
        if (move[1] - move[3] < 0 ) { if (board[*move][move[1]+(*index)] != 0) { free(index); return false; } }

        (*index)--;
    }

    free(index);
    return true;

}

bool is_diagonal(int x_moves, int y_moves) {

    if ((abs(x_moves)-abs(y_moves)) != 0) {
        return false;
    }

    return true;
}

int getManitud(int origin, int dest) {
    return (abs(origin-dest));
}

bool eat_piece(wchar_t ** board, int x, int y) {
    return (get_piece_team(board, x, y) != 0);
}

void freeAll(int * piece_team, int * x_moves, int * y_moves) {
    free(piece_team);
    free(x_moves);
    free(y_moves);
}

bool is_move_valid(wchar_t ** board, int player, int team, int * move) {

    int * piece_team = (int *)malloc(sizeof(int *));
    int * x_moves = (int *)malloc(sizeof(int *));
    int * y_moves = (int *)malloc(sizeof(int *));

    *piece_team = get_piece_team(board, *(move), move[1]);
    *x_moves = getManitud(*move, move[2]);
    *y_moves = getManitud(move[1], move[3]);
    
    // 主要error
    if (board[*(move)][move[1]] == 0) {
        send(player, "e-08", 4, 0);
        return false;
    }// 選到棋子為0
    if (*piece_team == get_piece_team(board, move[2], move[3])) {
        send(player, "e-09", 4, 0);
        return false;
    }// 選到對方棋子

    // 檢查是否為己方棋子
    if (team != *piece_team) {
        send(player, "e-07", 4, 0);
        return false;
    }

    switch (get_piece_type(board[*(move)][move[1]])) {
        case 0: /* --- ♚ --- */
            if (*x_moves > 1 || getManitud(move[1], move[3]) > 1) {
                send(player, "e-10", 5, 0);
                freeAll(piece_team, x_moves, y_moves);
                return false;
            }
            break;
        case 2: /* --- ♜ --- */
            if (!is_rect(move)) {
                send(player, "e-30", 5, 0);
                freeAll(piece_team, x_moves, y_moves);
                return false;
            }
            if (!is_rect_clear(board, move, *x_moves, *y_moves)) {
                send(player, "e-31", 4, 0);
                freeAll(piece_team, x_moves, y_moves);
                return false;
            }
            if (eat_piece(board, move[2], move[3])) {
                send(player, "i-99", 4, 0);
                freeAll(piece_team, x_moves, y_moves);
                return true;
            }
            break;
        case 3: /* ––– ♝ ––– */
            if (!is_diagonal(*x_moves, getManitud(move[1], move[3]))) {
                send(player, "e-40", 4, 0);
                freeAll(piece_team, x_moves, y_moves);
                return false; // Check if it's a valid diagonal move
            }
            if (!is_diagonal_clear(board, move)) {
                send(player, "e-41", 4, 0);
                freeAll(piece_team, x_moves, y_moves);
                return false;
            }
            if (eat_piece(board, move[2], move[3])) {
                send(player, "i-99", 4, 0);
                freeAll(piece_team, x_moves, y_moves);
                return true;
            }
            break;
        case 4: /* --- ♞ --- */
            if ((abs(*x_moves) != 1 || abs(*y_moves) != 2) && (abs(*x_moves) != 2 || abs(*y_moves) != 1)) {
                send(player, "e-50", 4, 0);
                freeAll(piece_team, x_moves, y_moves);
                return false;
            }
            if (eat_piece(board, move[2], move[3])) {
                send(player, "i-99", 4, 0);
                freeAll(piece_team, x_moves, y_moves);
                return true;
            }
            break;
        case 5: /* --- ♟ --- */
            if (*piece_team == 1 && move[2] == 0) {
                promote_piece(board, move[2], move[3], *piece_team);
                send(player, "i-98", 4, 0);
                freeAll(piece_team, x_moves, y_moves);
                return true;
            }
            if (*piece_team == -1 && move[2] == 7) {
                promote_piece(board, move[2], move[3], *piece_team);
                send(player, "i-98", 4, 0);
                freeAll(piece_team, x_moves, y_moves);
                return true;
            }
            // Moving in Y axis
            if (getManitud(move[1], move[3]) != 0) {
                if (!is_diagonal(*x_moves, *y_moves) || (get_piece_team(board, move[2], move[3]) == 0)) {
                    send(player, "e-60", 4, 0);
                    freeAll(piece_team, x_moves, y_moves);
                    return false;
                }
                if (eat_piece(board, move[2], move[3])) {
                    send(player, "i-99", 4, 0);
                    freeAll(piece_team, x_moves, y_moves);
                    return true;
                }
            } else {
                //是否為第一步
                if (move[0] == 6 && *piece_team == 1 && *x_moves == 2 ) {
                    return true;
                }
                if (move[0] == 1 && *piece_team == -1 && *x_moves == 2 ) {
                    return true;
                }
                if (*x_moves > 1) {
                    send(player, "e-62", 5, 0);
                    freeAll(piece_team, x_moves, y_moves);
                    return false;
                }
            }
            break;
        default:
            break;
    }

    freeAll(piece_team, x_moves, y_moves);
    return true;
}

void timestamp(char* ubuf) {
    
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

void * player1_rcv(void * client_socket){
    char buffer[128];
    char tmp[64];
    bzero(&tmp, 64);
    
    printf("等待玩家ㄧ傳送訊息....\n");
    
    while(1){
        bzero(buffer, 128);
        if(read(player1, buffer, sizeof(buffer))){
            if(buffer[0] == 't'){
                send(player2, buffer, sizeof(buffer), 0);
                buffer[0] = ' ';
                timestamp(tmp);
                printf("%s玩家一說：%s", tmp, buffer);
            }
            else if (buffer[2] == '-'){
                buffer[0] = 'n';
                send(player1, buffer, sizeof(buffer), 0);
            }
        }
        usleep(100);
    }
    return 0;
}

void * player2_rcv(void * client_socket){
    char buffer[128];
    char tmp[64];
    bzero(&tmp, 64);
    
    printf("等待玩家二傳送訊息....\n");
    
    while(1){
        bzero(buffer, 128);
        if(read(player2, buffer, sizeof(buffer))){
            if(buffer[0] == 't'){
                send(player1, buffer, sizeof(buffer), 0);
                buffer[0] = ' ';
                timestamp(tmp);
                printf("%s玩家二說：%s", tmp, buffer);

            }
            else if (buffer[2] == '-'){
                buffer[0] = 'n';
                send(player2, buffer, sizeof(buffer), 0);
            }
    }
        usleep(100);
    }
    return 0;
}


void * game_room(void *client_socket) {
    
    int player_one = *(int *)client_socket;
    int player_two;
    char buffer[128];
    int * move = (int *)malloc(sizeof(int)*4);
    char text[128];
    char quit[2];
    char tmp[64];
    
    pthread_t tid[2];

    bzero(&tmp, 64);
    //建立新board
    wchar_t ** board = create_board();
    char * one_dimension_board = create_od_board();
    initialize_board(board);

    player_is_waiting = 1; // 等待人數為1

    pthread_mutex_lock(&general_mutex); // 等待玩家二
    pthread_cond_wait(&player_to_join, &general_mutex); // 等待玩家加入信號

    player_two = challenging_player; // 將玩家二指派challenging_player
    player_is_waiting = 0; // 等待人數為0

    if (send(player_one, "i-p1", 4, 0) < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
    if (send(player_two, "i-p2", 4, 0) < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
    pthread_mutex_unlock(&general_mutex);

    sleep(1);

    // 廣播給玩家一與玩家二
    broadcast(board, one_dimension_board, player_one, player_two ,text);

    sleep(1);

    bool syntax_valid = false;
    bool move_valid = false;

    while (1) {

        send(player_one, "i-tm", 4, 0);
        send(player_two, "i-nm", 4, 0);

        // 等待玩家一移動並檢查走法
        printf("等待玩家一移動....\n");
        
        bzero(buffer, 128);
        pthread_create(&tid[0], NULL, &player2_rcv, &client_socket);
        
        while (!syntax_valid  || !move_valid) {
            bzero(buffer, 128);
            
            if (read(player_one, buffer, sizeof(buffer))) {
                //玩家一投降
                if (buffer[0]=='q'){
                    printf("玩家一投降！超屌玩家二獲勝！！\n");
                    quit[0] = 'q';
                    quit[1] = '1';
                    send(player_one, quit, 2, 0);
                    send(player_two, quit, 2, 0);
                    exit(1);
                }
                else if (buffer[2] == '-'){
                    //玩家一移動
                    printf("玩家一移動: %c%c%c%c%c\n", buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);

                        syntax_valid = is_syntax_valid(player_one, buffer);

                        translate_to_move(move, buffer ,text, 1); // 轉為move

                        move_valid = is_move_valid(board, player_one, 1, move);
                }
                else if (buffer[0] == 't'){
                    //玩家一傳送聊天訊息
                    send(player_two, buffer, sizeof(buffer), 0);
                    buffer[0] = ' ';
                    timestamp(tmp);
                    printf("%s玩家ㄧ說 :%s", tmp, buffer);
                }
                 
            }
            usleep(100);
    
        }
        pthread_cancel(tid[0]);
        printf("玩家一移動成功\n");

        syntax_valid = false;
        move_valid = false;

        move_piece(board, move);

        // 廣播給玩家一與玩家二
        broadcast(board, one_dimension_board, player_one, player_two ,text);
        
        if(END == 1)
            break;
        
        sleep(1);
        send(player_one, "i-nm", 4, 0);
        send(player_two, "i-tm", 4, 0);

        printf("等待玩家二移動....\n");
        
        bzero(buffer, 128);
        pthread_create(&tid[1], NULL, &player1_rcv, &client_socket);
        
        while (!syntax_valid  || !move_valid) {
            bzero(buffer, 128);
        
            if (read(player_two, buffer, 128)) {
                if (buffer[0] == 'q'){
                    printf("玩家二投降！超屌玩家一獲勝！！\n");
                    quit[0] = 'q';
                    quit[1] = '2';
                    send(player_one, quit, 2, 0);
                    send(player_two, quit, 2, 0);
                    exit(1);
                }
                else if (buffer[2] == '-'){
                    printf("玩家二移動： %c%c%c%c%c\n", buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);
                    syntax_valid = is_syntax_valid(player_two, buffer);

                    translate_to_move(move, buffer ,text ,2); // 轉為move

                    move_valid = is_move_valid(board, player_two, -1, move);
                }
                else if (buffer[0] == 't'){
                    send(player_one, buffer, sizeof(buffer), 0);
                    buffer[0] = ' ';
                    timestamp(tmp);
                    printf("%s玩家二說 :%s", tmp, buffer);
                }

            }
            usleep(100);
        }
        pthread_cancel(tid[1]);
        printf("玩家二移動成功\n");
        
        syntax_valid = false;
        move_valid = false;

        move_piece(board, move);

        // 廣播給玩家一與玩家二
        broadcast(board, one_dimension_board, player_one, player_two ,text);
        
        if(END == 1)
            break;
        
        sleep(1);

  }
    /* 刪除 board */
    free(move);
    
    pthread_cancel(pthread_self());
    return 0;
}

int main( void ) {

    int sockfd, client_socket, port_number, client_length, i = 1;
    struct sockaddr_in server_address, client;

    pthread_t tid[1];
    setlocale(LC_ALL, "en_US.UTF-8");
    
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
    printf("                                  *    server   *  \n");
    printf("                                  ***************  \n");
    
    printf("輸入port：");
    scanf("%d", &PORT);
    
    system("clear");

    // 條件變數
    pthread_cond_init(&player_to_join, NULL);
    pthread_mutex_init(&general_mutex, NULL);

    /* 呼叫socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR 開啟socket");
        exit(1);
    }

    /* 初始化socket */
    bzero((char *) &server_address, sizeof(server_address));
    port_number = PORT;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    /* 綁定socket*/
    if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("ERROR 綁定");
        exit(1);
    }

    /* MAX_QUEUE */
    listen(sockfd, 20);
    printf("Server 監聽 port %d\n", port_number);

    
    while(1) {
        client_length = sizeof(client);
        // 檢查是否有玩家在等待

        /* 連接client端 */
        client_socket = accept(sockfd, (struct sockaddr *)&client, (unsigned int *)&client_length);
        printf("– 連線來自 %d.%d.%d.%d:%d –\n",client.sin_addr.s_addr&0xFF, (client.sin_addr.s_addr&0xFF00)>>8, (client.sin_addr.s_addr&0xFF0000)>>16, (client.sin_addr.s_addr&0xFF000000)>>24, client.sin_port);
        if(i == 1){
            player1 = client_socket;
            i = 0;
        }
        
        
        if (client_socket < 0) {
            perror("ERROR 接收");
            exit(1);
        }

        // 無玩家等待，建立thread （game room）
        if (player_is_waiting == 0) {
            printf("玩家一加入中，開啟新局......\n");
            pthread_create(&tid[0], NULL, &game_room, &client_socket);
        }
        // 有玩家等待
        else {
            // Send user two signal
            printf("玩家二加入中，遊戲開始.....\n");
            challenging_player = client_socket;
            player2 = client_socket;
            pthread_cond_signal(&player_to_join);
        }
        if(END == 1)
        break;
    }

    close(sockfd);

    return 0;
}
