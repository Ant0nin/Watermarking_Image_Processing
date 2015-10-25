#include <CL\cl.h>
#include "opencl_errors.h"
#include "clExtend.h"
#include "bmpfuncs.h"

#define ERR_WRONG_INPUT_ARGUMENTS 1

int main(int argc, char *argv[]) {
	
	if (argc < 3)
		return ERR_WRONG_INPUT_ARGUMENTS;
	
	char *kernelName = argv[0];
	char *imageInputPath = argv[1];
	char *imageOutputPath = argv[2];
	
	int imageWidth, imageHeight;
	float *image = NULL;
	image = readImage(imageInputPath, &imageHeight, &imageWidth);

	cl_int status;
	cl_command_queue cmdQueue;
	cl_context context;
	cl_program kernel;
	cl_device_id devices;

	status = autoCreateCmdQueueAndContext(&cmdQueue, &context, &devices);
	kernel = createKernelProgramFromFile(&context, kernelName, &status);
	debugKernelWithLog(kernel, &devices);

	// ...

	return CL_SUCCESS;
}