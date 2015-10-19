_kernel void decryptMessage(
	__read_only image2d_t cleanImage,
	__read_only image2d_t cryptedImage,
	__global bool* outputMsg) {

	unsigned int groupId = get_group_id(0);
	unsigned int globalId = get_global_id(0);

	if (cryptedImage[globalId] > cleanImage[globalId])
		outputMsg[groupId] = true;
	else if (cryptedImage[globalId] < cleanImage[globalId])
		outputMsg[groupId] = false;

}