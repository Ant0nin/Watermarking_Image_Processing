#ifndef __CL_EXTEND__
#define __CL_EXTEND__

#include <CL\cl.h>

cl_int autoCreateCmdQueueAndContext(cl_command_queue *cmdQueue, cl_context *context, cl_device_id *devices);
cl_program createKernelProgramFromFile(cl_context *context, const char* kernelFileName, cl_int *status);
void debugKernelWithLog(cl_program program, cl_device_id *devices);

#endif