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
static bool_t is_collision(const char (*board)[TOTAL_WIDTH], const block_t* block);
static void time_delay();
static void clear_full_lines(char (*board)[TOTAL_WIDTH]);

enum {
    DELAY_TIME = 100
};

static const block_t s_SHAPES[BLOCK_TYPE_COUNT] = {
    { { 4, 1 }, { { -1, 0 }, { 1, 0 }, { 2, 0 } }, BLOCK_TYPE_I },
    { { 4, 1 }, { { 1, 0 }, { 0, 1 }, { 1, 1 } }, BLOCK_TYPE_O }, 
    { { 4, 2 }, { { -1, 0 }, { 0, -1 }, { 1, 0 } }, BLOCK_TYPE_T }, 
    { { 4, 2 }, { { -2, 0 }, { -1, 0 }, { 0, -1 } }, BLOCK_TYPE_L }, 
    { { 2, 2 }, { { 2, 0 }, { 1, 0 }, { 0, -1 } }, BLOCK_TYPE_J }, 
    { { 4, 1 }, { { 1, 0 }, { 0, 1 }, { -1, 1 } }, BLOCK_TYPE_S }, 
    { { 4, 1 }, { { -1, 0 }, { 0, 1 }, { 1, 1 } }, BLOCK_TYPE_Z }, 
};

void play_game(void)
{
    char board[TOTAL_HEIGHT][TOTAL_WIDTH];

    int y;
    int x;

    block_t block;
    block_t tmp_block;

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
    system("cls");

    srand(time(NULL));

    while (TRUE) {
        system("cls");
        block = s_SHAPES[rand() % BLOCK_TYPE_COUNT];

        if (!is_collision(board, &block)) {
            draw_block(board, &block);
        } else {
            printf("Game Over!");
            break;
        }
        print(board);
        time_delay();

        while (TRUE) {

            int start = clock();

            tmp_block = block;
            while (clock() < start + DELAY_TIME) {

                if (_kbhit()) {

                    switch(_getch()) {

                    case 'q':
                        tmp_block = rotate_CCW(&block);
                        break;
    
                    case 'e':
                        tmp_block = rotate_CW(&block);
                        break;

                    case 'a':
                        tmp_block = block;
                        tmp_block.origin.x -= 1;
                        break;

                    case 'd':
                        tmp_block = block;
                        tmp_block.origin.x += 1;
                        break;

                    default:
                        continue;
                    }

                    clear_block(board, &block);
                    if (!is_collision(board, &tmp_block)) {
                        block = tmp_block;
                    }
                    draw_block(board, &block);
                }

                system("cls");
                print(board);
                time_delay();
            }

            tmp_block = block;
            tmp_block.origin.y += 1;

            clear_block(board, &block);
            if (!is_collision(board, &tmp_block)) {
                block = tmp_block;
            } else {
                break;
            }
            draw_block(board, &block);

            system("cls");
            print(board);
        }
        draw_block(board, &block);

        clear_full_lines(board);
    }
}

static void print(const char (*board)[TOTAL_WIDTH])
{
    int y;
    for (y = 0; y < TOTAL_HEIGHT; ++y) {

        int x;
        const char* p_row = board[y];
        for (x = 0; x < TOTAL_WIDTH; ++x) {
            printf("%c", p_row[x]);
        }
        printf("\n");
    }
}

static void draw_block(char (*board)[TOTAL_WIDTH], const block_t* block)
{
    board[block->origin.y][block->origin.x] = 'x';
    board[block->origin.y + block->vectors[0].y][block->origin.x + block->vectors[0].x] = 'o';
    board[block->origin.y + block->vectors[1].y][block->origin.x + block->vectors[1].x] = 'o';
    board[block->origin.y + block->vectors[2].y][block->origin.x + block->vectors[2].x] = 'o';
}

static void clear_block(char (*board)[TOTAL_WIDTH], const block_t* block)
{
    board[block->origin.y][block->origin.x] = ' ';
    board[block->origin.y + block->vectors[0].y][block->origin.x + block->vectors[0].x] = ' ';
    board[block->origin.y + block->vectors[1].y][block->origin.x + block->vectors[1].x] = ' ';
    board[block->origin.y + block->vectors[2].y][block->origin.x + block->vectors[2].x] = ' ';
}

static block_t rotate_CW(const block_t* block)
{
    block_t result = *block;

    int i;

    if (block->type == BLOCK_TYPE_O) {
        goto ret;
    }

    for (i = OTHER_POINT_COUNT - 1; i >= 0; --i) {
        int x = (result.vectors)[i].x;
        int y = (result.vectors)[i].y;

        (result.vectors)[i].x = -y;
        (result.vectors)[i].y = x;
    }

ret:
    return result;
}

static block_t rotate_CCW(const block_t* block)
{
    block_t result = *block;

    int i;

    if (block->type == BLOCK_TYPE_O) {
        goto ret;
    }

    for (i = OTHER_POINT_COUNT - 1; i >= 0; --i) {
        int x = (result.vectors)[i].x;
        int y = (result.vectors)[i].y;

        (result.vectors)[i].x = y;
        (result.vectors)[i].y = -x;
    }

ret:
    return result;
}

static bool_t is_collision(const char (*board)[TOTAL_WIDTH], const block_t* block)
{
    int i;

    if (board[block->origin.y][block->origin.x] != ' ') {
        return TRUE;
    }

    for (i = OTHER_POINT_COUNT - 1; i >= 0; --i) {

        int x = block->origin.x + (block->vectors)[i].x;
        int y = block->origin.y + (block->vectors)[i].y;

        if (board[y][x] != ' ') {
            return TRUE;
        }
    }

    return FALSE;
}

static void time_delay()
{
    const int DELAY = 35;

    int start = clock();

    while (clock() < start + DELAY) {
    }
}

static void clear_full_lines(char (*board)[TOTAL_WIDTH])
{
    int y;
    int x;
    char* p_curr_row;
    for (y = TOTAL_HEIGHT - 2; y > 1;) {

        p_curr_row = board[y];        
        for (x = TOTAL_WIDTH - 2; x >= 1; --x) {

            if (p_curr_row[x] == ' ') {
                break;
            }
        }

        if (x == 0) {

            int i;
            for (i = y; i > 1; --i) {

                char* p_above_row = board[i - 1];
                p_curr_row = board[i];

                memcpy(p_curr_row, p_above_row, sizeof(char) * TOTAL_WIDTH);
            }
        } else {
            --y;
        }
    }

    p_curr_row = board[y];
    for (x = TOTAL_WIDTH - 2; x >= 1; --x) {

        if (p_curr_row[x] == ' ') {
            break;
        }
    }

    if (x == 0) {
        memset(p_curr_row + 1, ' ', sizeof(char) * FIELD_WIDTH);
    }
}
