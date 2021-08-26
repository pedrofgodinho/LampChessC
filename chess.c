#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tables.h"
#include "chess.h"


void init_board()
{
    for (int i = 0; i < 12; i++)
        bitboards[i] = 0ULL;
    
    // Pawns
    set_bit(bitboards[P], a2);
    set_bit(bitboards[P], b2);
    set_bit(bitboards[P], c2);
    set_bit(bitboards[P], d2);
    set_bit(bitboards[P], e2);
    set_bit(bitboards[P], f2);
    set_bit(bitboards[P], g2);
    set_bit(bitboards[P], h2);
    set_bit(bitboards[p], a7);
    set_bit(bitboards[p], b7);
    set_bit(bitboards[p], c7);
    set_bit(bitboards[p], d7);
    set_bit(bitboards[p], e7);
    set_bit(bitboards[p], f7);
    set_bit(bitboards[p], g7);
    set_bit(bitboards[p], h7);

    // Rooks
    set_bit(bitboards[R], a1);
    set_bit(bitboards[R], h1);
    set_bit(bitboards[r], a8);
    set_bit(bitboards[r], h8);

    // Knights
    set_bit(bitboards[N], b1);
    set_bit(bitboards[N], g1);
    set_bit(bitboards[n], b8);
    set_bit(bitboards[n], g8);

    // Bishops
    set_bit(bitboards[B], c1);
    set_bit(bitboards[B], f1);
    set_bit(bitboards[b], c8);
    set_bit(bitboards[b], f8);

    // Queens
    set_bit(bitboards[Q], d1);
    set_bit(bitboards[q], d8);

    // Kings
    set_bit(bitboards[K], e1);
    set_bit(bitboards[k], e8);

    // Occupancies
    occupancies[white] = 0ULL;
    occupancies[black] = 0ULL;
    for (int i = 0; i < 6; i++)
    {
        occupancies[white] |= bitboards[i];
        occupancies[black] |= bitboards[i + 6];
    }
    occupancies[both] = occupancies[white] | occupancies[black];

    // Flags
    side = white;
    enpassant = no_square;
    castle = wk | wq | bk | bq;
}


// Move Generation
int is_square_attacked(int square, int side)
{
    // queens
    if (get_queen_attacks(square, occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q])) return 1;
    // rooks
    if (get_rook_attacks(square, occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r])) return 1;    
    // bishops
    if (get_bishop_attacks(square, occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b])) return 1;
    // knights
    if (get_knight_attacks(square) & ((side == white) ? bitboards[N] : bitboards[n])) return 1;
    // pawns
    if ((side == white) && (get_pawn_attacks(black, square) & bitboards[P])) return 1;
    if ((side == black) && (get_pawn_attacks(white, square) & bitboards[p])) return 1;
    // kings
    if (get_king_attacks(square) & ((side == white) ? bitboards[K] : bitboards[k])) return 1;

    return 0;
}


void generate_moves()
{
    int source_square, target_square;
    U64 bitboard, attacks;

    // Side dependent moves
    if (side == white)
    {
        // Quiet Pawn Moves
        bitboard = (bitboards[P] >> 8) & ~occupancies[both];

        // Single Push
        for (attacks = bitboard; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square + 8;
            if (target_square <= h8)
            {
                printf("Push Pawn and Promote %s\n", square_to_coordinates[source_square]);
            }
            else
            {
                printf("Push Pawn %s\n", square_to_coordinates[source_square]);
            }
        }

        // Double Push
        for (attacks = (bitboard >> 8) & ~occupancies[both] & rank_4; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square + 16;
            printf("Double Push Pawn %s\n", square_to_coordinates[source_square]);
        }

        // Captures
        for (attacks = (bitboards[P] >> 7) & not_a_file & occupancies[black]; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square + 7;
            if (target_square <= h8)
            {
                printf("Capture with Pawn and Promote %s\n", square_to_coordinates[source_square]);
            }
            else
            {
                printf("Capture with Pawn %s\n", square_to_coordinates[source_square]);
            }
        }
        for (attacks = (bitboards[P] >> 9) & not_a_file & occupancies[black]; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square + 9;
            if (target_square <= h8)
            {
                printf("Capture with Pawn and Promote %s\n", square_to_coordinates[source_square]);
            }
            else
            {
                printf("Capture with Pawn %s\n", square_to_coordinates[source_square]);
            }
        }
    }
    else
    {
        // Quiet Pawn Moves
        bitboard = (bitboards[p] << 8) & ~occupancies[both];

        // Single Push
        for (attacks = bitboard; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square - 8;
            if (target_square >= a1)
            {
                printf("Push Pawn and Promote %s\n", square_to_coordinates[source_square]);
            }
            else
            {
                printf("Push Pawn %s\n", square_to_coordinates[source_square]);
            }
        }

        // Double Push
        for (attacks = (bitboard << 8) & ~occupancies[both] & rank_5; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square - 16;
            printf("Double Push Pawn %s\n", square_to_coordinates[source_square]);
        }

        // Captures
        for (attacks = (bitboards[p] << 7) & not_h_file & occupancies[white]; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square - 7;
            if (target_square >= a1)
            {
                printf("Capture with Pawn and Promote %s\n", square_to_coordinates[source_square]);
            }
            else
            {
                printf("Capture with Pawn %s\n", square_to_coordinates[source_square]);
            }
        }
        for (attacks = (bitboards[p] << 9) & not_a_file & occupancies[white]; attacks; unset_ls1b(attacks)) 
        {
            target_square = get_ls1b_index(attacks);
            source_square = target_square - 9;
            if (target_square >= a1)
            {
                printf("Capture with Pawn and Promote %s\n", square_to_coordinates[source_square]);
            }
            else
            {
                printf("Capture with Pawn %s\n", square_to_coordinates[source_square]);
            }
        }
    }
}



// IO
void parse_fen(char *fen)
{
    int i = 0;
    int fen_len = strlen(fen);

    memset(bitboards, 0ULL, sizeof(bitboards));
    memset(occupancies, 0ULL, sizeof(occupancies));
    side = white;
    enpassant = no_square;
    castle = 0;

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
                set_bit(bitboards[ascii_to_piece[c]], square);
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
        side = white;
    else
        side = black;

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
            castle |= wk;
            i++;
        }
        if (fen[i] == 'Q')
        {
            castle |= wq;
            i++;
        }
        if (fen[i] == 'k')
        {
            castle |= bk;
            i++;
        }
        if (fen[i] == 'q')
        {
            castle |= bq;
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
        enpassant = rank * 8 + file;
    }
    else
        enpassant = no_square;

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
        occupancies[white] |= bitboards[i];
        occupancies[black] |= bitboards[i + 6];
    }
    occupancies[both] = occupancies[white] | occupancies[black];
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

void print_board(int unicode)
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
                if (get_bit(bitboards[piece], square))
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
    printf("    Side to move: %s\n", side == white ? "white" : "black");
    printf("    EnPassant Square: %s\n", square_to_coordinates[enpassant]);
    printf("    Castle Rights: %c%c%c%c \n\n", 
            castle & wk ? 'K' : '-',
            castle & wq ? 'Q' : '-',
            castle & bk ? 'k' : '-',
            castle & bq ? 'q' : '-'
            );
}

// MAIN
int main()
{
    init_tables();


    //parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e2 0 1234");
    parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    //parse_fen("rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1");
    //parse_fen("r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9");
    //side = black;
    
    print_board(1);
    generate_moves();
    
    return 0;
}
