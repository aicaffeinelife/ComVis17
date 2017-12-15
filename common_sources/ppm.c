#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"


void ppm_read(FILE *fp, const char *fname, image_t **img){
 	fp = fopen(fname, "r");
 	int rows, cols, maxvals;
 	if (fp == NULL)
 	{
 		printf("The file is either corrupted or doesn't exist\n");
 		exit(1);

 	}

 	*img = (image_t *)malloc(sizeof(image_t));
 	fscanf(fp, "%s %d %d %d ", header, &cols, &rows, &maxvals);
 	(*img)->rows = rows;
 	(*img)->cols = cols;
 	(*img)->maxvals = maxvals;
 	(*img)->vals = (unsigned char*)calloc(rows*cols, sizeof(unsigned char));
 	fread((*img)->vals, sizeof(unsigned char), rows*cols, fp);
 	fclose(fp);

}



void ppm_write(FILE *fp, const char *fname, image_t *img){
	fp = fopen(fname, "w");
	if (fp == NULL)
 	{
 		printf("The file is either corrupted or doesn't exist\n");
 		exit(1);

 	}

 	fprintf(fp, "%s %d %d %d \n",header, img->rows, img->cols, img->maxvals);
 	fwrite(img->vals, sizeof(unsigned char), img->rows*img->cols, fp);
 	fclose(fp);

}