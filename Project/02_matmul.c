#include<stdio.h>
#include<CL/cl.h>
#include<stdlib.h>

#define CHECK_ERROR(err) \
	if(err != CL_SUCCESS) { \
		printf("[%s:%d] OpenCl error %d\n", __FILE__, __LINE__, err); \
	exit(EXIT_FAILURE); \
	}

#define MAX_SOURCE_SIZE (0x1000000)



int main(){
	char info[1024];
	size_t info_size;	
	

	cl_platform_id platform;
	cl_uint num_platform;

	clGetPlatformIDs(1, &platform, &num_platform);


	cl_uint num_device;
	cl_device_id device;

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);


	cl_context context;
	cl_int err;

	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	CHECK_ERROR(err);


	cl_command_queue queue;
	queue = clCreateCommandQueue(context, device, 0, &err);
	CHECK_ERROR(err);


	FILE *fp;
	const char fileName[] = "./kernel.cl";
	size_t source_size;
	char *source_str;

	fp = fopen(fileName, "r");
	if(!fp){
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}

	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1 , MAX_SOURCE_SIZE, fp);
	fclose(fp);

	cl_program program;
	program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &err);
	CHECK_ERROR(err);
	
	err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	CHECK_ERROR(err);

	cl_kernel kernel;
	kernel = clCreateKernel(program, "matmul", &err);


	int *A, *B, *C;
	cl_int COLA = 1000;
	cl_int ROWA = 1000;
	cl_int COLB = 1000;
	int i;

	A = (int*)malloc(sizeof(int)*ROWA*COLB);
	B = (int*)malloc(sizeof(int)*ROWA*COLB);
	C = (int*)malloc(sizeof(int)*ROWA*COLB);

	for(i = 0; i < ROWA*COLB; i++ ){
		A[i] = 1;
		B[i] = 1;
	}
	
	cl_mem bufA, bufB, bufC;
	bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int)*ROWA*COLB, NULL, &err);
	bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int)*ROWA*COLB, NULL, &err);
	bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int)*ROWA*COLB, NULL, &err);
	CHECK_ERROR(err);


	err = clEnqueueWriteBuffer(queue, bufA, CL_FALSE, 0, sizeof(int)*ROWA*COLB,A,0,NULL,NULL);
	CHECK_ERROR(err);
	err = clEnqueueWriteBuffer(queue, bufB, CL_FALSE, 0, sizeof(int)*ROWA*COLB,B,0,NULL,NULL);
	CHECK_ERROR(err);


	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 3, sizeof(cl_int), &ROWA);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 4, sizeof(cl_int), &COLA);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 5, sizeof(cl_int), &COLB);
	CHECK_ERROR(err);

	size_t global_size[2] = {COLB, ROWA};

	err = clEnqueueNDRangeKernel(queue, kernel, 1,NULL, global_size,NULL,0,NULL,NULL);
	CHECK_ERROR(err);


	err = clEnqueueReadBuffer(queue, bufC, CL_TRUE,0,sizeof(int)*ROWA*COLB,C,0,NULL,NULL);
	CHECK_ERROR(err);
	
	int sum = 0;

	for(i=0; i<ROWA*COLB; i++){
		sum += C[i];
	}

	printf("Sum : %d\n",sum);


	
	clReleaseMemObject(bufA);
	clReleaseMemObject(bufB);
	clReleaseMemObject(bufC);
	free(A);
	free(B);
	free(C);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

}
