float calcAverage(__global float *intensities, unsigned int N) {

	unsigned int groupId = get_group_id(0);
	unsigned int globalId = get_global_id(0);
	float sum = 0.0;

	for (int i = 0; i < N; i++)
		sum += intensities[globalId + i];

	return sum / N;
}

float calcVariance(__global float *intensities, float average, unsigned int N) {

	unsigned int globalId = get_global_id();
	float sum = 0.0;

	// Difficile à paralléliser AU SEIN DE chaque échantillon car sampleSize = 3 x 3 = 9 et c'est PAS une puissance de 2
	for (int i = 0; i < N; i++) {
		sum += (intensities[globalId + i] - average) ^ 2;
	}
	return sum / N;
}

float calcEcartType(float variance) {
	return sqrt(variance);
}

__kernel void calcEcartTypeForEachSample(
	__read_only image2d_t imageInput, unsigned int imgWidth, unsigned int imgHeight,
	sampler_t sampler, unsigned int sampleWidth, unsigned int sampleHeight, unsigned int sampleLength,
	__global float *outputEcartType, __global int2 *outputPositions, __local float4* intensities)
{
	int2 pixPos = { get_global_id(0), get_global_id(1) };

	int halfWidth = (int)(sampleWidth / 2);
	int halfHeight = (int)(sampleHeight / 2);
	int2 coords;

	for (int i = -halfWidth; i <= halfWidth; i++) {
		coords.y = pixPos.y + i;
		for (int j = -halfHeight; j <= halfHeight; j++)
		{
			coords.x = pixPos.x + j;
			intensities[j*sampleWidth + i] = read_imagef(imageInput, sampler, coords);
		}
	}

	float average = calcAverage(intensities, sampleLength);
	float variance = calcVariance(intensities, average, sampleLength);
	float ecartType = calcEcartType(variance);

	outputPositions[pixPos.x*imgWidth + pixPos.y] = pixPos;
	outputEcartType[pixPos.x*imgWidth + pixPos.y] = ecartType;
}