__kernel void calcEcartTypeForEachSample(
	__global float* imageInput, unsigned int imgWidth, unsigned int imgHeight,
	unsigned int sampleWidth, unsigned int sampleHeight, const unsigned int sampleLength,
	__global float *outputEcartType, __global int *outputPositionsX, __global int *outputPositionsY)
{
	int2 pixPos = { get_global_id(0), get_global_id(1) };

	if (pixPos.x != 0 && pixPos.x != (imgWidth - 1) && pixPos.y != 0 && pixPos.y != (imgHeight - 1))
	{
		float intensities[9];

		int targetId = (pixPos.y - 1) * (imgWidth - 2) + (pixPos.x - 1);
		float average = 0.0;

		for (int i = 0; i < sampleHeight; i++)
		{
			for (int j = 0; j < sampleWidth; j++)
			{
				int indexIntensities = (i)+sampleWidth*(j);
				int indexImage = (pixPos.x + i - 1) + (pixPos.y + j - 1) * imgWidth;
				intensities[indexIntensities] = imageInput[indexImage];

				average += intensities[indexIntensities];
			}
		}

		average = average / sampleLength;

		float variance = 0.0;
		for (int i = 0; i < sampleLength; i++) {
			variance += pow((intensities[i] - average), 2);
		}
		variance = variance / sampleLength;

		float ecartType = sqrt(variance);

		outputPositionsX[targetId] = pixPos.x;
		outputPositionsY[targetId] = pixPos.y;

		uint position1D = pixPos.x + pixPos.y * imgWidth;

		outputEcartType[targetId] = ecartType;
	}
}

__kernel void sortArray(__const unsigned int N, // taille de l'ensemble des écarts types
	__global float* input, // ecart types
	__global int* outputPos) {

	// TODO

}


__kernel void writeMessageOnImage(__global bool *message,
	__global int *positions, __global float *image)
{
	uint globalId = get_global_id(0);

	if (message[globalId] == 1)
		image[positions[globalId]] += 1.0;
	else
		image[positions[globalId]] -= 1.0;
}