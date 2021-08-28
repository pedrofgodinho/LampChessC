#ifndef CHESS_H
#define CHESS_H

#include "utils.h"

/*******************************
 * Board Structs
 *******************************/
typedef struct 
{
    u64 bitboards[12];
    u64 occupancies[3];
    int side;
    int enpassant;
    int castle;
} board_t;

typedef struct 
{
    int moves[256];
    int count;
} move_list_t;

typedef struct
{
    board_t boards[512];
    int ply;
} board_stack_t;


/*******************************
 * Stack Manipulation Macros
 *******************************/
#define stack_push(stack) \
    do { \
        memcpy((stack)->boards + (stack)->ply+1, (stack)->boards + (stack)->ply, sizeof(board_t)); \
        (stack)->ply++; \
    } while(0)
#define stack_pop(stack) ((stack)->ply--)
#define stack_reset(stack) ((stack)->ply = 0)
#define stack_current(stack) ((stack)->boards + (stack)->ply)


/*******************************
 * Move Manipulation Macros
 *******************************/
#define encode_move(source, target, piece, promoted, capture, double_push, enpassant, castling) \
    (source) | ((target) << 6) | ((piece) << 12) | ((promoted) << 16) | \
    ((capture) << 20) | ((double_push) << 21) | ((enpassant) << 22) | ((castling) << 23)

#define add_move(ml, move) \
    do { \
        (ml)->moves[ml->count] = move; \
        (ml)->count++; \
    } while (0)

#define get_move_source(move)      ((move) & 0x3f)
#define get_move_target(move)      (((move) & 0xfc0) >> 6)
#define get_move_piece(move)       (((move) & 0xf000) >> 12)
#define get_move_promoted(move)    (((move) & 0xf0000) >> 16)
#define get_move_capture(move)     (((move) & 0x100000))
#define get_move_double_push(move) (((move) & 0x200000))
#define get_move_enpassant(move)   (((move) & 0x400000))
#define get_move_castling(move)    (((move) & 0x800000))


/*******************************
 * Functions
 *******************************/
board_stack_t* make_stack();
void destroy_stack(board_stack_t *stack);

void generate_moves(board_t *board, move_list_t *moves);
int make_move(board_t *board, int move);
int make_move_if_capture(board_t *board, int move);
int is_square_attacked(board_t *board, int square, int side);

u64 perft(board_stack_t *stack, int depth);
u64 divide(board_stack_t *stack, int depth);
u64 timed_perft(board_stack_t *stack, int depth);
u64 timed_divide(board_stack_t *stack, int depth);

void print_bitboard(u64 bitboard);
void print_board(board_t *board, int unicode);
void print_moves(move_list_t *moves);


#endif
