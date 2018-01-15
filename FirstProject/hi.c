
#include <stdio.h>

#include <stdlib.h>




#ifdef _ _APPLE_ _

#include <OpenCL/opencl.h>

#else

#include <CL/cl.h>

#endif




#define MEM_SIZE (128)

#define MAX_SOURCE_SIZE (0x100000)




int main()

{

	cl_device_id device_cpu = NULL;

	cl_device_id device_gpu = NULL;

	cl_context context_cpu = NULL;

	cl_context context_gpu = NULL;

	cl_command_queue command_queue_cpu = NULL;

	cl_command_queue command_queue_gpu = NULL;

	cl_mem memobj_cpu = NULL;

	cl_mem memobj_gpu = NULL;

	cl_program program_cpu = NULL;

	cl_program program_gpu = NULL;

	cl_kernel kernel_cpu = NULL;

	cl_kernel kernel_gpu = NULL;

	cl_platform_id platform[2];

	cl_uint ret_num_devices;

	cl_uint ret_num_platforms;

	cl_int ret;




	char string_cpu[MEM_SIZE];

	char string_gpu[MEM_SIZE];




	FILE *fp;

	char fileName[] = "./hello.cl";

	char *source_str;

	size_t source_size;




	/* 커널을 포함한 소스 코드를 로드 */

	fp = fopen(fileName, "r");

	if (!fp) {

		fprintf(stderr, "Failed to load kernel.\n");

		exit(1);

	}

	source_str = (char*)malloc(MAX_SOURCE_SIZE);

	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);

	fclose(fp);




	/* 플랫폼, 디바이스 정보를 얻음 */

	ret = clGetPlatformIDs(2, platform, &ret_num_platforms);

	ret = clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_ALL, 1, &device_gpu, &ret_num_devices);

	ret = clGetDeviceIDs(platform[1], CL_DEVICE_TYPE_ALL, 1, &device_cpu, &ret_num_devices);




	/* OpenCL 컨텍스트 생성 */

	context_gpu = clCreateContext(NULL, 1, &device_gpu, NULL, NULL, &ret);

	context_cpu = clCreateContext(NULL, 1, &device_cpu, NULL, NULL, &ret);




	/* 커맨드 큐 생성 */

	command_queue_cpu = clCreateCommandQueue(context_cpu, device_cpu, 0, &ret);

	command_queue_gpu = clCreateCommandQueue(context_gpu, device_gpu, 0, &ret);




	/* 메모리 버퍼 생성 */

	memobj_cpu = clCreateBuffer(context_cpu, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(char), NULL, &ret);

	memobj_gpu = clCreateBuffer(context_gpu, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(char), NULL, &ret);




	/* 미리 로드한 소스 코드로 커널 프로그램을 생성 */

	program_cpu = clCreateProgramWithSource(context_cpu, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

	program_gpu = clCreateProgramWithSource(context_gpu, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);




	/* 커널 프로그램 빌드 */

	ret = clBuildProgram(program_cpu, 1, &device_cpu, NULL, NULL, NULL);

	ret = clBuildProgram(program_gpu, 1, &device_gpu, NULL, NULL, NULL);




	/* OpenCL 커널 생성*/

	kernel_cpu = clCreateKernel(program_cpu, "hello", &ret);

	kernel_gpu = clCreateKernel(program_gpu, "hello", &ret);




	/* OpenCL 커널 파라미터 설정 */

	ret = clSetKernelArg(kernel_cpu, 0, sizeof(cl_mem), (void *)&memobj_cpu);

	ret = clSetKernelArg(kernel_gpu, 0, sizeof(cl_mem), (void *)&memobj_gpu);




	/* OpenCL 커널 실행 */

	ret = clEnqueueTask(command_queue_cpu, kernel_cpu, 0, NULL, NULL);

	ret = clEnqueueTask(command_queue_gpu, kernel_gpu, 0, NULL, NULL);




	/* 실행 결과를 메모리 버퍼에서 얻음 */

	ret = clEnqueueReadBuffer(command_queue_cpu, memobj_cpu, CL_TRUE, 0, MEM_SIZE * sizeof(char), string_cpu, 0, NULL, NULL);

	ret = clEnqueueReadBuffer(command_queue_gpu, memobj_gpu, CL_TRUE, 0, MEM_SIZE * sizeof(char), string_gpu, 0, NULL, NULL);




	/* 결과 출력 */

	puts(string_cpu);

	puts(string_gpu);




	/* 종료 처리 */

	ret = clFlush(command_queue_cpu);

	ret = clFlush(command_queue_gpu);

	ret = clFinish(command_queue_cpu);

	ret = clFinish(command_queue_gpu);

	ret = clReleaseKernel(kernel_cpu);

	ret = clReleaseKernel(kernel_gpu);

	ret = clReleaseProgram(program_cpu);

	ret = clReleaseProgram(program_gpu);

	ret = clReleaseMemObject(memobj_cpu);

	ret = clReleaseMemObject(memobj_gpu);

	ret = clReleaseCommandQueue(command_queue_cpu);

	ret = clReleaseCommandQueue(command_queue_gpu);

	ret = clReleaseContext(context_cpu);

	ret = clReleaseContext(context_gpu);




	free(source_str);




	while (1);

	return 0;

}
