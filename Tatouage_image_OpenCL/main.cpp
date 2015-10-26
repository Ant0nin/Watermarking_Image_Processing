#include <CL\cl.h>
#include "opencl_errors.h"
#include "clExtend.h"
#include "bmpfuncs.h"

#define ERR_WRONG_INPUT_ARGUMENTS 1

int main(int argc, char *argv[]) {
	
	if (argc < 3)
		return ERR_WRONG_INPUT_ARGUMENTS;
	
	char *kernelName = argv[0];
	const char *imageInputPath = argv[1];
	const char *imageOutputPath = argv[2];
	
	int imageWidth, imageHeight;
	float *image = NULL;
	image = readImage(imageInputPath, &imageWidth, &imageHeight);

	cl_int status;
	cl_command_queue cmdQueue;
	cl_context context;
	cl_program program;
	cl_device_id devices;
	cl_event event;
	cl_kernel kernel;

	status = autoCreateCmdQueueAndContext(&cmdQueue, &context, &devices);
	program = createKernelProgramFromFile(&context, kernelName, &status);
	debugKernelWithLog(program, &devices);
	
	// ...



	storeImage(image, imageOutputPath, imageHeight, imageWidth, imageInputPath);

	return CL_SUCCESS;
}