#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"
#include "image.h"

#define MAX_ROWS 1232

char header[80];

typedef struct Pixel{
	int r;
	int c;
}pix_t; 


void write_msf_image(FILE *fp, const char *fname, unsigned char *out, int rows, int cols, int maxvals){
	
	fp = fopen(fname, "w");
	fprintf(fp, "%s %d %d %d\n", header, cols, rows, maxvals);
	fwrite(out, sizeof(unsigned char), rows*cols, fp);
	fclose(fp);

}

 unsigned char * create_thresholded_img(unsigned char *arr, int rows, int cols, int thresh){

	unsigned char *binary = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	int r,c; 
	for (r = 0; r < rows; r++)
	{	
		for (c = 0; c < cols; c++)
		{
			if (arr[r*cols + c] >= thresh)
			{
				binary[r*cols + c] = 255;
			} else{
				binary[r*cols + c] = 0;
			}
		}
	}
	return binary;
}

unsigned char *create_crop(unsigned char *arr, int cr, int cc, int rows, int cols, int temp_rows, int temp_cols){
	/*
		Returns a "crop" of the normalized image centered on (cr,cc).
	 */
	unsigned char *crop = (unsigned char *)calloc(temp_rows*temp_cols, sizeof(unsigned char));
	int dr,dc;
	for (dr = -7; dr <=7 ; ++dr)
	{
		for (dc = -4; dc <=4; ++dc)
		{
			crop[(dr+7)*temp_cols + (dc+4)] = arr[(cr+dr)*cols + (cc+dc)];
			// printf("%u ", );
		}
	}
	return crop;

}



int count_transitions(int r, int c, int row, int cols, unsigned char *crop){
	
	int txs = 0; 
	int i;
	unsigned char trans_arr[8];
	trans_arr[0] = crop[r*cols + c-1];
	trans_arr[1] = crop[(r-1)*cols + c-1];
	trans_arr[2] = crop[(r-1)*cols + c];
	trans_arr[3] = crop[(r-1)*cols + c+1];
	trans_arr[4] = crop[r*cols + c+1];
	trans_arr[5] = crop[(r+1)*cols + c+1];
	trans_arr[6] = crop[(r+1)*cols + c];
	trans_arr[7] = crop[(r+1)*cols + c-1];
	for (i = 1; i <= 7; ++i)
	{   
		if (trans_arr[i-1] == 0 && trans_arr[i] == 255)
		{
			txs++;
		}

		if (i+1 == 8)
		{
			if (trans_arr[i] == 0 && trans_arr[0] == 255)
			{
				txs++;
			}
		}
	}


	return txs;
}

int count_edges(int r, int c, int rows, int cols, unsigned char *crop){
	int dr, dc; 
	int edge_pxs = 0; 
	for (dr = -1; dr <= 1; dr++)
	{
		for (dc = -1; dc <=1 ; dc++)
		{
			if (crop[(r+dr)*cols + (c+dc)] == 0)
				{
					edge_pxs++;
				}	
		}
	}
	// printf("Edge pixesl at %d, %d: %d\n",r,c,edge_pxs);

	return edge_pxs;
}


int check_neighbors(int r, int c, int rows, int cols, unsigned char *crop){
	unsigned char north, south, east, west; 
	int mark = 0 ;
	north = crop[(r-1)*cols + c];
	south = crop[(r+1)*cols + c];
	east = crop[r*cols + c+1];
	west = crop[r*cols + c-1];
	if (north != 0 || east != 0 || (south != 0 && west != 0))
	{
		mark = 1;
	} else{
		mark = 0;
	}

	return mark;
}






int count_endpoints(int rows, int cols, unsigned char *crop){
	int endpts = 0, txs;
	int i,j;
	for (i = 0; i <rows; i++)
	{
		for (j = 0; j < cols; j++)
			{
				txs = count_transitions(i,j,15,9,crop);
				if (txs == 1 && crop[i*9 + j] == 0)
				{
						endpts++;
						printf("Endpoint at: %d %d\n",i,j);
				}
				}
				txs = 0;
			}

	return endpts;
}

int count_branchpts(int rows, int cols, unsigned char *crop){
	int bpts = 0, txs;
	int i,j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
			{
				txs = count_transitions(i,j,15,9,crop);
				if (txs > 2 && crop[i*9 +j] == 0)
				{
					bpts++;
					printf("Branchpoint at: %d %d\n",i,j);
				}
				txs = 0;				

			}
				
			}	

	return bpts;
}



