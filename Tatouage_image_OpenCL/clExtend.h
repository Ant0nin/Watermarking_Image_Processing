#ifndef __CL_EXTEND__
#define __CL_EXTEND__

#include <CL\cl.h>

cl_device_id* getDevices(cl_uint *num_devices, cl_int *status);
cl_program createKernelProgramFromFile(cl_context *context, const char* kernelFileName, cl_int *status);
void debugKernelProgramWithLog(cl_program program, cl_device_id *devices);

#endif