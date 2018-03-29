
void hello(){
	printf("hello!\n");
}


__kernel void vec_add(__global int* A, __global int* B, __global int* C) {
	int i = get_global_id(0);
	C[i] = A[i] + B[i];
}

__kernel void matmul(__global int* A, __global int* B, __global int* C, int ROWA, int COLA, int COLB){

	int i = get_global_id(1);
	int j = get_global_id(0);
	int k;
	int sum = 0;

	if(i < ROWA && j < COLB){
		for(k = 0; k < COLA; k++){
			sum+= A[i*COLA+k]*B[k*COLB+j];
		}
		C[i*COLB+j]=sum;
	}
}


__kernel void reduction_1item(__global int *g_num, __global int *g_sum){
	int i = get_global_id(0);
	int j;

	g_num[i] = 0;
	barrier(CLK_LOCAL_MEM_FENCE);
	atomic_add(g_sum, g_num[i]);
}


__kernel void reduction_barrier(__global int *g_num, __global int *g_sum, __local int *l_sum, int TotalNum){
	int i = get_global_id(0);
	int l_i = get_local_id(0);

	l_sum[l_i] = (i < TotalNum) ? g_num[i] : 0;
	barrier(CLK_LOCAL_MEM_FENCE);

	for(int p = get_local_size(0) / 2; p >= 1; p = p >> 1){
		if(l_i < p) l_sum[l_i] += l_sum[l_i + p];
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	if(l_i == 0){
		g_sum[get_group_id(0)] = l_sum[0];
	}
}

__kernel void reduction_atomic(__global int *g_num, __global int *g_sum, int TotalNum){
	int i = get_global_id(0);
	if(i < TotalNum){
		atomic_add(g_sum, g_num[i]);
	}
}
