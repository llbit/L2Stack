/*
    Copyright 2009 Jesper Öqvist (jesper@llbit.se)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
	license.txt for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "../tetlib/tetlib.h"
#include "../tetlib/tetai.h"

#include <stdio.h>
#include <time.h>

#include "rand.h"

#define GA_NUM_ITERATIONS 3
#define GA_NUM_NEXT 10
#define GA_NUM_PREVIEW 0
#define GA_POPULATION_SIZE 80
#define GA_NUM_PARAMETERS 10
#define GA_TARGET 200
#define GA_GARBAGE 16
#define GA_GARBAGE_RATIO 5
#define GA_MUTATION_CHANCE 0.014f
#define GA_PARAMETER_MAGNITUDE 2.f
#define GA_FMT "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n"

float **ga_LoadParams(const char *fn)
{
	float	**params = malloc(sizeof(float*)*GA_POPULATION_SIZE);
	FILE	*file;
	int	i;

	file = fopen(fn, "r");

	if (file == NULL) {
		printf("Could not open file for reading: %s\n", fn);
		return NULL;
	}
	
	for (i = 0; i < GA_POPULATION_SIZE; ++i) {
		params[i] = malloc(sizeof(float)*GA_NUM_PARAMETERS);
		fscanf(file, GA_FMT,
			&params[i][0],
			&params[i][1],
			&params[i][2],
			&params[i][3],
			&params[i][4],
			&params[i][5],
			&params[i][6],
			&params[i][7],
			&params[i][8],
			&params[i][9]);
	}

	fclose(file);
	return params;
}

int ga_WriteParams(const char *fn, float **params)
{
	FILE	*file;
	int	i;

	file = fopen(fn, "w");

	if (file == NULL) {
		printf("Could not open file for writing: %s\n", fn);
		return 1;
	}
	
	for (i = 0; i < GA_POPULATION_SIZE; ++i) {
		fprintf(file, GA_FMT,
			params[i][0],
			params[i][1],
			params[i][2],
			params[i][3],
			params[i][4],
			params[i][5],
			params[i][6],
			params[i][7],
			params[i][8],
			params[i][9]);
	}

	fclose(file);
	return 0;
}

void ga_FillNext(tetromino_t *next)
{
	int	i;
	for (i = 0; i < GA_NUM_NEXT; ++i) {
		next[i] = randt();
	}
}

tetromino_t ga_GetNext(tetromino_t *next)
{
	int		i;
	tetromino_t	n;
	
	n = next[0];
	for (i = 0; i < GA_NUM_NEXT-1; ++i) {
		next[i] = next[i+1];
	}
	next[GA_NUM_NEXT-1] = randt();
	return n;
}

int main(int argc, char **argv)
{
	well_t		*well;
	rotsys_t	*rotsys;
	tetromino_t	*next;
	tetromino_t	tet;
	int		i, j, k;
	float		**params;
	float		**new_params;
	aiparams_t	cpar;
	aiplacement_t	*p, *placed;
	float		*scores;
	int		lines, blocks;
	float		max;
	float		total, buff;
	int		cleared;
	float		roll;
	int		generation = 0;
	int		survived;
	int		garbagecount;
	FILE		*resultf;

	squickrand((unsigned int)time(NULL));

	params = ga_LoadParams("params.txt");
	if (params == NULL) {
		printf("Failed to load initial AI parameters. Defaulting to random parameters.\n");
		params = malloc(sizeof(float*)*GA_POPULATION_SIZE);
		for (i = 0; i < GA_POPULATION_SIZE; ++i) {
			params[i] = malloc(sizeof(float)*GA_NUM_PARAMETERS);
			for (j = 0; j < GA_NUM_PARAMETERS; ++j) {
				params[i][j] = randz() * GA_PARAMETER_MAGNITUDE;
			}
		}
	}

	resultf = fopen("evolved.txt", "a");
	if (resultf == NULL) {
		printf("Could not open file for writing: evolved.txt\nExiting.\n");
		return 1;
	}

	printf("Starting Tetris simulation with\n");
	printf("\t%d individuals per generation\n", GA_POPULATION_SIZE);
	printf("\t%d games per individual\n", GA_NUM_ITERATIONS);
	printf("\t%d lines per game\n", GA_TARGET);
	printf("\t%d lines of garbage\n", GA_GARBAGE);
	printf("\t%d preview piece(s)\n", GA_NUM_PREVIEW);

	well = tet_NewWell();
	rotsys = tet_RotARS();
	next = malloc(sizeof(tetromino_t)*GA_NUM_NEXT);
	ga_FillNext(next);
	scores = malloc(sizeof(float)*GA_POPULATION_SIZE);

	while (1)
	{
		for (i = 0; i < GA_POPULATION_SIZE; ++i) {
			scores[i] = 0;
			survived = 0;
			for (j = 0; j < GA_NUM_ITERATIONS; ++j) {
				lines = 0;
				blocks = 0;
				garbagecount = 0;
				tet_Clear(well);
				initrandt();
				for (k = 0; k < GA_GARBAGE; ++k) {
					tet_AddGarbageLine(well, quickrand() % TET_WELL_W);
				}
				while (blocks < GA_TARGET) {
					cpar.depth_factor = params[i][0];
					cpar.area_weight = params[i][1];
					cpar.hole_weight = params[i][2];
					cpar.depth_weight = params[i][3];
					cpar.overhang_weight = params[i][4];
					cpar.prep_weight = params[i][5];
					cpar.single_weight = params[i][6];
					cpar.double_weight = params[i][7];
					cpar.triple_weight = params[i][8];
					cpar.tetris_weight = params[i][9];

					tet = ga_GetNext(next);
					p = tet_NewAIPlacement(tet, well, rotsys, next, GA_NUM_PREVIEW);
					if (!tet_TestPlacement(well, p->p)) break;
					placed = tet_Place(&cpar, p);
					tet_MergePlacement(well, placed->p);
					cleared = tet_ClearLines(well);
					tet_FreeAIPlacement(p);
					tet_FreeAIPlacement(placed);

					lines += cleared;
					blocks += 1;

					while (garbagecount < lines / GA_GARBAGE_RATIO) {
						tet_AddGarbageLine(well, quickrand() % TET_WELL_W);
						garbagecount += 1;
					}
				}
				
				scores[i] += (float)lines;
				if (blocks >= GA_TARGET) {
					survived += 1;
				}
			}
			printf("[%03d-%02d(%02d)]: %.0f\n", generation, i, survived, scores[i]);
			fprintf(resultf, "[%03d-%02d(%02d)]: %.1f => ", generation, i, survived, scores[i]);
			fprintf(resultf, GA_FMT,
				params[i][0],
				params[i][1],
				params[i][2],
				params[i][3],
				params[i][4],
				params[i][5],
				params[i][6],
				params[i][7],
				params[i][8],
				params[i][9]);
		}

		// Normalize
		total = 0.f;
		max = 0.f;
		for (i = 0; i < GA_POPULATION_SIZE; ++i) {
			total += scores[i];
			if (scores[i] > max) {
				max = scores[i];
			}
		}
		printf("average score was %.2f\n", total/GA_POPULATION_SIZE);
		printf("maximum score was %.2f\n", max);
		fprintf(resultf, "average score was %.2f\n", total/GA_POPULATION_SIZE);
		fprintf(resultf, "maximum score was %.2f\n", max);
		
		// Select new
		new_params = malloc(sizeof(float*)*GA_POPULATION_SIZE);
		for (i = 0; i < GA_POPULATION_SIZE; ++i) {
			int	mom, dad;

			// Select mom
			buff = 0.f;
			roll = randf()*total;
			for (j = 0; j < GA_POPULATION_SIZE; ++j) {
				buff += scores[j];
				if (roll <= buff) {
					mom = j;
					break;
				}
			}

			// Select dad
			do {
				if (total == 0.f) {
					dad = quickrand() % GA_POPULATION_SIZE;
					continue;
				}
				buff = 0.f;
				roll = randf()*total;
				for (j = 0; j < GA_POPULATION_SIZE; ++j) {
					buff += scores[j];
					if (roll <= buff) {
						dad = j;
						break;
					}
				}
			} while (dad == mom);

			// Select new parameters
			new_params[i] = malloc(sizeof(float)*GA_NUM_PARAMETERS);
			for (j = 0; j < GA_NUM_PARAMETERS; ++j) {
				// Select parent for gene
				if (randf() <= 0.5f) {
					new_params[i][j] = params[mom][j];
				} else {
					new_params[i][j] = params[dad][j];
				}

				// 4% chance of mutation
				if (randf() <= GA_MUTATION_CHANCE) {
					new_params[i][j] = new_params[i][j] + randz() * GA_PARAMETER_MAGNITUDE;
				}
			}
		}
		for (j = 0; j < GA_POPULATION_SIZE; ++j) {
			free(params[j]);
		}
		free(params);
		params = new_params;

		ga_WriteParams("params.txt", params);

		generation += 1;
	}

	tet_FreeRotSys(rotsys);
	tet_FreeWell(well);
	free(next);
	for (j = 0; j < GA_POPULATION_SIZE; ++j) {
		free(params[j]);
	}
	free(params);
	free(scores);
	fclose(resultf);
	return 0;
}
