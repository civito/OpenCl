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
	clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &info_int, NULL);
	printf("Device CU num : %d\n", info_int);
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
	float *A, *B, *C;
	int i;

	A = (float*)malloc(sizeof(float)*4*4);
	B = (float*)malloc(sizeof(float)*4*4);
	C = (float*)malloc(sizeof(float)*4*4);

	for(i = 0; i < 16; i++ ){
		A[i] = i + 1;
		B[i] = i * 2;
	}
	
	cl_mem bufA, bufB, bufC;
	bufA = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*4*4, NULL, &err);
	bufB = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*4*4, NULL, &err);
	CHECK_ERROR(err);


	err = clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, sizeof(float)*4*4,A,0,NULL,NULL);
	CHECK_ERROR(err);
	err = clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, sizeof(float)*4*4,B,0,NULL,NULL);
	CHECK_ERROR(err);

	err = clEnqueueReadBuffer(queue, bufB, CL_TRUE,0,sizeof(float)*4*4,C,0,NULL,NULL);
	CHECK_ERROR(err);
	
	
	printf("Read Buffer : \n");
	for(i=0; i<16; i++){
		printf(" %f \n", C[i]);
	}


	





	/* Release Source */
	clReleaseKernel(kernel_vec_add);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);


	

	


	







	

	

}
