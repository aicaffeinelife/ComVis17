#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "image.h"
#include "ppm.h"

#define MARKED 1 
#define UNMARKED 0 
#define MAX_QUEUE 1000 


typedef struct 
{
	double x_coord; 
	double y_coord; 
	double z_coord; 
}normal_t;


void threshold_img(image_t *img, int thresh, unsigned char **out){

	int r,c;

	for(r = 0 ; r < img->rows; r++){
		for(c = 0; c < img->cols; c++){
			if(img->vals[r*img->cols + c] < thresh)
				(*out)[r*img->cols + c] = 0;
			else
				(*out)[r*img->cols + c] = 255;
		}
	}
}




void write_msf_image(FILE *fp, const char *fname, unsigned char *out, int rows, int cols, int maxvals){
	
	fp = fopen(fname, "w");
	fprintf(fp, "%s %d %d %d\n", header, cols, rows, maxvals);
	fwrite(out, sizeof(unsigned char), rows*cols, fp);
	fclose(fp);

}

void convert_two_to_three_dimension(int ImageFlag, int ROWS, int COLS, unsigned char *RangeImage, double P[][128*128]){
	int r,c; 
	double	cp[7];
	double	xangle,yangle,dist;
	double	ScanDirectionFlag,SlantCorrection;
	// unsigned char	RangeImage[128*128];
	//double		P[3][128*128];
	int             ImageTypeFlag;
	cp[0]=1220.7;		/* horizontal mirror angular velocity in rpm */
	cp[1]=32.0;		/* scan time per single pixel in microseconds */
	cp[2]=(COLS/2)-0.5;		/* middle value of columns */
	cp[3]=1220.7/192.0;	/* vertical mirror angular velocity in rpm */
	cp[4]=6.14;		/* scan time (with retrace) per line in milliseconds */
	cp[5]=(ROWS/2)-0.5;		/* middle value of rows */
	cp[6]=10.0;		/* standoff distance in range units (3.66cm per r.u.) */

	cp[0]=cp[0]*3.1415927/30.0;	/* convert rpm to rad/sec */
	cp[3]=cp[3]*3.1415927/30.0;	/* convert rpm to rad/sec */
	cp[0]=2.0*cp[0];		/* beam ang. vel. is twice mirror ang. vel. */
	cp[3]=2.0*cp[3];		/* beam ang. vel. is twice mirror ang. vel. */
	cp[1]/=1000000.0;		/* units are microseconds : 10^-6 */
	cp[4]/=1000.0;			/* units are milliseconds : 10^-3 */

	switch(ImageTypeFlag)
  {
  case 1:		/* Odetics image -- scan direction upward */
    ScanDirectionFlag=-1;
    break;
  case 0:		/* Odetics image -- scan direction downward */
    ScanDirectionFlag=1;
    break;
  default:		/* in case we want to do this on synthetic model */
    ScanDirectionFlag=0;
    break;
  }


if (ImageTypeFlag != 3)
  {
  for (r=0; r<ROWS; r++)
    {
    for (c=0; c<COLS; c++)
      {
      SlantCorrection=cp[3]*cp[1]*((double)c-cp[2]);
      xangle=cp[0]*cp[1]*((double)c-cp[2]);
      yangle=(cp[3]*cp[4]*(cp[5]-(double)r))+	/* Standard Transform Part */
	SlantCorrection*ScanDirectionFlag;	/*  + slant correction */
      dist=(double)RangeImage[r*COLS+c]+cp[6];
      P[2][r*COLS+c]=sqrt((dist*dist)/(1.0+(tan(xangle)*tan(xangle))
	+(tan(yangle)*tan(yangle))));
      P[0][r*COLS+c]=tan(xangle)*P[2][r*COLS+c];
      P[1][r*COLS+c]=tan(yangle)*P[2][r*COLS+c];
      }
    }
  }
  

}



