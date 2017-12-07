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
	unsigned char *pic_in, *pic_grey, *pic_blur, *pic_final;
	int i;
	int j;
} info;

int FILTER_SIZE;
int FILTER_SCALE;
int *filter_G;

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

unsigned char GaussianFilter(int w, int h, int imgWidth, int imgHeight, unsigned char *pic_grey)
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

		tmp += filter_G[j*ws + i] * pic_grey[b*imgWidth + a];
	};
	tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

void *function( void *ptr );
/*void *pic_malloc1( void *ptr );
void *pic_malloc2( void *ptr );
void *pic_malloc3( void *ptr );*/
void *grey( void *ptr );
void *Gaussian( void *ptr );
void *extend( void *ptr );

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
		unsigned char *pic_in, *pic_grey, *pic_blur, *pic_final;
		// read input BMP file
		BmpReader* bmpReader = new BmpReader();
		pic_in = bmpReader->ReadBMP(inputfile_name[*((int*)ptr)], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_blur = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		
		/*info a;
		a.imgWidth = imgWidth;
		a.imgHeight = imgHeight;
		a.pic_grey = pic_grey;
		a.pic_blur = pic_blur;
		a.pic_final = pic_final;
		pthread_t th0[3];
		pthread_create( &th0[0], NULL, pic_malloc1, &a);
		pthread_create( &th0[1], NULL, pic_malloc2, &a);
		pthread_create( &th0[2], NULL, pic_malloc3, &a);
		for (int i = 0; i<3; i++){
			pthread_join( th0[i], NULL);
		}*/
		/*//convert RGB image to grey image
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_grey[j*imgWidth + i] = RGB2grey(i, j, imgWidth, pic_in);
			}
		}*/
		info in[16];
		pthread_t th1[16];
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				in[j*4+i].imgWidth = imgWidth;
				in[j*4+i].imgHeight = imgHeight;
				in[j*4+i].pic_in = pic_in;
				in[j*4+i].pic_grey = pic_grey;
				in[j*4+i].pic_blur = pic_blur;
				in[j*4+i].pic_final = pic_final;
				in[j*4+i].i = i;
				in[j*4+i].j = j;
				pthread_create( &th1[j*4+i], NULL, grey, &in[j*4+i]);
			}
		}
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_join( th1[j*4+i], NULL);
			}
		}
		
		
		//apply the Gaussian filter to the image
		/*for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_blur[j*imgWidth + i] = GaussianFilter(i, j, imgWidth, imgHeight, pic_grey);
			}
		}*/
		pthread_t th2[16];
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_create( &th2[j*4+i], NULL, Gaussian, &in[j*4+i]);
			}
		}
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_join( th2[j*4+i], NULL);
			}
		}
		/*//extend the size form WxHx1 to WxHx3
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_final[3 * (j*imgWidth + i) + MYRED] = pic_blur[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYGREEN] = pic_blur[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYBLUE] = pic_blur[j*imgWidth + i];
			}
		}*/
		pthread_t th3[16];
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_create( &th3[j*4+i], NULL, extend, &in[j*4+i]);
			}
		}
		for (int j = 0; j<4; j++){
			for (int i = 0; i<4; i++){
				pthread_join( th3[j*4+i], NULL);
			}
		}
		
		// write output BMP file
		bmpReader->WriteBMP(outputBlur_name[*((int*)ptr)], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_blur);
		free(pic_final);
}

/*void *pic_malloc1( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	((info *)ptr)->pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
}

void *pic_malloc2( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	((info *)ptr)->pic_blur = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));

}

void *pic_malloc3( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	((info *)ptr)->pic_final = (unsigned char*)malloc(3*imgWidth*imgHeight*sizeof(unsigned char));
}*/

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

void *Gaussian( void *ptr )
{
	int imgHeight = ((info *)ptr)->imgHeight;
	int imgWidth = ((info *)ptr)->imgWidth;
	int h_top = imgHeight/4*((((info *)ptr)->j)+1);
	int h_button = imgHeight/4*(((info *)ptr)->j);
	int w_top = imgWidth/4*((((info *)ptr)->i)+1);
	int w_button = imgWidth/4*(((info *)ptr)->i);
	unsigned char *pic_grey = ((info *)ptr)->pic_grey;
	unsigned char *pic_blur = ((info *)ptr)->pic_blur;
	for (int j = h_button; j<h_top; j++) {
		for (int i = w_button; i<w_top; i++){
			pic_blur[j*imgWidth + i] = GaussianFilter(i, j, imgWidth, imgHeight, pic_grey);
		}
	}
}

void *extend( void *ptr )
{
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