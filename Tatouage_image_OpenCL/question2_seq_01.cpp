#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "bmpfuncs.h"

#define ERR_WRONG_INPUT_ARGUMENTS 1

#define MSG_LENGTH 1024

int main(int argc, char *argv[]) {

	const char *imageOriginalPath;
	const char *imageCryptedPath;

	if (argc == 2 || argc > 3) {
		printf("Need original image path and crypted image path as program arguments, or nothing to use default values.\n");
		system("pause");
		return ERR_WRONG_INPUT_ARGUMENTS;
	}
	else if (argc == 3) {
		imageOriginalPath = argv[1];
		imageCryptedPath = argv[2];
	}
	else {
		imageOriginalPath = "image/lena.bmp";
		imageCryptedPath = "image/output.bmp";
	}

	int imageWidth, imageHeight;
	float *image = NULL;
	image = readImage(imageOriginalPath, &imageWidth, &imageHeight);

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

	printf("Encoding message... ");
	for (int i = 0; i < msgLength; i++) {
		shift = rand() % maxShift;
		position = maxShift * i + shift;
		bit = message[i];
		if (bit == true)
			image[position] += diff; //!\ Pb valeurs extrêmes
		else
			image[position] -= diff; //!\ Pb valeurs extrêmes
	}
	printf("Done.\n");

	storeImage(image, imageCryptedPath, imageHeight, imageWidth, imageOriginalPath);

	system("pause");
	return EXIT_SUCCESS;
}