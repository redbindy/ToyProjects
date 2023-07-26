#include "tetris.h"

#include "stdio.h"
#include "string.h"
#include "conio.h"
#include "stdlib.h"
#include "time.h"

static void print(const char* board);
static void draw_block(char* board, const block_t* block);
static void clear_block(char* board, const block_t* block);
static void rotate_CW(block_t* block);
static void rotate_CCW(block_t* block);
static bool_t is_collision(const char* board, const block_t* block);
static void time_delay();
static void clear_full_lines(char* board);

static const block_t s_SHAPES[BLOCK_TYPE_COUNT] = {
    { { 4, 0 }, { { -1, 0 }, { 1, 0 }, { 2, 0 } }, BLOCK_TYPE_I }, 
    { { 4, 0 }, { { 1, 0 }, { 0, 1 }, { 1, 1 } }, BLOCK_TYPE_O }, 
    { { 4, 1 }, { { -1, 0 }, { 0, -1 }, { 1, 0 } }, BLOCK_TYPE_T }, 
    { { 4, 1 }, { { -2, 0 }, { -1, 0 }, { 0, -1 } }, BLOCK_TYPE_L }, 
    { { 2, 1 }, { { 2, 0 }, { 1, 0 }, { 0, -1 } }, BLOCK_TYPE_J }, 
    { { 4, 0 }, { { 1, 0 }, { 0, 1 }, { -1, 1 } }, BLOCK_TYPE_S }, 
    { { 4, 0 }, { { -1, 0 }, { 0, 1 }, { 1, 1 } }, BLOCK_TYPE_Z }
};

enum {
    ROW_DATA_LENGTH = FIELD_WIDTH + 1, 
    INPUT_DELAY_TIME = 200
};

// static char primary[FIELD_HEIGHT][ROW_DATA_LENGTH];
static char secondary[FIELD_HEIGHT][ROW_DATA_LENGTH];

void play_game(void)
{
    const char* starting_msg = "Press Any Key";
    const char* beep_sound = "\a";

    const char* game_over_msg = "Game Over!";

    const char* pause_argu = "pause";

    block_t block;
    block_t tmp_block;

    __asm {

        mov eax, 0x20202020
        movd xmm0, eax

        punpckldq xmm0, xmm0

        mov ecx, ROW_DATA_LENGTH*FIELD_HEIGHT
        init_loop:
            sub ecx, ROW_DATA_LENGTH

            // movq QWORD PTR primary[ecx], xmm0
            // movq QWORD PTR primary[ecx + 8], xmm0

            movq QWORD PTR secondary[ecx], xmm0
            movq QWORD PTR secondary[ecx + 8], xmm0

        jns init_loop

        push starting_msg
        {
            call printf
        }
        add esp, 4

        check_loop:

            call _kbhit

        cmp eax, FALSE
        je check_loop

        xor eax, eax
        push eax // NULL
        {
            call time
        }
        add esp, 4

        push eax // time()의 반환값
        {
            call srand
        }
        add esp, 4

        lea esi, block // block 포인터용

        game_loop:

            call rand

            and eax, BLOCK_TYPE_COUNT-1
            imul eax, eax, TYPE block_t
            add eax, OFFSET s_SHAPES

            push TYPE block_t // sizeof(block_t)
            push eax
            push esi
            {
                call memcpy
            }
            add esp, 4*3

            push esi
            push OFFSET secondary
            {
                call is_collision
            }
            add esp, 4*2

            cmp eax, FALSE
            jne game_over

            push esi
            push OFFSET secondary
            {
                call draw_block
            }
            add esp, 4*2

            push OFFSET secondary
            {
                call print
            }
            add esp, 4

            ctr_loop:

                call clock

                mov edi, eax
                add edi, INPUT_DELAY_TIME // start = clock() + DELAY_TIME

                lea eax, tmp_block

                push TYPE block_t
                push esi
                push eax
                {
                    call memcpy
                }
                add esp, 4*3

                ltm_loop:
                    call clock
                    cmp eax, edi
                    jae time_out

                    push esi
                    push OFFSET secondary
                    {
                        call clear_block
                    }
                    add esp, 4*2

                    call _kbhit
                    cmp eax, FALSE
                    je next

                // input_q:
                    call _getch
                    cmp eax, 'q'
                    jne input_e

                    mov ecx, 4
                    roq_loop:

                        lea edx, tmp_block

                        push ecx
                        push edx
                        {
                            call rotate_CCW
                        }
                        pop edx

                        push edx
                        push OFFSET secondary
                        {
                            call is_collision
                        }
                        add esp, 4*2

                        pop ecx

                        cmp eax, FALSE
                        je app_bloc

                    loop roq_loop

                    jmp next

                input_e:
                    cmp eax, 'e'
                    jne input_a

                    mov ecx, 4
                    roe_loop:

                        lea edx, tmp_block

                        push ecx
                        push edx
                        {
                            call rotate_CW
                        }
                        pop edx

                        push edx
                        push OFFSET secondary
                        {
                            call is_collision
                        }
                        add esp, 4*2

                        pop ecx

                        cmp eax, FALSE
                        je app_bloc

                    loop roe_loop

                    jmp next

                input_a:
                    cmp eax, 'a'
                    jne input_d

                    dec [tmp_block].origin.x

                    jmp chk_bloc

                input_d:
                    cmp eax, 'd'
                    jne input_s

                    inc [tmp_block].origin.x

                    jmp chk_bloc

                input_s:
                    cmp eax, 's'
                    jne next

                    jmp time_out

                chk_bloc:

                    lea eax, tmp_block

                    push eax
                    push OFFSET secondary
                    {
                        call is_collision
                    }
                    add esp, 4*2

                    cmp eax, FALSE
                    jne next

                app_bloc:

                    lea eax, tmp_block

                    push TYPE block_t
                    push eax
                    push esi
                    {
                        call memcpy
                    }
                    add esp, 4*3

                next:

                    push esi
                    push OFFSET secondary
                    {
                        call draw_block
                    }
                    add esp, 8

                    push OFFSET secondary
                    {
                        call print
                    }
                    add esp, 4

                jmp ltm_loop

            time_out:

                lea edi, tmp_block

                push TYPE block_t
                push esi
                push edi
                {
                    call memcpy
                }
                add esp, 4*3

                inc [edi].origin.y

                push esi
                push OFFSET secondary
                {
                    call clear_block
                }
                add esp, 4*2

                push edi
                push OFFSET secondary
                {
                    call is_collision
                }
                add esp, 4*2

                cmp eax, FALSE
                jne next_itr

                push TYPE block_t
                push edi
                push esi
                {
                    call memcpy
                }
                add esp, 4*3

                push esi
                push OFFSET secondary
                {
                    call draw_block
                }
                add esp, 4*2

            jmp ctr_loop

        next_itr:

            push esi
            push OFFSET secondary
            {
                call draw_block
            }
            add esp, 4*2

            push OFFSET secondary
            {
                call clear_full_lines
            }
            add esp, 4

            push beep_sound
            {
                call printf
            }
            add esp, 4

        jmp game_loop

        game_over:
        
        push game_over_msg
        {
            call printf
        }
        add esp, 4

        push pause_argu
        {
            call system
        }
        add esp, 4
    }
}

