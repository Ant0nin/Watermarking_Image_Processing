#include <stdio.h>
#include <cstdlib>
#include "bmpfuncs.h"

#define ERR_WRONG_INPUT_ARGUMENTS 1

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

	// Input

	int imageWidth, imageHeight;
	float *imageOriginal = NULL;
	imageOriginal = readImage(imageOriginalPath, &imageWidth, &imageHeight);
	float *imageCrypted = NULL;
	imageCrypted = readImage(imageCryptedPath, &imageWidth, &imageHeight);

	int imgLength = imageWidth * imageHeight;
	int count = 0;
	for (int i = 0; i < imgLength; i++) {
		if (imageCrypted[i] != imageOriginal[i])
			count++;
	}

	const size_t msgLength = count;
	bool *message;
	message = (bool*)malloc(msgLength);
	int j = 0;

	for (int i = 0; i < imgLength; i++) {
		if (imageCrypted[i] < imageOriginal[i]) {
			message[j] = 0;
			j++;
		}
		else if (imageCrypted[i] > imageOriginal[i]) {
			message[j] = 1;
			j++;
		}
	}

	// Display
	printf("\n");
	printf("-------------------------------------------\n");
	printf("Binary message length: %d bits\n", count);
	printf("Binary message content:\n");
	for (int i = 0; i < msgLength; i++) {
		printf("%d", message[i]);
	}
	printf("\n");
	printf("-------------------------------------------\n");
	printf("\n");

	system("pause");
	return EXIT_SUCCESS;
}