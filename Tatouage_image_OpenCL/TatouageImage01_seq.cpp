#include <time.h>
#include <cstdlib>

#include "bmpfuncs.h"

void main() {

	time_t t = time(NULL);
	srand((unsigned)&t);

	char imgPath[] = "image/grey.bmp";
	int width;
	int height;
	float* image2D = NULL;
	image2D = readImage(imgPath, &height, &width);
	int imgLength = width * height;

	const size_t msgLength = 32;
	bool* message;
	message = (bool*)malloc(msgLength);
	*message = 0b10101010111100110;

	bool* random;
	random = (bool*)malloc(imgLength);
	*random = (2 ^ msgLength) + (rand() * (int)((2 ^ imgLength) - (2 ^ msgLength)) / RAND_MAX);
	
	int positions[msgLength];
	int j = 0;
	for (int i = 0; i < imgLength; i++) {
		if (random[i] == true) {
			positions[j] = i;
			j++;
			if (j >= msgLength)
				break;
		}
	}
	
	for (int i = 0; i < msgLength; i++) {

		int pos = positions[i];
		bool bitMsg = message[i];

		if (bitMsg == true)
			image2D[pos] += 1.0;
		else
			image2D[pos] -= 1.0;

	}

	storeImage(image2D, "image/output.bmp", height, width, imgPath);
	system("pause");

}