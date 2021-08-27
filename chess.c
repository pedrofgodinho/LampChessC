//mingw printf fix for u64
#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include "tables.h"
#include "chess.h"
#include "uci.h"


/*******************************
 * Basic IO
 *******************************/
void print_bitboard(u64 bitboard)
{
    // Prints a u64 bitboard

    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            if (file == 0)
                printf(" %d  ", 8 - rank);
            printf(" %c", get_bit(bitboard, rank * 8 + file) ? '1' : '.');
        }
        printf("\n");
    }
    printf("\n     a b c d e f g h\n\n");
    printf("      Bitboard: 0x%llx\n\n", bitboard);
}

void print_board(board_t *board, int unicode)
{
    // Prints a chess board to the terminal. If unicode is enabled, it will use unicode chess pieces

    // Pieces
    for (int rank = 0; rank < 8; rank++)
    {
        printf(" %c  ", '8' - rank);
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            int had_piece = 0;
            for (int piece = 0; piece < 12; piece++)
            {
                if (get_bit(board->bitboards[piece], square))
                {
                    if (unicode)
                        printf("%s ", unicode_pieces[piece]);
                    else
                        printf("%c ", ascii_pieces[piece]);
                    had_piece = 1;
                    break;
                }
            }
            if (!had_piece)
                printf(". ");
        }
        printf("\n");
    }
    printf("\n    a b c d e f g h\n\n");

    // Flags
    printf("    Side to move: %s\n", board->side == white ? "white" : "black");
    printf("    EnPassant Square: %s\n", square_to_coordinates[board->enpassant]);
    printf("    Castle Rights: %c%c%c%c \n\n", 
            board->castle & wk ? 'K' : '-',
            board->castle & wq ? 'Q' : '-',
            board->castle & bk ? 'k' : '-',
            board->castle & bq ? 'q' : '-'
            );
}

void print_moves(move_list_t *moves)
{
    // Prints a list of moves.

    printf("\n\tmove\tpiece\tcapture\tdouble\tenpass\tcastling\n\n");
    for (int i = 0; i < moves->count; i++)
    {
        int move = moves->moves[i];
        int p = get_move_promoted(move);
        printf("%d\t%s%s",
                i,
                square_to_coordinates[get_move_source(move)],
                square_to_coordinates[get_move_target(move)]);
        if (p)
        {
            printf("%c", tolower(ascii_pieces[p]));
        }
        printf("\t%c\t%s\t%s\t%s\t%s\n",
                ascii_pieces[get_move_piece(move)],
                get_move_capture(move) ? "yes" : "no",
                get_move_double_push(move) ? "yes" : "no",
                get_move_enpassant(move) ? "yes" : "no",
                get_move_castling(move) ? "yes" : "no"
                );
    }
    printf("\nNumber of Moves: %d\n\n", moves->count);
}


/*******************************
 * Move Generation
 *******************************/
int is_square_attacked(board_t *board, int square, int side)
{
    // Returns whether the square is attacked by a side. 
    // Ex: (board, e1, white) will return 1 if white attacks e1

    // queens
    if (get_queen_attacks(square, board->occupancies[both]) & ((side == white) ? board->bitboards[Q] : board->bitboards[q])) return 1;
    // rooks
    if (get_rook_attacks(square, board->occupancies[both]) & ((side == white) ? board->bitboards[R] : board->bitboards[r])) return 1;    
    // bishops
    if (get_bishop_attacks(square, board->occupancies[both]) & ((side == white) ? board->bitboards[B] : board->bitboards[b])) return 1;
    // knights
    if (get_knight_attacks(square) & ((side == white) ? board->bitboards[N] : board->bitboards[n])) return 1;
    // pawns
    if ((side == white) && (get_pawn_attacks(black, square) & board->bitboards[P])) return 1;
    if ((side == black) && (get_pawn_attacks(white, square) & board->bitboards[p])) return 1;
    // kings
    if (get_king_attacks(square) & ((side == white) ? board->bitboards[K] : board->bitboards[k])) return 1;

    return 0;
}

