/*
    Copyright 2009 Jesper Öqvist (jesper@llbit.se)

    This file is part of L2Stack.
*/

#include "tetlib.h"

rng_t*	tet_NewRNG()
{
	rng_t*	rng;

	rng = malloc(sizeof(rng_t));
	rng->low = 0x49616E43;
	rng->high = 1;
	return rng;
}

void	tet_FreeRNG(rng_t** rng)
{
	if (*rng != NULL) {
		free(*rng);
		*rng = NULL;
	}
}

void	rng_Seed(rng_t* rng, uint32_t seed)
{
	rng->high = seed;
	rng->low = seed ^ 0x49616E42;
}

int	rng_GenInt(rng_t* rng)
{
	rng->high = (rng->high << 16) + (rng->high >> 16);
	rng->high += rng->low;
	rng->low += rng->high;
	return (int)rng->high;
}

float	rng_GenFloat(rng_t* rng)
{
	return (float)rng_GenInt(rng) / INT_MAX;
}
