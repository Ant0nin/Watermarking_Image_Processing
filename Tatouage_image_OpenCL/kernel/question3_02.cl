__kernel void calcEcartTypeForEachSample(
	__read_only image2d_t image, const uint imgWidth, const uint imgHeight,
	const uint sampleWidth, const uint sampleHeight, const uint sampleLength,
	sampler_t sampler, __global float *outputEcartType)
{
	int2 pixPos = { get_global_id(0), get_global_id(1) };

	float intensities[9]; // pb 9 => alloc

	float average = 0.0;
	for (int i = 0; i < sampleHeight; i++)
	{
		for (int j = 0; j < sampleWidth; j++)
		{
			int indexIntensities = (i)+sampleWidth*(j);
			int2 coords = { (pixPos.x + i - 1), (pixPos.x + j - 1) };
			float4 color = read_imagef(image, sampler, coords);

			// ISSUE : INCORRECT INTENSITY VALUE :
			intensities[indexIntensities] = color.x; // composante rouge = intensité
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

	uint position1D = pixPos.x + pixPos.y * imgWidth;
	outputEcartType[position1D] = intensities[0];
}

__kernel void maxEcartType(__const unsigned int N, // taille de l'ensemble des écarts types
	__const int block, // taille de la tranche
	__global float* ecartsType, // ecart types
	__global uint* outputPos) {

	int global_index = get_global_id(0) * block;
	float accumulator = 0;
	int position = 0;
	int upper_lim = (get_global_id(0) + 1) * block;

	if (upper_lim > N)
		upper_lim = N;

	while (global_index < upper_lim) {
		float element = ecartsType[global_index];
		if (accumulator < element) {
			accumulator = element;

			position = global_index + (512 - 1) + 2 * (int)(global_index / 512); // +3 = triche
		}
		global_index++;
	}

	int groupId = get_group_id(0);
	outputPos[groupId] = position;
}


__kernel void writeMessageOnImage(__global bool *message,
	__global uint *positions, __global float *image)
{
	uint globalId = get_global_id(0);

	if (message[globalId] == 1)
		image[positions[globalId]] += 100.0; // TODO : change to 1.0
	else
		image[positions[globalId]] -= 100.0; // TODO : change to 1.0
}