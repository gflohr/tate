/* This file is part of the chess engine tate.
 *
 * Copyright (C) 2002-2021 cantanea EOOD.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#include "state.h"

#define MATE -10000
#define INF ((-(MATE)) << 1)
#define MAXDEPTH 512

typedef struct Line {
	chi_move moves[MAXDEPTH];
	unsigned int num_moves;
} Line;

typedef struct Tree {
	chi_pos position;
	chi_move bestmove;
	int depth;
	unsigned long nodes;
	unsigned long evals;

	Line current_line;
} Tree;

#define DEBUG_SEARCH 1

#if DEBUG_SEARCH
static void
debug_start_search(Tree *tree, chi_move move)
{

}
#endif

static int
evaluate(Tree *tree)
{
	chi_pos *position = &tree->position;
	int score = 100 * chi_material(position);

	++tree->evals;
	
	return score;
}

static int
minimax(Tree *tree, int depth)
{
	chi_move moves[CHI_MAX_MOVES];
	chi_move *move_ptr;
	chi_pos *position = &tree->position;
	int bestvalue, value;
	chi_result result;

	++tree->nodes;

	if (chi_game_over(position, &result)) {
		// FIXME! Consider depth!
		if (chi_result_is_white_win(result)
		    || chi_result_is_black_win(result)) {
			return MATE;
		} else {
			return 0;
		}
	}

	if (depth == 0) {
		if (chi_on_move(position) == chi_white) {
			return evaluate(tree);
		} else {
			return -evaluate(tree);
		}
	}

	move_ptr = chi_legal_moves(position, moves);

	bestvalue = -INF;

	while (move_ptr-- >= moves) {
		chi_move move = *move_ptr;

		chi_apply_move(position, move);

		value = -minimax(tree, depth - 1);

		chi_unapply_move(position, move);

		if (value > bestvalue) {
			bestvalue = value;
			if (depth == tree->depth)
				tree->bestmove = move;
		}
	}

	return bestvalue;
}

void
think(void)
{
	chi_color_t on_move;
	Tree tree;

	if (chi_game_over(&tate.position, NULL)) return;

	memset(&tree, 0, sizeof tree);

	tree.position = tate.position;
	tree.depth = 4;

	on_move = chi_on_move(&tree.position);

	(void) minimax(&tree, tree.depth);

	tate.bestmove = tree.bestmove;
	tate.bestmove_found = 1;

	tate.pondermove_found = 0;

	fprintf(stderr, "info nodes searched: %lu\n", tree.nodes);
	fprintf(stderr, "info nodes evaluated: %lu\n", tree.evals);
}
