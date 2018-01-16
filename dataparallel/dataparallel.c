#include<stdio.h>
#include<stdlib.h>

#ifdef __APPLE__
#include<OpenCL / opencl.h>
#else
#include<CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

int main() {
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem Amobj = NULL;
	cl_mem Bmobj = NULL;
	cl_mem Cmobj = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_uint ret_num_device;
	cl_uint ret_num_platforms;
	cl_int ret;

	int i, j;
	float * A;
	float * B;
	float * C;

	A = (float *)malloc(4 * 4 * sizeof(float));
	B = (float *)malloc(4 * 4 * sizeof(float));
	C = (float *)malloc(4 * 4 * sizeof(float));

	FILE *fp;
	const char fileName[] = "./dataparallel.cl";
	size_t source_size;
	char *source_str;

	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}

	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			A[i * 4 + j] = i * 4 + j + 1;
			B[i * 4 + j] = i * 4 + j + 1;
		}
	}

	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_device);

	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret); //1개의 디바이스를 지정, 컨텍스트는 디바이스들을 제어할 하나의 컴퓨터라 생각

	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	Amobj = clCreateBuffer(context, CL_MEM_READ_WRITE, 4 * 4 * sizeof(float), NULL, &ret);
	Bmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, 4 * 4 * sizeof(float), NULL, &ret);
	Cmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, 4 * 4 * sizeof(float), NULL, &ret);

	ret = clEnqueueWriteBuffer(command_queue, Amobj, CL_TRUE, 0, 4 * 4 * sizeof(float), A, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, Bmobj, CL_TRUE, 0, 4 * 4 * sizeof(float), A, 0, NULL, NULL);

	program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t *)&source_size, &ret);
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	kernel = clCreateKernel(program, "dataParallel", &ret);

	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&Amobj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&Bmobj);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&Cmobj);

	size_t global_item_size = 4;
	size_t local_item_size = 1;

	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	ret = clEnqueueReadBuffer(command_queue, Cmobj, CL_TRUE, 0, 4 * 4 * sizeof(float), C, 0, NULL, NULL);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%7.2f ", C[i*4+j]);
		}
		printf("\n");
	}

	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseMemObject(Amobj);
	ret = clReleaseMemObject(Bmobj);
	ret = clReleaseMemObject(Cmobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	free(source_str);

	free(A);
	free(B);
	free(C);

	return 0;

}
