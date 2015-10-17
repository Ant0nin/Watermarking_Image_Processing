#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CL_CHECK(_expr) \
do { cl_int _err = _expr; \
	if (_err == CL_SUCCESS) break; \
	fprintf(stderr, "OpenCL Error: '%s' returned %d!\n", #_expr, (int)_err); \
	abort(); \
} while (0)

#define MAX_SOURCE_SIZE (0x100000)
//_CRT_SECURE_NO_DEPRECATE

void debugKernel(cl_program program, cl_device_id *devices){
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

char* readKernelSource(const char* kernelSourcePath, size_t *source_size){
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
	int *A = NULL; // Input arrayy
	int *B = NULL; // Input array
	int *C = NULL; // Output array
	int rowsA = 8, colsA = 8, colsB=8;
	const int elements = rowsA*colsA;//
	const int elementsA = rowsA*colsA;//
	const int elementsB = colsA*colsB;//
	const int elementsC = rowsA*colsB;//
	size_t datasize = sizeof(int)*elements;
	size_t datasizeA = sizeof(int)*elementsA;
	size_t datasizeB = sizeof(int)*elementsB;
	size_t datasizeC = sizeof(int)*elementsC;
	A = (int*)malloc(datasizeA);
	B = (int*)malloc(datasizeB);
	C = (int*)malloc(datasizeC);
	// Initialize the input data
	for (int i = 0; i < elementsA; i++) {
		A[i] = i;
	}
	printf("Matriz A\n");
	for (int r = 0; r < rowsA; r++){
		for (int c = 0; c < colsA; c++){
			printf("%i ", A[r*colsA + c]);
		}
		printf("\n");
	}
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
	cmdQueue = clCreateCommandQueue(context, devices[0], 0, &status);

	/////////////////step5///////////////////////
	cl_mem bufferA; // Input MatrizA on the device
	cl_mem bufferB; // Input MatrizB on the device
	cl_mem bufferC; // Output MatrizC on the device
	// Use clCreateBuffer() to create buffer objects
	// that will contain the data from the host arrays
	bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasizeA, NULL, &status);
	bufferB = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasizeB, NULL, &status);
	bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasizeC, NULL, &status);

	//////////////step6////////////////////////
	// Use clEnqueueWriteBuffer() to write input array A
	// to the device buffer bufferA
	status = clEnqueueWriteBuffer(cmdQueue, bufferA, CL_FALSE, 0, datasizeA, A, 0,
		NULL, NULL);


	/////////////load file///////////////////////
	/*size_t programSize=0;
	const char * filename = "MultipliMatriz-kernel.cpp";
	char* sProgramSource = readKernelSource(filename, &programSize);
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&sProgramSource, &programSize, &status);*/

	FILE *fp;
	char *source_str;
	size_t source_size;
	fp = fopen("kernel\\KernelBmpMaxValue.cl", "r");
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
	status=clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
	// build failed
	if (status != CL_SUCCESS) {
		debugKernel(program, devices);
		return 1;
	}
	//////////////step8///////////////////////////
	cl_kernel kernel = NULL;
	// Use clCreateKernel() to create a kernel from the
	// vector addition function (named "vecadd")
	kernel = clCreateKernel(program, "max_vector", &status);

	int tmpColsA = colsA, tmpRowsA=rowsA,tmpRowsB=colsA,tmpColsB=colsB;

	//////////////step9//////////////////////
	// Associate the input and output buffers with the
	// kernel using clSetKernelArg()
	status = clSetKernelArg(kernel, 0, sizeof(int), &elements);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferA);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferB);
	//status = clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufferC);

	///////////////step10//////////////////////
	// Define an index space (global work size) of work
	// items for execution. A workgroup size (local worksize) is not required, but can be used.
	size_t globalWorkSize[1];
	// There are 'elements' work-items
	globalWorkSize[0] = colsB*rowsA;

	size_t localWorkSize = 2;

	//////////////////step11//////////////////
	// Execute the kernel by using
	// clEnqueueNDRangeKernel().
	// 'globalWorkSize' is the 1D dimension of the work-items
	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 2, NULL, globalWorkSize,
		&localWorkSize, 0, NULL, NULL);
	//////////////////step12////////////////////////
	// Use clEnqueueReadBuffer() to read the OpenCL
	// output buffer (bufferC) to the host output array (C)
	size_t countWG = globalWorkSize[0] / localWorkSize;
	clEnqueueReadBuffer(cmdQueue, bufferB, CL_TRUE, 0, countWG, B, 0, NULL, NULL); // values
	//clEnqueueReadBuffer(cmdQueue, bufferC, CL_TRUE, 0, countWG, C, 0, NULL, NULL); // position

	// Verify the output
	for (int i = 0; i < countWG; i++){
		printf("value: %i, ", B[i]);
		//printf("position: %i, \n", C[i]);
	}
	/* 10 13 
	   28 40
	*/	
	/////////////////step13/////////////////////////
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
	system("pause");
	return 0;
}