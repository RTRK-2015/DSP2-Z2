/*
 * processing.h
 *
 *  Created on: 15.03.2016.
 *      Author: student
 */

#ifndef PROCESSING_H_
#define PROCESSING_H_

#include "tistdtypes.h"

void downsample(Int16 *input, Int16 *output, int M, int N);
void upsample(Int16 *input, Int16 *output, int L, int N);


#endif /* PROCESSING_H_ */
