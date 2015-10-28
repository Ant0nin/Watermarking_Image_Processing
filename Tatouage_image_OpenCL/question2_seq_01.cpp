#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "bmpfuncs.h"

#define ERR_WRONG_INPUT_ARGUMENTS 1

#define MSG_LENGTH 1024

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

	srand(time(NULL));

	// Input
	int imgLength = imageWidth * imageHeight;
	const size_t msgLength = MSG_LENGTH;
	bool message[msgLength];
	bool bit = true;

	// Message generation
	for (int i = 0; i < msgLength; i++) {

		if(i%3==0)
			bit=1;
		else
			bit=0;

		message[i] = bit;
	}

	unsigned int maxShift = imgLength / msgLength;
	unsigned int position = 0;
	unsigned int shift;
	float diff = 1.0;

	printf("Encoding message...\n");
	for (int i = 0; i < msgLength; i++) {
		shift = rand() % maxShift;
		position = maxShift * i + shift;
		bit = message[i];
		if (bit == true)
			image[position] += diff; //!\ Pb valeurs extrêmes
		else
			image[position] -= diff; //!\ Pb valeurs extrêmes
	}

	storeImage(image, imageOutputPath, imageHeight, imageWidth, imageInputPath);

	system("pause");
	return EXIT_SUCCESS;
}