void generate_moves(board_t *board, move_list_t *moves)
{
    // Generates pseudo-legal moves into a move list

    int source_square, target_square;
    u64 bitboard, attacks;

    moves->count = 0;

    // Side dependent moves
    if (board->side == white)
    {
        // Quiet Pawn Moves
        bitboard = (board->bitboards[P] >> 8) & ~board->occupancies[both];

        // Single Push
        for (attacks = bitboard; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square + 8;
            if (target_square <= h8)
            {
                add_move(moves, encode_move(source_square, target_square, P, Q, 0, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, P, R, 0, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, P, B, 0, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, P, N, 0, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, P, 0, 0, 0, 0, 0));
            }
        }

        // Double Push
        for (attacks = (bitboard >> 8) & ~board->occupancies[both] & rank_4; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square + 16;
            add_move(moves, encode_move(source_square, target_square, P, 0, 0, 1, 0, 0));
        }

        // Captures
        for (attacks = (board->bitboards[P] >> 7) & not_a_file & board->occupancies[black]; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square + 7;
            if (target_square <= h8)
            {
                add_move(moves, encode_move(source_square, target_square, P, Q, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, P, R, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, P, B, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, P, N, 1, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, P, 0, 1, 0, 0, 0));
            }
        }
        for (attacks = (board->bitboards[P] >> 9) & not_h_file & board->occupancies[black]; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square + 9;
            if (target_square <= h8)
            {
                add_move(moves, encode_move(source_square, target_square, P, Q, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, P, R, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, P, B, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, P, N, 1, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, P, 0, 1, 0, 0, 0));
            }
        }
        if (board->enpassant != no_square)
        {
            for (attacks = (get_pawn_attacks(!board->side, board->enpassant)) & board->bitboards[P]; attacks; unset_ls1b(attacks))
            {
                source_square = get_ls1b_index(attacks);
                target_square = board->enpassant;
                add_move(moves, encode_move(source_square, target_square, P, 0, 1, 0, 1, 0));
            }
        }

        // Castling Kingside
        if (board->castle & wk)
        {
            if (!(get_bit(board->occupancies[both], f1) || 
                  get_bit(board->occupancies[both], g1) || 
                  is_square_attacked(board, e1, black) || 
                  is_square_attacked(board, f1, black)
                  ))
            {
                add_move(moves, encode_move(e1, g1, K, 0, 0, 0, 0, 1));
            }
        }
        // Castling Queenside
        if (board->castle & wq)
        {
            if (!(get_bit(board->occupancies[both], b1) || 
                  get_bit(board->occupancies[both], c1) || 
                  get_bit(board->occupancies[both], d1) || 
                  is_square_attacked(board, e1, black) || 
                  is_square_attacked(board, d1, black) 
                  ))
            {
                add_move(moves, encode_move(e1, c1, K, 0, 0, 0, 0, 1));
            }
        }
    }
    else
    {
        // Quiet Pawn Moves
        bitboard = (board->bitboards[p] << 8) & ~board->occupancies[both];

        // Single Push
        for (attacks = bitboard; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square - 8;
            if (target_square >= a1)
            {
                add_move(moves, encode_move(source_square, target_square, p, q, 0, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, p, r, 0, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, p, b, 0, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, p, n, 0, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, p, 0, 0, 0, 0, 0));
            }
        }

        // Double Push
        for (attacks = (bitboard << 8) & ~board->occupancies[both] & rank_5; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square - 16;
            add_move(moves, encode_move(source_square, target_square, p, 0, 0, 1, 0, 0));
        }

        // Captures
        for (attacks = (board->bitboards[p] << 7) & not_h_file & board->occupancies[white]; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square - 7;
            if (target_square >= a1)
            {
                add_move(moves, encode_move(source_square, target_square, p, q, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, p, r, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, p, b, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, p, n, 1, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, p, 0, 1, 0, 0, 0));
            }
        }
        for (attacks = (board->bitboards[p] << 9) & not_a_file & board->occupancies[white]; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square - 9;
            if (target_square >= a1)
            {
                add_move(moves, encode_move(source_square, target_square, p, q, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, p, r, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, p, b, 1, 0, 0, 0));
                add_move(moves, encode_move(source_square, target_square, p, n, 1, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, p, 0, 1, 0, 0, 0));
            }
        }
        if (board->enpassant != no_square)
        {
            for (attacks = (get_pawn_attacks(!board->side, board->enpassant)) & board->bitboards[p]; attacks; unset_ls1b(attacks))
            {
                source_square = get_ls1b_index(attacks);
                target_square = board->enpassant;
                add_move(moves, encode_move(source_square, target_square, p, 0, 1, 0, 1, 0));
            }
        }
        
        // Castling Kingside
        if (board->castle & bk)
        {
            if (!(get_bit(board->occupancies[both], f8) || 
                  get_bit(board->occupancies[both], g8) || 
                  is_square_attacked(board, e8, white) || 
                  is_square_attacked(board, f8, white)
                  ))
            {
                add_move(moves, encode_move(e8, g8, k, 0, 0, 0, 0, 1));
            }
        }
        // Castling Queenside
        if (board->castle & bq)
        {
            if (!(get_bit(board->occupancies[both], b8) || 
                  get_bit(board->occupancies[both], c8) || 
                  get_bit(board->occupancies[both], d8) || 
                  is_square_attacked(board, e8, white) || 
                  is_square_attacked(board, d8, white) 
                  ))
            {
                add_move(moves, encode_move(e8, c8, k, 0, 0, 0, 0, 1));
            }
        }
    }

    // Knight Moves
    for (bitboard = board->bitboards[board->side == white ? N : n]; bitboard; unset_ls1b(bitboard)) // Could be made branchless, explore later
    {
        source_square = get_ls1b_index(bitboard);

        for (attacks = get_knight_attacks(source_square) & ~board->occupancies[board->side]; attacks; unset_ls1b(attacks))
        {
            target_square = get_ls1b_index(attacks);

            // quiet or capture?
            if (get_bit(board->occupancies[!board->side], target_square))
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? N : n, 0, 1, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? N : n, 0, 0, 0, 0, 0));
            }
        }
    }
    
    // Bishop Moves
    for (bitboard = board->bitboards[board->side == white ? B : b]; bitboard; unset_ls1b(bitboard)) // Could be made branchless, explore later
    {
        source_square = get_ls1b_index(bitboard);

        for (attacks = get_bishop_attacks(source_square, board->occupancies[both]) & ~board->occupancies[board->side]; attacks; unset_ls1b(attacks))
        {
            target_square = get_ls1b_index(attacks);

            // quiet or capture?
            if (get_bit(board->occupancies[!board->side], target_square))
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? B : b, 0, 1, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? B : b, 0, 0, 0, 0, 0));
            }
        }
    }

    // Rook Moves
    for (bitboard = board->bitboards[board->side == white ? R : r]; bitboard; unset_ls1b(bitboard)) // Could be made branchless, explore later
    {
        source_square = get_ls1b_index(bitboard);

        for (attacks = get_rook_attacks(source_square, board->occupancies[both]) & ~board->occupancies[board->side]; attacks; unset_ls1b(attacks))
        {
            target_square = get_ls1b_index(attacks);

            // quiet or capture?
            if (get_bit(board->occupancies[!board->side], target_square))
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? R : r, 0, 1, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? R : r, 0, 0, 0, 0, 0));
            }
        }
    }

    // Queen Moves
    for (bitboard = board->bitboards[board->side == white ? Q : q]; bitboard; unset_ls1b(bitboard)) // Could be made branchless, explore later
    {
        source_square = get_ls1b_index(bitboard);

        for (attacks = (get_bishop_attacks(source_square, board->occupancies[both]) | get_rook_attacks(source_square, board->occupancies[both])) & ~board->occupancies[board->side]; attacks; unset_ls1b(attacks))
        {
            target_square = get_ls1b_index(attacks);

            // quiet or capture?
            if (get_bit(board->occupancies[!board->side], target_square))
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? Q : q, 0, 1, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? Q : q, 0, 0, 0, 0, 0));
            }
        }
    }

    // King Moves
    for (bitboard = board->bitboards[board->side == white ? K : k]; bitboard; unset_ls1b(bitboard)) // Could be made branchless, explore later
    {
        source_square = get_ls1b_index(bitboard);

        for (attacks = get_king_attacks(source_square) & ~board->occupancies[board->side]; attacks; unset_ls1b(attacks))
        {
            target_square = get_ls1b_index(attacks);

            // quiet or capture?
            if (get_bit(board->occupancies[!board->side], target_square))
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? K : k, 0, 1, 0, 0, 0));
            }
            else
            {
                add_move(moves, encode_move(source_square, target_square, board->side == white ? K : k, 0, 0, 0, 0, 0));
            }
        }
    }
}


