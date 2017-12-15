#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "ppm.h"
#include "image.h"
#define MAX_ROWS 42
#define MAX_ITER 30
#define SQR(x) ((x)*(x))

char header[80];


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


void write_file(FILE *fp, const char *fname, unsigned char *out, int rows, int cols, int maxvals){

	fp = fopen(fname, "w");
	fprintf(fp, "%s %d %d %d\n", header, cols, rows, maxvals);
	fwrite(out, sizeof(unsigned char), rows*cols, fp);
	fclose(fp);
}

void calc_edges(unsigned char *vals, int rows, int cols, unsigned char *op){
	
	int dr,dc;
	int r,c;
	int sum_x, sum_y;
	int sobel_filt_x[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	}; 

	int sobel_filt_y[3][3] = {
		{1,2,1},
		{0,0,0},
		{-1,-2,-1}
	};
	for(r=1;r<rows-1;r++){

		for(c=1;c<cols-1;c++){

			sum_x = 0, sum_y = 0; 

			for(dr=-1;dr<=1;dr++){

				for(dc=-1;dc<=1;dc++){

					sum_x += vals[(r+dr)*cols + (c+dc)]*sobel_filt_x[(dr+1)][(dc+1)];
					sum_y += vals[(r+dr)*cols + (c+dc)]*sobel_filt_y[(dr+1)][(dc+1)];
				}
			}

			

			op[r*cols + c] = (unsigned char)sqrt(SQR(sum_x) + SQR(sum_y));

		}
	}



	
	
}

int calc_avg_distance(int *x_pos, int *y_pos){
   int avg, i; 
   int dists[MAX_ROWS], sum =0;
   for(i = 0; i< MAX_ROWS; i++){
   	  sum += SQR((y_pos[i%MAX_ROWS]- y_pos[(i+1)%MAX_ROWS])) + SQR((x_pos[i%MAX_ROWS] - x_pos[(i+1)%MAX_ROWS]));
   }
   avg = sum/MAX_ROWS;


   return avg; 
}



void calc_external(unsigned char *vals, int x, int y, int rows, int cols, int *ext_engy){
	int dr, dc; 
	
	for(dr = -3; dr<=3; dr++){
		for(dc = -3; dc<=3; dc++){
			ext_engy[(dr+3)*7 + (dc+3)] = -(int)vals[(y+dr)*cols + (x+dc)]; 
		}
	}

}


void calc_internal_one(int x, int y, int x_next, int y_next, int *int_engy){

	int dr,dc; 
	for(dr = -3; dr<=3; dr++){
		for(dc = -3; dc<=3; dc++){
			int_engy[(dr+3)*7 + (dc+3)] = ((y+dr)-y_next)*((y+dr)-y_next) + ((x+dc)-x_next)*((x+dc)-x_next);
		}
	}
	

}

void calc_internal_two(int x, int y, int x_next, int y_next, int *xpos, int *ypos, int *int_engy2){
	int dr,dc; 
	
	int avg = calc_avg_distance(xpos, ypos);
	//printf("%d\n", avg);
	for(dr = -3; dr<=3; dr++){
		for(dc = -3; dc<=3; dc++){
			int_engy2[(dr+3)*7 + (dc+3)] = SQR(((y+dr-y_next)*(y+dr-y_next) + (x+dc-x_next)*(x+dc -x_next)) - avg);
		}
	}
	

}

void sum_energies(unsigned char *vals, int *ie_1, int *ie_2, int r, int c, int rows, int cols, int *up_r, int *up_c){
	int *tot = (int *)calloc(49, sizeof(int));
	int up_id_r , up_id_c;
	int dr, dc; 
	int min;
	int part_sum = 0;
	for(dr = -3; dr<=3; dr++){

		for(dc = -3; dc<=3; dc++){
			part_sum = ie_1[(dr+3)*7 + (dc+3)] - (int)vals[(r+dr)*cols + (c+dc)];
			tot[(dr+3)*7 + (dc+3)] = part_sum + ie_2[(dr+3)*7 + (dc+3)];
		}
	}

	min = tot[0];
	up_id_r = -3;
	up_id_c = -3;
	for(dr = -3; dr<= 3; dr++){

		for(dc = -3; dc<= 3; dc++){

			if(tot[(dr+3)*7 + (dc+3)] < min){
				min = tot[(dr+3)*7 + (dc+3)];
				up_id_r = dr;
				up_id_c = dc; 
			}
		}
	}

	*up_r = r + up_id_r;
	*up_c = c + up_id_c;


	
}


