// Throw this code !

_kernel

void generateSampleValues(
	__global float *image, 
	unsigned int imgWidth, unsigned int imgHeight, 
	unsigned int sampleWidth, unsigned int sampleHeight
	__global int2 *outputCoords, __global float *outputIntensities) {

	unsigned int sampleSize = get_local_size(0); // = 9

	unsigned int groupId = get_group_id(0);
	unsigned int globalId = get_global_id(0);

	unsigned int offsetX = (imgWidth - (imgWidth % sampleWidth) / sampleWidth) * groupId; // Bad and wrong => CPU !
	unsigned int offsetY = (imgHeight - (imgHeight % sampleHeight) / sampleHeight) * groupId; // Bad and wrong => CPU !

	for (int i = offsetX; i < sampleWidth; i++) {
		for (int j = offsetY; j < sampleHeight; j++) {
			
			int2 coords;
			coords.x = i;
			coords.y = j;
			
			float intensity = image[coords.x * (coords.y + 1)];

			int index = i*j;
			outputCoords[index] = coords;
			outputIntensities[index] = intensity;
		}
	}
}

float calcAverage(__global *intensities, unsigned int N) {

	unsigned int groupId = get_group_id(0);
	unsigned int globalId = get_global_id(0);
	float sum = 0.0;
	float average;
	
	for (int i = 0; i < N; i++)
		sum += intensities[globalId + i];

	average = sum / N;
	return average;
}

float calcVariance(__global float *intensities, float average, unsigned int N) {
	
	unsigned int globalId = get_global_id();
	float sum = 0.0;
	float soustract;

	// Difficile à paralléliser AU SEIN DE chaque échantillon car sampleSize = 3 x 3 = 9 et c'est PAS une puissance de 2
	for (int i = 0; i < N; i++) {
		sum += (intensities[globalId + i] - average)^2;
	}
	return sum / N;
}

float calcEcartType(float variance) {
	return sqrt(variance);
}

int2* getBestPixels(__global int2 *coords, __global float *intensities, __global int2 *bestPixels, unsigned int N /* = 9 */) {
	
	float average = calcAverage(intensities, N);
	float variance = calcVariance(intensities, average, N);
	float ecartType = calcEcartType(variance);

	// TODO : Filtrer les valeurs extrêmes
	// TODO : tri des samples en fct de leurs variances
}

void encodeMessage(__global image_t *image, bool *message, __global *coords) {
	
	// TODO

}