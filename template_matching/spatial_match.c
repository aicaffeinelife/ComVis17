#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "ppm.h"

char header[80];

float calc_mean(image_t *img){
	float sum = 0.0;
	float mean; 
	int i,j;
	for (i = 0; i < img->rows; ++i)
	{
		for (j = 0; j < img->cols; ++j)
		{
			sum += img->vals[i*img->cols + j];
		}
	}

	mean = sum/(img->rows*img->cols);
	printf("%f\n", mean);
	return mean;

}




void read_template(FILE *fp, const char *fname, image_t **img){

	int rows, cols, maxvals;
	fp = fopen(fname, "r");
	if (fp == NULL)
	{
		printf("The template file doesn't exsist.\n");
		exit(1);
	}
	
	fscanf(fp, "%s\n %d %d\n %d\n", header, &cols, &rows, &maxvals);
	(*img)->rows = rows;
	(*img)->cols = cols;
	(*img)->maxvals = maxvals;
	(*img)->vals = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	fread((*img)->vals, sizeof(unsigned char), rows*cols, fp);
	fclose(fp);
}



void write_msf_image(FILE *fp, const char *fname, unsigned char *out, int rows, int cols, int maxvals){
	
	fp = fopen(fname, "w");
	fprintf(fp, "%s %d %d %d\n", header, cols, rows, maxvals);
	fwrite(out, sizeof(unsigned char), rows*cols, fp);
	fclose(fp);

}


int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		printf("Usage sp_match img_name template \n");
		exit(1);
	}
	int i;
	int r,c, dr,dc;
	int rows, cols, maxvals, nval;
	float sum, mean;
	image_t *img = (image_t *)malloc(sizeof(image_t));
	image_t *tplt = (image_t *)malloc(sizeof(image_t));
	FILE *fp;
	float  *out;
	unsigned char *norm, *binimg;

	
	ppm_read(fp, argv[1], &img);
	read_template(fp, argv[2], &tplt);
	mean = calc_mean(tplt);
	rows = img->rows; 
	cols = img->cols;
	maxvals = img->maxvals;
	out = (float *)calloc(rows*cols, sizeof(float));
	norm = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	binimg = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));

	
	for (r = 7; r < rows-7; r++)
	{	
		for (c = 4; c < cols-4; c++)
		{	 sum = 0;
			for (dr = -7 ; dr <= 7; dr++)
			{
				for (dc = -4; dc <= 4; dc++)
				{
					sum += img->vals[(r+dr)*img->cols + (c+dc)]*(tplt->vals[(dr+7)*tplt->cols+(dc+4)]-mean);
				}
			}
			out[r*cols + c] = sum;
			
		}
	}

	
	
	// /* normalize */
	for (r = 7; r < rows-7; r++)
	{	
		for (c = 4; c < cols-4; c++)
		{	
			nval = (out[r*cols + c]/65536)*255;
			norm[r*cols + c] = (unsigned char)(nval);
		}
	}
	// printf("%u\n", norm[8*cols + 500]);

	/*create binary image */ 
	// for (r = 7; r < rows-7; r++)
	// {	
	// 	for (c = 4; c < cols-4; c++)
	// 	{
	// 		if (norm[r*cols + c] >= 10)
	// 		{
	// 			binimg[r*cols + c] = 255;
	// 		} else{
	// 			binimg[r*cols + c] = 0;
	// 		}
	// 	}
	// }

	write_msf_image(fp, "norm_image.ppm", norm, rows, cols, maxvals);
	// ppm_write(fp, "mean_centered.ppm", img);
	return 0;
}