void copy_from(int *dest, int *src){
	int  i;
	for (i = 0; i < MAX_ROWS; ++i)
	{
		dest[i] = src[i];
	}
}

int main(int argc, char const *argv[])
{
	if (argc != 4)
	{
		printf("Usage snakes ip_img contour_list output_img\n");
		exit(1);
	}

	FILE *fp, *fp_cnt, *fp_res; 
	image_t *img = (image_t *)malloc(sizeof(image_t));
	int i, j;
	int dr,dc;
	int r,c;
	int cr, cc;
	int it; // iterator
	int rows, cols; 
	unsigned char *out;
	unsigned char *vals;
	int max_lim, min_lim; 
	int avg;
	
	ppm_read(fp, argv[1], &img); 
	int x_pos[MAX_ROWS], y_pos[MAX_ROWS]; 
	int x_pos_updated[MAX_ROWS], y_pos_updated[MAX_ROWS];
	// int *ext_energy, *int_energy_1, *int_energy_2, *tot_energy;

	fp_cnt = fopen(argv[2], "r");
	if (fp_cnt == NULL)
	{
		printf("Initial cotour list not found\n");
		exit(1);
	}
	rows = img->rows; 
	cols = img->cols; 
	out = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	vals = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	memcpy(out, img->vals, rows*cols*sizeof(unsigned char));
	for(i=0;i<MAX_ROWS;i++){
		fscanf(fp_cnt, "%d %d \n", &x_pos[i], &y_pos[i]);
	}
	fclose(fp_cnt);

	
	calc_edges(img->vals, rows, cols, vals);
	write_file(fp, "edge_img.ppm", vals, rows, cols, img->maxvals);
	

	// active contour 
	printf("Building active contours ...\n");
	it = 0; 
    copy_from(x_pos_updated, x_pos);
    copy_from(y_pos_updated, y_pos);
	
	while(it < MAX_ITER){

		copy_from(x_pos, x_pos_updated);
		copy_from(y_pos, y_pos_updated);

		for (i = 0; i < MAX_ROWS; ++i)
		{
			int new_r, new_c;
		 	cr = y_pos[i%MAX_ROWS];
			cc = x_pos[i%MAX_ROWS];
			int next_r = y_pos[(i+1) % MAX_ROWS];
			int next_c = x_pos[(i+1) % MAX_ROWS];
			int *int_energy_1 = (int *)calloc(7*7, sizeof(int));
			int *int_energy_2 = (int *)calloc(7*7, sizeof(int));
			calc_internal_one(cc, cr, next_c, next_r, int_energy_1);
			calc_internal_two(cc,cr,next_c, next_r, x_pos,y_pos, int_energy_2);
		    sum_energies(vals, int_energy_1, int_energy_2, cr,cc, rows, cols, &new_r, &new_c);
		    x_pos_updated[i] = new_c; 
		    y_pos_updated[i] = new_r;
		    free(int_energy_1);
		    free(int_energy_2);

		}

		it++;
	}

	fp_res = fopen("result_contours.txt", "w");
	for (i = 0; i < MAX_ROWS; ++i)
		{
			fprintf(fp_res, "%d %d \n", x_pos[i], y_pos[i]);
		}
	
	for(i=0;i<MAX_ROWS;i++){
		cr = y_pos_updated[i];
		cc = x_pos_updated[i];

	for(dr=-3; dr<=3; dr++){
		out[(cr+dr)*cols + cc] = 0;
	}

	for(dc=-3;dc<=3;dc++){
			out[cr*cols + cc+dc] = 0;
	}

	}
	write_file(fp, "hawk_expt.ppm", out, rows, cols, img->maxvals);
		

	//ppm_write(fp, argv[3], img);



	return 0;
}












