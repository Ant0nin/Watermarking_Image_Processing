#include <time.h>
#include <stdlib.h>
#include <stdio.h>


#include "bmpfuncs.h"

#define MSG_LENGTH 1024

int main() {

	srand(time(NULL));

	char imgPath[] = "image/lena.bmp";
	int width;
	int height;
	float* image2D = NULL;
	image2D = readImage(imgPath, &height, &width);
	int imgLength = width * height;

	const size_t msgLength = MSG_LENGTH;
	bool message[msgLength]; // 01010101...
	bool bit = true;
	for (int i = 0; i < msgLength; i++) {
		if(i%3==0)
			bit=1;
		else
			bit=0;
		//bit = !bit;
		message[i] = bit;
		printf("%d", bit);
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
	//80 pixeles
	//10 msg
	//cada 8

	unsigned int maxShift = imgLength / msgLength; //attention a l'arrondi
	unsigned int position = 0;
	unsigned int shift;
	float diff = 1.0;
	printf("Positions=\n");
	for (int i = 0; i < msgLength; i++) {
		shift = rand() % maxShift;
		position = maxShift * i + shift;
		bit = message[i];
		if (bit == true)
			image2D[position] += diff; //!\ Pb valeurs extr�mes
		else
			image2D[position] -= diff; //!\ Pb valeurs extr�mes
	}

	storeImage(image2D, "image/output.bmp", height, width, imgPath);

	system("pause");
	return 0;
}