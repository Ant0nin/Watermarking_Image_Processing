#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bmpfuncs.h"

#define MSG_LENGTH 1024

int main(int argc, char *argv[]) {

	const char *imageOriginalPath;
	const char *imageCryptedPath;

	if (argc == 2 || argc > 3) {
		printf("Need original image path and crypted image path as program arguments, or nothing to use default values.\n");
		system("pause");
		return EXIT_FAILURE;
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
	int imgLength = imageWidth * imageHeight;

	// Message generation
	const size_t msgLength = MSG_LENGTH;
	bool message[msgLength];
	bool bit = true;
	for (int i = 0; i < msgLength; i++) {
		bit = !bit;
		message[i] = bit;
	}
	float avg = 0.0;
	float ecart = 0.0;
	float *tmpimage2D = image;

	int ecartSize = 3;
	float tmpValue = 0.0;
	int xi, yi;
	unsigned int maxShift = (imageWidth - 2)*(imageHeight - 2) / msgLength;
	unsigned int partie = 1;
	unsigned int posSubImage = 0;
	float maxEcart = 0;
	float diff = 100.0;
	int posMaxEcart = 0;
	float *tmpEcart = NULL;
	int *tmpEcartPos = NULL;
	size_t maxEcartSize = sizeof(float)*(imageWidth - 2)*(imageHeight - 2);
	tmpEcart = (float*)malloc(maxEcartSize);
	size_t maxEcartSize1 = sizeof(int)*((imageWidth - 2)*(imageHeight - 2));
	tmpEcartPos = (int*)malloc(maxEcartSize1);

	printf("Encoding message (long waiting time)...");

	// Calcul de la moyenne
	for (int i = 1; i < imageHeight - 1; i++)
	{
		for (int j = 1; j < imageWidth - 1; j++) {
			avg = 0.0;

			// Calcul la moyenne de matrice 3x3
			for (yi = 0; yi < ecartSize; yi++)
			{
				for (xi = 0; xi < ecartSize; xi++)
				{
					avg += tmpimage2D[((i - 1 + yi)*imageWidth + (j + xi - 1))];
				}
			}
			avg = avg / (ecartSize*ecartSize);
			ecart = 0;
			//calcul ecart-type
			for (yi = 0; yi < ecartSize; yi++)
			{
				for (xi = 0; xi < ecartSize; xi++)
				{
					tmpValue = tmpimage2D[((i - 1 + yi)*imageWidth + (j + xi - 1))];
					ecart += pow((tmpValue - avg), 2);
				}
			}
			ecart = sqrt(ecart / (ecartSize*ecartSize));
			tmpEcart[posSubImage] = ecart;
			tmpEcartPos[posSubImage] = i*imageWidth + j;
			posSubImage++;
		}
	}

	bool isOrdered = false;
	size_t tmpSize = (imageWidth - 2)*(imageHeight - 2);
	float tmpSwap = 0.0;
	int	posTmpSwap=0;
	while (!isOrdered)
	{
		isOrdered = true;
		for (int i = 0; i < tmpSize-1; i++)
		{
			if (tmpEcart[i] < tmpEcart[i + 1])
			{
				tmpSwap = tmpEcart[i];
				tmpEcart[i] = tmpEcart[i + 1];
				tmpEcart[i + 1] = tmpSwap;
				// Position
				posTmpSwap = tmpEcartPos[i];
				tmpEcartPos[i] = tmpEcartPos[i + 1];
				tmpEcartPos[i + 1] = posTmpSwap;
				isOrdered = false;
			}
		}
		tmpSize--;
	}

	for (int i = 0; i < msgLength; i++) {
		if (message[i] == true)
			tmpimage2D[tmpEcartPos[i]] += diff;
		else
			tmpimage2D[tmpEcartPos[i]] -= diff;
	}
	printf("Done.\n");

	storeImage(image, imageCryptedPath, imageHeight, imageWidth, imageOriginalPath);

	//system("pause");
	return EXIT_SUCCESS;
}