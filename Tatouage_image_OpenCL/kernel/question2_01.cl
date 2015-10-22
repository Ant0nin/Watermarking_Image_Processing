__kernel void decryptMessage(
	__global float* cleanImage,
	__global float* cryptedImage,
	__global bool* outputMsg,
	const unsigned int SIZE_BLOCK) {

	unsigned int globalId = get_global_id(0);
	unsigned int index=globalId/SIZE_BLOCK;

	if (cryptedImage[globalId] > cleanImage[globalId])
		outputMsg[index] = 1;
	else if (cryptedImage[globalId] < cleanImage[globalId])
		outputMsg[index] = 0;

}