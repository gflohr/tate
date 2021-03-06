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

#include <check.h>

#include "libchi.h"

START_TEST(test_parse_move_san_bug)
{
	chi_pos pos;
	chi_move move;
/*
     a   b   c   d   e   f   g   h
   +---+---+---+---+---+---+---+---+
 8 | r |   |   |   |   |   | k |   | En passant not possible.
   +---+---+---+---+---+---+---+---+ White king castle: no.
 7 |   | p |   |   | b |   | p | p | White queen castle: no.
   +---+---+---+---+---+---+---+---+ Black king castle: no.
 6 | p |   |   | p |   |   |   |   | Black queen castle: no.
   +---+---+---+---+---+---+---+---+ Half move clock (50 moves): 1.
 5 |   |   | p | q | p | r |   |   | Half moves: 37.
   +---+---+---+---+---+---+---+---+ Next move: black.
 4 |   |   |   |   |   |   |   |   | Material: -1.
   +---+---+---+---+---+---+---+---+ Black has castled: no.
 3 |   | P |   | P | Q | N |   |   | White has castled: no.
   +---+---+---+---+---+---+---+---+
 2 | P |   |   |   |   | P | P | P |
   +---+---+---+---+---+---+---+---+
 1 |   | R | R |   |   |   |   | K |
   +---+---+---+---+---+---+---+---+
     a   b   c   d   e   f   g   h
 */
	const char *fen = "r5k1/1p2b1pp/p2p4/2pqpr2/8/1P1PQN2/P4PPP/1RR4K b - - 1 19";
	int errnum = chi_set_position(&pos, fen);

	ck_assert_int_eq(errnum, 0);

	errnum = chi_parse_move (&pos, &move, "Ra8-f8");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(0, 7));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 7));
	/* Same move now in SAN.  */
	errnum = chi_parse_move (&pos, &move, "Raf8");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(0, 7));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 7));
}
END_TEST

START_TEST(test_parse_move_san_pawn)
{
	chi_pos pos;
	chi_move move;
	int errnum;

	chi_init_position(&pos);

	errnum = chi_parse_move (&pos, &move, "e4");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 1));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(4, 3));
}
END_TEST

START_TEST(test_parse_move_san_piece)
{
	chi_pos pos;
	chi_move move;
	int errnum;

	chi_init_position(&pos);

	errnum = chi_parse_move (&pos, &move, "Nc3");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(1, 0));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(2, 2));
}
END_TEST

START_TEST(test_parse_move_san_piece_capture)
{
	chi_pos pos;
	chi_move move;
/*
     a   b   c   d   e   f   g   h
   +---+---+---+---+---+---+---+---+
 8 | r |   | b | q | k | b | n | r | En passant not possible.
   +---+---+---+---+---+---+---+---+ White king castle: yes.
 7 | p | p | p | p |   | p | p | p | White queen castle: yes.
   +---+---+---+---+---+---+---+---+ Black king castle: yes.
 6 |   |   | n |   |   |   |   |   | Black queen castle: yes.
   +---+---+---+---+---+---+---+---+ Half move clock (50 moves): 2.
 5 |   |   |   |   | p |   |   |   | Half moves: 4.
   +---+---+---+---+---+---+---+---+ Next move: white.
 4 |   |   |   |   | P |   |   |   | Material: +0.
   +---+---+---+---+---+---+---+---+ Black has castled: no.
 3 |   |   |   |   |   | N |   |   | White has castled: no.
   +---+---+---+---+---+---+---+---+
 2 | P | P | P | P |   | P | P | P |
   +---+---+---+---+---+---+---+---+
 1 | R | N | B | Q | K | B |   | R |
   +---+---+---+---+---+---+---+---+
     a   b   c   d   e   f   g   h
 */
	const char *fen = "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3";
	int errnum = chi_set_position(&pos, fen);

	ck_assert_int_eq(errnum, 0);

	errnum = chi_parse_move (&pos, &move, "Nxe5");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(5, 2));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(4, 4));
	errnum = chi_parse_move (&pos, &move, "N:e5");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(5, 2));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(4, 4));
	errnum = chi_parse_move (&pos, &move, "Ne5:");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(5, 2));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(4, 4));
}
END_TEST


