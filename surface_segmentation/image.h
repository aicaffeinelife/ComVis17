#ifndef IMAGE_H_
#define IMAGE_H_ 

typedef struct Image {
	int rows; 
	int cols;
	int maxvals;
	unsigned char *vals;
}image_t;




typedef struct Filter{
	int radius;
	int norm;
	const unsigned char *vals;
}filter_t;
#endif