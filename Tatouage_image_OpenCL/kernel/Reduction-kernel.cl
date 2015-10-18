__kernel void max_kernel(unsigned int N, // number of elements to reduce
	__global float* input, __global float* output,
	__local float* sdata) {
	// Get index into local data array and global array
	unsigned int localId = get_local_id(0), globalId = get_global_id(0);
	unsigned int groupId = get_group_id(0), wgSize = get_local_size(0);
	//output[0] = localId;
	//output[1] = groupId;
	//output[2] = globalId;
	//output[3] = wgSize;
	//output[4] = N;
	//output[globalId] = localId;
	// Read in data if within bounds
	sdata[localId] = (globalId<N) ? input[globalId] : 0;
	// Synchronize since all data needs to be in local memory and visible to all work items
	barrier(CLK_LOCAL_MEM_FENCE);
	// Each work item adds two elements in parallel. As stride increases, work items remain idle.
	for (int offset = wgSize; offset > 0; offset >>= 1) {
		if (localId < offset && localId + offset < wgSize)
			//sdata[localId] += sdata[localId + offset];
			if (sdata[localId]<sdata[localId + offset])
				sdata[localId] = sdata[localId + offset];
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	// Only one work item needs to write out result of the work group’s reduction
	barrier(CLK_LOCAL_MEM_FENCE);
	if (localId == 0) output[groupId] = sdata[0];
}