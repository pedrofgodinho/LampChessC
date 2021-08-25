#ifndef TABLES_H
#define TABLES_H

#include "constants.h"

// Precalculated Tables
extern U64 pawn_attacks[2][64];
extern U64 knight_attacks[64];
extern U64 king_attacks[64];
extern U64 bishop_masks[64];
extern U64 rook_masks[64];

// Functions
//U64 generate_pawn_attack(int side, int square);
//U64 generate_knight_attack(int square);
//U64 generate_king_attack(int square)
//void init_leaper_tables()
void init_tables();

#endif
