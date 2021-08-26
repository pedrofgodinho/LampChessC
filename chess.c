#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "tables.h"
#include "chess.h"


// IO
void parse_fen(board *board, char *fen)
{
    int i = 0;
    int fen_len = strlen(fen);

    memset(board->bitboards, 0ULL, sizeof(board->bitboards));
    memset(board->occupancies, 0ULL, sizeof(board->occupancies));
    board->side = white;
    board->enpassant = no_square;
    board->castle = 0;

    // TODO
    int clock1 = 0;
    int clock2 = 0;
    
    // Parse first part
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            if (i >= fen_len)
                die("Invalid FEN");
            unsigned char c = fen[i];
            if (c == 'P' || c == 'p' || c == 'N' || c == 'n' ||
                c == 'B' || c == 'b' || c == 'R' || c == 'r' ||
                c == 'Q' || c == 'q' || c == 'K' || c == 'k')
            {
                set_bit(board->bitboards[ascii_to_piece[c]], square);
                i++;
            } else if (c >= '0' && c <= '8') {
                file += c - '0' - 1;
                i++;
            } else if (c == '/') {
                if (file != 0)
                    die("Invalid FEN");
                i++;
                file = -1;
            } else {
                die("Invalid FEN");
            }
        }
    }

    // Parse second part
    if (fen[i] != ' ')
        die("Invalid FEN");

    i++;
    if (i + 8 > fen_len)
        die("Invalid FEN");

    if (fen[i] == 'w')
        board->side = white;
    else
        board->side = black;

    // parse third part
    i++;
    if (fen[i] != ' ')
        die("Invalid FEN");
    i++;

    if (fen[i] == '-')
    {
        i++;
    }
    else
    {
        if (fen[i] == 'K')
        {
            board->castle |= wk;
            i++;
        }
        if (fen[i] == 'Q')
        {
            board->castle |= wq;
            i++;
        }
        if (fen[i] == 'k')
        {
            board->castle |= bk;
            i++;
        }
        if (fen[i] == 'q')
        {
            board->castle |= bq;
            i++;
        }
    }

    i++;
    if (fen[i] != '-')
    {
        if (i + 6 > fen_len)
            die("Invalid FEN");
        int file = fen[i] - 'a';
        i++;
        int rank = 8 - (fen[i] - '0');
        board->enpassant = rank * 8 + file;
    }
    else
        board->enpassant = no_square;

    i++;
    i++;
    if (fen[i] > '9' || fen[i] < '0')
        die("Invalid FEN");
    while (fen[i] != ' ')
    {
        if (i + 2 >= fen_len)
            die("invalid fen");
        clock1 *= 10;
        clock1 += fen[i] - '0';
        i++;
    }

    i++;
    if (fen[i] > '9' || fen[i] < '0')
        die("Invalid FEN");
    while (i < fen_len)
    {
        clock2 *= 10;
        clock2 += fen[i] - '0';
        i++;
    }

    for (int i = 0; i < 6; i++)
    {
        board->occupancies[white] |= board->bitboards[i];
        board->occupancies[black] |= board->bitboards[i + 6];
    }
    board->occupancies[both] = board->occupancies[white] | board->occupancies[black];
}


void print_bitboard(U64 bitboard)
{
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

void print_board(board *board, int unicode)
{
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
    printf("    Side to move: %s\n", board->side == white ? "white" : "black");
    printf("    EnPassant Square: %s\n", square_to_coordinates[board->enpassant]);
    printf("    Castle Rights: %c%c%c%c \n\n", 
            board->castle & wk ? 'K' : '-',
            board->castle & wq ? 'Q' : '-',
            board->castle & bk ? 'k' : '-',
            board->castle & bq ? 'q' : '-'
            );
}


// Move Generation
int is_square_attacked(board *board, int square, int side)
{
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


void generate_moves(board *board, move_list *moves)
{
    int source_square, target_square;
    U64 bitboard, attacks;

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
                add_move(moves, encode_move(e8, g8, K, 0, 0, 0, 0, 1));
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
                printf("Castle Queenside\n");
                add_move(moves, encode_move(e8, c8, K, 0, 0, 0, 0, 1));
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


void print_move(int move)
{
    int p = get_move_promoted(move);
    printf("%s%s", square_to_coordinates[get_move_source(move)],
                   square_to_coordinates[get_move_target(move)]);
    if (p)
    {
        printf("%c", tolower(ascii_pieces[p]));
    }
    printf("\n");
}


void print_moves(move_list *moves)
{
    printf("\nmove\tpiece\tcapture\tdouble\tenpass\tcastling\n\n");
    for (int i = 0; i < moves->count; i++)
    {
        int move = moves->moves[i];
        int p = get_move_promoted(move);
        printf("%s%s",
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


// MAIN
int main()
{
    init_tables();

    board board;


    //parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e2 0 1234");
    parse_fen(&board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    //parse_fen(&board, "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1");
    //parse_fen("r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9");
    //parse_fen("r3k2r/pPppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    move_list move_list[1];

    print_board(&board, 1);
    generate_moves(&board, move_list);

    print_moves(move_list);
    return 0;
}
