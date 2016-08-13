/*
    Copyright 2009 Jesper Öqvist (jesper@llbit.se)

    This file is part of L2Stack.
*/

#include "tetlib.h"

static void rbag_NewBag(rand_t* r);

rand_t*	tet_NewRandStateless()
{
	rand_t* r;

	r = malloc(sizeof(rand_t));

	r->rng = tet_NewRNG();
	r->state = NULL;
	r->GetNext = &rsl_GetNext;

	return r;
}

rand_t*	tet_NewRandBag()
{
	rand_t* r;

	r = malloc(sizeof(rand_t));

	r->rng = tet_NewRNG();
	r->state = malloc(sizeof(tet_t)*7);
	rbag_NewBag(r);
	r->GetNext = &rbag_GetNext;

	return r;
}

/**
 * The 4 reroll randomizer used in TGM.
 */
rand_t*	tet_NewRand4Retry()
{
	rand_t* r;

	r = malloc(sizeof(rand_t));

	r->rng = tet_NewRNG();
	r->state = malloc(sizeof(tet_t)*4);
	r->state[0] = TET_Z;
	r->state[1] = TET_Z;
	r->state[2] = TET_Z;
	r->state[3] = TET_Z;
	r->auxint = 1;// first roll
	r->GetNext = &r4retry_GetNext;

	return r;
}

/**
 * The 6 reroll randomizer used in TAP.
 * Both 4 and 6 rerollers use 4 history,
 * but the history is initialized differently.
 */
rand_t*	tet_NewRand6Retry()
{
	rand_t* r;

	r = malloc(sizeof(rand_t));

	r->rng = tet_NewRNG();
	r->state = malloc(sizeof(tet_t)*4);
	r->state[0] = TET_Z;
	r->state[1] = TET_S;
	r->state[2] = TET_Z;
	r->state[3] = TET_S;
	r->auxint = 1;// first roll
	r->GetNext = &r6retry_GetNext;

	return r;
}

void tet_FreeRand(rand_t** r)
{
	if (*r == NULL)
		return;

	tet_FreeRNG(&((*r)->rng));
	if ((*r)->state != NULL) {
		free((*r)->state);
		(*r)->state = NULL;
	}
	free(*r);
	*r = NULL;
}

tet_t	rsl_GetNext(rand_t* r)
{
	return rng_GenInt(r->rng) % TET_COUNT;
}

static void rbag_NewBag(rand_t* r)
{
	r->state[0] = TET_I;
	r->state[1] = TET_J;
	r->state[2] = TET_L;
	r->state[3] = TET_T;
	r->state[4] = TET_S;
	r->state[5] = TET_Z;
	r->state[6] = TET_O;
	r->auxint = TET_COUNT;
}

tet_t	rbag_GetNext(rand_t* r)
{
	int	i;
	tet_t	tet;
	tet_t*	bag;
	int	roll;

	bag = r->state;
	roll = r->auxint;

	i = rng_GenInt(r->rng) % (roll);
	tet = bag[i];
	--roll;

	if (roll == 0) {
		rbag_NewBag(r);
	} else {
		bag[i] = bag[roll];
		r->auxint = roll;
	}

	return tet;
}

tet_t	r4retry_GetNext(rand_t* r)
{
	int	i;
	int	j;
	tet_t	tet;
	tet_t*	history;
	bool	first;

	history = r->state;
	first = r->auxint == 1;

	do {
		// reroll 4 times
		for (i = 0; i < 4; ++i) {
			tet = rng_GenInt(r->rng);
			tet = tet % TET_COUNT;
			for (j = 0; j < 4; ++j) {
				if (tet == history[j]) {
					break;
				}
			}
			if (j == 4) {
				break;
			}
		}
	} while (first && (tet == TET_S || tet == TET_Z || tet == TET_O));

	r->auxint = 0;

	// update history
	for (i = 0; i < 3; ++i) {
		history[i] = history[i + 1];
	}
	history[3] = tet;
	return tet;
}

tet_t	r6retry_GetNext(rand_t* r)
{
	int	i;
	int	j;
	tet_t	tet;
	tet_t*	history;
	bool	first;

	history = r->state;
	first = r->auxint == 1;

	do {
		// reroll 6 times
		for (i = 0; i < 6; ++i) {
			tet = rng_GenInt(r->rng);
			tet = tet % TET_COUNT;
			for (j = 0; j < 4; ++j) {
				if (tet == history[j]) {
					break;
				}
			}
			if (j == 4) {
				break;
			}
		}
	} while (first && (tet == TET_S || tet == TET_Z || tet == TET_O));

	r->auxint = 0;

	// update history
	for (i = 0; i < 3; ++i) {
		history[i] = history[i + 1];
	}
	history[3] = tet;
	return tet;
}

