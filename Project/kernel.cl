
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

