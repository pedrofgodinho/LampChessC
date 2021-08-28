#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ai.h"

// Alphazero Calculated Values
const int material_value[] = 
{
    100, 
    305,
    333,
    563,
    950,
    11000,
    -100,
    -305,
    -333,
    -563,
    -950,
    -11000,
};

// Piece Square Tables
const int pst_aux[6][64] =
{
    { // PAWN
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0,
    },
    { // KNIGHT
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    },
    { // BISHOP
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    },
    { // ROOK
          0,  0,  0,  0,  0,  0,  0,  0,
          5, 10, 10, 10, 10, 10, 10,  5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
          0,  0,  0,  5,  5,  0,  0,  0,
    },
    { // QUEEN
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20,
    },
    { // KING
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20,
    },
};

int pst[12][64];

void init_ai()
{
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 64; j++)
            pst[i][j] = pst_aux[i][j];
    for (int i = 6; i < 12; i++)
        for (int rank = 0; rank < 8; rank++)
            for (int file = 0; file < 8; file++)
                pst[i][rank * 8 + file] = -pst[i - 6][(7 - rank) * 8 + file];
}

int evaluate(board_t *board)
{
    int score = 0;

    for (int piece = 0; piece < 12; piece++)
    {
        for (u64 bitboard = board->bitboards[piece]; bitboard; unset_ls1b(bitboard))
        {
            score += material_value[piece] + pst[piece][get_ls1b_index(bitboard)];
        }
    }

    return board->side == white ? score : -score;
}

int alpha_beta_no_line(board_stack_t *stack, int alpha, int beta, int depth)
{
    if (depth == 0)
        return evaluate(stack_current(stack));

    move_list_t moves;
    int score = 0;
    int legal_moves = 0;
    int in_check = is_square_attacked(stack_current(stack), get_ls1b_index(stack_current(stack)->side == white ? stack_current(stack)->bitboards[K] : stack_current(stack)->bitboards[k]), !white);
    searched_nodes++;

    generate_moves(stack_current(stack), &moves);
    for (int i = 0; i < moves.count; i++)
    {
        stack_push(stack);
        if (!make_move(stack_current(stack), moves.moves[i]))
        {
            stack_pop(stack);
            continue;
        }
        legal_moves++;
        score = -alpha_beta_no_line(stack, -beta, -alpha, depth - 1);
        stack_pop(stack);
        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    if (legal_moves == 0)
    {
        if (in_check)
            return -MATE + stack->ply;
        return 0;
    }
    return alpha;
}

int alpha_beta(board_stack_t *stack, int alpha, int beta, int depth, int *line)
{
    if (depth == 0)
        return evaluate(stack_current(stack));

    move_list_t moves;
    int score = 0;
    int legal_moves = 0;
    int in_check = is_square_attacked(stack_current(stack), get_ls1b_index(stack_current(stack)->side == white ? stack_current(stack)->bitboards[K] : stack_current(stack)->bitboards[k]), !white);
    searched_nodes = 0;
    searched_nodes++;

    generate_moves(stack_current(stack), &moves);
    for (int i = 0; i < moves.count; i++)
    {
        stack_push(stack);
        if (!make_move(stack_current(stack), moves.moves[i]))
        {
            stack_pop(stack);
            continue;
        }
        legal_moves++;
        score = -alpha_beta_no_line(stack, -beta, -alpha, depth - 1);
        stack_pop(stack);
        if (score >= beta)
            return beta;
        if (score > alpha)
        {
            alpha = score;
            *line = moves.moves[i];
        }
    }
    if (legal_moves == 0)
    {
        if (in_check)
            return -MATE + stack->ply;
        return 0;
    }

    return alpha;
}

