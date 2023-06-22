#include "ascii_tetris.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

static void print(const char (*board)[TOTAL_WIDTH]);
static void draw_block(char (*board)[TOTAL_WIDTH], const block_t* block);
static void clear_block(char (*board)[TOTAL_WIDTH], const block_t* block);
static block_t rotate_CW(const block_t* block);
static block_t rotate_CCW(const block_t* block);
static void time_delay(void);

enum {
    DIMENSION = 2
};

static const block_t s_SHAPES[BLOCK_TYPE_COUNT] = {
    { { 4, 5 }, { { -1, 0 }, { 1, 0 }, { 2, 0 } } },
};

void play_game(void)
{
    char board[TOTAL_HEIGHT][TOTAL_WIDTH];

    int y;
    int x;

    block_t block;
    block_t rotated_block;

    memset(board, ' ', TOTAL_HEIGHT * TOTAL_WIDTH);

    for (y = 0; y < TOTAL_HEIGHT; ++y) {
        board[y][0] = '|';
        board[y][FIELD_WIDTH + 1] = '|';
    }

    for (x = 0; x < TOTAL_WIDTH; ++x) {
        board[0][x] = '-';
        board[FIELD_HEIGHT + 1][x] = '-';
    }

    printf("Press Any Key");
    while (!_kbhit()) {
    }
    time_delay();
    system("cls");

    block = s_SHAPES[0];
    while (TRUE) {

        print(board);
        
        
        while (TRUE) {
        }

        if (_kbhit()) {
        
            rotated_block = block;
            switch (_getch()) {

            case 'a':
                rotated_block = rotate_CCW(&block);
                break;

            case 'd':
                rotated_block = rotate_CW(&block);
                break;

            case 'w':
                break;

            default:
                break;
            }

            clear_block(board, &block);
            block = rotated_block;
            draw_block(board, &block);

        } else {

             int iter;

             vector_t p0 = block.rotation_axis;
             p0.y += 1;             

             clear_block(board, &block);

             if (board[p0.y][p0.x] != ' ') {
                 break;
             }

             for (iter = 0; iter < OTHER_POINT_COUNT; ++iter) {

                 vector_t p = p0;
                 p.x += (block.vectors)[iter].x;
                 p.y += (block.vectors)[iter].y;

                 if (board[p.y][p.x] != ' ') {
                     goto end;
                 }
             }

             block.rotation_axis.y += 1;

             draw_block(board, &block);
        }

        system("cls");
    }

end:
;
}

static void print(const char (*board)[TOTAL_WIDTH])
{
    int x;
    int y;

    for (y = 0; y < TOTAL_HEIGHT; ++y) {
        for (x = 0; x < TOTAL_WIDTH; ++x) {
            printf("%c", board[y][x]);
        }
        printf("\n");
    }
}

static void draw_block(char (*board)[TOTAL_WIDTH], const block_t* block)
{
    vector_t center = block->rotation_axis;

    int i;
    for (i = 0; i < OTHER_POINT_COUNT; ++i) {
        vector_t vector = (block->vectors)[i];

        board[center.y + vector.y][center.x + vector.x] = 'x';
    }

    board[center.y][center.x] = 'o';
}

static void clear_block(char (*board)[TOTAL_WIDTH], const block_t* block)
{
    vector_t center = block->rotation_axis;

    int i;
    for (i = 0; i < OTHER_POINT_COUNT; ++i) {
        vector_t vector = (block->vectors)[i];

        board[center.y + vector.y][center.x + vector.x] = ' ';
    }

    board[center.y][center.x] = ' ';
}

static block_t rotate_CW(const block_t* block)
{
    block_t result = *block;

    int i;
    for (i = 0; i < OTHER_POINT_COUNT; ++i) {
        int x = (result.vectors)[i].x;
        int y = (result.vectors)[i].y;

        (result.vectors)[i].x = -y;
        (result.vectors)[i].y = x;
    }

    return result;
}

static block_t rotate_CCW(const block_t* block)
{
    block_t result = *block;

    int i;
    for (i = 0; i < OTHER_POINT_COUNT; ++i) {
        int x = (result.vectors)[i].x;
        int y = (result.vectors)[i].y;

        (result.vectors)[i].x = y;
        (result.vectors)[i].y = -x;
    }

    return result;
}

static void time_delay(void)
{
    const int DELAY_TIME = 300;
    clock_t start_time = clock();

    while (clock() < start_time + DELAY_TIME) {
    }
}
