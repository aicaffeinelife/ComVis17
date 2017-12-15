#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "ppm.h"

#define MAX_ROWS 1262

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


// unsigned char * create_thresholded_img(unsigned char *arr, int rows, int cols, int thresh){

// 	unsigned char *binary = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
// 	int r,c; 
// 	for (r = 0; r < rows; r++)
// 	{	
// 		for (c = 0; c < cols; c++)
// 		{
// 			if (arr[r*cols + c] >= thresh)
// 			{
// 				binary[r*cols + c] = 255;
// 			} else{
// 				binary[r*cols + c] = 0;
// 			}
// 		}
// 	}
// 	return binary;
// }



int main(int argc, char const *argv[])
{
	if (argc != 4)
	{
		printf("Usage sp_match img_name template  ground truth \n");
		exit(1);
	}
	int i,j;
	int cr,cc;
	int r,c, dr,dc,t;
	int rows, cols, maxvals, nval;
	float max_lim, min_lim;
	float TP, FP, TN, FN; // params for roc analysis. 
	float tpr, fpr;
	int sum, mean;
	image_t *img = (image_t *)malloc(sizeof(image_t));
	image_t *tplt = (image_t *)malloc(sizeof(image_t));
	FILE *fp, *fpgt, *fpeval;
	int  *out, *mean_buf;
	unsigned char *norm, *binimg;

	char letter[MAX_ROWS];
	int x_pos[MAX_ROWS], y_pos[MAX_ROWS];
	int detected; // False since nothing is detected. 
	
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
	


	/* Read and parse ground truth */
	fpgt = fopen(argv[3], "r");
	if (fpgt == NULL)
	{
		printf("The file couldn't be found \n");
		exit(1);
	}

	for (i = 0; i < MAX_ROWS; i++)
	{
		fscanf(fpgt, "%c %d %d\n", &letter[i], &y_pos[i], &x_pos[i]);
	}
	fclose(fpgt);

	
	
	/* Threshold and calculate the tpr and fpr. */
	fpeval = fopen("detections.txt", "a");
	for (i = 0; i < 256; i+=15)
	{	
		printf("Creating binary image with thresh: %d\n", i);
		for (r = 0; r < rows; r++)
		{
			for (c = 0; c < cols ; c++)
			{
				if (norm[r*cols + c] >= i)
				{
					binimg[r*cols + c] = 255;
				} else {
					  binimg[r*cols + c] = 0;
				}
			}
		}

		
		TP = 0;
		FP = 0;
		TN = 0;
		FN = 0;

		for (j = 0; j < MAX_ROWS; j++)
		{
			cr = x_pos[j];
			cc = y_pos[j];
			// printf("row:%d, col:%d ",cr,cc);
			// printf("%u ", binimg[cr*cols + cc]);
			for (dr = -7; dr <= 7; dr++)
			{		detected = 0;
				for (dc = -4; dc <= 4; dc++)
				{  
					if (binimg[(cr+dr)*cols + (cc+dc)] == 255)
					{
						detected = 1;
						break;
					} else {
						 detected = 0;
					}
				}
				if (detected == 1)
				{
					break;
				}
			}
			if (detected == 1 && letter[j] == 'e')
			{
				TP = TP + 1;
			}else if(detected == 1 && letter[j] != 'e')
			{
				FP = FP + 1;
			}else if(detected == 0 && letter[j] == 'e')
			{
				FN = FN + 1;
			}else if(detected == 0 && letter[j] != 'e')
			{
				TN = TN + 1;
			}

			
		}
		if (i == 180)
		{
			write_msf_image(fp, "bin_image.ppm", binimg, rows, cols, maxvals);

		}

		tpr = TP/(TP+FN);
		fpr = FP/(FP+TN);
		printf("tpr: %f\n",tpr);
		printf("fpr: %f\n", fpr);
		// fprintf(fpeval, "%d %0.0f %0.0f %0.0f %0.0f\n", i, TP, FP, FN, TN);
		fprintf(fpeval, "%d %1.3f %1.3f\n", i, tpr, fpr);
		

	}
	fclose(fpeval);

	// write_msf_image(fp, "bin_image.ppm", binimg, rows, cols, maxvals);
	// ppm_write(fp, "mean_centered.ppm", img);

	free(norm);
	free(out);
	free(binimg);
	free(mean_buf);
	free(img->vals);
	free(img);
	free(tplt->vals);
	free(tplt);
	return 0;
}
