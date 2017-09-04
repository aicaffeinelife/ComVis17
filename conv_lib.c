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

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
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

	unsigned char *buf = (unsigned char *)calloc(img->rows*img->cols, sizeof(unsigned char));
	(*out)->rows = img->rows;
	(*out)->cols = img->cols;
	(*out)->maxvals = img->maxvals;
	(*out)->vals = (unsigned char *)calloc(img->rows*img->cols, sizeof(unsigned char));
	
	for (int r = 0; r < img->rows; r++)
	{   
		for (int c = 0; c < img->cols; c++)
		{
			int sum_x = 0;
			for (int dc = -rad_x; dc <= rad_x; dc++)
			{
				if (c+dc < 0 || c+dc >=img->cols)
				{
					img->vals[r*img->cols + c] = 0;
				}

				sum_x += img->vals[r*img->cols + c+dc]*fx->vals[dc+rad_x];
			}
			buf[r*img->cols + c] = sum_x/(fx->norm);
			printf("%u\n",buf[r*img->cols + c]);
		}
		
		
	}



for (int r = 0; r < img->rows; r++)
	{   
		for (int c = 0; c < img->cols; c++)
		{
			int sum_y = 0;
			for (int dr = -rad_y; dr <= rad_y; dr++)
			{
				if (r+dr < 0 || r+dr >=img->rows)
				{
					buf[r*img->cols + c] = 0;
				}

				sum_y += buf[(r+dr)*img->cols+c]*fy->vals[dr+rad_y];
			}
			(*out)->vals[r*img->cols + c] = sum_y/(fy->norm);
	}
		}
		
		
free(buf);

}


void conv_sliding_separable(image_t *img, filter_t *fx, filter_t *fy, image_t **out){
	int rad_x = fx->radius;
	int rad_y = fy->radius;

	unsigned char *buf = (unsigned char *)calloc(img->rows*img->cols, sizeof(unsigned char));
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
			 		}
			 		sum_x  += img->vals[r*img->cols+c+dc]*fx->vals[dc+rad_x];
			 	}
			 	buf[r*img->cols + c] = sum_x/fx->norm;
			 } else {
			 	buf[r*img->cols + c] = (sum_x - img->vals[r*img->cols + c-1] + img->vals[r*img->cols + c + 2*rad_x])/fx->norm;
			 }

			 printf("%u\n", buf[r*img->cols + c]);
		}
	}


	//  for (int r = 0; r < img->rows; r++)
	// {   int sum_y = 0;
	// 	for (int c = 0; c < img->cols; c++)
	// 	{
			

	// 		if (r == 0)
	// 		{
	// 			for (int dr = -rad_y; dr <= rad_y; dr++)
	// 			{
	// 				if (r+dr < 0)
	// 				{
	// 					buf[r*img->cols + c] = 0;
	// 				}

	// 				sum_y += buf[(r+dr)*img->cols+c]*fy->vals[dr+rad_y];
	// 			}
	// 			(*out)->vals[r*img->cols + c] = sum_y/fy->norm;
	// 		}
	// 		if (r+2*rad_x+1 > img->rows)
	//  			{
	//  				 img->vals[r*img->cols + c] = 0;
	 					
	//  			}	
			
	// 		(*out)->vals[(r+1)*img->cols + c] = (sum_y - buf[r*img->cols + c] + buf[(r+2*rad_y +1)*img->cols + c])/fy->norm;
	// 	}


	// }

	// free(buf);
}


// void conv_separable(image_t *img, filter_t *flt, image_t **out) {

// 	int rows = img->rows; 
// 	int cols = img->cols; 
// 	(*out)->rows = rows; 
// 	(*out)->cols = cols;
// 	(*out)->maxvals = img->maxvals;
// 	(*out)->vals = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));


// }