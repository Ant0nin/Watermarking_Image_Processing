// Lecture de fichier .cl
// Débogage kernel
// Découverte des plate-formes

#include <CL/cl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SOURCE_SIZE (0x100000)

cl_int autoCreateCmdQueueAndContext(cl_command_queue *cmdQueue, cl_context *context) {

	cl_int status = 0;
	
	cl_uint numPlatforms = 0;
	cl_platform_id *platforms = NULL;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	platforms = (cl_platform_id*)malloc(numPlatforms *sizeof(cl_platform_id));
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);

	cl_uint numDevices = 0;
	cl_device_id *devices = NULL;

	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
	devices = (cl_device_id*)malloc(numDevices *sizeof(cl_device_id));
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);

	*context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	*cmdQueue = clCreateCommandQueue(*context, devices[0], 0, &status);

	return status;
}

cl_program createKernelProgramFromFile(cl_context *context, const char* kernelName, cl_int *status) {
	
	FILE *fp;
	char *source_str;
	size_t source_size;
	char filename[64];

	strcpy(filename, "kernel/");
	strcat(filename, kernelName);
	strcat(filename, ".cl");

	fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	cl_program program = clCreateProgramWithSource(*context, 1,
		(const char **)&source_str, (const size_t *)&source_size, status);

	return program;
}

void debugKernelWithLog(cl_program program, cl_device_id *devices) {
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