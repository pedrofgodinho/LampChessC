#ifndef TABLES_H
#define TABLES_H

#include "utils.h"

// Precalculated Tables
extern U64 pawn_attacks[2][64];
extern U64 knight_attacks[64];
extern U64 king_attacks[64];
extern U64 bishop_masks[64];
extern U64 rook_masks[64];

// Functions
void init_tables();

#endif
