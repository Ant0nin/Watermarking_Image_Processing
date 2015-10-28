__kernel void reduction(__const unsigned int N, // number of elements to reduce
	__const int block,
	__global float* input, 
	__global float* output,
	__global float* outputPos) {
	int global_index = get_global_id(0) * block;
	float accumulator = 0;
	int position=0;
	int upper_lim = (get_global_id(0) + 1) * block;
	if (upper_lim > N) 
		upper_lim = N;
	while (global_index < upper_lim) {
		float element = input[global_index];
		if(accumulator < element){
			accumulator=element;
			position=global_index;
		}
		global_index++;
	} 
	output[get_group_id(0)] = accumulator ;
	outputPos[get_group_id(0)] = position ;
}