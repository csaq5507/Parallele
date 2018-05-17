__kernel void hillissteele(
	__global float* a,
	__global float* b,
	int n,
	int k
	){
	size_t i= get_global_id(0);
	
	if(i>=n)return;
			
		barrier(CLK_GLOBAL_MEM_FENCE);
			for(int j=0;j<n;j++){
			if(j<k+1) b[j]=a[j];
			else{
				b[j]=a[j]+a[j-(int)pow((float)2,(float)k)];

				}
			}
		}
