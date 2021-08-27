#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "uci.h"
#include "chess.h"


/*******************************
 * Input Functions
 *******************************/
int parse_fen(board_t *board, char *fen)
{
    // This function is beyond ugly but it's not performance critical and it's safe regardless of input fen validity. 
    // Might make this function a bit nicer later.
    // Returns 1 if fen is valid, 0 otherwise. Board might be corrupt after return 0

    int i = 0;
    int fen_len = strlen(fen);

    memset(board->bitboards, 0ULL, sizeof(board->bitboards));
    memset(board->occupancies, 0ULL, sizeof(board->occupancies));
    board->side = white;
    board->enpassant = no_square;
    board->castle = 0;

    // TODO
    int clock1 = 0;
    int clock2 = 0;
    
    // Parse first part
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            if (i >= fen_len)
                return 0;
            unsigned char c = fen[i];
            if (c == 'P' || c == 'p' || c == 'N' || c == 'n' ||
                c == 'B' || c == 'b' || c == 'R' || c == 'r' ||
                c == 'Q' || c == 'q' || c == 'K' || c == 'k')
            {
                set_bit(board->bitboards[ascii_to_piece[c]], square);
                i++;
            } else if (c >= '0' && c <= '8') {
                file += c - '0' - 1;
                i++;
            } else if (c == '/') {
                if (file != 0)
                    return 0;
                i++;
                file = -1;
            } else {
                return 0;
            }
        }
    }

    // Parse second part
    if (fen[i] != ' ')
        return 0;

    i++;
    if (i + 8 > fen_len)
        return 0;

    if (fen[i] == 'w')
        board->side = white;
    else
        board->side = black;

    // parse third part
    i++;
    if (fen[i] != ' ')
        return 0;
    i++;

    if (fen[i] == '-')
    {
        i++;
    }
    else
    {
        if (fen[i] == 'K')
        {
            board->castle |= wk;
            i++;
        }
        if (fen[i] == 'Q')
        {
            board->castle |= wq;
            i++;
        }
        if (fen[i] == 'k')
        {
            board->castle |= bk;
            i++;
        }
        if (fen[i] == 'q')
        {
            board->castle |= bq;
            i++;
        }
    }

    i++;
    if (fen[i] != '-')
    {
        if (i + 6 > fen_len)
            return 0;
        int file = fen[i] - 'a';
        i++;
        int rank = 8 - (fen[i] - '0');
        board->enpassant = rank * 8 + file;
    }
    else
        board->enpassant = no_square;

    i++;
    i++;
    if (fen[i] > '9' || fen[i] < '0')
        return 0;
    while (fen[i] != ' ')
    {
        if (i + 2 >= fen_len)
            return 0;
        clock1 *= 10;
        clock1 += fen[i] - '0';
        i++;
    }

    i++;
    if (fen[i] > '9' || fen[i] < '0')
        return 0;
    while (i < fen_len && fen[i] != ' ' && fen[i] != '\n')
    {
        clock2 *= 10;
        clock2 += fen[i] - '0';
        i++;
    }

    for (int i = 0; i < 6; i++)
    {
        board->occupancies[white] |= board->bitboards[i];
        board->occupancies[black] |= board->bitboards[i + 6];
    }
    board->occupancies[both] = board->occupancies[white] | board->occupancies[black];

    return i;
}

int parse_move(board_t* board, char *move_str)
{
    // Parses a uci move like "a7a8q". Returns the move if pseufo-legal, otherwise -1
    move_list_t moves;
    int len;
    int source_square, target_square, promoted = 0;

    len = strlen(move_str);
    
    if (len < 4)
        return 0;

    generate_moves(board, &moves);

    source_square = (move_str[0] - 'a') + (8 - (move_str[1] - '0')) * 8;
    target_square = (move_str[2] - 'a') + (8 - (move_str[3] - '0')) * 8;

    if (source_square < 0 || source_square >= 64 || target_square < 0 || target_square >= 64)
        return 0;

    if (len > 4)
    {
        promoted = ascii_to_piece[(unsigned char) move_str[4]] % 6;
    }

    for (int i = 0; i < moves.count; i++)
    {
        int move = moves.moves[i];

        if (source_square == get_move_source(move) && target_square == get_move_target(move) && promoted == get_move_promoted(move) % 6)
        {
            return move;
        }
    }

    return 0;
}


/*******************************
 * Output Functions
 *******************************/
void print_move(int move)
{
    // Prints a move in UCI format
    int p = get_move_promoted(move);
    printf("%s%s", square_to_coordinates[get_move_source(move)],
                   square_to_coordinates[get_move_target(move)]);
    if (p)
    {
        printf("%c", tolower(ascii_pieces[p]));
    }
    printf("\n");
}

void identify()
{
    printf("%s %s by %s\n", NAME, VERSION, AUTHOR);
}

/*******************************
 * UCI Command Parsing
 *******************************/
void parse_position_command(char *command, board_stack_t *stack)
{
    int res, move;

    if (!*command)
    {
        printf("Invalid position command\n");
        return;
    }

    if (!strncmp(command, "fen ", 4))
    {
        command += 4;
        stack_reset(stack);
        res = parse_fen(stack_current(stack), command);
        if (!res)
            printf("Invalid fen\n");
        else
            command += res;
    } else if (!strncmp(command, "startpos", 8))
    {
        stack_reset(stack);
        parse_fen(stack_current(stack), STARTPOS);
        command += 8;
    }
    else
    {
        printf("Invalid position command\n");
        return;
    }

    if (*command && !strncmp(command, " move ", 6))
    {
        command += 6;
        while ((move = parse_move(stack_current(stack), command)))
        {
            stack_push(stack);
            make_move(stack_current(stack), move);

            for (; *command != '\0' && *command != ' '; command++);
            if (!*command)
                break;
            command++;
        }
    }
}

void parse_go_command(char *command, board_stack_t *stack)
{
    if (!*command)
    {
        printf("Invalid go command\n");
        return;
    }

    if (!strncmp(command, "perft ", 6))
    {
        command += 6;
        if (!*command)
        {
            printf("Invalid go command");
            return;
        }
        timed_perft(stack, atoi(command));
    }
    else if (!strncmp(command, "divide ", 7))
    {
        command += 7;
        if (!*command)
        {
            printf("Invalid go command");
            return;
        }
        timed_divide(stack, atoi(command));
    }

    else
    {
        printf("Invalid go command\n");
    }
}

void start_uci()
{
    char *input = malloc(sizeof(char) * MAX_INPUT);
    board_stack_t *stack = make_stack();

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    identify();

    while(fgets(input, MAX_INPUT, stdin) != NULL)
    {
        // Remove new line
        input[strlen(input) - 1] = '\0';

        if (!strncmp(input, "uci", 4))
        {
            printf("id name %s %s\nid author %s\n", NAME, VERSION, AUTHOR);
            printf("uciok\n");
        }
        else if (!strncmp(input, "isready", 8))
        {
            printf("readyok\n");
        }
        else if (!strncmp(input, "ucinewgame", 11))
        {
            destroy_stack(stack);
            stack = make_stack();
        }
        else if (!strncmp(input, "quit", 5))
        {
            break;
        }
        else if (!strncmp(input, "position ", 9))
        {
            parse_position_command(input + 9, stack);
        } 
        else if (!strncmp(input, "go ", 3))
        {
            parse_go_command(input + 3, stack);
        }
        else if (!strncmp(input, "d", 2))
        {
            print_board(stack_current(stack), 1);
        }
        else
        {
            printf("Invalid Command.\n");
        }
    }

    destroy_stack(stack);
    free(input);
}