// calculate the surface normals for each surface. 
void surface_normals(double P[3][128*128], int dist, int rows, int cols, normal_t *n){

	
	double b_x_x, b_x_y, b_x_z; 
	double a_x_x, a_x_y, a_x_z;
	int r,c; 

	for(r=0; r < (rows-dist); r++){
		for(c=0; c < (cols-dist); c++){
			b_x_x = b_x_y = b_x_z = a_x_x = a_x_y = a_x_z = 0;

			b_x_x = P[0][(r+dist)*cols + c] - P[0][r*cols + c];
	 		b_x_y = P[1][(r+dist)*cols + c] - P[1][r*cols + c];
	 		b_x_z = P[2][(r+dist)*cols + c] - P[2][r*cols + c];

	 		a_x_x = P[0][r*cols + c + dist] - P[0][r*cols + c];
	 		a_x_y = P[1][r*cols + c + dist] - P[1][r*cols + c];
	 		a_x_z = P[2][r*cols + c + dist] - P[2][r*cols + c];
	 		n[r*cols + c].x_coord = (b_x_y*a_x_z - a_x_y*b_x_z);
	 		n[r*cols + c].y_coord = (b_x_z*a_x_x - b_x_x*a_x_z); 
	 		n[r*cols + c].z_coord = (a_x_x*b_x_y - b_x_x*a_x_y);

		}
	}
	

	 


	// n->x_coord = (b_x_y*a_x_z - a_x_y*b_x_z);
	// n->y_coord = (b_x_z*a_x_x - b_x_x*a_x_z); 
	// n->z_coord = (a_x_x*b_x_y - b_x_x*a_x_y);
	
}


double get_angle(normal_t n1, normal_t n2){
	double res, ang, len1, len2; 
	res = n1.x_coord*n2.x_coord + n1.y_coord*n2.y_coord + n1.z_coord*n2.z_coord;
	len1 = n1.x_coord*n1.x_coord + n1.y_coord*n1.y_coord + n1.z_coord*n1.z_coord;
	len2 = n2.x_coord*n2.x_coord + n2.y_coord*n2.y_coord + n2.z_coord*n2.z_coord;
	ang = acos(res/sqrt(len1*len2));
	return ang;
}



void RegionGrow(unsigned char *labels,
			    normal_t *nrml, 
			    int r, int c,
			    int rows, int cols,
			    int paint_over_label, 
			    int new_label, 
			    int *indices, 
			    int *count 
			    ){


	int dr, dc; 
	int queue[MAX_QUEUE],qh,qt;
	double sum_x, sum_y,sum_z;
	normal_t avg_nrml; 
	

	(*count) = 0;
	

	if (indices != NULL)
	{
		indices[0] = r*cols + c;
	}
	qh = 1; 
	qt = 0;
	queue[0] = r*cols +c;
	labels[0] = new_label;
	avg_nrml.x_coord = sum_x = nrml[r*cols+c].x_coord;
	avg_nrml.y_coord = sum_y = nrml[r*cols + c].y_coord;
	avg_nrml.z_coord = sum_z = nrml[r*cols + c].z_coord;



	while(qh != qt){
		
		for(dr=-2; dr<=2;dr++){
			
			for(dc=-2;dc<=2;dc++){
				
				if(dr == 0 && dc == 0)
					continue; 

				if((queue[qt]/cols + dr) < 0 || (queue[qt]/cols +dr) > (rows-2) || (queue[qt]%cols + dc) < 0 || (queue[qt]%cols + dc) > (cols-2))
					continue; 

				if(labels[(queue[qt]/cols +dr)*cols + (queue[qt]%cols+dc)] != paint_over_label)
					continue;

				if(get_angle(nrml[(queue[qt]/cols+dr)*cols + (queue[qt]%cols + dc)], avg_nrml) > 0.78)
						continue;

				labels[(queue[qt]/cols + dr)*cols + (queue[qt]%cols + dc)] = new_label;

				if(indices != NULL)
					indices[*count] = (queue[qt]/cols + dr)*cols + (queue[qt]%cols +dc); 

				
				sum_x += nrml[(r+dr)*cols+(c+dc)].x_coord;
				sum_y += nrml[(r+dr)*cols + (c+dc)].y_coord;
				sum_z += nrml[(r+dr)*cols + (c+dc)].z_coord;
				(*count)++;
				avg_nrml.x_coord = sum_x/(*count);
				avg_nrml.y_coord = sum_y/(*count);
				avg_nrml.z_coord = sum_z/(*count);

				queue[qh] = (queue[qt]/cols +dr)*cols + (queue[qt]%cols + dc); 
		
				qh = (qh + 1)%MAX_QUEUE;

				if(qh == qt)
				{
					printf("queue size exceeded \n");
					exit(0);
				}
			}
		}
		qt = (qt+1)%MAX_QUEUE;
	}

printf("%f %f %f \n", avg_nrml.x_coord, avg_nrml.y_coord, avg_nrml.z_coord);
}

