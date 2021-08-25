#include <stdio.h>
#include <string.h>
#include "tables.h"
#include "chess.h"

// IO
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
    printf("Bitboard: 0x%llx\n\n", bitboard);
}

void print_board(int unicode)
{
    for (int rank = 0; rank < 8; rank++)
    {
        printf(" %c  ", 'a' + rank);
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
    printf("\n    1 2 3 4 5 6 7 8\n\n");
}

// MAIN
int main()
{
    init_tables();
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

    print_board(0);
    
    return 0;
}
