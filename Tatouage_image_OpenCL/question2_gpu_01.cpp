#include <CL/cl.h>
#include <stdio.h>
#include <cstdlib>
#include "clExtend.h"
#include "opencl_errors.h"
#include "bmpfuncs.h"

#define MSG_LENGTH 1024

const char clFile_decoding[] = "question2_01";
const char kernel_decoding[] = "decryptMessage";

int main(int argc, char *argv[]) {

	const char *imageOriginalPath;
	const char *imageCryptedPath;

	if (argc == 2 || argc > 3) {
		printf("Need original image path and crypted image path as program arguments, or nothing to use default values.\n");
		system("pause");
		return EXIT_FAILURE;
	}
	else if (argc == 3) {
		imageOriginalPath = argv[1];
		imageCryptedPath = argv[2];
	}
	else {
		imageOriginalPath = "image/lena.bmp";
		imageCryptedPath = "image/output.bmp";
	}

	int imageWidth, imageHeight;
	float *imageOriginal = NULL;
	imageOriginal = readImage(imageOriginalPath, &imageWidth, &imageHeight);
	float *imageCrypted = NULL;
	imageCrypted = readImage(imageCryptedPath, &imageWidth, &imageHeight);
	int imgLength = imageWidth * imageHeight;

	cl_int status;
	cl_command_queue cmdQueue;
	cl_context context;
	cl_program program;
	cl_device_id *devices;
	cl_uint numDevices;
	cl_event event;
	cl_kernel kernel;

	cl_ulong time_start, time_end;
	double total_time;

	// OpenCL initializations

	devices = getDevices(&numDevices, &status);
	checkError(status, 50);

	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	checkError(status, 53);

	cmdQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);
	checkError(status, 56);
	clFinish(cmdQueue);

	program = createKernelProgramFromFile(&context, clFile_decoding, &status);
	checkError(status, 60);

	status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
	checkError(status, 63);

	kernel = clCreateKernel(program, kernel_decoding, &status);
	checkError(status, 66);

	// Input
	size_t imageSize = sizeof(float) * imgLength;
	cl_mem bufferImageOriginal = clCreateBuffer(context, CL_MEM_READ_ONLY, imageSize, NULL, &status);
	cl_mem bufferImageCrypted = clCreateBuffer(context, CL_MEM_READ_ONLY, imageSize, NULL, &status);
	size_t messageSize = MSG_LENGTH;
	cl_mem bufferOutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, messageSize, NULL, &status);
	status = clEnqueueWriteBuffer(cmdQueue, bufferImageOriginal, CL_FALSE, 0, imageSize, imageOriginal, 0, NULL, NULL);
	status = clEnqueueWriteBuffer(cmdQueue, bufferImageCrypted, CL_FALSE, 0, imageSize, imageCrypted, 0, NULL, NULL);

	// Mapping
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferImageOriginal);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferImageCrypted);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferOutput);
	const unsigned int blockSize = imgLength / messageSize;
	status = clSetKernelArg(kernel, 3, sizeof(cl_uint), &blockSize);

	// Worksize
	size_t workgroup_size;
    status = clGetKernelWorkGroupInfo(kernel, devices[0], CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t), &workgroup_size, NULL);
	size_t globalWorkSize[1];
	globalWorkSize[0] = imgLength;
	size_t localWorkSize = 1024;
	if(localWorkSize>workgroup_size)
		localWorkSize=workgroup_size;

	// Kernel execution
	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize, &localWorkSize, 0, NULL, &event);
	clWaitForEvents(1, &event);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time = (double)(time_end - time_start);
	
	size_t countWG = MSG_LENGTH;
	bool* outputMsg = NULL;
	outputMsg = (bool*)malloc(sizeof(bool) * countWG);
	clEnqueueReadBuffer(cmdQueue, bufferOutput, CL_TRUE, 0, countWG, outputMsg, 0, NULL, NULL); // values

	// Display
	printf("\n");
	printf("-------------------------------------------\n");
	printf("Binary message length: %d bits\n", MSG_LENGTH);
	printf("Binary message content:\n");
	for (int i = 0; i < MSG_LENGTH; i++) {
		printf("%d", outputMsg[i]);
	}
	printf("\n\n");
	printf("Quantity of image pixels: %d \n", (int)imgLength);
	printf("Local worksize: %d \n", (int)localWorkSize);
	printf("Kernel(s) execution time : %0.3f ms \n", (total_time / 1000000.0));
	printf("-------------------------------------------\n");
	printf("\n");

	// Free ressources
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(bufferImageOriginal);
	clReleaseMemObject(bufferImageCrypted);
	clReleaseMemObject(bufferOutput);
	clReleaseContext(context);
	free(imageOriginal);
	free(imageCrypted);
	free(outputMsg);
	free(devices);

	//system("pause");
	return EXIT_SUCCESS;
}