int main(int argc, char const *argv[])
{
	
	if(argc != 2){
		printf("Usage sseg <range_image> \n");
		exit(1);
	}
	FILE *fp; 
	int rows, cols;
	int r,c, i, dr,dc;
	int check;
	int *indices;
	int  TotalRegions, RegionSize;
	int   NewLabelColor;
	double ang; 

	const char *ofname = "thresh_img_1.ppm";
	double P[3][128*128];
	image_t *img = (image_t *)malloc(sizeof(image_t));
	

	ppm_read(fp, argv[1], &img);

	assert(img->rows == 128);
	assert(img->cols == 128);
	rows = img->rows; 
	cols = img->cols; 
	normal_t *nrml = (normal_t *)malloc(rows*cols*sizeof(normal_t));
	unsigned char *thresh_img = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	unsigned char *label = (unsigned char *)calloc(rows*cols, sizeof(unsigned char)); // the label image.
	//unsigned char *label = (unsigned char *)calloc()

	threshold_img(img, 128, &thresh_img);
	//write_msf_image(fp, ofname, thresh_img, rows, cols, img->maxvals);
	convert_two_to_three_dimension(1, img->rows, img->cols, img->vals, P);
	surface_normals(P, 3, rows, cols, nrml);
	
	

	TotalRegions = 0; 
	for(r = 2; r < rows-2; r++){
		for(c = 2; c < cols-2; c++){

			check = 0;
			
			
			for(dr = -2; dr <= 2; dr ++ ){
				for(dc = -2; dc <=2; dc++){
					if(thresh_img[(r+dr)*cols + (c+dc)]!= 0 || label[(r+dr)*cols + (c+dc)] != 0){
						check = 1;
						break;
					}

				}
			}


		
			
			if(!check){
				printf("Seed pixel @: %d %d\n", r,c);
				TotalRegions++; 
				if (TotalRegions == 255)
				{
					printf("Segmentation incomplete.\n");
					break;
				}

				
				RegionGrow(label,nrml,r,c,rows,cols,0,TotalRegions,
								indices,&RegionSize);
			

			if (RegionSize < 100)
        	{	/* erase region (relabel pixels back to 0) */
        		for (i=0; i<RegionSize; i++)
          		label[indices[i]]=0;
        		TotalRegions--;
        	}
      		else
        		printf("Region labeled %d is %d in size\n",TotalRegions,RegionSize);

			}


				
		}
		if (c < cols-2)
    		break;
    	

	}

	NewLabelColor = 255/TotalRegions;
	for(i = 0; i < (rows*cols); i++)
		label[i] = label[i]*NewLabelColor;

	printf("%d total regions were found\n",TotalRegions);
	fp = fopen("label_image.ppm", "w");
	fprintf(fp,"P5 %d %d 255\n",cols,rows);
	fwrite(label,1,rows*cols,fp);
	fclose(fp);	
	free(thresh_img);
	free(nrml);

	return 0;
	
}