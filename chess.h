#ifndef CHESS_H
#define CHESS_H

#include "utils.h"

// Board structs
typedef struct 
{
    U64 bitboards[12];
    U64 occupancies[3];
    int side;
    int enpassant;
    int castle;
} board;

typedef struct 
{
    int moves[256];
    int count;
} move_list;

typedef struct
{
    board boards[512];
    int ply;
} board_stack;


// Stack Macros
#define stack_push(stack) \
    do { \
        memcpy((stack)->boards + (stack)->ply+1, (stack)->boards + (stack)->ply, sizeof(board)); \
        (stack)->ply++; \
    } while(0)
#define stack_pop(stack) ((stack)->ply--)
#define stack_current(stack) ((stack)->boards + (stack)->ply)


// Move Macros
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


#endif