static void print(const char* board)
{
    const char* clear_argu = "cls";

    const char* top_bottom_frame = "------------------\n";

    const char* format_row = "%c%s%c\n";

    const char* const P_BOARD_END = board + ROW_DATA_LENGTH * FIELD_WIDTH;

    __asm {

        push clear_argu
        {
            call system
        }
        add esp, 4

        push top_bottom_frame
        {
            call printf
        }
        add esp, 4

        mov ebx, board
        oloop:
            
            push '|'
            push ebx
            push '|'
            push format_row
            {
                call printf
            }
            add esp, 4*4

            add ebx, ROW_DATA_LENGTH

        cmp ebx, P_BOARD_END
        jb oloop

        push top_bottom_frame
        {
            call printf
        }
        add esp, 4

        call time_delay
    }
}

static void draw_block(char* board, const block_t* block)
{
    __asm {

        mov esi, board
        mov edi, block

        mov eax, [edi].origin.y
        imul eax, eax, ROW_DATA_LENGTH
        add eax, [edi].origin.x
        add eax, esi

        mov edx, eax // origin 위치 저장

        mov [eax], 'o'

        lea edi, [edi].vectors

        mov eax, [edi].y
        imul eax, eax, ROW_DATA_LENGTH
        add eax, [edi]

        add eax, edx

        mov [eax], 'o'

        mov eax, [edi+(TYPE vector_t)].y
        imul eax, eax, ROW_DATA_LENGTH
        add eax, [edi+(TYPE vector_t)]

        add eax, edx

        mov [eax], 'o'

        mov eax, [edi+(TYPE vector_t)*2].y
        imul eax, eax, ROW_DATA_LENGTH
        add eax, [edi+(TYPE vector_t)*2]

        add eax, edx

        mov [eax], 'o'
    }
}

