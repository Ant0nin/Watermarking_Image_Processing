#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmpfuncs.h"

#define MAX_SOURCE_SIZE (0x100000)

int main() {
	

	int h,w;
	float *image_src=NULL;
	image_src=readImage("image/lena.bmp", &h, &w);

	const int elements = h*w;
	size_t imgsize = sizeof(int)*elements;
	float *image_dest;
	image_dest=(float*)malloc(imgsize);
	
	printf("Height:%d \n",h );
	printf("Width:%d \n",w );
	float max=0;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			int index = i*w + j;
			float current_px = image_src[index];
			//printf("%f \n ", image_src[index]);
			if(max < current_px)
				max = current_px;

			/*if (current_px < 255)
				image_dest[index] = image_src[index] + 1.0f;
			else*/
				image_dest[index] = image_src[index]+10;

			//printf("%.2f ,", image_src[i*j+j]);
		}
		//printf("\n");
		
	}
	printf("Max value:::::::::::::::::::::: %.2f \n",max);
	storeImage(image_dest, "image/lena_output.bmp", h, w, "image/lena.bmp");
	system("pause");
}