// Student ID:
// Name      :
// Date      : 2017.11.03

#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0

int imgWidth, imgHeight;
int FILTER_SIZE;
int FILTER_SCALE;
int *filter_G;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputBlur_name[5] = {
	"Blur1.bmp",
	"Blur2.bmp",
	"Blur3.bmp",
	"Blur4.bmp",
	"Blur5.bmp"
};

unsigned char *pic_in, *pic_blur, *pic_final;

void* GaussianFilter( void *ptr )
{	
	int offset = *((int*)ptr);
	for (int h = 0; h<imgHeight; h++) {
		for (int w = 0; w<imgWidth; w++){
			int tmp = 0;
			int a, b;
			int ws = (int)sqrt((float)FILTER_SIZE);
			for (int j = 0; j<ws; j++)
			for (int i = 0; i<ws; i++)
			{
				a = w + i - (ws / 2);
				b = h + j - (ws / 2);

				// detect for borders of the image
				if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

				tmp += filter_G[j*ws + i] * pic_in[3*(b*imgWidth + a)+offset];
			}
			tmp /= 3;
			tmp /= FILTER_SCALE;
			if (tmp < 0) tmp = 0;
			if (tmp > 255) tmp = 255;
			//pthread_mutex_lock( &mutex );
			pic_blur[h*imgWidth + w] += tmp;
			//pthread_mutex_unlock( &mutex );
		}
	}
}

int main()
{
	// read mask file
	FILE* mask;
	mask = fopen("mask_Gaussian.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	fscanf(mask, "%d", &FILTER_SCALE);

	filter_G = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_G[i]);
	fclose(mask);


	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k<5; k++){
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_blur = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_blur[j*imgWidth + i] = 0;
			}
		}
		pthread_t th1, th2, th3;
		int RED, GREEN, BLUE;
		RED = 2;
		GREEN = 1;
		BLUE = 0;
		//apply the Gaussian filter to the image
		pthread_create( &th1, NULL, GaussianFilter, &RED);
		pthread_create( &th2, NULL, GaussianFilter, &BLUE);
		pthread_create( &th3, NULL, GaussianFilter, &GREEN);
		pthread_join( th1, NULL);
		pthread_join( th2, NULL);
		pthread_join( th3, NULL);
		//extend the size form WxHx1 to WxHx3
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_final[3 * (j*imgWidth + i) + MYRED] = pic_blur[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYGREEN] = pic_blur[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYBLUE] = pic_blur[j*imgWidth + i];
			}
		}

		// write output BMP file
		bmpReader->WriteBMP(outputBlur_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_blur);
		free(pic_final);
	}

	return 0;
}