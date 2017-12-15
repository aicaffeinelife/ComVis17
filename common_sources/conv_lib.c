#include <stdio.h>
#include <stdlib.h>
#include "conv_lib.h"


void conv_naive(image_t *img, filter_t *flt, image_t **out){
	int rad = flt->radius;
	int rows, cols, maxvals;
	// *out = (image_t *)malloc(sizeof(image_t));
	rows = img->rows; 
	cols = img->cols;
	maxvals = img->maxvals;
	(*out)->rows = rows;
	(*out)->cols = cols;
	(*out)->maxvals = maxvals;
	(*out)->vals = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));

	for (int r = 3; r < rows-3; r++)
	{
		for (int c = 3; c < cols-3; c++)
		{
			int sum = 0;
			for (int dr = -rad; dr <= rad; dr++)
			{
				for (int dc = -rad; dc <= rad; dc++)
				{
					if (r+dr < 0 || r+dr > rows || c+dc < 0 || c+dc > cols)
					{
						continue;
					}

					sum += img->vals[(r+dr)*cols + (c+dc)];
				}
			}
			(*out)->vals[r*cols + c] = sum/flt->norm;
		}
	}
}


void conv_separable(image_t *img, filter_t *fx, filter_t *fy, image_t **out){

	int rad_x = fx->radius;
	int rad_y = fy->radius;

	int *buf = (int *)calloc(img->rows*img->cols, sizeof(int));
	(*out)->rows = img->rows;
	(*out)->cols = img->cols;
	(*out)->maxvals = img->maxvals;
	(*out)->vals = (unsigned char *)calloc(img->rows*img->cols, sizeof(unsigned char));
	
	for (int r = 0; r < (img->rows); r++)
	{		 
		for (int c = 3; c < (img->cols)-3; c++)
		{       int sum_x = 0;
			 	for (int dc = -3; dc <= 3; dc++)
			 	{	
			 		if (c+dc < 0 || c+dc > img->cols)
			 		{
			 			continue;
			 		}
			 		sum_x  += img->vals[r*img->cols+(c+dc)];
			 	}
			 	buf[r*img->cols + c] = sum_x;
			 } 
	}



for (int r = 3; r < (img->rows)-3; r++)
	{   
		for (int c = 0; c < (img->cols); c++)
		{
			int sum_y = 0;
			for (int dr = -3; dr <= 3; dr++)
			{
				if (r+dr < 0 || r+dr > (img->rows)-1)
				{
					continue;
				}

				sum_y += buf[(r+dr)*img->cols+c];
			}
			(*out)->vals[r*img->cols + c] = sum_y/(fx->norm * fy->norm);
	}
		}
		
		
free(buf);

}


void conv_sliding_separable(image_t *img, filter_t *fx, filter_t *fy, image_t **out){
	int rad_x = fx->radius;
	int rad_y = fy->radius;
	int sum_x, sum_y;
	int *buf = (int *)calloc(img->rows*img->cols, sizeof(int));
	(*out)->rows = img->rows;
	(*out)->cols = img->cols;
	(*out)->maxvals = img->maxvals;
	(*out)->vals = (unsigned char *)calloc(img->rows*img->cols, sizeof(unsigned char));
	int flag = 0;
	int r,c,dr,dc;
	for (r = 0; r < (img->rows) ; r++)
	{
		for ( c = 3; c < (img->cols)-3; c++)
		{ 	 
			if (flag == 0)
			{   flag = 1;
				sum_x = 0;
				for (dc = -3; dc <= 3; dc++)
				{
					sum_x += img->vals[r*img->cols + (c+dc)];
				}
				buf[r*img->cols + c] = sum_x;
			} else {
					 
					 sum_x += img->vals[r*img->cols + (c+3)];
					 sum_x -= img->vals[r*img->cols + (c-4)];
					 buf[r*img->cols + c] = sum_x;
			}
		}
		flag = 0;
	}

	flag = 0;
	for (c = 0; c < img->cols; c++)
	{
		for (r = 3; r < (img->rows)-3; r++)
		{
			if (flag == 0)
			{	flag = 1;
				sum_y = 0;
				for (dr = -3; dr <=3 ; dr++)
				{
					sum_y += buf[(r+dr)*img->cols + c];
				}
				(*out)->vals[r*img->cols + c] = sum_y/49;
			} else {
				 sum_y += buf[(r+3)*img->cols + c];	
				 sum_y -= buf[(r-4)*img->cols + c];
				 (*out)->vals[r*img->cols + c] = sum_y/49;
				 
			}
		}
		flag = 0;
	}

	free(buf);
}

