#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"
#include "image.h"


int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		printf("Usage diff_test img1 img2\n");
		exit(1);
	}

	image_t *img1, *img2;
	img1 = (image_t *)malloc(sizeof(image_t));
	img2 = (image_t *)malloc(sizeof(image_t));
	FILE *fp;
	ppm_read(fp, argv[1], &img1);
	ppm_read(fp, argv[2], &img2);
	int sum = 0;
	for (int i = 0; i < img1->rows *img2->cols; ++i)
	{
		if (img1->vals[i] != img2->vals[i])
		{
			sum += (int)((img1->vals[i]-img2->vals[i])/255);
			// printf("%d\n", (int)(img1->vals[i]-img2->vals[i]));
		}

	}

	printf("%d\n", sum);
	return 0;
}