int main(int argc, char const *argv[])
{
	if (argc != 5)
	{
		printf("Usage spm_improved img_name template ground_truth normalized_img \n");
		exit(1);
	}

	image_t *img, *msf; 
	FILE *fp, *fp_gt, *fpeval;
	int rows, cols;
	int r,c,dr,dc, t,i,j;
	int cr,cc;
	int flag, cnt;
	int x_pos[MAX_ROWS], y_pos[MAX_ROWS];
	char letters[MAX_ROWS];
	const char *fname;
	unsigned char *binimg;
	unsigned char *crop, *img_crop;
	unsigned char *img_thresh; 
	int detected,found_e,not_detected;
	int trans, edges, mark;
	float FP, TP, TN, FN; 
	float tpr, fpr;
	int endpts, bpts;
	
	
	img = (image_t *)malloc(sizeof(image_t));
	msf = (image_t *)malloc(sizeof(image_t));
	ppm_read(fp, argv[1], &img);
	ppm_read(fp, argv[4], &msf);
	rows = msf->rows; 
	cols = msf->cols; 
	
	fp_gt = fopen(argv[3], "r");
	if (fp_gt == NULL)
	{
		printf("The ground truth file was not found\n");
		exit(1);
	}
	for (i = 0; i < MAX_ROWS; ++i)
	{
		fscanf(fp_gt, "%c %d %d\n", &letters[i], &y_pos[i], &x_pos[i]);
	}
	fclose(fp_gt);

	
	img_thresh = create_thresholded_img(img->vals, img->rows, img->cols, 128);
	img_crop = create_crop(img_thresh, 25,89,img->rows, img->cols, 15,9);
	write_msf_image(fp, "image_crop_25_89.ppm", img_crop, 15, 9, img->maxvals);

	// thin the entire image.
	do{
		cnt = 0;
		pix_t marked[img->rows*img->cols];
							for (r = 1; r <= (img->rows-1); r++)
							{
							for (c = 1; c <= (img->cols-1); c++)
							{
								trans = count_transitions(r,c,img->rows,img->cols,img_thresh);
								edges = count_edges(r,c,img->rows,img->cols,img_thresh);
								mark = check_neighbors(r,c,img->rows,img->cols,img_thresh);
								
								if (img_thresh[r*img->cols + c] == 0 && trans == 1 && (edges<=7 && edges >= 3) && mark == 1)
								{
									marked[cnt].r = r;
									marked[cnt].c = c;
						    		cnt++;
								}
								trans = 0;
								edges = 0;
								mark = 0;
				
							}
							}
							flag = (cnt > 0);
							for (i = 0; i < cnt; ++i)
							{
								img_thresh[(marked[i].r)*img->cols + (marked[i].c)] = 255;
							}

		} while(flag == 1);

	
	fpeval = fopen("detections_improved.txt", "a");
	for (t = 15; t < 255; t+=15)
	{
		binimg = create_thresholded_img(msf->vals, rows, cols, t);

		TP = 0;
		FP = 0;
		TN = 0;
		FN = 0;
		for (j = 0; j < MAX_ROWS; ++j)
		{	
			cr = x_pos[j];
			cc = y_pos[j];
			for (dr = -7; dr <= 7; dr++)
			{		found_e = 0;
					not_detected = 0;
				for (dc = -4; dc <= 4; dc++)
				{  
					if (binimg[(cr+dr)*cols + (cc+dc)] == 255)
					{ 
					   crop = create_crop(img_thresh, cr,cc,img->rows, img->cols, 15,9);
					   
					   endpts = count_endpoints(15,9, crop);
					   bpts = count_branchpts(15,9,crop);
					   // if (cr == 25 && cc == 197)
					   // {
					   // 	// write_msf_image(fp, "image_crop_25_89_thin.ppm", crop, 15, 9, img->maxvals);
					   // 	 endpts = count_endpoints(15,9, crop);
					  	//  bpts = count_branchpts(15,9,crop);

					   // }

					   if (endpts == 1 && bpts == 1)
					   {  
					   	  if (letters[j] == 'e')
					   	  {
					   	  	TP = TP + 1;
					   	  } else {
					   	  	 FP = FP + 1;
					   	  }
					   	  found_e = 1;
					   	  free(crop);
					   	  break;
					   } else {
					   	
					   		not_detected = 1;
					   		break;
					   }
					    
					
					}
			
				}	
				if (not_detected == 1 || found_e == 1)
					{
						break;
					}	   
			}



			
		 if(not_detected == 1 || found_e == 0){

		 	if (letters[j] != 'e')
		 	{
		 		TN = TN + 1;
		 	}else{
		 		FN = FN + 1;
		 	}
		 }

		
			
} 
			tpr = TP/(TP + FN);
			fpr = FP/(FP + TN);
			fprintf(fpeval, "%d %1.3f %1.3f\n",t, tpr, fpr);
			// fprintf(fpeval, "%d %0.0f %0.0f %0.0f %0.0f\n", t, TP, FP, FN, TN);
}

	fclose(fpeval);

	free(img_crop);
	free(img_thresh);





	return 0;
}
