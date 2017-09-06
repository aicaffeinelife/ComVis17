#ifndef CONV_LIB_H
#define CONV_LIB_H 
#include <stdio.h>
#include <stdlib.h>
#include "image.h"

// void conv_naive(image_t *img, filter_t *filt, image_t **out);
// void conv_separable(image_t *img, filter_t *fx, filter_t *fy, image_t **out);
// void conv_sliding(image_t *img, filter_t *filt, image_t **out);
void conv_sliding_separable(image_t *img, filter_t *fx, filter_t *fy, image_t **out);

#endif