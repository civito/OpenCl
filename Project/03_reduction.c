#include<stdio.h>
#include<CL/cl.h>
#include<stdlib.h>
#include<string.h>

#define CHECK_ERROR(err) \
	if(err != CL_SUCCESS) { \
		printf("[%s:%d] OpenCl error %d\n", __FILE__, __LINE__, err); \
	exit(EXIT_FAILURE); \
	}

#define MAX_SOURCE_SIZE (0x1000000)


static int N = 16777216;


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
	cl_kernel kernel_reduction_barrier;
	kernel_reduction_barrier = clCreateKernel(program, "reduction_barrier", &err);

	int *g_num;
	int *partial_sum;
	int result = 0;

	size_t global_size = N;
	size_t local_size = 64;
	size_t num_work_group = global_size / local_size;

	g_num = (int*)malloc(sizeof(int)*N);
	partial_sum = (int *)malloc(sizeof(int)*num_work_group);

	int i;
	for(i=0; i< N; i++){
		g_num[i] = 1;
	}

	cl_mem buf_num, buf_partial_sum;
	buf_num = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)*N, NULL, &err);
	CHECK_ERROR(err);
	buf_partial_sum = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)*num_work_group, NULL, &err);
	CHECK_ERROR(err);

	err = clEnqueueWriteBuffer(queue, buf_num, CL_FALSE, 0, sizeof(int)*N,g_num,0,NULL,NULL);
	CHECK_ERROR(err);

	err = clSetKernelArg(kernel_reduction_barrier, 0, sizeof(cl_mem), &buf_num);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_reduction_barrier, 1, sizeof(cl_mem), &buf_partial_sum);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_reduction_barrier, 2, sizeof(int)*local_size, NULL);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_reduction_barrier, 3, sizeof(int), &N);
	CHECK_ERROR(err);


	err = clEnqueueNDRangeKernel(queue, kernel_reduction_barrier, 1,NULL, &global_size,&local_size,0,NULL,NULL);
	CHECK_ERROR(err);

	err = clEnqueueReadBuffer(queue, buf_partial_sum, CL_TRUE,0,sizeof(int)*num_work_group,partial_sum,0,NULL,NULL);
	CHECK_ERROR(err);

	int sum = 0;

	for(i = 0; i< num_work_group; i++){
		sum += partial_sum[i];
	}
	

	
	printf("result : %d\n",sum);


	/* Release Source */
	clReleaseMemObject(buf_num);
	clReleaseMemObject(buf_partial_sum);
	free(g_num);
	free(partial_sum);
	clReleaseKernel(kernel_reduction_barrier);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

}
