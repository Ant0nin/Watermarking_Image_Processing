#include <CL\cl.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmpfuncs.h"

int main() {

	int h, w;
	float *image_src = NULL;
	image_src = readImage("image/lena.bmp", &h, &w);

	const int elements = h*w;
	size_t imgsize = sizeof(int)*elements;
	float *image_dest;
	image_dest = (float*)malloc(imgsize);

	printf("Height:%d \n", h);
	printf("Width:%d \n", w);
	float max = 0;

	// Use this to check the output of each API call
	cl_int status;

	////////////////////////// STEP 1 //////////////////////////////////////////

	// Use clGetPlatformIDs() to retrieve the number of platforms
	cl_uint numPlatforms = 0;
	cl_platform_id *platforms = NULL;

	// Use clGetPlatformIDs() to retrieve the number of platforms
	status = clGetPlatformIDs(0, NULL, &numPlatforms);

	// Allocate enough space for each platform
	platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));

	// Fill in platforms with clGetPlatformIDs()
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);


	////////////////////////// STEP 2 //////////////////////////////////////////
	cl_uint numDevices = 0;
	cl_device_id *devices = NULL;
	// Use clGetDeviceIDs() to retrieve the number of
	// devices present
	status = clGetDeviceIDs(platforms[0],
		CL_DEVICE_TYPE_ALL,
		0, NULL, &numDevices);

	// Allocate enough space for each device
	devices = (cl_device_id*)malloc(numDevices *
		sizeof(cl_device_id));

	// Fill in devices with clGetDeviceIDs()
	status = clGetDeviceIDs(platforms[0],
		CL_DEVICE_TYPE_ALL,
		numDevices, devices,
		NULL);


	////////////////////////// STEP 3 //////////////////////////////////////////
	cl_context context = NULL;
	// Create a context using clCreateContext() and
	// associate it with the devices
	context = clCreateContext(NULL, numDevices,
		devices, NULL, NULL,
		&status);


	////////////////////////// STEP 4 //////////////////////////////////////////
	cl_command_queue cmdQueue;
	// Create a command queue using
	// clCreateCommandQueue(), and associate it with
	// the device you want to execute on
	cmdQueue = clCreateCommandQueue(context,
		devices[0],
		0, &status);

	////////////////////////// STEP 5 //////////////////////////////////////////
	cl_mem bufferA; // Input array on the device
	cl_mem bufferB; // Input array on the device
	cl_mem bufferC; // Output array on the device
					// Use clCreateBuffer() to create buffer objects
					// that will contain the data from the host arrays
	bufferA = clCreateBuffer(context,
		CL_MEM_READ_ONLY,
		datasize, NULL, &status);

	bufferB = clCreateBuffer(context,
		CL_MEM_READ_ONLY,
		datasize, NULL, &status);

	bufferC = clCreateBuffer(context,
		CL_MEM_WRITE_ONLY,
		datasize, NULL, &status);

	////////////////////////// STEP 6 //////////////////////////////////////////
	// Use clEnqueueWriteBuffer() to write input array A
	// to the device buffer bufferA
	status = clEnqueueWriteBuffer(cmdQueue, bufferA,
		CL_FALSE, 0,
		datasize, A, 0,
		NULL, NULL);

	// Use clEnqueueWriteBuffer() to write input array B
	// to the device buffer bufferB
	status = clEnqueueWriteBuffer(cmdQueue, bufferB,
		CL_FALSE, 0,
		datasize, B, 0,
		NULL, NULL);


	////////////////////////// STEP 7 //////////////////////////////////////////
	// Create a program using
	// clCreateProgramWithSource()
	cl_program program = clCreateProgramWithSource(
		context,
		sizeof(programSource) / sizeof(*programSource),
		programSource, NULL, &status);

	// Build (compile) the program for the devices with
	// clBuildProgram()
	status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);

	////////////////////////// STEP 8 //////////////////////////////////////////
	cl_kernel kernel = NULL;
	// Use clCreateKernel() to create a kernel from the
	// vector addition function (named "vecadd")
	kernel = clCreateKernel(program, "simpleMultiply", &status);


	////////////////////////// STEP 9 //////////////////////////////////////////
	// Associate the input and output buffers with the
	// kernel using clSetKernelArg()
	int *dimW;
	dimW = &w;
	int *dimH;
	dimH = &h;

	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferC);

	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), dimW);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), dimH);
	status = clSetKernelArg(kernel, 3, sizeof(cl_mem), dimW);
	status = clSetKernelArg(kernel, 4, sizeof(cl_mem), dimH);

	status = clSetKernelArg(kernel, 5, sizeof(cl_mem), &bufferA);
	status = clSetKernelArg(kernel, 6, sizeof(cl_mem), &bufferB);




	////////////////////////// STEP 10 //////////////////////////////////////////
	// Define an index space (global work size) of work
	// items for execution. A workgroup size (local work
	// size) is not required, but can be used.
	size_t globalWorkSize[2];
	// There are 'elements' work-items
	globalWorkSize[0] = w;
	globalWorkSize[1] = h;

	clFinish(cmdQueue);

	////////////////////////// STEP 11 //////////////////////////////////////////
	// Execute the kernel by using
	// clEnqueueNDRangeKernel().
	// 'globalWorkSize' is the 1D dimension of the
	// work-items
	status = clEnqueueNDRangeKernel(
		cmdQueue,
		kernel,
		2,
		NULL,
		globalWorkSize,
		NULL,
		0,
		NULL,
		NULL);

	////////////////////////// STEP 12 //////////////////////////////////////////
	// Use clEnqueueReadBuffer() to read the OpenCL
	// output buffer (bufferC) to the host output array (C)
	clEnqueueReadBuffer(
		cmdQueue,
		bufferC,
		CL_TRUE,
		0,
		datasize,
		C,
		0,
		NULL,
		NULL);

	// Display
	for (int i = 0; i < elements; i++) {
		printf("%f ", C[i]);
	}

	////////////////////////// STEP 13 //////////////////////////////////////////
	// Free OpenCL ressources
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(bufferA);
	clReleaseMemObject(bufferB);
	clReleaseMemObject(bufferC);
	clReleaseContext(context);
	// Free host ressources
	free(A);
	free(B);
	free(C);
	free(platforms);
	free(devices);



	return 0;
}