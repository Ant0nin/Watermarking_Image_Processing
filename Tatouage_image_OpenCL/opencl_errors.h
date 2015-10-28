#ifndef _OPENCL_ERRORS_H
#define _OPENCL_ERRORS_H

/* from http://codepad.org/6FNvVllQ from <Aldaron> on #opencl on
   freenode */

/* Example : 
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    checkError(status, "clGetPlatformIDs", __LINE__);

    Terminal output : Error : CL_DEVICE_NOT_FOUND (Line 155 : clGetDeviceIDs)
*/

static const char* clErrorString[] = {
    "CL_SUCCESS",                                   // 0
    "CL_DEVICE_NOT_FOUND",                          // -1
    "CL_DEVICE_NOT_AVAILABLE",
    "CL_COMPILER_NOT_AVAILABLE",
    "CL_MEM_OBJECT_ALLOCATION_FAILURE",
    "CL_OUT_OF_RESOURCES",
    "CL_OUT_OF_HOST_MEMORY",
    "CL_PROFILING_INFO_NOT_AVAILABLE",
    "CL_MEM_COPY_OVERLAP",
    "CL_IMAGE_FORMAT_MISMATCH",
    "CL_IMAGE_FORMAT_NOT_SUPPORTED",
    "CL_BUILD_PROGRAM_FAILURE",
    "CL_MAP_FAILURE",
    "CL_MISALIGNED_SUB_BUFFER_OFFSET",
    "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
    "CL_COMPILE_PROGRAM_FAILURE", 
    "CL_LINKER_NOT_AVAILABLE", 
    "CL_LINK_PROGRAM_FAILURE", 
    "CL_DEVICE_PARTITION_FAILED", 
    "CL_KERNEL_ARG_INFO_NOT_AVAILABLE", 
    "", 
    "", 
    "", 
    "", 
    "", 
    "", 
    "", 
    "", 
    "", 
    "",
    "CL_INVALID_VALUE",
    "CL_INVALID_DEVICE_TYPE",
    "CL_INVALID_PLATFORM",
    "CL_INVALID_DEVICE",
    "CL_INVALID_CONTEXT",
    "CL_INVALID_QUEUE_PROPERTIES",
    "CL_INVALID_COMMAND_QUEUE",
    "CL_INVALID_HOST_PTR",
    "CL_INVALID_MEM_OBJECT",
    "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
    "CL_INVALID_IMAGE_SIZE",
    "CL_INVALID_SAMPLER",
    "CL_INVALID_BINARY",
    "CL_INVALID_BUILD_OPTIONS",
    "CL_INVALID_PROGRAM",
    "CL_INVALID_PROGRAM_EXECUTABLE",
    "CL_INVALID_KERNEL_NAME",
    "CL_INVALID_KERNEL_DEFINITION",
    "CL_INVALID_KERNEL",
    "CL_INVALID_ARG_INDEX",
    "CL_INVALID_ARG_VALUE",
    "CL_INVALID_ARG_SIZE",
    "CL_INVALID_KERNEL_ARGS",
    "CL_INVALID_WORK_DIMENSION",
    "CL_INVALID_WORK_GROUP_SIZE",
    "CL_INVALID_WORK_ITEM_SIZE",
    "CL_INVALID_GLOBAL_OFFSET",
    "CL_INVALID_EVENT_WAIT_LIST",
    "CL_INVALID_EVENT",
    "CL_INVALID_OPERATION",
    "CL_INVALID_GL_OBJECT",
    "CL_INVALID_BUFFER_SIZE",
    "CL_INVALID_MIP_LEVEL",
    "CL_INVALID_GLOBAL_WORK_SIZE",
    "CL_INVALID_PROPERTY",
    "CL_INVALID_IMAGE_DESCRIPTOR",
    "CL_INVALID_COMPILER_OPTIONS",
    "CL_INVALID_LINKER_OPTIONS",
    "CL_INVALID_DEVICE_PARTITION_COUNT"
};

inline const char* clGetErrorString(cl_int errorCode) {
    static const char* INVALIDERROR = "INVALID_ERROR_CODE";
    int errorNumber = (errorCode < 0) ? -errorCode : errorCode;
    if (errorCode == -1000) return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    return (errorCode <= 64) ? clErrorString[errorNumber] : INVALIDERROR;
}

inline void checkError(cl_int errorCode, const char* msg, int line)
{
    if(errorCode != CL_SUCCESS)
    {
        fprintf(stderr, "Error : %s (Line %d : %s)\n", clGetErrorString(errorCode), line, msg);
        exit(1);
    }
}

inline void checkError(cl_int errorCode, int line)
{
    if(errorCode != CL_SUCCESS)
    {
        fprintf(stderr, "Error : %s (Line : %d)\n", clGetErrorString(errorCode), line);
		system("pause");
        exit(1);
    }
}

#endif /* _OPENCL_ERRORS_H */
