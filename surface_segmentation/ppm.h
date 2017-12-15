#ifndef PPM_H_
#define PPM_H_ 
#include <stdio.h>
#include <stdlib.h>
#include "image.h"

/*
	Basic I/O for ppm images. 
 */

char header[80];

void ppm_read(FILE *fp, const char *fname, image_t **img);
void ppm_write(FILE *fp, const char *fname, image_t *img);




#endif


