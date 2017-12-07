// Student ID: 0410001
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

typedef struct {
    int imgWidth;
    int imgHeight;
	unsigned char *pic_in, *pic_grey, *pic_Gx, *pic_Gy, *pic_blur, *pic_final;
	int i;
} info;

int FILTER_SIZE;
//int FILTER_SCALE;
int *filter_Gx;
int *filter_Gy;

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputSobel_name[5] = {
	"Sobel1.bmp",
	"Sobel2.bmp",
	"Sobel3.bmp",
	"Sobel4.bmp",
	"Sobel5.bmp"
};

unsigned char RGB2grey(int w, int h, int imgWidth, unsigned char *pic_in)
{
	int tmp = (
		pic_in[3 * (h*imgWidth + w) + MYRED] +
		pic_in[3 * (h*imgWidth + w) + MYGREEN] +
		pic_in[3 * (h*imgWidth + w) + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char GxFilter(int w, int h, int imgWidth, int imgHeight, unsigned char *pic_grey)
{
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

		tmp += filter_Gx[j*ws + i] * pic_grey[b*imgWidth + a];
	};
	//tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char GyFilter(int w, int h, int imgWidth, int imgHeight, unsigned char *pic_grey)
{
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

		tmp += filter_Gy[j*ws + i] * pic_grey[b*imgWidth + a];
	};
	//tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}
void *function( void *ptr );
void *grey( void *ptr );
void *Gx( void *ptr );
void *Gy( void *ptr );
void *calculate( void *ptr );
void *extend( void *ptr );
int main()
{
	// read mask file
	FILE* mask;
	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	//fscanf(mask, "%d", &FILTER_SCALE);

	filter_Gx = new int[FILTER_SIZE];
	filter_Gy = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_Gx[i]);
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_Gy[i]);
	fclose(mask);

	int i[5] = {0,1,2,3,4};
	pthread_t th[5];
	for (int k = 0; k<5; k++){
		pthread_create( &th[k], NULL, function, i + k);
	}
	for (int k = 0; k<5; k++){
		pthread_join( th[k], NULL);
	}

	return 0;
}

void *function( void *ptr )
{
		int imgWidth, imgHeight;
		unsigned char *pic_in, *pic_grey, *pic_Gx, *pic_Gy, *pic_blur, *pic_final;
		int k = *((int*)ptr);
		BmpReader* bmpReader = new BmpReader();
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_Gx = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_Gy = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_blur = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		
		/*//convert RGB image to grey image
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_grey[j*imgWidth + i] = RGB2grey(i, j);
			}
		}*/
		info in[4];
		pthread_t th1[4];
		for (int i = 0; i<4; i++){
			in[i].imgWidth = imgWidth;
			in[i].imgHeight = imgHeight;
			in[i].pic_in = pic_in;
			in[i].pic_grey = pic_grey;
			in[i].pic_Gx = pic_Gx;
			in[i].pic_Gy = pic_Gy;
			in[i].pic_blur = pic_blur;
			in[i].pic_final = pic_final;
			in[i].i = i;
			pthread_create( &th1[i], NULL, grey, &in[i]);
		}
		for (int i = 0; i<4; i++){
			pthread_join( th1[i], NULL);
		}
		
		/*//apply the Gx filter to the image
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_Gx[j*imgWidth + i] = GxFilter(i, j);
			}
		}*/
		
		/*//apply the Gy filter to the image
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_Gy[j*imgWidth + i] = GyFilter(i, j);
			}
		}*/
		pthread_t th2[4];
		pthread_t th3[4];
		for (int i = 0; i<4; i++){
			pthread_create( &th2[i], NULL, Gx, &in[i]);
			pthread_create( &th3[i], NULL, Gy, &in[i]);
		}
		for (int i = 0; i<4; i++){
			pthread_join( th2[i], NULL);
			pthread_join( th3[i], NULL);
		}
		
		/*pthread_t th3[4];
		for (int i = 0; i<4; i++){
			pthread_create( &th3[i], NULL, Gy, &in[i]);
		}
		for (int i = 0; i<4; i++){
			pthread_join( th3[i], NULL);
		}*/
		/*for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_blur[j*imgWidth + i] = sqrt(pic_Gx[j*imgWidth + i]*pic_Gx[j*imgWidth + i] + pic_Gy[j*imgWidth + i]*pic_Gy[j*imgWidth + i]);
			}
		}*/
		pthread_t th4[4];
		for (int i = 0; i<4; i++){
			pthread_create( &th4[i], NULL, calculate, &in[i]);
		}
		for (int i = 0; i<4; i++){
			pthread_join( th4[i], NULL);
		}
		
		//extend the size form WxHx1 to WxHx3
		/*for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_final[3 * (j*imgWidth + i) + MYRED] = pic_blur[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYGREEN] = pic_blur[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYBLUE] = pic_blur[j*imgWidth + i];
			}
		}*/
		pthread_t th5[4];
		for (int i = 0; i<4; i++){
			pthread_create( &th5[i], NULL, extend, &in[i]);
		}
		for (int i = 0; i<4; i++){
			pthread_join( th5[i], NULL);
		}
		// write output BMP file
		bmpReader->WriteBMP(outputSobel_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_Gx);
		free(pic_Gy);
		free(pic_blur);
		free(pic_final);
}

void *grey( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int top = imgWidth/4*((((info *)ptr)->i)+1);
	int button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_in = ((info *)ptr)->pic_in;
	unsigned char *pic_grey = ((info *)ptr)->pic_grey;
	for (int j = 0; j<imgHeight; j++) {
		for (int i = button; i<top; i++){
			pic_grey[j*imgWidth + i] = RGB2grey(i, j, imgWidth, pic_in);
		}		
	}
}

void *Gx( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int top = imgWidth/4*((((info *)ptr)->i)+1);
	int button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_grey = ((info *)ptr)->pic_grey;
	unsigned char *pic_Gx = ((info *)ptr)->pic_Gx;
	for (int j = 0; j<imgHeight; j++) {
		for (int i = button; i<top; i++){
			pic_Gx[j*imgWidth + i] = GxFilter(i, j, imgWidth, imgHeight, pic_grey);
		}
	}
}

void *Gy( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int top = imgWidth/4*((((info *)ptr)->i)+1);
	int button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_grey = ((info *)ptr)->pic_grey;
	unsigned char *pic_Gy = ((info *)ptr)->pic_Gy;
	for (int j = 0; j<imgHeight; j++) {
		for (int i = button; i<top; i++){
			pic_Gy[j*imgWidth + i] = GyFilter(i, j, imgWidth, imgHeight, pic_grey);
		}
	}
}

void *calculate( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int top = imgWidth/4*((((info *)ptr)->i)+1);
	int button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_Gx = ((info *)ptr)->pic_Gx;
	unsigned char *pic_Gy = ((info *)ptr)->pic_Gy;
	unsigned char *pic_blur = ((info *)ptr)->pic_blur;
	for (int j = 0; j<imgHeight; j++) {
		for (int i = button; i<top; i++){
			pic_blur[j*imgWidth + i] = sqrt(pic_Gx[j*imgWidth + i]*pic_Gx[j*imgWidth + i] + pic_Gy[j*imgWidth + i]*pic_Gy[j*imgWidth + i]);
		}
	}
}

void *extend( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int top = imgWidth/4*((((info *)ptr)->i)+1);
	int button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_blur = ((info *)ptr)->pic_blur;
	unsigned char *pic_final = ((info *)ptr)->pic_final;
	for (int j = 0; j<imgHeight; j++) {
		for (int i = button; i<top; i++){
			pic_final[3 * (j*imgWidth + i) + MYRED] = pic_blur[j*imgWidth + i];
			pic_final[3 * (j*imgWidth + i) + MYGREEN] = pic_blur[j*imgWidth + i];
			pic_final[3 * (j*imgWidth + i) + MYBLUE] = pic_blur[j*imgWidth + i];
		}
	}
}