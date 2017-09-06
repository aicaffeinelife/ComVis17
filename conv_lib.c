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

	for (int r = 1; r < rows-1; r++)
	{
		for (int c = 1; c < cols-1; c++)
		{
			int sum = 0;
			for (int dr = -rad; dr <= rad; dr++)
			{
				for (int dc = -rad; dc <= rad; dc++)
				{
					if (r+dr < 0 || r+dr > rows || c+dc < 0 || c+dc > cols)
					{
						img->vals[r*cols + c] = 0;
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
	
	for (int r = 1; r < (img->rows)-1; r++)
	{		 
		for (int c = 1; c < (img->cols)-1; c++)
		{       int sum_x = 0;
			 	for (int dc = -rad_x; dc <= rad_x; dc++)
			 	{	
			 		if (c+dc < 0 || c+dc > img->cols)
			 		{
			 			img->vals[r*img->cols +c] = 0;
			 		}
			 		sum_x  += img->vals[r*img->cols+(c+dc)];
			 	}
			 	buf[r*img->cols + c] = sum_x;
			 } 
	}



for (int r = 1; r < (img->rows); r++)
	{   
		for (int c = 1; c < (img->cols); c++)
		{
			int sum_y = 0;
			for (int dr = -rad_y; dr <= rad_y; dr++)
			{
				if (r+dr < 0 || r+dr > (img->rows)-1)
				{
					buf[r*img->cols + c] = 0;
				}

				sum_y += buf[(r+dr)*img->cols+c];
			}
			(*out)->vals[r*img->cols + c] = sum_y/(fx->norm * fy->norm);
			// printf("%u\n",(*out)->vals[r*img->cols + c]);
	}
		}
		
		
free(buf);

}


void conv_sliding_separable(image_t *img, filter_t *fx, filter_t *fy, image_t **out){
	int rad_x = fx->radius;
	int rad_y = fy->radius;

	int *buf = (int *)calloc(img->rows*img->cols, sizeof(int));
	(*out)->rows = img->rows;
	(*out)->cols = img->cols;
	(*out)->maxvals = img->maxvals;
	(*out)->vals = (unsigned char *)calloc(img->rows*img->cols, sizeof(unsigned char));
	
	for (int r = 1; r < img->rows ; r++)
	{	int sum_x =  img->vals[r*img->cols + 1] + img->vals[r*img->cols + 2]+img->vals[r*img->cols + 3] ;
		buf[r*img->cols + 1] = sum_x;
		for (int c = 2; c < (img->cols); c++)
		{   
			if (c + 3 < img->cols )
			{
				sum_x += img->vals[r*img->cols + c+3];

			} 

			if (c-4 > 0)
			{
				sum_x -= img->vals[r*img->cols + (c-4)];
			}
			buf[r*img->cols + c] = sum_x;
		}
	}


	for (int c = 1; c < img->cols; c++)
		{  int sum_y = buf[img->cols + c] + buf[2*img->cols + c] + buf[3*img->cols + c];
		    (*out)->vals[img->cols + c] = sum_y/49;	
			for (int r = 2; r < img->rows ; r++ )
			{
				if (r + 3 < img->rows)
				{
				sum_y += buf[(r+3)*img->cols + c];
				}

				if (r-4 > 0 )
			{
				sum_y -= buf[(r-4)*img->cols + c];
			}
			(*out)->vals[r*img->cols + c] = sum_y/49;
		}
	}


	free(buf);
}

