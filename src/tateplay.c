/* This file is part of the chess engine tate.
 *
 * Copyright (C) 2002-2019 cantanea EOOD.
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

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basename.h"
#include "closeout.h"
#include "error.h"
#include "progname.h"

#include "log.h"
#include "tateplay-game.h"
#include "tateplay-loop.h"

static void usage(int status);
static void version(void);
static void set_protocol(const char *name);
static void handle_sigchld(int signo);
static void reap_children(void);

static Game *game;

static int opt_protocol_seen = 0;

static const struct option long_options[] = {
	{ "white", required_argument, NULL, 'w' },
	{ "black", required_argument, NULL, 'b' },
	{ "protocol", required_argument, NULL, 'p' },
	{ "event", required_argument, NULL, 'e' },
	{ "site", required_argument, NULL, 's' },
	{ "round", required_argument, NULL, 'r' },
	{ "player-white", required_argument, NULL, CHAR_MAX + 1 },
	{ "player-black", required_argument, NULL, CHAR_MAX + 2 },
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'V' },
	{ "verbose", no_argument, NULL, 'v' },
	NULL
};

int
main(int argc, char *argv[])
{
	int optchar;
	bool do_help = false;
	bool do_version = false;
	bool white_seen = false;
	bool black_seen = false;
	struct sigaction sa;

	game = game_new();

	set_program_name(argv[0]);

	atexit(close_stdout);
	atexit(reap_children);

	sa.sa_handler = &handle_sigchld;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, 0) == -1)
		error(EXIT_FAILURE, errno, "cannot install SIGCHLD handler");

	while ((optchar = getopt_long(argc, argv,
	                              "w:b:hVv",
								  long_options, NULL)) != EOF) {
		switch (optchar) {
			case '\0':
				/* Long option.  */
				break;

			case 'w':
				white_seen = true;
				engine_add_argv(game->white, optarg);
				break;

			case 'b':
				black_seen = true;
				engine_add_argv(game->black, optarg);
				break;

			case 'p':
				set_protocol(optarg);
				break;

			case 'e':
				game_set_event(game, optarg);
				break;

			case 's':
				game_set_site(game, optarg);
				break;

			case 'r':
				game_set_round(game, optarg);
				break;

			case CHAR_MAX + 1:
				game_set_player_white(game, optarg);
				break;

			case CHAR_MAX + 2:
				game_set_player_black(game, optarg);
				break;

			case 'h':
				do_help = true;
				break;

			case 'V':
				do_version = true;
				break;

			case 'v':
				++verbose;
				break;

			default:
				usage(EXIT_FAILURE);
				/* NOTREACHED */
		}
	}

	if (do_version) version();
	if (do_help) usage(EXIT_SUCCESS);

	if (!(white_seen && black_seen)) {
		error(EXIT_SUCCESS, 0,
		      "The options '--white' and '--black' are mandatory");
		usage(EXIT_FAILURE);
	}

	log_realm("info", "starting white engine");
	if (!engine_start(game->white))
		error(EXIT_FAILURE, errno, "error starting white engine '%s'",
		      game->white->nick);

	log_realm("info", "starting black engine");
	if (!engine_start(game->black))
		error(EXIT_FAILURE, errno, "error starting black engine '%s'",
		      game->black->nick);

	game_print_pgn(game);

	sleep(3);

	log_realm("info", "terminating engines");
	game_destroy(game);

	return 0;
}

static void
usage(int status)
{
	if (status != EXIT_SUCCESS)
		fprintf(stderr, "Try '%s --help' for more information.\n",
		        program_name);
	else {
		printf("\
Usage: %s [OPTION]...\n\
", program_name);
		printf("\n");
		printf("\
Let two chess engines play against each other.\n\
");
		printf("\n");
		printf("\
Mandatory arguments to long options are mandatory for short options too.\n\
Similarly for optional arguments.\n\
");
		printf("\n");
		printf("\
Engine selection:\n");
		printf("\
  -w, --white=CMD_OR_ARG      white engine\n");
		printf("\
  -b, --black=CMD_OR_ARG      black engine\n");
		printf("\n");
		printf("\
Engine behavior and properties (first time for both engines, second time only\n\
for black):\n\
");
		printf("\
  -p, --protocol=PROTOCOL     one of 'uci' (default), 'xboard', or 'cecp'\n\
                              (an alias for 'xboard'\n");
		printf("\n");
		printf ("\
Game information:\n");
		printf ("\
  -e, --event=EVENT           the name of the event\n\
  -s, --site=SITE             the name of the site\n\
  -r, --round=ROUND           the round of the match\n\
  -f, --fen=FEN               the starting position in Forsyth-Edwards\n\
                              Notation (FEN)\n\
");
		printf("\n");
		printf("\
Informative output:\n");
		printf ("\
  -h, --help                  display this help and exit\n");
		printf ("\
  -V, --version               output version information and exit\n");
		printf ("\n");
		printf("\
  -v, --verbose               increase verbosity level\n");
		printf ("\n");
		fputs ("Report bugs at <https://github.com/gflohr/tate/issues>.\n",
		       stdout);
	}

	exit(status);
}

static void
version(void)
{
	printf("%s (%s) %s\n", basename(program_name), PACKAGE, VERSION);
	printf("Copyright (C) %s cantanea EOOD (http://www.cantanea.com)\n\
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
",
              "2002-2019");
	printf("Written by Guido Flohr.\n");
	exit (EXIT_SUCCESS);

}

static void
handle_sigchld(int signo)
{
	int saved_errno = errno;
	pid_t pid = 1;

	while (1) {
	pid = waitpid((pid_t) -1, 0, WNOHANG);
		if (pid <= 0) break;
		if (game->white && game->white->pid == pid) {
			child_exited = 1;
			log_realm(game->white->nick, "engine exited unexpectedly.");
		} else if (game->black && game->black->pid == pid) {
			child_exited = 1;
			log_realm(game->black->nick, "engine exited unexpectedly.");
		}
	}

	errno = saved_errno;
}

static void
reap_children(void)
{
	while (waitpid((pid_t) -1, 0, WNOHANG) > 0) {}
}

static void
set_protocol(const char *proto_name)
{
	EngineProtocol protocol;

	if (0 == strcasecmp("uci", proto_name)) {
		protocol = uci;
	} else if (0 == strcasecmp("xboard", proto_name)) {
		protocol = xboard;
	} else if (0 == strcasecmp("cecp", proto_name)) {
		protocol = xboard;
	} else {
		error(EXIT_SUCCESS, 0, "unsupported protocol '%s'.", proto_name);
		usage(EXIT_FAILURE);
	}

	switch(++opt_protocol_seen) {
		case 1:
			engine_set_protocol(game->white, protocol);
			/* FALLTHRU */
		case 2:
			engine_set_protocol(game->black, protocol);
			break;
		default:
			error(EXIT_SUCCESS, 0,
			      "option --protocol can be given at most twice.");
			usage(EXIT_FAILURE);
	}
}