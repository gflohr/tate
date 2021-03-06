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

#ifndef _LISCOPLAY_GAME_H
# define _LISCOPLAY_GAME_H        /* Allow multiple inclusion.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/time.h>

#include "libchi.h"
#include "stringbuf.h"
#include "liscoplay-engine.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Game {
	Engine *white;
	Engine *black;

	/* The current position.  */
	chi_pos pos;

	/* Moves played.  */
	chi_move *moves;
	size_t num_moves;

	/* Positions.  */
	char **fen;

	struct timeval start;

	chi_bool started;

	chi_result result;

	/* PGN properties (Seven Tag Roster STR) that can be set via
	 * command-line options.
	 */
	char *event;
	char *site;
	char *round;
	char *player_white;
	char *player_black;
} Game;

extern Game *game_new();
extern void game_destroy(Game *game);

extern void game_set_event(Game *game, const char *event);
extern void game_set_site(Game *game, const char *site);
extern void game_set_round(Game *game, const char *round);
extern void game_set_player_white(Game *game, const char *white);
extern void game_set_player_black(Game *game, const char *black);
extern void game_set_option_white(Game *game, char *optspec);
extern void game_set_option_black(Game *game, char *optspec);

extern chi_bool game_ping(Game *game);
extern chi_bool game_do_move(Game *game, const char *movestr);
extern void game_uci_time_control(Game *game, chi_stringbuf *sb);
extern void game_xboard_time_control(Game *game, chi_stringbuf *sb);

extern void game_print_pgn(const Game *game);

#ifdef __cplusplus
}
#endif

#endif
