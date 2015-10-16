#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#define CL_CHECK(_expr) \
do { cl_int _err = _expr; \
	if (_err == CL_SUCCESS) break; \
	fprintf(stderr, "OpenCL Error: '%s' returned %d!\n", #_expr, (int)_err); \
	abort(); \
} while (0)

int main(int argc, char **argv) {

	cl_platform_id platforms[100];
	cl_uint platforms_n = 0;
	CL_CHECK(clGetPlatformIDs(100, platforms,&platforms_n));
	printf("=== %d OpenCL platform(s) found: ===\n",
	platforms_n);
	for (int i=0; i<platforms_n; i++) {
		char buffer[10240];
		printf(" -- %d --\n", i);
		CL_CHECK(clGetPlatformInfo(platforms[i],CL_PLATFORM_PROFILE, 10240, buffer, NULL));
		printf(" PROFILE = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i],CL_PLATFORM_VERSION, 10240, buffer, NULL));
		printf(" VERSION = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i],CL_PLATFORM_NAME, 10240, buffer, NULL));
		printf(" NAME = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i],CL_PLATFORM_VENDOR, 10240, buffer, NULL));
		printf(" VENDOR = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i],
		CL_PLATFORM_EXTENSIONS, 10240, buffer,NULL));
		printf(" EXTENSIONS = %s\n", buffer);
	}
	if (platforms_n == 0) 
		return 1;
	cl_device_id devices[100];
	cl_uint devices_n = 0;
	CL_CHECK(clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_DEFAULT, 100, devices,&devices_n));
	printf("=== %d OpenCL device(s) found on platform:\n", devices_n);
	for (int i=0; i<devices_n; i++) {
		char buffer[10240];
		cl_uint buf_uint;
		cl_ulong buf_ulong;
		printf(" -- %d --\n", i);
		CL_CHECK(clGetDeviceInfo(devices[i],CL_DEVICE_NAME, sizeof(buffer), buffer, NULL));
		printf(" DEVICE_NAME = %s\n", buffer);
		CL_CHECK(clGetDeviceInfo(devices[i],CL_DEVICE_VENDOR, sizeof(buffer), buffer,NULL));
		printf(" DEVICE_VENDOR = %s\n", buffer);
		CL_CHECK(clGetDeviceInfo(devices[i],CL_DEVICE_VERSION, sizeof(buffer), buffer,NULL));
		printf(" DEVICE_VERSION = %s\n", buffer);
		CL_CHECK(clGetDeviceInfo(devices[i],CL_DRIVER_VERSION, sizeof(buffer), buffer,NULL));
		printf(" DRIVER_VERSION = %s\n", buffer);
		CL_CHECK(clGetDeviceInfo(devices[i],CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(buf_uint), &buf_uint, NULL));
		printf(" DEVICE_MAX_COMPUTE_UNITS =%u\n", (unsigned int)buf_uint);
		CL_CHECK(clGetDeviceInfo(devices[i],CL_DEVICE_MAX_CLOCK_FREQUENCY,sizeof(buf_uint), &buf_uint, NULL));
		printf(" DEVICE_MAX_CLOCK_FREQUENCY =%u\n", (unsigned int)buf_uint);
		CL_CHECK(clGetDeviceInfo(devices[i],
		CL_DEVICE_GLOBAL_MEM_SIZE,
		sizeof(buf_ulong), &buf_ulong, NULL));
		printf(" DEVICE_GLOBAL_MEM_SIZE = %llu\n",
		(unsigned long long)buf_ulong);
	}
	system("pause");
	if (devices_n == 0) return 1;
	return 0;
} // end main w