/*******************************
 * Board Manipulation
 *******************************/
board_stack_t* make_stack()
{
    board_stack_t *stack = malloc(sizeof(board_stack_t)); 
    memset(stack, 0ULL, sizeof(board_stack_t));
    return stack;
}

void destroy_stack(board_stack_t* stack)
{
    free(stack);
}

int make_move(board_t *board, int move)
{
    // Tries to make a move on the board. If the king would've been left in check, returns 0, else returns 1
    // If zero is returned, the move is still made. It is the caller's responsibility to undo it.

    int source_square = get_move_source(move);
    int target_square = get_move_target(move);
    int piece = get_move_piece(move);
    int promoted = get_move_promoted(move);
    int capture = get_move_capture(move);
    int castling = get_move_castling(move);
    int enpassant = get_move_enpassant(move);
    int double_push = get_move_double_push(move);

    // Remove the piece
    unset_bit(board->bitboards[piece], source_square);
    // Handle Capture
    if (capture)
    {
        if (enpassant)
        {
            if (board->side == white)
            {
                unset_bit(board->bitboards[p], target_square + 8);
                unset_bit(board->occupancies[black], target_square + 8);
            }
            else
            {
                unset_bit(board->bitboards[P], target_square - 8);
                unset_bit(board->occupancies[white], target_square - 8);
            }
        }
        else
        {
            // If capturing a piece on corner, disable castling that way
            if (target_square == a1)
                board->castle &= ~wq;
            else if (target_square == h1)
                board->castle &= ~wk;
            else if (target_square == a8)
                board->castle &= ~bq;
            else if (target_square == h8)
                board->castle &= ~bk;

            for (int i = !board->side * 6; i < 12; i++)
            {
                if (get_bit(board->bitboards[i], target_square))
                {
                    unset_bit(board->bitboards[i], target_square);
                    unset_bit(board->occupancies[!board->side], target_square);
                    break;
                }
            }
        }
    }
    // Place the piece and handle promotion
    if (promoted)
        set_bit(board->bitboards[promoted], target_square);
    else
        set_bit(board->bitboards[piece], target_square);

    // Update occupancies
    unset_bit(board->occupancies[board->side], source_square);
    set_bit(board->occupancies[board->side], target_square);

    // Set enpassant on double pawn push
    board->enpassant = no_square;
    if (double_push)
    {
        if (board->side == white)
            board->enpassant = target_square + 8;
        else
            board->enpassant = target_square - 8;
    }

    // Handle Castling
    if (castling)
    {
        switch (target_square)
        {
            case g1: // white king
                unset_bit(board->bitboards[R], h1);
                set_bit(board->bitboards[R], f1);
                unset_bit(board->occupancies[white], h1);
                set_bit(board->occupancies[white], f1);
                break;
            case c1: // white queen
                unset_bit(board->bitboards[R], a1);
                set_bit(board->bitboards[R], d1);
                unset_bit(board->occupancies[white], a1);
                set_bit(board->occupancies[white], d1);
                break;
            case g8: // black king
                unset_bit(board->bitboards[r], h8);
                set_bit(board->bitboards[r], f8);
                unset_bit(board->occupancies[black], h8);
                set_bit(board->occupancies[black], f8);
                break;
            case c8: // black queen
                unset_bit(board->bitboards[r], a8);
                set_bit(board->bitboards[r], d8);
                unset_bit(board->occupancies[black], a8);
                set_bit(board->occupancies[black], d8);
                break;
        }
    }

    // Update flags
    board->side = !board->side;
    if (piece == K)
        board->castle &= ~(wk | wq);
    if (piece == k)
        board->castle &= ~(bk | bq);
    if (piece == R)
    {
        if (source_square == a1)
            board->castle &= ~wq;
        else if (source_square == h1)
            board->castle &= ~wk;
    }
    else if (piece == r)
    {
        if (source_square == a8)
            board->castle &= ~bq;
        else if (source_square == h8)
            board->castle &= ~bk;
    }

    board->occupancies[both] = board->occupancies[white] | board->occupancies[black];

    // TODO filter out pseudo-legal moves better
    return !is_square_attacked(board, get_ls1b_index((board->side == white) ? board->bitboards[k] : board->bitboards[K]), board->side);
}


