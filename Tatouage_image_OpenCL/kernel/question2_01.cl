__kernel void decryptMessage(
	__global float* cleanImage,
	__global float* cryptedImage,
	__global bool* outputMsg) {

	unsigned int groupId = get_group_id(0);
	unsigned int globalId = get_global_id(0);
	if (cryptedImage[globalId] > cleanImage[globalId])
		outputMsg[groupId] = 1;
	else if (cryptedImage[globalId] < cleanImage[globalId])
		outputMsg[groupId] = 0;

}