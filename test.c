#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"
#include "image.h"
#include "conv_lib.h"


void test_naive(FILE *fp, image_t *img, image_t *out){
	filter_t *flt = (filter_t *)malloc(sizeof(filter_t));
	flt->vals = (const unsigned char[]){1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	flt->norm = 25;
	flt->radius = 2;
	conv_naive(img, flt, &out);
	ppm_write(fp, "smoothed_bridge_naive_5_5.ppm", out);
	// free(out->vals);
	// free(out);
}


void test_sliding(FILE *fp, image_t *img, image_t *out){
	filter_t *fx, *fy;
	fx = (filter_t *)malloc(sizeof(filter_t));
	fy = (filter_t *)malloc(sizeof(filter_t));
	fx->vals = (const unsigned char[]){1,1,1};
	fx->norm = 3;
	fx->radius = 1;
	fy->vals = (const unsigned char[]){1,1,1};
	fy->norm = 3;
	fy->radius = 1;

	conv_sliding_separable(img, fx, fy, &out);
	for (int i = 0; i < 4; ++i)
	{
		printf("%u\n",out->vals[i]);
	}
	ppm_write(fp, "smoothed_bridge2.ppm", out);
	free(fx);
	free(fy);
}



int main(int argc, char const *argv[])
{
	if (argc != 2 )
	{
		printf("Usage test imgname\n");
	}

	const char *fname = argv[1];
	FILE *fp; 
	image_t *img; 
	image_t *out = (image_t *)malloc(sizeof(image_t));
	filter_t *fx = (filter_t *)malloc(sizeof(filter_t));
	filter_t *fy = (filter_t *)malloc(sizeof(filter_t));
	fx->radius = 3;
	fx->vals = (const unsigned char []){1,1,1,1,1,1,1};
	fx->norm = 7;

	fy->radius =3;
	fy->vals = (const unsigned char []){1,1,1,1,1,1,1};
	fy->norm = 7;

	// filt->vals = (const unsigned char []){1,1,1,1,1,1,1,1,1};
	// filt->radius = 1;
	// filt->norm = 9;
	ppm_read(fp, fname, &img);
	// conv_sliding_separable(img, fx, fy, &out);
	test_sliding(fp, img, out);
	// test_naive(fp, img, out);
	// conv_separable(img, fx, fy, &out);
	// conv_naive(img, filt, &out);
	// for (int i = 0; i < 10; ++i)
	// {
	// 	printf("%u\n",out->vals[i]);
	// }
	// ppm_write(fp,"smoothed_bridge_7x7.ppm", out);
	// free(fx);
	// free(fy);
	free(out->vals);
	free(out);
	free(img->vals);
	free(img);
	

	return 0;
}