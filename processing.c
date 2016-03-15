/*
 * processing.c
 *
 *  Created on: 15.03.2016.
 *      Author: student
 */

#include "processing.h"

void downsample(Int16 *input, Int16 *output, int M, int N)
{
	int i;
	for (i = 0; i < N / M; i++) {
		output[i] = input[i*M];
	}
}


void upsample(Int16 *input, Int16 *output, int L, int N)
{
	int i, j;

	for (i = 0; i < N; i++) {
		output[i * L] = input[i];

		for (j = 0; j < L - 1; j++) {
			output[i * L + j + 1] = 0;
		}
	}
}
