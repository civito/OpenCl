#include <stdio.h> 
#include <CL/cl.h>
 

int main(int argc, char *argv[])
{
	cl_int err;
	cl_platform_id platform;
	cl_uint num_platform;

	err = clGetPlatformIDs(1, &platform, &num_platform);
	if(err != CL_SUCCESS)
	{
	printf("Error!\n");
	return -1;
	}

	printf("avilable number of platform : %d\n", num_platform);

	char Buffer[512];
	cl_int Size;

	err = clGetPlatformInfo(platform, CL_PLATFORM_VERSION, 512, Buffer, NULL);
	if(err != CL_SUCCESS)
	{
	printf("Error! cna't find platform info\n");
	return -2;
	}

	printf("%s\n", Buffer);

	cl_uint num_device;

	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, 0, &num_device);

	if(err != CL_SUCCESS)
	{
	printf("Error! can't find device\n");
	return -3;
	}

	printf("%d\n", num_device);

	cl_device_id *devices = (cl_device_id*)malloc(sizeof(cl_device_id) * num_device);
	if(!devices)
	{
	printf("Error! fail to malloc\n");
	return -4;
	}

	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_device, devices, &num_device);
	if( err != CL_SUCCESS )
	{

 printf("error!, 장치 ID를 얻을 수 없습니다.\n");
 return -5;

	}

	err = clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 512, Buffer, NULL);
	if(err != CL_SUCCESS)
	{

	printf("error!, 장치 정보를 얻을 수 없습니다.\n");
	return -6;

	}

	printf("%s\n", Buffer);

	return 0;
} 

