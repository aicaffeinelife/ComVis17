#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "conv_lib.h"
#include "ppm.h"


int main(int argc, char const *argv[])
{
	

	image_t *img;
	img = (image_t *)malloc(sizeof(image_t));
	// img->rows = 5;
	// img->cols = 5;
	// img->maxvals = 255;
	// img->vals = (unsigned char *)calloc(img->rows*img->cols, sizeof(unsigned char));
	// for (int i = 0; i < img->rows*img->cols; ++i)
	// {
	// 	img->vals[i] = i;
	// }
	FILE *fp;
	ppm_read(fp, "bridge.ppm", &img);
	

	image_t *out;
	out = (image_t *)malloc(sizeof(image_t));
	filter_t *fx, *fy;
	fx = (filter_t *)malloc(sizeof(filter_t));
	fy = (filter_t *)malloc(sizeof(filter_t));
	fx->vals = (const unsigned char[]){1,1,1,1,1,1,1};
	fx->norm = 7;
	fx->radius = 3;
	fy->vals = (const unsigned char[]){1,1,1,1,1,1,1};
	fy->norm = 7;
	fy->radius = 3;
	filter_t *flt = (filter_t *)malloc(sizeof(filter_t ));
	flt->radius = 1;
	flt->norm = 9;
	flt->vals = (const unsigned char []){1,1,1,1,1,1,1,1,1};

	// conv_naive(img, flt, &out);
	conv_separable(img, fx, fy, &out);
	// conv_sliding_separable(img, fx, fy, &out);
	// for (int i = 0; i < 5; ++i)
	// {
		
	// }
	ppm_write(fp, "test_sliding_separable.ppm", out);
	return 0;
}