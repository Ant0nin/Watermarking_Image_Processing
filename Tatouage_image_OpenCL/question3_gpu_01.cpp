#include <CL\opencl.h>
#include <stdio.h>
#include <cstdlib>
#include "bmpfuncs.h"

#define MAX_SOURCE_SIZE (0x100000)
#define MSG_LENGTH 1024

void debugKernel(cl_program program, cl_device_id *devices) {
	// size of the log
	size_t log_size;
	clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	// Allocate memory for the log
	char *log = (char *)malloc(log_size);
	// Get the log
	clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
	// Print the log
	printf("%s\n", log);
	system("pause");
}

char* readKernelSource(const char* kernelSourcePath, size_t *source_size) {
	FILE    *fp = NULL;
	size_t  sourceLength;
	char    *sourceString;
	fp = fopen(kernelSourcePath, "r");
	if (fp == 0)
	{
		printf("failed to open file");
		return NULL;
	}
	// get the length of the source code
	fseek(fp, 0, SEEK_END);
	sourceLength = ftell(fp);
	rewind(fp);
	// allocate a buffer for the source code string and read it in
	sourceString = (char *)malloc(sourceLength + 1);
	if (fread(sourceString, 1, sourceLength, fp) != sourceLength)
	{
		printf("\n\t Error : Fail to read file ");
		return 0;
	}
	sourceString[sourceLength + 1] = '\0';
	fclose(fp);
	source_size = &sourceLength;
	return sourceString;
}

