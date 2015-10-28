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
	float ecartVector[4];
	float tmpValue = 0.0;
	int xi, yi;
	unsigned int maxShift = (imageWidth - 2)*(imageHeight - 2) / msgLength;
	unsigned int partie = 1;
	unsigned int posSubImage = 0;
	float maxEcart = 0;
	float diff = 100.0;
	int posMaxEcart = 0;
	
	// Calcul de moyenne
	printf("Encoding message... ");
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
			// Calcul ecart-type
			for (yi = 0; yi < ecartSize; yi++)
			{
				for (xi = 0; xi < ecartSize; xi++)
				{
					tmpValue = tmpimage2D[((i - 1 + yi)*imageWidth + (j + xi - 1))];
					ecart += pow((tmpValue - avg), 2);
				}
			}
			ecart = sqrt(ecart / (ecartSize*ecartSize));
			if (maxEcart<ecart) {
				maxEcart = ecart;
				posMaxEcart = i*imageWidth + j;
			}

			if (posSubImage == maxShift*partie - 1) {
				// Changer le pixel pour le max ecart
				bit = message[partie - 1];
				if (bit == true)
					tmpimage2D[posMaxEcart] += diff;
				else
					tmpimage2D[posMaxEcart] -= diff;
				maxEcart = 0;
				posMaxEcart = i*imageWidth + j;
				partie++;
			}
			posSubImage++;
		}
	}
	printf("Done.\n");

	storeImage(image, imageCryptedPath, imageHeight, imageWidth, imageOriginalPath);

	//system("pause");
	return EXIT_SUCCESS;
}