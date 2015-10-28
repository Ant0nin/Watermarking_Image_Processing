#include <stdio.h>
#include <stdlib.h>
#include <CL\cl.h>
#include "clExtend.h"
#include "opencl_errors.h"
#include "bmpfuncs.h"

#define ERR_WRONG_INPUT_ARGUMENTS 1

const char clFile_reduction[] = "question1_01";
const char kernel_reduction[] = "reduction";

int main(int argc, char *argv[]) {
	
	// TODO : default values (lena)
	// TODO : line numbers in checkerrors
	
	/*if (argc < 2)
		return ERR_WRONG_INPUT_ARGUMENTS;
	
	const char *imageInputPath = argv[0];
	const char *imageOutputPath = argv[1];*/

	const char imageInputPath[] = "image/lena.bmp";
	const char imageOutputPath[] = "image/output.bmp";
	
	int imageWidth, imageHeight;
	float *image = NULL;
	image = readImage(imageInputPath, &imageWidth, &imageHeight);

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
	checkError(status, 44);

	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	checkError(status, 47);

	cmdQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);
	checkError(status, 50);
	clFinish(cmdQueue);

	program = createKernelProgramFromFile(&context, clFile_reduction, &status);
	checkError(status, 54);

	status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
	
	kernel = clCreateKernel(program, kernel_reduction, &status);
	checkError(status, 58);

	// Input
	int pixels_len = imageHeight*imageWidth;
	size_t datasize = sizeof(float)*pixels_len;
	cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
	cl_mem bufferB = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &status);
	cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &status);
	float *max_values = NULL;
	max_values = (float*)malloc(datasize);
	float *max_pos = NULL;
	max_pos = (float*)malloc(datasize);
	int totalN = pixels_len;
	size_t workgroup_size;
	status = clGetKernelWorkGroupInfo(kernel, devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &workgroup_size, NULL);
	size_t localWorkSize = 1024;
	if (localWorkSize > workgroup_size) {
		localWorkSize = workgroup_size;
	}
	status = clEnqueueWriteBuffer(cmdQueue, bufferA, CL_FALSE, 0, datasize, image, 0, NULL, NULL);

	// Mapping
	status = clSetKernelArg(kernel, 0, sizeof(int), &totalN);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferA);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferB);
	status = clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufferC);
	status = clSetKernelArg(kernel, 4, localWorkSize * sizeof(cl_float), NULL);
	status = clSetKernelArg(kernel, 5, localWorkSize * sizeof(cl_float), NULL);

	// Index space
	size_t globalWorkSize[1];
	globalWorkSize[0] = pixels_len;

	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize, &localWorkSize, 0, NULL, &event);
	clWaitForEvents(1, &event);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time = (double)(time_end - time_start);
	printf("Taille de l'image (en pixels) : %d, local worksize : %d\n", (int)pixels_len, (int)localWorkSize);
	printf("Temps d'execution : %0.3f milliseconds\n", (total_time / 1000000.0));

	size_t datasizeOut = sizeof(float)*(pixels_len / localWorkSize);
	clEnqueueReadBuffer(cmdQueue, bufferB, CL_TRUE, 0, datasizeOut, max_values, 0, NULL, NULL);
	clEnqueueReadBuffer(cmdQueue, bufferC, CL_TRUE, 0, datasizeOut, max_pos, 0, NULL, NULL);

	unsigned int countReduction = pixels_len / (int)localWorkSize;
	float max = max_values[0];
	float pos = max_pos[0];
	for (unsigned int i = 1; i < countReduction; i++) {
		//printf("%.1f, ", max_pos[i]);
		if (max_values[i] > max) {
			max = max_values[i];
			pos = max_pos[i];
		}
	}
	printf("\nThe max element is: %.1f \n", max);
	printf("\nThe pos of max element is: %.1f \n", pos);

	storeImage(image, imageOutputPath, imageHeight, imageWidth, imageInputPath);

	// Free resources
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(bufferA);
	clReleaseMemObject(bufferB);
	clReleaseMemObject(bufferC);
	clReleaseContext(context);
	free(image);
	free(max_values);
	free(max_pos);
	free(devices);

	system("pause");
	return CL_SUCCESS;
}