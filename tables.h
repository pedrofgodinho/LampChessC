#ifndef TABLES_H
#define TABLES_H

#include "utils.h"

/*******************************
 * Table Getter Macros
 *******************************/
#define get_bishop_attacks(square, occupancy) (bishop_attacks[square][((((occupancy) & bishop_masks[square]) * bishop_magic_numbers[square]) >> (64 - bishop_relevant_occupancy[square]))])
#define get_rook_attacks(square, occupancy) (rook_attacks[square][((((occupancy) & rook_masks[square]) * rook_magic_numbers[square]) >> (64 - rook_relevant_occupancy[square]))])
#define get_queen_attacks(square, occupancy) (get_rook_attacks(square, occupancy) | get_bishop_attacks(square, occupancy))
#define get_knight_attacks(square) (knight_attacks[square])
#define get_king_attacks(square) (king_attacks[square])
#define get_pawn_attacks(side, square) (pawn_attacks[side][square])

/*******************************
 * Precalculated Tables
 *******************************/
extern u64 pawn_attacks[2][64];
extern u64 knight_attacks[64];
extern u64 king_attacks[64];
extern u64 bishop_masks[64];
extern u64 rook_masks[64];
extern u64 bishop_magic_numbers[64];
extern u64 rook_magic_numbers[64];
extern const int bishop_relevant_occupancy[64];
extern const int rook_relevant_occupancy[64];
extern u64 bishop_attacks[64][512];
extern u64 rook_attacks[64][4096];

/*******************************
 * Initialization Function
 *******************************/
void init_tables();

#endif
