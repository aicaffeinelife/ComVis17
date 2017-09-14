#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "ppm.h"

char header[80];

int calc_mean(image_t *img){
	int sum = 0.0;
	int mean; 
	int i,j;
	for (i = 0; i < img->rows; ++i)
	{
		for (j = 0; j < img->cols; ++j)
		{
			sum += img->vals[i*img->cols + j];
		}
	}

	mean = sum/(img->rows*img->cols);
	printf(" mean %d\n", mean);
	return mean;

}

float find_max(int *arr, int n){
	int i;
	float max = arr[0];
	for (i = 0; i < n; i++)
	{
		if (arr[i] > max)
		{
			max = arr[i];
		}
	}
	return max;
}

float find_min(int *arr, int n){
	int i;
	float min = arr[0];
	for (i = 0; i < n; i++)
	{
		if (arr[i] < min)
		{
			min = arr[i];
		}
	}
	return min;
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
	float max_lim, min_lim; 
	int sum, mean;
	image_t *img = (image_t *)malloc(sizeof(image_t));
	image_t *tplt = (image_t *)malloc(sizeof(image_t));
	FILE *fp;
	int  *out, *mean_buf;
	unsigned char *norm, *binimg;
	
	ppm_read(fp, argv[1], &img);
	read_template(fp, argv[2], &tplt);
	mean = calc_mean(tplt);
	rows = img->rows; 
	cols = img->cols;
	maxvals = img->maxvals;
	out = (int *)calloc(rows*cols, sizeof(int));
	mean_buf = (int *)calloc(tplt->rows*tplt->cols, sizeof(int));
	norm = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	binimg = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));

	for (r = 0; r < tplt->rows; r++)
	{
		for (c = 0; c < tplt->cols; c++)
		{
			mean_buf[r*tplt->cols +c] = tplt->vals[r*tplt->cols + c] - mean;
		}
	}
	
	for (r = 7; r < rows-7; r++)
	{	
		for (c = 4; c < cols-4; c++)
		{	 sum = 0;
			for (dr = -7 ; dr <= 7; dr++)
			{
				for (dc = -4; dc <= 4; dc++)
				{
					sum += (img->vals[(r+dr)*img->cols + (c+dc)])*mean_buf[(dr+7)*tplt->cols + (dc+4)];
				}
			}
			out[r*cols + c] = sum;
			// printf("%d ", out[r*cols +c ] );
			
		}
	}

	max_lim = find_max(out, rows*cols);
	min_lim = find_min(out, rows*cols);
	
	/* normalize */
	for (r = 0; r < rows; r++)
	{
		for (c = 0; c < cols; c++)
		{
			norm[r*cols + c] = (out[r*cols + c]-min_lim)*(255/(max_lim-min_lim));	
			
		}
	}
	
	
	/*create binary image */ 
	// for (r = 7; r < rows-7; r++)
	// {	
	// 	for (c = 4; c < cols-4; c++)
	// 	{
	// 		if (norm[r*cols + c] >= 100)
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