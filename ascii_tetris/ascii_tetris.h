#ifndef ASCII_TETRIS_H
#define ASCII_TETRIS_H

#define _CRT_SECURE_NO_WARNINGS

enum {
    OTHER_POINT_COUNT = 3,

    FIELD_WIDTH = 10,
    FIELD_HEIGHT = 20,

    TOTAL_WIDTH = 10 + 2,
    TOTAL_HEIGHT = 20 + 2
};

typedef enum bool {
    FALSE,
    TRUE
} bool_t;

typedef enum block_type {

    BLOCK_TYPE_I,
    BLOCK_TYPE_O,
    BLOCK_TYPE_T,
    BLOCK_TYPE_L,
    BLOCK_TYPE_J,
    BLOCK_TYPE_S,
    BLOCK_TYPE_Z,
    BLOCK_TYPE_COUNT

} block_type_t;

typedef struct {

    int x;
    int y;

} vector_t;

typedef struct {

    vector_t rotation_axis;
    vector_t vectors[OTHER_POINT_COUNT];

} block_t;

void play_game(void);

#endif /* ASCII_TETRIS_H */