START_TEST(test_parse_move_san_ambiguous_pawn_capture)
{
	chi_pos pos;
	chi_move move;
/*
     a   b   c   d   e   f   g   h
   +---+---+---+---+---+---+---+---+
 8 | r | n | b | q | k | b | n | r | En passant possible on file d.
   +---+---+---+---+---+---+---+---+ White king castle: yes.
 7 | p | p | p |   |   | p | p | p | White queen castle: yes.
   +---+---+---+---+---+---+---+---+ Black king castle: yes.
 6 |   |   |   |   |   |   |   |   | Black queen castle: yes.
   +---+---+---+---+---+---+---+---+ Half move clock (50 moves): 0.
 5 |   |   |   | p | p |   |   |   | Half moves: 4.
   +---+---+---+---+---+---+---+---+ Next move: white.
 4 |   |   | P |   | P |   |   |   | Material: +0.
   +---+---+---+---+---+---+---+---+ Black has castled: no.
 3 |   |   |   |   |   |   |   |   | White has castled: no.
   +---+---+---+---+---+---+---+---+
 2 | P | P |   | P |   | P | P | P |
   +---+---+---+---+---+---+---+---+
 1 | R | N | B | Q | K | B | N | R |
   +---+---+---+---+---+---+---+---+
     a   b   c   d   e   f   g   h
 */
	const char *fen = "rnbqkbnr/ppp2ppp/8/3pp3/2P1P3/8/PP1P1PPP/RNBQKBNR w KQkq d6 0 3";
	int errnum = chi_set_position(&pos, fen);

	ck_assert_int_eq(errnum, 0);

	errnum = chi_parse_move (&pos, &move, "cxd");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(2, 3));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(3, 4));
	errnum = chi_parse_move (&pos, &move, "cd:");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(2, 3));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(3, 4));
	errnum = chi_parse_move (&pos, &move, "c:d");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(2, 3));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(3, 4));

/*
     a   b   c   d   e   f   g   h
   +---+---+---+---+---+---+---+---+
 8 |   |   |   |   | k |   |   |   | En passant not possible.
   +---+---+---+---+---+---+---+---+ White king castle: no.
 7 |   |   |   |   |   |   |   |   | White queen castle: no.
   +---+---+---+---+---+---+---+---+ Black king castle: no.
 6 |   |   |   |   |   | n |   |   | Black queen castle: no.
   +---+---+---+---+---+---+---+---+ Half move clock (50 moves): 0.
 5 |   |   |   |   | P | b |   |   | Half moves: 0.
   +---+---+---+---+---+---+---+---+ Next move: white.
 4 |   |   |   | K | P |   |   |   | Material: -4.
   +---+---+---+---+---+---+---+---+ Black has castled: no.
 3 |   |   |   |   |   |   |   |   | White has castled: no.
   +---+---+---+---+---+---+---+---+
 2 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 1 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
     a   b   c   d   e   f   g   h
 */
	fen = "4k3/8/5n2/4Pb2/3KP3/8/8/8 w - - 0 1";

	errnum = chi_set_position(&pos, fen);

	ck_assert_int_eq(errnum, 0);

	errnum = chi_parse_move (&pos, &move, "exf6");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 4));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 5));
	errnum = chi_parse_move (&pos, &move, "e:f6");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 4));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 5));
	errnum = chi_parse_move (&pos, &move, "ef6:");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 4));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 5));
	errnum = chi_parse_move (&pos, &move, "exf5");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 3));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 4));
	errnum = chi_parse_move (&pos, &move, "e:f5");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 3));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 4));
	errnum = chi_parse_move (&pos, &move, "ef5:");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 3));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 4));

	/* Test that move is completely specified.  */
	errnum = chi_parse_move (&pos, &move, "exf");
	ck_assert_int_eq(errnum, CHI_ERR_AMBIGUOUS_MOVE);
	errnum = chi_parse_move (&pos, &move, "e:f");
	ck_assert_int_eq(errnum, CHI_ERR_AMBIGUOUS_MOVE);
	errnum = chi_parse_move (&pos, &move, "ef:");
	ck_assert_int_eq(errnum, CHI_ERR_AMBIGUOUS_MOVE);
}
END_TEST

