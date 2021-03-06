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
#include <string.h>

#include <libchi.h>

#include "xalloc.h"
#include "lisco.h"

#define MIN_EV_SIZE (sizeof (EV_Entry) * 100000)

typedef struct ev_entry {
	bitv64 signature;
	short int score;
} EV_Entry;

EV_Entry* ev = NULL;

static unsigned long int ev_size = 0;
static unsigned long int half_ev_size = 0;

void
init_ev_hash(size_t memuse)
{
    if (memuse < MIN_EV_SIZE)
	memuse = MIN_EV_SIZE;

    half_ev_size = chi_closest_prime ((memuse / sizeof *ev) / 2);

    ev_size = half_ev_size << 1;
    ev = xrealloc (ev, ev_size * sizeof *ev);
    fprintf (stdout, 
	     "\
Allocated %lu bytes (%lu entries) for evaluation cache.\n",
	     ev_size * sizeof *ev, ev_size);

    clear_ev_hash();
}

void
clear_ev_hash(void)
{
	memset (ev, 0, ev_size * sizeof *ev);
}

int
probe_ev (chi_pos *pos, bitv64 signature, int *score)
{
    size_t offset;
    EV_Entry* hit;
    int wtm = chi_on_move (pos) == chi_white;

    if (wtm)
	offset = signature % ((bitv64) half_ev_size);
    else
	offset = half_ev_size + signature % ((bitv64) half_ev_size);

    hit = ev + offset;

    if (hit->signature == signature) {
	*score = hit->score;
	return 1;
    }

    return 0;
}

void
store_ev_entry (chi_pos *pos, bitv64 signature, int score)
{
    size_t offset;
    EV_Entry* hit;
    int wtm = chi_on_move (pos) == chi_white;

    if (wtm)
	offset = signature % ((bitv64) half_ev_size);
    else
	offset = half_ev_size + signature % ((bitv64) half_ev_size);

    hit = ev + offset;

    hit->signature = signature;
    hit->score = score;
}
