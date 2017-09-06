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
					if (r+dr < 0 || r+dr >= rows || c+dc < 0 || c+dc >= cols)
					{
						img->vals[r*cols + c] = 0;
					}

					sum += img->vals[(r+dr)*cols + (c+dc)]*flt->vals[(dr+rad)*(2*rad+1)+(dc+rad)];
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
			 		if (c+dc < 0 || c+dc >= img->cols)
			 		{
			 			img->vals[r*img->cols +c] = 0;
			 		}
			 		sum_x  += img->vals[r*img->cols+(c+dc)];
			 	}
			 	buf[r*img->cols + c] = sum_x;
			 } 
	}



for (int r = 1; r < (img->rows)-1; r++)
	{   
		for (int c = 1; c < (img->cols)-1; c++)
		{
			int sum_y = 0;
			for (int dr = -rad_y; dr <= rad_y; dr++)
			{
				if (r+dr < 0 || r+dr >= img->rows)
				{
					buf[r*img->cols + c] = 0;
				}

				sum_y += buf[(r+dr)*img->cols+c];
			}
			(*out)->vals[r*img->cols + c] = sum_y/9;
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
	
	for (int r = 0; r < img->rows; r++)
	{
		 
		for (int c = 0; c < img->cols; c++)
		{    int sum_x = 0;
			 if (c == 0)
			 {
			 	for (int dc = -rad_x; dc <= rad_x; dc++)
			 	{	
			 		if (c+dc < 0)
			 		{
			 			 img->vals[r*img->cols +c] = 0;
			 			// continue;
			 		}
			 		sum_x  += img->vals[r*img->cols+c+dc]*fx->vals[dc+rad_x];
			 	}
			 	buf[r*img->cols + c] = sum_x ;
			 	// printf("%d %d: %u \n",r,c, buf[r*img->cols + c]);
			 } else {
			 			if (c + rad_x  >=img->cols)
			 			{
			 				img->vals[r*img->rows + c] = 0;
			 			}
			 		buf[r*img->cols + c] = (sum_x - img->vals[r*img->cols + (c-rad_x-1)] + img->vals[r*img->cols + c + rad_x]);
			 	// printf("%d %d: %u \n",r,c, buf[r*img->cols + c]);
			 	// printf("%d %d: %u \n",r,c-1, buf[r*img->cols + c-1]);
			 	// printf("%d %d: %u \n",r,c-1, img->vals[r*img->cols + c -1]);
			 	// printf("%d %d: %u \n",r,c+1, img->vals[r*img->cols + c +1]);
			 }

		}
	}


	 for (int r = 0; r < img->rows; r++)
	{  int sum_y = 0;	
		for (int c = 0; c < img->cols; c++)
		{
			 
			if (r == 0)
			{
				for (int dr = -rad_y; dr <= rad_y; dr++)
				{
					if (r+dr < 0)
					{
						buf[r*img->cols + c] = 0;
					}

					sum_y += buf[(r+dr)*img->cols+c]*fy->vals[dr+rad_y];
				}
				(*out)->vals[r*img->cols + c] = sum_y;
			} else {
						if (r+ rad_x >=img->rows)
	 					{
	 				 		buf[r*img->cols + c] = 0;
	 					
	 					}	
			
					(*out)->vals[r*img->cols + c] = (sum_y - buf[(r-rad_x-1)*img->cols + c] + buf[(r + rad_x)*img->cols + c])/9;
				// if ((*out)->vals[r*img->cols + c] > 255)
				// 	{
				// 		printf("%d %d : %u\n",r,c,(*out)->vals[r*img->cols + c]);
				// 	}
							
			}
			
		}


	}

	free(buf);
}