START_TEST(test_parse_move_san_promotion)
{
	chi_pos pos;
	chi_move move;
/*
     a   b   c   d   e   f   g   h
   +---+---+---+---+---+---+---+---+
 8 |   |   |   |   |   | b |   |   | En passant not possible.
   +---+---+---+---+---+---+---+---+ White king castle: no.
 7 |   |   |   | K | P |   | P |   | White queen castle: no.
   +---+---+---+---+---+---+---+---+ Black king castle: no.
 6 |   |   |   |   |   | k |   |   | Black queen castle: no.
   +---+---+---+---+---+---+---+---+ Half move clock (50 moves): 0.
 5 |   |   |   |   |   |   |   |   | Half moves: 0.
   +---+---+---+---+---+---+---+---+ Next move: white.
 4 |   |   |   |   |   |   |   |   | Material: -1.
   +---+---+---+---+---+---+---+---+ Black has castled: no.
 3 |   |   |   |   |   |   |   |   | White has castled: no.
   +---+---+---+---+---+---+---+---+
 2 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 1 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
     a   b   c   d   e   f   g   h
 */
	const char *fen = "5b2/3KP1P1/5k2/8/8/8/8/8 w - - 0 1";
	int errnum = chi_set_position(&pos, fen);

	ck_assert_int_eq(errnum, 0);

	errnum = chi_parse_move (&pos, &move, "e8Q");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(4, 7));
	errnum = chi_parse_move (&pos, &move, "exfN");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 7));
	errnum = chi_parse_move (&pos, &move, "e:fN");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 7));
	errnum = chi_parse_move (&pos, &move, "ef:N");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 7));
	errnum = chi_parse_move (&pos, &move, "exfQ+");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 7));
	errnum = chi_parse_move (&pos, &move, "e:fQ+");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 7));
	errnum = chi_parse_move (&pos, &move, "ef:Q+");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(4, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 7));
}
END_TEST

START_TEST(test_parse_move_san_ambiguous_file)
{
	chi_pos pos;
	chi_move move;
/*
     a   b   c   d   e   f   g   h
   +---+---+---+---+---+---+---+---+
 8 |   |   |   |   |   |   |   |   | En passant not possible.
   +---+---+---+---+---+---+---+---+ White king castle: no.
 7 |   | R |   |   |   |   | R |   | White queen castle: no.
   +---+---+---+---+---+---+---+---+ Black king castle: no.
 6 |   |   |   |   |   |   |   |   | Black queen castle: no.
   +---+---+---+---+---+---+---+---+ Half move clock (50 moves): 0.
 5 |   |   | K |   |   |   |   |   | Half moves: 0.
   +---+---+---+---+---+---+---+---+ Next move: white.
 4 |   |   |   |   | k |   |   |   | Material: +10.
   +---+---+---+---+---+---+---+---+ Black has castled: no.
 3 |   |   |   |   |   |   |   |   | White has castled: no.
   +---+---+---+---+---+---+---+---+
 2 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 1 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
     a   b   c   d   e   f   g   h
 */
	const char *fen = "8/1R4R1/8/2K5/4k3/8/8/8 w - - 0 1";
	int errnum = chi_set_position(&pos, fen);

	ck_assert_int_eq(errnum, 0);

	errnum = chi_parse_move (&pos, &move, "Rbf7");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(1, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(5, 6));
	errnum = chi_parse_move (&pos, &move, "Rge7+");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(6, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(4, 6));
}
END_TEST

START_TEST(test_parse_move_san_ambiguous_rank)
{
	chi_pos pos;
	chi_move move;
/*
     a   b   c   d   e   f   g   h
   +---+---+---+---+---+---+---+---+
 8 |   |   |   |   |   |   |   |   | En passant not possible.
   +---+---+---+---+---+---+---+---+ White king castle: no.
 7 |   |   |   |   |   |   | R |   | White queen castle: no.
   +---+---+---+---+---+---+---+---+ Black king castle: no.
 6 |   |   |   |   |   |   |   |   | Black queen castle: no.
   +---+---+---+---+---+---+---+---+ Half move clock (50 moves): 0.
 5 |   |   | K |   |   |   |   |   | Half moves: 0.
   +---+---+---+---+---+---+---+---+ Next move: white.
 4 |   |   |   |   | k |   |   |   | Material: +10.
   +---+---+---+---+---+---+---+---+ Black has castled: no.
 3 |   |   |   |   |   |   |   |   | White has castled: no.
   +---+---+---+---+---+---+---+---+
 2 |   |   |   |   |   |   | R |   |
   +---+---+---+---+---+---+---+---+
 1 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
     a   b   c   d   e   f   g   h
 */
	const char *fen = "8/6R1/8/2K5/4k3/8/6R1/8 w - - 0 1";
	int errnum = chi_set_position(&pos, fen);

	ck_assert_int_eq(errnum, 0);

	errnum = chi_parse_move (&pos, &move, "R7g3");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(6, 6));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(6, 2));
	errnum = chi_parse_move (&pos, &move, "R2g4+");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(6, 1));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(6, 3));
}
END_TEST

