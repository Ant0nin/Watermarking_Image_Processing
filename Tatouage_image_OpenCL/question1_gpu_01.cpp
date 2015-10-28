#include <stdio.h>
#include <stdlib.h>
#include <CL\cl.h>
#include "clExtend.h"
#include "opencl_errors.h"
#include "bmpfuncs.h"

const char clFile_reduction[] = "question1_01";
const char kernel_reduction[] = "reduction";

int main(int argc, char *argv[]) {

	const char *imageInputPath;

	if (argc > 2) {
		printf("Need input image path as program argument, or nothing to use default value.\n");
		system("pause");
		return EXIT_FAILURE;
	}
	else if (argc == 2) {
		imageInputPath = argv[1];
	}
	else {
		imageInputPath = "image/lena.bmp";
	}

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
	checkError(status, 50);

	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	checkError(status, 53);

	cmdQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);
	checkError(status, 56);
	clFinish(cmdQueue);

	program = createKernelProgramFromFile(&context, clFile_reduction, &status);
	checkError(status, 60);

	status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
	checkError(status, 63);

	kernel = clCreateKernel(program, kernel_reduction, &status);
	checkError(status, 66);

	// Input
	int pixels_len = imageHeight*imageWidth;
	size_t datasize = sizeof(float)*pixels_len;
	cl_mem bufferImage = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
	checkError(status, 72);
	cl_mem bufferMaxValues = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &status);
	checkError(status, 74);
	cl_mem bufferMaxPos = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &status);
	float *max_values = NULL;
	max_values = (float*)malloc(datasize);
	float *max_pos = NULL;
	max_pos = (float*)malloc(datasize);
	int totalN = pixels_len;
	size_t workgroup_size;
	status = clGetKernelWorkGroupInfo(kernel, devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &workgroup_size, NULL);
	checkError(status, 83);

	size_t localWorkSize = 1024;
	if (localWorkSize > workgroup_size) {
		localWorkSize = workgroup_size;
	}
	status = clEnqueueWriteBuffer(cmdQueue, bufferImage, CL_FALSE, 0, datasize, image, 0, NULL, NULL);
	checkError(status, 90);

	// Mapping
	status = clSetKernelArg(kernel, 0, sizeof(int), &totalN);
	checkError(status, 94);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferImage);
	checkError(status, 96);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferMaxValues);
	checkError(status, 98);
	status = clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufferMaxPos);
	checkError(status, 100);
	status = clSetKernelArg(kernel, 4, localWorkSize * sizeof(cl_float), NULL);
	checkError(status, 102);
	status = clSetKernelArg(kernel, 5, localWorkSize * sizeof(cl_float), NULL);
	checkError(status, 104);

	// Index space
	size_t globalWorkSize[1];
	globalWorkSize[0] = pixels_len;

	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize, &localWorkSize, 0, NULL, &event);
	checkError(status, 111);
	clWaitForEvents(1, &event);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time = (double)(time_end - time_start);

	size_t datasizeOut = sizeof(float)*(pixels_len / localWorkSize);
	clEnqueueReadBuffer(cmdQueue, bufferMaxValues, CL_TRUE, 0, datasizeOut, max_values, 0, NULL, NULL);
	clEnqueueReadBuffer(cmdQueue, bufferMaxPos, CL_TRUE, 0, datasizeOut, max_pos, 0, NULL, NULL);

	// CPU work
	unsigned int countReduction = pixels_len / (int)localWorkSize;
	float max = max_values[0];
	unsigned int pos = (unsigned int)max_pos[0];
	for (unsigned int i = 1; i < countReduction; i++) {
		if (max_values[i] > max) {
			max = max_values[i];
			pos = (unsigned int)max_pos[i];
		}
	}
	unsigned int posX = pos % imageWidth;
	unsigned int posY = (pos - posX) / imageWidth;

	// Display
	printf("\n");
	printf("-------------------------------------------\n");
	printf("Max intensity pixel: %.1f \n", max);
	printf("Pixel position: X=%d, Y=%d \n", posX, posY);
	printf("\n");
	printf("Quantity of image pixels: %d \n", (int)pixels_len);
	printf("Local worksize: %d \n", (int)localWorkSize);
	printf("Kernel(s) execution time : %0.3f ms \n", (total_time / 1000000.0));
	printf("-------------------------------------------\n");
	printf("\n");

	// Free resources
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(bufferImage);
	clReleaseMemObject(bufferMaxValues);
	clReleaseMemObject(bufferMaxPos);
	clReleaseContext(context);
	free(image);
	free(max_values);
	free(max_pos);
	free(devices);

	//system("pause");
	return EXIT_SUCCESS;
}