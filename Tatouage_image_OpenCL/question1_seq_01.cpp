#include <stdio.h>
#include <stdlib.h>
#include "bmpfuncs.h"

#define ERR_WRONG_INPUT_ARGUMENTS 1

int main(int argc, char *argv[]) {

	const char *imageInputPath;
	const char *imageOutputPath;

	if (argc == 2 || argc > 3) {
		printf("Need input image path and output image path as program arguments, or nothing to use default values.\n");
		system("pause");
		return ERR_WRONG_INPUT_ARGUMENTS;
	}
	else if (argc == 3) {
		imageInputPath = argv[1];
		imageOutputPath = argv[2];
	}
	else {
		imageInputPath = "image/lena.bmp";
		imageOutputPath = "image/output.bmp";
	}

	int imageWidth, imageHeight;
	float *image = NULL;
	image = readImage(imageInputPath, &imageWidth, &imageHeight);

	const int elements = imageHeight*imageWidth;
	size_t imgsize = sizeof(int)*elements;
	float *image_dest;
	image_dest=(float*)malloc(imgsize);
	
	float max=0;
	unsigned int pos = 0;
	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			int index = i*imageWidth + j;
			float current_px = image[index];

			if (max < current_px) {
				max = current_px;
				pos = index;
			}

			image_dest[index] = image[index]+10;
		}		
	}
	unsigned int posX = pos % imageWidth;
	unsigned int posY = (pos - posX) / imageWidth;

	// Display
	printf("\n");
	printf("-------------------------------------------\n");
	printf("Max intensity pixel: %.1f \n", max);
	printf("Pixel position: X=%d, Y=%d \n", posX, posY);
	printf("\n");
	printf("Quantity of image pixels: %d \n", (int)elements);
	printf("-------------------------------------------\n");
	printf("\n");

	system("pause");
	return EXIT_SUCCESS;
}