START_TEST(test_parse_move_san_ambiguous_file_and_rank)
{
	chi_pos pos;
	chi_move move;
/*
     a   b   c   d   e   f   g   h
   +---+---+---+---+---+---+---+---+
 8 |   |   |   |   |   |   |   |   | En passant not possible.
   +---+---+---+---+---+---+---+---+ White king castle: no.
 7 |   |   |   |   |   |   | R |   | White queen castle: no.
   +---+---+---+---+---+---+---+---+ Black king castle: no.
 6 |   |   | R |   |   |   |   |   | Black queen castle: no.
   +---+---+---+---+---+---+---+---+ Half move clock (50 moves): 0.
 5 |   |   | K |   |   |   |   |   | Half moves: 0.
   +---+---+---+---+---+---+---+---+ Next move: white.
 4 |   |   |   |   | k |   |   |   | Material: +15.
   +---+---+---+---+---+---+---+---+ Black has castled: no.
 3 |   |   |   |   |   |   |   |   | White has castled: no.
   +---+---+---+---+---+---+---+---+
 2 |   |   |   |   |   |   | R |   |
   +---+---+---+---+---+---+---+---+
 1 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
     a   b   c   d   e   f   g   h
 */
	const char *fen = "8/6R1/2R5/2K5/4k3/8/6R1/8 w - - 0 1";
	int errnum = chi_set_position(&pos, fen);

	ck_assert_int_eq(errnum, 0);

	errnum = chi_parse_move (&pos, &move, "Rg2g6");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_from(move), chi_coords2shift(6, 1));
	ck_assert_int_eq(chi_move_to(move), chi_coords2shift(6, 5));
}
END_TEST

START_TEST(test_move_material_capture)
{
	const char *fen = "8/1k1q4/4P3/1K6/8/8/8/8 w - - 0 1";
	chi_pos position;
	int errnum;
	chi_move move;

	errnum = chi_set_position(&position, fen);
	ck_assert_int_eq(errnum, 0);
	errnum = chi_parse_move(&position, &move, "exd7");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_material(move), 9);
}

START_TEST(test_move_material_ep)
{
	const char *fen = "8/1k6/8/1K6/4pP2/8/8/8 b - f3 0 1";
	chi_pos position;
	int errnum;
	chi_move move;

	errnum = chi_set_position(&position, fen);
	ck_assert_int_eq(errnum, 0);
	errnum = chi_parse_move(&position, &move, "exf");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_material(move), 1);
}

START_TEST(test_move_material_promotion)
{
	const char *fen = "qb6/knP5/pp6/8/4K3/8/8/8 w - - 0 1";
	chi_pos position;
	int errnum;
	chi_move move;

	errnum = chi_set_position(&position, fen);
	ck_assert_int_eq(errnum, 0);
	errnum = chi_parse_move(&position, &move, "c8=N#");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_material(move), 2);
}

START_TEST(test_move_material_promotion_capture)
{
	const char *fen = "1k1q4/4P3/1K6/8/8/8/8/8 w - - 0 1";
	chi_pos position;
	int errnum;
	chi_move move;

	errnum = chi_set_position(&position, fen);
	ck_assert_int_eq(errnum, 0);
	errnum = chi_parse_move(&position, &move, "exd8=Q#");
	ck_assert_int_eq(errnum, 0);
	ck_assert_int_eq(chi_move_material(move), 17);
}

Suite *
parsers_suite(void)
{
	Suite *suite;
	TCase *tc_bugs;
    TCase *tc_san;
	TCase *tc_move_material;

	suite = suite_create("Parsers");

	tc_bugs = tcase_create("Bugs");
	tcase_add_test(tc_bugs, test_parse_move_san_bug);
	suite_add_tcase(suite, tc_bugs);

	tc_move_material = tcase_create("Move Material Balance");
	tcase_add_test(tc_move_material, test_move_material_capture);
	tcase_add_test(tc_move_material, test_move_material_ep);
	tcase_add_test(tc_move_material, test_move_material_promotion);
	tcase_add_test(tc_move_material, test_move_material_promotion_capture);
	suite_add_tcase(suite, tc_move_material);

	tc_san = tcase_create("SAN");
	tcase_add_test(tc_san, test_parse_move_san_pawn);
	tcase_add_test(tc_san, test_parse_move_san_piece);
	tcase_add_test(tc_san, test_parse_move_san_piece_capture);
	tcase_add_test(tc_san, test_parse_move_san_ambiguous_pawn_capture);
	tcase_add_test(tc_san, test_parse_move_san_promotion);
	tcase_add_test(tc_san, test_parse_move_san_ambiguous_file);
	tcase_add_test(tc_san, test_parse_move_san_ambiguous_rank);
	tcase_add_test(tc_san, test_parse_move_san_ambiguous_file_and_rank);
	suite_add_tcase(suite, tc_san);

	return suite;
}
