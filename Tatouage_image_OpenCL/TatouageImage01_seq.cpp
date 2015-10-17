#include <time.h>
#include <cstdlib>

#include "bmpfuncs.h"

void main() {

	time_t t = time(NULL);
	srand((unsigned)&t);

	char imgPath[] = "image/lena.bmp";
	int width;
	int height;
	float* image2D = NULL;
	image2D = readImage(imgPath, &height, &width);
	int imgLength = width * height;

	const size_t msgLength = 7869;
	bool message[msgLength] = { 1 };
	bool bit = true;
	for (int i = 0; i < msgLength; i++) {
		bit = !bit;
		message[i] = bit;
	}

	/*unsigned int mask = 0x80000000;
	unsigned int random = (2 ^ msgLength) + (rand() * (int)((2 ^ imgLength) - (2 ^ msgLength)) / RAND_MAX); // Bad

	int positions[msgLength];
	int j = 0;
	bool bit;
	for (int i = 0; i < imgLength; i++) 
	{
		mask = mask >> 1;
		bit = random & mask;
		if (bit == 1) {
			positions[j] = i;
			j++;
			if (j >= msgLength)
				break;
		}
	}*/
	
	unsigned int maxShift = imgLength / msgLength;
	unsigned int position = 0;
	unsigned int shift;
	float diff = 100.0;
	for (int i = 0; i < msgLength; i++) {
		
		shift = 1 + (rand() * (maxShift - 1) / RAND_MAX);
		position += shift;
		bit = message[i];

		if (bit == true)
			image2D[position] += diff; //!\ Pb valeurs extrêmes
		else
			image2D[position] -= diff; //!\ Pb valeurs extrêmes
	}

	storeImage(image2D, "image/output.bmp", height, width, imgPath);
	system("pause");

}