/*******************************
 * Perft
 *******************************/
u64 perft(board_stack_t *stack, int depth)
{
    // Executes a perft

    move_list_t move_list;
    int i;
    u64 nodes = 0;

    if (depth == 0)
        return 1ULL;

    generate_moves(stack_current(stack), &move_list);

    for (i = 0; i < move_list.count; i++) {
        stack_push(stack);
        if (make_move(stack_current(stack), move_list.moves[i]))
        {
            nodes += perft(stack, depth - 1);
        }
        stack_pop(stack);
    }
    return nodes;
}

u64 divide(board_stack_t *stack, int depth)
{
    // Perft with per move count

    move_list_t move_list;
    int i, prev, p;
    u64 nodes = 0;

    generate_moves(stack_current(stack), &move_list);

    if (depth == 0)
        return 1ULL;

    for (i = 0; i < move_list.count; i++) {
        stack_push(stack);
        if (make_move(stack_current(stack), move_list.moves[i]))
        {
            prev = nodes;
            nodes += perft(stack, depth - 1);
            p = get_move_promoted(move_list.moves[i]);
            printf("%s%s", square_to_coordinates[get_move_source(move_list.moves[i])],
                           square_to_coordinates[get_move_target(move_list.moves[i])]);
            if (p)
            {
                printf("%c", tolower(ascii_pieces[p]));
            }
            printf(": %lld\n", nodes - prev);
        }
        stack_pop(stack);
    }
    return nodes;
}

u64 timed_perft(board_stack_t *stack, int depth)
{
    u64 nodes;
    double secs;

    printf("Running perft depth %d...\n", depth);
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    nodes = perft(stack, depth);
    gettimeofday(&stop, NULL);

    secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    printf("Searched %llu nodes in %fs (%.1fnps)\n", nodes, secs, (double) nodes / secs);

    return nodes;
}

u64 timed_divide(board_stack_t *stack, int depth)
{
    u64 nodes;
    double secs;

    printf("Running perft depth %d...\n", depth);
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    nodes = divide(stack, depth);
    gettimeofday(&stop, NULL);

    secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    printf("Searched %llu nodes in %fs (%.1fnps)\n", nodes, secs, (double) nodes / secs);

    return nodes;
}


/*******************************
 * Main Function
 *******************************/
int main()
{
    init_tables();

    start_uci();

    return 0;
}
