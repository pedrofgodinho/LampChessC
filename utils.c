#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


/*******************************
 * Constants
 *******************************/
const u64 not_a_file = 0xfefefefefefefefeULL;
const u64 not_h_file = 0x7f7f7f7f7f7f7f7fULL;
const u64 not_hg_file = 0x3f3f3f3f3f3f3f3fULL;
const u64 not_ab_file = 0xfcfcfcfcfcfcfcfcULL;
const u64 rank_2 = 0x000ff000000000000ULL;
const u64 rank_4 = 0x0000000ff00000000ULL;
const u64 rank_5 = 0x000000000ff000000ULL;
const u64 rank_7 = 0x0000000000000ff00ULL;


/*******************************
 * Mappings
 *******************************/
const char *square_to_coordinates[] =
{
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "no square"
};

const char *ascii_pieces = "PNBRQKpnbrqk";

const char *unicode_pieces[] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟", "♞", "♝", "♜", "♛", "♚"};

const int ascii_to_piece[] = {
    ['P'] = P,
    ['N'] = N,
    ['B'] = B,
    ['R'] = R,
    ['Q'] = Q,
    ['K'] = K,
    ['p'] = p,
    ['n'] = n,
    ['b'] = b,
    ['r'] = r,
    ['q'] = q,
    ['k'] = k
};


/*******************************
 * Functions
 *******************************/
void die(char* msg)
{
    printf("%s\n", msg);
    exit(1);
}
