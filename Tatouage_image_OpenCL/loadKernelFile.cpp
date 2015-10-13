#include <string>
#include <fstream>
#include <CL\cl.h>

using namespace std;

/* Author : Nicolas Pavie */
void loadKernelFile(string filename) {
	ifstream source(filename.c_str(), ifstream::in);
	cl_int error = source.is_open ? CL_SUCCESS : CL_INVALID_KERNEL;
	string strSource=string(istreambuf_iterator <char>(source), istreambuf_iterator <char>());
	const char *programSource[1] = { strSource.c_str() };
	size_t programSourceLen[1] = { strSource.length() };
}
