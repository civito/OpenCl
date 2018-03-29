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
	cl_uint info_int;
	cl_uint info_CU;
	cl_ulong info_long;
	cl_bool info_bool;
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

	/* calculate on only 1 workitem */
	cl_kernel kernel_reduction_1item;
	kernel_reduction_1item = clCreateKernel(program, "reduction_1item", &err);

	int *g_num;
	int g_sum;
	int result = 0;

	int i;

	g_num = (int*)malloc(sizeof(int)*16777216);

	for(i = 0; i < 16777216; i++){
		g_num[i] = 0;
	}

	cl_mem bufn, bufs;
	bufn = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)*16777216, NULL, &err);
	CHECK_ERROR(err);
	bufs = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &err);
	CHECK_ERROR(err);

	err = clEnqueueWriteBuffer(queue, bufn, CL_FALSE, 0, sizeof(int)*16777216,g_num,0,NULL,NULL);
	CHECK_ERROR(err);
	err = clEnqueueWriteBuffer(queue, bufs, CL_FALSE, 0, sizeof(int),&g_sum,0,NULL,NULL);
	CHECK_ERROR(err);

	err = clSetKernelArg(kernel_reduction_1item, 0, sizeof(cl_mem), &bufn);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_reduction_1item, 1, sizeof(cl_mem), &bufs);
	CHECK_ERROR(err);

	size_t global_size = 16777216;
	size_t local_size = 1;

	err = clEnqueueNDRangeKernel(queue, kernel_reduction_1item, 1,NULL, &global_size,&local_size,0,NULL,NULL);
	CHECK_ERROR(err);

	err = clEnqueueReadBuffer(queue, bufs, CL_TRUE,0,sizeof(int),&result,0,NULL,NULL);
	CHECK_ERROR(err);
	

	printf("result : %d\n",result);
	


	/* Release Source */
	clReleaseMemObject(bufn);
	free(g_num);
	clReleaseKernel(kernel_reduction_1item);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

}
