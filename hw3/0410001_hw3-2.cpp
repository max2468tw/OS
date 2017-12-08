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
	int j;
	sem_t* sem;
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
		
		sem_t sem[16];
		
		info in[16];
		pthread_t th1[16];
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				sem_init(&sem[j*4+i], 0, 1);
				in[j*4+i].imgWidth = imgWidth;
				in[j*4+i].imgHeight = imgHeight;
				in[j*4+i].pic_in = pic_in;
				in[j*4+i].pic_grey = pic_grey;
				in[j*4+i].pic_Gx = pic_Gx;
				in[j*4+i].pic_Gy = pic_Gy;
				in[j*4+i].pic_blur = pic_blur;
				in[j*4+i].pic_final = pic_final;
				in[j*4+i].i = i;
				in[j*4+i].j = j;
				in[j*4+i].sem = &sem[j*4+i];
				pthread_create( &th1[j*4+i], NULL, grey, &in[j*4+i]);
			}
		}
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_join( th1[j*4+i], NULL);
			}
		}
		pthread_t th2[16];
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_create( &th2[j*4+i], NULL, Gx, &in[j*4+i]);
			}
		}
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_join( th2[j*4+i], NULL);
			}
		}

		pthread_t th3[16];
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_create( &th3[j*4+i], NULL, Gy, &in[j*4+i]);
			}
		}
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_join( th3[j*4+i], NULL);
			}
		}
		pthread_t th4[16];
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_create( &th4[j*4+i], NULL, calculate, &in[j*4+i]);
			}
		}
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_join( th4[j*4+i], NULL);
			}
		}
		
		pthread_t th5[16];
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_create( &th5[j*4+i], NULL, extend, &in[j*4+i]);
			}
		}
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_join( th5[j*4+i], NULL);
			}
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
	int h_top = imgHeight/4*((((info *)ptr)->j)+1);
	int h_button = imgHeight/4*(((info *)ptr)->j);
	int w_top = imgWidth/4*((((info *)ptr)->i)+1);
	int w_button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_in = ((info *)ptr)->pic_in;
	unsigned char *pic_grey = ((info *)ptr)->pic_grey;
	for (int j = h_button; j<h_top; j++) {
		for (int i = w_button; i<w_top; i++){
			pic_grey[j*imgWidth + i] = RGB2grey(i, j, imgWidth, pic_in);
		}		
	}
}

void *Gx( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int h_top = imgHeight/4*((((info *)ptr)->j)+1);
	int h_button = imgHeight/4*(((info *)ptr)->j);
	int w_top = imgWidth/4*((((info *)ptr)->i)+1);
	int w_button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_grey = ((info *)ptr)->pic_grey;
	unsigned char *pic_Gx = ((info *)ptr)->pic_Gx;
	for (int j = h_button; j<h_top; j++) {
		for (int i = w_button; i<w_top; i++){
			pic_Gx[j*imgWidth + i] = GxFilter(i, j, imgWidth, imgHeight, pic_grey);
		}
	}
}

void *Gy( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int h_top = imgHeight/4*((((info *)ptr)->j)+1);
	int h_button = imgHeight/4*(((info *)ptr)->j);
	int w_top = imgWidth/4*((((info *)ptr)->i)+1);
	int w_button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_grey = ((info *)ptr)->pic_grey;
	unsigned char *pic_Gy = ((info *)ptr)->pic_Gy;
	for (int j = h_button; j<h_top; j++) {
		for (int i = w_button; i<w_top; i++){
			pic_Gy[j*imgWidth + i] = GyFilter(i, j, imgWidth, imgHeight, pic_grey);
		}
	}
}

void *calculate( void *ptr )
{
	sem_t* sem = ((info *)ptr)->sem;
	sem_wait (sem);
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int h_top = imgHeight/4*((((info *)ptr)->j)+1);
	int h_button = imgHeight/4*(((info *)ptr)->j);
	int w_top = imgWidth/4*((((info *)ptr)->i)+1);
	int w_button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_Gx = ((info *)ptr)->pic_Gx;
	unsigned char *pic_Gy = ((info *)ptr)->pic_Gy;
	unsigned char *pic_blur = ((info *)ptr)->pic_blur;
	for (int j = h_button; j<h_top; j++) {
		for (int i = w_button; i<w_top; i++){
			pic_blur[j*imgWidth + i] = sqrt(pic_Gx[j*imgWidth + i]*pic_Gx[j*imgWidth + i] + pic_Gy[j*imgWidth + i]*pic_Gy[j*imgWidth + i]);
		}
	}
	sem_post (sem);
}

void *extend( void *ptr )
{
	sem_t* sem = ((info *)ptr)->sem;
	sem_wait (sem);
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int h_top = imgHeight/4*((((info *)ptr)->j)+1);
	int h_button = imgHeight/4*(((info *)ptr)->j);
	int w_top = imgWidth/4*((((info *)ptr)->i)+1);
	int w_button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_blur = ((info *)ptr)->pic_blur;
	unsigned char *pic_final = ((info *)ptr)->pic_final;
	for (int j = h_button; j<h_top; j++) {
		for (int i = w_button; i<w_top; i++){
			pic_final[3 * (j*imgWidth + i) + MYRED] = pic_blur[j*imgWidth + i];
			pic_final[3 * (j*imgWidth + i) + MYGREEN] = pic_blur[j*imgWidth + i];
			pic_final[3 * (j*imgWidth + i) + MYBLUE] = pic_blur[j*imgWidth + i];
		}
	}
}