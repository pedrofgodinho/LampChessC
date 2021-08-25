#include "tables.h"

U64 pawn_attacks[2][64];
U64 knight_attacks[64];
U64 king_attacks[64];

U64 bishop_masks[64];
U64 rook_masks[64];

// Precalculate Tables
U64 generate_pawn_attack(int side, int square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 1ULL << square;

    if(side == white)
    {
        if (bitboard & not_h_file) attacks |= bitboard >> 7;
        if (bitboard & not_a_file) attacks |= bitboard >> 9;
    }
    else
    {
        if (bitboard & not_a_file) attacks |= bitboard << 7;
        if (bitboard & not_h_file) attacks |= bitboard << 9;
    }
    return attacks;
}

U64 generate_knight_attack(int square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 1ULL << square;

    // up right
    if (bitboard & not_h_file) attacks |= bitboard >> 15;
    if (bitboard & not_hg_file) attacks |= bitboard >> 6;
    // up left
    if (bitboard & not_a_file) attacks |= bitboard >> 17;
    if (bitboard & not_ab_file) attacks |= bitboard >> 10;
    // down left
    if (bitboard & not_a_file) attacks |= bitboard << 15;
    if (bitboard & not_ab_file) attacks |= bitboard << 6;
    // down right
    if (bitboard & not_h_file) attacks |= bitboard << 17;
    if (bitboard & not_hg_file) attacks |= bitboard << 10;
    return attacks;
}

U64 generate_king_attack(int square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 1ULL << square;

    if (bitboard & not_h_file) attacks |= bitboard << 9;
    if (bitboard & not_h_file) attacks |= bitboard << 1;
    if (bitboard & not_h_file) attacks |= bitboard >> 7;
    if (bitboard & not_a_file) attacks |= bitboard >> 9;
    if (bitboard & not_a_file) attacks |= bitboard >> 1;
    if (bitboard & not_a_file) attacks |= bitboard << 7;
    attacks |= bitboard >> 8;
    attacks |= bitboard << 8;

    return attacks;
}

U64 generate_bishop_mask(int square)
{
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r < 7 && f < 7; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r > 0 && f < 7; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r < 7 && f > 0; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r > 0 && f > 0; r--, f--) attacks |= (1ULL << (r * 8 + f));

    return attacks;
}

U64 generate_rook_mask(int square)
{
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r < 7; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r > 0; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f < 7; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f > 0; f--) attacks |= (1ULL << (tr * 8 + f));

    return attacks;
}

U64 generate_bishop_attack(int square, U64 occupancy)
{
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) 
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy)
            break;
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) 
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy)
            break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) 
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy)
            break;
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) 
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy)
            break;
    }

    return attacks;
}

U64 generate_rook_attack(int square, U64 occupancy)
{
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r <= 7; r++) 
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & occupancy)
            break;
    }
    for (r = tr - 1; r >= 0; r--) 
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & occupancy)
            break;
    }
    for (f = tf + 1; f <= 7; f++) 
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & occupancy)
            break;
    }
    for (f = tf - 1; f >= 0; f--) 
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & occupancy)
            break;
    }

    return attacks;
}


void init_leaper_tables()
{
    for (int square = 0; square < 64; square++)
    {
        // Pawns
        pawn_attacks[white][square] = generate_pawn_attack(white, square);
        pawn_attacks[black][square] = generate_pawn_attack(black, square);

        // Knights
        knight_attacks[square] = generate_knight_attack(square);

        // Kings
        king_attacks[square] = generate_king_attack(square);
    }
}

void init_slider_tables()
{
    for (int square = 0; square < 64; square++)
    {
        bishop_masks[square] = generate_bishop_mask(square);
        rook_masks[square] = generate_rook_mask(square);
    }
}


void init_tables()
{
    init_leaper_tables();
    init_slider_tables();
}


