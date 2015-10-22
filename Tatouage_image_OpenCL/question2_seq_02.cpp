#include <stdio.h>
#include <cstdlib>
#include "bmpfuncs.h"

//#define MSG_LENGTH 1024

int main() {

	char pathImage2D_clean[] = "image/lena.bmp";
	char pathImage2D_crypted[] = "image/output.bmp";

	int width;
	int height;

	float* image2D_clean;
	float* image2D_crypted;

	image2D_clean = readImage(pathImage2D_clean, &width, &height);
	image2D_crypted = readImage(pathImage2D_crypted, &width, &height);

	int imgLength = width * height;

	int count = 0;
	for (int i = 0; i < imgLength; i++) {
		if (image2D_crypted[i] != image2D_clean[i])
			count++;
	}

	const size_t msgLength = count;
	bool *message;
	message = (bool*)malloc(msgLength);
	printf("Differents=%d,\n",count );
	int j = 0;
	for (int i = 0; i < imgLength; i++) {
		if (image2D_crypted[i] < image2D_clean[i]) {
			message[j] = 0;
			j++;
		}
		else if (image2D_crypted[i] > image2D_clean[i]) {
			message[j] = 1;
			j++;
		}

	}
	printf("Dernier=%d\n",j );

	for (int i = 0; i < msgLength; i++) {
		printf("%d", message[i]);
	}
	printf("\n");

	system("pause");

}