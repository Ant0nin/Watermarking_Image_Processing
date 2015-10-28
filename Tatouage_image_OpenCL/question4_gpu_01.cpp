#include <CL/cl.h>
#include <stdio.h>
#include <cstdlib>
#include "clExtend.h"
#include "opencl_errors.h"
#include "bmpfuncs.h"

#define MSG_LENGTH 1024

const char clFile_encoding[] = "question4_01";
const char kernel_calcEcartType[] = "calcEcartTypeForEachSample";
const char kernel_sortArray[] = "sortArray";
const char kernel_writeMessage[] = "writeMessageOnImage";

int main(int argc, char *argv[]) {

	const char *imageOriginalPath;
	const char *imageCryptedPath;

	if (argc == 2 || argc > 3) {
		printf("Need input image path and output image path as program arguments, or nothing to use default values.\n");
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
	float *image = NULL;
	image = readImage(imageOriginalPath, &imageWidth, &imageHeight);

	cl_int status;
	cl_command_queue cmdQueue;
	cl_context context;
	cl_program program;
	cl_device_id *devices;
	cl_uint numDevices;
	cl_event event;
	cl_kernel kernel;

	cl_ulong time_start, time_end;
	double total_time = 0;

	// OpenCL initializations

	devices = getDevices(&numDevices, &status);
	checkError(status, 50);

	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	checkError(status, 53);

	cmdQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);
	checkError(status, 56);
	clFinish(cmdQueue);

	program = createKernelProgramFromFile(&context, clFile_encoding, &status);
	debugKernelProgramWithLog(program, devices);
	checkError(status, 62);

	status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
	checkError(status, 66);

	// Message generation
	const size_t msgSize = MSG_LENGTH;
	bool message[msgSize];
	bool bit = true;
	for (int i = 0; i < msgSize; i++) {
		if (i % 3 == 0)
			bit = 1;
		else
			bit = 0;
		message[i] = bit;
	}

	// Input
	const int imgLength = (const int)(imageWidth * imageHeight);
	const int datasize = (const int)((imageWidth - 2) * (imageHeight - 2));

	unsigned int sampleWidth = 3;
	unsigned int sampleHeight = 3;
	unsigned int sampleLenght = sampleWidth * sampleHeight;

	float *ecartTypes = NULL;
	ecartTypes = (float*)malloc(sizeof(float) * datasize);

	int *positionsX = NULL;
	positionsX = (int*)malloc(sizeof(int) * datasize);

	int *positionsY = NULL;
	positionsY = (int*)malloc(sizeof(int) * datasize);

	cl_mem bufferImageOriginal = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * imgLength, NULL, &status);
	cl_mem bufferEcartType = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * datasize, NULL, &status);
	cl_mem bufferPositionX = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * datasize, NULL, &status);
	cl_mem bufferPositionY = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * datasize, NULL, &status);
	status = clEnqueueWriteBuffer(cmdQueue, bufferImageOriginal, CL_FALSE, 0, sizeof(float) * imgLength, image, 0, NULL, NULL);

	printf("Encoding message...");

	// First kernel : Calcul des ecarts types

	kernel = clCreateKernel(program, kernel_calcEcartType, &status);
	checkError(status, 66);

	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferImageOriginal);
	status = clSetKernelArg(kernel, 1, sizeof(cl_int), &imageWidth);
	status = clSetKernelArg(kernel, 2, sizeof(cl_int), &imageHeight);

	status = clSetKernelArg(kernel, 3, sizeof(cl_uint), &sampleWidth);
	status = clSetKernelArg(kernel, 4, sizeof(cl_uint), &sampleHeight);
	status = clSetKernelArg(kernel, 5, sizeof(cl_uint), &sampleLenght);

	status = clSetKernelArg(kernel, 6, sizeof(cl_mem), &bufferEcartType);
	status = clSetKernelArg(kernel, 7, sizeof(cl_mem), &bufferPositionX);
	status = clSetKernelArg(kernel, 8, sizeof(cl_mem), &bufferPositionY);

	size_t globalWorkSize2D[2];
	globalWorkSize2D[0] = imageWidth;
	globalWorkSize2D[1] = imageHeight;

	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 2, NULL, globalWorkSize2D, NULL, 0, NULL, &event);
	clWaitForEvents(1, &event);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time += (time_end - time_start);

	status = clEnqueueReadBuffer(cmdQueue, bufferEcartType, CL_TRUE, 0, sizeof(float) * datasize, ecartTypes, 0, NULL, NULL);
	status = clEnqueueReadBuffer(cmdQueue, bufferPositionX, CL_TRUE, 0, sizeof(int) * datasize, positionsX, 0, NULL, NULL);
	status = clEnqueueReadBuffer(cmdQueue, bufferPositionY, CL_TRUE, 0, sizeof(int) * datasize, positionsY, 0, NULL, NULL);

	// Second kernel : Tri des écarts types de la totalité de l'image dans l'ordre croissant

	float *maxEcartTypes = NULL;
	maxEcartTypes = (float*)malloc(sizeof(float) * msgSize);

	float *sortedArray = NULL;
	sortedArray = (float*)malloc(sizeof(float) * msgSize);

	kernel = clCreateKernel(program, kernel_sortArray, &status);

	cl_mem bufferPosition = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * msgSize, NULL, &status);
	int blockSize = datasize / msgSize;

	status = clSetKernelArg(kernel, 0, sizeof(cl_int), &datasize);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferEcartType);
	status = clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufferPosition);

	size_t globalWorkSize1D[1];
	globalWorkSize1D[0] = msgSize;
	const size_t localWorkSize = 1;

	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize1D, &localWorkSize, 0, NULL, &event);
	clWaitForEvents(1, &event);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time += (time_end - time_start);

	int *pos = NULL;
	pos = (int*)malloc(sizeof(int) * msgSize);
	status = clEnqueueReadBuffer(cmdQueue, bufferPosition, CL_TRUE, 0, sizeof(int) * msgSize, pos, 0, NULL, NULL);

	// Third kernel : Encodage du message

	kernel = clCreateKernel(program, kernel_writeMessage, &status);

	cl_mem bufferMessage = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(bool) * msgSize, NULL, &status);

	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferMessage);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferPosition);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferImageOriginal);

	globalWorkSize1D[0] = msgSize;

	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize1D, NULL, 0, NULL, NULL);

	status = clEnqueueReadBuffer(cmdQueue, bufferImageOriginal, CL_TRUE, 0, sizeof(float) * imgLength, image, 0, NULL, NULL);

	printf("Done.\n");

	storeImage(image, imageCryptedPath, imageWidth, imageHeight, imageOriginalPath);

	// Free ressources
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(bufferEcartType);
	clReleaseMemObject(bufferImageOriginal);
	clReleaseMemObject(bufferMessage);
	clReleaseMemObject(bufferPosition);
	clReleaseMemObject(bufferPositionX);
	clReleaseMemObject(bufferPositionY);
	clReleaseContext(context);
	free(image);
	free(devices);

	system("pause");
	return EXIT_SUCCESS;
}