#include <stdio.h>
#include "utils.h"
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
    
    U64 occupancy = 0ULL;
    print_bitboard(occupancy);
    set_bit(occupancy, d2);
    set_bit(occupancy, c4);
    set_bit(occupancy, d6);
    set_bit(occupancy, g4);
    print_bitboard(get_rook_attacks(d4, occupancy));


    return 0;
}
