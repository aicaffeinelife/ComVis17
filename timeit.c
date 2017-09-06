#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "image.h"
#include "ppm.h"
#include "conv_lib.h"


int main(int argc, char const *argv[])
{

	struct timespec tp1, tp2;
	image_t *img, *out;
	img = (image_t *)malloc(sizeof(image_t));
	out = (image_t *)malloc(sizeof(image_t));
	FILE *fp;
	double sum = 0.0; 
	double avg = 0.0;

	ppm_read(fp, "bridge.ppm", &img);

	// filter_t *flt = (filter_t *)malloc(sizeof(filter_t ));
	// flt->radius = 1;
	// flt->norm = 49;
	// flt->vals = (const unsigned char []){1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	filter_t *fx, *fy;
	fx = (filter_t *)malloc(sizeof(filter_t));
	fy = (filter_t *)malloc(sizeof(filter_t));
	fx->vals = (const unsigned char[]){1,1,1,1,1,1,1};
	fx->norm = 7;
	fx->radius = 3;
	fy->vals = (const unsigned char[]){1,1,1,1,1,1,1};
	fy->norm = 7;
	fy->radius = 3;
	for (int i = 0; i < 10; ++i)
	{	
		clock_gettime(CLOCK_REALTIME,&tp1);
		conv_separable(img, fx, fy, &out);
		clock_gettime(CLOCK_REALTIME, &tp2);
		sum += (tp2.tv_nsec-tp1.tv_nsec);
	}
	avg = sum/10;
	printf("Time taken on avg for %s convolution: %ld\n",argv[1],(long int)avg);
	return 0;
}