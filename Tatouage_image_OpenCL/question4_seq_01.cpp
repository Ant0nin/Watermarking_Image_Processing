#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


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
		bit = !bit;
		message[i] = bit;
	}

	float avg = 0.0;
	float ecart = 0.0;

	//Generation sim
	/*float tmpimage2D[16]={245,142,152,98,58,47,195,29,24,241,33,109,145,88,39,221};
	width=4;
	height=4;
	imgLength = width * height;*/
	float *tmpimage2D = image2D;


	int ecartSize = 3;
	float ecartVector[4];
	float tmpValue = 0.0;
	int xi, yi;
	unsigned int maxShift = (width - 2)*(height - 2) / msgLength;
	unsigned int partie = 1;
	unsigned int posSubImage = 0;
	float maxEcart = 0;
	float diff = 100.0;
	int posMaxEcart = 0;
	//tmp vector Ecartype
	float *tmpEcart = NULL;
	int *tmpEcartPos = NULL;
	size_t maxEcartSize = sizeof(float)*(width - 2)*(height - 2);
	tmpEcart = (float*)malloc(maxEcartSize);
	size_t maxEcartSize1 = sizeof(int)*((width - 2)*(height - 2));
	tmpEcartPos = (int*)malloc(maxEcartSize1);
	//calcul de moyene
	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++) {
			avg = 0.0;
			//calcul la moyenne de matriz 3x3
			for (yi = 0; yi < ecartSize; yi++)
			{
				for (xi = 0; xi < ecartSize; xi++)
				{
					avg += tmpimage2D[((i - 1 + yi)*width + (j + xi - 1))];
				}
			}
			avg = avg / (ecartSize*ecartSize);
			ecart = 0;
			//calcul ecart-type
			for (yi = 0; yi < ecartSize; yi++)
			{
				for (xi = 0; xi < ecartSize; xi++)
				{
					tmpValue = tmpimage2D[((i - 1 + yi)*width + (j + xi - 1))];
					ecart += pow((tmpValue - avg), 2);
				}
			}
			ecart = sqrt(ecart / (ecartSize*ecartSize));
			tmpEcart[posSubImage] = ecart;
			tmpEcartPos[posSubImage] = i*width + j;
			posSubImage++;
		}
		//printf("\n");
		/* code */
	}

	/*for (int i = 0; i < (width - 2)*(height - 2); i++){
		printf("%.2f \n", tmpEcartPos[i]);
	}*/
	//Trier le vector
	bool isOrdered = false;
	size_t tmpSize = (width - 2)*(height - 2);
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
				//pos
				posTmpSwap = tmpEcartPos[i];
				tmpEcartPos[i] = tmpEcartPos[i + 1];
				tmpEcartPos[i + 1] = posTmpSwap;
				isOrdered = false;
			}
		}
		tmpSize--;
	}

	for (int i = 0; i < 10; i++){
		printf("%d -> %.2f \n", tmpEcartPos[i], tmpEcart[i]);
	}

	for (int i = 0; i < msgLength; i++) {
		if (message[i] == true)
			tmpimage2D[tmpEcartPos[i]] += diff; //!\ Pb valeurs extrêmes
		else
			tmpimage2D[tmpEcartPos[i]] -= diff; //!\ Pb valeurs extrêmes
	}

	/*for (int i = 0; i < height; i++)
	{
	for (int j = 0; j < width; j++)
	{
	printf("%.2f ", tmpimage2D[i*width+j]);
	}
	printf("\n");
	}*/


	storeImage(image2D, "image/output.bmp", height, width, imgPath);

	system("pause");
	return 0;
}