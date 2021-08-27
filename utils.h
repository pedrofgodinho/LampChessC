#ifndef UTILS_H
#define UTILS_H

/*******************************
 * Bitboard Type
 *******************************/
#define u64 unsigned long long

/*******************************
 * Bit Manipulation Macros
 *******************************/
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define unset_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define count_bits(bitboard) __builtin_popcountll(bitboard)
#define get_ls1b_index(bitboard) __builtin_ffsll(bitboard) - 1
#define unset_ls1b(bitboard) ((bitboard) &= (bitboard) - 1)

/*******************************
 * QoL Enums and Mappings
 *******************************/
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, 
    no_square
};

enum { white, black, both };

enum { wk = 1, wq = 2, bk = 4, bq = 8 };

enum { P, N, B, R, Q, K, p, n, b, r, q, k };

extern const char *square_to_coordinates[];
extern const char *ascii_pieces;
extern const char *unicode_pieces[];
extern const int ascii_to_piece[]; 


/*******************************
 * Constants
 *******************************/
extern const u64 not_a_file;
extern const u64 not_h_file;
extern const u64 not_hg_file;
extern const u64 not_ab_file;
extern const u64 rank_4;
extern const u64 rank_5;


/*******************************
 * Functions
 *******************************/
void die(char* msg);

#endif
