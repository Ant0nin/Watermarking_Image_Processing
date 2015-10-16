#include <string>
#include <fstream>
#include <CL\cl.h>

using namespace std;

/* Author : Nicolas Pavie */
size_t loadKernelFile(string filename, const char* programSource) {
	ifstream source(filename.c_str(), ifstream::in);
	cl_int error = source.is_open() ? CL_SUCCESS : CL_INVALID_KERNEL;
	string strSource=string(istreambuf_iterator <char>(source), istreambuf_iterator <char>());
	programSource = strSource.c_str();
	return strSource.length();
}

int main() {
	
	const char* sourceProgram = NULL;
	loadKernelFile("kernel\\kernelBmpMaxValue.cl", sourceProgram);

	//printf("%s", programSource);
	system("pause");

	return 0;
}