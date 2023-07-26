#pragma once

enum
{
    BLOCK_COUNT = 4,

    FIELD_WIDTH = 16,
    FIELD_HEIGHT = 16,
};

typedef enum bool
{
    FALSE,
    TRUE
} bool_t;

typedef enum block_type
{
    BLOCK_TYPE_I,
    BLOCK_TYPE_O,
    BLOCK_TYPE_T,
    BLOCK_TYPE_L,
    BLOCK_TYPE_J,
    BLOCK_TYPE_S,
    BLOCK_TYPE_Z,
    BLOCK_TYPE_COUNT
} block_type_t;

typedef struct
{
    int x;
    int y;
} vector_t;

typedef struct
{
    vector_t origin;
    vector_t vectors[BLOCK_COUNT - 1];

    block_type_t type;

} block_t;

void play_game(void);
