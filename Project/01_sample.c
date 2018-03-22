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

	
	

	/* Get Platform Info */
	cl_platform_id platform;
	cl_uint num_platform;

	clGetPlatformIDs(1, &platform, &num_platform);
	printf("***** PlatForm Info ******\n");
	clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(char)*1024, info, NULL);
	printf("Platform Name : %s\n",info);
	clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(char)*1024, info, NULL);
	printf("platform Vendor : %s\n",info);
	clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(char)*1024, info, NULL);
	printf("platform Version : %s\n",info);


	printf("\n");
	/* Get Device Info */
	cl_uint num_device;
	cl_device_id device;

	printf("***** Device Info ******\n");
	clGetDeviceIDs(platform,CL_DEVICE_TYPE_CPU, 0, NULL, &num_device);
	printf("Available number of CPU device : %d\n", num_device);
	clGetDeviceIDs(platform,CL_DEVICE_TYPE_GPU, 0, NULL, &num_device);
	printf("Available number of GPU device : %d\n", num_device);

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);

	clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(char)*1024, info, NULL);
	printf("Device Name : %s\n", info);
	clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(char)*1024, info, NULL);
	printf("Device Vendor : %s\n", info);
	clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &info_CU, NULL);
	printf("Device CU num : %d\n", info_CU);
	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &info_size, NULL);
	printf("Device WORK_GROUP_SIZE : %zu\n", info_size);
	clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &info_long, NULL);
	printf("Device Global Memory Size : %ld\n", info_long);
	clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &info_long, NULL);
	printf("Device Local Memory Size : %ld\n", info_long);
	clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &info_long, NULL);
	printf("Device Size of Memory Object  : %ld\n", info_long);
	clGetDeviceInfo(device, CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(cl_bool), &info_bool, NULL);
	printf("IS Host and Device share Momory? : %d\n", info_bool);

	/* Create Context */
	cl_context context;
	cl_int err;

	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	CHECK_ERROR(err);

	/* Create Command Queue*/
	cl_command_queue queue;
	queue = clCreateCommandQueue(context, device, 0, &err);
	CHECK_ERROR(err);

	/* Build Program */
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

	/* Kernel Object */
	cl_kernel kernel_vec_add;
	kernel_vec_add = clCreateKernel(program, "vec_add", &err);

	/* Create Buffer */
	int *A, *B, *C;
	int i;

	A = (int*)malloc(sizeof(int)*16384);
	B = (int*)malloc(sizeof(int)*16384);
	C = (int*)malloc(sizeof(int)*16384);

	for(i = 0; i < 16384; i++ ){
		A[i] = 1;
		B[i] = 2;
	}
	
	cl_mem bufA, bufB, bufC;
	bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int)*16884, NULL, &err);
	bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int)*16384, NULL, &err);
	bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int)*16384, NULL, &err);
	CHECK_ERROR(err);


	err = clEnqueueWriteBuffer(queue, bufA, CL_FALSE, 0, sizeof(int)*16384,A,0,NULL,NULL);
	CHECK_ERROR(err);
	err = clEnqueueWriteBuffer(queue, bufB, CL_FALSE, 0, sizeof(int)*16384,B,0,NULL,NULL);
	CHECK_ERROR(err);


	/* Kerenl arg */
	err = clSetKernelArg(kernel_vec_add, 0, sizeof(cl_mem), &bufA);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_vec_add, 1, sizeof(cl_mem), &bufB);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_vec_add, 2, sizeof(cl_mem), &bufC);
	CHECK_ERROR(err);

	/* Executing Kernel */
	size_t global_size = 16384;
	size_t local_size = 256;

	err = clEnqueueNDRangeKernel(queue, kernel_vec_add, 1,NULL, &global_size,&local_size,0,NULL,NULL);
	CHECK_ERROR(err);

	/* Read Back from Device buffer */

	err = clEnqueueReadBuffer(queue, bufC, CL_TRUE,0,sizeof(int)*16384,C,0,NULL,NULL);
	CHECK_ERROR(err);
	
	int sum = 0;

	for(i=0; i<16384; i++){
		sum += C[i];
	}

	printf("Sum : %d\n",sum);


	


	/* Release Source */
	clReleaseMemObject(bufA);
	clReleaseMemObject(bufB);
	clReleaseMemObject(bufC);
	free(A);
	free(B);
	free(C);
	clReleaseKernel(kernel_vec_add);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

}
