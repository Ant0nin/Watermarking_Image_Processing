/*float calcAverage(__local float *intensities, unsigned int N) {

	float sum = 0.0;

	for (int i = 0; i < N; i++)
		sum += intensities[i];

	return sum / N;
}

float calcVariance(__local float *intensities, float average, unsigned int N) {

	unsigned int globalId = get_global_id(0);
	float sum = 0.0;

	// Difficile à paralléliser AU SEIN DE chaque échantillon car sampleSize = 3 x 3 = 9 et c'est PAS une puissance de 2
	for (int i = 0; i < N; i++) {
		sum += (intensities[i] - average) * (intensities[i] - average); // au carré
	}
	return sum / N;
}

float calcEcartType(float variance) {
	return sqrt(variance);
}*/

__kernel void calcEcartTypeForEachSample(
	__global float* imageInput, unsigned int imgWidth, unsigned int imgHeight, 
	unsigned int sampleWidth, unsigned int sampleHeight, unsigned int sampleLength, __local float* intensities,
	__global float *outputEcartType, __global int *outputPositionsX, __global int *outputPositionsY)
{
	int2 pixPos = { get_global_id(0), get_global_id(1) };

	if (pixPos.x != 0 && pixPos.x != (imgWidth - 1) && pixPos.y != 0 && pixPos.y != (imgHeight - 1)) 
	{
		intensities[0] = -1;
		intensities[1] = -1;
		intensities[2] = -1;
		intensities[3] = -1;
		intensities[4] = -1;
		intensities[5] = -1;
		intensities[6] = -1;
		intensities[7] = -1;
		intensities[8] = -1;

		for (int i = 0; i < sampleHeight; i++)
		{
			for (int j = 0; j < sampleWidth; j++)
			{
				int indexIntensities = (i) + sampleWidth*(j);
				int indexImage = (pixPos.x + i - 1) + (pixPos.y + j - 1) * imgWidth;
				intensities[indexIntensities] = imageInput[indexImage];
			}
		}

		float average = 0.0;
		for (int i = 0; i < sampleLength; i++) {
			average += intensities[i];
		}
		average = average / sampleLength;


		float variance = 0.0;
		for (int i = 0; i < sampleLength; i++) {
			variance += (intensities[i] - average) * (intensities[i] - average); // au carré
		}
		variance = variance / sampleLength;


		float ecartType = sqrt(variance);

		/*float average = calcAverage(intensities, sampleLength);
		float variance = calcVariance(intensities, average, sampleLength);
		float ecartType = calcEcartType(variance);*/

		int targetId = (pixPos.y - 1) * (imgWidth-2) + (pixPos.x - 1);

		outputPositionsX[targetId] = pixPos.x;
		outputPositionsY[targetId] = pixPos.y;
		outputEcartType[targetId] = average;

		

	}
}