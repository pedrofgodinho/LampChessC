#ifndef AI_H
#define AI_H

#include "chess.h"
#include "utils.h"

#define INF 999999
#define MATE 500000

long long int searched_nodes;

int evaluate(board_t *board);
void init_ai();
int alpha_beta(board_stack_t *stack, int alpha, int beta, int depth, int *line);

#endif