static void clear_block(char* board, const block_t* block)
{
    __asm {

        mov esi, board
        mov edi, block

        mov eax, [edi].origin.y
        imul eax, eax, ROW_DATA_LENGTH
        add eax, [edi].origin.x
        add eax, esi

        mov edx, eax // origin 위치 저장

        mov [eax], ' '

        lea edi, [edi].vectors

        mov eax, [edi].y
        imul eax, eax, ROW_DATA_LENGTH
        add eax, [edi]

        add eax, edx

        mov [eax], ' '

        mov eax, [edi + (TYPE vector_t)].y
        imul eax, eax, ROW_DATA_LENGTH
        add eax, [edi + (TYPE vector_t)]

        add eax, edx

        mov [eax], ' '

        mov eax, [edi + (TYPE vector_t) * 2].y
        imul eax, eax, ROW_DATA_LENGTH
        add eax, [edi + (TYPE vector_t) * 2]

        add eax, edx

        mov [eax], ' '
    }
}

static void rotate_CW(block_t* block)
{
    __asm {

        mov eax, block

        cmp [eax].type, BLOCK_TYPE_O
        je end

        lea edx, [eax].vectors

        mov ecx, (BLOCK_COUNT-2)*(TYPE vector_t)
        rot_loop:

            mov esi, [edx+ecx]
            mov edi, [edx+ecx].y

            neg edi

            mov [edx+ecx], edi
            mov [edx+ecx].y, esi

            sub ecx, TYPE vector_t
        jns rot_loop

        end:
    }
}

static void rotate_CCW(block_t* block)
{
    __asm {

        mov eax, block

        cmp [eax].type, BLOCK_TYPE_O
        je end

        lea edx, [eax].vectors

        mov ecx, (BLOCK_COUNT-2)*(TYPE vector_t)
        rot_loop:

            mov esi, [edx+ecx]
            mov edi, [edx+ecx].y

            neg esi

            mov [edx+ecx], edi
            mov [edx+ecx].y, esi

            sub ecx, TYPE vector_t
        jns rot_loop

        end:
    }
}

static bool_t is_collision(const char* board, const block_t* block)
{
    __asm {

        mov edi, block

        mov eax, [edi].origin.x
        cmp eax, 0
        jl collision

        cmp eax, ROW_DATA_LENGTH
        jge collision

        mov eax, [edi].origin.y
        cmp eax, 0
        jl collision

        cmp eax, FIELD_HEIGHT
        jge collision

        imul eax, eax, ROW_DATA_LENGTH
        add eax, [edi].origin.x
        add eax, board

        cmp [eax], ' '
        jne collision

        lea esi, [edi].vectors

        mov ecx, (BLOCK_COUNT-2)*(TYPE vector_t)
        chk_loop:

            mov edx, [edi].origin.x
            add edx, [esi+ecx]

            cmp edx, 0
            jl collision

            cmp edx, ROW_DATA_LENGTH
            jge collision

            mov eax, [edi].origin.y
            add eax, [esi+ecx].y

            cmp eax, 0
            jl collision

            cmp eax, FIELD_HEIGHT
            jge collision

            imul eax, eax, ROW_DATA_LENGTH
            add eax, edx

            add eax, board

            cmp [eax], ' '
            jne collision

            sub ecx, TYPE vector_t
        jns chk_loop

        xor eax, eax

        collision:
    }
}

static void time_delay()
{
    enum {
        DELAY = 20
    };

    __asm {
        call clock

        mov edi, eax 
        add edi, DELAY // start = clock() + DELAY;

        dly_loop:

            call clock
            
        cmp eax, edi
        jb dly_loop
    }
}

static void clear_full_lines(char* board)
{
    __asm {
        mov esi, board

        mov ecx, 4*3
        chf_loop:

            mov eax, [esi+ecx]
            cmp eax, 0x6F6F6F6F
            jne nxt_line

            sub ecx, 4
        jns chf_loop

        mov eax, ' '
        movd xmm0, eax
        punpcklbw xmm0, xmm0
        punpcklwd xmm0, xmm0
        punpckldq xmm0, xmm0

        movq QWORD PTR [esi], xmm0
        movq QWORD PTR [esi+8], xmm0

        nxt_line:
        mov ecx, (FIELD_HEIGHT - 1)*(ROW_DATA_LENGTH)
        cr_loop:

            mov edx, 4*3
            chk_loop:

                mov edi, ecx
                add edi, edx

                mov eax, [esi+edi]
                cmp eax, 0x6F6F6F6F
                jne next

                sub edx, 4
            jns chk_loop

            mov edx, ecx
            clr_loop:

                movq xmm0, QWORD PTR [esi+edx-ROW_DATA_LENGTH]
                movq QWORD PTR [esi+edx], xmm0

                movq xmm0, QWORD PTR [esi+edx-ROW_DATA_LENGTH+8]
                movq QWORD PTR [esi+edx+8], xmm0

                sub edx, ROW_DATA_LENGTH
            jnz clr_loop

            add ecx, ROW_DATA_LENGTH

        next:
            sub ecx, ROW_DATA_LENGTH
        jns cr_loop
    }
}
