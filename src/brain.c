/* This file is part of the chess engine lisco.
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
#include <stdlib.h>
#include <string.h>

#include <error.h>

#include <libchi.h>

#include "brain.h"
#include "search.h"
#include "lisco.h"
#include "time-control.h"
#include "time-management.h"

bitv64 total_nodes = 0;
bitv64 total_centiseconds = 0;
bitv64 nps_peak = 0;

int next_time_control;

static void init_tree(TREE *tree, chi_epd_pos *epd);
static int _think(chi_move *mv, chi_epd_pos *epd);

/* Time control options stockfish-style.  FIXME! Should be configurable!  */
TimePoint min_thinking_time;
TimePoint move_overhead;
TimePoint slow_mover;
TimePoint nodestime;

void
evaluate_move (chi_move move)
{
	fprintf (stdout, "  todo\n");
}

int
think(chi_move *mv, chi_epd_pos *epd)
{
	int retval;

	TimeLimits limits;
	
	time_limits_init(&limits);

	/* How many moves to go?  */

	retval = _think(mv, epd);
	time_left -= (now() - limits.start_time) * 10;

	return retval;
}

static int
_think(chi_move *mv, chi_epd_pos *epd)
{
	TREE tree;
	chi_move moves[CHI_MAX_MOVES];
	chi_move *move_ptr;

	int num_moves;
	int value;

	start_time = rtime();

	move_ptr = chi_legal_moves(&current, moves);

	num_moves = move_ptr - moves;

#if 0
	current_score = chi_material (&current) * 100;
	if (chi_on_move (&current) != chi_white)
		current_score = -current_score;
#endif
	fprintf (stdout, "Current score is: %d\n", current_score);
	if (num_moves == 0) {
		if (chi_check_check (&current)) {
			if (chi_on_move (&current) == chi_white) {
				fprintf (stdout, "0-1 {Black mates}\n");
			} else {
				fprintf (stdout, "1-0 {White mates}\n");
			}
		} else {
			fprintf (stdout, "1/2-1/2 {Stalemate}\n");
		}

		return EVENT_GAME_OVER;
	}

	if (current.half_move_clock >= 100) {
		fprintf (stdout, "1/2-1/2 {Fifty-move rule}\n");
		return EVENT_GAME_OVER;
	}

	*mv = moves[0];

	/* Better than nothing ...  */
	if (num_moves == 1) {
		return EVENT_CONTINUE;
	}

#if DEBUG_BRAIN
	max_depth = DEBUG_BRAIN;
	tree.time_for_move = 999999;
#endif

	init_tree(&tree, epd);

	/* How many time will we afford ourselves? */
	if (pondering) {
		tree.time_for_move = 0x7fffffff;
	} else {
		if (fixed_time) {
			tree.time_for_move = fixed_time;
		} else {
			if (go_fast) {
				long int alloc = allocate_time ();
				if (alloc > 40)
					tree.time_for_move = 40;
				else
					tree.time_for_move = alloc;
			} else {
				tree.time_for_move = allocate_time ();
			}
		}
	}

	next_time_control = 0;

	for (int depth = 1; depth <= max_depth; ++depth) {
		tree.depth = depth;
#if DEBUG_BRAIN
		if (max_ply > 1) {
			indent_output(&tree, 1);
			fprintf(stderr, "deepening search to %d plies\n", max_ply);
		}
#endif

		tree.best_move = chi_false;
		value = -search(&tree, depth, -INF, +INF);
		if (tree.best_move) *mv = tree.best_move;

#if DEBUG_BRAIN
		fprintf(stderr, "best move at depth %d(%d): ", max_ply, max_depth);
		my_print_move(*mv);
		fprintf(stderr, " (score: %d)\n", value);
		fflush(stderr);
#endif
		// FIXME! This has to be more sophisticated for correct
		// timing.  But we need pv handling first.
		if (tree.epd) {
			tree.epd->suggestion = *mv;
		}

		if (tree.status & EVENTMASK_ENGINE_STOP)
			return EVENT_CONTINUE;
	}

	return EVENT_CONTINUE;
}

#if DEBUG_BRAIN
void
indent_output (TREE *tree, int ply)
{
	int i;

//    int ply = tree->current_depth - depth;

//    for (i = depth; i < tree->current_depth; ++i)
//        fprintf (stderr, " ");

	for (i = 0; i < ply; ++i)
		fputc (' ', stderr);

	/* Assumed to be called *after* a move has been applied.  */
	if (chi_on_move(&tree->pos) != chi_white)
		fprintf(stderr, " [%s(%d)]: ", "white", ply);
	else
		fprintf(stderr, " [%s(%d)]: ", "black", ply);
}
#endif

void
my_print_move(chi_move mv)
{
	switch (chi_move_attacker (mv)) {
	case knight:
		fputc ('N', stderr);
		break;
	case bishop:
		fputc ('B', stderr);
		break;
	case rook:
		fputc ('R', stderr);
		break;
	case queen:
		fputc ('Q', stderr);
		break;
	case king:
		fputc ('K', stderr);
		break;
	}

	fprintf(stderr, "%s%c%s",
	        chi_shift2label (chi_move_from (mv)),
	        chi_move_victim (mv) ? 'x' : '-',
	        chi_shift2label (chi_move_to (mv)));
	switch (chi_move_promote (mv)) {
	case knight:
		fprintf (stderr, "=N");
		break;
	case bishop:
		fprintf (stderr, "=B");
		break;
	case rook:
		fprintf (stderr, "=R");
		break;
	case queen:
		fprintf (stderr, "=Q");
		break;
	}
}

static void
init_tree(TREE *tree, chi_epd_pos *epd)
{
	memset(tree, 0, sizeof *tree);
	tree->pos = current;

	// FIXME! Is this correct?
	tree->w_castled = chi_w_castled(&tree->pos);
	tree->b_castled = chi_b_castled(&tree->pos);

	tree->signatures[0] = game_hist[game_hist_ply].signature;
	tree->in_check[0] = chi_check_check(&tree->pos);

	tree->epd = epd;

	// FIXME! Initialize time_for_move.
}
