#include <stdio.h>
#include "constants.h"
#include "tables.h"

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

// MAIN
int main()
{

    init_tables();

    for (int square = 0; square < 64; square++)
    {
        print_bitboard(rook_masks[square]);
    }

    return 0;
}
