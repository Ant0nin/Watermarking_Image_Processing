#include <stdio.h>
#include <cstdlib>
#include "bmpfuncs.h"

#define MSG_LENGTH 8

void main() {

	char pathImage2D_clean[] = "image/lena.bmp";
	char pathImage2D_crypted[] = "image/output.bmp";

	int width;
	int height;

	float* image2D_clean;
	float* image2D_crypted;

	image2D_clean = readImage(pathImage2D_clean, &width, &height);
	image2D_crypted = readImage(pathImage2D_crypted, &width, &height);

	int imgLength = width * height;
	const int msgLength = MSG_LENGTH;
	bool message[msgLength];

	int j = 0;
	for (int i = 0; i < imgLength; i++) {
		if (image2D_crypted[i] < image2D_clean[i]) {
			message[j] = false;
			j++;
		}
		else if (image2D_crypted[i] > image2D_clean[i]) {
			message[j] = true;
			j++;
		}
	}

	for (int i = 0; i < msgLength; i++) {
		printf("%d", message[i]);
	}
	printf("\n");

	system("pause");

}