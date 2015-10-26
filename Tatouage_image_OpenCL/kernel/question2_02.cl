__kernel void decryptMessage(
	__global float* cleanImage,
	__global float* cryptedImage,
	__global bool* outputMsg,
	__local float* cacheImgCleanBlock,
	__local float* cacheImgCryptedBlock,
	const unsigned int SIZE_BLOCK) {

	unsigned int globalId = get_global_id(0);
	unsigned int localId = get_local_id(0);
	unsigned int wkSize = get_local_size(0);
	unsigned int index=0;
	cacheImgCleanBlock[localId]=cleanImage[globalId];
	cacheImgCryptedBlock[localId]=cryptedImage[globalId];
	barrier(CLK_LOCAL_MEM_FENCE);
	if(localId==0){
		for(int i=0;i<wkSize;i++){
			index=(globalId+i)/SIZE_BLOCK;
			if (cacheImgCryptedBlock[i] > cacheImgCleanBlock[i])
				outputMsg[index] = 1;
			else if (cacheImgCryptedBlock[i] < cacheImgCleanBlock[i])
				outputMsg[index] = 0;
		}
	}
}