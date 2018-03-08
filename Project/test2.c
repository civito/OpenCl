#include<stdio.h>
#include<CL/cl.h>

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

	

	

	

}