int main() {

	char pathImage2D_clean[] = "image/lena.bmp";
	char pathImage2D_crypted[] = "image/output.bmp";

	const size_t msgSize = MSG_LENGTH;
	bool message[msgSize]; // 01010101...
	bool bit = true;
	for (int i = 0; i < msgSize; i++) {
		if (i % 3 == 0)
			bit = 1;
		else
			bit = 0;
		//bit = !bit;
		message[i] = bit;
		printf("%d", bit);
	}
	printf("\n");

	system("pause");

	int width;
	int height;

	float* bmpImage_clean;
	float* bmpImage_crypted;

	bmpImage_clean = readImage(pathImage2D_clean, &width, &height);
	//bmpImage_crypted = readImage(pathImage2D_crypted, &width, &height);
	const int imgLength = (const int)(width * height);
	const int datasize = (const int)((width-2) * (height-2)); // Donner cette valeur pour la globalWorkSize

	unsigned int sampleWidth = 3;
	unsigned int sampleHeight = 3;
	unsigned int sampleLenght = sampleWidth * sampleHeight;

	float *ecartTypes = NULL;
	ecartTypes = (float*)malloc(sizeof(float) * datasize);

	int *positionsX = NULL;
	positionsX = (int*)malloc(sizeof(int) * datasize);

	int *positionsY = NULL;
	positionsY = (int*)malloc(sizeof(int) * datasize);

	// Use this to check the output of each API call
	cl_int status;

	/////////////step1////////////////////////////////////
	cl_uint numPlatforms = 0;
	cl_platform_id *platforms = NULL;
	// Use clGetPlatformIDs() to retrieve the number of platforms
	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	// Allocate enough space for each platform
	platforms = (cl_platform_id*)malloc(numPlatforms *sizeof(cl_platform_id));
	// Fill in platforms with clGetPlatformIDs()
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);

	/////////////////step2///////////////////
	cl_uint numDevices = 0;
	cl_device_id *devices = NULL;
	// Use clGetDeviceIDs() to retrieve the number of devices present
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
	// Allocate enough space for each device
	devices = (cl_device_id*)malloc(numDevices *sizeof(cl_device_id));
	// Fill in devices with clGetDeviceIDs()
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);

	////////////////////step3//////////////////////
	cl_context context = NULL;
	// Create a context using clCreateContext() and associate it with the devices
	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);

	////////////////step4//////////////////
	cl_command_queue cmdQueue;
	// Create a command queue using
	// clCreateCommandQueue(), and associate it with
	// the device you want to execute on
	cmdQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);

	/////////////////step5///////////////////////
	/*cl_image_format format;
	format.image_channel_order = CL_R;
	format.image_channel_data_type = CL_FLOAT;*/

	//cl_mem image2d_crypted;
	//cl_mem buffer_output;

	cl_mem image2d_clean = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * imgLength, NULL, &status);
	//image2d_crypted = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * datasize, NULL, &status);

	cl_mem output_ecartTypes = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * datasize, NULL, &status);
	cl_mem output_positionsX = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * datasize, NULL, &status); // pas nécessaire car matrice
	cl_mem output_positionsY = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * datasize, NULL, &status); // -------------------------'

	//////////////step6////////////////////////
	// Use clEnqueueWriteBuffer() to write input array A
	// to the device buffer bufferA
	/*size_t origin[3] = { 0,0,0 };
	size_t region[3] = { width, height, 1 };*/

	status = clEnqueueWriteBuffer(cmdQueue, image2d_clean, CL_FALSE, 0, sizeof(float) * imgLength, bmpImage_clean, 0, NULL, NULL);

	/////////////load file///////////////////////
	/*size_t programSize=0;
	const char * filename = "MultipliMatriz-kernel.cpp";
	char* sProgramSource = readKernelSource(filename, &programSize);
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&sProgramSource, &programSize, &status);*/

	FILE *fp;
	char *source_str;
	size_t source_size;
	fp = fopen("kernel/question3_01.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	//printf("%s", (const char **)&source_str);

	cl_program program = clCreateProgramWithSource(context, 1,
		(const char **)&source_str, (const size_t *)&source_size, &status);

	//////////////////////////////////////////////

	///////////////step7/////////////////////////////
	// Create a program using
	// clCreateProgramWithSource()

	/*cl_program program = clCreateProgramWithSource(context,
	sizeof(programSource) / sizeof(*programSource), programSource, NULL, &status);*/

	// Build (compile) the program for the devices with
	// clBuildProgram()
	status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
	// build failed
	if (status != CL_SUCCESS) {
		debugKernel(program, devices);
		return 1;
	}
	//////////////step8///////////////////////////
	// Use clCreateKernel() to create a kernel from the
	// vector addition function (named "vecadd")
	cl_kernel kernel = clCreateKernel(program, "calcEcartTypeForEachSample", &status);

	//////////////step9//////////////////////
	// Associate the input and output buffers with the
	// kernel using clSetKernelArg()
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &image2d_clean);
	status = clSetKernelArg(kernel, 1, sizeof(cl_int), &width);
	status = clSetKernelArg(kernel, 2, sizeof(cl_int), &height);

	status = clSetKernelArg(kernel, 3, sizeof(cl_uint), &sampleWidth);
	status = clSetKernelArg(kernel, 4, sizeof(cl_uint), &sampleHeight);
	status = clSetKernelArg(kernel, 5, sizeof(cl_uint), &sampleLenght);

	status = clSetKernelArg(kernel, 6, sizeof(cl_mem), &output_ecartTypes);
	status = clSetKernelArg(kernel, 7, sizeof(cl_mem), &output_positionsX);
	status = clSetKernelArg(kernel, 8, sizeof(cl_mem), &output_positionsY);

	///////////////step10//////////////////////
	// Define an index space (global work size) of work
	// items for execution. A workgroup size (local worksize) is not required, but can be used.
	size_t globalWorkSize[2];
	// There are 'elements' work-items
	globalWorkSize[0] = width;
	globalWorkSize[1] = height;

	//size_t localWorkSize = 16;

	/*ecartTypes[0] = 123.0;
	positionsX[0] = 456;
	positionsY[0] = 456;*/

	clFinish(cmdQueue);

	cl_event event;

	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, &event);
	
	clWaitForEvents(1, &event);

	cl_ulong time_start, time_end;
	double total_time;

	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time = time_end - time_start;

	printf("\nTemps d'execution = %0.3f ms\n", (total_time / 1000000.0));

	system("pause");

	status = clEnqueueReadBuffer(cmdQueue, output_ecartTypes, CL_TRUE, 0, sizeof(float) * datasize, ecartTypes, 0, NULL, NULL);
	status = clEnqueueReadBuffer(cmdQueue, output_positionsX, CL_TRUE, 0, sizeof(int) * datasize, positionsX, 0, NULL, NULL);
	status = clEnqueueReadBuffer(cmdQueue, output_positionsY, CL_TRUE, 0, sizeof(int) * datasize, positionsY, 0, NULL, NULL);

	system("pause");

	// Affichage des écarts types et positions respectives
	for (unsigned int i = 0; i < 16; i++) {
		printf("X: %d, Y: %d, ecart type: %f \n", positionsX[i], positionsY[i], ecartTypes[i]);
	}

	/*printf("X: %d, Y: %d, ecart type: %f \n", positionsX[0], positionsY[0], ecartTypes[0]);
	printf("X: %d, Y: %d, ecart type: %f \n", positionsX[1], positionsY[1], ecartTypes[1]);
	printf("X: %d, Y: %d, ecart type: %f \n", positionsX[2], positionsY[2], ecartTypes[2]);
	printf("X: %d, Y: %d, ecart type: %f \n", positionsX[3], positionsY[3], ecartTypes[3]);*/

	system("pause");

	float *maxEcartTypes = NULL;
	maxEcartTypes = (float*)malloc(sizeof(float) * msgSize);

	kernel = clCreateKernel(program, "maxEcartType", &status);

	cl_mem output_pos = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * msgSize, NULL, &status);

	int blockSize = datasize / msgSize;

	status = clSetKernelArg(kernel, 0, sizeof(cl_int), &datasize);
	status = clSetKernelArg(kernel, 1, sizeof(cl_int), &blockSize);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &output_ecartTypes);
	status = clSetKernelArg(kernel, 3, sizeof(cl_mem), &output_pos);

	size_t globalWorkSize2[1];
	globalWorkSize2[0] = msgSize;
	const size_t localWorkSize = 1; // utiliser modulo dans la formule pour éviter l'arrondi

	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize2, &localWorkSize, 0, NULL, NULL);

	int *pos = NULL;
	pos = (int*)malloc(sizeof(int) * msgSize);

	status = clEnqueueReadBuffer(cmdQueue, output_pos, CL_TRUE, 0, sizeof(int) * msgSize, pos, 0, NULL, NULL);

	for (int i = 0; i < msgSize; i++) {
		printf("Max ecart type position: %d\n", pos[i]);
	}

	system("pause");

	kernel = clCreateKernel(program, "writeMessageOnImage", &status);

	cl_mem message_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(bool) * msgSize, NULL, &status);

	//////////////step9//////////////////////
	// Associate the input and output buffers with the
	// kernel using clSetKernelArg()
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &message_buffer);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_pos);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &image2d_clean);

	///////////////step10//////////////////////
	// Define an index space (global work size) of work
	// items for execution. A workgroup size (local worksize) is not required, but can be used.
	// There are 'elements' work-items
	globalWorkSize2[0] = msgSize;

	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize2, NULL, 0, NULL, NULL);

	status = clEnqueueReadBuffer(cmdQueue, image2d_clean, CL_TRUE, 0, sizeof(float) * imgLength, bmpImage_clean, 0, NULL, NULL);

	storeImage(bmpImage_clean, pathImage2D_crypted, width, height, pathImage2D_clean);

	/////////////////step13/////////////////////////
	// Free OpenCL ressources
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(image2d_clean);
	//clReleaseMemObject(image2d_crypted);
	clReleaseContext(context);

	// Free host ressources
	free(bmpImage_clean);
	//free(bmpImage_crypted);
	free(platforms);
	free(devices);

	system("pause");
	return 0;
}